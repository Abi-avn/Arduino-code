#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <sstream>
#include <vector>
#include < time.h>
#include <thread>
using namespace std;



class Transaction {
private:

	time_t timestamp;
	double value;
public:
		string desc; //short description on the type of transaction
	Transaction(const string& desc, double value)
		: desc(desc), value(value) {
		timestamp = time(0); // Set current time as the timestamp
	}

	string toString() const {
		
		return "--" + desc + " : GBP " + to_string(value) + " on " + ctime(&timestamp);
	}
};

class Account {
protected:
	double balance;
	vector<Transaction> history;
	string accountType;
public:
	Account() : balance(0.0), accountType("Generic Account") {}
	const vector<Transaction>& getHistory() const {
		return history;
	}

	virtual void deposit(double amount) {
		if (amount > 0) {
			balance += amount;
			history.emplace_back("Deposit", amount);
		}
	}

	virtual bool withdraw(double amount) {
		if (amount > 0 && amount <= balance) {
			balance -= amount;
			history.emplace_back("Withdraw", amount);
			return true;
		}
		return false;
	}
	virtual string toString() const {
		return accountType + " Balance: " + to_string(balance);
	}
	virtual string accountPrint() const {
		return accountType;
	}
	virtual ~Account() {}
};

// InterestEarning Interface (abstract class)
class InterestEarning {
public:
	virtual void computeInterest() = 0;
	virtual ~InterestEarning() {}
};

// Current Account Class
class Current : public Account {
private:
	double overdraft;

public:
	Current(double overdraftLimit) : overdraft(overdraftLimit) {
		accountType = "Current Account";
	};

	bool withdraw(double amount) override {
		if (amount > 0 && (balance - amount >= -overdraft)) {
			balance -= amount;
			history.emplace_back("Withdraw", amount);
			return true;
		}
		return false;
	}

	string toString() const override {
		return accountType + " Balance: " + to_string(balance) +
			", Overdraft Limit: " + to_string(overdraft);
	}
};
// Savings Account Class
class Savings : public Account, public InterestEarning {
private:
	double interestRate;
	bool isa; // Individual Savings Account flag

public:
	double t = 0; 
	Savings(  bool isaFlag) :  isa(isaFlag) {
		if (isa == true) {
			accountType = "Savings Account ISA";
			interestRate = 0.0115;
		}
		else {
			accountType = "Savings Account";
			interestRate = 0.0085;
		}
	}

	void computeInterest()  {
		/*double interest = balance * (interestRate / 100);*/
		double interest = balance * pow(1 + (interestRate / 12), (12 * t));
		balance += interest;
		history.emplace_back("Interest Earned", interest);
		cout <<"projectedBalance: " << interest << endl;
	}

	string toString() const override {

		return accountType + " Balance: " + to_string(balance) +
			", Interest Rate: " + to_string(interestRate) +
			"%, ISA: " + (isa ? "Yes" : "No");
	}
};

vector<Account> accountVector;
vector<Savings> savingsVector;

int main()
{    

	auto stringInterface = [](string decision, string userCommand) {
		tuple<int, int, int> extractNumbers(const std::string & userCommand);
		istringstream iss(userCommand);
		string command;
		double num1, num2, num3;

		iss >> command >> num1 >> num2 >> num3;

		if (command == decision && num1 > 0) {
			auto p = make_tuple(num1, num2,num3);
			return p;
		}
		else {
			cout << "Invalid command.\n";
		}
				};




	vector <string> parameters;
	string userCommand;
	// you may also want to store a collection of opened accounts here

	cout << "~~~ Welcome to LincBank! ~~~" << endl;

	while (userCommand != "exit")
	{
		parameters.clear(); // clear ready for next command
		cout << endl << ">>> ";

		getline(cin, userCommand);
		char* cstr = new char[userCommand.length() + 1];
		strcpy(cstr, userCommand.c_str());

		char* token;
		token = strtok(cstr, " ");

		while (token != NULL)
		{
			parameters.push_back(token);
			token = strtok(NULL, " ");
		}

		// Define all commands as per the brief
		string command = parameters[0];

		if (command.compare("options") == 0)
		{
			// display the various commands to the user
			cout << "open - open type initial_deposit: open a current (1), savings (2) or ISA (3) account  " << endl;
			cout << "view - view [index]: view balance and recent transactions " << endl;
			cout << "withdraw - withdraw sum: withdraw funds from most recently viewed account  " << endl;
			cout << "deposit - deposit sum: deposit funds into most recently viewed account" << endl;
			cout << "transfer - transfer src dest sum: transfer funds between accounts " << endl;
			cout << "project - project years: project balance forward in time " << endl;
			cout << "exit - exit: close this application " << endl;
			cout << "options  - options: view these options again" << endl;
		}
		else if (command.compare("open") == 0)
		{
		
			auto commandInterface = stringInterface("open",  userCommand);

			auto open = [](auto commandInterface) {
				if (get<0>(commandInterface) == 1) {
					Current currentAccount(500); // Overdraft limit of 500
					currentAccount.deposit(get<1>(commandInterface));
					cout << currentAccount.toString() << endl;
					
					for (const auto& transaction : currentAccount.getHistory()) {
					cout << transaction.toString();	
					};
				
					accountVector.push_back(currentAccount);
					
					
				}
				else if (get<0>(commandInterface) == 2) {
					Savings savingsAccount( false); 
					savingsAccount.deposit(get<1>(commandInterface));
				
					cout << savingsAccount.toString() << endl;
					for (const auto& transaction : savingsAccount.getHistory()) {
						cout << transaction.toString();


					};
					accountVector.push_back(savingsAccount);
				}
				else {
					
					if (get<1>(commandInterface) >= 1000) {
						Savings savingsAccountISA(true); // 5% interest, ISA enabled
						savingsAccountISA.deposit(get<1>(commandInterface));
						cout << savingsAccountISA.toString() << endl;

						for (const auto& transaction : savingsAccountISA.getHistory()) {
							cout << transaction.toString();
						};

						accountVector.push_back(savingsAccountISA);
						savingsVector.push_back(savingsAccountISA);
					}
					else {
						cout << "ISA initial balance has to be 1000 or more... " << endl;
					}

				};
		
				};

			open(commandInterface);
			
			/////////////


			// allow a user to open an account  
			// e.g., Account* a = new Savings(...);
		}
		else if (command.compare("view") == 0)
		{
			

			istringstream iss(userCommand);
			string command;
			double num1    ;

			iss >> command >> num1;

			if (command == "view" && num1 > 0) {
				cout << accountVector[num1 - 1].toString() << endl;
			}
			else {
				cout << "All" << endl;
				for (int i = 0; i < accountVector.size(); i++) {
					auto user = accountVector[i];
					cout << user.toString() << endl;
					for (const auto& transaction : user.getHistory()) {
						cout << transaction.toString();
					};
				};
			}

		

			// display an account according to an index (starting from 1)
			// alternatively, display all accounts if no index is provided
		}
		else if (command.compare("withdraw") == 0)

		{
		
			auto commandInterface = stringInterface("withdraw", userCommand);
			cout << "Withdrawing    :  " << get<0>(commandInterface) << endl;
			auto recent = accountVector.back();
			recent.withdraw(get<0>(commandInterface));
			cout << recent.toString() << endl;
			for (const auto& transaction : recent.getHistory()) {
				cout << transaction.toString();
			};
			accountVector.back() = recent;
			// allow user to withdraw funds from an account
		}
		else if (command.compare("deposit") == 0)
		{
			auto commandInterface = stringInterface("deposit", userCommand);
			cout << "Depositing:  " << get<0>(commandInterface) << endl;


			auto recent = accountVector.back();
			recent.deposit(get<0>(commandInterface));
			cout << recent.toString() << endl;
			for (const auto& transaction : recent.getHistory()) {
				cout << transaction.toString();
			};

			accountVector.back() = recent;
			// allow user to deposit funds into an account
		}
		else if (command.compare("transfer") == 0)
		{

			auto commandInterface = stringInterface("transfer", userCommand);
			cout<< get<0>(commandInterface) << " " << get<1>(commandInterface)<<" " << get<2>(commandInterface) << endl;

			vector<string> selection = {"Current Account", "Savings Account", "Savings Account ISA"};
			
	
			cout << "Withdrawing "<< get<2>(commandInterface) <<" from " << get<0>(commandInterface) << endl;
	
			for (int i = 0; i < accountVector.size(); i++) {
				if (accountVector[i].accountPrint() == selection[get<0>(commandInterface)-1]) {
					//cout << i << endl;
					auto user = accountVector[i];
					user.withdraw(get<2>(commandInterface));
					cout<<user.toString() << endl;
					accountVector[i] = user;
					for (const auto& transaction : user.getHistory()) {
				cout << transaction.toString();
			};
			}
				else {
					cout << "Error, this account doesn't exist" << endl;
				}
			};
			
			cout << "Depositing " << get<2>(commandInterface) << " to " << get<1>(commandInterface) << endl;
			for (int i = 0; i < accountVector.size(); i++) {
				if (accountVector[i].accountPrint() == selection[get<1>(commandInterface)-1]) {
					//cout << i << endl;
					auto user = accountVector[i];
					user.deposit(get<2>(commandInterface));
					cout << user.toString() << endl;
					accountVector[i] = user;
					for (const auto& transaction : user.getHistory()) {
						/*history.emplace_back("Deposit", amount);*/
						cout << transaction.toString();
					};
				}
				else {
					cout << "Error, this account doesn't exist" << endl;
				}
			};

			// allow user to transfer funds between accounts
			// i.e., a withdrawal followed by a deposit!
		}
		else if (command.compare("project") == 0)
		{
			auto commandInterface = stringInterface("project", userCommand);
				if (savingsVector.size() != 0 && savingsVector[0].accountPrint() == "Savings Account ISA") {
					cout << 0 << endl;
					savingsVector[0].t = get<0>(commandInterface);
					savingsVector[0].computeInterest();
				}
				else {
					cout << "Error, this account doesn't exist" << endl;
				}
			
			// compute compound interest t years into the future
		}
		else if (command.compare("search"))
		{
			istringstream iss(userCommand);
			string command , trasactionCommand;
	
			double num1;

			iss >> command >> num1 >> trasactionCommand;

			vector<string> selection = { "Current Account", "Savings Account", "Savings Account ISA" };
			cout << "hello" << endl; 
			cout<< num1 << " "<< trasactionCommand << endl;

			//for (int i = 0; i < accountVector.size(); i++) {
			//	if (accountVector[i].accountPrint() == selection[get<0>(commandInterface) - 1]) {
			//		//cout << i << endl;
			//		auto user = accountVector[i];
			//		user.withdraw(get<2>(commandInterface));
			//		cout << user.toString() << endl;
			//		accountVector[i] = user;
			//	}
			//	else {
			//		cout << "Error, this account doesn't exist" << endl;
			//	}
			//};

		/*	allow users to search their account history for a transaction
		  (this is a stretch task)*/
		}

	};
	

	cout << "Press any key to quit...";
	getchar();
}

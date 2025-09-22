#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

class GlobalData {
public:
    ofstream output;
    ifstream input;

    // User variables
    string name, uname, password, role;

    // Vehicle variables
    string model, number, brand, rent;
};

class FileHandler : public GlobalData {
public:
    void saveUser(const string& name, const string& uname, const string& password, const string& role) {
        output.open("Users.txt", ios::app);
        if (!output) {
            cerr << "Cannot Open File" << endl;
        } else {
            output << name << " " << uname << " " << password << " " << role << endl;
        }
        output.close();
    }

    bool loadUser(const string& uname, const string& password) {
        input.open("Users.txt");
        if (!input) {
            cerr << "Cannot Open File" << endl;
            return false;
        }

        string line;
        while (getline(input, line)) {
            istringstream iss(line);
            string storedName, storedUname, storedPassword, storedRole;
            iss >> storedName >> storedUname >> storedPassword >> storedRole;
            if (storedUname == uname && storedPassword == password) {
                name = storedName;
                this->uname = storedUname;
                this->password = storedPassword;
                role = storedRole;
                input.close();
                return true;
            }
        }
        input.close();
        return false;
    }

    void saveVehicle(const string& filename, const string& brand, const string& model, const string& number, const string& rent) {
        output.open(filename, ios::app);
        if (!output) {
            cerr << "Cannot Open File" << endl;
        } else {
            output << brand << " " << model << " " << number << " " << rent << " Available" << endl;
        }
        output.close();
    }

    vector<string> loadVehicles(const string& filename) {
        vector<string> vehicles;
        input.open(filename);
        if (!input) {
            cerr << "Cannot Open File" << endl;
            return vehicles;
        }

        string line;
        while (getline(input, line)) {
            vehicles.push_back(line);
        }
        input.close();
        return vehicles;
    }

    void updateVehicles(const string& filename, const vector<string>& vehicles) {
        output.open(filename);
        if (!output) {
            cerr << "Cannot Open File" << endl;
            return;
        }

        for (const auto& vehicle : vehicles) {
            output << vehicle << endl;
        }
        output.close();
    }
};

class User : public FileHandler {
public:
    void signup() {
        int choice;
        cout << "Enter Full Name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter User Name: ";
        getline(cin, uname);
        cout << "Enter Password: ";
        getline(cin, password);
        cout << "Select Role \n 1. Renter \t 2. User \n: ";
        cin >> choice;

        role = (choice == 1) ? "Renter" : "User";
        saveUser(name, uname, password, role);
        cout << "Signup Successful" << endl;
    }

    bool login() {
        cout << "Enter User Name: ";
        cin >> uname;
        cout << "Enter Password: ";
        cin >> password;
        if (loadUser(uname, password)) {
            cout << "Login Successful" << endl;
            return true;
        } else {
            cout << "Incorrect Credentials" << endl;
            return false;
        }
    }
};

class Vehicle : public FileHandler {
protected:
    string vehicleType;

public:
    void addVehicle() {
        cout << "Enter " << vehicleType << " Brand: ";
        cin >> brand;
        cout << "Enter " << vehicleType << " Model: ";
        cin >> model;
        cout << "Enter " << vehicleType << " Number: ";
        cin >> number;
        cout << "Enter " << vehicleType << " Rent per hour: ";
        cin >> rent;
        saveVehicle(vehicleType + ".txt", brand, model, number, rent);
        cout << vehicleType << " added successfully!" << endl;
    }

    void showAllVehicles() {
        vector<string> vehicles = loadVehicles(vehicleType + ".txt");
        if (vehicles.empty()) {
            cout << "No " << vehicleType << "s found." << endl;
            return;
        }
        for (const auto& vehicle : vehicles) {
            cout << vehicle << endl;
        }
    }

    void rentVehicle(const string& uname) {
        vector<string> vehicles = loadVehicles(vehicleType + ".txt");
        if (vehicles.empty()) {
            cout << "No " << vehicleType << "s available." << endl;
            return;
        }

        cout << "Available " << vehicleType << "s:" << endl;
        int availableCount = 0;
        for (int i = 0; i < vehicles.size(); i++) {
            if (vehicles[i].find("Available") != string::npos) {
                cout << i + 1 << ". " << vehicles[i] << endl;
                availableCount++;
            }
        }

        if (availableCount == 0) {
            cout << "No " << vehicleType << "s available for rent." << endl;
            return;
        }

        int choice;
        cout << "Select the " << vehicleType << " to rent: ";
        cin >> choice;
        if (choice < 1 || choice > vehicles.size() || vehicles[choice - 1].find("Available") == string::npos) {
            cout << "Invalid choice!" << endl;
            return;
        }

        vehicles[choice - 1].replace(vehicles[choice - 1].find("Available"), 9, uname);
        updateVehicles(vehicleType + ".txt", vehicles);
        cout << "You have rented the " << vehicleType << "." << endl;
    }

    void returnVehicle(const string& uname) {
        vector<string> vehicles = loadVehicles(vehicleType + ".txt");
        bool found = false;
        for (auto& vehicle : vehicles) {
            if (vehicle.find(uname) != string::npos) {
                size_t rentPos = vehicle.find_last_of(" ");
                string rentStr = vehicle.substr(rentPos + 1);
                double hourlyRent = stod(rentStr);

                double hoursRented;
                cout << "Enter hours rented: ";
                cin >> hoursRented;

                double totalCost = hourlyRent * hoursRented;
                cout << "Total cost for renting: $" << totalCost << endl;

                vehicle.replace(vehicle.find(uname), uname.length(), "Available");
                found = true;
                break;
            }
        }

        if (found) {
            updateVehicles(vehicleType + ".txt", vehicles);
            cout << "Vehicle returned successfully." << endl;
        } else {
            cout << "No vehicle found rented by " << uname << "." << endl;
        }
    }

    void vehicleMenu(const string& uname) {
        int choice;
        do {
            cout << "\n" << vehicleType << " Menu" << endl;
            cout << "1. Add " << vehicleType << endl;
            cout << "2. Show All " << vehicleType << "s" << endl;
            cout << "3. Rent " << vehicleType << endl;
            cout << "4. Return " << vehicleType << endl;
            cout << "5. Exit to Main Menu" << endl;
            cout << "Enter choice: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    addVehicle();
                    break;
                case 2:
                    showAllVehicles();
                    break;
                case 3:
                    rentVehicle(uname);
                    break;
                case 4:
                    returnVehicle(uname);
                    break;
                case 5:
                    break;
                default:
                    cout << "Invalid choice! Please try again." << endl;
            }
        } while (choice != 5);
    }
};

class Car : public Vehicle {
public:
    Car() { vehicleType = "Car"; }
};

class Bike : public Vehicle {
public:
    Bike() { vehicleType = "Bike"; }
};

class Truck : public Vehicle {
public:
    Truck() { vehicleType = "Truck"; }
};

class Segway : public Vehicle {
public:
    Segway() { vehicleType = "Segway"; }
};

void mainMenu() {
    int choice;
    User user;

    do {
        cout << "\nMain Menu" << endl;
        cout << "1. Sign Up" << endl;
        cout << "2. Login" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                user.signup();
                break;
            case 2:
                if (user.login()) {
                    int vehicleChoice;
                    do {
                        cout << "\nSelect Vehicle Type" << endl;
                        cout << "1. Car" << endl;
                        cout << "2. Bike" << endl;
                        cout << "3. Truck" << endl;
                        cout << "4. Segway" << endl;
                        cout << "5. Logout" << endl;
                        cout << "Enter choice: ";
                        cin >> vehicleChoice;

                        switch (vehicleChoice) {
                            case 1: {
                                Car car;
                                car.vehicleMenu(user.uname);
                                break;
                            }
                            case 2: {
                                Bike bike;
                                bike.vehicleMenu(user.uname);
                                break;
                            }
                            case 3: {
                                Truck truck;
                                truck.vehicleMenu(user.uname);
                                break;
                            }
                            case 4: {
                                Segway segway;
                                segway.vehicleMenu(user.uname);
                                break;
                            }
                            case 5:
                                cout << "Logging out..." << endl;
                                break;
                            default:
                                cout << "Invalid choice! Please try again." << endl;
                        }
                    } while (vehicleChoice != 5);
                }
                break;
            case 3:
                cout << "Exiting program..." << endl;
                break;
            default:
                cout << "Invalid choice! Please try again." << endl;
        }
    } while (choice != 3);
}

int main() {
    mainMenu();
}
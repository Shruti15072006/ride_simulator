

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <map>
#include <iomanip>
#include <algorithm>
#include <unistd.h>

using namespace std;

class Location {
private:
    double x, y;

public:
    Location(double x = 0, double y = 0) : x(x), y(y) {}
    
    double getX() const { return x; }
    double getY() const { return y; }
    
    double distanceTo(const Location& other) const {
        return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));
    }
    
    string toString() const {
        return "(" + to_string((int)x) + "," + to_string((int)y) + ")";
    }
};

class Driver {
public:
    enum Status { OFFLINE, AVAILABLE, ON_TRIP };
    
private:
    int id;
    string name;
    Location location;
    Status status;
    string vehicleType;
    string licensePlate;
    double earnings;
    double rating;
    int totalTrips;

public:
    Driver(int id, string name, Location loc, string vehicle, string plate) 
        : id(id), name(name), location(loc), status(OFFLINE), vehicleType(vehicle), 
          licensePlate(plate), earnings(0.0), rating(5.0), totalTrips(0) {}
    
    int getId() const { return id; }
    string getName() const { return name; }
    Location getLocation() const { return location; }
    Status getStatus() const { return status; }
    string getVehicleType() const { return vehicleType; }
    string getLicensePlate() const { return licensePlate; }
    double getEarnings() const { return earnings; }
    double getRating() const { return rating; }
    int getTotalTrips() const { return totalTrips; }
    
    void setLocation(Location loc) { location = loc; }
    
    string getStatusString() const {
        switch(status) {
            case OFFLINE: return "OFFLINE";
            case AVAILABLE: return "AVAILABLE";
            case ON_TRIP: return "ON_TRIP";
            default: return "UNKNOWN";
        }
    }
    
    void goOnline() { 
        status = AVAILABLE; 
        cout << " Driver " << name << " is now ONLINE at location " << location.toString() << endl;
    }
    
    void goOffline() { 
        status = OFFLINE; 
        cout << " Driver " << name << " is now OFFLINE" << endl;
    }
    
    void startTrip() { 
        status = ON_TRIP; 
        cout << "Driver " << name << " started a trip" << endl;
    }
    
    void endTrip(double payment, double newRating) {
        status = AVAILABLE;
        earnings += payment;
        rating = (rating * totalTrips + newRating) / (totalTrips + 1);
        totalTrips++;
        cout << " Driver " << name << " earned ₹" << payment << " | New rating: " << fixed << setprecision(1) << rating << endl;
    }
    
    void moveTowards(Location target) {
        double dx = target.getX() - location.getX();
        double dy = target.getY() - location.getY();
        double distance = sqrt(dx * dx + dy * dy);
        
        if (distance > 0.1) {
            location = Location(
                location.getX() + (dx / distance) * 0.5,
                location.getY() + (dy / distance) * 0.5
            );
        } else {
            location = target;
        }
    }
};

class Rider {
private:
    int id;
    string name;
    Location location;
    bool hasActiveRide;
    double balance;

public:
    Rider(int id, string name, Location loc) 
        : id(id), name(name), location(loc), hasActiveRide(false), balance(1000.0) {}
    
    int getId() const { return id; }
    string getName() const { return name; }
    Location getLocation() const { return location; }
    bool hasRide() const { return hasActiveRide; }
    double getBalance() const { return balance; }
    
    void setRideStatus(bool status) { hasActiveRide = status; }
    void setLocation(Location loc) { location = loc; }
    
    void pay(double amount) { 
        balance -= amount; 
        cout  << name << " paid ₹" << amount << " | Remaining balance: ₹" << balance << endl;
    }
    
    void addBalance(double amount) { 
        balance += amount; 
        cout  << name << " added ₹" << amount << " to wallet" << endl;
    }
};

class Ride {
public:
    enum RideStatus { REQUESTED, DRIVER_ASSIGNED, PICKUP_REACHED, IN_PROGRESS, COMPLETED, CANCELLED };

private:
    int id;
    Rider* rider;
    Driver* driver;
    Location pickup;
    Location destination;
    RideStatus status;
    double fare;
    double distance;

public:
    Ride(int id, Rider* rider, Location pickup, Location destination)
        : id(id), rider(rider), driver(nullptr), pickup(pickup), 
          destination(destination), status(REQUESTED), fare(0.0) {
        calculateFare();
    }
    
    int getId() const { return id; }
    Rider* getRider() const { return rider; }
    Driver* getDriver() const { return driver; }
    Location getPickup() const { return pickup; }
    Location getDestination() const { return destination; }
    RideStatus getStatus() const { return status; }
    double getFare() const { return fare; }
    double getDistance() const { return distance; }
    
    string getStatusString() const {
        switch(status) {
            case REQUESTED: return "REQUESTED";
            case DRIVER_ASSIGNED: return "DRIVER_ASSIGNED";
            case PICKUP_REACHED: return "PICKUP_REACHED";
            case IN_PROGRESS: return "IN_PROGRESS";
            case COMPLETED: return "COMPLETED";
            case CANCELLED: return "CANCELLED";
            default: return "UNKNOWN";
        }
    }
    
    void assignDriver(Driver* drv) {
        driver = drv;
        status = DRIVER_ASSIGNED;
        driver->startTrip();
        rider->setRideStatus(true);
        
        cout << "Ride #" << id << " assigned to " << driver->getName() 
             << " (" << driver->getVehicleType() << ")" << endl;
        cout << " Pickup: " << pickup.toString() << " → Destination: " << destination.toString() << endl;
        cout << " Fare: ₹" << fixed << setprecision(2) << fare << " | Distance: " << distance << " km" << endl;
    }
    
    void update() {
        if (!driver || status == COMPLETED || status == CANCELLED) return;
        
        switch(status) {
            case DRIVER_ASSIGNED:
                if (driver->getLocation().distanceTo(pickup) < 1.0) {
                    status = PICKUP_REACHED;
                    cout << " Driver " << driver->getName() << " reached pickup point for " << rider->getName() << endl;
                } else {
                    driver->moveTowards(pickup);
                }
                break;
                
            case PICKUP_REACHED:
                status = IN_PROGRESS;
                cout << " Ride #" << id << " started! " << rider->getName() << " is on the way" << endl;
                break;
                
            case IN_PROGRESS:
                if (driver->getLocation().distanceTo(destination) < 1.0) {
                    completeRide();
                } else {
                    driver->moveTowards(destination);
                }
                break;
        }
    }
    
    void completeRide() {
        status = COMPLETED;
        rider->pay(fare);
        driver->endTrip(fare, 5.0); 
        rider->setRideStatus(false);
        rider->setLocation(destination);
        
        cout << "Ride #" << id << " completed! " << rider->getName() 
             << " reached destination" << endl;
    }
    
    void cancelRide() {
        status = CANCELLED;
        if (driver) {
            driver->goOnline();
        }
        rider->setRideStatus(false);
        cout << " Ride #" << id << " cancelled" << endl;
    }

private:
    void calculateFare() {
        distance = pickup.distanceTo(destination);
        fare = distance * 8.0 + 20.0; 
    }
};

class RideSharingSimulator {
private:
    vector<Rider*> riders;
    vector<Driver*> drivers;
    vector<Ride*> activeRides;
    vector<Ride*> completedRides;
    
    int nextRiderId;
    int nextDriverId;
    int nextRideId;
    
   
    map<string, map<string, double>> scenarios;

public:
    RideSharingSimulator() : nextRiderId(1), nextDriverId(1), nextRideId(1) {
        srand(time(0));
        initializeScenarios();
        initializeSampleData();
        setInitialDriversOnline(); 
    }
    
    ~RideSharingSimulator() {
        for (auto rider : riders) delete rider;
        for (auto driver : drivers) delete driver;
        for (auto ride : activeRides) delete ride;
        for (auto ride : completedRides) delete ride;
    }

private:
    void initializeScenarios() {
        scenarios["rush-hour"] = {{"driver_online_rate", 0.8}, {"ride_request_rate", 0.7}, {"surge", 1.5}};
        scenarios["moderate"] = {{"driver_online_rate", 0.6}, {"ride_request_rate", 0.4}, {"surge", 1.0}};
        scenarios["late-night"] = {{"driver_online_rate", 0.3}, {"ride_request_rate", 0.2}, {"surge", 2.0}};
        scenarios["weekend"] = {{"driver_online_rate", 0.7}, {"ride_request_rate", 0.6}, {"surge", 1.3}};
    }
    
    void initializeSampleData() {
        cout << " INITIALIZING RIDE-SHARING SIMULATOR...\n\n";
        
        
        riders.push_back(new Rider(nextRiderId++, "Aarav Sharma", Location(5, 5)));
        riders.push_back(new Rider(nextRiderId++, "Priya Patel", Location(15, 8)));
        riders.push_back(new Rider(nextRiderId++, "Rohan Singh", Location(8, 15)));
        riders.push_back(new Rider(nextRiderId++, "Neha Gupta", Location(12, 3)));
        riders.push_back(new Rider(nextRiderId++, "Vikram Joshi", Location(3, 12)));
        
        
        for (int i = 0; i < 15; i++) {
            Location randomLoc(rand() % 18 + 1, rand() % 18 + 1);
            string vehicles[] = {"Sedan", "Hatchback", "SUV", "Premium"};
            string plates[] = {"DL01AB", "MH02CD", "KA03EF", "TN04GH", "UP05IJ"};
            
            drivers.push_back(new Driver(
                nextDriverId++, 
                "Driver " + to_string(i+1),
                randomLoc,
                vehicles[rand() % 4],
                plates[rand() % 5] + to_string(rand() % 1000)
            ));
        }
        
        cout << "Created " << riders.size() << " riders and " << drivers.size() << " drivers\n";
    }
    
    void setInitialDriversOnline() {
        cout << "\n SETTING INITIAL DRIVERS ONLINE...\n";
        int onlineCount = 0;
       
        for (Driver* driver : drivers) {
            if (rand() % 100 < 60) { 
                driver->goOnline();
                onlineCount++;
            }
        }
        cout << onlineCount << " drivers are now online and available\n";
    }
    
    Location randomLocation() {
        return Location(rand() % 18 + 1, rand() % 18 + 1);
    }
    
    void setScenario(string scenarioName) {
        if (scenarios.find(scenarioName) == scenarios.end()) {
            cout << " Unknown scenario: " << scenarioName << endl;
            return;
        }
        
        auto scenario = scenarios[scenarioName];
        
        
        for (Driver* driver : drivers) {
            driver->goOffline();
        }
        
       
        int driversToGoOnline = drivers.size() * scenario["driver_online_rate"];
        cout << "\nSETTING SCENARIO: " << scenarioName << endl;
        cout << " Expected online drivers: " << driversToGoOnline << "/" << drivers.size() << endl;
        cout << " Surge multiplier: " << scenario["surge"] << "x\n";
        
       
        int onlineCount = 0;
        for (int i = 0; i < driversToGoOnline && onlineCount < driversToGoOnline; i++) {
            int driverIndex = rand() % drivers.size();
            if (drivers[driverIndex]->getStatus() == Driver::OFFLINE) {
                drivers[driverIndex]->goOnline();
                onlineCount++;
            }
        }
        
        cout  << onlineCount << " drivers are now online\n";
    }

public:
    void showAvailableDrivers() {
        cout << "\n AVAILABLE DRIVERS:\n";
        cout << "==========================================\n";
        
        int availableCount = 0;
        for (Driver* driver : drivers) {
            if (driver->getStatus() == Driver::AVAILABLE) {
                cout  << driver->getName() 
                     << " | " << driver->getVehicleType()
                     << " | " << driver->getLicensePlate()
                     << " | Location: " << driver->getLocation().toString()
                     << " | Rating: " << fixed << setprecision(1) << driver->getRating() 
                     << " | Trips: " << driver->getTotalTrips() << endl;
                availableCount++;
            }
        }
        
        if (availableCount == 0) {
            cout << " No drivers available at the moment\n";
            cout << " Try setting a scenario or use option 7 to set drivers online\n";
        } else {
            cout << "Total available: " << availableCount << " drivers\n";
        }
    }
    
    void showAllDrivers() {
        cout << "\n ALL DRIVERS STATUS:\n";
        cout << "==========================================\n";
        
        map<string, int> statusCount = {{"OFFLINE", 0}, {"AVAILABLE", 0}, {"ON_TRIP", 0}};
        
        for (Driver* driver : drivers) {
            string status = driver->getStatusString();
            statusCount[status]++;
            
            cout << " " << driver->getName() 
                 << " | " << status
                 << " | " << driver->getVehicleType()
                 << " | Location: " << driver->getLocation().toString();
            
            if (status == "ON_TRIP") {
                cout << " |  On Trip";
            } else if (status == "AVAILABLE") {
                cout << " |  Available";
            }
            cout << endl;
        }
        
        cout << "\n DRIVER SUMMARY:\n";
        cout << " Available: " << statusCount["AVAILABLE"] << endl;
        cout << " On Trip: " << statusCount["ON_TRIP"] << endl;
        cout << " Offline: " << statusCount["OFFLINE"] << endl;
        cout << " Online Rate: " << (statusCount["AVAILABLE"] + statusCount["ON_TRIP"]) * 100 / drivers.size() << "%\n";
    }
    
    void showRiders() {
        cout << "\n RIDERS:\n";
        cout << "==========================================\n";
        for (Rider* rider : riders) {
            cout << rider->getName() 
                 << " | Location: " << rider->getLocation().toString()
                 << " | Balance: ₹" << fixed << setprecision(2) << rider->getBalance()
                 << " | " << (rider->hasRide() ? "🚕 On trip" : "✅ Available") << endl;
        }
    }
    
    void requestRide(int riderIndex) {
        if (riderIndex < 0 || riderIndex >= riders.size()) {
            cout << " Invalid rider selection!\n";
            return;
        }
        
        Rider* rider = riders[riderIndex];
        if (rider->hasRide()) {
            cout << rider->getName() << " already has an active ride!\n";
            return;
        }
        
        Location pickup = rider->getLocation();
        Location destination = randomLocation();
        
        Ride* ride = new Ride(nextRideId++, rider, pickup, destination);
        activeRides.push_back(ride);
        
        cout << "\n RIDE REQUESTED:\n";
        cout << " From: " << rider->getName() << " at " << pickup.toString() << endl;
        cout << " To: Random location " << destination.toString() << endl;
        cout << " Estimated fare: ₹" << fixed << setprecision(2) << ride->getFare() << endl;
        
       
        assignDriverToRide(ride);
    }
    
    void assignDriverToRide(Ride* ride) {
        Driver* nearestDriver = nullptr;
        double minDistance = 1000.0;
        
        for (Driver* driver : drivers) {
            if (driver->getStatus() == Driver::AVAILABLE) {
                double distance = driver->getLocation().distanceTo(ride->getPickup());
                if (distance < minDistance) {
                    minDistance = distance;
                    nearestDriver = driver;
                }
            }
        }
        
        if (nearestDriver) {
            ride->assignDriver(nearestDriver);
            cout << " Nearest driver: " << nearestDriver->getName() 
                 << " (" << minDistance << " units away)\n";
        } else {
            cout << " No available drivers found! Ride cancelled.\n";
            cout << " Try setting a scenario to get more drivers online\n";
            ride->cancelRide();
          
            for (auto it = activeRides.begin(); it != activeRides.end(); ++it) {
                if (*it == ride) {
                    delete *it;
                    activeRides.erase(it);
                    break;
                }
            }
        }
    }
    
    void updateSimulation() {
        
        for (auto it = activeRides.begin(); it != activeRides.end();) {
            Ride* ride = *it;
            ride->update();
            
            if (ride->getStatus() == Ride::COMPLETED || ride->getStatus() == Ride::CANCELLED) {
                if (ride->getStatus() == Ride::COMPLETED) {
                    completedRides.push_back(ride);
                } else {
                    delete ride;
                }
                it = activeRides.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    void showActiveRides() {
        cout << "\n ACTIVE RIDES:\n";
        cout << "==========================================\n";
        
        if (activeRides.empty()) {
            cout << "No active rides at the moment\n";
            return;
        }
        
        for (Ride* ride : activeRides) {
            cout << "Ride #" << ride->getId() 
                 << " | " << ride->getRider()->getName()
                 << " → " << (ride->getDriver() ? ride->getDriver()->getName() : "No driver")
                 << " | Status: " << ride->getStatusString()
                 << " | Fare: ₹" << fixed << setprecision(2) << ride->getFare() << endl;
        }
    }
    
    void showStatistics() {
        cout << "\n SIMULATION STATISTICS:\n";
        cout << "==========================================\n";
        cout << "Total Riders: " << riders.size() << endl;
        cout << "Total Drivers: " << drivers.size() << endl;
        cout << "Active Rides: " << activeRides.size() << endl;
        cout << "Completed Rides: " << completedRides.size() << endl;
        
        double totalEarnings = 0;
        int totalTrips = 0;
        int availableDrivers = 0;
        for (Driver* driver : drivers) {
            totalEarnings += driver->getEarnings();
            totalTrips += driver->getTotalTrips();
            if (driver->getStatus() == Driver::AVAILABLE) {
                availableDrivers++;
            }
        }
        
        cout << "Total Driver Earnings: ₹" << fixed << setprecision(2) << totalEarnings << endl;
        cout << " Total Trips Completed: " << totalTrips << endl;
        cout << " Available Drivers Now: " << availableDrivers << endl;
        
        if (!completedRides.empty()) {
            double avgFare = 0;
            for (Ride* ride : completedRides) {
                avgFare += ride->getFare();
            }
            avgFare /= completedRides.size();
            cout << " Average Fare: ₹" << fixed << setprecision(2) << avgFare << endl;
        }
    }
    
  
    void setDriversOnlineManually() {
        cout << "\n SETTING DRIVERS ONLINE MANUALLY...\n";
        int count = 0;
        for (Driver* driver : drivers) {
            if (driver->getStatus() == Driver::OFFLINE) {
                driver->goOnline();
                count++;
            }
        }
        cout  << count << " drivers are now online\n";
    }
    
    void run() {
        cout << "================================================================================\n";
        cout << "                   UBER/RAPIDO RIDE-SHARING SIMULATOR (DATA MODE)              \n";
        cout << "================================================================================\n";
        
        bool running = true;
        while (running) {
            cout << "\n\n MAIN MENU:\n";
            cout << "==========================================\n";
            cout << "1. Show Available Drivers & Locations\n";
            cout << "2. Show All Drivers Status\n";
            cout << "3. Show Riders\n";
            cout << "4. Request Random Ride\n";
            cout << "5. Show Active Rides\n";
            cout << "6. Show Statistics\n";
            cout << "7. Set Scenario (Rush Hour/Moderate/Late Night/Weekend)\n";
            cout << "8. Set All Drivers Online (NEW)\n";
            cout << "9. Update Simulation (Advance Time)\n";
            cout << "10. Add Random Ride Requests\n";
            cout << "0. Exit\n";
            cout << "==========================================\n";
            cout << "Choose option: ";
            
            int choice;
            cin >> choice;
            
            switch(choice) {
                case 1:
                    showAvailableDrivers();
                    break;
                case 2:
                    showAllDrivers();
                    break;
                case 3:
                    showRiders();
                    break;
                case 4:
                    if (!riders.empty()) {
                        requestRide(rand() % riders.size());
                    }
                    break;
                case 5:
                    showActiveRides();
                    break;
                case 6:
                    showStatistics();
                    break;
                case 7:
                    {
                        cout << "Choose scenario:\n";
                        cout << "1. Rush Hour (80% drivers online)\n";
                        cout << "2. Moderate (60% drivers online)\n"; 
                        cout << "3. Late Night (30% drivers online)\n";
                        cout << "4. Weekend (70% drivers online)\n";
                        int sc;
                        cin >> sc;
                        string scenarios[] = {"rush-hour", "moderate", "late-night", "weekend"};
                        if (sc >= 1 && sc <= 4) {
                            setScenario(scenarios[sc-1]);
                        } else {
                            cout << " Invalid scenario choice!\n";
                        }
                    }
                    break;
                case 8:
                    setDriversOnlineManually();
                    break;
                case 9:
                    updateSimulation();
                    cout << " Simulation updated!\n";
                    break;
                case 10:
                    {
                        int numRequests = rand() % 3 + 1; 
                        cout << " Generating " << numRequests << " random ride requests...\n";
                        for (int i = 0; i < numRequests; i++) {
                            if (!riders.empty()) {
                                requestRide(rand() % riders.size());
                            }
                        }
                    }
                    break;
                case 0:
                    running = false;
                    break;
                default:
                    cout << " Invalid choice!\n";
            }
            
            
            sleep(1);
        }
        
        cout << "\n================================================================================\n";
        cout << "                         SIMULATION ENDED - FINAL STATISTICS                    \n";
        cout << "================================================================================\n";
        showStatistics();
    }
};

int main() {
    RideSharingSimulator simulator;
    simulator.run();
    return 0;
}
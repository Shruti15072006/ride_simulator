# 🚖 Ride Sharing Simulator in C++

A **console-based Ride Sharing Simulation System** developed in **C++**, modeling real-world platforms like **Uber**, **Ola**, and **Rapido**.  
The project demonstrates **core Object-Oriented Programming (OOP)** concepts such as encapsulation, inheritance, polymorphism, and abstraction — simulating real-time ride allocation, payment calculation, and dynamic scenarios (rush hour, late night, weekend, etc.).

---

## 🏫 Course Information
- **Course:** Object Oriented Programming (OOP)
- **Course Code:** 2CS502CC25  
- **Faculty:** Somenath Bera  
- **Institution:** Nirma University, Ahmedabad  
- **Department:** Computer Engineering  
- **Submitted by:**  
  - Shreya Dixit (24BCE108)  
  - Shruti Modi (24BCE106)  

---

## 🎯 Project Objective
- Simulate the **real-world working** of a ride-sharing system in C++  
- Demonstrate **OOP principles**: encapsulation, inheritance, abstraction, and polymorphism  
- Implement **driver–rider matching** based on real-time proximity  
- Measure **system efficiency** via metrics like average wait time, driver utilization, and revenue  
- Handle **dynamic operational scenarios** with surge pricing and availability  

---

## 🧠 System Overview

### 🌍 System Architecture
- City map: 20×20 grid  
- 15 drivers and 5 riders initialized  
- Each entity has independent states and properties  

**Driver States:**  
`OFFLINE`, `AVAILABLE`, `ON_TRIP`

**Ride States:**  
`REQUESTED`, `DRIVER_ASSIGNED`, `PICKUP_REACHED`, `IN_PROGRESS`, `COMPLETED`, `CANCELLED`

---

## ⚙️ Core Working Principle

1. Riders generate random ride requests  
2. The system finds the **nearest available driver** using Euclidean distance  
3. Driver moves toward the pickup location  
4. Ride transitions through multiple states until **completion**  
5. Fare is calculated dynamically using distance and surge factors  
6. Real-time statistics (earnings, wait time, driver utilization) are updated continuously  

---

## 📊 Operational Scenarios

| Scenario | Drivers Online | Demand Rate | Surge Multiplier |
|-----------|----------------|--------------|------------------|
| **Rush Hour** | 80% | 70% | 1.5× |
| **Moderate** | 60% | 40% | 1.0× |
| **Late Night** | 30% | 20% | 2.0× |
| **Weekend** | 70% | 60% | 1.3× |

---

## 🧩 Object-Oriented Design

### 1. Encapsulation
Each class manages its own data and methods:
- **Driver** – ID, location, status, rating, earnings  
- **Rider** – ID, location, balance, ride requests  
- **Ride** – Driver–rider association, fare, status, distance  
- **Location** – Coordinates and distance computation  

### 2. Abstraction
- Movement, fare calculation, and assignment logic are encapsulated and hidden behind clean interfaces.

### 3. Inheritance
- `Driver` and `Rider` inherit from a base `User` class for shared properties.

### 4. Polymorphism
- Virtual methods allow flexible extension and specialized behaviors for different user types.

### 5. Composition
- `Ride` contains `Driver` and `Rider` objects.  
- `Simulator` manages all active entities collectively.

---

## 🧱 Core Class Structure

### 📍 Location Class
```cpp
class Location {
private:
    double x, y;
public:
    Location(double x = 0, double y = 0);
    double distanceTo(const Location& other) const;
    void moveTowards(const Location& target, double speed);
    string toString() const;
};

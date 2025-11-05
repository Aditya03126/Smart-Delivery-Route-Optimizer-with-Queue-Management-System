/**
 * The program is a smart delivery route optimizer with a queue management system that allows users to
 * manage appointments, couriers, dispatch deliveries, optimize routes, view routing matrix, and exit
 * the system.
 * 
 * @return The program is returning 0, which indicates successful execution and termination of the main
 * function.
 */
/**
 * The program is a smart delivery route optimizer with a queue management system that allows users to
 * add appointments, couriers, view appointments and couriers, dispatch deliveries, optimize multiple
 * deliveries, view routing matrix, remove appointments, and exit the system.
 * 
 * @return The program is returning 0, which indicates successful execution and termination of the main
 * function.
 */
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <climits>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace std;

// ==================== LINKED LIST FOR APPOINTMENT SCHEDULING ====================

struct DeliveryAppointment {
    int appointmentId;
    string customerName;
    string address;
    string deliveryTime;
    int priority;  // 1 = highest, 5 = lowest
    string zone;
    bool completed;
    DeliveryAppointment* next;
    
    DeliveryAppointment(int id, string name, string addr, string time, int prio, string z)
        : appointmentId(id), customerName(name), address(addr), deliveryTime(time), 
          priority(prio), zone(z), completed(false), next(nullptr) {}
};

class AppointmentLinkedList {
private:
    DeliveryAppointment* head;
    int appointmentCount;
    
public:
    AppointmentLinkedList() : head(nullptr), appointmentCount(0) {}
    
    // Add appointment (sorted by priority and time)
    void addAppointment(int id, string name, string addr, string time, int prio, string zone) {
        DeliveryAppointment* newApp = new DeliveryAppointment(id, name, addr, time, prio, zone);
        
        if (!head || head->priority > prio || 
            (head->priority == prio && head->deliveryTime > time)) {
            newApp->next = head;
            head = newApp;
        } else {
            DeliveryAppointment* current = head;
            while (current->next && 
                   (current->next->priority < prio || 
                    (current->next->priority == prio && current->next->deliveryTime <= time))) {
                current = current->next;
            }
            newApp->next = current->next;
            current->next = newApp;
        }
        appointmentCount++;
        cout << "Appointment #" << id << " added successfully!\n";
    }
    
    // Remove appointment by ID
    bool removeAppointment(int id) {
        if (!head) return false;
        
        if (head->appointmentId == id) {
            DeliveryAppointment* temp = head;
            head = head->next;
            delete temp;
            appointmentCount--;
            return true;
        }
        
        DeliveryAppointment* current = head;
        while (current->next && current->next->appointmentId != id) {
            current = current->next;
        }
        
        if (current->next) {
            DeliveryAppointment* temp = current->next;
            current->next = current->next->next;
            delete temp;
            appointmentCount--;
            return true;
        }
        return false;
    }
    
    // Get next pending appointment
    DeliveryAppointment* getNextPendingAppointment() {
        DeliveryAppointment* current = head;
        while (current) {
            if (!current->completed) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }
    
    // Mark appointment as completed
    bool completeAppointment(int id) {
        DeliveryAppointment* current = head;
        while (current) {
            if (current->appointmentId == id && !current->completed) {
                current->completed = true;
                return true;
            }
            current = current->next;
        }
        return false;
    }
    
    // Display all appointments
    void displayAppointments() {
        if (!head) {
            cout << "No appointments scheduled.\n";
            return;
        }
        
        cout << "\n========== DELIVERY APPOINTMENTS ==========\n";
        cout << left << setw(5) << "ID" << setw(20) << "Customer" 
             << setw(25) << "Address" << setw(15) << "Time" 
             << setw(10) << "Priority" << setw(10) << "Zone" << setw(10) << "Status\n";
        cout << string(95, '-') << "\n";
        
        DeliveryAppointment* current = head;
        while (current) {
            cout << left << setw(5) << current->appointmentId
                 << setw(20) << current->customerName
                 << setw(25) << current->address
                 << setw(15) << current->deliveryTime
                 << setw(10) << current->priority
                 << setw(10) << current->zone
                 << setw(10) << (current->completed ? "Done" : "Pending") << "\n";
            current = current->next;
        }
        cout << "Total Appointments: " << appointmentCount << "\n\n";
    }
    
    int getCount() { return appointmentCount; }
    
    ~AppointmentLinkedList() {
        while (head) {
            DeliveryAppointment* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

// ==================== CIRCULAR QUEUE FOR COURIER ASSIGNMENTS ====================

struct Courier {
    int courierId;
    string name;
    string currentZone;
    bool available;
    int currentLoad;
    int maxLoad;
    
    // Default constructor
    Courier() : courierId(-1), name(""), currentZone(""), available(true), currentLoad(0), maxLoad(5) {}
    
    Courier(int id, string n, string zone) 
        : courierId(id), name(n), currentZone(zone), available(true), currentLoad(0), maxLoad(5) {}
};

class CircularQueue {
private:
    vector<Courier> couriers;
    int front;
    int rear;
    int size;
    int capacity;
    
public:
    CircularQueue(int cap) : front(-1), rear(-1), size(0), capacity(cap), couriers(cap) {
    }
    
    bool isEmpty() {
        return size == 0;
    }
    
    bool isFull() {
        return size == capacity;
    }
    
    void enqueue(int id, string name, string zone) {
        if (isFull()) {
            cout << "Courier queue is full!\n";
            return;
        }
        
        if (front == -1) front = 0;
        rear = (rear + 1) % capacity;
        couriers[rear] = Courier(id, name, zone);
        size++;
        cout << "Courier " << name << " (ID: " << id << ") added to queue.\n";
    }
    
    Courier dequeue() {
        if (isEmpty()) {
            return Courier(-1, "", "");
        }
        
        Courier courier = couriers[front];
        if (front == rear) {
            front = rear = -1;
        } else {
            front = (front + 1) % capacity;
        }
        size--;
        return courier;
    }
    
    Courier peek() {
        if (isEmpty()) {
            return Courier(-1, "", "");
        }
        return couriers[front];
    }
    
    // Get next available courier for a zone
    Courier getAvailableCourierForZone(string targetZone) {
        if (isEmpty()) return Courier(-1, "", "");
        
        int iterations = 0;
        
        // Search for courier in same zone or available
        while (iterations < size) {
            int index = (front + iterations) % capacity;
            if (couriers[index].available && 
                (couriers[index].currentZone == targetZone || couriers[index].currentLoad < couriers[index].maxLoad)) {
                return couriers[index];
            }
            iterations++;
        }
        
        // If no preferred courier found, return first available courier
        for (int i = 0; i < size; i++) {
            int index = (front + i) % capacity;
            if (couriers[index].available) {
                return couriers[index];
            }
        }
        
        // If no available courier found, return empty courier
        return Courier(-1, "", "");
    }
    
    void assignDelivery(int courierId, string zone) {
        for (int i = 0; i < size; i++) {
            int index = (front + i) % capacity;
            if (couriers[index].courierId == courierId) {
                couriers[index].available = false;
                couriers[index].currentZone = zone;
                couriers[index].currentLoad++;
                break;
            }
        }
    }
    
    void releaseCourier(int courierId) {
        for (int i = 0; i < size; i++) {
            int index = (front + i) % capacity;
            if (couriers[index].courierId == courierId) {
                couriers[index].available = true;
                couriers[index].currentLoad--;
                if (couriers[index].currentLoad < 0) couriers[index].currentLoad = 0;
                break;
            }
        }
    }
    
    void displayCouriers() {
        if (isEmpty()) {
            cout << "No couriers in queue.\n";
            return;
        }
        
        cout << "\n========== COURIER QUEUE ==========\n";
        cout << left << setw(10) << "ID" << setw(20) << "Name" 
             << setw(15) << "Zone" << setw(12) << "Status" 
             << setw(10) << "Load" << "\n";
        cout << string(67, '-') << "\n";
        
        for (int i = 0; i < size; i++) {
            int index = (front + i) % capacity;
            cout << left << setw(10) << couriers[index].courierId
                 << setw(20) << couriers[index].name
                 << setw(15) << couriers[index].currentZone
                 << setw(12) << (couriers[index].available ? "Available" : "Busy")
                 << setw(10) << (to_string(couriers[index].currentLoad) + "/" + to_string(couriers[index].maxLoad)) << "\n";
        }
        cout << "\n";
    }
};

// ==================== SPARSE MATRIX FOR ROUTING OPTIMIZATION ====================

struct SparseNode {
    int row;
    int col;
    double distance;
    SparseNode* next;
    
    SparseNode(int r, int c, double dist) 
        : row(r), col(c), distance(dist), next(nullptr) {}
};

class SparseMatrix {
private:
    int numLocations;
    vector<string> locationNames;
    SparseNode** rows;
    
public:
    SparseMatrix(int locations) : numLocations(locations) {
        rows = new SparseNode*[numLocations];
        for (int i = 0; i < numLocations; i++) {
            rows[i] = nullptr;
        }
        
        // Initialize location names
        locationNames = {
            "Zone-A-Center", "Zone-A-East", "Zone-A-West",
            "Zone-B-Center", "Zone-B-North", "Zone-B-South",
            "Zone-C-Center", "Zone-C-East", "Zone-C-West",
            "Warehouse"
        };
    }
    
    void addEdge(int from, int to, double distance) {
        if (from < 0 || from >= numLocations || to < 0 || to >= numLocations) {
            return;
        }
        
        // Add to sparse matrix
        SparseNode* newNode = new SparseNode(from, to, distance);
        newNode->next = rows[from];
        rows[from] = newNode;
        
        // Also add reverse edge (undirected graph)
        SparseNode* reverseNode = new SparseNode(to, from, distance);
        reverseNode->next = rows[to];
        rows[to] = reverseNode;
    }
    
    double getDistance(int from, int to) {
        if (from == to) return 0.0;
        
        SparseNode* current = rows[from];
        while (current) {
            if (current->col == to) {
                return current->distance;
            }
            current = current->next;
        }
        return -1.0; // No connection
    }
    
    // Dijkstra's algorithm for shortest path
    vector<int> findShortestPath(int start, int end) {
        vector<double> dist(numLocations, INT_MAX);
        vector<int> parent(numLocations, -1);
        vector<bool> visited(numLocations, false);
        
        dist[start] = 0;
        
        for (int count = 0; count < numLocations - 1; count++) {
            int u = -1;
            double minDist = INT_MAX;
            
            for (int v = 0; v < numLocations; v++) {
                if (!visited[v] && dist[v] < minDist) {
                    minDist = dist[v];
                    u = v;
                }
            }
            
            if (u == -1 || u == end) break;
            visited[u] = true;
            
            SparseNode* current = rows[u];
            while (current) {
                int v = current->col;
                if (!visited[v] && dist[u] != INT_MAX && 
                    dist[u] + current->distance < dist[v]) {
                    dist[v] = dist[u] + current->distance;
                    parent[v] = u;
                }
                current = current->next;
            }
        }
        
        // Reconstruct path
        vector<int> path;
        if (dist[end] == INT_MAX) {
            return path; // No path found
        }
        
        int current = end;
        while (current != -1) {
            path.push_back(current);
            current = parent[current];
        }
        reverse(path.begin(), path.end());
        return path;
    }
    
    double calculateRouteCost(const vector<int>& path) {
        if (path.size() < 2) return 0.0;
        
        double totalCost = 0.0;
        for (size_t i = 0; i < path.size() - 1; i++) {
            double dist = getDistance(path[i], path[i + 1]);
            if (dist > 0) {
                totalCost += dist;
            }
        }
        return totalCost;
    }
    
    int getLocationIndex(string zone) {
        for (size_t i = 0; i < locationNames.size(); i++) {
            if (locationNames[i].find(zone) != string::npos) {
                return i;
            }
        }
        return -1;
    }
    
    string getLocationName(int index) {
        if (index >= 0 && index < numLocations) {
            return locationNames[index];
        }
        return "Unknown";
    }
    
    void displayMatrix() {
        cout << "\n========== ROUTING MATRIX (SPARSE) ==========\n";
        cout << "Location Connections:\n";
        for (int i = 0; i < numLocations; i++) {
            cout << locationNames[i] << " -> ";
            SparseNode* current = rows[i];
            bool hasConnections = false;
            while (current) {
                if (hasConnections) cout << ", ";
                cout << locationNames[current->col] << "(" << current->distance << "km)";
                hasConnections = true;
                current = current->next;
            }
            if (!hasConnections) cout << "No connections";
            cout << "\n";
        }
        cout << "\n";
    }
    
    ~SparseMatrix() {
        for (int i = 0; i < numLocations; i++) {
            SparseNode* current = rows[i];
            while (current) {
                SparseNode* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] rows;
    }
};

// ==================== DELIVERY MANAGEMENT SYSTEM ====================

class DeliveryOptimizer {
private:
    AppointmentLinkedList appointments;
    CircularQueue courierQueue;
    SparseMatrix routingMatrix;
    int appointmentIdCounter;
    int courierIdCounter;
    
public:
    DeliveryOptimizer() 
        : appointments(), courierQueue(10), routingMatrix(10), 
          appointmentIdCounter(1), courierIdCounter(1) {
        initializeRoutingMatrix();
    }
    
    void initializeRoutingMatrix() {
        // Zone A connections
        routingMatrix.addEdge(0, 1, 3.5);  // Zone-A-Center -> Zone-A-East
        routingMatrix.addEdge(0, 2, 4.0);  // Zone-A-Center -> Zone-A-West
        routingMatrix.addEdge(0, 9, 8.0);  // Zone-A-Center -> Warehouse
        
        // Zone B connections
        routingMatrix.addEdge(3, 4, 5.0);  // Zone-B-Center -> Zone-B-North
        routingMatrix.addEdge(3, 5, 4.5);  // Zone-B-Center -> Zone-B-South
        routingMatrix.addEdge(3, 9, 10.0); // Zone-B-Center -> Warehouse
        
        // Zone C connections
        routingMatrix.addEdge(6, 7, 3.0);  // Zone-C-Center -> Zone-C-East
        routingMatrix.addEdge(6, 8, 3.5);  // Zone-C-Center -> Zone-C-West
        routingMatrix.addEdge(6, 9, 12.0); // Zone-C-Center -> Warehouse
        
        // Inter-zone connections
        routingMatrix.addEdge(0, 3, 15.0); // Zone-A -> Zone-B
        routingMatrix.addEdge(3, 6, 18.0); // Zone-B -> Zone-C
        routingMatrix.addEdge(0, 6, 20.0); // Zone-A -> Zone-C
    }
    
    void addAppointment() {
        string name, address, time, zone;
        int priority;
        
        cout << "\nEnter customer name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter address: ";
        getline(cin, address);
        cout << "Enter delivery time (HH:MM): ";
        cin >> time;
        cout << "Enter priority (1-5, 1=highest): ";
        cin >> priority;
        if (priority < 1) priority = 1;
        if (priority > 5) priority = 5;
        cout << "Enter zone (A, B, or C): ";
        cin >> zone;
        
        string fullZone = "Zone-" + zone;
        appointments.addAppointment(appointmentIdCounter++, name, address, time, priority, fullZone);
    }
    
    void addCourier() {
        string name, zone;
        cout << "\nEnter courier name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter initial zone (A, B, or C): ";
        cin >> zone;
        
        string fullZone = "Zone-" + zone;
        courierQueue.enqueue(courierIdCounter++, name, fullZone);
    }
    
    void dispatchNextDelivery() {
        DeliveryAppointment* nextApp = appointments.getNextPendingAppointment();
        if (!nextApp) {
            cout << "No pending appointments to dispatch.\n";
            return;
        }
        
        Courier courier = courierQueue.getAvailableCourierForZone(nextApp->zone);
        if (courier.courierId == -1) {
            cout << "No available courier for appointment #" << nextApp->appointmentId << "\n";
            return;
        }
        
        cout << "\n========== DISPATCH ASSIGNMENT ==========\n";
        cout << "Appointment #" << nextApp->appointmentId << "\n";
        cout << "Customer: " << nextApp->customerName << "\n";
        cout << "Address: " << nextApp->address << "\n";
        cout << "Zone: " << nextApp->zone << "\n";
        cout << "Assigned to: " << courier.name << " (ID: " << courier.courierId << ")\n";
        
        // Calculate route
        int warehouseIndex = 9; // Warehouse index
        int targetZoneIndex = routingMatrix.getLocationIndex(nextApp->zone);
        
        if (targetZoneIndex != -1) {
            vector<int> route = routingMatrix.findShortestPath(warehouseIndex, targetZoneIndex);
            double cost = routingMatrix.calculateRouteCost(route);
            
            cout << "\nOptimized Route:\n";
            cout << "Warehouse";
            for (size_t i = 1; i < route.size(); i++) {
                cout << " -> " << routingMatrix.getLocationName(route[i]);
            }
            cout << "\nTotal Distance: " << fixed << setprecision(2) << cost << " km\n";
            cout << "Estimated Cost: $" << fixed << setprecision(2) << (cost * 1.5) << "\n";
        }
        
        courierQueue.assignDelivery(courier.courierId, nextApp->zone);
        appointments.completeAppointment(nextApp->appointmentId);
        cout << "\nDelivery dispatched successfully!\n\n";
    }
    
    void optimizeMultipleDeliveries() {
        vector<DeliveryAppointment*> pendingApps;
        DeliveryAppointment* current = appointments.getNextPendingAppointment();
        
        while (current) {
            pendingApps.push_back(current);
            // Get next after marking this one temporarily
            DeliveryAppointment* temp = current;
            current = nullptr;
            DeliveryAppointment* search = appointments.getNextPendingAppointment();
            if (search && search->appointmentId != temp->appointmentId) {
                current = search;
            }
        }
        
        if (pendingApps.empty()) {
            cout << "No pending deliveries to optimize.\n";
            return;
        }
        
        cout << "\n========== BATCH ROUTE OPTIMIZATION ==========\n";
        cout << "Optimizing " << pendingApps.size() << " deliveries...\n\n";
        
        int warehouseIndex = 9;
        double totalDistance = 0.0;
        
        for (size_t i = 0; i < pendingApps.size(); i++) {
            int targetIndex = routingMatrix.getLocationIndex(pendingApps[i]->zone);
            if (targetIndex != -1) {
                vector<int> route = routingMatrix.findShortestPath(warehouseIndex, targetIndex);
                double cost = routingMatrix.calculateRouteCost(route);
                totalDistance += cost;
                
                cout << "Delivery #" << pendingApps[i]->appointmentId 
                     << " to " << pendingApps[i]->zone 
                     << ": " << fixed << setprecision(2) << cost << " km\n";
            }
        }
        
        cout << "\nTotal Route Distance: " << fixed << setprecision(2) << totalDistance << " km\n";
        cout << "Estimated Total Cost: $" << fixed << setprecision(2) << (totalDistance * 1.5) << "\n\n";
    }
    
    void displayMenu() {
        cout << "\n========== SMART DELIVERY ROUTE OPTIMIZER ==========\n";
        cout << "1. Add Delivery Appointment\n";
        cout << "2. Add Courier\n";
        cout << "3. View All Appointments\n";
        cout << "4. View Courier Queue\n";
        cout << "5. Dispatch Next Delivery\n";
        cout << "6. Optimize Multiple Deliveries\n";
        cout << "7. View Routing Matrix\n";
        cout << "8. Remove Appointment\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
    }
    
    void run() {
        int choice;
        while (true) {
            displayMenu();
            cin >> choice;
            
            switch (choice) {
                case 1:
                    addAppointment();
                    break;
                case 2:
                    addCourier();
                    break;
                case 3:
                    appointments.displayAppointments();
                    break;
                case 4:
                    courierQueue.displayCouriers();
                    break;
                case 5:
                    dispatchNextDelivery();
                    break;
                case 6:
                    optimizeMultipleDeliveries();
                    break;
                case 7:
                    routingMatrix.displayMatrix();
                    break;
                case 8: {
                    int id;
                    cout << "Enter appointment ID to remove: ";
                    cin >> id;
                    if (appointments.removeAppointment(id)) {
                        cout << "Appointment removed successfully!\n";
                    } else {
                        cout << "Appointment not found!\n";
                    }
                    break;
                }
                case 9:
                    cout << "Exiting system...\n";
                    return;
                default:
                    cout << "Invalid choice! Please try again.\n";
            }
        }
    }
};

// ==================== MAIN FUNCTION ====================

int main() {
    cout << "========================================\n";
    cout << "  SMART DELIVERY ROUTE OPTIMIZER\n";
    cout << "  with Queue Management System\n";
    cout << "========================================\n";
    
    DeliveryOptimizer optimizer;
    
    // Add some sample data for demonstration
    cout << "\nInitializing with sample data...\n";
    optimizer.run();
    
    return 0;
}
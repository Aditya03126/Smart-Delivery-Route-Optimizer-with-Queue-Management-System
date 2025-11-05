# Smart-Delivery-Route-Optimizer-with-Queue-Management-System


# This projects does three main things:

1. Manages delivery appointments (using a linked list)

2. Handles couriers in a circular queue system

3. Optimizes delivery routes between zones using a sparse matrix and Dijkstra’s algorithm

Everything works together in a menu-driven system where the user can add, view, dispatch, and optimize deliveries.

# Main Components and Their Roles
# 1. Appointment Management (Linked List)

This part manages all customer delivery appointments.

It stores details like:

-Appointment ID

-Customer name

-Address

-Delivery time

-Priority (1 = highest, 5 = lowest)

-Zone (A, B, or C)

-Status (Pending / Done)

Appointments are stored in a Linked List, sorted by priority and time.

It allows you to:

-Add new appointments

-Remove appointments by ID

-View all scheduled deliveries

-Mark deliveries as completed

 Example:
If two customers request deliveries — one with higher priority or earlier time — it automatically gets listed first.

# 2. Courier Management (Circular Queue)

This part manages delivery couriers (drivers).

It uses a Circular Queue to:

-Add new couriers with details:

-Courier ID

-Name

-Zone they are currently in

-Load capacity (how many deliveries they can take)

-Keep track of which couriers are:

Available �

Busy �

Assign couriers automatically based on:

-Zone proximity

-Availability

-Current load

 Example:
If a courier in Zone-A is free, and a delivery is also in Zone-A, that courier will be chosen first.

# 3. Route Optimization (Sparse Matrix + Dijkstra’s Algorithm)

This part helps to find the best (shortest) route from the warehouse to delivery zones.

It uses:

Sparse Matrix: stores only existing connections (roads) between zones — saves memory.

Dijkstra’s Algorithm: calculates the shortest path between two zones.

Each zone is represented as a node, and distances (in km) between them are edges.

 Example:
If Warehouse → Zone-A → Zone-B → Zone-C has different distances,
the program automatically finds the least distance route and shows:

Path

Total Distance

Estimated Delivery Cost

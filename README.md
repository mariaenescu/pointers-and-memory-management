# Pointers-and-Memory-Management

The engineers at Ferrari need your help to win races this season. They have realized that one of the problems with their car is caused by faulty sensors (purchased from AliExpress) that transmit incorrect data to the engineering team. Help the team identify and eliminate the erroneous data to win races.

Implementation:

The implementation will involve creating a vector of type Sensor* that will contain two types of sensors: Tire Sensor and Power Management Unit Sensor (see the structures below). A Sensor structure will include the associated sensor data and a vector of indices representing the operations to be performed on the sensor data. There are a total of 8 operations in the form of functions that need to be called on the sensor data. The implementation of these operations can be found in the operations.c file in the skeleton.
To facilitate their use, the following function is provided (also in operations.c) that will build an array of operations: void get_operations(void** operations)

Priorities:

The values received from Power Management Unit sensors are more important than those received from Tire Sensors. Therefore, we want the Power Management Unit sensors to be placed first in the sensor array.
The program will read commands from the keyboard until the "exit" command is received, upon which the program will free the memory and exit.
The received commands will follow the following format:
-print - prints the sensor at the given position, in the required format. If the index received from the keyboard is negative or greater than the size of the vector, the message "Index not in range!" will be displayed.
-analyze - performs all operations on the sensor at the given position. If the index received from the keyboard is negative or greater than the size of the vector, the message "Index not in range!" will be displayed.
-clear - removes all sensors that contain erroneous values.
-exit - frees the memory and exits the program.

1. Print (simple).

2. Print (complex):
The sensor vector will be printed in the same format as presented above, taking into account their priorities.

3. Analyze:
All operations of the specified sensor will be performed in the order they were given.

4. Clear:
Sensors that contain erroneous values will be removed from the vector. A sensor is considered invalid if it does NOT meet at least one of the following conditions:

5. Exit:
Upon receiving this command, the memory is deallocated, and the program stops.

File Format for Input: Binary format

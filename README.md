# Navigation_System
 
I worked on this program as my final course assignment during my CMPUT 275 (Intro to Tangible computing - II) course. Some of the code, images and cordinates were given by professor and some of code is written by me.

Programming Languages: 

• C++ <br/>
• Python

Software Used:

• Linux<br/>
• VMware<br/>
• Sublime

Navigation System using Dijkstra’s Algorithm

Navigation system (like Google Maps) where user can scroll around on a map and select the start point and end point of trip, after both points have been selected the program will calculate the shortest path between both points and display it on plotter program.

The coordinates of start and end points are sent to a route-finding server.  The server is a C++ program that runs the graph search algorithm on a road network (i.e., a directed graph) upon receiving a route finding request from the client that includes coordinates of the start and end points of a trip. It then sends coordinates of the waypoints along the route, which are nodes on the shortest path, from the trip’s start point to its end point, to the client. 

This server computes the shortest path between the two selected points and returns the route information to a plotter program. The client is the plotter program written in Python. It displays the map and allows the user to use the mouse or keyboard to zoom and scroll around on a map of Edmonton to select the start and end points of a trip. The plotter displays the route as line segments overlaid on the original map by connecting the waypoints.

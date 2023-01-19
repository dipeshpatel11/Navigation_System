/*-------------------------------------
   NAME : Dipesh Patel
   CCID : dipesh1
   CMPUT 275, Winter 2022

   Assignment : Trivial Navigation System (Parts II)
---------------------------------------*/
#include <iostream>
#include <cassert>
#include <fstream>
#include <string>
#include <list>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "wdigraph.h"
#include "dijkstra.h"

#include <sstream>

#define MAX_SIZE 1024  // Define buffer length.
struct Point
{
  long long lat, lon;
};

// returns the manhattan distance between two points
long long manhattan(const Point &pt1, const Point &pt2)
{
  long long dLat = pt1.lat - pt2.lat, dLon = pt1.lon - pt2.lon;
  return abs(dLat) + abs(dLon);
}

// finds the id of the point that is closest to the given point "pt"
int findClosest(const Point &pt, const unordered_map<int, Point> &points)
{
  pair<int, Point> best = *points.begin();

  for (const auto &check : points)
  {
    if (manhattan(pt, check.second) < manhattan(pt, best.second))
    {
      best = check;
    }
  }
  return best.first;
}

// read the graph from the file that has the same format as the "Edmonton graph" file
void readGraph(const string &filename, WDigraph &g, unordered_map<int, Point> &points)
{
  ifstream fin(filename);
  string line;

  while (getline(fin, line))
  {
    // split the string around the commas, there will be 4 substrings either way
    string p[4];
    int at = 0;
    for (auto c : line)
    {
      if (c == ',')
      {
        ++at;  // start new string
      }
      else
      {
        // append character to the string we are building
        p[at] += c;
      }
    }

    if (at != 3)
    {
      // empty line
      break;
    }

    if (p[0] == "V")
    {
      // new Point
      int id = stoi(p[1]);
      assert(id == stoll(p[1])); // sanity check: asserts if some id is not 32-bit
      points[id].lat = static_cast<long long>(stod(p[2]) * 100000);
      points[id].lon = static_cast<long long>(stod(p[3]) * 100000);
      g.addVertex(id);
    }
    else
    {
      // new directed edge
      int u = stoi(p[1]), v = stoi(p[2]);
      g.addEdge(u, v, manhattan(points[u], points[v]));
    }
  }
}

int create_and_open_fifo(const char *pname, int mode)
{
  // creating a fifo special file in the current working directory
  // with read-write permissions for communication with the plotter
  // both proecsses must open the fifo before they can perform
  // read and write operations on it
  if (mkfifo(pname, 0666) == -1)
  {
    cout << "Unable to make a fifo. Ensure that this pipe does not exist already!" << endl;
    exit(-1);
  }

  // opening the fifo for read-only or write-only access
  // a file descriptor that refers to the open file description is
  // returned
  int fd = open(pname, mode);

  if (fd == -1)
  {
    cout << "Error: failed on opening named pipe." << endl;
    exit(-1);
  }

  return fd;
}

// keep in mind that in part 1, the program should only handle 1 request
// in part 2, you need to listen for a new request the moment you are done
// handling one request
int main()
{
  WDigraph graph;
  unordered_map<int, Point> points;
  const char *inpipe = "inpipe";
  const char *outpipe = "outpipe";

  // Open the two pipes
  int in = create_and_open_fifo(inpipe, O_RDONLY);
  cout << "inpipe opened..." << endl;
  int out = create_and_open_fifo(outpipe, O_WRONLY);
  cout << "outpipe opened..." << endl;

  // build the graph
  readGraph("server/edmonton-roads-2.0.1.txt", graph, points);

  // string to check for input.
  string inputString = "";
  while (true)
  {
    // read a request
    Point sPoint, ePoint;
    // building buffer of size 200.
    char *buffer = new char[MAX_SIZE];
    // double and double array to hold
    // start and end latitude and
    // longitude pairs in degrees.
    double sLat, sLon, eLat, eLon = 0;
    // reading pipe inpipe.
    read(in, buffer, MAX_SIZE);
    // concatenating the buffer to a string.
    for (int i = 0; i < MAX_SIZE; ++i)
    {
      inputString += buffer[i];
    }
    // if we find 'Q' in input string
    // then break out and deallocate
    // memory and clear string.
    if (inputString.find('Q') != string::npos)
    {
      inputString = "";
      delete[] buffer;
      break;
    }
    // reading in inputString
    // while taking care of white spaces and newlines.
    stringstream fin(inputString);
    fin >> sLat >> sLon >> eLat >> eLon;
    // double array to hold
    // start and end latitude and
    // longitude pairs in degrees.
    double co_ordinateArray[4] = {sLat, sLon, eLat, eLon};
    // array to hold converted latitude longitutde pairs from
    // double to long long.
    long long cordArray[4];
    for (int i = 0; i < 4; ++i)
    {
      cordArray[i] = static_cast<long long>(co_ordinateArray[i] * 100000);
    }
    // Building start point and end point struct.
    sPoint = {cordArray[0], cordArray[1]};
    ePoint = {cordArray[2], cordArray[3]};
    // get the points closest to the two points we read
    int start = findClosest(sPoint, points), end = findClosest(ePoint, points);

    // run dijkstra's algorithm, this is the unoptimized version that
    // does not stop when the end is reached but it is still fast enough
    unordered_map<int, PIL> tree;
    dijkstra(graph, start, tree);

    // NOTE: in Part II you will use a different communication protocol than Part I
    // So edit the code below to implement this protocol

    // read off the path by stepping back through the search tree
    if (tree.find(end) == tree.end())
    {
      // writing E to the output pipe and to indicate that there is no path
      write(out, "E\n", 16);
    }
    else
    {
      // read off the path by stepping back through the search tree
      list<int> path;
      while (end != start)
      {
        path.push_front(end);
        end = tree[end].first;
      }
      path.push_front(start);
      // Declaring the ouput string.
      string str = "";
      for (int v : path)
      {
        // Convert Latitude and Longitude to string.
        string convertedLati = to_string(points[v].lat);
        string convertedLongi = to_string(points[v].lon);

        // TO add decimal at 2nd index for latitude and 4th index for longitude.
        convertedLati.insert(2, ".");
        convertedLongi.insert(4, ".");
        // adding them to the output string.
        str += convertedLati + ' ' + convertedLongi + '\n';

        convertedLongi = "";
        convertedLongi = "";
      }

      str += "E\n";  // adding E with newline to mark stop of outpipe.

      write(out, str.c_str(), str.size());  // writing the string to outpipe.
      // clearing output and input for next iteration.
      str = "";
      inputString = "";
    }
  }
  // closing pipe in and out
  close(in);
  close(out);
  // delete in and out pipe.
  unlink(inpipe);
  unlink(outpipe);
  return 0;
}

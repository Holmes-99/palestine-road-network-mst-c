# Palestine Road Network — Minimum Spanning Tree

A graph-based program written in C that constructs a minimal cost road network
connecting Palestinian cities and towns, built as part of the COMP2421 
Data Structures course at Birzeit University.

## The Problem
Given a set of Palestinian cities and the distances between them,
find the minimum cost road network that connects all cities —
a classic Minimum Spanning Tree (MST) problem.

## Algorithms Implemented
- **Kruskal's Algorithm** — sorts all edges by weight, greedily picks
  the cheapest edges that don't form a cycle (uses Union-Find)
- **Prim's Algorithm** — grows the MST from a source city, always
  picking the cheapest edge connecting the tree to a new city

Both algorithms are compared in terms of:
- Total MST cost (in km)
- Execution time (measured using `clock()`)

## Graph Scale
- Minimum 50 cities (graph order)
- Minimum 200 roads (graph size)

## How to Run

### Compile
gcc road_network.c -o road_network

### Run
./road_network

Make sure `cities.txt` is in the same folder before running.

## Input Format
Each line in `cities.txt` represents a road between two cities:
City1#City2#Distance(km)

Example:
Jerusalem#Gaza#41
Bethlehem#Jerusalem#9
Haifa#Nazareth#6

## Menu Options
1. Load Cities — load file, build graph, print size and order
2. Apply Prim's Algorithm — enter source city, view MST, cost, and time
3. Apply Kruskal's Algorithm — view MST, total cost, and time
4. Compare Both Algorithms — side by side cost and execution time

## Sample Output

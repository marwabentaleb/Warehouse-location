# Warehouse-location
The goal is to find the optimal warehouse location that minimizes transportation and warehousing costs adapting several methods : Linear programming, Branch&amp;Bound, Branch&amp;Cut, Algorithme glouton, Tabu search, Genetic algorithm...

# Getting Started
These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.
## Prerequisites
To run the codes, you must install the CPLEX solver.
## Running the tests
Once you download the project and install the CPLEX solver, you must follow the following instructions to start the tests :
### Step 1 
Open terminal and go to the project folder
### Step 2 
If you want to test the methods using the generated data file named data.dat, you can skip this step. Otherwise, you can generate you data file by taping :
  1. make DataGeneration
  2. ./DataGeneration
  
Then, you have to answer the questions displayed by typing the desired number such as :
  * choose the number of cities 
  * choose the number of stores  
  * the maximum value that the demand can take
  * ...
  
Then, you will get a new dataset stored in the file data.dat
### Step 3 
To launch the resolution of the mathematical model, you have to tape : 
  1. make MILP
  2. ./MILP
  
Then, you will obtain the optimal solution.
### Step 4 
To obtain the solution by applying Branch and bound method, you have to tape : 
  1. make BranchAndBound
  2. ./BranchAndBound

### Step 5 
To obtain the solution by using the algorithm glouton, you have to tape : 
  1. make AlgoGlouton
  2. ./AlgoGlouton


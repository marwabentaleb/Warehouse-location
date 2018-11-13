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
  make DataGeneration
  ./DataGeneration
Then, you have to answer the questions displayed by typing the desired number such as :
  * choose the number of cities 
  * choose the number of stores  
  * the maximum value that the demand can take
  * ...

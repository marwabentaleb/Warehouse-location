
// -------------------------------------------------------------- -*- C++ -*-
// File : DataGeneration.cpp
// --------------------------------------------------------------------------

/* ------------------------------------------------------------


Goal is to generat a data file

------------------------------------------------------------ */

#include <iostream>
#include <vector>
#include <time.h>
#include<cstdlib>
#include <numeric>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>

using namespace std;

int main() 
{	
	srand((unsigned int)time(NULL));
	float maxDemande, minDemande, minInstallationCost, maxInstallationCost, minTransportationCost, maxTransportationCost;
	int n, m, rnd, sumDemand, range;
	bool IsInteger = false;
	bool SufficientCapacity = false;

	vector<int> Demand;  
	vector<int> Capacity;
	vector<int> InstallationCost;
	vector< vector<int> > TransportationCost;

	cout << "choose the number of cities : \n";
	cin >> n;
	cout << "choose the number of stores : \n";
	cin >> m;
	
	// creation of demand vector
	
	while(IsInteger == false){
		cout << "Enter the max of demande (integer): \n";
		cin >> maxDemande;
		if (maxDemande == (int) maxDemande){
			IsInteger = true;
		}
	}
	
	IsInteger = false;
	while(IsInteger == false){
		cout << "Enter the min of demande (integer): \n";
		cin >> minDemande;
		if ((minDemande == (int) minDemande) and (maxDemande > minDemande)){
			IsInteger = true;
		}
	}

	range = maxDemande - minDemande + 1;
	for(int i=0; i<n; i++){
		rnd = minDemande + rand()%range;
		Demand.push_back(rnd);
	}
	cout << "------------------ Demand vector is created -----------------\n" << endl;
	// creation of Capacity vector
	for(int j=0; j<m; j++){
		Capacity.push_back(0);	
	}
 
	sumDemand = accumulate(Demand.begin(), Demand.end(), 0);

	int minDemd = *(min_element( Demand.begin(), Demand.end()));

	int maxDemd = *(max_element( Demand.begin(), Demand.end()));

	while (SufficientCapacity == false){
		for(int j=0; j<m; j++){
			Capacity[j] = maxDemd/2 + rand()%(2*maxDemd - minDemd/2 +1);
		}
		if (accumulate(Capacity.begin(), Capacity.end(), 0) > sumDemand  ) {
			SufficientCapacity = true;
		}
	}
	cout << "----------------- Capacity vector is created ----------------\n" << endl;

	// creation of InstallationCost vector
	IsInteger = false;
	while(IsInteger == false){
		cout << "Enter the max of installation cost (integer): \n";
		cin >> maxInstallationCost;
		if (maxInstallationCost == (int) maxInstallationCost){
			IsInteger = true;
		}
	}

	IsInteger = false;
	while(IsInteger == false){
		cout << "Enter the min of installation cost (integer): \n";
		cin >> minInstallationCost;
		if ((minInstallationCost == (int) minInstallationCost) and (maxInstallationCost > minInstallationCost)){
			IsInteger = true;
		}
	}
	
	range = maxInstallationCost - minInstallationCost + 1;
	for(int i=0; i<m; i++){
		rnd = minInstallationCost + rand()%range;
		InstallationCost.push_back(rnd);
	}

	cout << "------------ Installation cost vector is created ------------\n" << endl;

	// creation of TransportationCost matrix

	IsInteger = false;
	while(IsInteger == false){
		cout << "Enter the max of transportation cost (integer): \n";
		cin >> maxTransportationCost;
		if (maxTransportationCost == (int) maxTransportationCost){
			IsInteger = true;
		}
	}

	IsInteger = false;
	while(IsInteger == false){
		cout << "Enter the min of transportation cost (integer): \n";
		cin >> minTransportationCost;
		if ((minTransportationCost == (int) minTransportationCost) and (maxTransportationCost > minTransportationCost)){
			IsInteger = true;
		}
	}
	range = maxTransportationCost - minTransportationCost + 1;
	vector<int> tmp(m);

	for(int i=0; i<n; i++){
		
		for(int j=0; j<m; j++){
			rnd = minTransportationCost + rand()%range;
			tmp[j] = rnd;
		}
		TransportationCost.push_back(tmp);
	}

	cout << "----------- Transportation cost matrix is created -----------" << endl;
	
	// Creation of data file



        ofstream file("data.dat", ios::out | ios::trunc);  

 

        if(file){ 

		// number of cities
		file << n << "|" << endl;

		// number of store
		file << m << "|" << endl;

		// demand
		for(int i=0; i<n-1; i++){
			file << Demand[i] << ",";
		}
		file << Demand[n-1]<< "|" << endl;

		// Installation cost
		for(int i=0; i<m-1; i++){
			file << InstallationCost[i] << ",";
		}
		file << InstallationCost[m-1]<< "|" << endl;

		// Capacity
		for(int i=0; i<m-1; i++){
			file << Capacity[i] << ",";
		}
		file << Capacity[m-1]<< "|" << endl;

		// Transportation cost
		for(int i=0; i<n; i++){
			for(int j=0; j<m-1; j++){
				file << TransportationCost[i][j] << ",";
			}
			file << TransportationCost[i][m-1] << "|" << endl;
		}

		cout << "data file is ready" << endl;
                file.close();  
	}else  
                cerr << "Impossible to open the file !" << endl;

	return 0;
}

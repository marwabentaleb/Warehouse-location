
// -------------------------------------------------------------- -*- C++ -*-
// File : MILP.cpp
// --------------------------------------------------------------------------

/* ------------------------------------------------------------

Problem Method
-------------------

This file containt a proposal of a greedy algorithm.

------------------------------------------------------------ */

#include "GetData.hpp"
#include <ilcplex/ilocplex.h>
#include <vector>
#include <algorithm> 
#include <stdio.h>
#include <utility>

ILOSTLBEGIN

typedef IloArray<IloNumArray> FloatMatrix;

IloInt nbcity, nbstore;

IloNumArray D, S, F;

FloatMatrix C;

bool sortbysec(const pair<int,int> &a, const pair<int,int> &b){
	// function to compare two pairs
    	return (a.second < b.second); 
} 

class store { 
	// class to keep the numero of the store and the cities served by it 
	// as well as the quantity served for each city
	public :
		store(int num, vector<pair <int,float> > vec); // builder
		int num_store; // numero of the store
		vector<pair <int,float> > supply_sended; // vector contains th numero of the city 
							// as well as the quantity served by the store
};

store::store(int num, vector<pair <int,float> > vec){
	// builder
	num_store = num;
	for(int i =0; i< vec.size(); i++){
		supply_sended.push_back(vec[i]);
	}
}

struct c_unique {
  int current;
  c_unique() {current=0;}
  int operator()() {return ++current;}
} UniqueNumber;

int OpenSite(const IloEnv env, const vector<int> remainingSites, const vector<int> citesNotLabeled){
	// find the position of the store to open according to the total cost
	vector<float> costTmp;
	float cost_tmp;
	for(int i=0; i<remainingSites.size(); i++){
		cost_tmp=F[remainingSites[i]-1];

		for(int j=0; j < citesNotLabeled.size(); j++){

			cost_tmp += C[citesNotLabeled[j]-1][i];
		}
		costTmp.push_back(cost_tmp);
	}
	vector<float>::iterator result;
	result = find( costTmp.begin(), costTmp.end(), *(min_element(costTmp.begin(), costTmp.end())) );
	return (distance(costTmp.begin(), result));	
}


int main(int, char**) {
	// initialisation
   	IloEnv env;
	char data[] = "data.dat";
	int posSiteToOpen;
	float capacity_occupped;
	float supply;
	int nb;
	// vector contains the demand of each city
	vector<float> Demand_tmp;
	getData(env,data,D,S,F,C,nbcity,nbstore);
	for(int i =0; i< nbcity; i++){
		Demand_tmp.push_back(D[i]);
	}
	// vector contains the city which has not been served yet or not the totality of the demand is served
	vector<int> citesNotLabeled(nbcity);
	// store has not been opened yet
	vector<int> remainingSites(nbstore);
	// we asume that all the stores are closed. So all the cites are not labeled
    	generate(citesNotLabeled.begin(), citesNotLabeled.end(), UniqueNumber);
	generate(remainingSites.begin(), remainingSites.end(), UniqueNumber);

	vector<pair <int,float> > vec_tmp;
	vector<int> cites_to_label;
	vector<pair <int,float> > CostTransportation_store_tmp;

	vector<store> store_supply_city;
	int open_store =0;
	// we stop when the entire request is served
	do{
		posSiteToOpen = OpenSite(env,remainingSites,citesNotLabeled);
		capacity_occupped = 0;
		vec_tmp = vector<pair <int,float> >();
		cites_to_label = vector<int>();
		CostTransportation_store_tmp = vector<pair <int,float> >();
		// Fill the vector 
		for(int i=0; i < citesNotLabeled.size(); i++){
			CostTransportation_store_tmp.push_back(make_pair(i,C[citesNotLabeled[i]-1][remainingSites[posSiteToOpen]-1]));
		}

		// Storing the vector by second item (cost transportation)
		sort(CostTransportation_store_tmp.begin(), CostTransportation_store_tmp.end(), sortbysec);

		for(int i=0; i < CostTransportation_store_tmp.size(); i++){
			
			supply = min(Demand_tmp[CostTransportation_store_tmp[i].first],(float) S[remainingSites[posSiteToOpen]-1]-capacity_occupped);
			capacity_occupped += supply;
			if (supply !=0){
				vec_tmp.push_back(make_pair(citesNotLabeled[CostTransportation_store_tmp[i].first],supply));
			}
			if (supply == Demand_tmp[CostTransportation_store_tmp[i].first]){
				cites_to_label.push_back(citesNotLabeled[CostTransportation_store_tmp[i].first]);

			}else {Demand_tmp[CostTransportation_store_tmp[i].first] -= supply;}
			if (capacity_occupped == S[remainingSites[posSiteToOpen]-1]){break;}
		}


		for(int i=0; i<cites_to_label.size();i++){
			vector<int>::iterator result = find(citesNotLabeled.begin(), citesNotLabeled.end(), cites_to_label[i]);
			citesNotLabeled.erase(citesNotLabeled.begin() + distance(citesNotLabeled.begin(), result));
		}

		store store_tmp(remainingSites[posSiteToOpen],vec_tmp);
		store_supply_city.push_back(store_tmp);

		remainingSites.erase(remainingSites.begin() + posSiteToOpen);
	}while(citesNotLabeled.size()>0);

	// Print the result 
	env.out() << "----------------------------------------------\n" << endl;
	for(int i=0; i<store_supply_city.size(); i++){
		env.out() <<"store " <<store_supply_city[i].num_store << " is open to serve :" << endl;
		for(int j=0; j<store_supply_city[i].supply_sended.size(); j++){
			 env.out() << "city "<<(store_supply_city[i].supply_sended[j]).first << " with a quantity of "<< (store_supply_city[i].supply_sended[j]).second << endl;
		}
		env.out() << endl;
	}
	env.out() << "----------------------------------------------" << endl;
	// cost calculation
	float cost_installation = 0;
	float cost_transportation = 0;
	for(int i=0; i<store_supply_city.size(); i++){
		cost_installation += F[store_supply_city[i].num_store-1];
		for(int j=0; j<store_supply_city[i].supply_sended.size(); j++){
			cost_transportation += (store_supply_city[i].supply_sended[j]).second*
			C[(store_supply_city[i].supply_sended[j]).first-1][store_supply_city[i].num_store-1];
		}
	}
	
	env.out() << "\n" << "Cost : " <<cost_installation + cost_transportation << endl;


	
	
   	env.end();
   return 0;
}

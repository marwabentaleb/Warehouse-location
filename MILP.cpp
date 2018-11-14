
// -------------------------------------------------------------- -*- C++ -*-
// File : MILP.cpp
// --------------------------------------------------------------------------

/* ------------------------------------------------------------

Problem Description
-------------------

Goal is to find the optimal place to implement a store and the optimal 
amont of artical sending from the store to the customer (city).

Each installation of a store has a cost F.
Each store has a capacity S.
Each city has a demand D.
Transportation of an artical to the customer (city) has a cost C 
which depends on the store and the ville.

Minimize the total cost (cost of installation and cost of transportation).

------------------------------------------------------------ */

#include "GetData.hpp"
#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef IloArray<IloNumArray> FloatMatrix;

IloInt nbcity, nbstore;

IloNumArray D, S, F;

FloatMatrix C;

static void setmodel(const IloEnv env,IloModel& model, IloBoolVarArray& y, NumVarMatrix& x, FloatMatrix C, IloNumArray F, IloNumArray S, IloNumArray D){
	// to set model

	IloExpr obj(env);
	y = IloBoolVarArray(env,nbstore);
	for(IloInt i=0; i<nbstore;i++ ){
		obj +=F[i]*y[i]; // cost of installation
	}
	
	x = NumVarMatrix(env,nbcity); 
	for(int i=0;i<nbcity;i++) {
		x[i]=IloNumVarArray(env,nbstore, 0.0, IloInfinity, ILOFLOAT);
	}
	
	for(IloInt j=0; j<nbcity;j++ ){
		for(IloInt i=0; i<nbstore;i++){
			obj +=C[j][i]*x[j][i]; // cost of transportation
		}
	}

	model.add(IloMinimize(env, obj));
      	obj.end();

	for(IloInt j=0; j<nbcity; j++){
		model.add(IloSum(x[j]) >= D[j]); // supply must satisfy demand
		for(IloInt i=0; i<nbstore; i++){
			model.add(x[j][i] <= D[j]); // moved supply mustn't surpass demand
		}
	}

	
	for(IloInt i=0; i<nbstore; i++){
		IloExpr c(env);
		for(IloInt j=0; j<nbcity; j++){
			c+= x[j][i];
			model.add(x[j][i] <= S[i]);
		}
		model.add(c <= y[i]*S[i]); // moved supply must respect the capacity and the fact that the store is installed 
	}
}

int main(int, char**) {
   IloEnv env;
   try {

	char data[] = "data.dat"; // file's name that contains the data
	getData(env,data,D,S,F,C,nbcity,nbstore); // get data
	
	IloBoolVarArray y(env,nbstore); // y's initialization
	NumVarMatrix x(env,nbcity); // x's initialization
	IloModel model(env); // model's initialization
	setmodel(env, model, y, x, C, F, S, D); // model's creation

	IloCplex cplex(model); // solveur
	cplex.extract(model);
      	cplex.exportModel("pb_store.lp");
      	cplex.solve(); // model's resolution

	env.out() << "-Solution status = " << cplex.getStatus() << endl;
	env.out() << "------------------------------------------------------------"<< endl;
      	env.out() << "-Cost = " << cplex.getObjValue() << endl;
	env.out() << "------------------------------------------------------------"<< endl;
	env.out() << "-Solution : \n" << endl;
	 
	for(IloInt i=0; i<nbstore; i++){
		env.out() << "store_" << i + 1<< " : " << cplex.getValue(y[i]) << "\t";
	}
	env.out() << "\n" << endl;      	
	
	for(IloInt j=0; j<nbcity; j++){
		for(IloInt i=0; i<nbstore; i++){
			env.out() << cplex.getValue(x[j][i]) << "\t";	
		}
		env.out() << "\n";
	}    
	env.out() << "------------------------------------------------------------"<< endl;  
   }
   catch (IloException& ex) {
      cerr << "Error: " << ex << endl;
   }
   catch (...) {
      cerr << "Error" << endl;
   }
   env.end();
   return 0;
}



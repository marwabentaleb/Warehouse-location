#ifndef GETDATA_HPP
#define GETDATA_HPP

#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

typedef IloArray<IloNumArray> FloatMatrix;

void getData(const IloEnv env, char nom_fichier[], IloNumArray& D, IloNumArray& S, IloNumArray& F, FloatMatrix& C, IloInt& nbcity, IloInt& nbstore) { 
	// to get data from data file

	ifstream fichier(nom_fichier);

	if(fichier){

		int nb_lu;
		char un_char;

		fichier >> nbcity >> un_char;
		fichier >> nbstore >> un_char;

		D=IloNumArray(env,nbcity); 
		for(int j=0;j<nbcity;j++) {
			fichier >> nb_lu >> un_char;
			D[j]=nb_lu; 
		} 
		
		F=IloNumArray(env,nbcity); 
		for(int j=0;j<nbstore;j++) {
			fichier >> nb_lu >> un_char;

			F[j]=nb_lu; 
		} 

		S=IloNumArray(env,nbcity); 
		for(int j=0;j<nbstore;j++) {
			fichier >> nb_lu >> un_char;
			S[j]=nb_lu; 
		}
		
		C = FloatMatrix(env,nbcity); 
		for(int i=0;i<nbcity;i++) {
			C[i]=IloNumArray(env,nbstore);
		}

		for(int i=0;i<nbcity;i++) {
			for(int j=0;j<nbstore;j++) {
				fichier >> nb_lu >> un_char;
				C[i][j] = nb_lu; 
			}
		}
	}
}

#endif

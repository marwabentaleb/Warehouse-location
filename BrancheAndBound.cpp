
// -------------------------------------------------------------- -*- C++ -*-
// File: BrancheAndBound.cpp
// --------------------------------------------------------------------------

/* ------------------------------------------------------------

Methode Description
-------------------

Goal is to apply Branche and Bound methode to solve the problem.

------------------------------------------------------------ */

#include <ilcplex/ilocplex.h>
#include <ilconcert/ilomodel.h>
ILOSTLBEGIN

typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef IloArray<IloNumArray> FloatMatrix;
typedef IloArray<IloIntArray> IntMatrix;

IloInt nbcity, nbstore, nbIterations, nbIter, nbIterCourant;

IloNumArray D, S, F;

FloatMatrix C;

class Node { 
	public :
		Node(IloInt lev, IloInt statNode, IloInt visitNb, IntMatrix* ptCon, IloNum c, const IloEnv env); //Constructeur
		IloInt level; // level of node in decision tree 
		IntMatrix* ptConstraint; // pointer to the constraints that different from those of initial problem
		IloInt statusNode; // Node is rejected or not (infeasible or found a integer solution)
		IloInt visitNumber; // Number of the visit
		IloNum cost; //cost of function
		class Node* child1; 
		class Node* child2;  
		
};

Node::Node(IloInt lev, IloInt statNode, IloInt visitNb, IntMatrix* ptCon, IloNum c, const IloEnv env){
	level = lev;
	statusNode = statNode;
	visitNumber = visitNb;
	ptConstraint = ptCon;
	cost = c;
}
void getData(const IloEnv env, char nom_fichier[]) { 
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

void solveModel(const IloEnv env, Node& node, FloatMatrix C, IloNumArray F, IloNumArray S, IloNumArray D, IloNumVarArray y, NumVarMatrix x, IloNum& bestCostInteger, Node* ptBestNode){
	nbIterCourant +=1;
	IloModel model(env);
	y = IloNumVarArray(env,nbstore,0.0,1.0,ILOFLOAT);

	// cost function 
	IloExpr obj(env);
	
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
	
	// constraints initial pb 
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
	
	// add constraints related to the node 

	try{
		nbIter = 0;
		for(IloInt i=0; i<nbIterations; i++){

			if ( IloSum((*(node.ptConstraint))[i]) == 0 ){
				nbIter = i;
				break;
			}else{
				for(IloInt j=0; j<nbstore; j++){
					if ((*(node.ptConstraint))[i][j] == 1){
						model.add(y[j] == 1);
					}
					if ((*(node.ptConstraint))[i][j] == -1){
						model.add(y[j] == 0);
					}
				}
			}
		}
		//model.add(*(node.ptConstraint));
		env.out() << "OK" << endl;
	}
	catch (IloException& ex) {
	      cerr << "Error: " << ex << endl;
	}
	// solving the problem
	IloCplex cplex(model); // solveur
	cplex.extract(model);
      	cplex.exportModel("pb_store.lp");
      	cplex.solve(); // model's resolution
	env.out()<< cplex.getStatus() << endl;
	
	try{
		// optimal solution
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

		// Up data of the node 
		node.statusNode = 0; // we assume the solution is integer
		IloInt firstFraction = 0; 
		while (firstFraction < nbstore) {
			if (cplex.getValue(y[firstFraction]) != (IloInt) cplex.getValue(y[firstFraction])) {
				node.statusNode = 1;
				break;
			}else firstFraction+=1;
		}
		node.cost = cplex.getObjValue();
		
		if (node.statusNode == 0){ // we found an integer solution that means we should up date the best cost integer
			if (bestCostInteger > node.cost){ 
				bestCostInteger = node.cost;
				ptBestNode = &node;
			}
		}else{
			if (node.cost >= bestCostInteger){
				node.statusNode ==0;
			}else{
				
				// Child1 
				IntMatrix constraintsChild1(env);
				// Child2
				IntMatrix constraintsChild2(env);

				constraintsChild1 = (*(node.ptConstraint));
				constraintsChild2 = (*(node.ptConstraint));

				// we branch on the first fraction of the solution
				constraintsChild1[nbIter][firstFraction] = 1;
				IntMatrix* ptConstraintsChild1 = &constraintsChild1;
				
				Node child1(node.level,1, node.visitNumber + 1,ptConstraintsChild1,IloInfinity,env);
				env.out() << *child1.ptConstraint << endl;
				if (nbIterCourant <nbIterations){
					solveModel(env,child1,C,F,S,D,y,x,bestCostInteger,ptBestNode);
				}

				env.out() << "**************************" << endl;

				constraintsChild2[nbIter][firstFraction] = -1;
				IntMatrix* ptConstraintsChild2 = &constraintsChild2;
				Node child2(node.level,1, node.visitNumber + 2,ptConstraintsChild2,IloInfinity,env);
				env.out() << *child2.ptConstraint << endl;
				if (nbIterCourant <nbIterations){
					solveModel(env,child2,C,F,S,D,y,x,bestCostInteger,ptBestNode);
				}

			}
		}
	}
	catch (IloException& ex) {
		// No solution	
		// Up data of the node 
		node.statusNode = 0;
		node.cost = IloInfinity;
		cerr << "Error: " << ex << endl;
	}
}

int main(int, char**) {
   IloEnv env;
   try {

	nbIterations = 30;
	char data[] = "data2.dat"; // file's name that contains the data
	getData(env,data); // get data
	IntMatrix* ptCon;
	IntMatrix cons(env); 

	cons = IntMatrix(env,nbIterations); 

	for(int i=0;i<nbIterations;i++) {
		cons[i]=IloIntArray(env,nbstore);
	}
	ptCon = &cons;

	Node node(0,1,0,ptCon,0,env);
	IloNumVarArray y(env,nbstore); // y's initialization
	NumVarMatrix x(env,nbcity); // x's initialization
	IloInt bestVisit = 0;
	IloNum bestCostInteger = IloInfinity;
	Node* ptBestNode;

	nbIterCourant = 0;
	solveModel(env,node,C,F,S,D,y,x,bestCostInteger,ptBestNode);
	env.out() << "------------------------------------------------" << endl;
	env.out() << "Nb iterations : " << nbIterCourant << endl;
	env.out() << "Optimal cost : " << bestCostInteger << endl;
	
	


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







#include <iostream>
#include <string>
#include <list>
#include <map>
#include <fstream>
#include <vector>
#include <sstream>
#include <queue>
#include "math.h"

using namespace std;

//separe une string un tableau selon un delimiteur
std::vector<std::string> &split(const std::string &s, char delim,
		std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

class Graphe {

public:

	int getID(string s);
	Graphe(int n, const string& path);
	list<int> getNeighbours(int i);
	int getSize();
	string getName(int i);

	list<int> pathbis(string source, string target);
	list<int> path(int source, int target); // c'est là qu'est implémenté dijstrka

	//affiche la correspondance string -> int
	void printIDs();

	//affiche la correspondance int -> string
	void printNames();

	//affiche la relation d'adjacence
	void printAdjacence();

private:

	int size;
	string* name;
	list<int>* adjacence;
	list<int>* pred;
	map<string, int> IDs;

};

//affiche la correspondance ID-Nom
void Graphe::printIDs() {
	for (std::map<string, int>::iterator it = this->IDs.begin();
			it != this->IDs.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
	return;
}

//affiche la correspondance Nom-ID
void Graphe::printNames() {
	for (int i = 0; i < this->size; i++) {
		if (name[i].compare("")) {
			std::cout << this->name[i] << std::endl;
		}
	}
	return;
}

//affiche le vecteur d'adjacence, adjacence[i] contient la liste des succésseurs de i
void Graphe::printAdjacence() {
	for (int i = 0; i < size; i++) {
		if(name[i]!=""){
		std::cout << name[i] << ": [";
		for (std::list<int>::iterator it = adjacence[i].begin();
				it != adjacence[i].end(); ++it) {
			std::cout << name[(*it)] << ";";
		}
		std::cout << "]" << std::endl;
	}
	}
	return;
}

int Graphe::getID(string s) {
	return (this->IDs.find(s)->second);
}

list<int> Graphe::getNeighbours(int i) {
	return (adjacence[i]);
}

int Graphe::getSize() {
	return (this->size);
}

string Graphe::getName(int i) {
	return (name[i]);
}

//constructeur de graphe
Graphe::Graphe(int n, const string& path) {
	this->size = n;

	//allocation des différents tableaux
	this->name = new string[n];
	this->adjacence = new list<int> [n];
	this->pred = new list<int> [n];

	//début du parsing du fichier avec les stations
	int i = 0;
	string fileString[n];
	ifstream fichier("metro1");
	if (!fichier) {
		cout << "impossible de trouver le fichier" << endl;
		//assert(fichier);
	} else {
		string tmp;
		while (!fichier.eof()) {
			getline(fichier, tmp); //lecture d'une ligne du fichier

			if (tmp.compare("") != 0) //on check si la ligne n'est pas vide
					{
				std::vector<std::string> elems;
				if (tmp[0] != '#') {
					split(tmp, ':', elems);
					string tmp2 = elems[1];
					if ((this->IDs.find(tmp2) == this->IDs.end())
							&& (tmp2.size() != 0)) {
						std::cout << "élement n°: " << i << ", nom :" << tmp2
								<< std::endl;
						fileString[i] = tmp2;
						IDs[tmp2] = i;
						name[i] = tmp2;
						i++;
					}
				} else {
					fileString[i] = "#";
					i++;
				}
			}
		}
		std::cout << "fin du parsing correspondance nom - chiffre" << std::endl;
	}
	std::cout << "printing du tableau " << std::endl;
	for (int j = 0; j < n; j++) {
		if (fileString[j].compare("")) {
			std::cout << fileString[j] << std::endl;
		}
	}

	//on initialise le vecteur d'adjacence sachant que IDs est initialisé.
	ifstream fichier2("metro2");
	if (!fichier2) {
		std::cout << "impossible de trouver le fichier" << std::endl;

	} else {

		string tmp;
		string tmpPrevious;
		string tmpCurrent;
		bool reset = false;

		while (!fichier2.eof()) {
			getline(fichier2, tmp);
			std::vector<std::string> elems;
			if (tmp.compare("") != 0) {
				if (tmp[0] != '#') {

					if (reset == false) {
						std::cout << "tmp Current = " << tmpCurrent;
						std::cout << "tmp Previous = " << tmpPrevious;
						std::cout << std::endl;

						tmpPrevious = tmpCurrent;
						split(tmp, ':', elems);
						tmpCurrent = elems[1];
						(this->adjacence[IDs[tmpPrevious]]).push_back(
								IDs[tmpCurrent]);
						//std::cout << "debug 1 \n";
						(this->pred[IDs[tmpCurrent]]).push_back(
								IDs[tmpPrevious]);
						//std::cout << "debug 2 \n";
					} else {
						split(tmp, ':', elems);
						tmpCurrent = elems[1];
						reset = false;
					}

				} else {
					reset = true;
					//split(tmp,':',elems);
					//tmpCurrent=elems[1];
				}
			}
		}
	}
	std::cout << "fin de la création du graphe" << std::endl;

}


//on crée une classe qui implémente la méthode "operator" pour pouvoir lui donner en argument lors de la création de la file de priorité
class CompareDist {
public:
	bool operator()(pair<int, int> n1, pair<int, int> n2) {

		if (n1.second > n2.second)
			return true;
		else
			return false;
	}
};

list<int> Graphe::path(int source, int target) {
	int distance[this->size];
	bool checked[this->size];
	for (int i = 0; i < this->size; i++) {
		checked[i] = false;
	}
	checked[source] = true;
	std::priority_queue<pair<int, int>, vector<pair<int, int> >, CompareDist> still; //contient les sommets qu'il reste à explorer et leur distance, priority queue permet de trouver celui qui realise la distance minimale en temps logarithmique.

	//initialisation (999= + l'infini)
	for (int i = 0; i < size; i++) {
		if (i != source) {
			still.push(make_pair(i, 999));
		} else {
			still.push(make_pair(i, 0));
		}
		distance[i] = 999;
	}

	distance[source] = 0;

	list<int> solution; //liste qui contient le chemin optimal.

	int s = -1;
	//début de la boucle
	while (!still.empty() && (s != target)) {
		int s = still.top().first; //on prend celui qui minimise le deuxieme élément des pairs, ie la distance à la target
		checked[s] = true;
		
		//log de debuggage
		//std::cout << s << "," << distance[s] << std::endl;
		still.pop();
		for (list<int>::iterator it = adjacence[s].begin();
				it != adjacence[s].end(); ++it) {
			distance[(*it)] = min(distance[(*it)], distance[s] + 1);
			
			//log de debuggage		
			//std::cout << "distance :" << (*it) << " : " << distance[*it]
			//		<< std::endl;

			if (!checked[*it]) {
				still.push(make_pair((*it), distance[(*it)]));
			}
		}
	}

	//log de débuggage
	//std::cout << "fin du calcul des distances et d = " << distance[target]
	//		<< std::endl;


	//on a calculé les distances, maintenant on fait l'inverse pour trouver le chemin
	bool reach = false;
	int current = target;
	int node;
	while (!reach) {
		std::cout << "current = " << current << "et d = " << distance[current]
				<< std::endl;
		for (list<int>::iterator it = pred[current].begin();
				it != pred[current].end(); ++it) {
			if ((*it) == source) {
				reach = true;
				solution.push_front(source);
			} else {
				if (distance[(*it)] == distance[current] - 1) {
					node = (*it);
				}
			}
		}
		std::cout << "node = " << node << std::endl;
		if(!reach){solution.push_front(node);}
		current = node;
	}

	std::cout << " fin du backtracking" << std::endl;
	std::cout << "Pour aller de " << name[source] << " à " << name[target]
			<< " :";
	for (list<int>::iterator it = solution.begin(); it != solution.end();
			++it) {
		std::cout << name[*it] << "(" << distance[*it] << ")" << ";";
	}
	return solution;
}

list<int> Graphe::pathbis(string s1, string s2) {
	return (this->path(IDs[s1], IDs[s2]));
}

class Menu{

public:
	Menu(string* c, int n){
		this->c=c;
		this->length=n;
        }	
	void afficher(){
		std::cout << "\n";
		for(int i=0;i<this->length;i++){
			std::cout <<"[" << i << "]" << " " << c[i] << std::endl;
		}		
	}
	
private:
	string *c;
	int length;
};


int main() {
	Graphe sub = Graphe(400, "metro1");
	//sub.printAdjacence();
	//sub.path(2, 5);
	//sub.pathbis("Denfert Rochereau", "Villiers");

		
        string rep;
        for(int i=0;i<150;i++){
		rep[i]='\0';
	}
        string c []={"Afficher les stations", "Afficher la relation", "calculer plus court chemin"};
	Menu m = Menu(c,3);
	
	while(true){
		m.afficher();
		int r;
		std::cin >> r;
 		switch (r){
		
		case 0 :
		  sub.printNames();
		  break;
		case 1 :
		  sub.printAdjacence();
               	  break;
		case 2 :
		  string tmp="";
		  string source;
		  string source2;

		  std::cout << "Point de départ?" << std::endl;
 		  //std::getline (std::cin,source);
		  cin>>source;
		  //std::cout << source << " " <<source2 << std::endl;

		  string target;
		  std::cout << "Point d'arrivée?" << std::endl;
		  //std::getline (std::cin,target);
		  cin>>target;
		  

		  sub.pathbis(source,target);
		  std::cout << target;

		}
        }
	return 0;
}


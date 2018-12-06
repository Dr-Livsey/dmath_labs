#include <map>
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <fstream>
#include <sstream>
#include <functional>
#include "Interface.h"

#define or ||
#define and &&
#define AllPaths -1

typedef std::vector<std::vector<short>> Matrix;
typedef std::vector<std::pair<unsigned, short>> Paths;


class Graph
{

public:
	Graph(std::string fname);
	Graph(const char *fname) { isOriented = false; readGraph(std::string(fname)); }
	Graph(size_t);
	Graph(bool isor = false) : isOriented(isor){}
	Graph(const Graph& other) { *this = other; }
	~Graph();

	size_t vertexAmount() const;
	size_t edgeAmount() const;

	void showDistance(short a, short b) const;
	void showEccentricity(short vertex) const;
	void showRadius() const;
	void showDiameter() const;
	void showCenter() const;
	void showPeripheralVertex() const;

	void showSources() const;
	void showStocks() const;
	void showIzolatedVertexes() const;
	void showLeafs() const;

	void showDegSequance() const;
	void showOrDegSequence() const;
	void showVertexDegree(short vertex) const;

	void showAdjacencyList() const;
	void showAdjacencyList(std::string) const;

	void showAdjacencyMatrix() const;
	void showIncidenceMatrix() const;
	void showWeightedMatrix() const;

	void DeleteVertex(short vertex);
	void DeleteEdge(short a, short b);
	void AddEdge(short a, short b);

	void AddVertex(std::string);
	void AddVertex(std::vector<short>);

	/*дополнение графа*/
	void getAdditionGraph();
	/*подразбиение ребра*/
	void splitEdge(short, short);
	/*стягивание ребра*/
	void pullOffEdge(short, short);
	/*стягивание графа*/
	void pullOffGraph(const std::vector<short> &);
	/*отождествление вершин*/
	void vertexIdentification(short, short);
	/*дублирование вершины*/
	void vertexDuplicate(short vertex);
	/*размножение вершины*/
	void vertexReproduction(short vertex);

	/*Union graphs*/
	Graph operator||(const Graph&);
	Graph operator&(const Graph&);
	/*Connect graphs*/
	Graph operator*(const Graph&);
	/*Multiply graphs*/
	Graph operator+(const Graph&);

	Graph& operator=(const Graph&other)
	{
		this->isOriented = other.isOriented;
		this->AdjacencyMatrix = other.AdjacencyMatrix;
		return *this;
	}

	/*Lab2*/
	void algorithmPrima();
	void showBfs();
private:
	/*возвращает вершины в разных компонентах связности*/
	std::vector<short> con_components();
	void bfs(short v);
	Matrix carcass_to_matrix(const std::vector<std::pair<int,short>>&);
	/******/

	bool isOriented;
	bool isWeighted;
	Matrix WeighedMatrix;
	Matrix AdjacencyMatrix;

	void readGraph(std::string);
	void readAdMatrix(std::ifstream&);
	void readAdList(std::ifstream&);
	void readIMatrix(std::ifstream&);

	bool checkAdNotOrientedMatrix();

	std::vector<short> getVertexVector(std::string);
	std::list<std::string> getRowList(std::ifstream&);

	void showMatrix(const Matrix&m, std::string name) const;

	Matrix buildIMatrix() const;
	Matrix buildAdList() const;

	size_t inDegree(short vertex) const;
	size_t outDegree(short vertex) const;
	size_t getVertexDegree(short vertex) const;

	std::vector<size_t> getDegSequence() const;

	std::vector<int> selectVertexes(std::function<bool(short)>) const;

	Paths getAllPaths(short a) const;
	std::pair<unsigned, short> getEccentricity(short vertex) const;
	std::pair<unsigned, short> getRadius() const;
	std::pair<unsigned, short> getDiameter() const;
	std::vector<short> getCenter() const;
	std::vector<short> getPeriferalVertexes() const;

	bool isConnectedGraph(Paths p) const;

	void resizeAdMatrix(size_t);
	Matrix& resizeAdMatrix(Matrix&, size_t);
};


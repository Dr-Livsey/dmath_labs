#include <map>
#include <vector>
#include <list>
#include <functional>

typedef std::vector<std::vector<short>> Matrix;
typedef std::vector<std::pair<unsigned, short>> Paths;

/*TODO:
	1. Соединение графов
	2. Произведение графов*/

class Graph
{

public:
	Graph(std::string fname);
	Graph();
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
	void showVertexDegree(short vertex) const;

	void showAdjacencyList() const;
	void showAdjacencyMatrix() const;
	void showIncidenceMatrix() const;

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
	/*отождествление вершин*/
	void vertexIdentification(short, short);
	/*дублирование вершины*/
	void vertexDuplicate(short vertex);
	/*размножение вершины*/
	void vertexReproduction(short vertex);

	/*Union graphs*/
	Graph operator||(const Graph&);
	/*Connect graphs*/
	Graph operator*(const Graph&);
	/*Multiply graphs*/
	Graph operator+(const Graph&);

private:
	bool isOriented;
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
	Paths getCenter() const;
	Paths getPeriferalVertexes() const;

	bool isConnectedGraph(Paths p) const;

	void resizeAdMatrix(size_t);
};


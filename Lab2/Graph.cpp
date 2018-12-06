#include "Graph.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <deque>
#include <queue>
#include <fstream>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/bind.hpp>

using std::cout;
using std::endl;

auto chomp = [](std::string &s) -> std::string
{
	std::size_t found = s.find_last_not_of(" \t\f\v\n\r");
	if (found != std::string::npos) s.erase(found + 1);
	return s;
};
auto transposeMatrix = [](Matrix &m)
{
	Matrix tm(m[0].size());
	for (std::vector<short> &v : tm) v.resize(m.size());

	for (int i = 0; i < m.size(); i++)
	{
		for (int j = 0; j < m[0].size(); j++)
		{
			tm[j][i] = m[i][j];
		}
	}
	m = tm;

	tm.clear();
	Matrix().swap(tm);
};

/*Vector push front*/
void push_front(std::vector<short>&v, short value)
{
	std::reverse(v.begin(), v.end());
	v.push_back(value);
	std::reverse(v.begin(), v.end());
}
void push_front(Matrix& m, std::vector<short> newVector)
{
	std::reverse(m.begin(), m.end());
	m.push_back(newVector);
	std::reverse(m.begin(), m.end());
}

/*Vector remove duplicates*/
template<typename T> 
void vector_uniq(std::vector<T>&v)
{
	std::sort(v.begin(), v.end());
	auto last = std::unique(v.begin(), v.end(),
		[](T &a, T &b)
	{
		if (std::is_same<T,  std::pair<unsigned, short>>::value) 
			return a.second == b.second;
		
		return a == b;
	});

	v.erase(last, v.end());
}


bool comparePathItems(std::pair<unsigned, short>& a, std::pair<unsigned, short>& b)
{
	/*if second == -1 then this vertex does not have parent*/
	return b.second != -1 and (b > a or a.second == -1);
}

Graph::Graph(std::string fname)
{
	isOriented = false;
	isWeighted = false;
	readGraph(fname);
}
Graph::Graph(size_t initSize)
{
	isOriented = false;
	isWeighted = false;
	resizeAdMatrix(initSize);
	resizeAdMatrix(WeighedMatrix, initSize);
}

Graph::~Graph()
{
}

size_t Graph::vertexAmount() const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	cout << "The number of vertices: " 
		 << AdjacencyMatrix.size() << endl;

	return AdjacencyMatrix.size();
}
size_t Graph::edgeAmount() const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	Matrix IMatrix = buildIMatrix();
	size_t edgesAmount = IMatrix[0].size();

	cout << "The number of edges: "
		<< edgesAmount << endl;

	IMatrix.clear();
	Matrix().swap(IMatrix);

	return edgesAmount;
}

void Graph::showDistance(short a, short b) const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	try
	{
		a--;
		WeighedMatrix.at(a);

		if (b != AllPaths)
		{
			b--;
			WeighedMatrix.at(b);
		}
	}
	catch (const std::out_of_range&)
	{
		throw std::exception("One or both vertices do not belong to the graph.");
	}

	Paths paths = getAllPaths(a);

	std::function<void(short)> showPath = [&showPath, &paths](short v)
	{
		if (paths[v].second != v)
		{
			showPath(paths[v].second);
		}

		if (v == paths[v].second )
			cout << v + 1;
		else 
			cout << "->" << v + 1;
	};

	if (b == AllPaths)
	{
		for (int i = 0; i != paths.size(); i++)
		{
			if (paths[i].second < 0) continue;

			cout << "Vertex: [" << i + 1 << "] Distance: " << paths[i].first << " Path: ";
			showPath(i);
			cout << endl;
		}
	}
	else
	{
		if (paths[b].second < 0)
		{
			throw std::exception("There are no routes");
		}

		cout << "Vertex: [" << b + 1 << "] Distance: " << paths[b].first << " Path: ";
		showPath(b);
		cout << endl;
	}
}
void Graph::showEccentricity(short vertex) const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	vertex--;
	std::pair<unsigned, short> maxElement = getEccentricity(vertex);
	cout << "Eccentricity of [" << vertex + 1 << "] is " << maxElement.first << endl;
}
void Graph::showRadius() const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	std::pair<unsigned, short>  radius = getRadius();
	cout << "Radius is " << radius.first << endl;
}
void Graph::showDiameter() const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	std::pair<unsigned, short> diameter = getDiameter();
	cout << "Diameter is " << diameter.first << endl;
}
void Graph::showCenter() const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	std::vector<short> centers = getCenter();

	cout << "Center(s): ";
	if (centers.empty()) cout << "empty";
	for (short curCenter : centers)
	{
		cout << "[" << curCenter << "] ";
	}
	cout << endl;
}
void Graph::showPeripheralVertex() const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	std::vector<short> periferals = getPeriferalVertexes();

	cout << "Periferal Vertices: ";

	if (periferals.empty()) cout << "empty";

	for (short curPeriferal : periferals)
	{
		cout << "[" << curPeriferal << "] ";
	}
	cout << endl;
}

void Graph::showSources() const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	auto inDegreeCopy = boost::bind(&Graph::inDegree, this, boost::placeholders::_1);

	if (isOriented == false)
		throw std::exception("Graph is not oriented.");

	std::vector<int> selVertexes = selectVertexes
	([inDegreeCopy](short v) -> bool
	{
		/*Incoming degree is equal to zero.*/
		return inDegreeCopy(v) == 0;
	});

	if (selVertexes.empty())
	{
		cout << "There are no sources." << endl;
		return;
	}

	cout << "Sources:";
	for (int sv : selVertexes)
			cout << " [" << sv + 1 << "]";
	cout << endl;
}
void Graph::showStocks() const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	auto outDegreeCopy = boost::bind(&Graph::outDegree, this, boost::placeholders::_1);

	if (isOriented == false)
		throw std::exception("Graph is not oriented.");

	std::vector<int> selVertexes = selectVertexes
	([outDegreeCopy](short v) -> bool
	{
		/*Outcoming degree is equal to zero.*/
		return outDegreeCopy(v) == 0;
	});

	if (selVertexes.empty())
	{
		cout << "There are no stocks." << endl;
		return;
	}

	cout << "Stocks:";
	for (int sv : selVertexes) cout << " [" << sv + 1 << "]";
	cout << endl;
}

void Graph::showIzolatedVertexes() const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	auto ZeroDegree = boost::bind(&Graph::getVertexDegree, this, boost::placeholders::_1);

	std::vector<int> isolatedVertexes = selectVertexes
	([ZeroDegree](short v) -> bool
	{
		return ZeroDegree(v) == 0;
	});

	if (isolatedVertexes.empty())
		cout << "There are no isolated vertices." << endl;
	else
	{
		cout << "Isolated vertices:";
		for (int iv : isolatedVertexes) cout << " [" << iv + 1<< "]";
		cout << endl;
	}
}
void Graph::showLeafs() const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	auto OneDegree = boost::bind(&Graph::getVertexDegree, this, boost::placeholders::_1);

	std::vector<int> leafs = selectVertexes
	([OneDegree](short v) -> bool
	{
		return OneDegree(v) == 1;
	});


	if (leafs.empty())
		cout << "There are no leafs." << endl;
	else
	{
		cout << "Leafs:";
		for (int lv : leafs) cout << " [" << lv + 1 << "]";
		cout << endl;
	}
}

size_t Graph::inDegree(short vertex) const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");
	else if (isOriented == false)
		throw std::exception("inDegree: Graph is not oriented.");

	unsigned inDegree = 0;

	for (auto rowIt = AdjacencyMatrix.begin(); rowIt < AdjacencyMatrix.end(); rowIt++)
	{
		int rowIdx = rowIt - AdjacencyMatrix.begin();

		inDegree += (rowIdx == vertex) ? (*rowIt).at(vertex) / 2 : (*rowIt).at(vertex);
	}

	return inDegree;
}
size_t Graph::outDegree(short vertex) const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	bool isor = isOriented;

	auto sum = [vertex, isor](const std::vector<short> &v)
	{
		unsigned s = 0;
		for (auto vert = v.cbegin(); vert != v.cend(); vert++)
		{
			short vertexValue = *vert;

			if (isor == true && (vert - v.cbegin()) == vertex)
				s += vertexValue / 2;
			else
				s += vertexValue;
		}
		return s;
	};

	unsigned outDegree = sum(AdjacencyMatrix.at(vertex));

	return outDegree;
}
size_t Graph::getVertexDegree(short vertex) const
{
	size_t outdeg = 0, indeg = 0;

	try
	{
		outdeg = outDegree(vertex);
		if (isOriented)
			indeg = inDegree(vertex);
	}
	catch (const std::exception&)
	{
		std::string warning = "Vertex " + \
			std::to_string(vertex + 1) + \
			" does not belong to this graph.";
		throw std::exception(warning.c_str());
	}

	return outdeg + indeg;
}

std::vector<size_t> Graph::getDegSequence() const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	std::vector<size_t> dVector;
	for (int i = 0; i < AdjacencyMatrix.size(); i++)
		dVector.push_back(getVertexDegree(i));

	std::sort(dVector.begin(), dVector.end(), [](size_t a, size_t b) -> bool
											  {return a > b;});

	return dVector;
}

std::vector<int> Graph::selectVertexes(std::function<bool(short)> f_ptr) const
{
	std::vector<int> selectedVertexes;
	for (int i = 0; i < AdjacencyMatrix.size(); i++)
	{
		if (f_ptr(i) == true)
			selectedVertexes.push_back(i);
	}
	return selectedVertexes;
}

Paths Graph::getAllPaths(short a) const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	using std::vector;
	using std::pair;
	using std::map;
	using std::priority_queue;

	/*Build adjacency list*/
	Matrix AdList = buildAdList();

	/*Deikstra algorithm*/

				/*dist, parent*/
	vector<pair<unsigned, short>> distances(AdList.size());
	std::fill(distances.begin(), distances.end(), std::make_pair(UINT_MAX, -1));
	
	priority_queue<
			/*distance, vertex*/
		pair<unsigned, short>, 
		vector<pair<unsigned, short>>,
		std::greater<pair<unsigned, short>>> nextVertices;

	distances[a] = std::make_pair(0, a);
	nextVertices.push(std::make_pair(0, a));

	while (nextVertices.empty() == false)
	{
		/*Current vertex*/
		pair<unsigned, short> curVertex = nextVertices.top();
		nextVertices.pop();
		
		if (curVertex.first > distances.at(curVertex.second).first) continue;

		vector<short> AdListRow = AdList.at(curVertex.second);
		for (auto AdVertex = AdListRow.cbegin(); AdVertex != AdListRow.cend(); AdVertex++)
		{
			unsigned &lastDistance = distances.at(*AdVertex).first;
			unsigned newDistance = distances.at(curVertex.second).first + WeighedMatrix[curVertex.second][*AdVertex];

			if (lastDistance > newDistance)
			{
				/*Assign parent*/
				distances.at(*AdVertex).second = curVertex.second;
				/*Assign new distance*/
				lastDistance = newDistance;

				nextVertices.push(std::make_pair(lastDistance, *AdVertex));
			}
		}
	}

	return distances;
}

std::pair<unsigned, short> Graph::getEccentricity(short vertex) const
{
	AdjacencyMatrix.at(vertex);

	Paths paths = getAllPaths(vertex);

	//if (isConnectedGraph(paths) == false)
	//	throw std::exception("The graph is not connected");

	Paths::iterator maxElement = std::max_element(paths.begin(), paths.end(), comparePathItems);

	std::pair<unsigned, short> pathItem = *maxElement;

	return pathItem;
}
std::pair<unsigned, short> Graph::getRadius() const
{
	std::vector<std::pair<unsigned, short>> eVector;

	for (int i = 0; i < AdjacencyMatrix.size(); i++)
	{
		std::pair<unsigned, short> eccentricity = getEccentricity(i);
		eVector.push_back(eccentricity);
	}
	
	Paths::iterator minElement = std::min_element
	(
		eVector.begin(), 
		eVector.end(),
		[](std::pair<unsigned, short>& a, std::pair<unsigned, short>& b) -> bool
		{
			return  a.first != 0 && b > a;
		}
	);
	std::pair<unsigned, short> min = *minElement;

	return min;
}
std::pair<unsigned, short> Graph::getDiameter() const
{

	std::vector<std::pair<unsigned, short>> eVector;

	for (int i = 0; i < AdjacencyMatrix.size(); i++)
	{
		std::pair<unsigned, short> eccentricity = getEccentricity(i);
		eVector.push_back(eccentricity);
	}

	Paths::iterator maxElement = std::max_element(eVector.begin(), eVector.end());
	std::pair<unsigned, short> max = *maxElement;

	return max;
}

std::vector<short> Graph::getCenter() const
{

	unsigned radius = getRadius().first;
	std::vector<short> centers;

	for (int i = 0; i < AdjacencyMatrix.size(); i++)
	{
		std::pair<unsigned, short> currentEccentricity = getEccentricity(i);

		if (currentEccentricity.first == radius)
			centers.push_back(i + 1);	
	}

	//vector_uniq<std::pair<unsigned, short>>(centers);
	return centers;
}
std::vector<short> Graph::getPeriferalVertexes() const
{
	unsigned diameter = getDiameter().first;
	std::vector<short> periferals;

	for (int i = 0; i < AdjacencyMatrix.size(); i++)
	{
		std::pair<unsigned, short> currentEccentricity = getEccentricity(i);

		if (currentEccentricity.first == diameter)
			periferals.push_back(i + 1);
	}

	//vector_uniq<std::pair<unsigned, short>>(periferals);

	return periferals;
}

bool Graph::isConnectedGraph(Paths p) const
{
	for (std::pair<unsigned, short> item : p)
	{
		if (item.second < 0)
			return false;
	}

	return true;
}

void Graph::resizeAdMatrix(size_t s)
{
	if (s == AdjacencyMatrix.size()) return;

	AdjacencyMatrix.resize(s);
	for (std::vector<short> &v : AdjacencyMatrix)
		v.resize(s);
}
Matrix & Graph::resizeAdMatrix(Matrix &m, size_t s)
{
	if (s == m.size()) return m;

	m.resize(s);
	for (std::vector<short> &v : m)
		v.resize(s);

	return m;
}

void Graph::showDegSequance() const
{
	if (isOriented == false)
	{
		std::vector<size_t> dVector = getDegSequence();
		cout << "Sequance of degrees:";
		for (size_t deg : dVector) cout << " " << deg << " ";
		cout << endl;
	}
	else showOrDegSequence();
}
void Graph::showOrDegSequence() const
{
	if (isOriented == false)
		throw std::exception("Graph is not oriented.");

	for (int i = 0; i < AdjacencyMatrix.size(); i++) showVertexDegree(i + 1);
}
void Graph::showVertexDegree(short vertex) const
{
	size_t outdeg = 0, indeg = 0;

	//vertex starts from 1.
	try
	{
		outdeg = outDegree(vertex - 1);
		if (isOriented)
			indeg = inDegree(vertex - 1);
	}
	catch (const std::exception&)
	{
		std::string warning = "Vertex " + \
			std::to_string(vertex) + \
			" does not belong to this graph.";
		throw std::exception(warning.c_str());
	}

	if (isOriented == false)
		cout << "Degree of vertex [" << vertex << "] is " << outdeg << endl;
	else
	{
		cout << "Vertex: [" << vertex << "]" << endl;
		cout << "Income degree is " << indeg << endl;
		cout << "Outcome degree is " << outdeg << endl;
	}
}

void Graph::showAdjacencyList() const
{
	if (AdjacencyMatrix.empty())
	{
		cout << "Adjacency List is empty." << endl;
		return;
	}

	Matrix AdList = buildAdList();

	auto view_vector = [](const std::vector<short>&v) 
						{
							for (short c : v) cout << " " << c + 1;
							cout << "}";
						};
	
	cout << "Adjacency List:\n";
	for (auto vertex = AdList.cbegin(); vertex != AdList.cend(); vertex++)
	{
		cout << "{" << vertex - AdList.cbegin() + 1;
		view_vector(*vertex);
	}
	cout << "\n\n";

	AdList.clear();
	Matrix().swap(AdList);
}
void Graph::showAdjacencyList(std::string fname) const
{
	if (AdjacencyMatrix.empty())
	{
		cout << "Adjacency List is empty." << endl;
		return;
	}

	std::ofstream adfile(fname);

	if (adfile.is_open() == false)
		throw std::exception("Can not create\\open file");

	Matrix AdList = buildAdList();

	auto view_vector = [&adfile](const std::vector<short>&v)
	{
		for (short c : v) adfile << " " << c + 1;
		adfile << "}";
	};

	adfile << "Adjacency List:\n";
	for (auto vertex = AdList.cbegin(); vertex != AdList.cend(); vertex++)
	{
		adfile << "{" << vertex - AdList.cbegin() + 1;
		view_vector(*vertex);
	}
	adfile << "\n\n";

	AdList.clear();
	Matrix().swap(AdList);

	adfile.close();
}
void Graph::showWeightedMatrix() const
{
	showMatrix(WeighedMatrix, "Weighted");
}
void Graph::showAdjacencyMatrix() const
{
	showMatrix(AdjacencyMatrix, "Adjacency");
}
void Graph::showMatrix(const Matrix&m, std::string name) const
{
	name = name + " Matrix";

	if (m.empty() == true)
	{
		cout << name << " is empty." << endl;
		return;
	}

	cout << name << ":\n";

	if (name != "Incidence Matrix")
	{
		cout << "   ";
		for (auto i = m.cbegin(); i != m.cend(); i++)
			cout << "[" << (i - m.cbegin()) + 1 << "]";
		cout << endl;
	}

	for (auto row = m.cbegin(); row != m.cend(); row++)
	{
		/*row -> [row->cbegin()][][][][][][row->cend()]*/

		cout << "[" << (row - m.cbegin()) + 1 << "]";
		for (auto column = row->cbegin(); column != row->cend(); column++)
		{
			if ((*column) > -1) cout << " ";

			cout << (*column) << " ";
		}
		cout << endl;
	}
	cout << endl;
}

Matrix Graph::buildIMatrix() const
{
	if (AdjacencyMatrix.empty() == true)
	{
		throw std::exception("Incidence Matrix is empty.");
	}

	size_t vertexAmount = AdjacencyMatrix.size();
	Matrix IM;

	for (int i = 0; i < vertexAmount; i++)
	{
		int j = (isOriented) ? 0 : i;

		for (j; j < vertexAmount; j++)
		{
			unsigned short vertexValue = AdjacencyMatrix[i][j];
			std::vector<short> v(vertexAmount);
			if (vertexValue > 0)
			{
				while (vertexValue)
				{
					if (i != j)
					{
						v[i] = 1, v[j] = (isOriented) ? -1 : 1;
					}
					else if (i == j)	v[i] = 2;

					IM.push_back(v);
					vertexValue -= (i == j) ? 2 : 1;
				}
				v.clear();
				std::vector<short>().swap(v);
			}
		}
	}

	transposeMatrix(IM);

	return IM;
}
Matrix Graph::buildAdList() const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Adjacency List is empty.");

	Matrix AdList;

	auto getVector = [](std::vector<short> vertices, int rowIdx) -> std::vector<short>
	{
		std::vector<short> v;
		for (auto c = vertices.cbegin(); c != vertices.cend(); c++)
		{
			unsigned short vertexVal = *c;
			int idx = c - vertices.cbegin();

			while (vertexVal)
			{
				v.push_back(c - vertices.cbegin());
				vertexVal -= (idx == rowIdx) ? 2 : 1;
			}
		}
		return v;
	};

	for (auto row = AdjacencyMatrix.cbegin(); row != AdjacencyMatrix.cend(); row++)
		AdList.push_back(getVector(*row, row - AdjacencyMatrix.cbegin()));

	return AdList;
}

void Graph::showIncidenceMatrix() const
{
	Matrix IMatrix = buildIMatrix();
	showMatrix(IMatrix, "Incidence");

	IMatrix.clear();
	Matrix().swap(IMatrix);
}

void Graph::DeleteVertex(short vertex)
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	vertex--;
	AdjacencyMatrix.at(vertex);

	AdjacencyMatrix.erase(AdjacencyMatrix.cbegin() + vertex);

	if (isWeighted)
		WeighedMatrix.erase(WeighedMatrix.cbegin() + vertex);
	
	for (int i = 0; i < AdjacencyMatrix.size(); i++)
	{
		AdjacencyMatrix[i].erase(AdjacencyMatrix[i].cbegin() + vertex);

		if (isWeighted)
			WeighedMatrix[i].erase(WeighedMatrix[i].cbegin() + vertex);
	}
}
void Graph::DeleteEdge(short a, short b)
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	a--, b--;
	if (AdjacencyMatrix.at(a).at(b) == 0)
	{
		std::string sEdge = "(" + std::to_string(a + 1) + "," + std::to_string(b + 1) + ")";
		throw std::exception(std::string("Edge " + sEdge + " does not exist").c_str());
	}

	AdjacencyMatrix.at(a).at(b) -=  (a == b) ? 2 : 1;
	if (isWeighted)
		WeighedMatrix.at(a).at(b) = 0;

	if (isOriented == false && a != b)
	{
		AdjacencyMatrix.at(b).at(a)--;
		if (isWeighted)
			WeighedMatrix.at(b).at(a) = 0;
	}
}
void Graph::AddEdge(short a, short b)
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	if (isWeighted)
		throw std::exception("Graph is weighted.");

	a--, b--;
	AdjacencyMatrix.at(a).at(b) += (a == b) ? 2 : 1;
	
	if (isOriented == false && a != b)
		AdjacencyMatrix.at(b).at(a)++;
}

void Graph::AddVertex(std::string newVertex)
{
	/*if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");*/

	std::vector<short>  newVertexVector;

	if (newVertex != "")
	{
		chomp(newVertex);

		boost::regex base("^(?:-?\\d{1,}(?: *|$| *\\n))+$");

		if (boost::regex_match(newVertex, base) == false)
			throw std::exception("Wrong initialization of new vertex");

		newVertexVector = getVertexVector(newVertex);

		if (newVertexVector.size() != AdjacencyMatrix.size() + 1)
			throw std::exception("Wrong initialization of new vertex: Invalid size.");

		for (short v : newVertexVector)
		{
			if (v < 0)
				throw std::exception("Wrong initialization of new vertex: Extra characters.");
		}
	}
	else
	{
		newVertexVector.resize(AdjacencyMatrix.size() + 1);
	}

	AdjacencyMatrix.push_back(newVertexVector);
	if (isWeighted)
		WeighedMatrix.push_back(newVertexVector);

	for (int i = 0; i < AdjacencyMatrix.size(); i++)
	{
		if (isOriented == false)
		{
			AdjacencyMatrix[i].push_back(newVertexVector[i]);
			if (isWeighted)
				WeighedMatrix[i].push_back(newVertexVector[i]);
		}
		else
		{
			AdjacencyMatrix[i].push_back(0);
			if (isWeighted)
				WeighedMatrix[i].push_back(0);
		}
	}
	
	AdjacencyMatrix.back().pop_back();
	if (isWeighted)
		WeighedMatrix.back().pop_back();
}
void Graph::AddVertex(std::vector<short> newVertexVector)
{
	if (isWeighted)
		throw std::exception("Graph is weighted.");


	if (newVertexVector.size() == 1 && newVertexVector[0] == 0)
	{
		newVertexVector.resize(AdjacencyMatrix.size() + 1);
	}
	else if (newVertexVector.size() != AdjacencyMatrix.size() + 1)
	{
		throw std::exception("Wrong initialization of new vertex: Extra characters.");
	}

	for (short v : newVertexVector)
	{
		if (v < 0)
			throw std::exception("Wrong initialization of new vertex: Extra characters.");
	}

	AdjacencyMatrix.push_back(newVertexVector);
	for (int i = 0; i < AdjacencyMatrix.size(); i++)
	{
		if (isOriented == false)
			AdjacencyMatrix[i].push_back(newVertexVector[i]);
		else
			AdjacencyMatrix[i].push_back(0);
	}

	AdjacencyMatrix.back().pop_back();
}

void Graph::getAdditionGraph()
{
	if (isWeighted)
		throw std::exception("Graph is weighted.");


	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	for (int i = 0; i < AdjacencyMatrix.size(); i++)
	{
		for (int j = 0; j < AdjacencyMatrix.size(); j++)
		{
			if (i != j)
			{
				if (AdjacencyMatrix[i][j])
					AdjacencyMatrix[i][j] = 0;
				else
					AdjacencyMatrix[i][j] = 1;
			}
			else if(i == j) AdjacencyMatrix[i][j] = 0;
		}
	}
}

void Graph::splitEdge(short a, short b)
{
	if (isWeighted)
		throw std::exception("Graph is weighted.");

	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	DeleteEdge(a, b);
	AddVertex("");

	AddEdge(a, (short)AdjacencyMatrix.size());
	AddEdge((short)AdjacencyMatrix.size(), b);
}

void Graph::pullOffEdge(short a, short b)
{
	if (isWeighted)
		throw std::exception("Graph is weighted.");

	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	DeleteEdge(a, b);
	vertexIdentification(a, b);
}

void Graph::pullOffGraph(const std::vector<short> &constVector)
{
	if (isWeighted)
		throw std::exception("Graph is weighted.");

	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	if (constVector.empty())
		throw std::exception("Please, specify the vertices");

	std::vector<short> VertexVector = const_cast<std::vector<short>&>(constVector);

	std::sort(VertexVector.begin(), VertexVector.end());
	auto last = std::unique(VertexVector.begin(), VertexVector.end());
	VertexVector.erase(last, VertexVector.end());

	std::vector<short> outCome(AdjacencyMatrix.size() + 1);
	std::vector<short> inCome(AdjacencyMatrix.size() + 1);
	std::priority_queue<short, std::vector<short>> delQ; // Reference to delete vertex

	for (auto vertex = VertexVector.cbegin(); vertex != VertexVector.cend(); vertex++)
	{
		short curVertex = (*vertex);
		curVertex--;

		/*Check range*/
		AdjacencyMatrix.at(curVertex);

		for (int j = 0; j < AdjacencyMatrix.size(); j++)
		{
			outCome[j] = std::max(AdjacencyMatrix[curVertex][j], outCome[j]);
			if (outCome[j] > 0)
				outCome[j] = 1;

			inCome[j] = std::max(AdjacencyMatrix[j][curVertex], inCome[j]);
			if (inCome[j] > 0)
				inCome[j] = 1;
		}

		//Save vertex to delete
		delQ.push(curVertex);
	}

	AdjacencyMatrix.push_back(outCome);

	for (int i = 0; i < AdjacencyMatrix.size(); i++)
		AdjacencyMatrix[i].push_back(inCome[i]);

	AdjacencyMatrix.back().pop_back();

	while (delQ.empty() == false)
	{
		short curV = delQ.top(); delQ.pop();

		// delete row
		AdjacencyMatrix.erase(AdjacencyMatrix.cbegin() + curV);

		//delete column
		for (int i = 0; i < AdjacencyMatrix.size(); i++)
			AdjacencyMatrix[i].erase(AdjacencyMatrix[i].cbegin() + curV);
	}
}

void Graph::vertexIdentification(short a, short b)
{
	if (isWeighted)
		throw std::exception("Graph is weighted.");

	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	if (a == b)
		throw std::exception("The same vertex is specified.");

	a--, b--;
	AdjacencyMatrix.at(a).at(b);

	/*Outcome edges*/
	/*
		 j1        jn
	a -> [][][][][][]
	.
	.
	b -> [][][][][][]
	*/
	std::vector<short> outCome(AdjacencyMatrix.size() + 1);
	std::vector<short> inCome(AdjacencyMatrix.size() + 1);
	for (int j = 0; j < AdjacencyMatrix.size(); j++)
	{
		outCome[j] = std::max(AdjacencyMatrix[a][j], AdjacencyMatrix[b][j]);
		if (outCome[j] > 0) 
				outCome[j] = 1;
	}

	//if (isOriented)
	//{
	//	/*save loop's*/
	//	outCome[AdjacencyMatrix.size()] = AdjacencyMatrix[b][a];
	//}

	if (isOriented == false)
		std::copy(outCome.begin(), outCome.end(), inCome.begin());
	else
	{
		for (int i = 0; i < AdjacencyMatrix.size(); i++)
		{
			inCome[i] = std::max(AdjacencyMatrix[i][a], AdjacencyMatrix[i][b]);
			if (inCome[i] > 0)
					inCome[i] = 1;
		}
	}

	AdjacencyMatrix.push_back(outCome);

	for (int i = 0; i < AdjacencyMatrix.size(); i++)
			AdjacencyMatrix[i].push_back(inCome[i]);

	AdjacencyMatrix.back().pop_back();

	/*After deleting numbers decrease*/
	short firstDelete = std::min(a + 1, b + 1);
	short secondDelete = std::max(a + 1, b + 1);

	DeleteVertex(firstDelete), DeleteVertex(secondDelete - 1);
}

void Graph::vertexDuplicate(short vertex)
{
	if (isWeighted)
		throw std::exception("Graph is weighted.");

	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	vertex--;
	AdjacencyMatrix.at(vertex);

	std::vector<short> newOutCome(AdjacencyMatrix.size() + 1);
	std::vector<short> newInCome(AdjacencyMatrix.size() + 1);

	std::copy(AdjacencyMatrix[vertex].begin(), AdjacencyMatrix[vertex].end(), newOutCome.begin());

	/*copy loops*/
	newOutCome[AdjacencyMatrix.size()] = newOutCome[vertex];
	newOutCome[vertex] = 0;

	if (isOriented == false)
		std::copy(newOutCome.begin(), newOutCome.end(), newInCome.begin());
	else
	{
		for (int i = 0; i < AdjacencyMatrix.size(); i++)
			if (i != vertex) newInCome[i] = AdjacencyMatrix[i][vertex];
	}

	AdjacencyMatrix.push_back(newOutCome);
	for (int i = 0; i < AdjacencyMatrix.size(); i++)
		AdjacencyMatrix[i].push_back(newInCome[i]);

	AdjacencyMatrix.back().pop_back();
}

void Graph::vertexReproduction(short vertex)
{
	if (isWeighted)
		throw std::exception("Graph is weighted.");

	/*with loops*/
	vertexDuplicate(vertex);
	AddEdge(vertex, (short)AdjacencyMatrix.size());
	//AdjacencyMatrix[AdjacencyMatrix.size() - 1][AdjacencyMatrix.size() - 1] = 0;
}

Graph Graph::operator||(const Graph &G2)
{
	if (isWeighted)
		throw std::exception("Graph is weighted.");

	if (G2.AdjacencyMatrix.empty() or AdjacencyMatrix.empty())
		throw std::exception("G1 or G2 is empty");

	if (G2.isOriented != isOriented)
		throw std::exception("Types of graphs are not equal");

	Graph Graph1, Graph2;
	size_t resultSize = std::max(AdjacencyMatrix.size(), G2.AdjacencyMatrix.size());

	Graph1.AdjacencyMatrix = AdjacencyMatrix;
	Graph1.resizeAdMatrix(resultSize);

	Graph2.AdjacencyMatrix = G2.AdjacencyMatrix;
	Graph2.resizeAdMatrix(resultSize);

	Matrix &m1 = Graph1.AdjacencyMatrix;
	Matrix &m2 = Graph2.AdjacencyMatrix;

	for (int i = 0; i < resultSize; i++)
	{
		for (int j = 0; j < resultSize; j++)
		{
			m1[i][j] = std::max(m1[i][j], m2[i][j]);
		}
	}
	Graph1.isOriented = isOriented;
	return Graph1;
}
Graph Graph::operator&(const Graph &G2)
{
	if (isWeighted)
		throw std::exception("Graph is weighted.");

	if (G2.AdjacencyMatrix.empty() or AdjacencyMatrix.empty())
		throw std::exception("G1 or G2 is empty");

	if (G2.isOriented != isOriented)
		throw std::exception("Types of graphs are not equal");

	Graph Graph1, Graph2;
	size_t resultSize = std::min(AdjacencyMatrix.size(), G2.AdjacencyMatrix.size());

	Graph1.AdjacencyMatrix = AdjacencyMatrix;
	Graph1.resizeAdMatrix(resultSize);

	Graph2.AdjacencyMatrix = G2.AdjacencyMatrix;
	Graph2.resizeAdMatrix(resultSize);

	Matrix &m1 = Graph1.AdjacencyMatrix;
	Matrix &m2 = Graph2.AdjacencyMatrix;

	for (int i = 0; i < resultSize; i++)
	{
		for (int j = 0; j < resultSize; j++)
		{
			m1[i][j] = std::min(m1[i][j], m2[i][j]);
		}
	}
	Graph1.isOriented = isOriented;
	return Graph1;
}
Graph Graph::operator*(const Graph &G2)
{
	if (isWeighted)
		throw std::exception("Graph is weighted.");

	if(G2.AdjacencyMatrix.empty() or AdjacencyMatrix.empty())
					throw std::exception("G1 or G2 is empty");

	if (G2.isOriented != isOriented)
		throw std::exception("Types of graphs are not equal");

	Graph Graph1, Graph2;
	Graph1.AdjacencyMatrix = AdjacencyMatrix, Graph2.AdjacencyMatrix = G2.AdjacencyMatrix;

						/*G1  ,  G2*/
	std::deque<std::pair<short, short>> vertexStack;

	/*vertexStack example:
		G1, G2
		{0, 0}
		{0, 1}
		{1, 0}
		{1, 1}
	*/
	for (int i = 0; i < Graph1.AdjacencyMatrix.size(); i++)
	{
		for (int j = 0; j < Graph2.AdjacencyMatrix.size(); j++)
		{
			vertexStack.push_back(std::make_pair(i, j));
		}
	}

	Graph resultGraph;

	resultGraph.resizeAdMatrix(vertexStack.size());

	Matrix& resultAdMatrix = resultGraph.AdjacencyMatrix;

	for (int VertexPairCount = 0; VertexPairCount < vertexStack.size(); VertexPairCount++)
	{
		/*Take current pair*/
		std::pair<short, short> currentPair = vertexStack.at(VertexPairCount);
			
		for (auto Pair = vertexStack.cbegin(); Pair != vertexStack.cend(); Pair++)
		{
			size_t pos = Pair - vertexStack.cbegin();
			bool firstMatch = (Pair->first == currentPair.first);
			bool secMatch = (Pair->second == currentPair.second);
			
			if (firstMatch)
			{
				int i = currentPair.second, j = Pair->second;

				resultAdMatrix[VertexPairCount][pos] += Graph2.AdjacencyMatrix[i][j];
			}
			if (secMatch)
			{
				int i = currentPair.first, j = Pair->first;

				resultAdMatrix[VertexPairCount][pos] += Graph1.AdjacencyMatrix[i][j];
			}
		}
	}

	resultGraph.isOriented = isOriented;
	return resultGraph;
}
Graph Graph::operator+(const Graph &_G2)
{
	if (isWeighted)
		throw std::exception("Graph is weighted.");

	Graph G1 = *this;
	Matrix& G1Matrix = G1.AdjacencyMatrix;
	size_t startSize = G1Matrix.size();

	Graph G2 = const_cast<Graph&>(_G2);
	Matrix& G2Matrix = G2.AdjacencyMatrix;

	if (G2Matrix.empty() or G1Matrix.empty())
		throw std::exception("G1 or G2 is empty");

	for (int i = 0; i < G2Matrix.size(); i++)
	{
		for (int j = 0; j < G1Matrix.size(); j++)
		{
			if (G1Matrix.size() - j - 1 < startSize)
				push_front(G1Matrix[j], 1);
			else 
				push_front(G1Matrix[j], 0);
		}
		
		std::vector<short> horizontal(G1Matrix.size() + 1);

		if (isOriented == false)
			std::fill(horizontal.end() - startSize, horizontal.end(), 1);

		push_front(G1Matrix, horizontal);
	}

	return (G1 || G2);
}

std::vector<short> Graph::getVertexVector(std::string VertexString)
{
	std::vector<short> Vertexes;
	boost::smatch matches;
	boost::regex base_chunk("(-?\\d{1,})(?: *|$|\\n)");

	std::string::const_iterator i(VertexString.cbegin());
	while (boost::regex_search(i, VertexString.cend(), matches, base_chunk))
	{
		i += matches.position() + matches[0].length();
		short newVertexValue;
		try
		{
			newVertexValue = boost::lexical_cast<short>(matches[1]);
			/*if (newVertexValue < -1)
					throw boost::bad_lexical_cast();*/
		}
		catch (const boost::bad_lexical_cast &bdl)
		{
			throw std::exception(bdl.what());
		}

		Vertexes.push_back(newVertexValue);
	}

	return Vertexes;
}
std::list<std::string> Graph::getRowList(std::ifstream &initfile)
{
	std::string VertexString;
	std::list<std::string> lRows;

	bool firstLine = true;
	unsigned Columns = 2; //must be more, than 1
	unsigned Rows = 0;

	while (initfile.good())
	{
		std::getline(initfile, VertexString);
		boost::regex base("^(?: *-?\\d{1,}(?: *|$| *\\n))+$");

		bool match_res = boost::regex_match(VertexString, base);

		/*Check the vertex line to base form*/
		if (match_res == false)
		{
			std::string err_string = "Invalid expression: \"" + VertexString + "\"";
			throw std::exception(err_string.c_str());
		}

		chomp(VertexString);

		size_t vertexAmount = getVertexVector(VertexString).size();

		/*Amount of vertexes must be more then two.*/
		if (firstLine && vertexAmount > 0)
		{
			Columns = (unsigned)vertexAmount;
			firstLine = false;
		}
		else if (Columns != vertexAmount)
			throw std::exception("Matrix is incorrect.");

		lRows.push_back(VertexString);
	}

	return lRows;
}

void Graph::readGraph(std::string filename)
{
	auto readAttribute = [](std::ifstream &initfile) -> std::vector<std::string>
	{
		std::vector<std::string> tokens;
		boost::regex base_attr("^(AM|AL|IM)(?: )+(oriented|not oriented)(?: +(w))* *$");
		boost::smatch matches;
		std::string attr;
		
		std::getline(initfile, attr);
		if (boost::regex_match(attr, matches, base_attr) == false)
		{
			initfile.close();
			throw std::exception("Wrong attribute. Please, specify the init form of the graph.\n"
								"Correct attributes: AM -> Adjacency Matrix\n"
								"                    AL -> Adjacency List\n"
								"                    IM -> Icidence Matrix\n"
								"                    w  -> Weighted Matrix\n");
		}
		else if (matches[3].str() == "w" && matches[1].str() != "AM")
		{
			initfile.close();
			throw std::exception("Weighted graph must be initialized in Adjacency Matrix\n");
		}

		tokens.push_back(matches[1].str());
		tokens.push_back(matches[2].str());
		tokens.push_back(matches[3].str());
		return tokens;
	};

	std::ifstream initfile(filename, std::ifstream::in);

	try
	{
		if (initfile.is_open() == false)
			throw std::exception("Could not open file.");

		std::vector<std::string> attributes = readAttribute(initfile);
		std::string attr = attributes[0];

		if (attributes[1] == "oriented") isOriented = true;
		if (attributes[2] == "w") isWeighted = true;


		AdjacencyMatrix.clear();
		WeighedMatrix.clear();

		Matrix().swap(WeighedMatrix);
		Matrix().swap(AdjacencyMatrix);

		if (attr == "AM")
		{
			readAdMatrix(initfile);

			WeighedMatrix = AdjacencyMatrix;

			if (isWeighted)
			{
				/*
					если граф взвешенный, в оригинальной матрице смежности 
					отмечаем местоположение вершин
				*/
				for (int i = 0; i < AdjacencyMatrix.size(); i++)
				{
					for (int j = 0; j < AdjacencyMatrix.size(); j++)
					{
						if (AdjacencyMatrix[i][j] != 0)
							AdjacencyMatrix[i][j] = 1;
					}
				}
			}
			else
			{
				/*
					если граф не взвешенный, то в весовой матрице все весы равны единице
				*/
				for (int i = 0; i < WeighedMatrix.size(); i++)
				{
					for (int j = 0; j < WeighedMatrix.size(); j++)
					{
						if (WeighedMatrix[i][j] != 0)
							WeighedMatrix[i][j] = 1;
					}
				}
			}
		}
		else if (attr == "AL")
			readAdList(initfile);
		else if (attr == "IM")
			readIMatrix(initfile);
	}
	catch (const std::exception &e)
	{
		initfile.close();
		AdjacencyMatrix.clear();
		Matrix().swap(AdjacencyMatrix);

		throw std::exception(e);
	}

	initfile.close();
}

void Graph::readAdMatrix(std::ifstream &initfile)
{
	std::list<std::string> lRows = getRowList(initfile);

	unsigned Rows = 0;
	for (std::string row : lRows)
	{
		AdjacencyMatrix.resize(Rows + 1);
		AdjacencyMatrix[Rows] = getVertexVector(row);
		Rows++;
	}

	if (AdjacencyMatrix.size() != AdjacencyMatrix[0].size())
	{
		throw std::exception("Adjacency Matrix is incrorrect.");
	}

	checkAdNotOrientedMatrix();
}
void Graph::readAdList(std::ifstream &initfile)
{
	boost::regex base("^(?:{\\d{1,}(?: *\\d{1,})*}){1,}(?:$|\\n)");
	
	std::string str;
	std::getline(initfile, str);

	if (boost::regex_match(str, base) == false)
		throw std::exception("Anjacency List is incorrect.");

	chomp(str);

	std::vector<short> Vertexes;
	boost::smatch matches;
	boost::regex base_chunk("{(\\d{1,})(?: *\\d{1,})*}");
	unsigned prevRow = 0, curRow;

	auto parse_chunk = [](std::string chunk, size_t rowSize, int rowIdx) -> std::vector<short>
	{
		std::vector<short> vRow(rowSize);
		boost::regex base(" (\\d{1,})");
		boost::smatch matches;
		std::string::const_iterator chunk_iter(chunk.cbegin());
		
		while (boost::regex_search(chunk_iter, chunk.cend(), matches, base))
		{
			chunk_iter += matches.position() + matches[0].length();
			short columnNumber = boost::lexical_cast<short>(matches[1].str()) - 1;
			
			//if loop add + 2
			vRow.at(columnNumber) += (rowIdx == columnNumber) ? 2 : 1;
		}

		return vRow;
	};

	std::string::const_iterator i(str.cbegin());
	std::list<std::string> rows;
	while (boost::regex_search(i, str.cend(), matches, base_chunk))
	{
		i += matches[0].length();
		curRow = boost::lexical_cast<unsigned>(matches[1].str());

		if (curRow != prevRow + 1)
			throw std::exception("Anjacency List is incorrect.");

		prevRow++;
		rows.push_back(matches[0].str());
	}

	int RowIdx = 0;
	for (std::string row : rows)
	{
		AdjacencyMatrix.resize(RowIdx + 1);
		AdjacencyMatrix[RowIdx] = parse_chunk(row, rows.size(), RowIdx);
		RowIdx++;
	};

	checkAdNotOrientedMatrix();
}
void Graph::readIMatrix(std::ifstream &initfile)
{
	Matrix templateMatrix;
	std::list<std::string> lRows = getRowList(initfile);
	
	unsigned Rows = 0;
	for (std::string row : lRows)
	{
		templateMatrix.resize(Rows + 1);
		templateMatrix[Rows] = getVertexVector(row);

		AdjacencyMatrix.resize(Rows + 1);
		AdjacencyMatrix[Rows].resize(lRows.size());
		Rows++;
	}

	transposeMatrix(templateMatrix);

	unsigned vAmount;
	int columnIdx;
	unsigned rowIdx;
	for (auto row = templateMatrix.cbegin(); row != templateMatrix.cend(); row++)
	{
		vAmount = 0, columnIdx = -1;

		for (auto v = row->cbegin(); v != row->cend(); v++)
		{
			if (*v > 2 || *v < -1)
				throw std::exception("Incidence matrix is incrorrect.");

			if (*v && vAmount == 2 || *v == -1 && isOriented == false)
				throw std::exception("Incidence matrix is incrorrect.");

			unsigned distance = v - row->cbegin();
			if (!vAmount)
			{
				switch (*v)
				{
				case 1:
					rowIdx = distance;
					vAmount++;
					break;
				case 2:
					/*loop*/
					AdjacencyMatrix[distance][distance] += 2;
					vAmount += 2;
					break;
				case -1:
					columnIdx = distance;
					vAmount++;
					break;
				default:
					break;
				}
			}
			else if (vAmount)
			{
				switch (*v)
				{
				case 1:
					if (columnIdx > -1)
						AdjacencyMatrix[distance][columnIdx] += 1;
					else if (!isOriented)
					{
						AdjacencyMatrix[rowIdx][distance] += 1;
						AdjacencyMatrix[distance][rowIdx] += 1;
					}
					else 
						throw std::exception("Incidence matrix is incrorrect.");
					vAmount++;
					break;
				case -1:
					if (columnIdx < 0)
						AdjacencyMatrix[rowIdx][distance] += 1;
					else
						throw std::exception("Incidence matrix is incorrect.");
					vAmount++;
					break;
				default:
					break;
				}
			}
		}
		if (vAmount != 2)
			throw std::exception("Incidence matrix is incorrect.");
	}

	checkAdNotOrientedMatrix();
}

bool Graph::checkAdNotOrientedMatrix()
{
	if (AdjacencyMatrix.empty())
		throw std::exception("AdMatrix is empty.");

	if (AdjacencyMatrix.size() != AdjacencyMatrix[0].size())
		throw std::exception("Graph initializaion is incrorrect.");

	for (int i = 0; i < AdjacencyMatrix.size(); i++)
	{
		for (int j = 0; j < AdjacencyMatrix[0].size(); j++)
		{
			if (!isWeighted && AdjacencyMatrix[i][j] < 0)
				throw std::exception("Graph initializaion is incrorrect.");

			if (isOriented == false &&
				AdjacencyMatrix[i][j] != AdjacencyMatrix[j][i])
				throw std::exception("Graph initialization is incrorrect. Matrix is not symmetrical.");

			if (i == j and (AdjacencyMatrix[i][j] % 2))
				throw std::exception("Graph initialization is incrorrect. Invalid loops.");
		}
	}
	return true;
}


std::vector<short> Graph::con_components()
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	using std::deque;
	using std::vector;
	using std::pair;
	using std::map;


	/*Build adjacency list*/
	Matrix AdList = buildAdList();

	vector<bool>				passed(AdjacencyMatrix.size(), false);
	vector<pair<int, short>>	parents(AdjacencyMatrix.size());
	std::fill(parents.begin(), parents.end(), std::make_pair(INT_MAX, -1));

	deque<short> nextVertices;

	short v = 0;

	/*вершины в разных компонентах связности*/
	std::vector<short> components;

	for (short v = 0; v < AdjacencyMatrix.size(); v++)
	{
		if (parents[v].second < 0)
		{
			components.push_back(v);

			passed[v] = true;
			parents[v].second = v;
			parents[v].first = 0;

			nextVertices.push_back(v);

			while (nextVertices.empty() == false)
			{
				/*Current vertex*/
				short curVertex = nextVertices.back(); nextVertices.pop_back();

				vector<short> AdListRow = AdList.at(curVertex);
				for (auto adIt = AdListRow.cbegin(); adIt != AdListRow.cend(); adIt++)
				{
					short curAdVertex = *adIt;
					int weight_vu = WeighedMatrix[curVertex][curAdVertex];

					if (passed[curAdVertex] == false)
					{
						passed[curAdVertex] = true;
						parents[curAdVertex].second = curVertex;
						parents[curAdVertex].first = weight_vu;

						nextVertices.push_back(curAdVertex);
					}
				}
			}
		}
	}

	return components;
}

void Graph::showBfs()
{
	std::vector<short> components = con_components();

	for (auto c : components)
	{
		bfs(c);
	}
}

void Graph::bfs(short v)
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	using std::vector;
	using std::pair;
	using std::map;
	using std::deque;


	/*Build adjacency list*/
	Matrix AdList = buildAdList();

	vector<bool>	passed(AdjacencyMatrix.size(), false);
	vector<pair<int, short>> parents(AdjacencyMatrix.size());
	std::fill(parents.begin(), parents.end(), std::make_pair(INT_MAX, -1));

	deque<short> nextVertices;

	passed[v] = true;
	parents[v].second = v;
	parents[v].first = 0;

	nextVertices.push_back(v);

	std::ofstream bfs_log("bfs_log.txt", std::ofstream::app);
	std::stringstream slog;

	while (nextVertices.empty() == false)
	{
		/*Current vertex*/
		short curVertex = nextVertices.front(); nextVertices.pop_front();

		slog << "Teкущая вершина: [" << curVertex + 1 << "]\n";
		slog << "Предок: [" << parents[curVertex].second + 1 << "]\n";
		slog << "Расстояние до предка: " << parents[curVertex].first << "\n";

		slog << "Смежные вершины: " << endl;
		vector<short> AdListRow = AdList.at(curVertex);
		for (auto adIt = AdListRow.cbegin(); adIt != AdListRow.cend(); adIt++)
		{
			short curAdVertex = *adIt;
			int weight_vu = WeighedMatrix[curVertex][curAdVertex];

			slog << "# Вершина: [" << curAdVertex + 1 << "]";
			slog << ", Вес (" << curVertex + 1<< ", " << curAdVertex + 1 << ") = " << weight_vu;

			if (passed[curAdVertex] == false)
			{
				passed[curAdVertex] = true;
				parents[curAdVertex].second = curVertex;
				parents[curAdVertex].first = weight_vu;

				nextVertices.push_back(curAdVertex);

				slog << " +" << endl;
			}
			else slog << " -" << endl;
		}

		slog << "\n";
	}

	Matrix result = carcass_to_matrix(parents);

	int sum = 0;
	for (const std::pair<int, short> &p : parents)
		sum += p.first;

	slog << "Общий вес полученного остова: " << sum;
	showMatrix(result, "Bfs");

	bfs_log << slog.str();
	bfs_log.close();
}

void Graph::algorithmPrima()
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	using std::vector;
	using std::pair;
	using std::map;
	using std::priority_queue;

	/*Build adjacency list*/
	Matrix AdList = buildAdList();
	/*
		int   - Расстояние от i-ой вершины до построенного дерева.
		short - Предок i-ой вершины. 
	*/
	vector<pair<int, short>> distances(AdList.size());
	std::fill(distances.begin(), distances.end(), std::make_pair(INT_MAX, -1));
	/*
		Приоритетная очередь вершин графа, где ключ — distances[i]
	*/
	priority_queue<pair<int, short>, vector<pair<int, short>>, std::greater<pair<int, short>>> Q;
	/*
		Множество ребер минимального остовного дерева
	*/			
	vector<bool> used(AdList.size(), false);

	distances[0] = std::make_pair(0, 0);
	used[0] = true;

	Q.push(std::make_pair(0, 0));

	std::ofstream log_prima("alg_prima.txt", std::ofstream::app);
	std::stringstream slog;

	while (Q.empty() == false)
	{
		pair<int, short> v = Q.top(); Q.pop();
		used[v.second] = true;
		slog << "Teкущая вершина: [" << v.second + 1 << "]\n";
		slog << "Предок: [" << distances[v.second].second + 1 << "]\n";
		slog << "Расстояние до предка: " << distances[v.second].first << "\n";

		/*
			Для каждой вершины u смежной с v
		*/
		slog << "Смежные вершины: " << endl;
		vector<short> AdListRow = AdList.at(v.second);
		for (auto AdVertex = AdListRow.cbegin(); AdVertex != AdListRow.cend(); AdVertex++)
		{
			short u = *AdVertex;
			int weight_vu = WeighedMatrix[v.second][u];

			slog << "# Вершина: [" << u + 1 << "], Вес (" << v.second + 1<< "," << u + 1 << ") = " << weight_vu;
			slog << " , Приоритет = " << ((distances[u].first == INT_MAX) ? "inf" : std::to_string(distances[u].first));
			slog << ", Предок: [" << ((distances[u].second == -1) ? "nil" : std::to_string(distances[u].second + 1)) << "]";

			if (used[u] == false && weight_vu < distances[u].first)
			{
				distances[u].first = weight_vu;
				distances[u].second = v.second;

				Q.push(std::make_pair(distances[u].first, u));
				
				slog << " +" << endl;
			}
			else slog << " -" << endl;
		}

		slog << "\n";
	}

	Matrix result = carcass_to_matrix(distances);

	int sum = 0;
	for (const std::pair<int, short> &p : distances)
		sum += p.first;

	slog << "Общий вес полученного остова: " << sum;
	showMatrix(result, "Algorithm Prima");

	log_prima << slog.str();
	log_prima.close();
}

Matrix Graph::carcass_to_matrix(const std::vector<std::pair<int, short>>& dist)
{
	Matrix w_matrix;
	resizeAdMatrix(w_matrix, dist.size());

	for (int i = 0; i < dist.size(); i++)
	{ 
		if (i == dist[i].second || dist[i].second < 0) continue;

		w_matrix[i][dist[i].second] = dist[i].first;
		if (isOriented == false)
		{
			w_matrix[dist[i].second][i] = dist[i].first;
		}
	}

	return w_matrix;
}

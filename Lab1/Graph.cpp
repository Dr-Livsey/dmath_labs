#include "Graph.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <deque>
#include <queue>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/bind.hpp>

#define or ||
#define and &&
#define AllPaths -1

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

Graph::Graph()
{
	isOriented = false;
	readGraph("init_graph.txt");

	try
	{
		showAdjacencyMatrix();
		splitEdge(1, 5);
		showAdjacencyMatrix();
	}
	catch (const std::out_of_range &)
	{
		cout << "Exception: Vertex or vertices does not belong to this graph." << endl;
	}
	catch (const std::exception &ex)
	{
		cout << "Exception: " << ex.what() << endl;
	}
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
		AdjacencyMatrix.at(a);

		if (b != AllPaths)
		{
			b--;
			AdjacencyMatrix.at(a);
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
			throw std::exception("Vertices are not located in a single connected component");
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

	std::pair<unsigned, short> radius = getDiameter();
	cout << "Diameter is " << radius.first << endl;
}
void Graph::showCenter() const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	Paths centers = getCenter();

	cout << "Center(s): ";
	if (centers.empty()) cout << "empty";
	for (std::pair<unsigned, short> curCenter : centers)
	{
		cout << "[" << curCenter.second << "] ";
	}
	cout << endl;
}
void Graph::showPeripheralVertex() const
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	Paths periferals = getPeriferalVertexes();

	cout << "Periferal Vertices: ";

	if (periferals.empty()) cout << "empty";

	for (std::pair<unsigned, short> curPeriferal : periferals)
	{
		cout << "[" << curPeriferal.second << "] ";
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

	unsigned inDegree = 0;
	for (std::vector<short> vVector : AdjacencyMatrix)
	{
		inDegree += vVector.at(vertex);
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
			/*If *vert is loop*/
			if (isor == false && (vert - v.cbegin()) == vertex)
				s = s + vertexValue * 2;
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

	//vertex starts from 1.
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

std::vector<short> Graph::mergeRows
					(
						const std::vector<short>&r1, 
						const std::vector<short>&r2, 
						std::function<short(short, short)> f
					) const
{
	if (r1.size() != r2.size())
		throw std::exception("Invalid row(s) - wrong size");

	std::vector<short> resultVector(r1.size());

	for (int i = 0; i < r1.size(); i++)
	{
		resultVector[i] = f(r1[i], r2[i]);
	}

	return resultVector;
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
			unsigned newDistance = distances.at(curVertex.second).first + 1;

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

	if (isConnectedGraph(paths) == false)
		throw std::exception("The graph is not connected");

	Paths::iterator maxElement = std::max_element(paths.begin(), paths.end());

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

	Paths::iterator minElement = std::min_element(eVector.begin(), eVector.end());
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

Paths Graph::getCenter() const
{
	unsigned radius = getRadius().first;
	Paths centers;

	for (int i = 0; i < AdjacencyMatrix.size(); i++)
	{
		std::pair<unsigned, short> currentEccentricity = getEccentricity(i);

		if (currentEccentricity.first == radius)
			centers.push_back(currentEccentricity);	
	}

	return centers;
}
Paths Graph::getPeriferalVertexes() const
{
	unsigned diameter = getDiameter().first;
	Paths periferals;

	for (int i = 0; i < AdjacencyMatrix.size(); i++)
	{
		std::pair<unsigned, short> currentEccentricity = getEccentricity(i);

		if (currentEccentricity.first == diameter)
			periferals.push_back(currentEccentricity);
	}

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

void Graph::showDegSequance() const
{
	std::vector<size_t> dVector = getDegSequence();
	cout << "Sequance of degrees:";
	for (size_t deg : dVector) cout << " " << deg << " ";
	cout << endl;
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
		int j = i;
		if (isOriented == true) j = 0;

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
						v[i] = 1, v[j] = 1;
						if (isOriented)
							v[j] = -1;
					}
					else if (i == j)
						v[i] = 2;

					IM.push_back(v);
					vertexValue--;
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

	auto getVector = [](std::vector<short> vertices) -> std::vector<short>
	{
		std::vector<short> v;
		for (auto c = vertices.cbegin(); c != vertices.cend(); c++)
		{
			unsigned short vertexVal = *c;
			while (vertexVal)
			{
				v.push_back(c - vertices.cbegin());
				vertexVal--;
			}
		}
		return v;
	};

	for (auto row = AdjacencyMatrix.cbegin(); row != AdjacencyMatrix.cend(); row++)
		AdList.push_back(getVector(*row));

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
	
	for (int i = 0; i < AdjacencyMatrix.size(); i++)
	{
		AdjacencyMatrix[i].erase(AdjacencyMatrix[i].cbegin() + vertex);
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

	AdjacencyMatrix.at(a).at(b)--;

	if (isOriented == false && a != b)
		AdjacencyMatrix.at(b).at(a)--;
}
void Graph::AddEdge(short a, short b)
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	a--, b--;
	AdjacencyMatrix.at(a).at(b)++;

	if (isOriented == false && a != b)
		AdjacencyMatrix.at(b).at(a)++;
}
void Graph::AddVertex(std::string newVertex)
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

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
	else newVertexVector.resize(AdjacencyMatrix.size() + 1);

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
		}
	}
}

void Graph::splitEdge(short a, short b)
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	if (a == b)
		throw std::exception("This edge is loop");

	DeleteEdge(a, b);
	AddVertex("");

	AddEdge(a, (short)AdjacencyMatrix.size());
	AddEdge((short)AdjacencyMatrix.size(), b);
}

void Graph::pullOffEdge(short a, short b)
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	if (a == b)
		throw std::exception("This edge is loop");

	DeleteEdge(a, b);

	/**/
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
			if (newVertexValue < -1)
					throw boost::bad_lexical_cast();
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
		boost::regex base("^(?:-?\\d{1,}(?: *|$| *\\n))+$");

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
		if (firstLine && vertexAmount > 1)
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
	std::ifstream initfile(filename, std::ifstream::in);

	if (initfile.is_open() == false)
		throw std::exception("Could not open file.");

	auto readAttribute = [](std::ifstream &initfile) -> std::vector<std::string>
	{
		std::vector<std::string> tokens;
		boost::regex base_attr("^(AM|AL|IM)(?: )+(oriented|not oriented)$");
		boost::smatch matches;
		std::string attr;
		
		std::getline(initfile, attr);
		if (boost::regex_match(attr, matches, base_attr) == false)
		{
			initfile.close();
			throw std::exception("Wrong attribute. Please, specify the init form of the graph.\n"
								"Correct attributes: AM -> Adjacency Matrix\n"
								"		    AL -> Adjacency List\n"
								"		    IM -> Icidence Matrix\n");
		}

		tokens.push_back(matches[1].str());
		tokens.push_back(matches[2].str());
		return tokens;
	};

	std::vector<std::string> attributes = readAttribute(initfile);
	std::string attr = attributes[0];

	if (attributes[1] == "oriented") isOriented = true;

	/*read graph and convert it to AdMatrix.*/
	AdjacencyMatrix.clear();
	Matrix().swap(AdjacencyMatrix);

	try
	{
		if (attr == "AM")
			readAdMatrix(initfile);
		else if (attr == "AL")
			readAdList(initfile);
		else if (attr == "IM")
			readIMatrix(initfile);
	}
	catch (const std::exception &e)
	{
		AdjacencyMatrix.clear();
		Matrix().swap(AdjacencyMatrix);
		cout << e.what() << endl;
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
	boost::regex base("^(?:{\\d{1,}(?: *\\d{1,})*}){2,}(?:$|\\n)");
	
	std::string str;
	std::getline(initfile, str);

	if (boost::regex_match(str, base) == false)
		throw std::exception("Anjacency List is incorrect.");

	chomp(str);

	std::vector<short> Vertexes;
	boost::smatch matches;
	boost::regex base_chunk("{(\\d{1,})(?: *\\d{1,})*}");
	unsigned prevRow = 0, curRow;

	auto parse_chunk = [](std::string chunk, size_t rowSize) 
						-> std::vector<short>
	{
		std::vector<short> vRow(rowSize);
		boost::regex base(" (\\d{1,})");
		boost::smatch matches;
		std::string::const_iterator chunk_iter(chunk.cbegin());
		
		while (boost::regex_search(chunk_iter, chunk.cend(), matches, base))
		{
			chunk_iter += matches.position() + matches[0].length();
			short columnNumber = boost::lexical_cast<short>
											(
												matches[1].str()
						
											) - 1;
			vRow.at(columnNumber) += 1;
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
		AdjacencyMatrix[RowIdx] = parse_chunk(row, rows.size());
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
					AdjacencyMatrix[distance][distance] += 1;
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

	for (int i = 0; i < AdjacencyMatrix.size(); i++)
	{
		for (int j = 0; j < AdjacencyMatrix[0].size(); j++)
		{
			if (AdjacencyMatrix[i][j] < 0)
				throw std::exception("Graph initializaion is incrorrect.");

			if (isOriented == false &&
				AdjacencyMatrix[i][j] != AdjacencyMatrix[j][i])
				throw std::exception("Graph initialization is incrorrect.");
		}
	}
	return true;
}

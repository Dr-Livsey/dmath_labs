#pragma once
#include <string>
#include <map>
#include <functional>
#include <deque>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/system/error_code.hpp>

class Graph;

class Interface
{
public:
	void operator()(void);

private:

	void show(int,std::string);
	void get(int, std::string);
	void add(int, std::string);
	void delete_(int, std::string);

	void create_graph(const char *);
	void create_graph(const char *, const char *);
	void delete_graph(const char *);
	void copy_graph(const char *, const char *);

	void parse_command(std::string);

	void print_cmd(int);
	void print_show();
	void print_get();
	void print_add();
	void print_delete();
	void print_graphs();
	void help();

	std::pair<short, short> get_edge(const std::string&);
	Graph*					get_graph(std::string&);
	std::deque<std::string> get_tokens(std::string, boost::regex);
	void					get_function(Graph *, int, const std::deque<std::string>&);

	std::map<std::string, std::unique_ptr<Graph>> data_base;
};


// определение числа вершин;
#define _vertexAmount			0
// определение числа ребер(дуг);
#define _edgeAmount				1
// определение степени произвольной вершины
#define _showVertexDegree		2
// определение степенной последовательности графа;
#define _showDegSequence		3
// определение матрицы смежности;
#define _showAdjacencyMatrix	4
// определение матрицы инцидентности;
#define _showIncidenceMatrix	5
// определение списка смежности;
#define _showAdjacencyList		6
// определение висячих вершин
#define _showLeafs				7
//определение изолированных вершин;
#define _showIsolatedVertices	8
// определение в орграфе истоков 
#define _showSources			9
// определение в орграфе стоков;
#define _showStocks				10
// определение расстояния между двумя вершинами;
#define _showDistance			11
// определение эксцентриситета вершины;
#define _showEx			        12
// определение диаметра графа;
#define _showDiameter			13
// определение радиуса графа;
#define _showRadius				14
// определение центра графа;
#define _showCenter				15
// определение периферийных вершин;
#define _showPeriferalVertices	16
// добавление вершины в граф;
#define _AddVertex				17
// удаление вершины в графе;
#define _DeleteVertex			18
// добавление в граф;
#define _AddEdge				19
// удаление ребра в графе;
#define _DeleteEdge				20
// определение дополнения графа;
#define _getAdditionalGraph		21
// подразбиение ребра;
#define _splitEdge				22
// стягивание графа;
#define _pullOfGraph			23
// стягивание ребра;
#define _pullOfEdge				24
// отождествление вершин;
#define _vertexIdentification	25
// дублирование вершины;
#define _vertexDuplicate		26
// размножение вершины;
#define _vertexReproduction		27
// объединение(дизъюнктивное) графов;
#define _or						28
// соединение графов;
#define _plus					29
// произведение графов;
#define _multiply				30
// вывод графов в текстовый файл в виде списка смежности
#define _showAdListFile			31
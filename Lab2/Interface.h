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


// ����������� ����� ������;
#define _vertexAmount			0
// ����������� ����� �����(���);
#define _edgeAmount				1
// ����������� ������� ������������ �������
#define _showVertexDegree		2
// ����������� ��������� ������������������ �����;
#define _showDegSequence		3
// ����������� ������� ���������;
#define _showAdjacencyMatrix	4
// ����������� ������� �������������;
#define _showIncidenceMatrix	5
// ����������� ������ ���������;
#define _showAdjacencyList		6
// ����������� ������� ������
#define _showLeafs				7
//����������� ������������� ������;
#define _showIsolatedVertices	8
// ����������� � ������� ������� 
#define _showSources			9
// ����������� � ������� ������;
#define _showStocks				10
// ����������� ���������� ����� ����� ���������;
#define _showDistance			11
// ����������� ��������������� �������;
#define _showEx			        12
// ����������� �������� �����;
#define _showDiameter			13
// ����������� ������� �����;
#define _showRadius				14
// ����������� ������ �����;
#define _showCenter				15
// ����������� ������������ ������;
#define _showPeriferalVertices	16
// ���������� ������� � ����;
#define _AddVertex				17
// �������� ������� � �����;
#define _DeleteVertex			18
// ���������� � ����;
#define _AddEdge				19
// �������� ����� � �����;
#define _DeleteEdge				20
// ����������� ���������� �����;
#define _getAdditionalGraph		21
// ������������ �����;
#define _splitEdge				22
// ���������� �����;
#define _pullOfGraph			23
// ���������� �����;
#define _pullOfEdge				24
// �������������� ������;
#define _vertexIdentification	25
// ������������ �������;
#define _vertexDuplicate		26
// ����������� �������;
#define _vertexReproduction		27
// �����������(�������������) ������;
#define _or						28
// ���������� ������;
#define _plus					29
// ������������ ������;
#define _multiply				30
// ����� ������ � ��������� ���� � ���� ������ ���������
#define _showAdListFile			31
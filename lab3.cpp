#include "Graph.h"
#include <iostream>

#define inf SHRT_MAX

using std::cout;
using std::endl;

struct edge
{
	edge(short s, short d, short w) :
		src(s), dst(d), weigth(w) {}
	short src;
	short dst;
	int weigth;
};

void showDistMatrix(std::ofstream &os, Matrix &dist);
void showParentsMatrix(std::ofstream &os, Matrix &parents, Matrix &dist);
void showFloydPaths(std::ofstream &os, const Matrix &dist, const Matrix &parents, short vi, short vj);

void showPath(std::ofstream &os, const std::vector<std::pair<int, short>> &distances, short v);

std::vector<edge> Graph::get_edges()
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	std::vector<edge> edges;

	for (int i = 0; i < AdjacencyMatrix.size(); i++)
	{
		for (int j = 0; j < AdjacencyMatrix.size(); j++)
		{
			if (AdjacencyMatrix[i][j])
			{
				edge e(i, j, WeighedMatrix[i][j]);
				edges.push_back(e);
			}
		}
	}

	return edges;
}

void Graph::ford_bellman(short v1)
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	using std::vector;
	using std::pair;

	v1--;

	/*check range*/
	AdjacencyMatrix.at(v1);

	std::vector<edge> edges = get_edges();

	/*Build adjacency list*/
	Matrix AdList = buildAdList();

				/*dist, parent*/
	vector<pair<int, short>> distances(AdList.size());
	std::fill(distances.begin(), distances.end(), std::make_pair(inf, -1));

	distances[v1] = std::make_pair(0, v1);

	/*logging*/
	std::ofstream log("ford-bellman.txt");

	//log << "              ";
	//for (int log_i = 0; log_i < AdjacencyMatrix.size(); log_i++)
	//	log  << std::setw(6) <<  "D[" + std::to_string(log_i + 1) + "]";
	//log << endl;
	/*******/

	int iterations = 0;
	short check_cycle;
	for (int k = 1; k < AdjacencyMatrix.size(); k++, iterations++)
	{
		check_cycle = -1;
		log << "Итерация: " << std::setw(2) << iterations + 1 << ":" << endl;
		std::string d_str = "# D(" + std::to_string(k + 1) + ")";
		for (int j = 0; j < edges.size(); j++)
		{
			short src = edges[j].src;
			short dst = edges[j].dst;
			int weight = edges[j].weigth;

			std::string j_str = d_str + "[" + std::to_string(dst + 1) + "]\t";

			log << j_str << " = min{ D(" << k << ")[" << dst + 1 << "], ";
			log << "D(" << k << ")[" << src + 1 << "] + ";
			log << "Weight(" << src + 1 << ", " << dst + 1 << ") } = ";

			if (distances[src].first != inf &&
				distances[dst].first > distances[src].first + weight)
			{
				distances[dst].second = src;
				distances[dst].first = distances[src].first + weight;
				check_cycle = dst;

				log << std::to_string(distances[src].first) + " + " + std::to_string(weight) + "\t[+]\n";
			}
			else log << std::to_string(distances[dst].first) + "\t[-]\n";
		}

		log << endl;

		/*logging*/
		//for (int log_i = 0; log_i < AdjacencyMatrix.size(); log_i++)
		//	log << ((distances[log_i].first == inf) ? "i" : std::to_string(distances[log_i].first)) << std::setw(6);
		//log << endl;
		/********/
	}

	/*В графе есть отрицательный цикл*/
	if (check_cycle != -1)
	{
		short v = check_cycle;
		log << "\nОтрицательный цикл: ";
		log << "[" << v + 1 << "]";
		while (true)
		{
			v = distances[v].second;
			log << " <-- [" << v + 1 << "]";

			if (v == check_cycle) 
				break;
		}
	}
	else
	{
		log << "\n\n";
		for (int i = 0; i < distances.size(); i++)
		{
			log << "Вес: " << std::setw(5) << ((distances[i].first == inf) ? "inf" : std::to_string(distances[i].first));
			log << "\tПуть: ", showPath(log, distances, i), log << endl;;
		}
	}

	log.close();
}

void Graph::floyd(short v1, short v2)
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	using std::vector;
	using std::pair;

	v1--; v2--;

	size_t n = AdjacencyMatrix.size();
	/*
		parents[i][j] = номеру вершины,
		являющейся предпоследней в кратчайшем (vi, vj) пути
	*/
	Matrix parents;	resizeMatrix(parents, n);
	/*
		расстояния для всех пар vi, vj
	*/
	Matrix distances(WeighedMatrix);

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (i != j)
			{
				if (!AdjacencyMatrix[i][j])
					distances[i][j] = inf;

				parents[i][j] = i;
			}
		}
	}
	
	/*logging*/
	std::ofstream log("floyd.txt");
	/*******/
	int iterations = 0;
	for (int k = 0; k < n; k++)
	{
		log << "\n" << endl;
		showDistMatrix(log, distances);

		std::string d_str = "# D(" + std::to_string(k + 1) + ")";
		for (int i = 0; i < n; i++)
		{
			std::string i_str = d_str + "[" + std::to_string(i + 1) + ", ";
			for (int j = 0; j < n; j++)
			{
				log << i_str;
				log << j + 1 << "]\t = min{ D(" << k << ")[" << i + 1 << ", " << j + 1 << "], ";
				log << "D(" << k << ")[" << i + 1 << ", " << k + 1 << "] + ";
				log << "D(" << k << ")[" << k + 1 << ", " << j + 1 << "] }";

				if (distances[i][k] < inf && distances[k][j] < inf)
				{
					if (distances[i][j] > distances[i][k] + distances[k][j])
					{
						distances[i][j] = distances[i][k] + distances[k][j];
						parents[i][j] = parents[k][j];
						continue;
					}
				}

				log << " = " << ((distances[i][j] == inf) ? "inf" : std::to_string(distances[i][j])) << "\t[-]\n";
			}
			log << endl;
		}
	}

	log << "\nRESULT: \n";
	showDistMatrix(log, distances);
	showParentsMatrix(log, parents, distances);
	showFloydPaths(log, distances, parents, v1, v2);

	//for (int i = 0; i < n; i++)
	//	showFloydPaths(log, distances, parents, v1, i);

	log.close();
}

void showDistMatrix(std::ofstream &os, Matrix &dist)
{
	Matrix &m = dist;

	os << "Distance matrix:" << endl;

	os << "     ";
	for (auto i = m.cbegin(); i != m.cend(); i++)
		os << std::setw(4) << "[" + std::to_string(i - m.cbegin() + 1) + "]";
	os << endl;

	for (auto row = m.cbegin(); row != m.cend(); row++)
	{
		os << std::setw(4) << "[" + std::to_string(row - m.cbegin() + 1) + "]";
		for (auto column = row->cbegin(); column != row->cend(); column++)
		{
			os << std::setw(4) << ((*column) == inf ? "i" : std::to_string((*column)));
		}
		os << endl;
	}
	os << endl;
}

void showParentsMatrix(std::ofstream &os, Matrix &parents, Matrix &dist)
{
	Matrix &m = parents;

	os << "Parents matrix:" << endl;

	os << "     ";
	for (auto i = m.cbegin(); i != m.cend(); i++)
		os << std::setw(4) << "[" + std::to_string(i - m.cbegin() + 1) + "]";
	os << endl;

	for (auto row = m.cbegin(); row != m.cend(); row++)
	{
		os << std::setw(4) << "[" + std::to_string(row - m.cbegin() + 1) + "]";
		for (auto column = row->cbegin(); column != row->cend(); column++)
		{
			int i = row - m.cbegin();
			int j = column - row->cbegin();

			os << std::setw(4) << ((dist[i][j] == inf || i == j) ? "-" : std::to_string((*column) + 1));
		}
		os << endl;
	}
	os << endl;
}

void showPath(std::ofstream &os, const std::vector<std::pair<int, short>> &distances, short v)
{
	if (distances[v].second >= 0 && distances[v].second != v)
	{
		showPath(os, distances, distances[v].second);
	}

	if (v == distances[v].second || distances[v].second < 0)
	{
		os << "[" << v + 1 << "]";
	}
	else
		os << " --> [" << v + 1 << "]";
};

void showFloydPaths(std::ofstream &os, const Matrix &dist, const Matrix &parents, short vi, short vj)
{
	if (dist.at(vi).at(vj) == inf)
	{
		os << "Пути из [" << vi + 1 << "] в [" << vj + 1 << "] не существует." << endl;
		return;
	}

	std::vector<short> path = { short(vj) };

	os << "Вес: " << std::setw(5) << dist[vi][vj];

	short k = parents[vi][vj];
	while (k != vj)
	{	
		path.insert(path.begin(), k);

		vj = k;
		k = parents[vi][k];
	}

	os << "\tПуть: ";
	for (auto it = path.cbegin(); it != path.cend(); it++)
	{
		if (it == path.cbegin())
			os << "[" << *it + 1 << "]";
		else
		{
			os << " --> [" << *it + 1 << "]";
		}
	}
	os << endl;;
}
#include "Graph.h"
#include <iostream>

#define inf SHRT_MAX

using std::cout;
using std::endl;


label_pack Graph::labeling(const short &s, const short &t, const Matrix &F)
{
	if (AdjacencyMatrix.empty())
		throw std::exception("Graph is empty.");

	using std::vector;
	using std::deque;

	vector<short>   Previous(AdjacencyMatrix.size(), -1);
	vector<short>   h(AdjacencyMatrix.size(), inf);
	vector<short>	choice(AdjacencyMatrix.size());
	deque<short>    Q;

	Previous[s] = s;

	Q.push_back(s);

	const Matrix &A = WeighedMatrix;

	while (h[t] == inf && Q.empty() == false)
	{
		short w = Q.front(); Q.pop_front();

		for (short v = 0; v < AdjacencyMatrix.size(); v++)
		{
			if (h[v] == inf && ((A[w][v] - F[w][v]) > 0))
			{
				h[v] = std::min((int)h[w], (A[w][v] - F[w][v]));
				Previous[v] = w;
				Q.push_back(v);
				choice[v] = 1;
			}
		}
		
		for (short v = 0; v < AdjacencyMatrix.size(); v++)
		{
			if (v == s) continue;

			if (h[v] == inf && F[v][w] > 0)
			{
				h[v] = std::min(h[w], F[v][w]);
				Previous[v] = w;
				Q.push_back(v);
				choice[v] = -1;
			}
		}
	}

	return label_pack(Previous, h, choice);
}

void get_st(const Graph &g, short &s, short &t);
void showFMatrix(std::ofstream &os, Matrix &F);

int Graph::edmonds_karp()
{
	Matrix F; resizeMatrix(F, AdjacencyMatrix.size());

	int f_value = 0;

	short s, t; get_st(*this, s, t);

	///*logging*/
	std::ofstream log("edmonds_karp.txt");
	log << "# s = " << s + 1<< endl;
	log << "# t = " << t + 1 << "\n\n";
	unsigned it = 1;
	/*******/

	label_pack lp;

	do
	{
		log << "# Итерация:\t" << it << endl;
		lp = labeling(s, t, F);

		if (lp.h[t] < inf)
		{

			log << "f - доп. цепь: ";
			show_chain(log, *this, lp, F, s, t);

			log << "h(P) = " << lp.h[t] << endl;

			f_value += lp.h[t]; 
			
			for (short v = t; v != s;)
			{
				short w = lp.Previous[v];

				if (lp.choice[v] == 1)
					F[w][v] = F[w][v] + lp.h[t];
				else
					F[v][w] = F[v][w] - lp.h[t];

				v = w;
			}

			log << "=============> ";
			show_chain(log, *this, lp, F, s, t);
		}
		else
		{
			log << "# f - доп. цепь: empty";
		}

		log << "\n\n" << endl;
		it++;
	} 
	while (lp.h[t] != inf);

	showFMatrix(log, F);

	log << "Максимальный поток: " << f_value << endl;
	cout << "Max flow: " << f_value << endl;
	log.close();

	return f_value;
}

void show_chain(std::ofstream &os, const Graph &g, const label_pack &lp, const Matrix &F, short s, short t)
{
	std::deque<short> chain = { t };

	while (t != s)
	{
		t = lp.Previous[t];
		chain.push_front(t);
	}

	os << "[" << s + 1 << "]";
	for (int v = 1; v < chain.size(); v++)
	{
		short cur = chain[v], prev = chain[v - 1];
		if (lp.choice[cur] == 1)
		{
			os << " --" << F[prev][cur] << "/" << g.WeighedMatrix[prev][cur] << "--> ";
		}
		else
		{
			os << " <-" << F[cur][prev] << "/" << g.WeighedMatrix[cur][prev] << "--- ";
		}

		os << "[" << cur + 1 << "]";
	}
	os << endl;
}

void get_st(const Graph &g, short &s, short &t)
{
	s = -1, t = -1;

	for (short v = 0; v < g.AdjacencyMatrix.size(); v++)
	{
		size_t o_deg = g.outDegree(v);
		size_t i_deg = g.inDegree(v);

		if (o_deg && !i_deg)
		{
			if (s != -1)
				throw std::exception("Bad net. Two or more sources.");
			s = v;
		}
		else if (i_deg && !o_deg)
		{
			if (t != -1)
				throw std::exception("Bad net. Two or more stocks.");
			t = v;
		}
	}

	if (s == -1 || t == -1)
		throw std::exception("Bad network. No stock or source.");
}

void showFMatrix(std::ofstream &os, Matrix &F)
{
	Matrix &m = F;

	os << "F matrix:" << endl;

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
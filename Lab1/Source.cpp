#include "Graph.h"
#include <iostream>
using std::cout;
using std::endl;

int main()
{
	try
	{
		Graph G1("graph1.txt");
		Graph G2("graph2.txt");

		Graph Result = G1 || G2;

		Result.showAdjacencyMatrix();
	}
	catch (const std::out_of_range &)
	{
		cout << "Exception: Vertex or vertices does not belong to this graph." << endl;
	}
	catch (const std::exception &ex)
	{
		cout << "Exception: " << ex.what() << endl;
	}

	system("pause");
}
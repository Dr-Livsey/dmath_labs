#include "Graph.h"
#include "Interface.h"
#include <iostream>
using std::cout;
using std::endl;

int main()
{
	try
	{
		Graph g("test.txt");

		g.floyd(1, 13);
		g.ford_bellman(1);

		//g.showBfs();

		//Interface my_interface;
		//my_interface();
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

//Задание 2

////а)
//G1.AddVertex("");
//G1.AddVertex("");
//G1.DeleteVertex(3);
//
////б)
////Уменьшили значение всех вершин выше удаленной [3], 
////так как граф уменьшился на одну вершину
//G1.AddEdge(3, 10);
//G1.AddEdge(5, 11);
//G1.AddEdge(3, 5);
//G1.AddEdge(11, 1);
//G1.AddEdge(10, 1);
//
//G1.DeleteEdge(2, 3);
//G1.DeleteEdge(9, 9);
//
////c)
//Graph G4 = G1;
//
//G4.getAdditionGraph();

////д)
//G3.AddVertex("");
//G3.AddVertex("");
//G3.AddVertex("");
//G3.DeleteVertex(3);
//
//G3.AddEdge(2, 1);
//G3.AddEdge(4, 1);
//G3.AddEdge(5, 6);
//G3.DeleteEdge(3, 5);

////е)
//
//G3.AddEdge(2, 1);
//G3.AddEdge(5, 1);
//G3.AddEdge(6, 7);
//G3.DeleteEdge(4, 6);
//
//G3.AddVertex("");
//G3.AddVertex("");
//G3.AddVertex("");
//G3.DeleteVertex(3);
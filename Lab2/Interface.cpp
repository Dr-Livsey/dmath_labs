#include "Interface.h"
#include "Graph.h"
#include <locale>

#include <iostream>

using std::cout;
using std::endl;

/*
	1. Реализовать get, add, del и операции +, *, || 
	2. В parse_command первые три if относятся к выводу информации
	3. Третий if в parse_command относится к операцию с графами. TESTED 100%
	4. В четвертом ифе в соответствующие функции операций подается строка 
		без названия операции и номера команды. Там требуется распарсить только имя графа и аргументы.
		Образец в show().*/


void Interface::operator()(void)
{
	std::string cmd;
	cout << std::endl;

	while (cmd != "exit")
	{
		try
		{
			cout << "# ";
			std::getline(std::cin, cmd);

			parse_command(cmd);
			cout << endl;

		}
		catch (const std::out_of_range &)
		{
			cout << "Exception: Vertex or vertices does not belong to this graph.\n" << endl;
		}
		catch (const std::exception &ex)
		{
			cout << "Exception: " << ex.what() << "\n\n";
		}
	}

}

void Interface::show(int cmd_num, std::string raw_cmd)
{
	if (cmd_num >= 0 && cmd_num <= 16 || (cmd_num >= 33 && cmd_num <= 35))
	{		
		Graph *graph_ptr = get_graph(raw_cmd);
		std::deque<std::string> stokens = get_tokens(raw_cmd, boost::regex("\\d{1,}"));

		//Если cmd_num из этого промежутка и в команде заданы stokens то exception
		if ((cmd_num >= 0 && cmd_num <= 1 ||
			cmd_num >= 3 && cmd_num <= 10 ||
			cmd_num >= 13 && cmd_num <= 16) && stokens.size())
		{
			throw std::exception("Invalid arguments in command: show");
		}

		get_function(graph_ptr, cmd_num, stokens);
	}
	else if (cmd_num == 32)	print_graphs();
	else
	{
		throw std::exception("Incorrect command number. (to see correct commands enter: \"show ?\")");
	}
}

void Interface::get(int cmd_num, std::string raw_cmd)
{
	/*  "21  <название графа>                     - определение дополнения графа\n"
		"22  <название графа> <р1>                - подразбиение ребра\n"
		"23  <название графа> {<в1>, ..., <вn>}   - стягивание графа\n"
		"24  <название графа> <р1>                - стягивание ребра\n"
		"25  <название графа> <р1>                - отождествление вершин\n"
		"26  <название графа> <в1>                - дублирование вершины\n"
		"27  <название графа> <в1>                - размножение вершины\n"
		"28  <название графа1> <название_графов2> - умножение графов\n"
		"29  <название графа1> <название_графов2> - присоединение графов\n"
		"30  <название графа1> <название_графов2> - объединение графов\n";
	*/

	if (cmd_num >= 21 and cmd_num <= 27)
	{
		Graph *graph_ptr = get_graph(raw_cmd);
		boost::regex token_base("\\d{1,}|\\(\\d{1,}, *\\d{1,}\\)");
		std::deque<std::string> stokens = get_tokens(raw_cmd, token_base);

		get_function(graph_ptr, cmd_num, stokens);
	}
	else if (cmd_num >= 28 and cmd_num <= 30)
	{
		Graph *graph_ptr = get_graph(raw_cmd);
		boost::regex token_base("\\\"[^\\\"]+?\\\"");

		std::deque<std::string> stokens = get_tokens(raw_cmd, token_base);

		get_function(graph_ptr, cmd_num, stokens);
	}
	else
	{
		throw std::exception("Incorrect command number. (to see correct commands enter: \"get ?\")");
	}
}

void Interface::add(int cmd_num, std::string raw_cmd)
{
	if (cmd_num == 17 or cmd_num == 19)
	{
		Graph *graph_ptr = get_graph(raw_cmd);
		boost::regex token_base("\\d{1,}|\\(\\d{1,}, *\\d{1,}\\)");
		std::deque<std::string> stokens = get_tokens(raw_cmd, token_base);

		if ((cmd_num == 17) && stokens.size())
		{
			throw std::exception("Invalid arguments in command: add 17");
		}

		get_function(graph_ptr, cmd_num, stokens);
	}
	else
	{
		throw std::exception("Incorrect command number. (to see correct commands enter: \"add ?\")");
	}
}

void Interface::delete_(int cmd_num, std::string raw_cmd)
{
	if (cmd_num == 18 or cmd_num == 20)
	{
		Graph *graph_ptr = get_graph(raw_cmd);

		boost::regex token_base("\\d{1,}|\\(\\d{1,}, *\\d{1,}\\)");
		std::deque<std::string> stokens = get_tokens(raw_cmd, token_base);

		get_function(graph_ptr, cmd_num, stokens);
	}
	else
	{
		throw std::exception("Incorrect command number. (to see correct commands enter: \"del ?\")");
	}
}

void Interface::parse_command(std::string row_cmd)
{
	boost::regex op_base("^ *(show|add|del|get|help) +\\?$");
	boost::regex graph_op_base("^ *(create|delete|copy) +(?:\\\"[^\\\"]+?\\\" *)+$");
	boost::regex ops("^ *(show|add|del|get) +(\\d{1,}) *(.*)$");
	boost::smatch matches;

	if (boost::regex_match(row_cmd, boost::regex("^ *help *$")))
	{
		help();
	}
	else if (boost::regex_match(row_cmd, matches, op_base))
	{
		if (matches[1].str() == "show")			print_show();	
		else if (matches[1].str() == "add")		print_add();
		else if (matches[1].str() == "del")  print_delete();
		else if (matches[1].str() == "get")		print_get();
	}
	else if (boost::regex_match(row_cmd, matches, boost::regex("^ *(\\d{1,}) +\\?$")))
	{
		int cmd_num = boost::lexical_cast<int>(matches[1].str());
		print_cmd(cmd_num);
	}
	else if (boost::regex_match(row_cmd, matches, graph_op_base))
	{
		std::string action_name = matches[1].str();

		std::deque<std::string> stokens = get_tokens(row_cmd, boost::regex("\\\"[^\\\"]+?\\\""));

		if (action_name == "create")
		{
			if (stokens.size() == 2) 
				create_graph(stokens[0].c_str(), stokens[1].c_str());
			else if (stokens.size() == 1)
				create_graph(stokens[0].c_str());
			else 
				throw std::exception("Too many parameters in command \"create\"");
		}
		else if (action_name == "delete")
		{
			if (stokens.size() == 1)
				delete_graph(stokens[0].c_str());
			else
				throw std::exception("Too many parameters in command \"delete\"");
		}
		else if (action_name == "copy")
		{
			if (stokens.size() == 2)
				copy_graph(stokens[0].c_str(), stokens[1].c_str());
			else
				throw std::exception("Invalid parameters in command \"copy\"");
		}
	}
	else if (boost::regex_match(row_cmd, matches, ops))
	{
		int cmd_num = boost::lexical_cast<int>(matches[2].str());

		if (matches[1].str() == "show")			show(cmd_num, matches[3].str());
		else if (matches[1].str() == "add")		add(cmd_num, matches[3].str());
		else if (matches[1].str() == "del")  delete_(cmd_num, matches[3].str());
		else if (matches[1].str() == "get")		get(cmd_num, matches[3].str());
	}
	else
	{
		throw std::exception(std::string("Invalid expression: \"" + row_cmd + "\"").c_str());
	}
	
}

std::deque<std::string> Interface::get_tokens(std::string row_cmd, boost::regex stoken_base)
{
	std::deque<std::string> stokens;
	boost::smatch matches;

	/*Example: stokens = "aaaaaa", "bbbbb" , "ccccc"*/
	std::string::const_iterator i(row_cmd.cbegin());
	while (boost::regex_search(i, row_cmd.cend(), matches, stoken_base))
	{
		i += matches.position() + matches[0].length();
		stokens.push_back(matches[0].str());
	}
	return stokens;
}

void Interface::create_graph(const char *graph_name)
{
	if (data_base.find(graph_name) != data_base.end())
		throw std::exception("Graph with this name already exists");

	std::string response;
	while (true)
	{
		cout << "Oriented? (Yes/No): ", std::getline(std::cin, response);

		if (response != "Yes" and response != "No")
		{
			cout << "Response was not recognized. Repeat, please\n" << endl;
		}
		else break;
	}

	bool isor = response == "Yes" ? true : false;

	data_base[std::string(graph_name)] = std::unique_ptr<Graph>(new Graph(isor));
}

void Interface::create_graph(const char *graph_name, const char *fname)
{
	boost::smatch stokens;
	std::string temp_s = std::string(fname);
	boost::regex_match(temp_s, stokens, boost::regex("\\\"([^\\\"]+?)\\\""));

	if (data_base.find(graph_name) != data_base.end())
		throw std::exception("Graph with this name already exists");

	data_base[std::string(graph_name)] = std::unique_ptr<Graph>(new Graph(stokens[1].str()));
}

void Interface::delete_graph(const char *graph_name)
{
	if (data_base.find(graph_name) == data_base.end())
		throw std::exception("No graph with the same name");

	data_base.erase(std::string(graph_name));
}

void Interface::copy_graph(const char *dst, const char *src)
{
	if (data_base.find(std::string(dst)) == data_base.end())
		throw std::exception(std::string("No graph's with name \"" + std::string(dst) + "\"").c_str());
	else if (data_base.find(std::string(src)) == data_base.end())
		throw std::exception(std::string("No graph's with name \"" + std::string(src) + "\"").c_str());

	(*data_base[std::string(dst)]) = (*data_base[std::string(src)]);
}

void Interface::print_graphs()
{
	if (data_base.empty())
	{
		cout << "Graph list is empty." << endl;
		return;
	}

	cout << "Graph list:" << endl;
	for (auto It = data_base.cbegin(); It != data_base.cend(); It++)
	{
		cout << "name: " << It->first << endl;
	}
}

void Interface::print_show()
{
	setlocale(LC_ALL, "Russian");
	const char *str =
		"0  <название графа>			- определение числа вершин\n"
		"1  <название графа>			- определение числа ребер(дуг)\n"
		"2  <название графа> <в1>                - определение степени произвольной вершины\n"
		"3  <название графа>                     - определение степенной последовательности графа\n"
		"4  <название графа>		        - определение матрицы смежности\n"
		"5  <название графа>			- определение матрицы инцидентности\n"
		"6  <название графа>			- определение списка смежности\n"
		"7  <название графа>			- определение висячих вершин\n"
		"8  <название графа>			- определение изолированных вершин\n"
		"9  <название графа>			- определение в орграфе истоков\n"
		"10 <название графа>			- определение в орграфе стоков\n"
		"11 <название графа> <в1> <в2>	        - определение расстояния между двумя вершинами\n"
		"12 <название графа> <в1>		- определение эксцентриситета вершины\n"
		"13 <название графа>			- определение диаметра графа\n"
		"14 <название графа>			- определение радиуса графа\n"
		"15 <название графа>			- определение центра графа\n"
		"16 <название графа>			- определение периферийных вершин\n"
		"32                                      - вывод доступных графов на экран\n"
		"33 <название графа>            - получение минимального остова графа алгоритмом Прима\n"
		"34 <название графа>            - получение остова графа поиском в ширину\n"
		"35 <название графа>            - вывод матрицы весов\n";

	cout << str << endl;
}

void Interface::print_add()
{
	setlocale(LC_ALL, "Russian");
	const char *str =
		"17  <название графа>         - добавление вершины в граф\n"
		"19  <название графа> <р1>    - добавление ребра в граф\n";

	cout << str << endl;
}

void Interface::print_delete()
{
	setlocale(LC_ALL, "Russian");
	const char *str =
		"18  <название графа> <в1>    - удаление вершины в графе\n"
		"20  <название графа> <р1>    - удаление ребра в графе\n";

	cout << str << endl;
}

void Interface::print_get()
{
	setlocale(LC_ALL, "Russian");
	const char *str =
		"21  <название графа>                     - определение дополнения графа\n"
		"22  <название графа> <р1>                - подразбиение ребра\n"
		"23  <название графа> {<в1>, ..., <вn>}   - стягивание графа\n"
		"24  <название графа> <р1>                - стягивание ребра\n"
		"25  <название графа> <в1> <в1>           - отождествление вершин\n"
		"26  <название графа> <в1>                - дублирование вершины\n"
		"27  <название графа> <в1>                - размножение вершины\n"
		"28  <название графа1> <название_графов2> - умножение графов\n"
		"29  <название графа1> <название_графов2> - присоединение графов\n"
		"30  <название графа1> <название_графов2> - объединение графов\n";

	cout << str << endl;
}

void Interface::help()
{
	setlocale(LC_ALL, "Russian");
	const char *str =
		"Список доступных комманд в операции: <операция> ?\n"
		"Доступные операции: show | add | del | get\n\n"
		"Ребра указываются в виде: (в1, в2)\n"

		"Создать пустой граф:    create \"название графа\"\n"
		"Создать не пустой граф: create \"название графа\" \"имя файла\"\n"
		"Удалить граф:           delete \"название графа\"\n"
		"Копировать граф:        copy   \"название графа назначения\" \"название исходного графа\"\n\n"

		"Соединение графов:      <Г_рез> = <Г1> + <Г2>\n"
		"Произведение графов:    <Г_рез> = <Г1> * <Г2>\n"
		"Объединение графов:     <Г_рез> = <Г1> || <Г2>\n\n"

		"Формат команды:  1. <show|add|del|get> <номер_операции> <название_графа> [параметры]\n"
		"Пример команды:  1. show 2 \"G1\" 5\n";

	cout << str << endl;
}

void Interface::print_cmd(int cmd_num)
{
	if (cmd_num >= 0 && cmd_num < 17 || cmd_num == 31 || cmd_num >=32 && cmd_num <= 35)
	{
		print_show();
	}
	else if (cmd_num == 17 || cmd_num == 19)
	{
		print_add();
	}
	else if (cmd_num == 18 || cmd_num == 20)
	{
		print_delete();
	}
	else if (cmd_num > 20 && cmd_num < 31)
	{
		print_get();
	}
	else
	{
		cout << "Unknown command number: " << cmd_num << "\n";
		help();
	}
}

Graph * Interface::get_graph(std::string &cmd_line)
{
	boost::regex base
	(
		"^(\\\"[^\\\"]+?\\\")(?: +(?:\\d{1,}|\\(\\d{1,}, *\\d{1,}\\)|\\\"[^\\\"]+?\\\"))* *$"
	);
	boost::smatch matches;
	bool take_gname = boost::regex_match(cmd_line, matches, base);

	if (take_gname == false)
		throw std::exception("Invalid arguments");

	if (data_base.find(matches[1].str()) == data_base.end())
		throw std::exception("No graph with the same name");

	Graph *g = data_base[matches[1].str()].get();

	/*erase graph name*/
	cmd_line.erase(cmd_line.begin(), cmd_line.begin() + matches[1].length());

	return g;
}

std::pair<short, short> Interface::get_edge(const std::string &str_edge)
{
	boost::regex base("^\\((\\d{1,}), *(\\d{1,})\\)$");
	boost::smatch matches;
	bool is_edge = boost::regex_match(str_edge, matches, base);

	if (is_edge == false)
		throw std::exception(std::string("Invalid arguments: \"" + str_edge + "\" is not the edge").c_str());

	std::pair<short, short> edge =
	{
		boost::lexical_cast<short>(matches[1].str()),
		boost::lexical_cast<short>(matches[2].str())
	};

	return edge;
}

void Interface::get_function(Graph *graph, int cmd_num, const std::deque<std::string>&args)
{
	switch (cmd_num)
	{
	case 0:
		graph->vertexAmount();
		break;
	case 1:
		graph->edgeAmount();
		break;
	case 2:
	{
		if (args.size() != 1)
			throw std::exception("Invalid amount of arguments in \"showVertexDegree\"");
		short v;
		try
		{
			v = boost::lexical_cast<short>(args[0]);
		}
		catch (boost::bad_lexical_cast &)
		{
			throw std::exception("Invalid arguments in \"showVertexDegree\"");
		}
		graph->showVertexDegree(v);
	}
		break;
	case 3:
		graph->showDegSequance();
		break;
	case 4:
		graph->showAdjacencyMatrix();
		break;
	case 5:
		graph->showIncidenceMatrix();
		break;
	case 6:
		graph->showAdjacencyList();
		break;
	case 7:
		graph->showLeafs();
		break;
	case 8:
		graph->showIzolatedVertexes();
		break;
	case 9:
		graph->showSources();
		break;
	case 10:
		graph->showStocks();
		break;
	case 11:
	{
		if (args.size() != 2)
			throw std::exception("Invalid amount of arguments in \"showDistance\"");

		short a, b;
		try
		{
			a = boost::lexical_cast<short>(args[0]);
			b = boost::lexical_cast<short>(args[1]);
		}
		catch (boost::bad_lexical_cast &)
		{
			throw std::exception("Invalid arguments in \"showDistance\"");
		}

		graph->showDistance(a, b);
	}
		break;
	case 12:
	{
		if (args.size() != 1)
			throw std::exception("Invalid amount of arguments in \"showEccentricity\"");
		short b;
		try
		{
			b = boost::lexical_cast<short>(args[0]);
		}
		catch (boost::bad_lexical_cast &)
		{
			throw std::exception("Invalid arguments in \"showEccentricity\"");
		}
		graph->showEccentricity(b);
	}
		break;
	case 13:
		graph->showDiameter();
		break;
	case 14:
		graph->showRadius();
		break;
	case 15:
		graph->showCenter();
		break;
	case 16:
		graph->showPeripheralVertex();
		break;
	case 17:
		graph->AddVertex("");
		break;
	case 18:
		if (args.size() != 1)
			throw std::exception("Invalid amount of arguments in \"DeleteVertex\"");
		{
			short b;
			try
			{
				b = boost::lexical_cast<short>(args[0]);
			}
			catch (boost::bad_lexical_cast &)
			{
				throw std::exception("Invalid arguments in \"DeleteVertex\"");
			}

			graph->DeleteVertex(b);
		}
		break;
	case 19:/*Добавление ребра*/
		if (args.size() != 1)
			throw std::exception("Invalid amount of arguments in \"AddEdge\"");
		{
			std::pair<short, short> edge = get_edge(args[0]);
			graph->AddEdge(edge.first, edge.second);
		}
		break;
	case 20:/*Удаление ребра*/
		if (args.size() != 1)
			throw std::exception("Invalid amount of arguments in \"DeleteEdge\"");
		{
			std::pair<short, short> edge = get_edge(args[0]);
			graph->DeleteEdge(edge.first, edge.second);
		}
		break;
	case 21:/*Дополнение графа*/
		if (args.size() != 0)
			throw std::exception("Invalid amount of arguments in \"getAdditionGraph\"");
		graph->getAdditionGraph();
		break;
	case 22:/*Подразбиение ребра*/
		if (args.size() != 1)
			throw std::exception("Invalid amount of arguments in \"splitEdge\"");
		{
			std::pair<short, short> edge = get_edge(args[0]);
			graph->splitEdge(edge.first, edge.second);
		}
		break;
	case 23:/*Стягивание графа*/
	{
		std::vector<short> v;
		for (int i = 0; i < args.size(); i++)
		{
			short vert;
			try
			{
				vert = boost::lexical_cast<short>(args[i]);
			}
			catch (const boost::bad_lexical_cast&)
			{
				throw std::exception("Invalid arguments in \"splitGraph\"");
			}

			v.push_back(vert);
		}
		graph->pullOffGraph(v);
	}
		break;
	case 24:/*Стягивание ребра*/
		if (args.size() != 1)
			throw std::exception("Invalid amount of arguments in \"pullOfEdge\"");
		{
			std::pair<short, short> edge = get_edge(args[0]);
			graph->pullOffEdge(edge.first, edge.second);
		}
		break;
	case 25:/*Отождествление вершин*/
		if (args.size() != 2)
			throw std::exception("Invalid amount of arguments in \"vertexIdentification\"");
		{
			short a, b;
			try
			{
				a = boost::lexical_cast<short>(args[0]);
				b = boost::lexical_cast<short>(args[1]);
			}
			catch (boost::bad_lexical_cast &)
			{
				throw std::exception("Invalid arguments in \"vertexIdentification\"");
			}
			graph->vertexIdentification(a, b);
		}
		break;
	case 26:/*Дублирование вершины*/
		if (args.size() != 1)
			throw std::exception("Invalid amount of arguments in \"vertexDuplicate\"");

		short b;
		try
		{
			b = boost::lexical_cast<short>(args[0]);
		}
		catch (boost::bad_lexical_cast &)
		{
			throw std::exception("Invalid arguments in \"vertexDuplicate\"");
		}
		graph->vertexDuplicate(b);
		break;
	case 27:
		if (args.size() != 1)
			throw std::exception("Invalid amount of arguments in \"vertexReproduction\"");
		{
			short b;
			try
			{
				b = boost::lexical_cast<short>(args[0]);
			}
			catch (boost::bad_lexical_cast &)
			{
				throw std::exception("Invalid arguments in \"vertexReproduction\"");
			}
			graph->vertexReproduction(b);
		}
		break;
	case 28:/*умножение графов*/
		if (args.size() != 1)
			throw std::exception("Invalid amount of arguments");

		if (data_base.find(args[0]) == data_base.end())
			throw std::exception(std::string("No graph with the same name: " + args[0]).c_str());
		{
			Graph *g = data_base[args[0]].get();

			(*graph) = (*graph) * (*g);
		}

		break;
	case 29:/*присоединение графов*/
		if (args.size() != 1)
			throw std::exception("Invalid amount of arguments");

		if (data_base.find(args[0]) == data_base.end())
			throw std::exception(std::string("No graph with the same name: " + args[0]).c_str());
		{
			Graph *g = data_base[args[0]].get();

			(*graph) = (*graph) + (*g);
		}
		break;
	case 30:/*объединение графов*/
		if (args.size() != 1)
			throw std::exception("Invalid amount of arguments");

		if (data_base.find(args[0]) == data_base.end())
			throw std::exception(std::string("No graph with the same name: " + args[0]).c_str());
		{
			Graph *g = data_base[args[0]].get();

			(*graph) = (*graph) || (*g);
		}
		break;
	case 33: /*алгоритм Прима*/
		graph->algorithmPrima();
		break;
	case 34: /*поиск в ширину*/
		graph->showBfs();
		break;
	case 35:/*вывод матрицы весов*/
		graph->showWeightedMatrix();
		break;
	default:
		break;
	}
}

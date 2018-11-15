#include "Interface.h"
#include "Graph.h"
#include <locale>

#include <iostream>

using std::cout;
using std::endl;

/*
	1. ����������� get, add, delete � �������� +, *, || 
	2. � parse_command ������ ��� if ��������� � ������ ����������
	3. ������ if � parse_command ��������� � �������� � �������. TESTED 100%
	4. � ��������� ��� � ��������������� ������� �������� �������� ������ 
		��� �������� �������� � ������ �������. ��� ��������� ���������� ������ ��� ����� � ���������.
		������� � show().*/


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
	if (cmd_num >= 0 && cmd_num <= 16)
	{
		boost::regex base("^(\\\"[^\\\"]+?\\\")(?: +\\d{1,})* *$");
		boost::smatch matches;
		bool take_gname = boost::regex_match(raw_cmd, matches, base);

		if (take_gname == false)
			throw std::exception("Invalid arguments in command: show");

		if (data_base.find(matches[1].str()) == data_base.end())
			throw std::exception("No graph with the same name");
		
		Graph *graph_ptr = data_base[matches[1].str()].get();

		std::string srest;
		std::copy(raw_cmd.begin() + matches[1].length(), raw_cmd.end(), std::back_inserter(srest));

		std::deque<std::string> stokens = get_tokens(srest, boost::regex("\\d{1,}"));

		//���� cmd_num �� ����� ���������� � � ������� ������ stokens �� exception
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

void Interface::get(int cmd_num, std::string)
{
}

void Interface::add(int cmd_num, std::string)
{
}

void Interface::delete_(int cmd_num, std::string)
{
}

void Interface::parse_command(std::string row_cmd)
{
	boost::regex op_base("^ *(show|add|delete|get|help) +\\?$");
	boost::regex graph_op_base("^ *(create|delete|copy) +(?:\\\"[^\\\"]+?\\\"(?: *|$))+$");
	boost::regex ops("^ *(show|add|delete|get) +(\\d{1,}) *(.*)$");
	boost::smatch matches;

	if (boost::regex_match(row_cmd, boost::regex("^ *help *$")))
	{
		help();
	}
	else if (boost::regex_match(row_cmd, matches, op_base))
	{
		if (matches[1].str() == "show")			print_show();	
		else if (matches[1].str() == "add")		print_add();
		else if (matches[1].str() == "delete")  print_delete();
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
		else if (matches[1].str() == "delete")  delete_(cmd_num, matches[3].str());
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

	data_base[std::string(graph_name)] = std::unique_ptr<Graph>(new Graph());
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
		"0  <�������� �����>			- ����������� ����� ������\n"
		"1  <�������� �����>			- ����������� ����� �����(���)\n"
		"2  <�������� �����> <�1>                - ����������� ������� ������������ �������\n"
		"3  <�������� �����>                     - ����������� ��������� ������������������ �����\n"
		"4  <�������� �����>		        - ����������� ������� ���������\n"
		"5  <�������� �����>			- ����������� ������� �������������\n"
		"6  <�������� �����>			- ����������� ������ ���������\n"
		"7  <�������� �����>			- ����������� ������� ������\n"
		"8  <�������� �����>			- ����������� ������������� ������\n"
		"9  <�������� �����>			- ����������� � ������� �������\n"
		"10 <�������� �����>			- ����������� � ������� ������\n"
		"11 <�������� �����> <�1> <�2>	        - ����������� ���������� ����� ����� ���������\n"
		"12 <�������� �����> <�1>		- ����������� ��������������� �������\n"
		"13 <�������� �����>			- ����������� �������� �����\n"
		"14 <�������� �����>			- ����������� ������� �����\n"
		"15 <�������� �����>			- ����������� ������ �����\n"
		"16 <�������� �����>			- ����������� ������������ ������\n"
		"32                                      - ����� ��������� ������ �� �����\n";

	cout << str << endl;
}

void Interface::print_add()
{
	setlocale(LC_ALL, "Russian");
	const char *str =
		"17  <�������� �����>         - ���������� ������� � ����\n"
		"19  <�������� �����> <�1>    - ���������� ����� � ����\n";

	cout << str << endl;
}

void Interface::print_delete()
{
	setlocale(LC_ALL, "Russian");
	const char *str =
		"18  <�������� �����> <�1>    - �������� ������� � �����\n"
		"20  <�������� �����> <�1>    - �������� ����� � �����\n";

	cout << str << endl;
}

void Interface::print_get()
{
	setlocale(LC_ALL, "Russian");
	const char *str =
		"21  <�������� �����>                     - ����������� ���������� �����\n"
		"22  <�������� �����> <�1>                - ������������ �����\n"
		"23  <�������� �����> {<�1>, ..., <�n>}   - ���������� �����\n"
		"24  <�������� �����> <�1>                - ���������� �����\n"
		"25  <�������� �����> <�1>                - �������������� �������\n"
		"26  <�������� �����> <�1>                - ������������ �������\n"
		"27  <�������� �����> <�1>                - ����������� �������\n";

	cout << str << endl;
}

void Interface::help()
{
	setlocale(LC_ALL, "Russian");
	const char *str =
		"������ ��������� ������� � ��������: <��������> ?\n"
		"��������� ��������: show | add | delete | get\n\n"
		"����� ����������� � ����: (�1, �2)\n"

		"������� ������ ����:    create \"�������� �����\"\n"
		"������� �� ������ ����: create \"�������� �����\" \"��� �����\"\n"
		"������� ����:           delete \"�������� �����\"\n"
		"���������� ����:        copy   \"�������� ����� ����������\" \"�������� ��������� �����\"\n\n"

		"���������� ������:      <�_���> = <�1> + <�2>\n"
		"������������ ������:    <�_���> = <�1> * <�2>\n"
		"����������� ������:     <�_���> = <�1> || <�2>\n\n"

		"������ �������:  1. <show|add|delete|get> <�����_��������> <��������_�����> [���������]\n"
		"                 2. <��������_�����_���> = <��������_�����1> <|| | * | +> <��������_�����2>\n"
		"������ �������:  1. show 2 \"G1\" 5\n"
		"                 2.\"G1\" = \"G2\" + \"G3\"\n";

	cout << str << endl;
}

void Interface::print_cmd(int cmd_num)
{
	if (cmd_num >= 0 && cmd_num < 17 || cmd_num == 31 || cmd_num == 32)
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
	else if (cmd_num > 20 && cmd_num < 28)
	{
		print_get();
	}
	else
	{
		cout << "Unknown command number: " << cmd_num << "\n";
		help();
	}
}

void get_function(Graph *graph, int cmd_num, const std::deque<std::string>&args)
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
		short v = boost::lexical_cast<short>(args[0]);
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
		short a = boost::lexical_cast<short>(args[0]);
		short b = boost::lexical_cast<short>(args[1]);
		graph->showDistance(a, b);
	}
		break;
	case 12:
	{
		if (args.size() != 1)
			throw std::exception("Invalid amount of arguments in \"showEccentricity\"");
		short b = boost::lexical_cast<short>(args[0]);
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
		break;
	case 18:
		break;
	case 19:
		break;
	case 20:
		break;
	case 21:
		break;
	case 22:
		break;
	case 23:
		break;
	case 24:
		break;
	case 25:
		break;
	case 26:
		break;
	case 27:
		break;
	case 28:
		break;
	case 29:
		break;
	case 30:
		break;
	default:
		break;
	}
}
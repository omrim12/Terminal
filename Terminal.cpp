#include "Terminal.h"
/*********************/
Terminal::Terminal():system(File_system()),command_dict(std::map<std::string,int>{ }){
	command_dict.insert({"lproot",lproot});
	command_dict.insert({"pwd", pwd});
	command_dict.insert({"exit", Exit});
	command_dict.insert({"cat",cat});
	command_dict.insert({"wc",wc});
	command_dict.insert({"ls",ls});
	command_dict.insert({"touch",touch});
	command_dict.insert({"remove",Remove});
	command_dict.insert({"rmdir",rmdir});
	command_dict.insert({"chdir",chdir});
	command_dict.insert({"mkdir",mkdir});
	command_dict.insert({"ln",ln});
	command_dict.insert({"copy",Copy});
	command_dict.insert({"move",Move});
	command_dict.insert({"read",read});
	command_dict.insert({"write",write});
};
/*********************/
Terminal::~Terminal()	{}
/*********************/
void Terminal::run()	{

	int index;
	char char_buff;
	bool exit_val = false;
	string input = "";
	string command = "";
	string operand1 = "";
	string operand2 = "";
	string operand3 = "";
	stringstream command_parser;


	while( !exit_val )	{

		// clear input buffers
		operand1 = "";
		operand2 = "";
		operand3 = "";
		command = "";
		command_parser.clear();

		// retrieve input line
		getline(cin,input);

		// parse input line
		command_parser << input;
		command_parser >> command >> operand1 >> operand2 >> operand3;
		if( command == "" )	{ continue; }

		try	{
			// invalid input
			if( command_dict.find(command) == command_dict.end() )	{throw InvalidInput();}

			// execute user input request
			if( operand3 == "" )	{
				if( operand2 == "" )	{
					if( operand1 == "" )	{
						switch( (*command_dict.find(command)).second )	{

							case lproot:
								system.lproot();
								break;

							case pwd:
								system.pwd();
								break;

							case Exit:
								exit_val = true;
								break;

							case ls:
								system.ls();
								break;

							default:
								throw InvalidInput();
								break;
						}
					}
					else	{
						switch( (*command_dict.find(command)).second )	{
							case cat:
								if( system[operand1] == nullptr )	{throw File::FileNotFoundException(); }
								system[operand1]->cat();
								break;

							case wc:
								if( system[operand1] == nullptr )	{throw File::FileNotFoundException(); }
								system[operand1]->wc();
								break;

							case touch:
								system.touch(operand1);
								break;

							case Remove:
								system.remove(operand1);
								break;

							case rmdir:
								system.rmdir(operand1);
								break;

							case chdir:
								system.chdir(operand1);
								break;

							case mkdir:
								system.mkdir(operand1);
								break;

							default:
								throw InvalidInput();
								break;
						}
					}
				}
				else 	{
					switch( (*command_dict.find(command)).second )	{
						case ln:
							if( system[operand1] == nullptr )	{throw File::FileNotFoundException();}
							system[operand1]->ln((*system[operand2]));
							break;

						case Copy:
							system.copy(operand1,operand2);
							break;

						case Move:
							system.move(operand1,operand2);
							break;

						case read:
							command_parser.clear();
							command_parser << operand2;
							command_parser >> index;
							if( command_parser.fail() )	{throw InvalidInput();}
							if( system[operand1] == nullptr )	{throw File::FileNotFoundException();}
							//TODO : fix the read operation !!!!!!!!!!
							cout << (*system[operand1])[index] << endl;
							break;

						default:
							throw InvalidInput();
							break;
					}

				}
			}
			else	{
				switch( (*command_dict.find(command)).second )	{
				case write:
					if( system[operand1] == nullptr )	{ throw File::FileNotFoundException(); }
					if( operand3.length() > 1 )	{throw InvalidInput();}
					command_parser.clear();
					command_parser << operand2;
					command_parser >> index;
					if( command_parser.fail() )	{ throw InvalidInput(); }
					char_buff = operand3[0];
					(*(system[operand1]))[index] = char_buff;
					break;

				default:
					throw InvalidInput();
					break;
				}
			}
		}

		// catch any exception thrown
		catch( exception& e )	{cerr << e.what() << endl;}
	}


	// program has ended. clear all system's content.
}
/*********************/

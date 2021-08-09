#ifndef TERMINAL_H_
#define TERMINAL_H_
#include "File_system.h"
#include "commands.h"
/**************************/
class Terminal	{

	File_system system;
	std::map<string,int> command_dict; // a command mapper ( helpful for implementing terminal I/O using switch cases on the given command ).

public:
	class InvalidInput : public exception	{
	public : virtual const char* what() const noexcept	{ return "ERROR: invalid input"; }
	};
	Terminal();
	~Terminal();
	void run(); // mini-terminal Main program.
};
/**************************/
#endif

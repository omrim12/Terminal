#include "Terminal.h"

int main() {
	try	{Terminal terminal; terminal.run();}
	catch( ... )	{cerr << "ERROR: something went wrong - terminal shutdown." << endl;}
	return 0;
}

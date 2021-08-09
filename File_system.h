#ifndef FILE_SYSTEM_H_
#define FILE_SYSTEM_H_
#include "File.h"
#include <vector>
#include <iterator>
#include <map>
#include <ctime>
#include <algorithm>
#include <iterator>
using namespace std;
/*****************************/
class Terminal;
/*****************************/
class File_system	{

	/**data members**/
	std::map<string,std::vector<File*> > system;
	string current_dir;

	/**helper functions**/
	bool startswith(string root, string sub_root) const;

public :
	/**exceptions**/
	class NoSuchDirectory : public exception	{
	public : virtual const char* what() const noexcept	{ return "ERROR: no such directory as requested."; }
	};
	class DirectoryExists : public exception	{
	public : virtual const char* what() const noexcept	{ return "ERROR: requested directory already exists."; }
	};
	class RootException : public exception	{
	public : virtual const char* what() const noexcept	{ return "ERROR: all directories are under root 'V/'."; }
	};

	/**c'tors & d'tors**/
	File_system();
	~File_system();

	/**random access operators**/
	File* operator[](string filename); // returns a pointer to a File named after filename if exists. otherwise, returns a nullptr
	std::vector<File*>::iterator operator[](File* file); // returns an iterator to a given file in system.

	/**File interface methods**/
	void touch(string filename); // updates time stamp of a file. if a file with given filename does not exist, it creates such file.
	void copy(string src_file, string dest_file) throw(File::FileNotFoundException); // copies src_file to dest_file. dest_file might not exist.
	void remove(string filename) throw(File::FileNotFoundException); // deletes a file by given filename.
	void move(string src_file, string dest_file) throw(File::FileNotFoundException); // moves content src_file to dest_file. dest_file might not exist.

	/**Virtual directory interface methods**/
	void rmdir(const string& dir_name) throw(NoSuchDirectory); // removes an existing directory by its name including all of its
															   // sub directories. if it doesn't exist, a proper exception will be thrown.
	void chdir(const string& dir_name) throw(NoSuchDirectory); // changes current directory to a directory named after dir_name. if there's no such one,
															   //a proper exception is thrown.
	void mkdir(string& dir_name) throw(DirectoryExists,RootException); // creates a new directory named after dir_name if doesn't exists.
																			 // if so, a proper exception will be thrown.
	void ls() const; // prints out the content of current directory
	void lproot() const; // prints out all content in File_system (directory by directory) including reference counters of all files within.
	void pwd() const; // prints out current directory.
};
/*****************************/
#endif

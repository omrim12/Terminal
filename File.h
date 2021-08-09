#ifndef FILE_H_
#define FILE_H_
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstring>
using namespace std;
/***********************************/
class File_system;
/***********************************/
class File	{

private:
	struct file_ref_count	{
		int refCount;
		std::fstream *raw_file;
		string raw_filename;
		file_ref_count(std::fstream* file, string& filename);
		~file_ref_count();
	};

	file_ref_count *file_guard;
	string filename;
	string dir_name;
	int next_loc_to_access;

public:
	friend class File_system;

	/*exceptions*/
	class FileRangeException : public exception	{
	public : virtual const char* what() const noexcept	{return "ERROR: given index exceeded file length.";}
	};
	class FileNotFoundException : public exception	{
	public : virtual const char* what() const noexcept	{return "ERROR: file does not exist.";}
	};

	/*c'tors & d'tors*/
	File(const string& filename, string& dir);
	File(const File& other, const string& new_filename);
	File& operator=(const File& other);
	~File();

	/*interface operators*/
	void cat() const throw(FileNotFoundException); // prints out file content.
	void wc() const throw(FileNotFoundException); // prints out number of lines, words and characters within file.
	void ln(const File& other) throw(FileNotFoundException); // hard link between this and other.
	string getFileName() const;

	/*read & write*/
	File& operator[](int i) throw(FileRangeException);	// writing to a file.
	File& operator=(char new_char); // used for writing new_char into the file in previously given location.
	operator char(); // used for returning a single character from the file in previously given location.

	/*personal use*/
	int getCount()	const	{return file_guard->refCount;}
	int length() const;
	bool FileLengthExceed(int i) const; //Indicate if a given index for read/write to a file exceeded file length.
	string toString() const;

};
/********************************/
#endif

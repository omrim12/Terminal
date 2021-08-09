#include "File.h"
#include "File_system.h"
/******************************/
File::file_ref_count::file_ref_count(std::fstream *file, string& filename):refCount(1),raw_file(const_cast<std::fstream*>(file)),
										raw_filename(filename)	{}
/******************************/
File::file_ref_count::~file_ref_count()	{delete raw_file;}
/******************************/
File::~File()	{
	// only file pointing at reference counter's raw file.
	// clear file's content and close.
	if( --file_guard->refCount == 0 )	{
		file_guard->raw_file->open(file_guard->raw_filename.c_str(),ios::out);
		file_guard->raw_file->write("",0);
		file_guard->raw_file->close();
		delete file_guard;
	}
}
/******************************/
File::File(const string& filename, string& dir):file_guard(new file_ref_count(new std::fstream(filename.c_str(),ios::app),const_cast<string&>(filename))) ,
			filename(filename),dir_name(dir), next_loc_to_access(-1)	{
			file_guard->raw_file->close();
}
/******************************/
File::File(const File& other, const string& new_filename):file_guard(other.file_guard), filename(new_filename),dir_name(other.dir_name),
														next_loc_to_access(-1) { ++other.file_guard->refCount; }
/******************************/
File& File::operator=(const File& other)	{ // This operator behaves as a hard link between few of this and other's data members.
											  // called within the function ln.

	// if reference counters are the same one, nothing to be done.
	if( this->file_guard == other.file_guard )	{return *this;}

	// if this has been initialized already before.
	if( file_guard != nullptr )	{
		if( --file_guard->refCount == 0 )	{ delete file_guard; }
	}

	// point to other's reference counter.
	// hard link between this and other is being preformed.
	file_guard = other.file_guard;
	file_guard->refCount++;

	return *this;
}
/******************************/
void File::cat() const throw(FileNotFoundException)	{ // This function prints the content of the file.

	if( file_guard->raw_file == nullptr )	{throw FileNotFoundException();}

	string buffer = "";
	file_guard->raw_file->open(file_guard->raw_filename.c_str(),ios::in);

	while(	getline(*file_guard->raw_file,buffer)	)	{cout << buffer << endl;}

	file_guard->raw_file->clear();
	file_guard->raw_file->close();
}
/******************************/
void File::wc() const throw(FileNotFoundException)	{ // This function prints out number of lines, words and characters that are within the file.

	if( file_guard->raw_file == nullptr )	{throw FileNotFoundException();}
	file_guard->raw_file->open(file_guard->raw_filename.c_str(),ios::in);

	int lines = 0;
	int words = 0;
	int chars = 0;

	string line = "";
	string word = "";
	stringstream ss;

	while(	getline(*file_guard->raw_file,line)	)	{
		ss.clear();
		lines++;
		ss << line;
		while( ss >> word )	{ chars += word.length(); words++; }
	}

	cout << lines << " " << words << " " << chars << endl;

	file_guard->raw_file->clear();
	file_guard->raw_file->close();
}
/******************************/
void File::ln(const File& other) throw(FileNotFoundException)	{ // This function preforms a hard link from this to other's file.
																  // this hard link is implemented with reference counting method,
																  // which will cause a behaviour where both files are holding a mutual
																  // pointer to the same reference counter ( where the raw file is stored ).

	if( filename == other.filename )	{return;}

	// cannot hard link with a non-existing file.
	if( other.file_guard->raw_file == nullptr )	{throw FileNotFoundException();}

	// operator = is used for actual hard link.
	// implementation description is within the operator.
	else	{*this = other;}
}
/******************************/
File& File::operator=(char new_char)	{ // This operator is used for writing a single character to a file ( file[i] = some_char; )

	//string save_raw_filename = file_guard->raw_filename;
	file_guard->raw_file->open(file_guard->raw_filename.c_str(),ios::in);
	//file_guard->raw_filename = save_raw_filename;
	string content = "";
	string buffer;

	// retrieving older content (before change request)
	while( getline(*file_guard->raw_file,buffer) )	{content += buffer; content += '\n';}
	file_guard->raw_file->close();

	// updating new content
	if( content.size() == 0 )	{content = " ";}
	content[next_loc_to_access] = new_char;

	// writing new content to file.
	file_guard->raw_file->open(file_guard->raw_filename.c_str(),ios::out);
	file_guard->raw_file->write(content.c_str(),content.size());
	file_guard->raw_file->close();

	return *this;
}
/******************************/
File& File::operator[](int i) throw(FileRangeException)	{ // this operator is used both for read-only and write to a file.
														  // its purpose is to update a proxy variable for next given location
														  // to access the file.
	if( FileLengthExceed(i) )	{ throw FileRangeException(); }
	next_loc_to_access = i;
	return *this;
}
/******************************/
bool File::FileLengthExceed(int i) const	{ // returns if a given index exceeded file's length.

	if( i > length() )	{return true;}
	return false;

}
/******************************/
int File::length() const	{ // returns file's length.

	file_guard->raw_file->open(file_guard->raw_filename.c_str(),ios::in);
	file_guard->raw_file->seekg(0,ios::end);
	int end = file_guard->raw_file->tellg();
	file_guard->raw_file->seekg(0,ios::beg);
	int start = file_guard->raw_file->tellg();
	file_guard->raw_file->close();

	return end - start;
}
/******************************/
File::operator char()	{ // This function performes the actual reading process from the file.
						  // ( a copy is returned, hence no change is made on this file's data ).

	file_guard->raw_file->open(file_guard->raw_filename.c_str(), ios::in);
	file_guard->raw_file->seekg(next_loc_to_access,ios::beg);
	char retrieved = file_guard->raw_file->get();
	file_guard->raw_file->close();

	return retrieved;
}
/******************************/
string File::getFileName() const	{return filename;}
/******************************/
string File::toString() const	{

	stringstream ss;
	string toString;

	ss << file_guard->refCount;
	toString += filename;
	toString += " - ";
	toString += ss.str();

	return toString;
}


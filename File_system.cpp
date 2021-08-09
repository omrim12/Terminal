#include "File_system.h"
#include "Terminal.h"
/**************************/
File_system::File_system():system(std::map<string,std::vector<File*> >{}), current_dir(string("V/"))	{
	system.insert({"V/",std::vector<File*>{}}); // inserting root directory.
}
/**************************/
File_system::~File_system()	{rmdir("V/");}// this call will clear all sub directories of root 'V/' including deleting dynamically allocated files within.
/**************************/
File* File_system::operator [](string filename)	{
  
	// iterating through the whole system's directories
	// looking for a file named after filename.
	for( auto it1 = system.begin() ; it1 != system.end(); it1++ )	{
     if( (*it1).second.size() == 0 )  { continue; }
     for( auto file : (*it1).second )  {
       if( file->filename == filename )  {return file;}
     }
	}

	// couldnt find a file named after filename.
	return nullptr;
}
/**************************/
std::vector<File*>::iterator File_system::operator[](File *file)	{

	auto it1 = system.begin() ;
	auto it2 = (*it1).second.begin();

	// iterating through the whole system's directories
	// looking for the file that is named after filename.
	for( ; it1 != system.end(); it1++ )	{
		for( ; it2 != (*it1).second.end(); it2++ )	{
			if( *it2 ==  file )	{
				break;
			}
		}
	}

	return it2;
}
/**************************/
void ftouch(const char* file)	{
    ofstream xfile;
    xfile.open(file);
    xfile.flush();
    xfile.close();
}
/******************************/
void File_system::touch(string filename)	{
  
	/**A file named after filename exists in system. update file's time stamp and return.**/
	if( (*this)[filename] != nullptr )	{
  
		// save content from file (flush might remove content)
		(*this)[filename]->file_guard->raw_file->open((*this)[filename]->file_guard->raw_filename.c_str(),ios::in);
		string buffer;
		string content = "";
		while( getline(*(*this)[filename]->file_guard->raw_file,buffer) )	{content += buffer; content += '\n';}
		(*this)[filename]->file_guard->raw_file->close();

		// update time stamp
		ftouch((*this)[filename]->file_guard->raw_filename.c_str());

		// rewrite original content back to file
		(*this)[filename]->file_guard->raw_file->open((*this)[filename]->file_guard->raw_filename.c_str(),ios::out);
		(*this)[filename]->file_guard->raw_file->write(content.c_str(),content.size());
		(*this)[filename]->file_guard->raw_file->close();

		return;
	}

	/**otherwise, insert a new file named after filename to a directory in the system as the following instructions: **/
	else	{
  
		// retrieve the index of where the path symbol is first seen (from end to beginning).
		unsigned int where_dirname_ends;
		unsigned int i;
		for(i = filename.length() - 1; i >= 0; i--)	{
			if( filename[i] == '/' )	{break;}
      if( i == 0 ) { break; }
		}

		//1. if filename doesn't include absolute path. as default, insert to current directory and return.
		if( i == 0 )	{
			std::vector<File*>& current = system.at(current_dir);
			current.push_back(new File(filename,current_dir));
			return;
		}

		//2. filename includes absolute path. retrieve path and lookup for specific directory in system.
		//   if doesnt exists, a defaultive insertion to current directory will be used
		//	 (hence that the function doesn't throw any exceptions by definition).
		where_dirname_ends = i;
		string dir_name = "";

		// parse both filename and absolute path.
		for(i = 0; i < filename.length(); i++)	{
			if( i <= where_dirname_ends )	{dir_name += filename[i];}
		}

		// look up for given path in system.
		try	{
			std::vector<File*>& dir = system.at(dir_name);
			dir.push_back(new File(filename,dir_name));
		}

		// given path doesn't exist in file. insert in current directory instead.
		catch(exception& e)	{
			std::vector<File*>& current = system.at(current_dir);
			current.push_back(new File(filename,current_dir));
			return;
		}
	}
}
/******************************/
void File_system::copy(std::string src_file, std::string dest_file) throw(File::FileNotFoundException)	{

	// Files are identical. no need to copy
	if( src_file == dest_file ) 	{return;}

	// fetch both files to preform a copy.
	File *src_File = (*this)[src_file];
	File *dest_File = (*this)[dest_file];

	// no such source file. a copy cannot be performed.
	if( src_File == nullptr )	{ throw File::FileNotFoundException(); }

	/**retrieve content from source file**/
	string buffer;
	string content = "";
	src_File->file_guard->raw_file->open(src_File->file_guard->raw_filename.c_str(),ios::in);
	while( getline(*src_File->file_guard->raw_file,buffer ) )	{
		content += buffer;
		content += '\n';
	}

	/**clear flags and close file**/
	src_File->file_guard->raw_file->clear();
	src_File->file_guard->raw_file->close();

	// destination file doesn't appear anywhere in files system.
	// a new file will be created and inserted into proper directory
	// ( conditions for file-to-directory compatibility appear in touch function ).
	if( dest_File == nullptr )	{
		touch(dest_file);
		dest_File = (*this)[dest_file];
	}

	/**write content to destination file (in case of existing file)**/
	dest_File->file_guard->raw_file->open(dest_File->file_guard->raw_filename.c_str(),ios::out);
	dest_File->file_guard->raw_file->write(content.c_str(),content.size());
	dest_File->file_guard->raw_file->close();

}
/******************************/
void File_system::remove(string filename) throw(File::FileNotFoundException)	{

	// retrieve wished file pointer.
	File* to_remove = (*this)[filename];

	if( to_remove == nullptr )	{throw File::FileNotFoundException();}

	// clear file's content (for after use)
	if( (*this)[filename]->file_guard->refCount == 1 )	{
		(*this)[filename]->file_guard->raw_file->open((*this)[filename]->file_guard->raw_filename.c_str(),ios::out);
		(*this)[filename]->file_guard->raw_file->write("",0);
		(*this)[filename]->file_guard->raw_file->close();
	}

	// remove the file from both file system and heap ( all files are dynamically allocated ).
	(*system.find((to_remove)->dir_name)).second.erase((*this)[to_remove]);
	delete to_remove;
}
/******************************/
void File_system::move(string src_file, string dest_file) throw(File::FileNotFoundException)	{

	// file named after src_file couldnt be found.
	if( (*this)[src_file] == nullptr )	{throw File::FileNotFoundException();}

	// both src_file and dest_file are hard linked.
	// no need to move content from source to destination,
	// but only to remove src_file.
	if( (*this)[dest_file] != nullptr )	{
		if( (*this)[dest_file]->file_guard->raw_filename == (*this)[src_file]->file_guard->raw_filename )	{
			remove(src_file);
			return;
		}
	}

	copy(src_file,dest_file); // copies content from source to destination file
	remove(src_file);		  // deletes source file
}
/******************************/
bool File_system::startswith(string root, string sub_root) const	{ // this function is used for spotting if a directory which is named after
																	  // sub_root is a sub directory of a directory named after root.

	if( sub_root.length() < root.length() )	{return false;}

	for(unsigned int i = 0; i < root.length(); i++)	{
		if( root[i] != sub_root[i] )	{return false;}
	}

	return true;
}
/******************************/
void File_system::rmdir(const string& dir_name) throw(NoSuchDirectory)	{ // Assuming a dirname given as absolute path.

	int detected = 0; // counting number of sub directories to dir_name found.

	// dir_name is root.
	// system will be cleared and files in all directories (including directories
	// which are not root) will be deleted.
	if( dir_name == "V/" )	{
		for(auto it1 = system.begin(); it1 != system.end(); )	{
			for( auto file : (*it1).second )	{delete file;}
      (*it1).second.clear();
			if( (*it1).first != "V/" )	{ system.erase(it1++); }
			else	{++it1;}
		}
		// moving to root directory as default.
		chdir("V/");
		return;
	}

	// looking for all paths that starts with given directory name
	// and erasing them. for that to happen, all given directories must include full path.
	for( auto it1 = system.begin(); it1 != system.end(); )	{
		if( startswith(dir_name,(*it1).first) )	{
			for( auto file : (*it1).second )	{delete file;}
			system.erase(it1++);
			++detected;
		}
		else	{++it1;}
	}

	// moving to root directory as default.
	chdir("V/");

	// in case that there was no such directory. throw a proper exception.
	if( detected == 0 )	{throw NoSuchDirectory();}
}
/******************************/
void File_system::chdir(const string& dir_name) throw(NoSuchDirectory)	{ // Assuming that an absolute path should be given as argument. otherwise,
																		  // it will be considered as non existing.

	if( dir_name[dir_name.length() - 1] != '/' )	{const_cast<string&>(dir_name) += '/';}

	try	{
		system.at(dir_name); // will throw out_of_range exception if there's no such directory.
		current_dir = dir_name;
	}
	catch( exception& e )	{throw NoSuchDirectory();}
}
/******************************/
void File_system::mkdir(string& dir_name) throw(DirectoryExists,RootException)	{ // assuming that an absolute path must be given to system in order for
																		  	  	  	  	  // other directory interface functions to work.

	if( dir_name[dir_name.length()-1] != '/' )	{ dir_name += '/'; }

	// if there's a directory named after given dir_name
	if( system.find(dir_name) != system.end() )	{throw DirectoryExists();}

	// all directories must be sub_roots of V/
	if( !startswith("V/",dir_name) )	{throw RootException();}

	system.insert({dir_name,std::vector<File*>{}});
}
/******************************/
void File_system::ls() const	{

	cout << current_dir << ":" << endl;
	for(unsigned int i = 0; i < system.at(current_dir).size(); i++)	{
		cout << system.at(current_dir)[i]->toString() << endl;
	}
}
/******************************/
void File_system::lproot() const	{

	// retrieve all system directories names and
	// sort them by hierarchy related to root.
	std::vector<string> keys{};
	for( auto it = system.begin(); it != system.end(); it++ )	{keys.push_back( (*it).first );}
	std::sort(keys.begin(),keys.end(),[](string a, string b)->bool{return std::count(a.begin(),a.end(),'/') < std::count(b.begin(),b.end(),'/');});

	// print all files in system by hierarchy (top down).
	for(unsigned int i = 0; i < keys.size(); i++ )	{
		cout << keys[i] << ":" << endl;
		for(unsigned int j = 0; j < system.at(keys[i]).size(); j++)	{
			cout << system.at(keys[i])[j]->toString() << endl;
		}
		cout << endl;
	}
}
/******************************/
void File_system::pwd() const	{ cout << current_dir << endl; }
/******************************/

#Terminal 

Student name: Omri bar-haim
Student ID: 315696161

Few important details for mini-terminal usage:
----------------------------------------------

1. All directories inputs must include full path in order 
   for the program to run as expected.

2. When using lproot / ls for printing the system's files status, two files
   that are hard link are holding the same reference counter, hence the counter
   for them both will be 2 or more (if more files are hard linked too).
   The logic behind this implementation is that both files are holding 
   a pointer to the same content, even though their names are different. 

3. New files ( made by 'touch' command ) will be inserted to 
   a directory in the system according to full path, if exists.
   if a given path only contains the filename or the directory 
   does not exist, it will be inserted into current pointed directory.

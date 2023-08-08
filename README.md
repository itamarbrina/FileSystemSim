# FileSystemSim
Illustration of how the computer's disk file system works.

## Description

The program gives an illustration of how the computer's disk file system works.<br>
The program does it by getting random commands from the user and performing simple tasks such as:
<ol>
 	<li> print the disk content.</li>
 	<li> disk format.</li>
 	<li> creating files.</li>
 	<li> open files.</li>
 	<li> close files</li>
 	<li> write to file.</li>
 	<li> read to file.</li>
 	<li> delete file.</li>
</ol>
	
#### Note that :
<ol>
	<li> The program doesn't actually write to the real disk, it uses a file with the name: "DISK_SIM_FILE.txt" as a disk that should be.</li>
	<li> The simulator uses only one Directory called MainDir.</li>
</ol>

#### In this program, we use :

->The following Data structure/objects: <br>
<ul>
		<li> FsFile - holds basic information about the file, such as :
				 file size, amount of blocks that the file uses currently, the location of the index block, and the size of the block.
			  This object contains the following functions:
        <ol>
			    <li> get/set_index_block - allows access to the private variable from other departments.</li>
  			  <li> getfile_size - allows access to the variable that holds the size of the file from other functions.</li>
  			  <li> internal_block_space - check if there is free space in the least block.</li>
  			  <li> external_block_space - check if the index block is full.</li>
  			  <li> add_block - adding a block to the count of blocks.</li>
  			  <li> add_writing - increase the size of the file by 1 for every writing.</li>
        </ol>
    </li>
		<li> FileDescriptor - attached between the name of the file and the FsFile.
			in addition, hold a boolean variable 'inUse' that shows if the file opens or close
			The object contains the following functions:
      <ol>
				  <li> FileDescriptor - The constructor of the Object - accepts the file name, and the FsFile, and connects them.</li>
				  <li> getFileName/getFileInfo/getInUse/setInUse - allows access to the private variable from other departments.</li>
				  <li> ~FileDescriptor - The destructor of the Object.</li>
      </ol>
    </li>
		<li>fsDisk - The main Object that presents the file system, contains all the functionalities that the user can perform.
			This object holds the main data structures:
      <ol>
  			<li> BitVector - an array that shows which blocks are currently in use.</li>
  			<li> MainDir - The main directory is Map of FileDescriptor using the fileName as the key - this data struct will contain all the files that currently exist.</li>
  			<li> OpenFileDescriptor - This array holds the FileDescriptor of the open files only.</li>
  			<li> block_size - holds the size of one block.</li>
      </ol>
		This Object contains all the functions that appear right below:
    </li>
</ul>

#### functions:
There are nine main functions:
<ul>
 	<li> fsDisk - The constructor of the class - its purpose is to initialize all the values and open the disk file correctly.</li>
	<li> listAll - the purpose of this function is to print all the disk content.</li>
	<li> fsFormat - simulate the format of the disk, init the disk content, then init the block size, and define the Bit Vector, MainDir, and OpenFileDescriptor.</li>
	<li> CreateFile - The createFile function opens a new file in the simulator (if it is possible).  the function gets the name of the file from the user.
			The function returns the number of fd in success  and -1 in failure.</li>
	<li> OpenFile - This function gets the name of a file and tries to open it (add it to the OpenFileDescriptor).</li>
	<li> CloseFile - This function gets a fd of file and closes him (remove it from the OpenFileDescriptor).</li>
	<li> WriteToFile - This function accepts the fd of a file, a string to write, and the size of characters to write, and writes these characters from the string to the file.</li>
	<li> ReadFromFile - This function accept fd, a buffer that contains the reading, length to read, and read from a file.</li>
	<li> DelFile - This function accepts the name of a file and deletes it.</li>
</ul>

There are several additional functions:
<ul>
	<li> ~fsDisk - The destructor- his purpose is to free all the memory before losing the connection to the object.</li>
	<li> find_empty_block - This method helps to find empty blocks that are not currently in use.</li>
	<li> MainDirFreer - frees the MainDir map and all the content inside it.</li>
	<li> OpenFileDescriptorFreer - frees the OpenFileDescriptor array.</li>
	<li> find_emptyIn_OpenFileDescriptor - responsible to find empty entries in the Open files array.</li>
	<li> is_in - This method except the file name and an array of file descriptors and checks if the file is in the array.</li>
</ul>

## Program files
The program itself:  "FinalProject.cpp".<br>
The program uses external files: <br>
	"DISK_SIM_FILE.txt"  - the file that simulates the disk content.<br>

## How to compile?
two options are possible:<br>
at first, the user needs to create a main file (the main.cpp is an example of that file) that includes sim_mem.h library, then:<br>
1. compile through clion compiler (or other c editor) and use through the console.
2. compile through terminal using g++ FinalProject.cpp <main_name>.cpp -o main.
   and then: ./main.

// Ex6 - Disk File System simulator:
/**
 * The program simulates a disk file system in the computer.\n
 * This simulator will show how the the files saved and organized in the disk.\n
 * The simulator works with index block method - with only one block and allows to use :\n
 * 1. print the disk content.
 * 2. disk format.
 * 3. creating files.
 * 4. open files.
 * 5. close files
 * 6. write to file.
 * 7. read to file.
 * 8. delete file.

@author Itamar Brina*/
#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <cstring>
#include <cmath>
#include <fcntl.h>

using namespace std;

#define DISK_SIZE 256


/**This function turns integer number to binary.
 * @param n is the number you want to turn into binary number.
 * @param c will hold the binary value of n.*/
void decToBinary(int n, char &c)
{
    // array to store binary number
    int binaryNum[8];

    // counter for binary array
    int i = 0;
    while (n > 0)
    {
        // storing remainder in binary array
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }

    // printing binary array in reverse order
    for (int j = i - 1; j >= 0; j--)
    {
        if (binaryNum[j] == 1)
            c = c | 1u << j;
    }
}


/**The class represent a file's DB that save basic information about the file.\n*/
class FsFile {
    /*var that contains the size of the file in chars:*/
    int file_size;
    /*option to save the number of the blocks that are currently in use by the "Disk":*/
    int block_in_use;
    /*var that holds the location of the index block in the "Disk" file:*/
    int index_block;
    /*var that contains the number of chars that can be saved in one block:*/
    int block_size;

public:

    /**default constructor , with the purpose of initialize the different vars in the class.
     * @param _block_size the size of characters that could be saved in one block of the file. */
    FsFile(int _block_size){
        file_size = 0;
        block_in_use = 0;
        block_size = _block_size;
        index_block = -1;
    }


    /**this method only return the size of the file in bytes.*/
    int getfile_size() const{
        return file_size;
    }

    /**this method only return the size of the file in bytes.*/
    int get_index_block() const{
        return index_block;
    }

    /**this method set the index_block to the new value:
     * @param new_value is the new value of index_block.*/
    void set_index_block(int new_value){
        if(new_value >= 0)
            index_block = new_value;
    }

    /**The internal_block_space method checks if there is available place in the blocks that already allocated.
     * @return true if there is space , false if isn't.*/
    bool internal_block_space () const {
        if(file_size < (block_size * block_in_use))
            return true;
        return false;
    }


    /**The internal_block_space method checks if there is available place in the index_block.
     * @return true if there is space , false if isn't.*/
    bool external_block_space () const {
        if (block_in_use == block_size)
            return false;
        return true;
    }

    /**Need to use that function when adding block to the index block:*/
    void add_block(){
        block_in_use++;
    }

    /**Need to use that function after writing every char to the file:*/
    void add_writing(){
        file_size++;
    }
};


/**The class FileDescriptor gathered the information of file and the file it self.\n
 * in addition holds boolean that shows if the file currently in use.*/
class FileDescriptor {
    string file_name;
    FsFile* fs_file;
    bool inUse;

public:

    FileDescriptor(string FileName, FsFile* fsi) {
        file_name = FileName;
        fs_file = fsi;
        inUse = true;
    }


    string getFileName() {
        return file_name;
    }


    FsFile* getFileInfo() {
        return fs_file;
    }


    bool getInUse() const {
        return inUse;
    }


    void setInUse(bool inUseVal) {
        this->inUse = inUseVal;
    }


    ~FileDescriptor(){
        delete(fs_file);
    }
};


#define DISK_SIM_FILE "DISK_SIM_FILE.txt"
#define SIZE 256


/**The fsDisk class intended to represent the disk itself. */
class fsDisk {

    FILE *sim_disk_fd;
    bool is_formated;

    /*the BitVector show which blocks are variable:*/
    int BitVectorSize;
    int *BitVector;

    /*var that contains the number of chars that can be saved in one block:*/
    int block_size;

    /*The main dir will contain all the files:*/
    map<string , FileDescriptor*> MainDir;

    /*The OpenFileDescriptor will contain open files:*/
    FileDescriptor ** OpenFileDescriptor;

public:

    /**The default constructor, the constructor initialize the values of all the class's fields.*/
    fsDisk() {
        /*open the file that will be the "Disk":*/
        sim_disk_fd = fopen(DISK_SIM_FILE , "r+");
        /*checks if the file is not NULL:*/
        assert(sim_disk_fd);


        for (int i=0; i < DISK_SIZE ; i++) {
            int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
            ret_val = (int)fwrite( "\0" ,  1 , 1, sim_disk_fd);
            /*the return value should be 1 , else the function failed:*/
            assert(ret_val == 1);
        }

        /*make sure that the write to the file have been performed:*/
        fflush(sim_disk_fd);
        is_formated = false;

        BitVector = nullptr;
        OpenFileDescriptor = nullptr;
        BitVectorSize = 0;
        block_size = 4;
    }


    /**print function:*/
    void listAll() {
        map<string , FileDescriptor*>:: iterator itr;
        int i = 0;
        for (itr = MainDir.begin() ; itr != MainDir.end() ; itr++ ) {
            cout << "index: " << i << ": FileName: " << itr->second->getFileName() << " , isInUse: "
                 << itr->second->getInUse() << endl;
            i++;
        }

        char bufy;
        cout << "Disk content: '";
        for (i = 0; i < DISK_SIZE; i++)
        {
            cout << "(";
            int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
            assert(ret_val >= 0);
            ret_val = (int)fread(&bufy, 1, 1, sim_disk_fd);
            assert(ret_val >= 0);
            cout << bufy;
            cout << ")";
        }
        cout << "'" << endl;
    }


    /**fsFormat function perform disk formatting in the simulator.
     * @param blockSize is the size of block in the disk that will be define during the formatting. the default value is 4.
     * @result the disk's content will erase and the disk will show that he passed formatting.*/
    void fsFormat( int blockSize =4 ) {
        if (blockSize <= 1){
            /*turn it to the default size:*/
            blockSize = 4;
        }
        /*init the value so if the function stopped from which reason, the disk show that it doesn't formatted:*/
        is_formated = false;

        printf ("Formatting...\n");

        /*erasing the currently existing files on the disk:*/
        for (int i=0; i < DISK_SIZE ; i++) {
            int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
            ret_val = (int)fwrite( "\0" ,  1 , 1, sim_disk_fd);
            /*the return value should be 1 , else the function failed:*/
            assert(ret_val == 1);
        }

        /*make sure that the write to the file have been performed:*/
        fflush(sim_disk_fd);

        /*initialize vars values:*/
        this->block_size = blockSize;
        printf ("The disk have been formatted successfully , block size is : %d\n" , blockSize);

        /*creates the bit vector:*/
        if (BitVector != nullptr)
            delete[](this->BitVector);

        BitVectorSize = SIZE/blockSize;
        BitVector = new int[BitVectorSize];
        /*init the Bit Vector:*/
        for (int i = 0 ; i < BitVectorSize ; i++)
            BitVector[i] = 0;

        /*free the main directory :*/
        MainDirFreer();

        /*declare the OpenFileDescriptor array:*/
        OpenFileDescriptorFreer();
        OpenFileDescriptor = new FileDescriptor*[BitVectorSize];
        for (int i = 0 ; i < BitVectorSize ; i++)
            OpenFileDescriptor[i] = nullptr;


        /*finish with declare the disk as formatted:*/
        is_formated = true;
    }


    /**The createFile function open a new file in the simulator (if it possible) .
     * @param fileName is the name of the file the user want to open.
     * @return The function return the number of fd in success  and -1 in failure
     * @result If the function succeed than the new file will be create and an index block will \n
     *          allocates for him , in addition the file will appear in the main dir and in the open files vector.*/
    int CreateFile(string fileName) {
        /*checking if the disk pass format:*/
        if(!is_formated){
            fprintf(stderr , "The disk isn't formatted.\n");
            return -1;
        }

        /*check if the file already exist:*/
        map<string , FileDescriptor*>:: iterator itr;
        itr = MainDir.find(fileName);
        if(itr != MainDir.end()){
            fprintf(stderr, "The file already exist!\n");
            return -1;
        }

        int empty_block = find_empty_block();
        int empty_FD_entry = find_emptyIn_OpenFileDescriptor();

        if (empty_block == -1){
            fprintf(stderr , "The disk is full, cannot open another file!\n");
            return -1;
        }
        if(MainDir.size() >= BitVectorSize){
            fprintf(stderr , "The Main Directory is Full!\n");
            return -1;
        }
        if (empty_FD_entry == -1){
            fprintf(stderr, "Fatal error!! The open files vector is full!\n");
            return -1;
        }


        /*creates FsFile:*/
        FsFile *file_info;
        file_info = new FsFile(block_size);

        /*open a fd (connect the name of the file and the information of the file)*/
        FileDescriptor* fd;
        fd = new FileDescriptor(fileName ,file_info);

        /*add to main dir:*/
        MainDir.insert(pair<string,FileDescriptor*> (fileName , fd));

        /*add to open files vector:*/
        OpenFileDescriptor[empty_FD_entry] = fd;
        return empty_FD_entry;
    }


    /**The OpenFile function accept a file name and trying to open this file.\n
     * In other words, the function simulate file opening, like in the computer, there we use double
     * click or other acts to operate the same thing.
     * @param fileName - The name of the file we want to open
     * @return -1 if the function failed , other wise the function will return the fd of the file.*/
    int OpenFile(string fileName) {

        /*checking if the disk pass format:*/
        if(!is_formated){
            fprintf(stderr , "The disk isn't formatted.\n");
            return -1;
        }

        /*checking if the file is existing:*/
        map<string , FileDescriptor*>:: iterator itr;
        itr = MainDir.find(fileName);
        if (itr == MainDir.end()){
            fprintf(stderr , "The file is not exist!\n");
            return -1;
        }
        /*checking if the file is already open:*/
        if (is_in(fileName,OpenFileDescriptor) >= 0){
            cout << "The file '"<< fileName << "' is already exist!" << endl;
            return -1;
        }

        /*add to open files vector:*/
        int empty_FD_entry = find_emptyIn_OpenFileDescriptor();
        OpenFileDescriptor[empty_FD_entry] = itr->second;
        OpenFileDescriptor[empty_FD_entry]->setInUse(true);
        return empty_FD_entry;
    }


    /**The CloseFile function accept a fd and trying to close this file.\n
     * In other words, the function simulate file closing, like in the computer, there we use 'X' button
     * or other acts to operate the same thing.
     * @param fd - The fd of the file we want to close.
     * @return -1 if the function failed , other wise the function will return the fd of the file.*/
    string CloseFile(int fd) {
        /*checking if the disk pass format:*/
        if(!is_formated){
            fprintf(stderr , "The disk isn't formatted.\n");
            return "-1";
        }

        /*checking if the file is already open:*/
        if (OpenFileDescriptor[fd] == nullptr){
            fprintf(stderr , "The file not found.\n");
            return "-1";
        }

        /*save the file name:*/
        string fileName = OpenFileDescriptor[fd]->getFileName();
        /*remove the file from the open file vector:*/
        OpenFileDescriptor[fd]->setInUse(false);
        OpenFileDescriptor[fd] = nullptr;

        /*calculate the return string:*/
        return fileName;
    }


    /**This function simulate writing to existing file.\n
     * The writing could preform only if the disk have formatted and the file is exist and open.
     * @param fd the location fo the file in the open files vector.
     * @param buf is the buffer that holds the content that the user want to write.
     * @param len is the amount of chars the user want to write.
     * @return in failure the function return -1 , in success the function return the amount of characters that written.*/
    int WriteToFile(int fd, char *buf, int len ) {
        /*checking if the disk pass format:*/
        if(!is_formated){
            fprintf(stderr , "The disk isn't formatted.\n");
            return -1;
        }

        /*checking if the file is already open:*/
        if (OpenFileDescriptor[fd] == nullptr){
            fprintf(stderr , "The file not found.\n");
            return -1;
        }

        /*checking if the length is valid:*/
        if(len > strlen(buf)){
            fprintf(stderr , "The length of the buffer is not valid!\n");
            return -1;
        }

        /*creating index block if needed:*/
        if (OpenFileDescriptor[fd]->getFileInfo()->get_index_block() == -1) {
            int empty_block = find_empty_block();
            if (empty_block == -1){
                fprintf(stderr , "The disk is full, cannot allocate index_block!\n");
                return -1;
            }
            OpenFileDescriptor[fd]->getFileInfo()->set_index_block(empty_block);
            /*set the block to be taken*/
            BitVector[empty_block] = 1;
        }


        /*writing the information:*/
        int i = 0;
        while (i < len) {
            /*location will hold the address in the index block we want to read:*/
            int location = OpenFileDescriptor[fd]->getFileInfo()->get_index_block() * block_size;
            location += OpenFileDescriptor[fd]->getFileInfo()->getfile_size() / block_size;

            /*check if there is space in the allocated blocks:*/
            if (OpenFileDescriptor[fd]->getFileInfo()->internal_block_space()) {
                /*reading the address in the index block:*/
                int ret_val = fseek(sim_disk_fd, location, SEEK_SET);
                assert(ret_val >= 0);
                char buffer = 0x00;
                ret_val = (int)fread(&buffer, 1, 1, sim_disk_fd);
                assert(ret_val == 1);

                /*get the address that we can write to:*/
                location = (int) (buffer - '0') * block_size;
                location += OpenFileDescriptor[fd]->getFileInfo()->getfile_size() % block_size;

                /*writing the data*/
                ret_val = fseek(sim_disk_fd, location, SEEK_SET);
                assert(ret_val >= 0);
                ret_val = (int)fwrite( &(buf[i]) ,  1 , 1, sim_disk_fd);
                assert(ret_val == 1);

                /*make sure that the writing to the file have been performed:*/
                fflush(sim_disk_fd);

                /*update the DB:*/
                OpenFileDescriptor[fd]->getFileInfo()->add_writing();

                i++;
            }
            else{
                /*checks if there is enough space in the index block for the new block:*/
                if(!OpenFileDescriptor[fd]->getFileInfo()->external_block_space()){
                    fprintf(stderr , "Not enough space in the file!\n");
                    break;
                }

                int empty_block = find_empty_block();
                /*checks if there is enough space in the disk for the new block:*/
                if(empty_block == -1){
                    fprintf(stderr , "Not enough space in the disk!\n");
                    break;
                }

                /*allocate the block to the file:*/
                BitVector[empty_block] = 1;
                char binary_empty_block = 0x00;
                decToBinary(empty_block + '0' , binary_empty_block);

                /*write to the index block:*/
                int ret_val = fseek(sim_disk_fd, location, SEEK_SET);
                assert(ret_val >= 0);
                ret_val = (int)fwrite( &binary_empty_block ,  1 , 1, sim_disk_fd);
                assert(ret_val == 1);

                /*make sure that the writing to the file have been performed:*/
                fflush(sim_disk_fd);

                /*update the DB:*/
                OpenFileDescriptor[fd]->getFileInfo()->add_block();
            }
        }

        return i;
    }


    /**This function simulate reading from existing file.\n
    * The reading could preform only if the disk have formatted and the file is exist and open.
     * @param fd the location fo the file in the open files vector.
     * @param buf is the buffer that holds the content that the user want to read.
     * @param len is the amount of chars the user want to read.
     * @return in failure the function return -1 , in success the function return the amount of characters that have reading \n
     * and the result will store in buf.*/
    int ReadFromFile(int fd, char *buf, int len ) {
        /*init the buffer:*/
        buf[0] = '\0';

        /*checking if the disk pass format:*/
        if(!is_formated){
            fprintf(stderr , "The disk isn't formatted.\n");
            return -1;
        }

        /*checking if the file is already open:*/
        if (OpenFileDescriptor[fd] == nullptr){
            fprintf(stderr , "The file not found.\n");
            return -1;
        }

        /*checking if the length is valid:*/
        if(len > OpenFileDescriptor[fd]->getFileInfo()->getfile_size()){
            fprintf(stderr , "The length of the buffer is not valid!\n");
            return -1;
        }

        /*init the buffer:*/
        buf[len] = '\0';

        /*performing the reading:*/
        int i = 0;
        while(i < len){
            /*location will hold the address in the index block we want to read:*/
            int location = OpenFileDescriptor[fd]->getFileInfo()->get_index_block() * block_size;
            location += i / block_size;
            /*read the index block:*/
            int ret_val = fseek(sim_disk_fd, location, SEEK_SET);
            assert(ret_val >= 0);
            char buffer = 0x00;
            ret_val = (int)fread(&buffer, 1, 1,sim_disk_fd);
            assert(ret_val == 1);
            /*change the location according to the result:*/
            location = (int)(buffer - '0') * block_size;
            location += i%block_size;
            /*reading the file:*/
            ret_val = fseek(sim_disk_fd, location, SEEK_SET);
            assert(ret_val >= 0);
            ret_val = (int)fread(&buf[i], 1, 1,sim_disk_fd);
            assert(ret_val == 1);
            i++;
        }
        return i;
    }


    /**This function delete file in the simulator and free all the resources that file use.
     * @param FileName - is the name of the file we want to the delete.
     * @return if the function failed the return value will be -1, other wise will be 1.*/
    int DelFile( string FileName ) {
        /*checking if the disk pass format:*/
        if(!is_formated){
            fprintf(stderr , "The disk isn't formatted.\n");
            return -1;
        }

        /*try to find the file in the main directory:*/
        map<string , FileDescriptor*>:: iterator itr;
        itr = MainDir.find(FileName);
        if (itr == MainDir.end()){
            fprintf(stderr, "The file doesn't found!\n");
            return -1;
        }
        if(itr->second->getInUse() == 1){
            /*if the file is open need to close him first:*/
            for(int i = 0 ; i < BitVectorSize ; i++)
                if(OpenFileDescriptor[i]!= nullptr && OpenFileDescriptor[i]->getFileName() == FileName){
                    string res = CloseFile(i);
                    /*check the result*/
                    if (res == "-1"){
                        fprintf(stderr , "fatal error cannot close the file , the function failed!\n");
                        return -1;
                    }
                    break;
                }
        }
        /*delete the blocks that the file use:*/
        int index_block_location =  itr->second->getFileInfo()->get_index_block() * block_size;
        int file_size = itr->second->getFileInfo()->getfile_size();
        int ret_val;

        if(file_size != 0){
            for (int i = 0 ; i <=((file_size-1) / block_size) ; i++){
                unsigned char buf = 0x00;
                ret_val = fseek(sim_disk_fd, index_block_location+i, SEEK_SET);
                assert(ret_val >= 0);
                ret_val = (int)fread(&buf,1,1,sim_disk_fd);
                assert(ret_val == 1);
                BitVector[(int)buf-'0'] = 0;
                int block_location = (int)(buf -'0') * block_size;
                for(int j=0 ; j<block_size ; j++){
                    ret_val = fseek(sim_disk_fd, block_location+j, SEEK_SET);
                    assert(ret_val >= 0);
                    ret_val = (int)fwrite( "\0" ,  1 , 1, sim_disk_fd);
                    assert(ret_val == 1);
                    fflush(sim_disk_fd);
                }
                ret_val = fseek(sim_disk_fd, index_block_location+i, SEEK_SET);
                assert(ret_val >= 0);
                ret_val = (int)fwrite( "\0" ,  1 , 1, sim_disk_fd);
                assert(ret_val == 1);
                fflush(sim_disk_fd);
            }
        }

        /*delete the index block and the file from main dir:*/
        BitVector[itr->second->getFileInfo()->get_index_block()] = 0;
        delete(itr->second);
        MainDir.erase(itr);
        return 1;
    }


    /**The destructor method is responsible to free all the allocated memory in the end of the
     * running.*/
    ~fsDisk(){

        /*free the BitVector:*/
        delete[](this->BitVector);


        /*erasing the currently existing files on the disk:*/
        for (int i=0; i < DISK_SIZE ; i++) {
            int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
            ret_val = (int)fwrite( "\0" ,  1 , 1, sim_disk_fd);
            /*the return value should be 1 , else the function failed:*/
            assert(ret_val == 1);
        }

        /*make sure that the write to the file have been performed:*/
        fflush(sim_disk_fd);

        fclose(sim_disk_fd);

        /*free the OpenFileDescriptor*/
        OpenFileDescriptorFreer();

        /*free the MainDir:*/
        MainDirFreer();
    }

private:

    /**The find_empty_block method find empty block that not currently in use.
     * @return the empty block index in BitVector array, or -1 if there isn't available block.*/
    int find_empty_block(){
        for (int i = 0 ; i < BitVectorSize ; i++)
            if(BitVector[i] == 0)
                return i;
        return -1;
    }

    /**This method free the MainDir array:*/
    void MainDirFreer (){
        map<string , FileDescriptor*>:: iterator itr;
        for (itr = MainDir.begin() ; itr != MainDir.end() ; itr = MainDir.begin()) {
            delete(itr->second);
            MainDir.erase(itr);
        }
    }

    /**This method free the OpenFileDescriptor*/
    void OpenFileDescriptorFreer(){
        if (OpenFileDescriptor != nullptr) {
            delete [](OpenFileDescriptor);
        }
    }

    /**The find_emptyIn_OpenFileDescriptor method responsible to find empty entry in the Open files array.
*   @return the free index in the array or -1.*/
    int find_emptyIn_OpenFileDescriptor (){
        for (int i = 0 ; i < BitVectorSize ; i++){
            if((OpenFileDescriptor[i]) == nullptr)
                return i;
        }
        return -1;
    }

    /**The method is_in except file name and array of file descriptor and checks if the file is in the array.
     * @param file_name -the name of the file
     * @param array -the array of file descriptors.
     * @return the index in the array that the file in if the file is in the array, -1 otherwise.*/
    int is_in (string & file_name , FileDescriptor ** array) const{
        for(int i = 0 ; i < BitVectorSize ; i++){
            if(array[i]!=nullptr && array[i]->getFileName()==file_name)
                return i;
        }
        return -1;
    }

};

/**
 * The program simulates a disk file system in the computer.\n
 * This simulator will show how the the files saved and organized in the disk.\n
 * The simulator works with index block method - with only one block.
 * */
int main() {
    int blockSize;
    int direct_entries;
    string fileName;
    char str_to_write[DISK_SIZE];
    char str_to_read[DISK_SIZE];
    int size_to_read;
    int _fd;

    fsDisk *fs = new fsDisk();
    int cmd_;
    while(1) {
        cin >> cmd_;
        switch (cmd_)
        {
            case 0:   // exit
                delete fs;
                exit(0);
                break;

            case 1:  // list-file
                fs->listAll();
                break;

            case 2:    // format
                cin >> blockSize;
                fs->fsFormat(blockSize);
                break;

            case 3:    // creat-file
                cin >> fileName;
                _fd = fs->CreateFile(fileName);
                cout << "CreateFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 4:  // open-file
                cin >> fileName;
                _fd = fs->OpenFile(fileName);
                cout << "OpenFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 5:  // close-file
                cin >> _fd;
                fileName = fs->CloseFile(_fd);
                cout << "CloseFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 6:   // write-file
                cin >> _fd;
                cin >> str_to_write;
                _fd = fs->WriteToFile( _fd , str_to_write , strlen(str_to_write) );
                cout << "The amount of characters that writing :"<< _fd << endl;
                break;

            case 7:    // read-file
                cin >> _fd;
                cin >> size_to_read ;
                _fd = fs->ReadFromFile( _fd , str_to_read , size_to_read );
                cout << "ReadFromFile: " << str_to_read << endl;
                cout << "The amount of characters that reading :"<< _fd << endl;
                break;

            case 8:   // delete file
                cin >> fileName;
                _fd = fs->DelFile(fileName);
                cout << "DeletedFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;
            default:
                break;
        }
    }

}

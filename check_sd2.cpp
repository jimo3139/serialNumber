#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <list>
#include <string.h>
#include <errno.h>
#include <iomanip>
#include <sstream>

using namespace std;

bool DoesDirectoryExist(const string& strName)
{
	if(strName.empty())
	{
		cout<<"ERR012:, Path to Directory is empty = "<<strName<<endl;
		return false;
	}
	struct stat st;

	if ((stat(strName.c_str(), &st) == 0) && (((st.st_mode) & S_IFMT) == S_IFDIR))
	{	     
		//cout<<"Path to Directory is = "<<strName<<endl;
		//cout <<"stat mode="<<st.st_mode << endl;
		//cout <<"device ID="<<st.st_dev << endl;
		//cout <<"file serial number="<<st.st_ino << endl;
		//cout <<"nlink="<<st.st_nlink << endl;
		//cout <<"uid="<<st.st_uid << endl;
		//cout <<"gid="<<st.st_gid << endl;
		//cout <<"rdev="<<st.st_rdev << endl;
		//cout <<"file size="<<st.st_size << endl;
		//cout <<"blksize="<<st.st_blksize << endl;
		//cout <<"blocks="<<st.st_blocks << endl;
		//cout <<"atime="<<st.st_atime << endl;
		//cout <<"mtime="<<st.st_mtime << endl;
		//cout <<"ctime="<<st.st_ctime << endl;

		return true;
	}
	else
	{
		//cout<<"No Path, creating Directory "<<strName<<endl;
		//cout <<"stat mode="<<st.st_mode << endl;
	}
	return false;
}
bool DoesFileExist(const string& strName)
{
	if(strName.empty())
	{
		cout<<"ERR013: Path to file is empty = "<<strName<<endl;
		return false;
	}
	struct stat st;
	if ((stat(strName.c_str(), &st) == 0))
	{
		//cout<<"Path to File is = "<<strName<<endl;
		//cout <<"stat mode="<<st.st_mode << endl;
		//cout <<"file size="<<st.st_size << endl;
		//cout <<"device ID="<<st.st_dev << endl;
		//cout <<"file serial number="<<st.st_ino << endl;
		//cout <<"nlink="<<st.st_nlink << endl;
		//cout <<"uid="<<st.st_uid << endl;
		//cout <<"gid="<<st.st_gid << endl;
		//cout <<"rdev="<<st.st_rdev << endl;
		//cout <<"blksize="<<st.st_blksize << endl;
		//cout <<"blocks="<<st.st_blocks << endl;
		//cout <<"atime="<<st.st_atime << endl;
		//cout <<"mtime="<<st.st_mtime << endl;
		//cout <<"ctime="<<st.st_ctime << endl;

		return true;
	}
	else
	{
		//cout<<"No Path to File is = "<<strName<<endl;
		//cout <<"stat mode="<<st.st_mode << endl;
	}
	return false;
}
bool CreateDirectory(const string& strDir){
	if(mkdir(strDir.c_str(),0777) == 0)
		return true;
	return false;
}

bool MountCardx(const string& strPort,const string& partitionNum)
{
	string strMountDir = "/mnt/skd" + partitionNum;
	if(!DoesDirectoryExist(strMountDir)){
		if(!CreateDirectory(strMountDir)){
			cout<<"ERR014: Failed to create /mnt/skdx directory "<<endl;
			return false;
		}
	}


	string strMountSrc2 = "mount /dev/" + strPort + partitionNum + " /mnt/skd" + partitionNum;
	const char * cmd2 = strMountSrc2.c_str();
	cout<<"MSG012: mount: "<<cmd2<<endl; 
	system(cmd2);

	string strRootDir = "invalid";
	string strNetworkDir = "invalid";
	if(partitionNum == "2") {
		strRootDir = "/mnt/skd2/home/root";
		strNetworkDir = "/mnt/skd2/etc/network";
	}
	else if(partitionNum == "3") {
		strRootDir = "/mnt/skd3/home/root";
		strNetworkDir = "/mnt/skd3/etc/network";
	}

	if(!DoesDirectoryExist(strRootDir)){
		cout<<"ERR015: Cannot find mnt/skdx/home/root directory"<<endl;
		return false;
	}
	if(!DoesDirectoryExist(strNetworkDir)){
		cout<<"ERR016: Cannot find mnt/skdx/etc/network directory"<<endl;
		return false;
	}

	return true;
}
bool UnmountCard(const string& partitionNum)
{
	if(partitionNum == "2") {
		system("umount /mnt/skd2 ");
		sleep(1);
		rmdir("/mnt/skd2");
		cout<<"MSG016: unmounting /mnt/skd2."<<endl;
	}
	else if(partitionNum == "3") {
		system("umount /mnt/skd3 ");
		sleep(1);
		rmdir("/mnt/skd3");
		cout<<"MSG017: unmounting /mnt/skd3."<<endl;
	}
	else {
		cout<<"ERR020:  Nothing to unmount."<<endl;
		return false;
	}
    return true;
}
bool MountCard1(const string& strPort)
{
	string strMountDir = "/mnt/skd1";
	if(!DoesDirectoryExist(strMountDir)){
		if(!CreateDirectory(strMountDir)){
			cout<<"ERR022: Failed to create /mnt/skd1 directory "<<endl;
			return false;
		}
	}
	string strMountSrc2 = "mount /dev/" + strPort + "1 /mnt/skd1";
	const char * cmd2 = strMountSrc2.c_str();
	cout<<"MSG020: Executed: "<<strMountSrc2<<endl; 
	system(cmd2);
 
	return true;
}

bool UnmountCard1()
{
    system("umount /mnt/skd1 ");
    sleep(1);
    rmdir("/mnt/skd1");
    cout<<"MSG023: Unmount boot partition /mnt/skd1 and deleting."<<endl;
    return true;
}
bool GetMacAddress(string& strMacaddr, string& partitionNum)
{
	FILE *fp;
	char path[1024];

/* Open the command for reading. */
	string strCommand = "invalid";
	if(partitionNum == "2") {
		strCommand = "cat /mnt/skd2/etc/network/interfaces | grep hwaddress " ;
	}
	if(partitionNum == "3") {
		strCommand = "cat /mnt/skd3/etc/network/interfaces | grep hwaddress " ;
	}
	fp = popen(strCommand.c_str(), "r");
	if (fp == NULL) {
		cout<<"ERR024: Failed to get file list "<<endl;
		return false;
	}

/* Read the output a line at a time - output it. */
	if(fgets(path, sizeof(path)-1, fp) != NULL) {
		string strValue(path);
		pclose(fp);
		//cout<<"macaddr line: "<<strValue<<endl;
		string::size_type pos_end = strValue.find_last_not_of(" \t");
		if(pos_end == string::npos) return false;

		strMacaddr = strValue.substr(pos_end -12, 12);
		//cout<<strMacaddr<<endl;
		if(strMacaddr.size() != 12) return false;
		return true;

	}else{
		pclose(fp);
		return false;
	}
}

char findKaiCard()
{
   // Mount either sda1 or sdb1 boot partition.

	string mountPort1 = "sda"; 
	if(MountCard1(mountPort1))
	{
		string strUenvFileName = "/mnt/skd1/uEnv.txt";
		if(DoesFileExist(strUenvFileName)){ 
			cout<<"MSG300: uEnv.txt file found here: "<<strUenvFileName<<endl;  
			return ('a');
		}
		UnmountCard1();
	}
	
	mountPort1 = "sdb";
	if(MountCard1(mountPort1))
	{
		string strUenvFileName = "/mnt/skd1/uEnv.txt";
		if(DoesFileExist(strUenvFileName)){ 
			cout<<"MSG301: uEnv.txt file found here: "<<strUenvFileName<<endl;  
			return ('b');
		}
	}
	else {
		UnmountCard1();
		cout<<"ERR491: uEnv.txt file not found in sda1 or sdb1."<<endl;
		return ('0');
	}
}
char uEnvCheck()
{
    char sdPart = '0';
    string x;
    ifstream inFile;
    std::string str1 ("root=/dev/mmcblk0p2");
    std::string str2 ("root=/dev/mmcblk0p3");


    inFile.open("/mnt/skd1/uEnv.txt");
    if (!inFile) {
        cout << "ERR025: Unable to open file /mnt/skd1/uEnv.txt"<<endl;
		UnmountCard1();
        return '0';
    }
    else cout<<"MSG024: File /mnt/skd1/uEnv.txt opened successfully."<<endl;
    
    while (inFile >> x) {
	if (str1.compare(x) == 0) {
	    cout<<"MSG025: uEnv.txt indicates "<<x<<endl;  
	    sdPart = '2';
	    break;
	}
	else if (str2.compare(x) == 0) {
	    cout<<"MSG026: uEnv.txt indicates "<<x<<endl;  
	    sdPart = '3';
	    break;
	}
    }    
    inFile.close();
    UnmountCard1();
    sleep(2);
    cout<<"MSG027: uEnvCheck()...Signaling to use partion #"<<sdPart<<endl;
    return sdPart;
}
bool displayModel(const string& path){
    streampos size;
    char * memblock;
    struct stat results;
    ifstream inFile;

    inFile.open (path.c_str());
 
    if (inFile.fail()) 
    { 
	    cout << "Could Not Open File: " << path << "\n"; 
	    return '0';
    } 
    
    if (stat(path.c_str(), &results) == 0)
	    cout<<"EEPROM file size: "<<results.st_size<<endl;  
    else
	    cout<<"BAD file size   : "<<results.st_size<<endl;  

    inFile.seekg (1424, ios::beg);

    unsigned char modelStr[20];
    inFile.read ((char *)&modelStr[0],20);

    unsigned short cByte;
    int i = 0;

    // Last 20 bytes are, 2 for model number, 2 for version, and 16 for name.
    //cout<<"Last 20 Bytes   : ";  
    //for(i = 0; i < 20; i++)
    //{
    //	    cByte = modelStr[i];
    //	    cout<< std::hex << setw(2) << setfill('0') << cByte << ' ';
    //}
    cout<<"\nModel Number    : ";  
    unsigned short upByte = modelStr[1];
    unsigned short lowByte = modelStr[0];
    cout<< std::hex << setw(2)<< std::setfill('0') << upByte;
    cout<< std::hex << setw(2)<< std::setfill('0') << lowByte << '\n';


    cout<<"Model Revision  : ";  
    upByte = modelStr[3];
    lowByte = modelStr[2];
    cout<< std::hex << setw(2)<< std::setfill('0') << upByte;
    cout<< std::hex << setw(2)<< std::setfill('0') << lowByte << '\n';

    string myName(modelStr, modelStr + sizeof modelStr / sizeof modelStr[0]);
    string part = myName.substr(4,myName.length());
    int balance = strlen(part.c_str());
    cout<<"Model Name      : "<<part.substr(0,balance)<<endl;

    return 1;
}

int main(int argc, char** argv) {

	cout<<"\n\nCHECK_SD: Version 7.0 Jan 11, 2018\n"<<endl;

	char port  = findKaiCard();
	if(port == '0')
	{
		cout<<"ERR000: Failed to locate uEnv.txt file. Exiting."<<endl;
		return -1;
	}

	string devDevice = "";
	if(port == 'a') devDevice = "sda";
	if(port == 'b') devDevice = "sdb";
	cout<<"MSG102: mount SD card port: "<<devDevice<<endl;

	string partitionNum = "0";

	partitionNum = uEnvCheck();
	if(partitionNum == "0")
	{
		cout<<"ERR100: Failed to open uEnv.txt file. Exiting."<<endl;
		return -1;
	}

	cout<<"MSG005: Partition "<<partitionNum<<" used to load eeprom.bin"<<endl;

	if(!MountCardx(devDevice,partitionNum))
	{
		cout<<"ERR001: Failed to mount SD card /dev/"<<devDevice<<endl;
		return -1;
	}
	else cout<<"MSG001: Mounted SD card device /dev/"<<devDevice<<endl;

	string strEEpromFileName = "INVALID";
	if(partitionNum == "2") {
		strEEpromFileName = "/mnt/skd2/home/root/eeprom.bin";
		cout<<"MSG006: Partition 2 found: "<<strEEpromFileName<<endl;  
	}
	else if(partitionNum == "3") {
		strEEpromFileName = "/mnt/skd3/home/root/eeprom.bin";
		cout<<"MSG007: Partition 3 found: "<<strEEpromFileName<<endl;  
	}
	else
		cout<<"ERR004: Failed to locate eeprom.bin file... Exiting"<<endl;

	if(!DoesFileExist(strEEpromFileName)){ 
		cout<<"ERR010: EEPROM file was not found: "<<strEEpromFileName<<endl;  
		UnmountCard(partitionNum);
		return -1;
	}
	else
		cout<<"MSG010: EEPROM file found here: "<<strEEpromFileName<<endl;  

	string strMacAddress;
	if(!GetMacAddress(strMacAddress,partitionNum)){
		cout<<"ERR011: Failed to extract MAC address"<<endl;
		return -1;
	}
	cout<<"\n\nMac Address     : 0x"<<strMacAddress<<endl;

	std::string str = strMacAddress;
	str.erase(0, str.find_first_not_of('0')); 
	std::cout << "Mac Address Len : " << strMacAddress.length() << " characters.\n";

	// What ever the length is, back up by 3.
	int nBase;
	nBase = (str.length() - 3);

	cout<<"MAC Index Pos   : "<<nBase<<endl;

	// If there are leading zeros in the mac address substr() doesn't do the right thing.
	string strSerialHex = strMacAddress.substr(nBase,3);

	cout<<"MAC Low 12 bits : 0x"<<strSerialHex<<endl;
	unsigned int nSerialNumber;
	std::stringstream ss;
	ss << std::hex << strSerialHex;
	ss >>nSerialNumber;
	cout<<"MAC Low 12 bits : "<<nSerialNumber<<endl;
	cout<<"Serial Number   : "<<nSerialNumber + 1000<<endl;
	displayModel(strEEpromFileName);
	UnmountCard(partitionNum);
	return 0;
}

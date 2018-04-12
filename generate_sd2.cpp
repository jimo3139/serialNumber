#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sys/types.h>  
#include <sys/stat.h>  
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <list>
#include <iomanip>
#include <sstream> 
using namespace std;

bool ExtractSerialNumber(const string& strName, unsigned int& nNumber);

bool DoesDirectoryExist(const string& strName)
{
    if(strName.empty())
        return false;
    struct stat st;

    if ((stat(strName.c_str(), &st) == 0) && (((st.st_mode) & S_IFMT) == S_IFDIR))
        return true;


    return false;
}
bool DoesFileExist(const string& strName)
{
    if(strName.empty())
        return false;
    struct stat st;
    if ((stat(strName.c_str(), &st) == 0))
        return true;
    return false;
}
bool CreateDirectory(const string& strDir){
   if(mkdir(strDir.c_str(),0777) == 0)
        return true;
    return false;
}

bool MountCardVar(const string& strPort, char& strPart)
{
	string strMountDir = "/mnt/skd2";
	if(!DoesDirectoryExist(strMountDir)){
		if(!CreateDirectory(strMountDir)){
			cout<<"ERR001: Failed to create /mnt/skd2 directory "<<endl;
			return false;
		}
	}

	string strMountSrc2 = "mount /dev/" + strPort + strPart + " /mnt/skd2";
	const char * cmd2 = strMountSrc2.c_str();
	cout<<"MSG002: Executed "<<strMountSrc2<<endl; 
	system(cmd2);
 
	string strRootDir = "/mnt/skd2/home/root";
	string strNetworkDir = "/mnt/skd2/etc/network";
	if(!DoesDirectoryExist(strRootDir)){
		cout<<"ERR003: Cannot find mnt/skd2/home/root directory"<<endl;
		return false;
	}
	else
		cout<<"MSG003: Found mnt/skd2/home/root directory"<<endl;

	if(!DoesDirectoryExist(strNetworkDir)){
		cout<<"ERR004: Cannot find mnt/skd2/etc/network directory"<<endl;
		return false;
	}
	else
		cout<<"MSG004: Found mnt/skd2/etc/network directory"<<endl;

	return true;
}
bool UnmountCard1()
{
    system("umount /mnt/skd1 ");
    sleep(1);
    rmdir("/mnt/skd1");
    cout<<"MSG005: Executed Unmount /mnt/skd1 and deleting."<<endl;
    return true;
}
bool UnmountCard2()
{
    system("umount /mnt/skd2 ");
    sleep(1);
    rmdir("/mnt/skd2");
    cout<<"MSG006: Unmount /mnt/skd2 and deleting."<<endl;
    return true;
}

bool MountCard1(const string& strPort)
{
	string strMountDir = "/mnt/skd1";
	if(!DoesDirectoryExist(strMountDir)){
		if(!CreateDirectory(strMountDir)){
			cout<<"ERR006: Failed to create /mnt/skd1 directory "<<endl;
			return false;
		}
	}
	string strMountSrc2 = "mount /dev/" + strPort + "1 /mnt/skd1";
	const char * cmd2 = strMountSrc2.c_str();
	cout<<"MSG007: Executed: "<<strMountSrc2<<endl; 
	system(cmd2);
 
	return true;
}

bool name_compare(const std::string& first, const std::string& second)
{
	unsigned int n1 = 0;
	unsigned int n2 = 0;
	ExtractSerialNumber(first,n1);
	ExtractSerialNumber(second, n2);
	return n1< n2;
     
}
bool addModel(const string& path, const string& model, const string& rev, const string& name){
    streampos size;
    char * memblock;
    struct stat results;
    ofstream myFile;

    myFile.open (path.c_str() , fstream::app |fstream::out | fstream::binary);
 
    if (myFile.fail()) 
    { 
	    cout << "ERR008: Could Not Open File: " << path << "\n"; 
	    return '0';
    } 
    
    if (stat(path.c_str(), &results) == 0)
    {
	    cout<<"MSG008: file size: "<<results.st_size<<endl;  
    }
    else
    {	  
	    cout<<"ERR009: Error file size: "<<results.st_size<<endl;  
	    return '0';
    }

    unsigned int myNumber;
    std::stringstream ss;
    ss << std::hex << model.c_str();
    ss >> myNumber;
    myFile.write ((char*)&myNumber, 2);

    unsigned int myRev;
    unsigned int myPad;
    std::stringstream rr;
    rr << std::hex << rev.c_str();
    rr >> myRev;
    myFile.write ((char*)&myRev, 2);

    std::cout << "MSG551: Name Length  = " << name.length() << " characters.\n";
    myFile.write ((char*)name.c_str(), name.length());

    unsigned int bal;
    bal = 16 - name.length();
    std::cout << "MSG552: Padding Name = " << bal << " zeros.\n";

    string myZero = "00";
    std::stringstream rrt;
    for(int i = 0; i < bal; i++)
    {
	    rrt << std::hex << myZero.c_str();
	    rrt >> myPad;
	    myFile.write ((char*)&myPad, 1);
    }

    myFile.close();
    return 1;
}


char findKaiCard()
{
   // Mount either sda1 or sdb1 boot partition.

	string mountPort1 = "sda"; 
	if(MountCard1(mountPort1))
	{
		cout<<"MSG500: sda1 is mounted to /mnt/skd1/uEnv.txt"<<endl;  
		string strUenvFileName = "/mnt/skd1/uEnv.txt";
		if(DoesFileExist(strUenvFileName)){ 
			cout<<"MSG300: uEnv.txt file found here: "<<strUenvFileName<<endl;  
			return ('a');
		}
		UnmountCard1();
		cout<<"MSG502: uEnv.txt file not found on sda1, going to try sdb1."<<endl;  
	}
	
	mountPort1 = "sdb";
	if(MountCard1(mountPort1))
	{
		cout<<"MSG501: sdb1 is mounted to /mnt/skd1/uEnv.txt"<<endl;  
		string strUenvFileName = "/mnt/skd1/uEnv.txt";
		if(DoesFileExist(strUenvFileName)){ 
			cout<<"MSG301: uEnv.txt file found here: "<<strUenvFileName<<endl;  
			return ('b');
		}
		cout<<"ERR503: uEnv.txt file not found on sda1, or sdb1."<<endl;  
		UnmountCard1();
		return ('0');
	}
	else {
		cout<<"ERR491: Unable to mount sda1 or sdb1."<<endl;
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
        cout << "ERR009: Unable to open file /mnt/skd1/uEnv.txt"<<endl;
	UnmountCard1();
        return '0';
    }
    else cout<<"MSG009: File /mnt/skd1/uEnv.txt opened successfully."<<endl;
    
    while (inFile >> x) {
	if (str1.compare(x) == 0) {
	    cout<<"MSG010: uEnv.txt indicates "<<x<<endl;  
	    sdPart = '2';
	    break;
	}
	else if (str2.compare(x) == 0) {
	    cout<<"MSG011: uEnv.txt indicates "<<x<<endl;  
	    sdPart = '3';
	    break;
	}
    }    
    inFile.close();
    UnmountCard1();
    sleep(2);
    cout<<"\nMSG012: uEnvCheck()...Signaling to use partion #"<<sdPart<<endl;
    return sdPart;
}
bool GetFileList(const string& strDir, list<string>& files)
{
  FILE *fp;
  char path[1024];

  /* Open the command for reading. */
  string strCommand = "/bin/ls " + strDir;
  fp = popen(strCommand.c_str(), "r");
  if (fp == NULL) {
    cout<<"ERR011: Failed to get file list "<<endl;
    return false;
  }

  /* Read the output a line at a time - output it. */
  while (fgets(path, sizeof(path)-1, fp) != NULL) {
      string val(path);
      string::size_type pos_n = val.find_last_of("n");
      if(pos_n != string::npos)
          val = val.substr(0,pos_n + 1);
      files.push_back(val);
 }

  /* close */
  pclose(fp);
  files.sort(name_compare);
  return !files.empty();

}
bool CopyFile(const string& name, const string& destination){
    string strCommand = "cp " + name ;
    strCommand += " " + destination;
    strCommand += "/eeprom.bin";
    system(strCommand.c_str());
    cout<<"MSG013: Executed: "<<strCommand<<endl; 

}
bool MoveFile(const string& strFile, const string& strSrcDir, const string& strDestDir){
    string strCommand = "mv " + strSrcDir;
    strCommand += "/" + strFile;
    strCommand += " " + strDestDir;
    strCommand += "/.";
    system(strCommand.c_str());
    cout<<"MSG014: Executed: "<<strCommand<<endl; 

    string strNewFileName = strDestDir;
    strNewFileName +="/" + strFile;

    if(!DoesFileExist(strNewFileName)){
        cout<<"ERR014: Failed to move eprom file"<<endl;
        return false;
    }
    else
	cout<<"MSG015: File Found: "<<strNewFileName<<endl; 

    return true;
}
bool ExtractSerialNumber(const string& strName, unsigned int& nNumber){
    string::size_type pos_dot = strName.find_last_of(".");
    if(pos_dot == string::npos){
        cout<<"ERR017: Unexpected file name format: "<<strName<<endl;
        return false;
    }

    string::size_type pos = strName.find_last_of("_");
    if(pos == string::npos || pos >= pos_dot){
       cout<<"ERR018: Unexpected file name format: "<<strName<<endl;
       return false; 
    }

    string strNum = strName.substr(pos + 1, pos_dot-pos);

    nNumber = atoi(strNum.c_str());
    return true;
}
void GenerateMacAddress(unsigned int nSerialNum, string& strMacAddr){
    stringstream stream;
    stream << std::hex << (nSerialNum);
    string strHexStr = stream.str() ;
    strMacAddr = "70B3D5EB7";
    while(strMacAddr.size() + strHexStr.size() < 12)
        strMacAddr +="0";
    strMacAddr += strHexStr;
    cout<<"MSG018: GenerateMacAddress: serial number (dec) "<<nSerialNum<<", mac address: "<<strMacAddr<<endl;
}
bool CreateNetIfaceFile(const string& strMacaddr){
    char buff[1024];
     if(getcwd(buff,sizeof(buff))==NULL){
         cout<<"ERR019: Failed to get current directory"<<endl;
         return false;
     }
    string strWorkinDir(buff);
    string strDummyFile = strWorkinDir;
    strDummyFile += "/dummy_interfaces";
    if(!DoesFileExist(strDummyFile)){
        cout<<"ERR020: Dummy interface file is not found"<<endl;
        return false;
    }
    string strTmpFile = "interfaces";
    string strCommand = "cp " + strDummyFile;
    strCommand += " " + strWorkinDir + string("/") + strTmpFile;
    system(strCommand.c_str());

    cout<<"MSG020: Executed: "<<strCommand<<endl;

    string strNewFile = strWorkinDir + string("/") + strTmpFile;
     if(!DoesFileExist(strNewFile)){
        cout<<"ERR021: Failed to copy file"<<endl;
        return false;
    }
    strCommand = "sed -i -- 's/&&&/" + strMacaddr + "/g' " + strNewFile;
    cout<<"MSG021: Executed: "<<strCommand<<endl;
    system(strCommand.c_str());
    
    string strIfacesDir = "/mnt/skd2/etc/network";
    if(!MoveFile(strTmpFile,strWorkinDir,strIfacesDir)){
        cout<<"ERR022: Failed to copy ifaces file"<<endl;
        return false;
    }
    return true;
}
int main(int argc, char** argv) {

    char sdPart = '0';	
    cout<<"\n\nGENERATE_SD: Version 7.0 Jan 11, 2018\n"<<endl;

    string strAvailableBinFileDir, strDisposedBinFileDir;
    string strModelNumber, strModelRevision;
    string strModelName;
    
    if(argc < 5){
	cout<<"Usage: generate_sd <.bin_files_dir> <used .bin_filed_dir> <Model> <Model Rev> <Model Name>"<<endl;
	return -1;
     }
    strAvailableBinFileDir = argv[1];
    strDisposedBinFileDir = argv[2];
    strModelNumber = argv[3];
    strModelRevision = argv[4];
    strModelName = argv[5];
    cout<<"MSG030: Directories Used:  "<<strAvailableBinFileDir<<", "<<strDisposedBinFileDir<<endl;
    cout<<"MSG031: Model:  "<<strModelNumber<<", Rev "<<strModelRevision<<", Name "<<strModelName<<endl;

    if(!DoesDirectoryExist(strAvailableBinFileDir)){
        cout<<"ERR030: Cannot find directory "<<strAvailableBinFileDir<<endl;
        return -1;
    }
    list<string> files;
    if(!GetFileList(strAvailableBinFileDir,files)){
        cout<<"ERR031: Failed to get file list"<<endl;
        return -1;
    }
    cout<<"MSG032: Available Files: Files remaining in the EEPROM directory = "<<files.size()<<endl;

     if(!DoesDirectoryExist(strDisposedBinFileDir)){
         if(!CreateDirectory(strDisposedBinFileDir)){
             cout<<"ERR032: Failed to create directory: "<<strDisposedBinFileDir<<endl;
             return -1;
         }
     }

    bool bError = false;
    bool bExit = false;
    while(!bExit && !files.empty()){
        cout<<"\n\nINSERT NEW SD card. Enter any key when ready or q to exit, then press enter.";
        char ch;
        cin>>ch;
        if(ch == 'q') break;
        sleep(2);

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

        sdPart = uEnvCheck();
	cout<<"MSG035: Partition "<<sdPart<<" used to load eeprom.bin"<<endl;

       // Mount either sda2or3 or sdb2or3
	string mountPort = "sda"; 
	if(!MountCardVar(mountPort, sdPart))
	{
		cout<<"ERR036: Failed to mount SD card /dev/sda, trying sdb partition "<<sdPart<<endl;
		string mountPort = "sdb";
		if(!MountCardVar(mountPort, sdPart))
		{
			cout<<"ERR037: Failed to mount SD card /dev/sda... or dev/sdb... Exiting"<<endl;
			continue;
		}	
		else cout<<"MSG037: Mounted SD card device /dev/sdb"<<sdPart<<endl;
	}
	else cout<<"MSG038: Mounted SD card device /dev/sda"<<sdPart<<endl;

        string strFileName = files.front(); 
        cout<<"MSG050: Using EEPROM file: "<<strFileName<<endl;

        files.pop_front();

        string strNextName = files.front();
        cout<<"MSG051: Next  EEPROM file: "<<strNextName<<endl;

        string strLastName = files.back();
        cout<<"MSG052: Last  EEPROM file: "<<strLastName<<endl;

	string strPath = strAvailableBinFileDir;
        strPath +="/" + strFileName;
        string strDestRootDir = "/mnt/skd2/home/root";
        addModel(strPath,strModelNumber,strModelRevision,strModelName);
        CopyFile(strPath,strDestRootDir);
	cout<<"MSG053: Dest Path: "<<strDestRootDir<<endl;
        strDestRootDir +="/eeprom.bin" ;
        if(!DoesFileExist(strDestRootDir)){
            cout<<"ERR050: Failed to copy eprom file"<<endl;
            bError = true;
            break;
        }
	else
	   cout<<"MSG054: Copied and Verified: "<<strDestRootDir<<endl;

        if(!MoveFile(strFileName,strAvailableBinFileDir,strDisposedBinFileDir)){
            cout<<"ERR039: Failed to move eeprom file";
            bError = true;
            break;
        }
	else
	   cout<<"MSG055: Moving File Complete: "<<strFileName<<endl;


        unsigned int nSerialNumber;
        if(!ExtractSerialNumber(strFileName,nSerialNumber)){
            cout<<"ERR055: Failed to extract serial number "<<endl;
            bError = true;
            break;
        }
	else
	   cout<<"MSG056: Extracted Serial Number: "<<nSerialNumber<<endl;


        if(nSerialNumber > 0xFFF ){
 		cout<<"ERR040: SERIAL NUMBER exceeds max 0xFFF"<<endl;
		UnmountCard2();
		return -1;
	}
        cout<<"MSG057: Serial Number: "<<nSerialNumber + 1000<<endl;
        string strMacAddr ;
        GenerateMacAddress(nSerialNumber,strMacAddr);
        cout<<"MSG058: MAC Address: "<<strMacAddr<<endl;
        if(!CreateNetIfaceFile(strMacAddr)){
            cout<<"ERR041: Failed updating network interfaces file"<<endl;
            bError = true;
            break;
        }
	sleep(2);
        UnmountCard2();
        cout<<"\n\nMSG060: Remove Card............. Done.\n\n"<<endl;
        bExit = true; // no looping for now
    }
    if(bError){
        UnmountCard2();
        cout<<"ERR042: EXITING DUE TO ERRORS"<<endl;
    }else if(files.empty()){
	cout<<"ERR043: No more files. EXITING....."<<endl;
    }
    return 0;
}

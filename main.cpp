#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

int main(){
    string asciiArt = R"(
      ___           ___           ___           ___     
     /\  \         /\  \         /\__\         /\__\    
    /::\  \       /::\  \       /:/  /        /::|  |   
   /:/\:\  \     /:/\ \  \     /:/  /        /:|:|  |   
  /::\~\:\  \   _\:\~\ \  \   /:/__/  ___   /:/|:|__|__ 
 /:/\:\ \:\__\ /\ \:\ \ \__\  |:|  | /\__\ /:/ |::::\__\
 \/__\:\/:/  / \:\ \:\ \/__/  |:|  |/:/  / \/__/~~/:/  /
      \::/  /   \:\ \:\__\    |:|__/:/  /        /:/  / 
      /:/  /     \:\/:/  /     \::::/__/        /:/  /  
     /:/  /       \::/  /       ~~~~           /:/  /   
     \/__/         \/__/                       \/__/    
    )";
  cout<<asciiArt;
  cout<<"=========================================================================================="<<endl;
  cout<<"Welcome to ASVM, a tool for you to open a vmware vitural machine automactically when boot."<<endl;
  cout<<"=========================================================================================="<<endl;
  cout<<"Input your vmware vmx file path"<<endl;
  cout<<"e.g./home/xxxx/Windows\\ XP\\ Professional/ Note:If your there are some space in your file name such as /home/xxxx/vmware/Windows XP Professional/,please in put it like /home/xxxx/vmware/Windows\\ XP\\ Professional/)"<<endl;;
  cout<<"------------------------------------------------------------------------------------------"<<endl;
  string path, vmxname;
  getline(cin,path);
  cin.ignore();
  cout<<"Input your vmx file name"<<endl;
  cout<<"e.g.Windows\\ XP\\ Professional.vmx (Note:If your there are some space in your file name such as [Windows XP Professional.vmx],please in put it like Windows\\ XP\\ Professional.vmx)"<<endl;
  getline(cin, vmxname);
  cin.ignore();
  cout<<"------------------------------------------------------------------------------------------"<<endl;
  cout<<"Do you want your vm start up as fullscreen? \n1.yes 2.no (input 1 or 2) ";
  int fullscreen;
  cin>>fullscreen;
  cin.ignore();
  if (fullscreen==1){
    ofstream outfile("asvm.sh");
    outfile<<"cd "<<path<<endl;
    outfile<<"vmware "<<vmxname<<" -X";
    outfile.close();
  }
  else{
    ofstream outfile("asvm.sh");
    outfile<<"cd "<<path<<endl;
    outfile<<"vmware "<<vmxname<<" -x";
    outfile.close();
  }
  system("chmod +x asvm.sh");
  cout<<"------------------------------------------------------------------------------------------"<<endl;
  cout << "Do you want to add this script to your crontab for automatic startup? \n1.yes 2.no (input 1 or 2) ";
  int crontaboption;
  cin>>crontaboption;
  if(crontaboption==1){
    string currentpath = fs::current_path().string() + "/asvm.sh";
    string cronCommand = "(crontab -l 2>/dev/null; echo \"@reboot " + currentpath + "\") | crontab -";
    int result = system(cronCommand.c_str());
    if(result==0){
      cout<<"Successfully added 'asvm.sh' to crontab. The virtual machine will now start automatically at boot.";
    }
    else{
      cout<<"Error: Failed to add script to crontab. Please check your permissions or try adding it manually.";
    }
  }
  else{
    cout << "The script has been created, but it will not start automatically. You can run it manually." << endl;
  }
  return 0;
}
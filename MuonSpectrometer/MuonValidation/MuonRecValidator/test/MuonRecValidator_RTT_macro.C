/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>   
#include <string>
#include <vector>
#include <map>
#include "utils.h"

/////////////////////////////////////////
// wide analysis configuration strings //
/////////////////////////////////////////

bool castorConn = true;
std::string castor_path="/castor/cern.ch/user/m/mbellomo/MuonRecValidator/";
std::string reference_file = "MuonRecValidator_trig1_misal1.005145.PythiaZmumu.recon.AOD.v12000601.aan.root";
std::string reference_release = "v12000601";
//std::string reference_file = "MuonRecValidator_trig1_misal1_valid1.005145.PythiaZmumu.recon.AOD.v13002003.aan.root";
//std::string reference_release = "v13002003";

std::string test_file;
std::string test_release;

// directory where plots will go
std::string plot_directory = ".";
// string that will get attached to all plot names 
// ---> just change .eps to .gif etc if you want a different format
std::string plotname_extension = ".eps";  

 
void MuonRecValidator_RTT_macro(std::string atlastag, std::string dstnum, std::string sampletype, std::string recotype, std::string release)
{
  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle();

  test_release = release;
  test_file = "MuonRecValidator_"+atlastag+"."+dstnum+"."+sampletype+".recon."+recotype+"."+release+".aan.root";

  if(castorConn) reference_file="rfio:"+castor_path+reference_file;

  // build file name 
  std::string chain_filename = "MuonRecValidator_"+atlastag+"."+dstnum+"."+sampletype+".recon."+recotype+"."+release+".aan.root"; 
  const char* chain_filename_char=chain_filename.c_str();

  cout << "********************************" << endl;
  cout << "**   Opening ROOT file name : " << chain_filename << "   **" << endl;
  cout << "********************************" << endl;

  //cout << "\nLoading Loader for ROOT std::vector<double> compatibility..." << endl;
  //gROOT->ProcessLine(".L loader.C+");
  //gROOT->ProcessLine(".L loader_C.so");
  //Cintex::Enable();
  //cout << " ---> done" << endl;

  cout << "\nLoading MuonRecValidator_RTT_perfplots.C..." << endl;
  gROOT->ProcessLine(".L MuonRecValidator_RTT_perfplots.C");
  cout << " ---> done" << endl;
  
  cout << "\nRunning ...MuonRecValidator_RTT_perfplots.C..." << endl;
  MuonRecValidator_RTT_perfplots(chain_filename_char);
  cout << " ---> done" << endl; 
  
}

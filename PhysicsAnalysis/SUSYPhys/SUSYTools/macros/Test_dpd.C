/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include<iostream>
#include <TMatrixD.h>
#include <TVectorD.h>
#include <TH1F.h>
#include <TLorentzVector.h>
#include <TSystem.h>
#include <TChain.h>
#include <TString.h>
#include <TTree.h>
#include <TLegend.h>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <exception>

using namespace std;

void Test_dpd(std::string argStr, int DEBUG, int write_branches){

  //Define number of messages by value of DEBUG; if int DEBUG == 1 print all messages into txt file
  //write_branches == 1 => will write list of branches into txt file list_of_branches.txt

  if (DEBUG != 1) DEBUG = 0;
  if (write_branches != 1) write_branches = 0;

  //First part: Read in file list

  /*
  std::string argStr;

  char buf[256+1];
  unsigned int delpos;
  std::ifstream ifs("input.txt");
  while (true)
    {
      ifs.read(buf,256);
      if (ifs.eof())
        {
          if (ifs.gcount() == 0) break;
          delpos = ifs.gcount()-1;
        }
      else
        {
          delpos = ifs.gcount();
        }
      buf[delpos] = 0x00;
      argStr += buf;
    }
  */
  std::ofstream ofs("output.txt");
  std::ofstream ofs2;
  int error_counter = 0;

  // split by ','
  std::vector<std::string> fileList;
  for (size_t i=0,n; i <= argStr.length(); i=n+1)
    {
      n = argStr.find_first_of(',',i);
      if (n == string::npos)
        n = argStr.length();
      std::string tmp = argStr.substr(i,n-i);
      std::string ttmp;
      for(unsigned int j=0; j<tmp.size(); j++)
	{
	  if(tmp[j]==' ' || tmp[j]=='\n') continue;
	  ttmp+=tmp[j];
	}
      fileList.push_back(ttmp);
    }

  // open input files
  int open_tree = 0;
  TChain * chain = new TChain("susy","");
  for (unsigned int iFile=0; iFile<fileList.size(); ++iFile)
    {
      std::cout << "open " << fileList[iFile].c_str() << std::endl;
      ofs << "open " << fileList[iFile].c_str() << std::endl;
      open_tree = chain->AddFile(fileList[iFile].c_str(),0);
      if (DEBUG==1) cout << "Status of file appened to chain: " << open_tree << endl;
      if (open_tree == 0) {
	ofs << "Error: File cannot be opened or tree \'susy\' does not exist" << endl;
	error_counter++;
      }
    }

  TTree *tree;
  tree = chain;


  Long64_t entries = tree->GetEntries();


  if (DEBUG==1) cout << "Number of entries in tree: " << entries << endl;
  if (DEBUG==1) ofs << "Number of entries in tree: " << entries << endl;
  if (entries ==0) {
    cout << "Error: Tree has no entries" << endl;
    ofs << "Error: Tree has no entries" << endl;
    error_counter++;
  }

  Long64_t b = 0;

  TObjArray * list_of_branches = (TObjArray*)tree->GetListOfBranches();

  int number_of_branches = 0;
  if(write_branches == 1 && !ofs2.is_open()) ofs2.open("list_of_branches.txt");

  for (int j=0; j<=list_of_branches->LastIndex(); j++)
    {
      number_of_branches++;
      std::string tmp_branch = ((TBranch*)list_of_branches->At(j))->GetName(); 
      if(write_branches == 1) ofs2 << tmp_branch << endl; 
      if(tmp_branch!="" && tree->GetBranch(tmp_branch.c_str())) {
	b = (tree->GetBranch(tmp_branch.c_str()))->GetTotalSize();
	if (DEBUG==1) cout << "Size of branch " << tmp_branch << ": " << b << endl;
	if (DEBUG==1) ofs << "Size of branch " << tmp_branch << ": " << b << endl;
	if (b <= 0) {
	  cout << "Error in branch " << tmp_branch << endl;
	  ofs << "Error in branch " << tmp_branch << endl;
	  error_counter++;
	}
      }
      else if(tmp_branch!="" && !(tree->GetBranch(tmp_branch.c_str())))
	{
	  cout << "Error: " << tmp_branch << " does not exist" << endl;
	  ofs << "Error: " << tmp_branch << " does not exist" << endl;
	  error_counter++;
	}
    }
  if (DEBUG==1) cout <<"Number of branches: " << number_of_branches << endl; 
  cout << error_counter << " error(s) was(were) reported." << endl;
}

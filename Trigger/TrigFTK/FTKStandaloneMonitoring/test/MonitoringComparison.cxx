/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>
#include <fstream>

#include <getopt.h>

#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "TTree.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TFrame.h"
#include "TH1F.h"
#include "TBenchmark.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TrigFTKSim/FTKTrackStream.h"
#include "FTKStandaloneMonitoring/FTKTrkAssoc.h"
#include "FTKStandaloneMonitoring/CompareFTKEvents.h"

using namespace std;

#ifndef TRUE
  #define TRUE                  0x01
#endif

#ifndef FALSE
  #define FALSE                 0x00
#endif

#define BUFSIZE  (4*1024)

// Globals
int verbose = FALSE;
int readfile = FALSE;
char filename[200] = {0};
std::string inputfilename;
uint32_t checkword=0xaa1234aa;



/**************/
void usage(void)
/**************/
{
  std::cout << "Valid options are ..." << std::endl;
  std::cout << "-f x: Read BS_FTK data from file. The parameter is the path and the name of the file" << std::endl;
  std::cout << "-n x: Read NTUP_FTK data from file. The parameter is the path and the name of the file" << std::endl;
  std::cout << "-m x: maximum number of events" << std::endl;
  std::cout << "-v  : Verbose output                               -> Default: FALSE" << std::endl;
  std::cout << std::endl;
}



/*****************************/
int main(int argc, char **argv)
/*****************************/
{

  std::string inputfilenameBS= "/afs/cern.ch/user/g/giulini/workdir/public/FTKcomparison/compareInputs/OUT.BS_FTK.root";
  std::string inputfilenameNTUPFTK= "/afs/cern.ch/user/g/giulini/workdir/public/FTKcomparison/compareInputs/OUT.NTUP_FTK.root";
  //evtinfo TTree NTUP_FTK
  int c;
  int evtmax=0;
  static struct option long_options[] = {"DVS", no_argument, NULL, '1'}; 
  while ((c = getopt_long(argc, argv, "f:n:v:m:h", long_options, NULL)) != -1)
    switch (c) 
    {
    case 'h':
      std::cout << "Usage: " << argv[0] << " [options]: "<< std::endl;
      usage();
      exit(-1);
      break;

    case 'f':   
      readfile = TRUE;
      sscanf(optarg,"%s", filename);
      inputfilenameBS.clear();
      inputfilenameBS.append(filename);
      std::cout << "read BS_FTK data from file: " << std::endl<< inputfilenameBS<< std::endl << std::endl;
      break;

    case 'n':   
      readfile = TRUE;
      sscanf(optarg,"%s", filename);
      inputfilenameNTUPFTK.clear();
      inputfilenameNTUPFTK.append(filename);
      std::cout << "read NTUP_FTK data from file: "<< std::endl << inputfilenameNTUPFTK<< std::endl << std::endl;
      break;
      
    case 'm':   
      sscanf(optarg,"%d", &evtmax);
      std::cout << "maximum number of events: "<< evtmax << std::endl << std::endl;
      break;
      
    case 'v': verbose = TRUE;               break;

    default:
      std::cout << "Invalid option " << c << std::endl;
      std::cout << "Usage: " << argv[0] << " [options]: " << std::endl;
      usage();
      exit (-1);
    }
  ;
  
  CompareFTKEvents *comparison= new CompareFTKEvents(inputfilenameBS,inputfilenameNTUPFTK);
  comparison->PrintFiles();      
  if (verbose) comparison->SetVerbose();  
  if (evtmax!=0)    comparison->SetNEvents(evtmax);
  std::string str= "prova";
  std::vector<std::string> variable_list={"pt","eta","phi","d0","z0","chi2","ETA_PHI"};
  
  std::vector<std::string> histo_list;
  for (auto & istr: variable_list){
      std::stringstream ss;
      ss << "HWSW_" << istr;
      std::stringstream ss2;
      ss2 << "HWonly_" << istr;
      histo_list.push_back(ss.str());
      histo_list.push_back(ss2.str());
  }
  histo_list.push_back("m_res_pt");
  histo_list.push_back("m_res_d0");
  histo_list.push_back("nTrk_same_hw_sw");
  histo_list.push_back("nTrk_different_hw_sw");
  histo_list.push_back("nTrk_only_hw");
  histo_list.push_back("nTrk_only_sw");
  comparison->SetHistos(histo_list);
  comparison->Execute();      
  return 0;
}


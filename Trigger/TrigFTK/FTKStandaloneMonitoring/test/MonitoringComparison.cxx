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
#include <ipc/core.h>

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



/**************/
void usage(void)
/**************/
{
  std::cout << "Valid options are ..." << std::endl;
  std::cout << "-f x: Read BS_FTK data from file. The parameter is the path and the name of the file" << std::endl;
  std::cout << "-n x: Read NTUP_FTK data from file. The parameter is the path and the name of the file" << std::endl;
  std::cout << "-m x: maximum number of events" << std::endl;
  std::cout << "-v  : Verbose output                               -> Default: FALSE" << std::endl;
  std::cout << "-p x: partition name" << std::endl;
  std::cout << "-s  : not to initialize a partition" << std::endl;
  std::cout << std::endl;
}



/*****************************/
int main(int argc, char **argv)
/*****************************/
{

  std::string  partition_name="";
  //default files
  std::string inputfilenameBS= "/afs/cern.ch/user/g/giulini/workdir/public/FTKcomparison/compareInputs/OUT.BS_FTK.root";
  std::string inputfilenameNTUPFTK= "/afs/cern.ch/user/g/giulini/workdir/public/FTKcomparison/compareInputs/OUT.NTUP_FTK.root";
  //evtinfo TTree NTUP_FTK
  int c;
  int evtmax=0;
  bool setup_partition=true;
  static struct option long_options[] = {"DVS", no_argument, NULL, '1'}; 
  while ((c = getopt_long(argc, argv, "f:n:m:p:svh", long_options, NULL)) != -1)
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
      
    case 'p':   
      sscanf(optarg,"%s", filename);
      partition_name.clear();
      partition_name.append(filename);
      std::cout << "parition name: " << std::endl<< partition_name<< std::endl << std::endl;
      break;
      
    case 'm':   
      sscanf(optarg,"%d", &evtmax);
      std::cout << "maximum number of events: "<< evtmax << std::endl << std::endl;
      break;
      
    case 's': setup_partition = false;  break;

    case 'v': verbose = TRUE;  break;


    default:
      std::cout << "Invalid option " << c << std::endl;
      std::cout << "Usage: " << argv[0] << " [options]: " << std::endl;
      usage();
      exit (-1);
    }
  ;

  try {
     IPCCore::init( argc, argv );
  }
  catch( daq::ipc::Exception & ex ) {
      //something went very bad, report and exit
      std::cout<<"ERROR: IPCCore could not be initialised"<<std::endl;
      return 1;
  }

  // initialization of the class to compare FTK events of HW (BS_FTK) and SW (NTUP_FTK)
  CompareFTKEvents *comparison= new CompareFTKEvents(inputfilenameBS,inputfilenameNTUPFTK);
  if (setup_partition) comparison->SetupPartition(partition_name);
  if (verbose) comparison->SetVerbose();  
  if (evtmax!=0)    comparison->SetNEvents(evtmax);
  // create list of histograms to be published on oh
  std::vector<std::string> variable_list={"pt","eta","phi","d0","z0","chi2","ETA_PHI"};//  
  std::vector<std::string> histo_list;
  for (auto & istr: variable_list){
      std::stringstream ss;
      ss << "HWSW_" << istr; // labels for FTK tracks completely matched (= same track parameters) btw HW and SW
      std::stringstream ss2;
      ss2 << "HWonly_" << istr; // labels for HW FTK tracks not matched to any SW track
      std::stringstream ss3;
      ss3 << "SWonly_" << istr; // labels for SW FTK tracks not matched to any HW track
      std::stringstream ss4;
      ss4 << "HWvsSWhw_" << istr; // labels for HW FTK tracks geometrically matched to SW tracks but with different track parameters
      std::stringstream ss5;
      ss5 << "HWvsSWsw_" << istr; // labels for SW FTK tracks geometrically matched to HW tracks but with different track parameters
      histo_list.push_back(ss.str());
      histo_list.push_back(ss2.str());
      histo_list.push_back(ss3.str());
      histo_list.push_back(ss4.str());
      histo_list.push_back(ss5.str());
      if (istr!="ETA_PHI"){
          std::stringstream ss6;
          ss6 << "HWvsSWdiff_" << istr; // resolution distributions for FTK tracks matched btw HW and SW but with different track parameters
          histo_list.push_back(ss6.str());
      }
  }
  histo_list.push_back("nTrk_same_hw_sw");
  histo_list.push_back("nTrk_different_hw_sw");
  histo_list.push_back("nTrk_only_hw");
  histo_list.push_back("nTrk_only_sw");
  histo_list.push_back("nTrk_HW");
  histo_list.push_back("nTrk_SW");
  comparison->SetHistos(histo_list);
  comparison->Execute();      
  return 0;
}


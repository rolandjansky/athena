#include "TString.h"
#include "TSystem.h"
#include "TArrayI.h"
#include "TDatime.h"
#include <fstream>
#include <iostream>
#include <typeinfo>
#include <stdio.h>
#include <string>

// Main macro to run - returns 0 in case of success
int RTTrun(TString jobOptionFile="",
	   TString dataFile="", Int_t nEvents=100000,
	   TString OutputLevel="INFO", TString athenaOptions="");

int RTTrun(TString jobOptionFile,
	   TString dataFile, Int_t nEvents,
	   TString OutputLevel, TString athenaOptions)
{
  //------------------------------------------------------------------
  // This is a RTT test of an HepMCAnalysis_i job
  //----------------------------------------------------------------------
  Int_t sysExecOut;

  TString dataTemp = dataFile;

  sysExecOut = gSystem->Exec("echo \"================================================================================\"");
  sysExecOut = gSystem->Exec("echo \"|                  RTT Test of HepMCAnalysis_i                            |\"");
  sysExecOut = gSystem->Exec("echo \"================================================================================\"");

//   std::cout<<"name of the job option file to be run: ";
//   std::cin >> jobOptionFile;
//   std::cout<<std::endl;

//   std::cout<<"jobOptionFile = "<<jobOptionFile<<std::endl;

  //---------------------
  // Some Initial settings
  //---------------------
  //set name of the logfile
  TString logFile="HepMCAnalysis_i_";
  logFile += jobOptionFile;
  logFile += ".log";
  
  // Execute athena MC job
  TString mcJobLogFile = "HepMCAnalysis_i_";
  mcJobLogFile += jobOptionFile;
  mcJobLogFile += ".log";

  TString mcJobExecutable;
  
  mcJobExecutable = "JOBOPTSEARCHPATH=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC12JobOptions/latest/common:$JOBOPTSEARCHPATH; ";
  mcJobExecutable += "athena.py";
  mcJobExecutable += " -l " + OutputLevel;
  if(athenaOptions !="") mcJobExecutable += " "+athenaOptions+" ";
  mcJobExecutable += " -c ";
  if (nEvents>0) mcJobExecutable += TString::Format("\"EvtMax=%d",nEvents);
  mcJobExecutable += "\"";
  
  //mcJobExecutable = "Generate_trf.py ecmEnergy=7000. runNumber=105100 firstEvent=1 maxEvents=-1 randomSeed=54298752 jobConfig=./";
  //mcJobExecutable += jobOptionFile;
  //mcJobExecutable += " outputEVNTFile=EVNT.";
  //mcJobExecutable += jobOptionFile;
  //mcJobExecutable += ".root";

  sysExecOut = gSystem->Exec("echo \"================================================================================\"");
  sysExecOut = gSystem->Exec("echo \"|                      Start the MC job                                        |\"");
  sysExecOut = gSystem->Exec("echo \"================================================================================\"");

  std::cout << "Start athena job:\t" << mcJobExecutable << " " << jobOptionFile << " >& " << mcJobLogFile << std::endl;
  sysExecOut = gSystem->Exec(mcJobExecutable + " " + jobOptionFile + " >& " + mcJobLogFile);
  //std::cout << "Start athena job:\t" << mcJobExecutable << std::endl;
  //sysExecOut = gSystem->Exec(mcJobExecutable + " >& " + mcJobLogFile);

  sysExecOut = gSystem->Exec("echo \"================================================================================\"");
  sysExecOut = gSystem->Exec("echo \"|                      Finish the MC job                                       |\"");
  sysExecOut = gSystem->Exec("echo \"================================================================================\"");

  
  return 0;
}

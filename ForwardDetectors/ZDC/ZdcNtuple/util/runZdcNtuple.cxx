#include "xAODRootAccess/Init.h"
#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/ToolsDiscovery.h"
#include "EventLoop/Job.h"
#include "EventLoop/DirectDriver.h"
#include <EventLoop/OutputStream.h>
#include <EventLoopAlgs/NTupleSvc.h>
#include <EventLoopAlgs/AlgSelect.h>
#include "EventLoopGrid/PrunDriver.h"
#include "EventLoop/LSFDriver.h"

#include "SampleHandler/DiskListLocal.h"
#include "SampleHandler/DiskListEOS.h"
#include <TSystem.h>
#include "SampleHandler/ScanDir.h"

#include "ZdcNtuple/ZdcNtuple.h"

int main( int argc, char* argv[] ) {

  // Take the submit directory from the input if provided:
  std::string submitDir = "submitDir";
  if( argc > 1 ) submitDir = argv[ 1 ];

  std::string readDir = "file:/tmp/steinber";
  if( argc > 2 ) readDir = argv[ 2 ];

  int nevt = -1;
  if( argc > 3 ) nevt = std::atoi(argv[ 3 ]);

  std::string enableStr = "" ;
  if ( argc > 4 ) enableStr = argv[4];

  std::string vers = "0";
  if ( argc > 5 ) vers = argv[5];
  
  int localTrackLimit = 1e6;
  if ( argc > 6 ) localTrackLimit = std::atoi(argv[ 6 ]);

  std::string grlOption = "";
  if ( argc > 7 ) grlOption = argv[ 7 ];

  // Set up the job for xAOD access:
  xAOD::Init().ignore();

  // Construct the samples to run on:
  SH::SampleHandler sh;
  //sh.setMetaString( "nc_grid_filter", "*MinBias.merge.AOD.*");

  //const char* inputFilePath = gSystem->ExpandPathName(readDir.c_str());
  //SH::DiskListLocal list (readDir);

  std::string fileDir;
  bool file_mode = 0;
  bool eos_mode = 0;
  bool eoslsf_mode = 0;
  bool grid_mode = 0;

  if (readDir.find("file:")==0)
    {
      fileDir = readDir.substr(5); // remove file:
      file_mode = 1;
      std::cout << "Setting up file mode:" << fileDir << std::endl;
      SH::DiskListLocal list (fileDir);
      SH::ScanDir().filePattern("*").scan(sh,list);
    }

  if (readDir.find("eos:")==0)
    {
      fileDir = readDir.substr(4);
      eos_mode = 1;
      std::cout << "Setting up EOS mode:" << fileDir << std::endl;
      std::string eospath = "root://eosatlas.cern.ch/"+fileDir;
      SH::DiskListEOS list(fileDir,eospath);
      SH::ScanDir().filePattern("*").scan(sh,list);
    }

  if (readDir.find("eoslsf:")==0)
    {
      fileDir = readDir.substr(7);
      eoslsf_mode = 1;
      std::cout << "Setting up EOS LSF mode:" << fileDir << std::endl;
      std::string eospath = "root://eosatlas.cern.ch/"+fileDir;
      SH::DiskListEOS list(fileDir,eospath);
      SH::ScanDir().filePattern("*").scan(sh,list);
    }

  if (readDir.find("grid:")==0)
    {
      fileDir = readDir.substr(5);
      grid_mode = 1;
      std::cout << "Setting up Grid mode:" << fileDir << std::endl;
      SH::addGrid (sh, fileDir);
      sh.setMetaString("nc_grid_filter","*");
    }

  // Set the name of the input TTree. It's always "CollectionTree"
  // for xAOD files.
  sh.setMetaString( "nc_tree", "CollectionTree" );
  // Print what we found:
  sh.print();

  // Create an EventLoop job:
  EL::Job job;
  job.sampleHandler( sh );
  
  // Add our analysis to the job:
  ZdcNtuple* zdcAna = new ZdcNtuple();

  std::cout << "runtime options:" << enableStr << std::endl;

  zdcAna->outputName = "output"; 
  if (enableStr.find("debug")!=std::string::npos) { zdcAna->debug = true; }
  if (enableStr.find("slimmed")!=std::string::npos) { zdcAna->slimmed = true; }
  if (enableStr.find("tree")!=std::string::npos) { zdcAna->enableOutputTree = true; }
  if (enableStr.find("samples")!=std::string::npos) { zdcAna->enableOutputSamples = true; }
  if (enableStr.find("trigger")!=std::string::npos) { zdcAna->enableTrigger = true; }
  if (enableStr.find("zdcCalib")!=std::string::npos) { zdcAna->zdcCalib = true; }
  if (enableStr.find("zdcLaser")!=std::string::npos) { zdcAna->zdcLaser = true; }
  if (enableStr.find("tracks")!=std::string::npos) { zdcAna->enableTracks = true; }
  if (enableStr.find("clusters")!=std::string::npos) { zdcAna->enableClusters = true; }
  if (enableStr.find("TT")!=std::string::npos) { zdcAna->enableTT = true; }
  if (enableStr.find("jets")!=std::string::npos) { zdcAna->enableJets = true; }
  if (enableStr.find("electrons")!=std::string::npos) { zdcAna->enableElectrons = true; }
  if (enableStr.find("photons")!=std::string::npos) { zdcAna->enablePhotons = true; }
  if (enableStr.find("trigjet")!=std::string::npos) { zdcAna->enableTriggerJets = true; }
  if (enableStr.find("muons")!=std::string::npos) { zdcAna->enableMuons = true; }
  if (enableStr.find("truth")!=std::string::npos) { zdcAna->enableTruth = true; }
  if (enableStr.find("noflipdelay")!=std::string::npos) { zdcAna->flipDelay = false; }
  if (enableStr.find("doflipdelay")!=std::string::npos) { zdcAna->flipDelay = true; }
  if (enableStr.find("trackLimitReject")!=std::string::npos) { zdcAna->trackLimitReject = true; }
  if (enableStr.find("reprocZdc")!=std::string::npos) { zdcAna->reprocZdc = true; }
  if (enableStr.find("7samp")!=std::string::npos) { zdcAna->nsamplesZdc = 7; }
  if (enableStr.find("15samp")!=std::string::npos) { zdcAna->nsamplesZdc = 15; }
  if (enableStr.find("noGRL")!=std::string::npos) { zdcAna->useGRL = false;}
  if (enableStr.find("express2016A")!=std::string::npos) { zdcAna->express2016A = true;}
  if (enableStr.find("upc2015")!=std::string::npos) { zdcAna->upc2015 = true;}
  if (enableStr.find("mb2015")!=std::string::npos) { zdcAna->mb2015 = true;}
  if (enableStr.find("upcL2015")!=std::string::npos) { zdcAna->upcL2015 = true;}
  if (enableStr.find("main2016A")!=std::string::npos) { zdcAna->main2016A = true;}
  if (enableStr.find("upc2016A")!=std::string::npos) { zdcAna->upc2016A = true;}
  if (enableStr.find("express2016B")!=std::string::npos) { zdcAna->express2016B = true;}
  if (enableStr.find("main2016B")!=std::string::npos) { zdcAna->main2016B = true;}
  if (enableStr.find("upc2016B")!=std::string::npos) { zdcAna->upc2016B = true;}
  if (enableStr.find("upc2016C")!=std::string::npos) { zdcAna->upc2016C = true;}
  if (enableStr.find("mboverlay2016")!=std::string::npos) { zdcAna->mboverlay2016 = true;}

  if (enableStr.find("writeOnlyTriggers")!=std::string::npos) { zdcAna->writeOnlyTriggers = true;}
  if (grlOption != "") zdcAna->grlFilename = grlOption;
  
  zdcAna->trackLimit = localTrackLimit;

  // Add ntuple service

  EL::OutputStream output  ("output");
  job.outputAdd (output);
  EL::NTupleSvc *ntuple = new EL::NTupleSvc ("output");
  // configure ntuple object
  job.algsAdd (ntuple);

  // Add my algorithms
  job.algsAdd( zdcAna );

  if (nevt>-1) job.options()->setDouble (EL::Job::optMaxEvents, nevt);
  std::cout << "Running " << nevt << " events" << std::endl;
  
  if (file_mode||eos_mode)
    {
      // Run the job using the local/direct driver:
      EL::DirectDriver driver;
      driver.submit( job, submitDir );
      //if (zdcAna->zdcCalib) job.options()->setString (EL::Job::optXaodAccessMode, EL::Job::optXaodAccessMode_branch);
      if (zdcAna->zdcCalib) job.options()->setString(EL::Job::optXaodAccessMode, EL::Job::optXaodAccessMode_athena);
    }
  
  if (grid_mode)
    {
      EL::PrunDriver driver;
      driver.options()->setString("nc_outputSampleName", "user.steinber.%in:name[0]%.%in:name[1]%.%in:name[2]%.%in:name[3]%.%in:name[4]%.%in:name[5]%.%in:name[6]%."+vers);
      driver.options()->setDouble(EL::Job::optGridMergeOutput, 1); //run merging jobs for all samples before downloading (recommended) 
      driver.options()->setString(EL::Job::optGridNFilesPerJob,  "3"); //By default, split in as few jobs as possible      
      //driver.options()->setString(EL::Job::optGridSite,"CERN-PROD");
      driver.submitOnly( job, submitDir );
    }

  if (eoslsf_mode)
    {
      EL::LSFDriver driver;
      TString shellCommand = "export export LSB_JOB_REPORT_MAIL=N; export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase &&";
      shellCommand += "source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh" ;
      driver.options()->setString (EL::Job::optSubmitFlags, "-L /bin/bash -q 1nd");
      driver.options()->setString (EL::Job::optFilesPerWorker,  "20");
      driver.shellInit = shellCommand.Data();
      driver.submitOnly( job, submitDir );
    }

  return 0;
}

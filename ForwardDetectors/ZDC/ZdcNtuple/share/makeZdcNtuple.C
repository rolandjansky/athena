/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


int makeZdcNtuple(std::string submitDir = "submitDir",std::string readDir = "file:/tmp/steinber",int nevt = -1,std::string enableStr = "",std::string vers = "0",int localTrackLimit = 1e6,std::string grlOption = "")
{

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
  bool isMC = 0;
  
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
  // sh.print();

  // Create an EventLoop job:
  EL::Job job;
  job.sampleHandler( sh );
  job.outputAdd (EL::OutputStream ("ANALYSIS"));

  EL::AnaAlgorithmConfig zdcAna;
  zdcAna.setType("ZdcNtuple");
  zdcAna.setName("ZdcNtupleAna");
 

  std::cout << "runtime options:" << enableStr << std::endl;
  
  bool zdcCalib = false;
  if (enableStr.find("debug")!=std::string::npos) { zdcAna.setProperty("debug", true); }
  if (enableStr.find("isMC")!=std::string::npos) { zdcAna.setProperty("isMC", true); isMC = true;}
  if (enableStr.find("slimmed")!=std::string::npos) { zdcAna.setProperty("slimmed", true); }
  if (enableStr.find("tree")!=std::string::npos) { zdcAna.setProperty("enableOutputTree", true); }
  if (enableStr.find("samples")!=std::string::npos) { zdcAna.setProperty("enableOutputSamples", true); }
  if (enableStr.find("trigger")!=std::string::npos) { zdcAna.setProperty("enableTrigger", true); }
  if (enableStr.find("zdcCalib")!=std::string::npos) { zdcAna.setProperty("zdcCalib",true); zdcCalib=true; }
  if (enableStr.find("zdcLaser")!=std::string::npos) { zdcAna.setProperty("zdcLaser", true); }
  if (enableStr.find("tracks")!=std::string::npos) { zdcAna.setProperty("enableTracks", true); }
  if (enableStr.find("clusters")!=std::string::npos) { zdcAna.setProperty("enableClusters", true); }
  if (enableStr.find("TT")!=std::string::npos) { zdcAna.setProperty("enableTT", true); }
  if (enableStr.find("enablejets")!=std::string::npos) { zdcAna.setProperty("enableJets", true); }
  if (enableStr.find("disablejets")!=std::string::npos) { zdcAna.setProperty("enableJets", false); }
  if (enableStr.find("electrons")!=std::string::npos) { zdcAna.setProperty("enableElectrons", true); }
  if (enableStr.find("photons")!=std::string::npos) { zdcAna.setProperty("enablePhotons", true); }
  if (enableStr.find("trigjet")!=std::string::npos) { zdcAna.setProperty("enableTriggerJets", true); }
  if (enableStr.find("muons")!=std::string::npos) { zdcAna.setProperty("enableMuons", true); }
  if (enableStr.find("truth")!=std::string::npos) { zdcAna.setProperty("enableTruth", true); }
  if (enableStr.find("noflipdelay")!=std::string::npos) { zdcAna.setProperty("flipDelay", false); }
  if (enableStr.find("doflipdelay")!=std::string::npos) { zdcAna.setProperty("flipDelay", true); }
  if (enableStr.find("trackLimitReject")!=std::string::npos) { zdcAna.setProperty("trackLimitReject", true); }
  if (enableStr.find("reprocZdc")!=std::string::npos) { zdcAna.setProperty("reprocZdc", true);zdcAna.setProperty("auxSuffix","RP"); }
  if (enableStr.find("7samp")!=std::string::npos) { zdcAna.setProperty("nsamplesZdc", 7); }
  if (enableStr.find("15samp")!=std::string::npos) { zdcAna.setProperty("nsamplesZdc", 15); }
  if (enableStr.find("noGRL")!=std::string::npos) { zdcAna.setProperty("useGRL", false);}
  if (enableStr.find("express2016A")!=std::string::npos) { zdcAna.setProperty("express2016A", true);}
  if (enableStr.find("upc2015")!=std::string::npos) { zdcAna.setProperty("upc2015", true);}
  if (enableStr.find("mb2015")!=std::string::npos) { zdcAna.setProperty("mb2015", true);}
  if (enableStr.find("upcL2015")!=std::string::npos) { zdcAna.setProperty("upcL2015", true);}
  if (enableStr.find("main2016A")!=std::string::npos) { zdcAna.setProperty("main2016A", true);}
  if (enableStr.find("upc2016A")!=std::string::npos) { zdcAna.setProperty("upc2016A", true);}
  if (enableStr.find("express2016B")!=std::string::npos) { zdcAna.setProperty("express2016B", true);}
  if (enableStr.find("main2016B")!=std::string::npos) { zdcAna.setProperty("main2016B", true);}
  if (enableStr.find("upc2016B")!=std::string::npos) { zdcAna.setProperty("upc2016B", true);}
  if (enableStr.find("upc2016C")!=std::string::npos) { zdcAna.setProperty("upc2016C", true);}
  if (enableStr.find("mboverlay2016")!=std::string::npos) { zdcAna.setProperty("mboverlay2016", true);}
  if (enableStr.find("upc2018")!=std::string::npos) { zdcAna.setProperty("upc2018", true);}
  if (enableStr.find("mb2018")!=std::string::npos) { zdcAna.setProperty("mb2018", true);}

  if (enableStr.find("zdc2015")!=std::string::npos) { zdcAna.setProperty("zdcConfig","PbPb2015");zdcAna.setProperty("doZdcCalib",true); }
  if (enableStr.find("zdc2016")!=std::string::npos) { zdcAna.setProperty("zdcConfig","pPb2016");zdcAna.setProperty("doZdcCalib",false); }
  if (enableStr.find("zdc2018")!=std::string::npos) { zdcAna.setProperty("zdcConfig","PbPb2018");zdcAna.setProperty("doZdcCalib",true); }

  if (enableStr.find("writeOnlyTriggers")!=std::string::npos) { zdcAna.setProperty("writeOnlyTriggers", true);}
   if (grlOption != "")
    {
      zdcAna.setProperty("grlFilename",grlOption);
      zdcAna.setProperty("useGRL",true);
    }
  else
    {
      zdcAna.setProperty("useGRL",false);
    }
  
  zdcAna.setProperty("trackLimit", localTrackLimit);

  job.algsAdd(zdcAna);

  // Add ntuple service
  /*
  EL::OutputStream output  ("output");
  job.outputAdd (output);
  EL::NTupleSvc *ntuple = new EL::NTupleSvc ("output");
  // configure ntuple object
  job.algsAdd (ntuple);
  */

  // Add my algorithms
  //job.algsAdd( zdcAna );

  if (nevt>-1) job.options()->setDouble (EL::Job::optMaxEvents, nevt);
  std::cout << "Running " << nevt << " events" << std::endl;
  
  if (file_mode||eos_mode)
    {
      // Run the job using the local/direct driver:
      EL::DirectDriver driver;
      if (zdcCalib||isMC) 
	{
	  std::cout << "Setting athena access mode" << std::endl;
	  job.options()->setString(EL::Job::optXaodAccessMode, EL::Job::optXaodAccessMode_athena);
	}
      driver.submit( job, submitDir );
      //if (zdcAna->zdcCalib) job.options()->setString (EL::Job::optXaodAccessMode, EL::Job::optXaodAccessMode_branch);
    }
  
  if (grid_mode)
    {
      EL::PrunDriver driver;
      driver.options()->setString("nc_outputSampleName", "user.steinber.%in:name[0]%.%in:name[1]%.%in:name[2]%.%in:name[3]%.%in:name[4]%.%in:name[5]%.%in:name[6]%."+vers);
      driver.options()->setDouble(EL::Job::optGridMergeOutput, 1); //run merging jobs for all samples before downloading (recommended) 
      if (zdcCalib)
	{
	  driver.options()->setString(EL::Job::optGridNFilesPerJob,  "1"); //By default, split in as few jobs as possible      	  
	}
      else
	{
	  driver.options()->setString(EL::Job::optGridNFilesPerJob,  "5"); //By default, split in as few jobs as possible      
	}
      
      //driver.options()->setString(EL::Job::optGridSite,"CERN-PROD");
      driver.submitOnly( job, submitDir );
    }

  if (eoslsf_mode)
    {
      EL::LSFDriver driver;
      TString shellCommand = "export export LSB_JOB_REPORT_MAIL=N; export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase &&";
      shellCommand += "source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh" ;
      driver.options()->setString (EL::Job::optSubmitFlags, "-L /bin/bash -q 1nd");
      driver.options()->setString (EL::Job::optFilesPerWorker,  "1");// was 20 for normal work
      driver.shellInit = shellCommand.Data();
      driver.submitOnly( job, submitDir );
    }

  return 0;
}

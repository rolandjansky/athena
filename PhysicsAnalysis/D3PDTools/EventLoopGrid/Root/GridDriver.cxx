/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventLoopGrid/GridDriver.h"
#include "EventLoopGrid/GridWorker.h"
#include "EventLoop/Algorithm.h"
#include "EventLoop/Job.h"
#include "EventLoop/OutputStream.h"
#include <PathResolver/PathResolver.h>
#include "RootCoreUtils/Assert.h"
#include "RootCoreUtils/StringUtil.h"
#include <RootCoreUtils/ThrowMsg.h>
#include "RootCoreUtils/hadd.h"
#include "SampleHandler/MetaFields.h"
#include "SampleHandler/MetaObject.h"
#include "SampleHandler/Sample.h"
#include "SampleHandler/SampleGrid.h"
#include "SampleHandler/SampleHandler.h"
#include "TFile.h"
#include "TList.h"
#include "TRegexp.h"
#include "TSystem.h"
#include "TTree.h"
#include "TObjString.h"
#include "TObjArray.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

ClassImp(EL::GridDriver)

namespace EL {

  std::string getRootCoreConfig ()
  {
    boost::regex expr (".*-.*-.*-.*");

     const char *ROOTCORECONFIG = getenv ("ROOTCORECONFIG");
     if (ROOTCORECONFIG)
     {
       if (RCU::match_expr (expr, ROOTCORECONFIG))
	 return ROOTCORECONFIG;
       std::cout << "ERROR: invalid value \"" << ROOTCORECONFIG
		 << "\" for $ROOTCORECONFIG" << std::endl;
     }
     const char *rootCmtConfig = getenv ("rootCmtConfig");
     if (rootCmtConfig)
     {
       if (RCU::match_expr (expr, rootCmtConfig))
	 return rootCmtConfig;
       std::cout << "ERROR: invalid value \"" << rootCmtConfig
		 << "\" for $rootCmtConfig" << std::endl;
     }
     std::cout <<
       "WARNING: no valid value for cmt config found.\n"
       "WARNING: using \"x86_64-slc6-gcc47-opt\" instead.\n"
       "WARNING: consider updating to a more recent RootCore version\n"
       "WARNING: or analysis release\n" << std::endl;
     return "x86_64-slc6-gcc47-opt";
   }



   std::string GridDriver::gangaLogFile = "";

   namespace {

     const int dummyIdCompleted = -0xDEAD;
     const int dummyIdFailed = -0xFA11; 

     const char* cvmfsATLASLocalRootBase = 
       "/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase";    
     const char* shAtlasLocalSetup = 
       ". ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh";    
     const char* gangaDir = 
       gSystem->ExpandPathName("~/gangadir-EL");

     struct GangaTrfDef {
       std::string sampleName;
       std::string inDS;
       std::string outDS;
       std::string rootVer;
       std::string inputSdBox;
       std::string inputFiles;
       std::string outputFiles;
       std::string downloadDir;
       std::string downloadMask;
       std::string userArea;
     }; 

     struct EnvReqs {
       bool needGanga; 
       bool needPanda; 
       bool needProxy; 
       bool needRootCore; 
       bool needDQ2; 
       EnvReqs() : needGanga(false),
		   needPanda(false),
		   needProxy(false),
		   needRootCore(false),
		   needDQ2(false) {}
     };

     /*
     bool readJobInfo(const std::string location, bool printInfo = true);
     */

     /*
     bool readJobInfo(const std::string location, Int_t &nEventsTotal, 
		      Int_t &nFilesTotal, Float_t &realTimeTotal,
		      Float_t &cpuTimeTotal, Float_t &eventsPerRealSecondTotal,
		      Float_t &eventsPerCpuSecondTotal, bool printInfo);
     */

     /*
     bool readJobInfo(const std::string location, Int_t &nEventsTotal, 
		      Int_t &nFilesTotal, bool  printInfo = false);
     */

     std::string getStrValues(const std::string &inLines, const std::string &key);

     const std::string pyList(const std::string &list);

     bool checkEnvironment(EnvReqs args);

     const std::string getNickname();

     bool sendGangaCmd(const std::string &cmd);

     bool sendGangaCmd(const std::string &cmd, std::string &out);

     const std::string gangaTrfCmd(const GangaTrfDef &args, 
				   const GridDriver &driver);

     int readTaskID(const std::string& location);
     bool writeTaskID(const std::string& location, const int taskId);

   } //namespace
 } // namespace EL

 void EL::GridDriver::testInvariant() const {
   RCU_INVARIANT (this != 0);
 }

 EL::GridDriver::GridDriver() :  express(false),
				 mergeOutput(false),				
				 memory(-1),
				 maxCpuCount(-1),
				 nFiles(0),
				 nFilesPerJob(0),
				 nJobs(-1),
				 nMinorRetries(4),
				 nMajorRetries(4),
				 nGBPerJob(13),
				 rootVer(gROOT->GetVersion()),
				 cmtConfig(getRootCoreConfig()) {
   RCU_NEW_INVARIANT (this);
 }

 void EL::GridDriver::doSubmit(const EL::Job& job,  
			       const std::string& location) const {
   RCU_READ_INVARIANT(this);
   RCU_REQUIRE(not location.empty());
   using namespace std;

   //Parent class ensures location is absolute, but if originally specified 
   //as a relative path, it may still contain '..' which will cause trouble
   //later as dq2 -H option used by Ganga Tasks requires the canonical path
   string jobDir = location;
   {
     Ssiz_t len, pos;
     TString tsLocation(location.c_str());
     const char *noDir = "/[A-Za-z0-9_\\.-]+/\\.\\.";      
     while ((pos = TRegexp(noDir).Index(tsLocation,&len,0)) != -1) 
       tsLocation.Remove(pos, len);      
     jobDir = tsLocation.Data();
   }

   //Create input sandbox dir with ELG files
   //TODO: some error checks here 
   const string jobELGDir   = jobDir + "/elg";
   const string taskIdFile  = jobELGDir + "/taskID";
   const string dsContFile  = jobELGDir + "/outputDQ2container";
   const string sandboxDir  = jobELGDir + "/inputsandbox";
   const string downloadDir = jobELGDir + "/download";
   const string runShFile   = sandboxDir + "/runjob.sh";
   //const string runShOrig   = "$ROOTCOREBIN/data/EventLoopGrid/runjob.sh"; 
   const std::string runShOrig = PathResolverFindCalibFile("EventLoopGrid/runjob.sh");
   const string mergeShFile = sandboxDir + "/elg_merge";
   //const string mergeShOrig = 
   //  "$ROOTCOREBIN/user_scripts/EventLoopGrid/elg_merge";
   const std::string mergeShOrig = PathResolverFindCalibFile("EventLoopGrid/elg_merge");
   const string jobDefFile  = sandboxDir + "/jobdef.root";
   gSystem->Exec(Form("mkdir -p %s", jobELGDir.c_str()));
   gSystem->Exec(Form("mkdir -p %s", sandboxDir.c_str()));
   gSystem->Exec(Form("mkdir -p %s", downloadDir.c_str()));
   gSystem->Exec(Form("cp %s %s", runShOrig.c_str(), runShFile.c_str()));
   gSystem->Exec(Form("chmod +x %s", runShFile.c_str()));
   gSystem->Exec(Form("cp %s %s", mergeShOrig.c_str(), mergeShFile.c_str()));
   gSystem->Exec(Form("chmod +x %s", mergeShFile.c_str()));

   if (gSystem->Exec("ps aux | grep `whoami` | grep -e [g]anga > /dev/null") == 0) {
     if (not stopServer()) {
       cerr << "Could not stop currently running ganga service" << endl;
       return;
    }
    gSystem->Exec("sleep 3");
  }

  cout << "Please wait while grid jobs are created, ";
  cout << "this can take several minutes..." << endl;

  EnvReqs env;
  env.needGanga = true; 
  env.needPanda = true; 
  env.needProxy = true; 
  env.needRootCore = true; 
  
  if (not checkEnvironment(env)) return;

  const string nickname = getNickname();
  if (nickname[0] == '\0') {
    cerr << "Error: Failed to get grid nickname from panda"; 
    return;
  }    

  TList outputs;

  {//Save the Algorithms and sample MetaObjects to be sent with the jobs 
    TFile f(jobDefFile.c_str(), "RECREATE"); 
    f.WriteTObject(&job.jobConfig(), "jobConfig", "SingleKey");      

    for (EL::Job::outputIter out = job.outputBegin(),
	   end = job.outputEnd(); out != end; ++out) {
      outputs.Add(out->Clone());
    }      
    f.WriteTObject(&outputs, "outputs", "SingleKey");      
    for (SH::SampleHandler::iterator sample = job.sampleHandler().begin();
	 sample != job.sampleHandler().end();  ++sample) {
      SH::MetaObject meta(*(*sample)->meta());
      meta.fetchDefaults(*job.options());
      meta.fetchDefaults(*options());
      f.WriteObject(&meta, (*sample)->name().c_str());
      //f.WriteObject((*sample)->meta(), (*sample)->name().c_str());
    }
    f.Close();      
  }
  SH::MetaObject meta(*job.options());
  meta.fetchDefaults(*options());
    
  map<string, SH::SampleHandler> outMap; // <label,samples>     
  list<std::string> outDSs; //Created dq2 datasets for output 

  int iJob = 0;

  std::stringstream gangaCmd;

  for (SH::SampleHandler::iterator sample = job.sampleHandler().begin();
       sample != job.sampleHandler().end(); ++sample) {

    TString inDS;
    TString outDS;
      
    iJob++;

    //If SampleGrid, loop over childs if SampleComposite
    inDS = (*sample)->meta()->castString("nc_grid", (*sample)->name(), SH::MetaObject::CAST_NOCAST_DEFAULT);
      
    // Determine outDS name
    if (not outputSampleName.empty()) 
      outDS = outputSampleName;
    else  //TODO: Decide on a good default here
      outDS = "user.%nickname%.%in:name%";

    outDS.ReplaceAll("%in:name%", (*sample)->name());
    outDS.ReplaceAll("%nickname%", nickname);
    std::stringstream ss((*sample)->name());
    std::string item;
    int field = 0;
    while(std::getline(ss, item, '.')) {
      std::stringstream sskey;
      sskey << "%in:name[" << ++field << "]%";
      outDS.ReplaceAll(sskey.str(), item);
    }	
    while (outDS.Index("%in:") != -1) {
      int i1 = outDS.Index("%in:");
      int i2 = outDS.Index("%", i1+1);
      TString metaName = outDS(i1+4, i2-i1-4);
      outDS.ReplaceAll("%in:"+metaName+"%", 
		       (*sample)->meta()->castString(std::string(metaName.Data()), "", SH::MetaObject::CAST_NOCAST_DEFAULT));
    }
    outDS.ReplaceAll("/", "");
    if (!outDS.BeginsWith(Form("user.%s", nickname.c_str()))) {
      //ERROR: Malformed outDS
      cerr << "Error: Bad output dataset name: " << outDS << endl;
      cerr << "It should begin with " << Form("user.%s", nickname.c_str()) << endl;
      return;
    }
      
    string outputfilenames = "hist-output.root";
    {
      TIter itr(&outputs);
      TObject *obj = 0;
      while ((obj = itr())) {
	EL::OutputStream *os = dynamic_cast<EL::OutputStream*>(obj);
	const string name = os->label() + ".root";
	outputfilenames += "," + name;
      }
    }
      
    gSystem->Exec(Form("mkdir -p %s", 
		       (downloadDir + "/" + (*sample)->name()).c_str()));

    GangaTrfDef trfDef;
    trfDef.sampleName = (*sample)->name();
    trfDef.inDS = inDS.Data();
    trfDef.outDS = outDS.Data();
    trfDef.inputSdBox = 
      extFile + (extFile.empty() ? "" : ",") + jobDefFile + "," + runShFile;
    trfDef.inputFiles = 
      (*sample)->meta()->castString("nc_grid_filter", "*.root*", SH::MetaObject::CAST_NOCAST_DEFAULT);
    trfDef.outputFiles = outputfilenames;    
    trfDef.downloadDir = downloadDir;
    trfDef.downloadMask = "hist-output.root";
    trfDef.userArea = jobELGDir;
    trfDef.rootVer = rootVer; 

    gangaCmd << endl << "# Transform " << iJob << endl; 
    gangaCmd << gangaTrfCmd(trfDef, *this);
    gangaCmd << endl;

    outDSs.push_back(std::string(outDS.Data()));	
  }

  string taskName = location;
  if (taskName.rfind('/') != string::npos)
    taskName = taskName.substr(taskName.rfind('/')+1);
  stringstream submitCmd;
  submitCmd   
    << "# This file was generated by EventLoop GridDriver "
    << "for EL job " << taskName << "\n"
    << "t = AtlasTask()\n"
    << "t.name = '" << taskName << "'\n" 
    << "t.comment += 'location:" << location << "'\n"
    << "t.float = 200\n"
    << "app = Athena()\n"
    << "app.athena_compile = True\n"
    << "app.option_file = 'runjob.sh'\n"
    << "app.atlas_exetype = 'EXE'\n"
    << "app.useRootCore = True\n"
    << "app.append_to_user_area = ['.root','.cxx','.C','.h']\n"
    << "app.user_area_path = '" << jobELGDir << "'\n"
    << "app.atlas_release='None'\n"     
    << "app.atlas_cmtconfig = '" << meta.castString (Job::optCmtConfig, cmtConfig) << "'\n" 
    << "app.prepare()\n"
    << "app.atlas_release=''\n"
    << "app.atlas_dbrelease=''\n"
    << "config.Athena.EXE_MAXFILESIZE=2097152\n"
    << "config.Configuration.autoGenerateJobWorkspace = False\n"
    << "print\n"
    << "print \"TaskID: %d\" % t.id\n"
    << "print \"TaskContainer: %s\" % t.getContainerName()\n"
    << gangaCmd.str()
    << "t.run()\n" 
    << "import time\n"
    << "time.sleep(60)\n";

  {
    ofstream of((jobELGDir + "/submit.py").c_str()); 
    of << submitCmd.str();
    of.close();
  }

  string curDir = gSystem->pwd();
  gSystem->cd(sandboxDir.c_str());
  string gangaMsg;
  bool ok = sendGangaCmd(submitCmd.str(), gangaMsg);
  gSystem->cd(curDir.c_str());

  if (not ok) {
    cerr << "Could not start ganga service - job submission failed" << endl;
    return;
  }

  cout << "Done. Call EL::GridDriver::status(\"" << location 
       << "\") to follow the progress of your jobs." << endl;

  int taskId = -1;
  string container = getStrValues(gangaMsg, "TaskContainer: ");
  istringstream(getStrValues(gangaMsg, "TaskID: ")) >> taskId;
  RCU_ASSERT(taskId >= 0);
  writeTaskID(location, taskId);

  {
    ofstream of(dsContFile.c_str());
    of << container;
    of.close();
  }
  
  stringstream queryOutDsCmd;
  queryOutDsCmd << "for tr in tasks(" << taskId << ").transforms: "
		<< "print \"%(in)s: %(out)s \" % "
		<< "{\"in\": tr.application.options, "
		<< "\"out\": tr.getContainerName()}\n";
  sendGangaCmd(queryOutDsCmd.str(), gangaMsg);

  for (SH::SampleHandler::iterator sample = job.sampleHandler().begin();
       sample != job.sampleHandler().end(); ++sample) {
    string sampleOutDs = getStrValues(gangaMsg, (*sample)->name() + ": ");
    if (*sampleOutDs.rbegin() == '\n')
     sampleOutDs  = sampleOutDs.substr(0, sampleOutDs.size()-1);
    if (*sampleOutDs.rbegin() == ' ')
     sampleOutDs  = sampleOutDs.substr(0, sampleOutDs.size()-1);

    //Create a sample for each output and add it to that output's handler 
    for (EL::Job::outputIter out=job.outputBegin();
	 out != job.outputEnd(); ++out) {
      SH::SampleGrid * mysample = new SH::SampleGrid((*sample)->name());
      mysample->meta()->setString("nc_grid", sampleOutDs);
      mysample->meta()->setString("nc_grid_filter", "*" + out->label() + ".root*");
      outMap[out->label()].add(mysample);
    }
    SH::SampleGrid * mysample=new SH::SampleGrid((*sample)->name());
    mysample->meta()->setString("nc_grid", sampleOutDs);
    mysample->meta()->setString("nc_grid_filter", "hist-output.root");
    //mysample->setMetaDouble("norigfiles", nFilesInInputDS);
    outMap["hist"].add(mysample);                	          
  }

  //Print the created dq2 datasets
  //This is broken as Tasks appends extra fluff to the ds names
  //outDSs.sort();
  //outDSs.unique();
  //cout << "The following dq2 output datasets will be created:" << endl;
  //for (list<string>::iterator it=outDSs.begin(); it!=outDSs.end(); ++it) {
  //  cout << *it << endl;
  //}

  // //Print link to Analysis Task Monitor
  // stringstream url;
  // url << "https://dashb-atlas-prodsys-prototype.cern.ch/templates/task-analysis/#user=default";
  // url << "&table=Mains"; 
  // url << "&p=1";
  // url << "&records=" << outDSs.size() + 1;
  // url << "&activemenu=1";
  // url << "&timerange=lastMonth";
  // //url << "&from=";
  // //url << "&till=";
  // //url << "&refresh=0";
  // url << "&pattern=";
  // url << container << "/+%7C%7C+";
  // //for (list<string>::iterator it=outDSs.begin(); it!=outDSs.end(); ++it) {    
  // //  url << *it << "/+%7C%7C+";
  // //}
  // cout << "You can follow the job progress using the Analysis Task Monitor:"; 
  // cout << endl << url.str() << endl;
  // cout << "(Please note: this service is still under development!)\n";
    
  //Save the output Sample Handlers
  for (EL::Job::outputIter output=job.outputBegin(),
	 end=job.outputEnd(); output != end; ++output)
    {
      outMap[output->label()].fetch(job.sampleHandler());
      outMap[output->label()].save(jobDir + "/output-" + output->label());
    };
  outMap["hist"].fetch(job.sampleHandler());
  outMap["hist"].save(jobDir + "/output-hist");
    
}

 
bool EL::GridDriver::doRetrieve(const std::string& location) const {
  RCU_READ_INVARIANT(this);
  using namespace std;    

  const string hist_sh_dir    = location + "/output-hist";
  const string jobDownloadDir = location + "/elg/download";
  const string dsContFile     = location + "/elg/outputDQ2container";

  EnvReqs env;
  env.needGanga = true; 
  env.needProxy = true; 
  if (not checkEnvironment(env)) {
    //In case of failure, errors are already reported by checkEnvironment()
    return false; 
  }

  SH::SampleHandler sh;
  sh.load(hist_sh_dir);

  int taskId = readTaskID(location); 
  if (taskId == dummyIdCompleted) {
    cout << "Job has already been completed and output downloaded" << endl;
    return true;
  }
  if (taskId == dummyIdFailed) {
    cout << "The job has failed permanently and will not be tried again" << endl;
    return true;
  }
  RCU_ASSERT(taskId >= 0); //At this point, taskId must be that of a real task

  stringstream cmd;
  cmd << "for [x,y] in [[u,t] for t in tasks(" << taskId << ").transforms " 
      << "for u in t.units]: print "
      << "\"elgunit: %s %s %s\" % (y.application.options, x.name, x.status)";
 
  string out;
  bool ok = sendGangaCmd(cmd.str(), out);

  if (not ok) {
    cerr << "Could not start ganga server" << endl;
    return false;
  }
    
  const string unitList = getStrValues(out, "elgunit: ");

  if (unitList.empty()) {
    cerr << "Could not get the list of task partitions from ganga" << endl;
    cerr << "Possibly the task registry has been lost ";
    cerr << "or the job submission failed" << endl;
    return false;
  }

  bool isIncomplete = false; //At least one unit is not completed 
  bool isRunning = false;    //At least one unit is still running
  bool isFailed = false;     //At least one unit is failed
  int nTotalUnits = 0;
  int nTotalUnitsCompleted = 0;
  int nTotalTransformsCompleted = 0;

  for (SH::SampleHandler::iterator sample = sh.begin();
       sample != sh.end(); ++sample) {      

    const string mergedHistFile 
      = location + "/hist-" + (*sample)->name() + ".root";

    bool transformCompleted = true;
    string filesToMerge;

    istringstream units(unitList);
    int nUnits = 0;
    string entry;
    while (getline(units, entry)) {
      string sampleName, unitName, unitStatus;
      istringstream s(entry);
      s >> sampleName;
      s >> unitName;
      s >> unitStatus;

      if ((*sample)->name() != sampleName) continue;
      nUnits++;
      bool unitCompleted = (unitStatus == "completed");
      bool unitFailed    = (unitStatus == "failed");
      bool unitBad       = (unitStatus == "bad");
      nTotalUnits++;
      if (unitCompleted) nTotalUnitsCompleted++;
      if (unitFailed || unitBad) isFailed = true;
      if (not unitCompleted && not unitFailed && not unitBad) isRunning=true;
      if (not unitCompleted) {
	isIncomplete = true;
	transformCompleted = false;
	continue;
      }
	
      const string unitDir = 
	jobDownloadDir + "/" + (*sample)->name() + "/" + unitName;	
      const string findCmd 
	= "find " + unitDir + " -name \"*.hist-output.root*\" |  tr '\n' ' '";
      filesToMerge += gSystem->GetFromPipe(findCmd.c_str()).Data();		
    }
    if (transformCompleted) nTotalTransformsCompleted++;
    if (nUnits == 0) transformCompleted = false;
    if (transformCompleted) {
      if (gSystem->Exec(Form("[ -f %s ]", mergedHistFile.c_str()))) {
	if (filesToMerge.empty()) { //Debug
	  cerr << "Did not find files to merge\n";
	  cerr << "ganga response was " << out << endl;
	  cerr << "Stopping here" << endl;
	  return false;
	}

	istringstream iss(filesToMerge);
	set<string> input((istream_iterator<string>(iss)), 
			  istream_iterator<string>());	  
	RCU::hadd(mergedHistFile.c_str(), vector<string>(input.begin(), input.end()));

	gSystem->Exec(Form("rm %s", filesToMerge.c_str())); 	  
      }	
    }
  }

  std::cout << nTotalTransformsCompleted << "/" << sh.size() 
	    << " samples processed (" << nTotalUnitsCompleted << "/"
	    << nTotalUnits << " jobs completed)" << endl;

  if (isRunning) return false;

  //At this point, task should be either completed or failed
  if (isFailed != isIncomplete) {
    std::cerr << "At this point, task should be either completed or failed\n"
              << "somehow it's not" << std::endl;
  }

  if (isFailed) {
    cerr << "The job has failed and has reached maximum number of retries.";
    cerr << endl << "It will not be tried again. Sorry." << endl;
  }

  stringstream removeCmd;
  removeCmd << "tasks(" << taskId << ").remove(remove_jobs=True)";
  if (sendGangaCmd(removeCmd.str())) {
    writeTaskID(location, isFailed ? dummyIdFailed : dummyIdCompleted);
  } else {
    cerr << "Warning: The job has stopped running but could not be removed ";
    cerr << "from ganga. Please try calling retrieve() again later to ";
    cerr << "clean up the job records." << endl;
  }

  env.needProxy = true;
  env.needDQ2 = true;
  if (checkEnvironment(env)) {
    string container;
    ifstream f(dsContFile.c_str());
    f >> container;
    f.close();
    cout << "The following DQ2 containers have been created:" << endl;
    gSystem->Exec(("dq2-ls \"" + 
		   container.substr(0,container.length()-1) +
		   ".*/\"").c_str());
  }  
  
  return true;
}

void EL::GridDriver::status(const std::string& location) {
  using namespace std;    
  const string hist_sh_dir = location + "/output-hist";
  const string dsContFile  = location + "/elg/outputDQ2container";
  SH::SampleHandler sh;
  sh.load(hist_sh_dir);

  EnvReqs env;
  env.needGanga = true;
  env.needProxy = true;
  if (not checkEnvironment(env)) return; 

  int taskId = readTaskID(location); 
  if (taskId == dummyIdCompleted) {
    cout << "Job is completed completed and output downloaded" << endl;
    return;
  } else if (taskId == dummyIdFailed) {
    cout << "The job has failed permanently and will not be tried again" << endl;
    return;
  }
  RCU_ASSERT(taskId >= 0); 
  
  stringstream cmd;
  cmd << "for [x,y] in [[u,t] for t in tasks(" << taskId << ").transforms " 
      << "for u in t.units]: print "
      << "\"unit: %s %s\" % (x.name, x.status)";
  string out;
  bool ok = sendGangaCmd(cmd.str(), out);
  if (not ok) {
    cerr << "Could not start ganga server" << endl;
    return;
  }
  const string unitList = getStrValues(out, "unit: ");
  if (unitList.empty()) {
    cerr << "Could not get the list of task partitions from ganga" << endl;
    cerr << "Possibly the task registry has been lost" << endl;
    return;
  }
  cout << unitList;
}

void EL::GridDriver::status() {
  std::istringstream tasks(listActive());
  std::string location;
  while (getline(tasks, location)) {
    std::cout << std::endl << location << std::endl;
    status(location);
  } 
}

void EL::GridDriver::kill(const std::string& location) {
  EnvReqs env;
  env.needGanga = true;
  env.needProxy = true;
  if (not checkEnvironment(env)) return; 

  std::stringstream removeCmd;
  int taskId = readTaskID(location);
  if (taskId > 0) {
    removeCmd << "tasks(" << taskId << ").pause()";
    removeCmd << "tasks(" << taskId << ").remove(remove_jobs=True)";
    if (sendGangaCmd(removeCmd.str())) {
      writeTaskID(location, dummyIdFailed);
    } else {
      std::cerr << "Error: Could not remove the job at " << location << std::endl;
    }
  } else if (taskId == dummyIdCompleted) {
    std::cout << "The job is already completed!" << std::endl;
  }
}

void EL::GridDriver::killAll() {
  std::istringstream tasks(listActive());
  std::string location;
  std::cout << "Killing jobs:" << std::endl;
  while (getline(tasks, location)) {
    std::cout << location << std::endl;
    kill(location);
  }
}

std::string EL::GridDriver::listActive() {
  std::string gangaMsg;
  sendGangaCmd("for t in tasks: print t.comment", gangaMsg);
  return getStrValues(gangaMsg, "location:");
}

bool EL::GridDriver::startServer() {
  return sendGangaCmd("");
}

bool EL::GridDriver::stopServer() {
  EnvReqs env;
  env.needGanga = true;
  if (not checkEnvironment(env)) {
    std::cout << "Failed to set up ganga\n";
    return false;
  }
  const std::string gangaBin = gSystem->GetFromPipe("command -v ganga").Data();
  const std::string gangaInstall = gangaBin.substr(0, gangaBin.find("/bin/ganga"));
  const std::string gangaPythonDir = gangaInstall + "/python";
  std::stringstream cmd;
  cmd << "export PYTHONPATH=$PYTHONPATH:" <<  gangaPythonDir << "\n"
      << "python << EOL\n"
      << "from GangaService.Lib.ServiceAPI.ServiceAPI import GangaService\n"
      << "gs = GangaService()\n"
      << "gs.gangadir = \"" << gangaDir << "\"\n"
      << "gs.killServer()\n"
      << "EOL\n";
  return (gSystem->Exec(cmd.str().c_str()) == 0);
}

void EL::GridDriver::reset() {
  gSystem->Exec(Form("rm -rf %s", gangaDir));
}

namespace EL { 
  namespace {

    //The functions in this namespace could be split into a separate "utilities"
    //module. In the interest of one day merging this package with the main EL
    //package, they are kept here to keep the number of EventLoopGrid related 
    //files to a minimum.

    /*
    //TODO: This function is currently not used as the sample meta data
    //are not filled correctly during submission
    bool readJobInfo(const std::string location,
		     Int_t   &nEventsTotal,
		     Int_t   &nFilesTotal,
		     Float_t &realTimeTotal,
		     Float_t &cpuTimeTotal,
		     Float_t &eventsPerRealSecondTotal, 
		     Float_t &eventsPerCpuSecondTotal,
		     bool    printInfo) {
      using namespace std;
  
      TFile f(location.c_str());

      if (f.IsZombie() || not f.IsOpen()) {
	cerr << "Failed to oen file " << location << endl;
	return false;
      }

      TTree *tree = (TTree*)f.Get("EventLoopGridJobInfo");
    
      if (tree == 0) {
	cerr << "Could not read tree 'EventLoopGridJobInfo' ";
	cerr << "in file " << location << ". ";
	cerr << "File is corrupt or is not an EventLoopGrid histogram collection";
	cerr << endl;
	return false;
      }    

      nEventsTotal = 0;
      nFilesTotal = 0;
      realTimeTotal = 0;
      cpuTimeTotal = 0;
      eventsPerRealSecondTotal = 0;
      eventsPerCpuSecondTotal = 0;
    
      Int_t   nEvents = 0;
      Int_t   nFiles = 0;
      Float_t realTime = 0;
      Float_t cpuTime = 0;
      Float_t eventsPerRealSecond = 0;
      Float_t eventsPerCpuSecond = 0;

      TBranch *bnEvents = tree->GetBranch("eventsRead");
      TBranch *bnFiles = tree->GetBranch("filesRead");
      TBranch *brealTime = tree->GetBranch("loopWallTime");
      TBranch *bcpuTime = tree->GetBranch("loopCpuTime");
      TBranch *beventsPerRealSecond = tree->GetBranch("eventsPerWallSec");
      TBranch *beventsPerCpuSecond = tree->GetBranch("eventsPerCpuSec");
          
      bnEvents->SetAddress(&nEvents);
      bnFiles->SetAddress(&nFiles);
      brealTime->SetAddress(&realTime);
      bcpuTime->SetAddress(&cpuTime);
      beventsPerRealSecond->SetAddress(&eventsPerRealSecond);
      beventsPerCpuSecond->SetAddress(&eventsPerCpuSecond);
    
      if (printInfo) {
	cout << "Job information for file " << location << ":" << endl;
      }

      for (Int_t i = 0; i < tree->GetEntriesFast(); i++) {
	bnEvents->GetEvent(i);
	bnFiles->GetEvent(i);
	brealTime->GetEvent(i);
	bcpuTime->GetEvent(i);
	beventsPerRealSecond->GetEvent(i);
	beventsPerCpuSecond->GetEvent(i);

	if (printInfo) {
	  cout << "  Job " << setw(3) << setfill('0') << i;
	  cout << " Input files read: " << nFiles;
	  cout << " Events read: " << nEvents;
	  cout << " Loop real time: " << realTime;
	  cout << " Loop CPU time: " << cpuTime;
	  cout << " events per real second: " << eventsPerRealSecond;
	  cout << " events per cpu second: " << eventsPerCpuSecond;
	  cout << endl;	  
	}

	nEventsTotal += nEvents;
	nFilesTotal += nFiles;
	realTimeTotal += realTime;
	cpuTimeTotal += cpuTimeTotal;
      }
    
      eventsPerRealSecondTotal 
	= nEventsTotal / (realTimeTotal != 0 ? realTimeTotal : 1.);
      eventsPerCpuSecondTotal 
	= nEventsTotal / (cpuTimeTotal != 0 ? cpuTimeTotal : 1.);

      f.Close();

      return true;
    }
    */

    /*
    bool readJobInfo(const std::string location, bool printInfo) {
      Int_t nEventsTotal = 0, nFilesTotal = 0;
      Float_t realTimeTotal = 0, cpuTimeTotal = 0;
      Float_t eventsPerRealSecondTotal = 0, eventsPerCpuSecondTotal = 0;
      return readJobInfo(location,  nEventsTotal, nFilesTotal, realTimeTotal, 
			 cpuTimeTotal, eventsPerRealSecondTotal, 
			 eventsPerCpuSecondTotal, printInfo);
    } 
    */     

    /*
    bool readJobInfo(const std::string location,
		     Int_t &nEventsTotal, 
		     Int_t &nFilesTotal,
		     bool  printInfo) {
      Float_t realTimeTotal = 0, cpuTimeTotal = 0;
      Float_t eventsPerRealSecondTotal = 0, eventsPerCpuSecondTotal = 0;
      return readJobInfo(location,  nEventsTotal, nFilesTotal, realTimeTotal, 
			 cpuTimeTotal, eventsPerRealSecondTotal, 
			 eventsPerCpuSecondTotal, printInfo);
    }
    */

    //This function extracts substrings from a text, 
    //It is used to parse the output from GangaService.
    //Returns: "value" from every substring matching "key: value"
    std::string getStrValues(const std::string &inLines, const std::string &key) {
      std::string out;      
      TString res(inLines);
      TObjArray *lines = res.Tokenize("\n");
      TIter itr(lines);          
      TObject *obj = 0;
      while ((obj = itr())) {
	TString l = ((TObjString*)obj)->String();
	if (l.Index(key.c_str()) != -1) {
	  TString val = l(l.Index(key.c_str())+key.length(), l.Length());
	  out += val.Data();
	  out += "\n";
	}
      }
      delete lines;
      return out;
    }

    //This function takes a string like "var1 var2 var3" and formats it as a
    //python list of strings. It is used in the creation of python commands.
    //Returns: A string of format "['var1','var2','var3']".
    const std::string pyList(const std::string &list) {
      if (list[0] == '\0') return "";
      TString s(("['" + list + "']").c_str());
      s.ReplaceAll(" ", "");
      s.ReplaceAll(",", "','");
      return s.Data();
    }

    //This function performs a number of checks on the environment we are
    //running in. If necessary, it tries to set up the requested components.
    bool checkEnvironment(EnvReqs env) {
      using namespace std;
    
      if (env.needProxy) env.needPanda = true;

      // if (env.needRootCore && not gSystem->Getenv("ROOTCOREBIN")) {
      // 	cerr << "Error: Must set up RootCore first" << endl;
      // 	return false;
      // }

      env.needPanda &= bool(gSystem->Exec("command -v prun >/dev/null"));
      env.needGanga &= bool(gSystem->Exec("command -v ganga >/dev/null"));

      bool needALRB = (env.needPanda || env.needGanga);
      needALRB &= ( ! gSystem->Getenv("ATLAS_LOCAL_ROOT_BASE"));

      if (needALRB) {
	if (gSystem->Exec(Form("[ -d %s ]", cvmfsATLASLocalRootBase)) == 0)
	  gSystem->Setenv("ATLAS_LOCAL_ROOT_BASE", cvmfsATLASLocalRootBase);      
	else {
	  cerr << "ERROR: variable ATLASLocalRootBase is not set "
	       << "and CVMFS ATLASLocalRootBase is not available - "
	       << "can't set up ATLAS software on this computer.\n";
	  return false;
	}
      }

      if (env.needPanda || env.needGanga) {      
	try {	
	  stringstream cmd;
	  cmd << "set > __env1" << endl;
	  cmd << shAtlasLocalSetup << " > /dev/null 2>&1" << endl;
	  if (env.needPanda) 
	    cmd << "localSetupPandaClient --noAthenaCheck >/dev/null 2>&1"<<endl;
	  if (env.needGanga) 
	    cmd << "localSetupGanga > /dev/null 2>&1" << endl;
	  if (env.needDQ2)
	    cmd << "localSetupDQ2Client --quiet" << endl;
	  cmd << "set > __env2" << endl;
	  cmd << "grep -v -Fx -f __env1 __env2" << endl;
	  cmd << "rm -f __env1 __env2" << endl;      
	  string env = gSystem->GetFromPipe(cmd.str().c_str()).Data();
	  istringstream ssEnv(env);
	  string line;
	  while(getline(ssEnv, line)) {
	    TString s(line.c_str());
	    gSystem->Setenv(TString(s(0,s.Index("="))).Data(),	
			    TString(s(s.Index("=")+1, s.Length())).Data());
	  }
	}
	catch (...) {
	  cerr << "Error setting up ATLAS software, see log for details";
	  gSystem->Exec("rm -f __env1 __env2");  
	  return false;
	}                  
      }
    
      if (env.needPanda && bool(gSystem->Exec("command -v prun>/dev/null"))) {
	cerr << "Error setting up panda!!\n";
	return false;
      }

      for (int retry = 0; env.needProxy && retry < 3; retry++) {
	const char pyCheckProxy[] = 
	  "from pandatools import PsubUtils;"
	  "PsubUtils.checkGridProxy('',False,False,None);";
	env.needProxy &= 
	  bool(gSystem->Exec(Form("python -c \"%s\"", pyCheckProxy)));    
      }

      if (env.needProxy) {
	cerr << "Error: Failed to set up grid proxy" << endl;
	return false;
      }

      return true;
    }

    //This function returns the users grid nickname. 
    //It is the callers responsibility to ensure that panda tools 
    //and voms proxy are available. 
    const std::string getNickname() {
      const char pyGetNickName[] =
	"from pandatools import PsubUtils;"
	"print PsubUtils.getNickname();";
      return gSystem->GetFromPipe(Form("python -c \"%s\" 2>/dev/null", 
				       pyGetNickName)).Data();    
    }
    
    //This function sends a command to the ganga service.
    //Returns: true if the command was sent succesfully, otherwise false.
    bool sendGangaCmd(const std::string &cmd, std::string &out) {
      using namespace std;

      EnvReqs env;
      env.needGanga = true;
      env.needProxy = true;
      if (not checkEnvironment(env)) return false;

      const string gangaCfg = "RUNTIME_PATH=GangaAtlas:GangaPanda";
      const string gangaArg = "-o[Configuration]" + gangaCfg;
      const string gangaBin = gSystem->GetFromPipe("command -v ganga").Data();
      const string gangaInstall = gangaBin.substr(0, gangaBin.find("/bin/ganga"));
      const string gangaPythonDir = gangaInstall + "/python";

      stringstream sendCmd;
      sendCmd
	<< "export PYTHONPATH=$PYTHONPATH:" <<  gangaPythonDir << "\n"
	<< "python << EOL\n"
	<< "from GangaService.Lib.ServiceAPI.ServiceAPI import GangaService\n"
	<< "import time\n"
	<< "gs = GangaService()\n"
	<< "gs.gangadir = \"" << gangaDir << "\"\n"
	<< "gs.gangacmd = \"" << gangaBin << " " << gangaArg << "\"\n"
	<< "gs.timeout = 24*60\n"      
	<< "if not gs.startServer():\n"
	<< "    gs.killServer()\n" 
	<< "    if not gs.startServer():\n"
	<< "        time.sleep(30)\n"      
	<< "        if not gs.startServer():\n"
	<< "            gs.killServer()\n" 
	<< "            time.sleep(60)\n"      
	<< "            if not gs.startServer():\n"
	<< "                print \"Failed to start server\"\n" 
	<< "                exit(0)\n"
	<< "print gs.sendCmd(\"\"\"\n"
	<< cmd
	<< "\"\"\")\n"
	<< "EOL\n";    

      out = gSystem->GetFromPipe(sendCmd.str().c_str()).Data();

      if (out.find("Failed to start server") != string::npos) return false;

      if (not GridDriver::gangaLogFile.empty()) {
      	ofstream f(GridDriver::gangaLogFile.c_str(), ios::out | ios::app);
	f << "COMMAND:" << endl << cmd << endl;
	f << "RESPONSE:" << endl << out << endl;
	f.close();
      }

      istringstream stream(out);
      string line;
      while (std::getline(stream, line)) {
	if (line.find(": ERROR") != string::npos) {
	  cout << line << endl;
	}
      }

      return true;
    }    

    bool sendGangaCmd(const std::string &cmd) {
      std::string out;
      return sendGangaCmd(cmd, out);
    }

    //This function generates python code to create a Ganga Transform
    const std::string gangaTrfCmd(const GangaTrfDef &args, 
				  const GridDriver &driver) {
      RCU_REQUIRE("" != args.sampleName);
      RCU_REQUIRE("" != args.inDS);
      RCU_REQUIRE("" != args.outDS);
      RCU_REQUIRE("" != args.inputSdBox);
      RCU_REQUIRE("" != args.outputFiles);
      RCU_REQUIRE("" != args.downloadDir);
      
      using namespace std;      
      
      std::stringstream gangaCmd;
      gangaCmd << "trf = AtlasTransform()" << endl;        
      gangaCmd << "trf.name = '" 
	       << args.outDS.substr(args.outDS.find('.', 5) + 1) << "'" << endl;
      gangaCmd << "t.appendTransform(trf)" << endl;
      if (*args.inDS.rbegin() == '/') {
	gangaCmd << "trf.initializeFromContainer('" 
		 << args.inDS << "')" << endl;
      } else {
	gangaCmd << "trf.initializeFromDatasets(" 
		 << pyList(args.inDS) << ")" << endl;
      }
      gangaCmd << "for u in trf.units: u.inputdata.names_pattern = "
	       << pyList(args.inputFiles) << endl;
      gangaCmd << "trf.setRunLimit(" 
	       << driver.nMinorRetries + driver.nMajorRetries << ")" << endl;
      gangaCmd << "trf.setMinorRunLimit(" << driver.nMinorRetries << ")" << endl;
      gangaCmd << "trf.setMajorRunLimit(" << driver.nMajorRetries << ")" << endl;
      gangaCmd << "trf.application = app" << endl;
      gangaCmd << "trf.application.options = '" << args.sampleName << "'" << endl;
      gangaCmd << "trf.outputdata = DQ2OutputDataset()" << endl;
      gangaCmd << "trf.outputdata.datasetname = '" << args.outDS << "'" << endl;
      gangaCmd << "trf.outputdata.outputdata = " 
	       << pyList(args.outputFiles) << endl;
      gangaCmd << "trf.copy_output = DQ2OutputDataset()" << endl;
      gangaCmd << "trf.copy_output.datasetname = '" << args.outDS << "'" << endl;
      gangaCmd << "trf.copy_output.outputdata = " 
	       << pyList(args.outputFiles) << endl;
      gangaCmd << "trf.backend = Jedi()" << endl;    
      gangaCmd << "trf.backend.requirements.rootver = '" 
               << args.rootVer << "'" << endl;    
      gangaCmd << "trf.local_location = '" 
	       << args.downloadDir + "/" + args.sampleName << "'" << endl; 
      gangaCmd << "trf.include_file_mask = " 
	       << pyList(args.downloadMask) << endl;      
      if (0 < driver.nFiles)
	gangaCmd << "for u in trf.units: u.inputdata.number_of_files = "
		 << driver.nFiles << endl;
      if (0 < driver.nFilesPerJob)
	gangaCmd << "trf.files_per_job = " << driver.nFilesPerJob << endl;
      if (0 < driver.nGBPerJob)
	gangaCmd << "trf.MB_per_job = " << (int)(driver.nGBPerJob*1000) << endl;
      if (0 < driver.nJobs)
	gangaCmd << "trf.subjobs_per_unit = " << driver.nJobs << endl;
      if (not driver.destSE.empty())
	gangaCmd << "trf.outputdata.location = '" << driver.destSE << "'" << endl;
      if (not driver.site.empty())  
	gangaCmd << "trf.backend.site = '" << driver.site << "'" << endl;
      if (0 < driver.memory) 
	gangaCmd << "trf.backend.requirements.memory = " << driver.memory << endl;
      if (0 < driver.maxCpuCount)
	gangaCmd << "trf.backend.requirements.cputime = " 
		 << driver.maxCpuCount << endl;
      if (driver.express)   
	gangaCmd << "trf.backend.requirements.express = True" << endl; 
      if (driver.mergeOutput)  
	gangaCmd << "trf.backend.requirements.enableMerge = True" << endl; 
      gangaCmd << "trf.backend.requirements.configMerge['exec'] = "
	       << "'elg_merge jobdef.root %OUT %IN'" << endl;
      if (not driver.excludedSite.empty())  
	gangaCmd << "trf.backend.requirements.excluded_sites = " 
		 << pyList(driver.excludedSite) << endl; 
      if (not driver.cloud.empty())  
	gangaCmd << "trf.backend.requirements.cloud = " << driver.cloud << endl
		 << "trf.backend.requirements.anyCloud = False" << endl;
      return gangaCmd.str();
    }
   
    int readTaskID(const std::string& location) {
      RCU_REQUIRE(not location.empty());
      const std::string taskIdFile = location + "/elg/taskID";
      int taskId = -1;       
      std::ifstream f;
      try {
	f.open(taskIdFile.c_str());
	f >> taskId;      
      } catch (...) {
	std::cerr << "Could not read taskID from " << taskIdFile << std::endl;
      }
      f.close();
      return taskId;
    }

    bool writeTaskID(const std::string& location, const int taskId) {
      RCU_REQUIRE(not location.empty());
      const std::string taskIdFile = location + "/elg/taskID";
      try {
	std::ofstream f(taskIdFile.c_str());
	f << taskId;
	f.close();
      } catch (...) {
	return false;
      }
      return true;
    }

  } //namespace
} //namespace EL




//Method stubs for obsolete functions

void EL::GridDriver::gather(const std::string location) const {
  RCU_READ_INVARIANT(this);
  using namespace std;
  cerr << "GridDriver::gather(): This function is obsolete and ";
  cerr << "will be removed in a future version. ";
  cerr << "Please use retrieve() or wait() instead." << endl;
  doRetrieve(location); 
}

SH::SampleGrid* EL::GridDriver::createSampleFromDQ2(const std::string& dataset)
  const {
  RCU_READ_INVARIANT(this);
  using namespace std;
  cerr << "GridDriver::createSampleFromDQ2(): This function is obsolete and ";
  cerr << "will be removed in a future version. ";
  cerr << "Please use SH::scanDQ2() instead." << endl;
  std::string name=dataset;
  size_t pos=0;
  while((pos=name.find("/", pos)) != std::string::npos) 
    name.replace(pos++, 1, "_");
  SH::SampleGrid * mysample=new SH::SampleGrid(name);
  mysample->meta()->setString("nc_grid", dataset);
  mysample->meta()->setString("nc_grid_filter", "*");          
  return mysample;
}

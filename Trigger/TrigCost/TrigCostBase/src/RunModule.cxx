/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <algorithm>
#include <iostream>
#include <sstream>

#ifdef ANP_CPU_PROFILER
#include <google/profiler.h>
#endif

#ifdef ANP_HEAP_PROFILER
#include <google/heap-profiler.h>
#endif

// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TStopwatch.h"
#include "TSystem.h"

// Local
#include "TrigCostBase/EventSvc.h"
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/HistMan.h"
#include "TrigCostBase/NtupleSvc.h"
#include "TrigCostBase/Thread.h"
#include "TrigCostBase/RunModule.h"

using namespace std;

namespace Anp
{
  //-----------------------------------------------------------------------------
  // Helper functions
  //
  std::string PrintStopWatch(TStopwatch &watch)
  {
    watch.Stop();

    double realt = watch.RealTime();
    double cput  = watch.CpuTime();
      
    watch.Reset();
    watch.Start();

    const int hours = static_cast<int>(realt/3600.0);
    const int  min  = static_cast<int>(realt/60.0) - 60*hours;
    
    realt -= hours * 3600;
    realt -= min * 60;
    
    if (realt < 0) realt = 0;
    if (cput  < 0) cput  = 0;
    
    const int sec = static_cast<int>(realt);
    
    std::stringstream str;
    str << "Real time " 
	<< setw(2) << setfill('0') << hours 
	<< ":" << setw(2) << setfill('0') << min
	<< ":" << setw(2) << setfill('0') << sec
	<< " CPU time " << setprecision(3) << fixed << cput;

    return str.str();
  }

  std::pair<std::string, std::string> SplitPath(const std::string &file) {

    std::pair<std::string, std::string> res("", file);

    const size_t ipos = file.find_last_of('/');

    if(ipos != string::npos) {
      res.first  = file.substr(0, ipos+1);
      res.second = file.substr(ipos+1, string::npos);
    }
    
    return res;
  }

  std::vector<Anp::NtupleSvc *> run_ntuples; 
  
  unsigned iFill = 0, mFill = 0;
  unsigned iRead = 0, mRead = 0;
}

//-----------------------------------------------------------------------------
Anp::RunModule::RunModule()
  :fFile(0),
   fTimer(new TStopwatch()),
   fTreeSize(0),
   fDebug(false),
   fDebugThread(false),
   fDoThreads(false),
   fNEvent(1000),
   fNPrint(100),
   fNQueue(100),
   fThread(0)
{
}

//-----------------------------------------------------------------------------
Anp::RunModule::~RunModule()
{
  delete fTimer;
  
  if(fThread) delete fThread;
}

//-----------------------------------------------------------------------------
void Anp::RunModule::Execute(const string &config_path)
{
  //
  // Main function call: read configuration and execute C++ code
  //
  cout << "Starting RunModule execution..." << endl;

  Registry reg;
  reg.ReadXML(config_path);

  //
  // First, configure self
  //
  RunModule::Config(reg);   

  if(fInputFiles.empty()) {
    log() << "Execute - missing input files.. bye!" << endl;
    return;
  }

#ifdef ANP_HEAP_PROFILER
  string heap_file = "heap-profile-runmodule-exec";
  fReg.Get("RunModule::HeapProfile", heap_file);

  cout << "RunModule::Exec - compiled with perftools:" << endl
       << "   HeapProfilerStart(" << heap_file << ")" << endl;

  HeapProfilerStart(heap_file.c_str());
#endif

  RunModule::Init();
  RunModule::Exec();
  RunModule::Done();

#ifdef ANP_HEAP_PROFILER
  cout << "RunModule::Exec - compiled with perftools: HeapProfilerStop." << endl;
  HeapProfilerDump("dump-heap-profile-1");
  HeapProfilerStop();
  HeapProfilerDump("dump-heap-profile-2");
#endif

  cout << "Finished RunModule execution." << endl;
}

//-----------------------------------------------------------------------------
void Anp::RunModule::Config(const Anp::Registry &reg)
{
  //
  // Global job connfiguration
  //
  fReg = reg;

  reg.Get("RunModule", "Debug",     fDebug);
  reg.Get("RunModule::DebugThread", fDebugThread);
  reg.Get("RunModule::DoThreads",   fDoThreads);
  reg.Get("RunModule::NEvent",      fNEvent);
  reg.Get("RunModule::NPrint",      fNPrint);
  reg.Get("RunModule::TreeName",    fTreeName);
  reg.Get("RunModule::TreePoll",    fTreePoll);
  reg.Get("RunModule::TreeThread",  fTreeThread);
  reg.Get("RunModule::TreeSize",    fTreeSize);

  //
  // Config event service
  //
  Anp::EventSvc::Instance().Config(reg);
  
  //
  // Load additional libraries
  //
  std::vector<std::string> libs;
  reg.Get("RunModule::LoadLibrary", libs);
  
  for(unsigned i = 0; i < libs.size(); ++i) {
    gSystem->Load(libs.at(i).c_str());
  }

  //
  // Create algorithm
  //
  string alg_name, alg_type;

  reg.Get("RunModule::AlgType", alg_type);
  reg.Get("RunModule::AlgName", alg_name);
  
  fAlg = Factory<AlgEvent>::Instance().Create(alg_type);
  if(fAlg.valid()) {
    fAlg->SetName(alg_name);
    fAlg->SetType(alg_type);

    cout << "RunModule::Config - created algorithm: \"" 
	 << alg_name << "\" - \"" << alg_type << "\"" << endl;
  }
  else {
    cerr << "RunModule::Config - failed to create algorithm: \"" << alg_type << "\"" << endl;
  }
  
  //
  // Configure HistMan
  //
  Registry hreg;
  reg.Get("RunModule::HistMan", hreg);

  HistMan::Instance().Config(hreg);

  cout << "RunModule::Config - processing histogram files..." << endl;

  while(hreg.KeyExists("ReadFile")) {
    //
    // Run find command on input directory
    //
    string rfile;
    hreg.Get("ReadFile", rfile);
    hreg.RemoveKey("ReadFile");

    if(Anp::file_size(rfile) > 1.0e-6) {
      HistMan::Instance().ReadFile(rfile);      
      cout << "  read: " << rfile << endl ;
    }
    else {
      cout << "  skip: " << rfile << endl;
    }
  }

  bool debug_hist = false;
  hreg.Get("Debug", debug_hist);
  if(debug_hist) HistMan::Instance().Print();

  //
  // Read input files 
  //
  Registry ireg;
  
  if(reg.Get("RunModule::InputFiles", ireg)) {
    if(fDebug) {
      log() << "Config - reading Registry with input files" << endl;
    }

    while(ireg.KeyExists("File")) {
      //
      // Add individual files
      //
      string data;
      ireg.Get("File", data);
      ireg.RemoveKey("File");
      RunModule::AddInputFile(data);
    }
  }
  else {
    log() << "Config - input files are not available at config step" << endl;
  }

  if(fDebug) {
    cout << "RunModule::Config" << endl
	 << "   NEvent  = " << fNEvent  << endl
	 << "   Debug   = " << fDebug   << endl;
    
    reg.Print();
    
  }
}

//-----------------------------------------------------------------------------
bool Anp::RunModule::Init()
{
  //
  // Initialiaze self and algorithms
  //
  string fileinit, treeinit;  
  fReg.Get("RunModule::FileInit", fileinit);  
  fReg.Get("RunModule::TreeInit", treeinit);
  
  if(fAlg.valid()) {
    if(fDebug) {
      cout << "RunModule::Init - configuring algorithm: " << fAlg->GetName() << endl;
    }
    
    Registry areg;
    if(fReg.Get(fAlg->GetName(), areg)) {
      fAlg -> Config(areg);
      log() << "Init - retrieved Registry for top algorithm: " << fAlg->GetName() << endl;
    }
    else {
      log() << "Init - using global Registry for top algorithm: " << fAlg->GetName() << endl;
      fAlg -> Config(fReg);
    }

    if(fDebug) {
      cout << "RunModule::Init - finished configuring algorithm: " << fAlg->GetName() << endl;
    }
  }

  //
  // Create output ROOT file
  //
  string outfname;

  if(fReg.Get("RunModule::OutputFile", outfname) && outfname.size() > 4) {
    fFile = new TFile(outfname.c_str(), "RECREATE");
    cout << "RunModule::Init - opened output ROOT file: " << outfname << endl;

    if(fAlg.valid()) {
      fAlg -> Save(fFile);
    }
  }
  else {
    if(fDebug) {
      cout << "RunModule::Init - output file is not set" << endl;
    }
  }

  //
  // Read first (initial) file
  //
  if(!fileinit.empty() && !treeinit.empty()) {
    Registry reg;
    reg.Set("SIGNAL::FilePath", fileinit);
    reg.Set("SIGNAL::FileInit", string("yes"));
    reg.Set("SIGNAL::TreeName", treeinit);
    reg.Set("SIGNAL::TreeSize", 0);

    //
    // Read tree for algorithm initialization
    //
    cout << "RunModule::Init - initial file:\n   " << fileinit << endl;

    long ievent = 0;
    ReadFile(reg, ievent);

    cout << "RunModule::Init - read " << ievent << " initial event(s)" << endl;
  }

  //
  // Initialize algorithms
  //
  if(fAlg.valid()) {
    if(fDebug) {
      cout << "RunModule::Init - initializing algorithm: " << fAlg->GetName() << endl;
    }

    fAlg -> Init();

    if(fDebug) {
      cout << "RunModule::Init - finished initializing algorithm: " << fAlg->GetName() << endl;
    }
  }
  
  //
  // Un-register initial tree
  //
  if(!treeinit.empty()) {
    Anp::NtupleSvc::Instance(treeinit).Reset();
  }

  //
  // Initialize thread
  //
  if(fDoThreads) {
    cout << "RunModule::Init - creating new thread..." << endl;
    fThread = new Thread(Anp::DoRunModuleWork, this);
    
    for(unsigned i = 0; i < fNQueue; ++i) {
      stringstream str;
      str << "thread_queue_" << setw(3) << setfill('0') << i;

      NtupleSvc &svc = NtupleSvc::Instance(str.str());
      run_ntuples.push_back(&svc);
    }

    for(unsigned i = 0; i < run_ntuples.size(); ++i) {
      run_ntuples.at(i)->Print();
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
void Anp::RunModule::AddInputFile(const std::string &file)
{
  //
  // Add ROOT files in local or network directory matching stored keys
  //
  if(file.empty()) {
    if(fDebug) {
      log() << "AddInputFile - ignore empty file" << endl;
    }
    return;
  }

  if(file.find(".root") == string::npos) {
    log() << "AddInputFile - ignore non-ROOT file: " << file << endl;
    return;
  }
  
  if(std::find(fInputFiles.begin(), fInputFiles.end(), file) == fInputFiles.end()) {
    if(fDebug) {
      log() << "AddInputFile - adding file: " << file << endl;
    }
    fInputFiles.push_back(file);
  }
  else {
    log() << "AddInputFile - skipping file already in the list: " << file << endl;
  }
}

//-----------------------------------------------------------------------------
void Anp::RunModule::ClearInputFiles()
{
  //
  // Clear input files
  //
  if(fDebug) {
    log() << "ClearInputFiles - clearing " << fInputFiles.size() << " input file(s)" << endl;
  }
  fInputFiles.clear();
}

//-----------------------------------------------------------------------------
const std::vector<std::string>& Anp::RunModule::GetInputFiles() const
{
  return fInputFiles;
}

//-----------------------------------------------------------------------------
void Anp::RunModule::Exec()
{
  //
  // Process input files 
  //

#ifdef ANP_CPU_PROFILER
  string profile_file = "cpu-profile-runmodule-exec";
  fReg.Get("RunModule::CpuProfile", profile_file);

  cout << "RunModule::Exec - compiled with perftools:" << endl
       << "   ProfilerStart(" << profile_file << ")" << endl;

  ProfilerStart(profile_file.c_str());
#endif

  fTimer -> Start();

  if(fAlg.valid()) {
    fAlg -> Signal(fReg, "BEG_EXEC");
  }

  //
  // Iterate over input files and count events
  //
  if(!fTreePoll.empty()) {
    long total_count = 0;
    
    for(unsigned i = 0; i < fInputFiles.size(); ++i) {
      //
      // Counte number of entries in tree for a file
      //
      Registry reg;
      reg.Set("SIGNAL::FilePath", fInputFiles.at(i));
      reg.Set("SIGNAL::FileInit", "no");
      reg.Set("SIGNAL::TreeName", fTreePoll);
      reg.Set("SIGNAL::TreeSize", fTreeSize);
      
      PollFile(reg);
      
      long tree_count = 0;
      reg.Get(fTreePoll, tree_count);
      
      total_count += tree_count;
    }
    
    EventSvc::Instance().Init();

    cout << "RunModule::Exec - tree " << fTreePoll << ": " << fInputFiles.size()
	 << " input file(s) contain " << total_count << " event(s)" << endl;
  }

  //
  // Iterate over input files and read events
  //
  for(unsigned i = 0; i < fInputFiles.size(); ++i) {
    //
    // Check event count
    //
    const string &file = fInputFiles.at(i);
    const string &path = Anp::SplitPath(file).first;

    if(fDebug) {
      cout << "RunModule::Exec - path=" << path << endl;
    }

    const unsigned dir_index = EventSvc::Instance().GetDirectoryIndex(path);
    long &icount = fCountMap.insert(map<unsigned, long>::value_type(dir_index, 0)).first->second;

    if(StopNow(icount)) break;

    Registry reg;
    reg.Set("SIGNAL::FilePath", file);
    reg.Set("SIGNAL::FileInit", "no");
    reg.Set("SIGNAL::TreeName", fTreeName);
    reg.Set("SIGNAL::TreeSize", fTreeSize);

    ReadFile(reg, icount);
  }

  //
  // Inform other thread that work is completed
  //  
  if(fThread) {	  
    Anp::Lock<Anp::Mutex> lock(fThread->GetMutex());
    
    while(fThread->GetCond().GetValue() == 1) {
      fThread->GetCond().Wait(fThread->GetMutex());
    }

    fThread->GetCond().SetValue(-1);
    fThread->GetCond().Signal();
  }

#ifdef ANP_CPU_PROFILER
  cout << "RunModule::Exec - compiled with perftools: ProfilerStop." << endl;
  ProfilerStop();
#endif
}

//-----------------------------------------------------------------------------
void Anp::RunModule::Done()
{
  //
  // End of processing...
  //

#ifdef ANP_CPU_PROFILER
  string profile_file = "cpu-profile-runmodule-done";
  fReg.Get("RunModule::CpuProfileDone", profile_file);

  cout << "RunModule::Done - compiled with perftools:" << endl
       << "   ProfilerStart(" << profile_file << ")" << endl;

  ProfilerStart(profile_file.c_str());
#endif

  if(fAlg.valid()) {
    fAlg -> Done();
  }

  //
  // Erase algorithms (delete them if nobody else holds handle)
  //
  cout << "RunModule::Done - clearing algorithm handle" << endl;
  if(fAlg.valid()) {
    fAlg.release();
  }

  //
  // Erase factory hold list
  //
  std::vector<Anp::FactoryBase *> &flist = Anp::GetFactoryList();

  cout << "RunModule::Done - clearing " << flist.size() << " factory object(s)" << endl;

  for(unsigned int i = 0; i < flist.size(); ++i) {
    Anp::FactoryBase *f = flist[i];
    f->Clear();
  }
  
  //
  // Print out job summary
  //
  cout << "RunModule - processed " << fCountMap.size() << " input dataset(s)" << endl;

  for(map<unsigned, long>::const_iterator it = fCountMap.begin(); it != fCountMap.end(); ++it) {
    cout << "   dataset #" << it->first << ": read " << it->second << " event(s)" << endl;
  }

  // Close output file (if exists)
  if(fFile) {
    cout << "RunModule::Done - closing ROOT file: " << fFile->GetName() << endl;
    fFile->Write();
    fFile->Close();
    fFile = 0;
  }

#ifdef ANP_CPU_PROFILER
  cout << "RunModule::Done - compiled with perftools: ProfilerStop." << endl;
  ProfilerStop();
#endif

  cout << "RunModule::Done - finished." << endl;
}

//-----------------------------------------------------------------------------
Anp::AlgEvent* Anp::RunModule::GetAlg()
{
  if(fAlg.valid()) return fAlg.get();
  return 0;
}

//-----------------------------------------------------------------------------
void Anp::RunModule::ReadFile(Registry &reg, long &icount)
{

  //
  // Open TFile, get TTree, prepare algorithm and read events
  //
  string fpath;
  int msize = 0;
  bool finit = false;
  vector<string> trees;

  reg.Get("SIGNAL::FilePath", fpath);
  reg.Get("SIGNAL::FileInit", finit);  
  reg.Get("SIGNAL::TreeName", trees);
  reg.Get("SIGNAL::TreeSize", msize);

  // Ignore small files
  if((fpath.find("castor") == string::npos && fpath.find("eos") == string::npos) && Anp::file_size(fpath) < 1.0e-5) {
    if(fDebug) {
      cerr << "RunModule::ReadFile - ignore small file:\n   " << fpath << endl;
    }
    return;
  }

  if(trees.empty()) {
    if(fDebug) {
      cerr << "RunModule::ReadFile - no trees are requested" << endl;
    }
    return;
  }


  TFile *file = TFile::Open(fpath.c_str(), "READ");
  if(!file || !file->IsOpen()) {
    cerr << "RunModule::ReadFile - failed to open:\n" << "   " << fpath << endl;
    if(file) delete file;
    return;
  }

  //
  // Set path to current file - used to find matching dataset 
  // and corresponding weight. Also find directory index.
  //
  EventSvc::Instance().SetDataset(fpath);

  if(fAlg.valid()) {
    if(finit) fAlg -> Signal(reg, "INIT_FILE");
    else      fAlg -> Signal(reg, "NEW_FILE");
  }

  for(unsigned itree = 0; itree < trees.size(); ++itree) {
    const string &tname = trees.at(itree);
    
    // Get tree from file
    TTree *tree = dynamic_cast<TTree *> (file->Get(tname.c_str()));
    if(!tree) {
      if(fDebug) {
	std::cerr << "RunModule::ReadFile - TTree is not found: " << tname << std::endl;
      }
      continue;
    }

    // Check tree size
    const long tsize = tree->GetEntries();
    if(!finit && msize > 0 && msize != tsize) {
      if(fDebug) {
	cout << "RunModule::ReadFile - skipping file : " << fpath << endl;
      }
      continue;
    }   

    //
    // Check event count
    //
    if(StopNow(icount)) break;
    
    // Register Tree
    NtupleSvc &svc_run = NtupleSvc::Instance(tname+"_run");
    NtupleSvc &svc_alg = NtupleSvc::Instance(tname);
    
    // Copy branch registrations
    svc_alg.CopyTo(svc_run);

    svc_run.Reset();
    svc_run.Set(tree, Anp::NtupleSvc::READ);
    
    run_ntuples.push_back(&svc_run);

    const bool use_thread = fThread && fTreeThread == tname;
    
    if(fDebug) {
      cout << "RunModule::ReadFile - use thread=" << use_thread << ", reading " << tsize 
	   << " events from tree=\"" << tname << "\" and file=\"" << fpath << "\"" << endl;
    }

    for(long i = 0; i < tsize; ++i) {      
      if(fDebug) {
	cout << "RunModule::ReadFile - " << tname << " entry #" << i << endl;
      }

      //
      // Count events: global counter and dataset counts
      //  
      if(StopNow(icount)) break;

      ++icount;

      if(finit) {
	//
	// Special case: read configuration from initialization file
	//
	tree -> GetEntry(i);
	svc_run.Set(Anp::NtupleSvc::NEW_ENTRY);
	svc_run.CopyTo(svc_alg);
	if(fAlg.valid()) fAlg->Exec();
	continue;
      }

      if(icount % fNPrint == 0) {
	cout << "Record #" << setw(8) << setfill(' ') << icount << " "
	     << Anp::PrintStopWatch(*fTimer) << endl;
      }
      
      //
      // Read data
      //
      tree -> GetEntry(i);
      svc_run.Set(Anp::NtupleSvc::NEW_ENTRY);

      //
      // Execute algorithm
      //      
      if(use_thread) {
	Anp::Lock<Anp::Mutex> lock(fThread->GetMutex());
	
	while(fThread->GetCond().GetValue() == 1) {
	  fThread->GetCond().Wait(fThread->GetMutex());
	}
      
	if(fThread->GetCond().GetValue() != 0) {
	  cerr << "RunModule::ReadFile - bad cond var=" << fThread->GetCond().GetValue() << endl;
	  break;
	}
	
	//
	// Copy from working tree to algorithm tree
	//
	svc_run.CopyTo(svc_alg);
	
	//
	// Inform other thread that we have new data
	//
	fThread->GetCond().SetValue(1);
	fThread->GetCond().Signal();
      }
      else {
	svc_run.CopyTo(svc_alg);
	if(fAlg.valid()) fAlg->Exec();
      }
    }
    
    if(use_thread) {
      Anp::Lock<Anp::Mutex> lock(fThread->GetMutex());
      
      while(fThread->GetCond().GetValue() == 1) {
	fThread->GetCond().Wait(fThread->GetMutex());
      }
      
      svc_run.Reset();
      svc_alg.Reset();
    }
    else {
      if(!finit) {
	svc_run.Reset();
	svc_alg.Reset();
      }
    }
  }

  if(fDebug) {
    std::cout << "RunModule::ReadFile - Closing file: " << fpath.c_str() << std::endl;
  }  

  delete file;
}

//-----------------------------------------------------------------------------
void Anp::RunModule::PollFile(Registry &reg)
{
  //
  // Open TFile, get TTree, count events
  //
  string file_path, tree_name;
  reg.Get("SIGNAL::FilePath", file_path);
  reg.Get("SIGNAL::TreeName", tree_name);

  // Ignore small files
  if((file_path.find("castor") == string::npos && file_path.find("eos") == string::npos) && Anp::file_size(file_path) < 1.0e-5) {
    if(fDebug) {
      cerr << "RunModule::PollFile - ignore small file:\n   " << file_path << endl;
    }
    return;
  }

  // No trees requested
  if(tree_name.empty()) {
    if(fDebug) {
      cerr << "RunModule::PollFile - no trees are requested" << endl;
    }
    return;
  }

  // Open file
  TFile *file = TFile::Open(file_path.c_str(), "READ");
  if(!file || !file->IsOpen()) {
    cerr << "RunModule::PollFile - failed to open:\n" << "   " << file_path << endl;
    if(file) delete file;
    return;
  }

  long nevent = 0;
  
  // Get tree from file
  TTree *tree = dynamic_cast<TTree *> (file->Get(tree_name.c_str()));
  if(tree) {
    nevent = tree->GetEntries();
  }
  else {
    if(fDebug) {
      std::cerr << "RunModule::PollFile - TTree is not found: " << tree_name << std::endl;
    }
  }

  if(fDebug) {
    cout << "RunModule::PollFile - counted " << nevent << " events from file/tree: " 
	 << file_path << "/" << tree_name << endl;
  }
  
  //
  // Clear registry and set event counts for trees
  //
  reg.Clear();
  reg.Set(tree_name, nevent);
  EventSvc::Instance().AddDataset(file_path, nevent);
  
  delete file;
}

//---------------------------------------------------------------------------------------
std::ostream& Anp::RunModule::log() const
{    
  std::cout << "RunModule::";
  return cout;
}

//---------------------------------------------------------------------------------------
void* Anp::DoRunModuleWork(void *alg_)
{
  //
  // Global function for running in a new thread
  //
  if(!alg_) {
    cerr << "DoRunModuleWork - null argument pointer" << endl;
    return 0;
  }

  //
  // Get algorithm pointer
  //
  RunModule *alg = static_cast<RunModule *> (alg_);
  if(!(alg -> fThread)) {
    cerr << "DoRunModuleWork - RunModule has no valid thread" << endl;
    return 0;
  }

  Thread *thr = alg->fThread;
  if(thr -> GetStatus() != 0) {
    cerr << "DoRunModuleWork - thread status != 0" << endl;
    return 0;
  }
  
  if(!(alg->fAlg.valid())) {
    cerr << "DoRunModuleWork - invalid algorithm pointer" << endl;
    return 0;
  }

  cout << "DoRunModuleWork - enter function..." << endl;  
   
  const bool debug = alg->fDebugThread;

  AlgEvent *exec = alg->fAlg.get();
  CondVar  &cond = thr->GetCond();
  Mutex    &mutx = thr->GetMutex();
  
  while(true) {
    Anp::Lock<Anp::Mutex> lock(mutx);

    //
    // Wait on another thread
    //
    while(cond.GetValue() == 0) {
      cond.Wait(mutx);
    }

    //
    // All work is done, exit function
    //
    if(cond.GetValue() != 0 &&
       cond.GetValue() != 1) {
      if(debug) {
	cout << "DoRunModuleWork - cond var=" << cond.GetValue() << " - thread is done." << endl;
      }
      break;
    }    
    
    //
    // Do actual work and reset status to idle
    //
    if(cond.GetValue() == 1) {
      exec->Exec();
      cond.SetValue(0);
      cond.Signal();
    }
  }

  cout << "DoRunModuleWork - exit function." << endl;  
  return alg;
}

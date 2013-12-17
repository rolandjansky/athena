/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_RUNMODULE_H
#define ANP_RUNMODULE_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : RunModule
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 *  
 *  Top level event loop manager.
 * 
 *  Module for reads ntuple files and creates AlgEvent instances.
 *  Created algorithms go through these steps:
 *
 *  - Config() - create and configure C++ algorithms
 *  - Init()   - initialize algorithms and read configuration (Registry from python or XML)
 *  - Exec()   - event loop
 *  - Done()   - finalize algorithms
 *
 *  This function configures and executes job using input XML file:
 *
 *  - Execute() - configure input files execute above functions 
 *                using registry read from input path to XML file
 *
 **********************************************************************************/

// C/C++
#include <map>
#include <set>
#include <vector>

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/Registry.h"

class TFile;
class TTree;
class TStopwatch;

namespace Anp
{
  void* DoRunModuleWork(void *run_);

  class Thread;

  class RunModule {
  public:
    
    RunModule();
    virtual ~RunModule();
    
    void Execute(const std::string &config_path);

    void Config(const Anp::Registry &reg);
    
    bool Init();
    void Exec();
    void Done();
    
    AlgEvent* GetAlg();

    void AddInputFile(const std::string &file);
    
    void ClearInputFiles();

    const std::vector<std::string>& GetInputFiles() const;

  private:

    void ReadFile(Registry &reg, long &icount);
    void PollFile(Registry &reg);
    
    bool StopNow(long count) { return fNEvent > 0 && count+1 > fNEvent; }

    std::ostream& log() const;
    
  private:

    friend void* DoRunModuleWork(void *);

  private:

    TFile                     *fFile;         // Output ROOT file pointer
    TStopwatch                *fTimer;        // Event processing timer
    Anp::Registry              fReg;          // Global Registry configuration

    int                        fTreeSize;     // Required number of events in each TTree    
    std::string                fTreeName;     // Name of input ROOT tree(s)
    std::string                fTreePoll;     // Main tree name for file count polls
    std::string                fTreeThread;   // Name of tree for reading in seperate thread

    Handle<AlgEvent>           fAlg;          // Algorithm to be executed

    // Properties:
    bool                       fDebug;        // Print debug info
    bool                       fDebugThread;  // Print thread debug info
    bool                       fDoThreads;    // Run threads
    
    long                       fNEvent;       // Number of events to read
    long                       fNPrint;       // Number of events to print    
    unsigned                   fNQueue;       // Size of thread data list

    // Variables:
    std::map<unsigned, long>   fCountMap;     // Map for counting number of processed events
    std::vector<std::string>   fInputFiles;   // Input files
    Thread                    *fThread;       // Thread object - needs more work
  };
}

#endif

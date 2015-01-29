/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGNTSAVETTOOL_H
#define TRIG_TRIGNTSAVETTOOL_H

//
// Save TrigMonEvent and TrigMonConfig in TTrees
//
// Tool have two options for TTree output:
//  - Create new TFile and create TTrees
//  - Use HistSvc to get TTrees
//

// Framework
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostMonitor/ITrigNtTool.h"

class TTree;
class TFile;
class ITHistSvc;
class IIncidentSvc;

namespace Trig
{
  static const InterfaceID IID_TrigNtSaveTool("Trig::TrigNtSaveTool", 1, 0);
  
  class TrigNtSaveTool 
    : public virtual Trig::ITrigNtTool, 
      public virtual AthAlgTool,
      public virtual IIncidentListener {
  public:
    
    static const InterfaceID& interfaceID() { return IID_TrigNtSaveTool; }    

    TrigNtSaveTool(const std::string &type,
       const std::string &name,
       const IInterface* parent);
    
    virtual ~TrigNtSaveTool();
    
    StatusCode initialize();
    StatusCode finalize();

    bool Fill(TrigMonConfig *config);
    bool Fill(TrigMonEvent &event);

    void handle(const Incident& inc);

  private:
    
    bool Open(const std::string &file);

    void Close();

    TTree* MakeTree(const std::string &tname, const std::string &sname);    

  private:

    // Tool properties
    bool                     m_writeFile;        // Write ROOT file
    bool                     m_writeCostOnly;    // Save only events with cost data
    bool                     m_writeRateOnly;    // Save only trigger rates
    bool                     m_printPostSummary; // Print POST_COST summary?

    std::string              m_streamConfig;     // TrigMonConfig stream
    std::string              m_streamEvent;      // TrigMonEvent  stream
    std::string              m_fileName;         // ROOT file name
    std::string              m_filePath;         // ROOT file path

    std::string              m_treeNameEvent;    // TTree name for TrigMonEvent
    std::string              m_treeNameConfig;   // TTree name for TrigMonConfig
    
    int                      m_branchBuffer;     // TBranch buffer size
    int                      m_branchSplit;      // TBranch split level
    int                      m_fileCompr;        // ROOT file compression

    // Athena tool and service handles
    ServiceHandle<ITHistSvc>    m_histSvc;
    ServiceHandle<IIncidentSvc> m_incSvc;

    // Tool variables
    TFile                   *m_file;
    TTree                   *m_treeConfig;
    TTree                   *m_treeEvent;

    TrigMonConfig           *m_config;
    TrigMonEvent            *m_event;

    unsigned                 m_countConfig;
    unsigned                 m_countEvent;

    unsigned                 m_filenConfig;
    unsigned                 m_filenEvent;   
  };
}

#endif

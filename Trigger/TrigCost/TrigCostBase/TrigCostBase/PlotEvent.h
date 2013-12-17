/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_PLOTEVENT_H
#define ANP_PLOTEVENT_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : PlotEvent
 * @Author : Rustem Ospanov
 *
 * @Brief  : Plot event data: int, float pairs
 * 
 **********************************************************************************/

// C/C++
#include <map>
#include <string>
#include <sstream>

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostBase/NtupleSvc.h"
#include "TrigCostBase/AlgEvent.h"

class TDirectory;
class TH1;
class TH2;

namespace Anp
{
  class PlotEvent: public virtual AlgEvent {
  public:
    
    PlotEvent();
    virtual ~PlotEvent();
    
    void Config(const Registry &reg);
    
    void Save(TDirectory *dir);
    
    bool Init() { return true; }

    void Exec();

    void Done() {}

    void Run(const TrigMonEvent &event);

    void Plot(const unsigned int key, const float var);
    void Plot(const std::string &key, const float var, const std::string &hname = "");

    std::map<uint32_t, TH1 *>&    GetKeyMap() { return fKeyHist; }
    std::map<std::string, TH1 *>& GetVarMap() { return fVarHist; }

    TH1* GetKeyHist(uint32_t key);
    TH1* GetVarHist(const std::string &key);

  private:

    typedef std::map<uint32_t, TH1 *> KeyMap;
    typedef std::map<std::string, TH1 *> VarMap;

  private:

    TH1 *FindTH1(const unsigned int hid);
    TH1 *FindTH1(const std::string &key, const std::string &hname = "");
    
  private:
      
    TDirectory            *fDir;        // output directory

    Branch<TrigMonEvent>   fEntry;      // Trigger data

    std::string            fDirName;    // output directory name
    std::string            fDirHist;    // xml directory for histogram definitions
    bool                   fDebug;      // print extra information

    KeyMap                 fKeyHist;    // histogram map
    VarMap                 fVarHist;    // histogram map
  };
}

#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArCollisionTimeMonTool
 * @author Vikas Bansal <Vikas@uvic.ca>
 *
 */

#ifndef LARMONTOOLS_LARCOLLISIONTIMEMONTOOL_H
#define LARMONTOOLS_LARCOLLISIONTIMEMONTOOL_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "LArRecEvent/LArCollisionTime.h"

#include <map>
#include <string>
#include <bitset>
#include <vector>
#include "TMath.h"
#include "TTree.h"

class ITHistSvc;

class TTree;
class TH1F_LW;
class TH2F_LW;


namespace Trig {
class IBunchCrossingTool;
}

class LArCollisionTimeMonTool: public ManagedMonitorToolBase
{
 public:
  LArCollisionTimeMonTool(const std::string& type, 
                      const std::string& name,
		      const IInterface* parent);

  /** @brief Default destructor */
  virtual ~LArCollisionTimeMonTool();

  /** @brief Overwrite dummy method from AlgTool */
  StatusCode initialize();

  /** Book general histograms
   *  Implement pure virtual methods of IMonitorToolBase */
  StatusCode bookHistograms();

  /** Called each event */
  StatusCode fillHistograms();

  /** Regularly called to fill noise maps
   *  Overwrite dummy method from MonitorToolBase */
  StatusCode procHistograms();

  // hack to use this function to update the bcid numbers cache
  StatusCode updateBCID(IOVSVC_CALLBACK_ARGS);

 protected:

  // services
  //const EventInfo* m_evtInfo;

  //ITHistSvc* m_rootStore;
  
 private:

  // Properties
  unsigned int             m_lumi_blocks;

  float m_ECTimeDiff;
  float m_ECTimeAvg;

  unsigned m_nhist;
  TH1F_LW** m_LArCollTime_h; 
  TH1F_LW** m_LArCollTime_lb_h;
  TH1F_LW** m_LArCollTime_lb_timeCut_h;
  TH1F_LW** m_LArCollTime_lb_singlebeam_timeCut_h;
  TH2F_LW** m_LArCollTime_vs_LB_h; 
  TH2F_LW** m_LArCollTime_vs_BCID_h;
  TH1F_LW** m_LArCollAvgTime_h; 
  TH2F_LW** m_LArCollAvgTime_vs_LB_h; 
  TH2F_LW** m_LArCollAvgTime_vs_BCID_h;             

  int m_distance; // distance from train front to fill second histos
  bool m_IsOnline;

  // Counters
  int m_eventsCounter;

  float m_timeCut;
  int m_minCells;
  bool m_eWeighted;
  bool m_newrun;

  //ToolHandle<IBunchGroupTool> m_bunchGroupTool;
  ToolHandle<Trig::IBunchCrossingTool> m_bunchGroupTool;
  std::vector<unsigned int> m_bcid_allowed;
  bool m_bcid_init;


  std::string m_histPath;
  SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoKey{this, "EventInfoKey", "EventInfo"};
  SG::ReadHandleKey<LArCollisionTime> m_key{this, "Key", "LArCollisionTime"};

  void cleanup();
};

#endif

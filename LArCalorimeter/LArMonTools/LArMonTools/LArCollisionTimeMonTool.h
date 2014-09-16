/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArCollisionTimeMonTool
 * @author Vikas Bansal <Vikas@uvic.ca>
 *
 */

#ifndef LAR_COLLISION_TIME_MON_TOOL_H
#define LAR_COLLISION_TIME_MON_TOOL_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include <map>
#include <string>
#include <bitset>
#include <vector>
#include "TH1.h"
#include "TH2I.h"
#include "TH2F.h"
#include "TMath.h"
#include "TProfile2D.h"
#include "TTree.h"

class StoreGateSvc;
class ITHistSvc;

class TH1I;
class TH2I;
class TH2F;
class TProfile2D;
class TTree;

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

 protected:

  // services
  //const EventInfo* m_evtInfo;

  //StoreGateSvc* m_eventStore;
  //  StoreGateSvc* m_detectorStore;
  //ITHistSvc* m_rootStore;
  
 private:

  // Properties
  unsigned int             m_lumi_blocks;

  float m_ECTimeDiff;
  float m_ECTimeAvg;


  TH1F* m_LArCollTime_h; 
  TH1F* m_LArCollTime_lb_h;
  TH1F* m_LArCollTime_lb_timeCut_h;
  TH1F* m_LArCollTime_lb_singlebeam_timeCut_h;
  TH2F* m_LArCollTime_vs_LB_h; 
  TH2F* m_LArCollTime_vs_BCID_h;
  TH1F* m_LArCollAvgTime_h; 
  TH2F* m_LArCollAvgTime_vs_LB_h; 

  // Counters
  int m_eventsCounter;

  float m_timeCut;
  int m_minCells;
  bool m_eWeighted;
  bool newrun;

};

#endif

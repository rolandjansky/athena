/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArAffectedRegions
 * @author Jessica Leveque <jleveque@in2p3.fr>
 *
 */

#ifndef LARAFFECTEDREGIONS_H
#define LARAFFECTEDREGIONS_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "StoreGate/StoreGateSvc.h"

#include <string>
#include <map>

class ITHistSvc;

#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"
class TProfile_LW;

class LArAffectedRegions: public ManagedMonitorToolBase
{
 public:
  LArAffectedRegions(const std::string& type, 
                      const std::string& name,
		      const IInterface* parent);

  /** @brief Default destructor */
  virtual ~LArAffectedRegions();

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

  /** Style function */
  void SetHistoStyle(TH2I_LW* h);

 protected:

  // services
  ITHistSvc* m_rootStore;

 private:

  // Properties
  bool m_isonline;
  float m_delta_phi;
  float m_delta_eta;

  // Other things
  int m_eventsCounter;

  // Coverage Maps
  TH2I_LW* m_hLArAffectedRegionsEMBPS[2];
  TH2I_LW* m_hLArAffectedRegionsEMB[2];
  TH2I_LW* m_hLArAffectedRegionsEMECPS[2];
  TH2I_LW* m_hLArAffectedRegionsEMEC[2];
  TH2I_LW* m_hLArAffectedRegionsHECA[4];
  TH2I_LW* m_hLArAffectedRegionsHECC[4];
  TH2I_LW* m_hLArAffectedRegionsFCALA[3];
  TH2I_LW* m_hLArAffectedRegionsFCALC[3];

};

#endif

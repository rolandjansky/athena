/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLS_L1CALOLARTOWERENERGY_H_
#define _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLS_L1CALOLARTOWERENERGY_H_

// Athena includes
#include "AsgTools/AsgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "Identifier/Identifier.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h"
#include "TrigT1CaloCalibTools/L1CaloCells2TriggerTowers.h"
#include "TrigT1CaloCondSvc/L1CaloCondSvc.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArRecConditions/ILArBadChanTool.h"
#include "LArCabling/LArCablingLegacyService.h"
#include "LArIdentifier/LArOnlineID.h"

// PABC interface include
#include "TrigT1CaloCalibToolInterfaces/IL1CaloLArTowerEnergy.h"

// STL inlcudes
#include <vector>
#include <map>
#include <string>
#include <iostream>

/**
  * AlgTool to retreive the trigger tower energy in the LArg readout
  * Author Antoine Marzin <antoine.marzin@cern.ch>
  * Author John Morris <john.morris@cern.ch>
  */



namespace LVL1 {

class L1CaloLArTowerEnergy : virtual public IL1CaloLArTowerEnergy, public asg::AsgTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( L1CaloLArTowerEnergy , LVL1::IL1CaloLArTowerEnergy )

public:
  // constructor
  L1CaloLArTowerEnergy( const std::string& name );
  virtual ~L1CaloLArTowerEnergy() {};

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  bool initL1CaloLArTowerEnergy(const CaloCellContainer& cellContainer, const TriggerTowerCollection &triggerTowerCollection);
  float EtLArg(const Identifier& TTid) const;
  bool hasMissingFEB(const Identifier& TTid) const;

protected:
  void reset();
  double IDeta(const Identifier& TTid) const;

private:

  // properties
  std::string m_triggerTowerCollectionName; // name of transient TriggerTower container
  std::string m_caloCellContainerName; // name of transient CaloCell container.
  std::string m_l1CaloCells2TriggerTowersToolName;

  L1CaloCondSvc *m_condSvc;
  const CaloLVL1_ID *m_lvl1Helper;
  const CaloIdManager *m_caloMgr;
  const LArOnlineID *m_LArOnlineHelper;

  CaloTriggerTowerService* m_ttService;
  ToolHandle<LVL1::IL1CaloCells2TriggerTowers> m_cells2tt;

  ToolHandle< ILArBadChanTool > m_badChannelTool; // Handle to badChannelTool
  ToolHandle<LArCablingLegacyService> m_larCablingSvc;  // Handle to LarCablingService
  ToolHandle<LVL1::IL1TriggerTowerTool> m_ttTool; // Handle to L1TriggerTowerTool



  typedef std::map<Identifier, double> mapTT;
  typedef std::map<double, std::pair<double, int> > mapSum;

  //definition of the maps containing the EtCells of each TT
  mapTT m_map_Etcells_em;
  mapTT m_map_Etcells_had;


  //definition of the maps containing the average EtCells in phi
  mapSum m_map_sumEtcells_phi_em;
  mapSum m_map_sumEtcells_phi_had;

};
} // end of namespace

#endif //L1CALOLARTOWERENERGY_H


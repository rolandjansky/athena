/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBTOOLS_L1CALOLARTOWERENERGY_H
#define TRIGT1CALOCALIBTOOLS_L1CALOLARTOWERENERGY_H

// Athena includes
#include "AsgTools/AsgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "Identifier/Identifier.h"
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h"
#include "TrigT1CaloCalibTools/L1CaloCells2TriggerTowers.h"
#include "TrigT1CaloCondSvc/L1CaloCondSvc.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArRecConditions/ILArBadFebMasker.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArIdentifier/LArOnlineID.h"

// PABC interface include
#include "TrigT1CaloCalibToolInterfaces/IL1CaloLArTowerEnergy.h"

// STL includes
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

class
L1CaloLArTowerEnergy : virtual public IL1CaloLArTowerEnergy, public asg::AsgTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( L1CaloLArTowerEnergy , LVL1::IL1CaloLArTowerEnergy )

public:
  // constructor
  L1CaloLArTowerEnergy( const std::string& name );
  virtual ~L1CaloLArTowerEnergy() {};

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual bool hasMissingFEB(const Identifier& TTid) const override;
  virtual bool hasMissingFEB(const LArOnOffIdMapping& cabling,
                             const Identifier& TTid) const override;

protected:
  virtual double IDeta(const Identifier& TTid) const override;

private:
  const CaloLVL1_ID *m_lvl1Helper;
  const CaloIdManager *m_caloMgr;
  const LArOnlineID *m_LArOnlineHelper;

  CaloTriggerTowerService* m_ttService;

  ToolHandle< ILArBadFebMasker > m_badFebMasker; // Handle to badChannelTool

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey
    {this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
};
} // end of namespace

#endif //L1CALOLARTOWERENERGY_H


/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCABLINGTEST_H
#define LARCABLINGTEST_H

//Dear emacs, this is -*-c++-*-
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "Identifier/HWIdentifier.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArCalibLineMapping.h"
#include "LArRecConditions/LArFebRodMapping.h"


class LArOnlineID;
class CaloCell_ID;

class LArCablingTest : public AthAlgorithm
{
 public:
  LArCablingTest(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArCablingTest();

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute() ;
  StatusCode finalize(){return StatusCode::SUCCESS;}
 private:

    SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
    SG::ReadCondHandleKey<LArCalibLineMapping>  m_CLKey{this, "CalibLineKey", "LArCalibLineMap", "SG calib line key"};
    SG::ReadCondHandleKey<LArFebRodMapping>  m_RodKey{this, "FebRodKey", "LArFebRodMap", "SG ROD mapping key"};

  bool m_print;
  unsigned m_mode;
  const LArOnlineID* m_onlineId;
  const CaloCell_ID* m_caloCellId;

  void print (const HWIdentifier& hwid, std::ostream& out, const LArOnOffIdMapping* cabling, const LArCalibLineMapping *clCont); 
};

#endif

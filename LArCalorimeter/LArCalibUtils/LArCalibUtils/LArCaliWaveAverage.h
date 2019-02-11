//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARCALIWAVEAVERAGE_H
#define LARCALIWAVEAVERAGE_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ICondSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "Identifier/HWIdentifier.h"
#include "LArRawConditions/LArCaliWave.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRawConditions/LArCaliWaveContainer.h"

#include <string>

class LArCaliWaveAverage:public AthAlgorithm {

public:

  LArCaliWaveAverage (const std::string& name, ISvcLocator* pSvcLocator);
  ~LArCaliWaveAverage();
  
  StatusCode initialize() override final;
  StatusCode execute() override final;
  StatusCode stop() override final;
  StatusCode finalize()override final {return StatusCode::SUCCESS;}

private:

   SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this, "OnOffMap", "LArOnOffIdMap", "SG key for mapping object"};
   SG::ReadCondHandleKey<LArCaliWaveContainer> m_keyInput{this, "KeyInput", "LArCaliWave", "SG key of input cali wave container"};
   SG::WriteCondHandleKey<LArCaliWaveContainer>  m_keyOutputCorr{this, "KeyOutputCorr", "LArCaliWaveCorr", "SG key for corrected container"};
   SG::WriteCondHandleKey<LArCaliWaveContainer> m_keyOutputSymm{this, "KeyOutputSymm", "LArCaliWaveSymm", "SG key for symmetrized container"};

  const LArOnlineID* m_onlineHelper;
  
  const LArEM_ID*    m_emId;
  const LArHEC_ID*   m_hecId;
  const LArFCAL_ID*  m_fcalId;

  std::string m_groupingType;
  
  std::vector<unsigned> m_chids;

  ServiceHandle<ICondSvc> m_condSvc;

  std::vector<HWIdentifier> SymmetricChannels(HWIdentifier ChID,std::vector<unsigned> ChannelsNotToUse, const LArOnOffIdMapping* cabling );
  LArCaliWave WaveAverage(std::vector<LArCaliWave> ToBeAveraged);

};

#endif

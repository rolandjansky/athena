//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARCALIWAVEAVERAGE_H
#define LARCALIWAVEAVERAGE_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/StoreGateSvc.h"
#include "LArTools/LArCablingService.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "Identifier/HWIdentifier.h"
#include "LArRawConditions/LArCaliWave.h"

#include <string>

class LArCaliWaveAverage:public AthAlgorithm {

public:

  LArCaliWaveAverage (const std::string& name, ISvcLocator* pSvcLocator);
  ~LArCaliWaveAverage();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

private:

  LArCablingService* m_larCablingSvc;
  const LArOnlineID* m_onlineHelper;
  
  const LArEM_ID*    m_emId;
  const LArHEC_ID*   m_hecId;
  const LArFCAL_ID*  m_fcalId;

  std::string m_keyInput;
  std::string m_keyOutputCorr;
  std::string m_keyOutputSymm;
  std::string m_groupingType;
  
  //std::vector<HWIdentifier> m_chids;
  std::vector<unsigned> m_chids;

  std::vector<HWIdentifier> SymmetricChannels(HWIdentifier ChID,std::vector<unsigned> ChannelsNotToUse );
  LArCaliWave WaveAverage(std::vector<LArCaliWave> ToBeAveraged);

};

#endif

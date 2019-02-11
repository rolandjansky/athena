//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARMASTERWAVEBUILDER_H
#define LARMASTERWAVEBUILDER_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "LArRawConditions/LArWave.h"
#include "LArRawConditions/LArWaveHelper.h"
#include "LArRawConditions/LArCaliWave.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArCalibLineMapping.h"

#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/LArEM_ID.h"

#include <vector>
#include <string>
#include <map>

class LArMasterWaveBuilder : public AthAlgorithm {
public:
  LArMasterWaveBuilder(const std::string & name, ISvcLocator * pSvcLocator);

  ~LArMasterWaveBuilder();

  //standard algorithm methods
  StatusCode initialize() ;
  StatusCode execute() {return StatusCode::SUCCESS;} //empty method
  StatusCode stop() ;
  StatusCode finalize() {return StatusCode::SUCCESS;}

  StatusCode perMWCal(const std::vector<LArCaliWave*>, LArWave &mwfWave, LArWave &intrWave);

private:

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<LArCalibLineMapping>  m_CLKey{this, "CalibLineKey", "LArCalibLineMap", "SG calib line key"};

  double diffWave(const LArCaliWave& w1, const LArCaliWave& w2, int& tbin, bool normPeak) const ;
  double diffWave(const LArCaliWave& w1, const LArCaliWave& w2, bool normPeak) const ;
  
  LArWaveHelper* m_waveHelper ;

  std::string m_keyinput, m_keyoutput ;
  
  // Grouping type
  std::string m_groupingType;
  
  int m_ADCsatur , m_DACmin ;
  bool m_timeShift , m_normalize , m_diffWaveNormPeak , m_buildMWoneDAC , m_listAllAnalysedChannels ;
  double m_linearityTolerance ;

  std::vector<unsigned> m_dacMinLayer0;
  std::vector<unsigned> m_dacMinLayer1;
  std::vector<unsigned> m_dacMinLayer2;
  std::vector<unsigned> m_dacMinLayer3;
  
  std::vector<unsigned> m_dacMaxLayer0;
  std::vector<unsigned> m_dacMaxLayer1;
  std::vector<unsigned> m_dacMaxLayer2;
  std::vector<unsigned> m_dacMaxLayer3;

};

#endif

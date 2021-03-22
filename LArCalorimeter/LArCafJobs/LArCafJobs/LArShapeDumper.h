/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArShapeDumper
 * @author Nicolas.Berger@cern.ch
 *   */

#ifndef LArShapeDumper_H
#define LArShapeDumper_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArCafJobs/DataStore.h"
#include "LArRawConditions/LArPhysWaveContainer.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/CaloGain.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include <fstream>
#include <string>
#include <stdint.h>
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "LArCafJobs/ILArShapeDumperTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "CaloConditions/CaloNoise.h"

#include "LArRawConditions/LArADC2MeV.h"
#include "LumiBlockData/BunchCrossingCondData.h"



class MsgStream;
class StoreGateSvc;
class ILArPedestal;
class CaloDetDescrManager;
class ILArBadChannelMasker;
class ILArADC2MeVTool;
class ILArShape;
class ILArAutoCorr;
class HWIdentifier;
class Identifier;
class CaloDetDescrElement;

namespace LArSamples {
  class EventData;
  class RunData;
}

namespace Trig {
  class ChainGroup;
}

class LArShapeDumper : public AthAlgorithm
{
 public:
  LArShapeDumper(const std::string & name, ISvcLocator * pSvcLocator);

  ~LArShapeDumper();

  //standart algorithm methods
  virtual StatusCode initialize();
  virtual StatusCode start();
  virtual StatusCode execute();
  virtual StatusCode stop();
  virtual StatusCode finalize();

  int makeEvent(LArSamples::EventData*& eventData, int run, int event, int lumiBlock, int bunchXing) const;
  
 private:
   
  std::string m_fileName;

  int m_prescale;  
  unsigned m_count;  
  unsigned m_maxChannels;
  unsigned m_nWrongBunchGroup;
  unsigned m_nPrescaledAway;
  unsigned m_nLArError;
  unsigned m_nNoDigits;


  std::string m_digitsKey, m_channelsKey;
  std::string m_caloType; // EM, HEC, FCAL
  double m_energyCut, m_noiseSignifCut;
  int m_minADCMax;
  std::string m_gainSpec;
  bool m_dumpDisc;
  std::vector<std::string> m_triggerNames;

  ToolHandle<ILArShapeDumperTool> m_dumperTool{this,"LArShapeDumperTool","LArShapeDumperTool"};
  ToolHandle<ILArBadChannelMasker> m_badChannelMasker;

  SG::ReadCondHandleKey<LArADC2MeV>   m_adc2mevKey{this,"ADC2MeVKey","LArADC2MeV","SG Key of ADC2MeV conditions object"};
 
  ToolHandle<Trig::TrigDecisionTool> m_trigDec;

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<LArBadChannelCont> m_BCKey{this, "BadChanKey", "LArBadChannel", "SG bad channels key"};
  SG::ReadCondHandleKey<CaloNoise> m_noiseCDOKey{this,"CaloNoiseKey","totalNoise","SG Key of CaloNoise data object"};

  ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc;  // for tests...

  SG::ReadCondHandleKey<BunchCrossingCondData> m_bcDataKey {this, "BunchCrossingCondDataKey", "BunchCrossingData" ,"SG Key of BunchCrossing CDO"};


  SG::ReadCondHandleKey<ILArPedestal> m_pedestalKey{this,"PedestalKey","LArPedestal","SG Key of LArPedestal object"};

  const ILArPedestal* m_larPedestal;
  const CaloDetDescrManager* m_caloDetDescrMgr;
  const LArOnlineID* m_onlineHelper;
  const DataHandle<ILArAutoCorr> m_autoCorr;
  //const DataHandle<LArPhysWaveContainer> m_physWave;

  bool m_doStream, m_doTrigger, m_doOFCIter, 
	m_doAllEvents, m_doRoIs, m_doAllLvl1, m_dumpChannelInfos;
  bool m_doEM, m_doHEC, m_doFCAL;
  bool m_gains[CaloGain::LARNGAIN];

  bool m_onlyEmptyBC;

  LArSamples::DataStore* m_samples;
  LArSamples::RunData* m_runData;
  std::vector<const Trig::ChainGroup*> m_triggerGroups;
  TRandom m_random;
};

#endif

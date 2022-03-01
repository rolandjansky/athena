/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDIGITIZATION_LARHITEMPATTODIGITALG_H
#define LARDIGITIZATION_LARHITEMPATTODIGITALG_H
// +======================================================================+
// +                                                                      +
// + Author ........: Denis O. Damazio                                    +
// + Institut ......: BNL                                                 +
// + Creation date .: 16/02/2022                                          +
// +                                                                      +
// +======================================================================+
//
// ....... include
//

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "LArSimEvent/LArHitContainer.h"

#include "LArRawEvent/LArDigitContainer.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteHandle.h"

#include "StoreGate/ReadCondHandle.h"
#include "LArRawConditions/LArADC2MeV.h"
#include "LArRawConditions/LArAutoCorrNoise.h"
#include "LArDigitization/LArHitEMap.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "LArElecCalib/ILArNoise.h"
#include "LArElecCalib/ILArOFC.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArfSampl.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArRecConditions/LArBadChannelMask.h"

// services
#include "AthenaKernel/IAthRNGSvc.h"
namespace CLHEP {
  class HepRandomEngine;
}
class StoreGateSvc;
class ITriggerTime;
class LArOnlineID;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class CaloCell_ID;

#define MAXADC 4096       // Maximal Adc count + 1 ( used for the overflows)

class LArHitEMapToDigitAlg : public AthReentrantAlgorithm
{
public:
  // Constructor
  LArHitEMapToDigitAlg(const std::string& name, ISvcLocator* pSvcLocator);

  // initialize all condition keys
  virtual StatusCode initialize();
  // Just do it
  virtual StatusCode execute(const EventContext& context) const;

protected:

  // access to many conditions
  template<class T> const T* pointerFromKey(const EventContext& context, const SG::ReadCondHandleKey<T>& key) const;

  StatusCode MakeDigit(const EventContext& ctx, const Identifier & cellId,
               const HWIdentifier & ch_id,
	       LArDigit*& DigitContainer, LArDigit*& DigitContainer_DigiHSTruth,
               const std::vector<std::pair<float,float> >* TimeE,
               const LArDigit * rndm_digit, CLHEP::HepRandomEngine * engine,
               const std::vector<std::pair<float,float> >* TimeE_DigiHSTruth = nullptr) const;
  
  
  StatusCode ConvertHits2Samples(const EventContext& ctx, const Identifier & cellId, HWIdentifier ch_id,
                   CaloGain::CaloGain igain,
                   const std::vector<std::pair<float,float> >  *TimeE,  std::vector<double> &sampleList) const;

  // Keys to many conditions
  SG::ReadCondHandleKey<ILArNoise>    m_noiseKey{this,"NoiseKey","LArNoiseSym","SG Key of ILArNoise object"};
  SG::ReadCondHandleKey<ILArfSampl>   m_fSamplKey{this,"fSamplKey","LArfSamplSym","SG Key of LArfSampl object"};
  SG::ReadCondHandleKey<ILArOFC>      m_OFCKey{this, "OFCKey", "LArOFC", "SG Key of OFC conditions object"};
  SG::ReadCondHandleKey<ILArPedestal> m_pedestalKey{this,"PedestalKey","LArPedestal","SG Key of LArPedestal object"};
  SG::ReadCondHandleKey<ILArShape>    m_shapeKey{this,"ShapeKey","LArShapeSym","SG Key of LArShape object"};
  SG::ReadCondHandleKey<LArADC2MeV>   m_adc2mevKey{this,"ADC2MeVKey","LArADC2MeV","SG Key of ADC2MeV conditions object"};
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  const LArOnOffIdMapping* m_cabling{}; //Set in perpareEvent, used also in mergeEvent

  SG::ReadCondHandleKey<LArAutoCorrNoise> m_autoCorrNoiseKey{this,"AutoCorrNoiseKey","LArAutoCorrNoise","SG Key of AutoCorrNoise conditions object"};

  SG::ReadCondHandleKey<LArBadChannelCont> m_bcContKey {this, "BadChanKey", "LArBadChannel", "SG key for LArBadChan object"};
  SG::ReadCondHandleKey<LArBadFebCont> m_badFebKey{this, "BadFebKey", "LArBadFeb", "Key of BadFeb object in ConditionsStore"};
  Gaudi::Property<std::vector<std::string> > m_problemsToMask{this,"ProblemsToMask",{},"Bad-Channel categories to mask entirly"}; 
  LArBadChannelMask m_bcMask;
 
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey{this,"CaloDetDescrManager", "CaloDetDescrManager"};
  
  // keys to inputs
  SG::ReadHandleKey<LArHitEMap> m_hitMapKey{this,"LArHitEMapKey","LArHitEMap"};
  SG::ReadHandleKey<LArHitEMap> m_hitMapKey_DigiHSTruth{this,"LArHitEMap_DigiHSTruthKey","LArHitEMap_DigiHSTruth"};
  SG::ReadHandleKey<LArDigitContainer> m_inputDigitContainerKey{this, "InputDigitContainer", "",
       "Name of input digit container"}; // input digit container name 
  // keys to output
  SG::WriteHandleKey<LArDigitContainer> m_DigitContainerName{this, "DigitContainer", "LArDigitContainer_MC",
       "Name of output digit container"};    // output digit container name list 
  SG::WriteHandleKey<LArDigitContainer>  m_DigitContainerName_DigiHSTruth{this, "DigitContainer_DigiHSTruth", 
       "LArDigitContainer_DigiHSTruth", "Name of output signal digit container"};    // output digit container name list
  Gaudi::Property<std::string> m_randomStreamName{this, "RandomStreamName", "LArDigitization", ""};
 
  // services needed
  ServiceHandle<IAthRNGSvc> m_rndmGenSvc{this, "RndmSvc", "AthRNGSvc", ""};
  Gaudi::Property<uint32_t> m_randomSeedOffset{this, "RandomSeedOffset", 2, ""}; //
  Gaudi::Property<bool> m_useLegacyRandomSeeds{this, "UseLegacyRandomSeeds", false,
       "Use MC16-style random number seeding"};

  enum CaloNum{EM,HEC,FCAL,EMIW};
  double m_LowGainThresh[4]{};
  double m_HighGainThresh[4]{};
  // Some properties for digits production
  Gaudi::Property<double> m_EnergyThresh{this, "EnergyThresh", -99.,
       "Hit energy threshold (default=-99)"};           // Zero suppression energy threshold
  Gaudi::Property<int>    m_NSamples{this, "Nsamples", 5,
       "Number of ADC samples (default=5)"};               // number of samples in Digit
  Gaudi::Property<bool> m_NoiseOnOff{this, "NoiseOnOff", true,
      "put electronic noise (default=true)"};            // noise (in all sub-detectors) is on if true
  Gaudi::Property<unsigned int> m_firstSample{this, "firstSample", 0,
       "First sample to use for the shape for in-time signal"};      // first sample to use for pulse shape for in time energy deposit
  Gaudi::Property<bool> m_usePhase{this, "UsePhase", false,
       "use 1ns binned pulse shape (default=false)"};               // use tbin phase to get shape (default = false for Atlas)
  Gaudi::Property<bool> m_RndmEvtOverlay{this, "RndmEvtOverlay", false,
      "Pileup and/or noise added by overlaying random events (default=false)"};         // Pileup and noise added by overlaying random events
  Gaudi::Property<bool> m_isMcOverlay{this, "isMcOverlay", false,
      "Is input Overlay from MC or data (default=false, from data)"};             // true if input RDO for overlay are from MC, false if from data
  Gaudi::Property<bool> m_doDigiTruth{this, "DoDigiTruthReconstruction", false,
      "Also create information about reconstructed digits for HS hits"};
  // Switches (true by default) on Noise for each sub-detector (can be combined)
  Gaudi::Property<bool> m_NoiseInEMB {this, "NoiseInEMB", true,
       "put noise in EMB (default=true)"};               // noise in Barrel is off if false
  Gaudi::Property<bool> m_NoiseInEMEC{this, "NoiseInEMEC", true,
       "put noise in EMEC (default=true)"};              // noise in EndCap is off if false
  Gaudi::Property<bool> m_NoiseInHEC{this, "NoiseInHEC", true,
       "put noise in HEC (default=true)"};                  // noise in HEC is off if false
  Gaudi::Property<bool> m_NoiseInFCAL{this, "NoiseInFCAL", true,
       "put noise in FCAL (default=true)"};              // noise in FCAL is off if false
  Gaudi::Property<bool> m_pedestalNoise{this, "PedestalNoise", false,
      "Use noise from Pedestal structure instead of LArNoise (default=false)"};
  Gaudi::Property<bool> m_roundingNoNoise{this, "RoundingNoNoise", true,
      "if true add random number [0:1[ in no noise case before rounding ADC to integer, if false add only 0.5 average"};  // flag used in NoNoise case: if true add random number [0;1[ in ADC count, if false add only average of 0.5
  Gaudi::Property<bool> m_Windows{this, "Windows", false,
      "Window mode (produce digits only around true e/photon) (default=false)"};
  Gaudi::Property<float> m_WindowsEtaSize{this, "WindowsEtaSize", 0.4,
      "Eta size of window (default=0.4)"};
  Gaudi::Property<float> m_WindowsPhiSize{this, "WindowsPhiSize", 0.5,
      "Phi size of window (default=0.5)"};
  Gaudi::Property<float> m_WindowsPtCut{this, "WindowsPtCut", 5000.,
      "Pt cut on e/photons for window mode (Default=5GeV)"};


  const CaloCell_ID*     m_calocell_id{};
  const LArEM_ID*        m_larem_id{};
  const LArHEC_ID*       m_larhec_id{};
  const LArFCAL_ID*      m_larfcal_id{};
  const LArOnlineID*     m_laronline_id{};

};

template<class T>
const T* LArHitEMapToDigitAlg::pointerFromKey(const EventContext& context, const SG::ReadCondHandleKey<T>& key) const {
  SG::ReadCondHandle<T> aHandle(key, context);
  const T* object = *aHandle;
  if (object == nullptr) ATH_MSG_ERROR("Object could not be fetched with key " << aHandle.key() );
  return object;
}

#endif


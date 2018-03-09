/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDIGITIZATION_LARPILEUPTOOL_H
#define LARDIGITIZATION_LARPILEUPTOOL_H
// +======================================================================+
// +                                                                      +
// +  Author : G.Unal                                                     +
// +  move LAr digitization framework to use PileUpTool                   +
// +                                                                      +
// +======================================================================+
//
// ....... include
//

#include "PileUpTools/PileUpToolBase.h"
#include "LArDigitization/ILArPileUpTool.h"

#include "AthenaKernel/IAtRndmGenSvc.h"

#include "CaloIdentifier/CaloGain.h"

#include "LArElecCalib/ILArNoise.h"
#include "LArElecCalib/ILArAutoCorrNoiseTool.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArADC2MeVTool.h"
#include "LArElecCalib/ILArfSampl.h"

#include "LArRecConditions/ILArBadChannelMasker.h"
#include "LArRecConditions/ILArBadChanTool.h"

#include "StoreGate/DataHandle.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Property.h"

class StoreGateSvc;
class PileUpMergeSvc;
class ITriggerTime;
class LArDigitContainer;
class LArOnlineID;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class LArHitEMap;
class LArDigit;
class ILArOFC;
namespace CLHEP {
  class HepRandomEngine;
}

class LArPileUpTool : virtual public ILArPileUpTool, public PileUpToolBase
{
//
// >>>>>>>> public method
//
 public:

  LArPileUpTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);

  ~LArPileUpTool();

  virtual StatusCode initialize() override final;

  virtual StatusCode prepareEvent(unsigned int nInputEvents) override final;

  virtual StatusCode mergeEvent() override final;

  virtual StatusCode processBunchXing(int bunchXing,
                                      SubEventIterator bSubEvents,
                                      SubEventIterator eSubEvents) override final;

  virtual StatusCode processAllSubEvents() override final;

  virtual StatusCode fillMapFromHit(StoreGateSvc* seStore,float tbunch,bool isSignal) override final;

  static const InterfaceID& interfaceID() {
    return ILArPileUpTool::interfaceID();}

 private:

#define MAXADC 4096       // Maximal Adc count + 1 ( used for the overflows)


  StatusCode AddHit(const Identifier & cellId, float energy, float time, bool iSignal, unsigned int offset, unsigned int ical);


  StatusCode MakeDigit(const Identifier & cellId,
                   HWIdentifier & ch_id,
                   const std::vector<std::pair<float,float> >  *TimeE,
                   //const LArDigit * rndm_digit);
                   const LArDigit * rndm_digit, const std::vector<std::pair<float,float> > *TimeE_DigiHSTruth = nullptr);


  StatusCode ConvertHits2Samples(const Identifier & cellId,
                   CaloGain::CaloGain igain,
                   //const std::vector<std::pair<float,float> >  *TimeE);
                   const std::vector<std::pair<float,float> >  *TimeE,  std::vector<double> &sampleList);


  float  get_strip_xtalk(int eta);
  float  get_middleback_xtalk(int eta);
  float  get_strip_xtalk_ec(int region, int eta);
  float  get_middleback_xtalk_ecow(int eta);
  float  get_middleback_xtalk_eciw(int eta);
  float  get_stripmiddle_xtalk(int eta);
  float  get_stripmiddle_xtalk_ec(int region, int eta);
  float  get_2strip_xtalk(int eta);
  float  get_2strip_xtalk_ec(int region,int eta);
  float  get_middle_xtalk1(int eta);
  float  get_middle_xtalk2(int eta);
  float  get_middle_xtalk1_ec(int eta);
  float  get_middle_xtalk2_ec(int eta);
  void   cross_talk(const IdentifierHash& idHash,
                    const Identifier& cellId,
                    const float& energy,
                    std::vector<IdentifierHash>& neighbourList,
                    std::vector<float>& energyList);
  bool  fillMapfromSum(float bunchTime);

//
// >>>>>>>> private data parts
//
  PileUpMergeSvc* m_mergeSvc;
  LArHitEMap* m_hitmap;   // map of hits in cell
  LArHitEMap* m_hitmap_DigiHSTruth;   // map of hits in cell 
  std::vector <std::string> m_HitContainer; // hit container name list
  std::vector<int> m_CaloType;

//
// ........ Algorithm properties
//
  std::string m_SubDetectors;      // subdetectors
  std::string m_DigitContainerName;    // output digit container name list
  std::string m_DigitContainerName_DigiHSTruth;    // output digit container name list
  LArDigitContainer* m_DigitContainer;
  LArDigitContainer* m_DigitContainer_DigiHSTruth;
  std::vector<std::string> m_EmBarrelHitContainerName;
  std::vector<std::string> m_EmEndCapHitContainerName;
  std::vector<std::string> m_HecHitContainerName;
  std::vector<std::string> m_ForWardHitContainerName;
  bool m_NoiseOnOff;            // noise (in all sub-detectors) is on if true
  bool m_PileUp;                // pile up or not
// Switches (true by default) on Noise for each sub-detector (can be combined)
  bool m_NoiseInEMB;               // noise in Barrel is off if false
  bool m_NoiseInEMEC;              // noise in EndCap is off if false
  bool m_NoiseInHEC;               // noise in HEC    is off if false
  bool m_NoiseInFCAL;              // noise in FCAL   is off if false
  //put false if you want cancel the noise in one or several sub-detectors
  bool m_CrossTalk;                // flag for Cross Talk
  bool m_CrossTalkStripMiddle;     // flag for strip-middle cross talk (if m_CrooTalk is true)
  bool m_CrossTalk2Strip;          // flag for 2nd neighbor cross-talk
  bool m_CrossTalkMiddle;          // flag for middle to middle cross-talk
  float m_scaleStripXtalk;          // scale factor for strip to strip cross-talk
  float m_scaleStripMiddle;         // scale factor for strip-middle cross-talk
  float m_scaleMiddleXtalk;         // scale factor for middle-middle cross-talk
// Windows mode
  bool m_Windows;
  float m_WindowsEtaSize;
  float m_WindowsPhiSize;
  float m_WindowsPtCut;
//
  enum CaloNum{EM,HEC,FCAL,EMIW};
  double m_LowGainThresh[4];       // energy thresholds for the low gain
  double m_HighGainThresh[4];      // energy thresholds for the high gain
  double m_EnergyThresh;           // Zero suppression energy threshold
  //double m_AdcPerGeV;              // adc = UnCalibretedEnergy*Gain/m_AdcPerGeV + Pedestal
  int    m_NSamples;               // number of samples in Digit
  unsigned int m_firstSample;      // first sample to use for pulse shape for in time energy deposit
  bool   m_usePhase;               // use tbin phase to get shape (default = false for Atlas)
  std::string  m_rndmSvc;          // random service name
  bool m_rndmEvtRun;               // use run,event number for random number seeding
  bool m_useTriggerTime;
  bool m_RndmEvtOverlay;         // Pileup and noise added by overlaying random events
  bool m_isMcOverlay;             // true if input RDO for overlay are from MC, false if from data
  bool m_useBad;
  std::string m_RandomDigitContainer; // random digit container name list

  std::string m_pedestalKey ;
  bool m_useMBTime;
  bool m_recordMap;
  bool m_useLArHitFloat;
  bool m_pedestalNoise;
  bool m_addPhase;
  float m_phaseMin;
  float m_phaseMax;
  bool m_ignoreTime;
  int m_sampleGainChoice;
  bool m_roundingNoNoise;  // flag used in NoNoise case: if true add random number [0;1[ in ADC count, if false add only average of 0.5

// Detector Description objects

  const DataHandle<ILArNoise>     m_dd_noise;
  const DataHandle<ILArfSampl>    m_dd_fSampl;
  const DataHandle<ILArPedestal>  m_dd_pedestal;
  const DataHandle<ILArShape>     m_dd_shape;
  ToolHandle<ILArADC2MeVTool>     m_adc2mevTool;
  ToolHandle<ILArAutoCorrNoiseTool> m_autoCorrNoiseTool;
  ToolHandle<ILArBadChannelMasker> m_maskingTool;
  ToolHandle<ILArBadChanTool> m_badChannelTool;
  ToolHandle<ITriggerTime> m_triggerTimeTool;

  const LArEM_ID*        m_larem_id;
  const LArHEC_ID*       m_larhec_id;
  const LArFCAL_ID*      m_larfcal_id;
  const LArOnlineID*     m_laronline_id;

  bool m_skipNoHit;

  IAtRndmGenSvc* m_AtRndmGenSvc;
  CLHEP::HepRandomEngine* m_engine;

  bool m_doDigiTruth;

  std::vector<double> m_Samples;
  std::vector<double> m_Samples_DigiHSTruth;
  std::vector<double> m_Noise;
  double m_Rndm[32];
  std::vector<bool> m_SubDetFlag;
  std::vector<float> m_energySum;
  std::vector<float> m_energySum_DigiHSTruth;
  int m_nhit_tot;
  float m_trigtime;
  unsigned int m_n_cells;

  const ILArOFC* m_larOFC;

};

#endif

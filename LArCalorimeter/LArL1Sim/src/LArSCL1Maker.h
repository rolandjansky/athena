/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARL1SIM_LARSCL1MAKER_H
#define LARL1SIM_LARSCL1MAKER_H
// +======================================================================+
// +                                                                      +
// + Author ........: Denis O. Damazio                                    +
// + Institut ......: BNL                                                 +
// + Creation date .: 18/11/2013                                          +
// +                                                                      +
// +======================================================================+
//
// ....... include
//

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAthRNGSvc.h"

#include "LArDigitization/LArHitEMap.h"
#include "LArRawConditions/LArAutoCorrNoise.h"
#include "LArRawConditions/LArADC2MeV.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRawEvent/LArDigitContainer.h"

#include "LArElecCalib/ILArfSampl.h"
#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArNoise.h"


class ITriggerTime;
class CaloCell_SuperCell_ID;
class CaloCell_ID;
class LArOnline_SuperCellID;

class CaloSuperCellDetDescrManager;

/**
   @brief The aim of this algorithm is the simulation of the LAr analogue Super-Cell sums. 

   It includes correct allocation of cells to Super-Cell, pulse profile as a function of time, saturation, appropriate noise addition, pile-up. <br>
   The resulting signals are an input to the Preprocessor (which in turn performs digitization, filtering, bunch crossing id., noise suppression,...) <br>
   Since it needs hits, the simulation only takes "simul" datasets as input, NOT digitized datasets.

   @warning although the output is not digitized, LArSCL1Maker is part of the digitization simulation.

   @author Denis O. Damazio (BNL)
 */

class LArSCL1Maker : public AthReentrantAlgorithm
{
//
// >>>>>>>> public methods
//
 public:
  /** constructor */
  LArSCL1Maker(const std::string& name, ISvcLocator* pSvcLocator);

  /** destructor */
  ~LArSCL1Maker();
//
// ..... Gaudi algorithm hooks
//
  /**  Read ascii files for auxiliary data (puslse shapes, noise, etc...) */
  StatusCode initialize();
  /**       Create  LArSCL1  object
      save in TES (2 containers: 1 EM, 1 hadronic)
  */
  StatusCode execute(const EventContext& context) const;

  StatusCode finalize();


 private:

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKeySC{this,"ScCablingKey","LArOnOffIdMapSC","SG Key of SC LArOnOffIdMapping object"};
  /// Property: Pulse shape (conditions input).
  SG::ReadCondHandleKey<ILArShape> m_shapesKey
  {this, "ShapeKey", "LArShapeSC", "SG Key of Shape conditions object"};
 
  /// Property: Fraction of Energy Sampled (conditions input).
  SG::ReadCondHandleKey<ILArfSampl> m_fracSKey
  {this, "FracSKey", "LArfSamplSC", "SG Key of fSamplS conditions object"};
 
  /// Property: Pedestal offset (conditions input).
  SG::ReadCondHandleKey<ILArPedestal> m_pedestalSCKey
  {this, "PedestalKey", "LArPedestalSC", "SGKey of LArPedestal object"};

  /// Property: Electronics Noise (conditions input).
  SG::ReadCondHandleKey<ILArNoise> m_noiseSCKey
  {this, "LArNoiseKey", "LArNoiseSC", "SGKey of LArNoise object"};

  /// Property: AutoCorr Noise (conditions input).
  SG::ReadCondHandleKey<LArAutoCorrNoise> m_autoCorrNoiseSCKey
  {this, "LArAutoCorrKey", "LArAutoCorrNoiseSC", "SGKey of LArAutoCorrNoise object"};

  /// Property: ADC2MeV conversion (conditions input).
  SG::ReadCondHandleKey<LArADC2MeV> m_adc2mevSCKey
  {this, "LArADC2MeVKey", "LArADC2MeVSC", "SGKey of LArADC2MeV object"};

  template <class T>
  const T* retrieve(const EventContext& context, SG::ReadCondHandleKey<T> handleKey) const {
	SG::ReadCondHandle<T> handle( handleKey, context);
	if ( not handle.isValid() ) {
		ATH_MSG_ERROR("could not retrieve : " << handle.key() );
		return nullptr;
	} else return handle.cptr();
  }

//
// >>>>>>>> private algorithm parts
//
  /** initialize hit map 
   */

  std::vector<float> computeSignal(const Identifier towerId, const int Ieta, const int specialCase,
				   std::vector<float> visEnergy, const int refTime) const;

  std::vector<float> computeNoise(const Identifier towerId, const int Ieta,
				  std::vector<float>& inputV) ;


  /** Method to print SuperCell Conditions */
  void printConditions(const HWIdentifier& hwSC);

  /** Method for converting Hits from samples (simplified version
 * of the same method in LarPileUpTool) */
 void ConvertHits2Samples(const EventContext& context, const HWIdentifier & hwSC, CaloGain::CaloGain igain,
       const std::vector<std::pair<float,float> >& TimeE,
	std::vector<float>& samples) const;

//
// >>>>>>>> private data parts
//


  IChronoStatSvc*              m_chronSvc;
  Gaudi::Property<std::string> m_randomStreamName{this, "RandomStreamName", "LArSCL1Maker", ""};
  ServiceHandle<IAthRNGSvc> m_atRndmGenSvc{this, "RndmSvc", "AthRNGSvc", ""};
  Gaudi::Property<uint32_t> m_randomSeedOffset{this, "RandomSeedOffset", 2, ""};
  Gaudi::Property<bool> m_useLegacyRandomSeeds{this, "UseLegacyRandomSeeds", false, "Use MC16-style random number seeding"};


  /** Alorithm property: use trigger time or not*/
  bool m_useTriggerTime;
  /** pointer to the TriggerTimeTool */
  //ToolHandle<ITriggerTime> p_triggerTimeTool;
 
  int m_BeginRunPriority;

  ToolHandle<ICaloSuperCellIDTool>           m_scidtool;
  /** pointer to the offline TT helper */
  const CaloCell_SuperCell_ID*     m_scHelper;
  /** pointer to the offline id helper  */
  const CaloCell_ID*               m_OflHelper = nullptr;
  /** pointer to the online LAr helper */
  const LArOnline_SuperCellID*   m_OnlSCHelper;
  

   ServiceHandle<IIncidentSvc> m_incSvc;

  /** number of sampling (in depth) */
  static const short s_NBDEPTHS = 4 ;          
  /** number of samples in TTL1s */
  static const short s_NBSAMPLES = 7 ;         
  /** max number of samples in pulse shape */
  static const short s_MAXSAMPLES = 24 ;       
  /** peak position  */
  static const short s_PEAKPOS = 3 ;           
  /** number of eta bins */
  static const short s_NBETABINS = 15 ;        
  /** number of energies at which saturation is described (em) */
  static const short s_NBENERGIES = 12 ;       

  /** auxiliary HEC data: auto-correlation matrix */
  std::vector< std::vector<float> > m_autoCorrHec ;

  /** hit map */
  SG::ReadHandleKey<LArHitEMap> m_hitMapKey{this,"LArHitEMapKey","LArHitEMap"};
  /** Background Digit Overlay, default key is Bkg_LArDigitSCL2 */
  SG::ReadHandleKey<LArDigitContainer> m_bkgDigitsKey{ this, "BkgDigitKey", ""};
  /** output Lar Digits SC container */
  SG::WriteHandleKey<LArDigitContainer> m_sLArDigitsContainerKey {this, "SCL1ContainerName","LArDigitSCL1","Output LArDigit container"};


  /** list of hit containers */
  std::vector <std::string> m_HitContainer;

/** algorithm property: sub-detectors to be simulated */
  std::string m_SubDetectors;          

#ifdef DONTDO
/** algorithm property: container name of the EMB hits */
  std::string m_EmBarrelHitContainerName;
/** algorithm property: container name of the EMEC hits */
  std::string m_EmEndCapHitContainerName;
/** algorithm property: container name of the HEC hits */
  std::string m_HecHitContainerName;
/** algorithm property: container name of the FCAL hits */
  std::string m_ForWardHitContainerName;
#endif


/** algorithm property: noise (in all sub-detectors) is on if true         */
  bool m_NoiseOnOff;               
/** algorithm property: pile up or not */
  bool m_PileUp;                   
/** algorithm property: no calibration mode for EM towers */
  bool m_noEmCalibMode;            
/** algorithm property: no calibration mode for had towers */
  bool m_noHadCalibMode;            
/** algorithm property: switch chrono on */
  bool m_chronoTest;               

  /** output number of samples */
  unsigned int m_nSamples;
  /** output first samples */
  unsigned int m_firstSample;

   std::string m_saveHitsContainer;

   const CaloSuperCellDetDescrManager* m_sem_mgr;

};

#endif


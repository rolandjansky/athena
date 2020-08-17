/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARL1SIM_LARTTL1MAKER_H
#define LARL1SIM_LARTTL1MAKER_H
// +======================================================================+
// +                                                                      +
// + Author ........: F. Ledroit                                          +
// + Institut ......: ISN Grenoble                                        +
// + Creation date .: 09/01/2003                                          +
// +                                                                      +
// +======================================================================+
//
// ....... include
//

#include "AthenaBaseComps/AthAlgorithm.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"

#include "LArDigitization/LArHitEMap.h"
#include "LArElecCalib/ILArfSampl.h"

#include "AthenaKernel/IAthRNGSvc.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "LArSimEvent/LArHitContainer.h"
#include "LArRawEvent/LArTTL1Container.h"

class ITriggerTime;
class CaloTriggerTowerService;
class CaloLVL1_ID;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class LArHitEMap;

/**
   @brief The aim of this algorithm is the simulation of the LAr analogue trigger tower sums. 

   It includes correct allocation of cells to towers, pulse profile as a function of time, saturation, appropriate noise addition, pile-up. <br>
   The resulting signals are an input to the Preprocessor (which in turn performs digitization, filtering, bunch crossing id., noise suppression,...) <br>
   Since it needs hits, the simulation only takes "simul" datasets as input, NOT digitized datasets.

   @warning although the output is not digitized, LArTTL1Maker is part of the digitization simulation.

   @author F. Ledroit (LPSC-Grenoble)
 */

class LArTTL1Maker : public AthAlgorithm,
                     public IIncidentListener 
{
//
// >>>>>>>> public methods
//
 public:
  /** constructor */
  LArTTL1Maker(const std::string& name, ISvcLocator* pSvcLocator);

  /** destructor */
  ~LArTTL1Maker();
//
// ..... Gaudi algorithm hooks
//
  /**  Read ascii files for auxiliary data (puslse shapes, noise, etc...) */
  virtual StatusCode initialize();
  /**       Create  LArTTL1  object
      save in TES (2 containers: 1 EM, 1 hadronic)
  */
  virtual StatusCode execute();

  virtual StatusCode finalize();
  virtual void handle(const Incident&);


 private:

//
// >>>>>>>> private algorithm parts
//

  /** initialize hit map 
   */

  //virtual StatusCode initHitMap();

  std::vector<float> computeSignal(const Identifier towerId, const int Ieta, const int specialCase,
				   std::vector<float> visEnergy, const int refTime) const;

  std::vector<float> computeNoise(const Identifier towerId, const int Ieta,
				  std::vector<float>& inputV) ;

  /** method called at the begining of execute() to fill the hit map */
  //StatusCode fillEMap(int& totHit) ;

  /** method called at initialization to read auxiliary data from ascii files */
  StatusCode readAuxiliary();

  int decodeInverse(int region, int eta);

//
// >>>>>>>> private data parts
//

  IChronoStatSvc*              m_chronSvc;
  ServiceHandle<IAthRNGSvc> m_RandomSvc{this, "RndmSvc", "AthRNGSvc", ""};

  /** Alorithm property: use trigger time or not*/
  bool m_useTriggerTime;
  /** Alorithm property: name of the TriggerTimeTool*/
  ToolHandle<ITriggerTime> m_triggerTimeTool;

  int m_BeginRunPriority;

  ToolHandle<CaloTriggerTowerService>  m_ttSvc;
  /** pointer to the offline TT helper */
  const CaloLVL1_ID*           m_lvl1Helper;
  /** pointer to the offline EM helper */
  const LArEM_ID*              m_emHelper;
  /** pointer to the offline HEC helper */
  const LArHEC_ID*             m_hecHelper;
  /** pointer to the offline FCAL helper */
  const LArFCAL_ID*            m_fcalHelper;
 /** pointer to the offline id helper  */
  const CaloCell_ID*           m_OflHelper;
  /** Sampling fractions retrieved from DB */
  //const DataHandle<ILArfSampl>    m_dd_fSampl;
  SG::ReadCondHandleKey<ILArfSampl> m_fSamplKey;

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

  /** auxiliary EM data: reference energies for saturation simulation */
  std::vector<float> m_refEnergyEm ;
  /** auxiliary EM data: pulse shapes */
  std::vector<std::vector<float> > m_pulseShapeEm ;
  /** auxiliary EM data: pulse shape derivative */
  std::vector<std::vector<float> > m_pulseShapeDerEm ;
  /** auxiliary EM data: auto-correlation matrix */
  std::vector<float> m_autoCorrEm ;

  /** auxiliary EMBarrel data: sin(theta) */
  std::vector<float> m_sinThetaEmb ;
  /** auxiliary EMBarrel data: calibration coefficient */
  std::vector<float> m_calibCoeffEmb ;
  /** auxiliary EMBarrel data:  noise rms */
  std::vector<float> m_noiseRmsEmb ;
  // if later we want to disentangle between pre-sum and summing electronic noise...
  //  std::vector<std::vector<float> > m_noiseRmsEmb ;

  /** auxiliary EMEC data: sin(theta)  */
  std::vector<float>  m_sinThetaEmec ;
  /** auxiliary EMEC data: calibration coeeficient */
  std::vector<float>  m_calibCoeffEmec ;
  /** auxiliary EMEC data: noise rms */
  std::vector<float> m_noiseRmsEmec ;
  // if later we want to disentangle between pre-sum and summing electronic noise...
  //  std::vector<std::vector<float> > m_noiseRmsEmec ;

  /** auxiliary HEC data: pulse shape */
  std::vector<float> m_pulseShapeHec ;
  /** auxiliary HEC data: pulse shape derivative */
  std::vector<float> m_pulseShapeDerHec ;
  /** auxiliary HEC data: calibration coefficients */
  std::vector<float> m_calibCoeffHec ;
  /** auxiliary HEC data: sin(theta) */
  std::vector<float> m_sinThetaHec ;
  /** auxiliary HEC data: saturation energy */
  std::vector<float> m_satEnergyHec ;
  /** auxiliary HEC data: noise rms */
  std::vector<float> m_noiseRmsHec ;
  /** auxiliary HEC data: auto-correlation matrix */
  std::vector< std::vector<float> > m_autoCorrHec ;

  /** auxiliary FCAL data: relative gains */
  std::vector<float> m_cellRelGainFcal ;
  /** auxiliary FCAL data: pulse shapes */
  std::vector< std::vector<float> > m_pulseShapeFcal ;
  /** auxiliary FCAL data: pulse shape derivatives */
  std::vector< std::vector<float> > m_pulseShapeDerFcal ;
  /** auxiliary FCAL data: calibration coefficients */
  std::vector< std::vector<float> > m_calibCoeffFcal ;
  /** auxiliary FCAL data: calibration coefficients */
  std::vector<float> m_calibCoeffFcalEm ;
  /** auxiliary FCAL data: calibration coefficients */
  std::vector<float> m_calibCoeffFcalHad ;
  /** auxiliary FCAL data: noise rms */
  //  std::vector<float> m_noiseRmsFcal ;
  std::vector< std::vector<float> > m_noiseRmsFcal ;
  /** auxiliary FCAL data: auto-correlation matrix */
  std::vector<float> m_autoCorrFcal ;

  /** hit map */
  SG::ReadHandleKey<LArHitEMap> m_hitMapKey{this,"LArHitEMapKey","LArHitEMap"};

/** algorithm property: container name for the EM TTL1s */
  SG::WriteHandleKey<LArTTL1Container> m_EmTTL1ContainerName;   
/** algorithm property: container name for the HAD TTL1s */
  SG::WriteHandleKey<LArTTL1Container> m_HadTTL1ContainerName;  

  std::array<SG::ReadHandleKey<LArHitContainer>,4> m_xxxHitContainerName; 

/** algorithm property: noise (in all sub-detectors) is on if true         */
  bool m_NoiseOnOff;               
/** algorithm property: pile up or not */
  bool m_PileUp;                   
/** algorithm property: no calibration mode for EM towers */
  bool m_noEmCalibMode;            
/** algorithm property: no calibration mode for had towers */
  bool m_noHadCalibMode;            
/** algorithm property: debug threshold */
  float m_debugThresh;              
/** algorithm property: switch chrono on */
  bool m_chronoTest;               

/** key for saving truth */
   std::string m_truthHitsContainer;

};

#endif

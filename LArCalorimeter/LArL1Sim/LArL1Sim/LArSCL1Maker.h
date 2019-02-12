/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"

#include "LArDigitization/LArHitEMap.h"
#include "LArElecCalib/ILArAutoCorrNoiseTool.h"
#include "LArElecCalib/ILArADC2MeVTool.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"



class PileUpMergeSvc;
class IAtRndmGenSvc;
class ITriggerTime;
class CaloCell_SuperCell_ID;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class LArOnline_SuperCellID;
class ILArShape;
class ILArfSampl;
class ILArPedestal;
class ILArNoise;

class CaloSuperCellDetDescrManager;

namespace CLHEP
{
  class HepRandomEngine;
}

/**
   @brief The aim of this algorithm is the simulation of the LAr analogue Super-Cell sums. 

   It includes correct allocation of cells to Super-Cell, pulse profile as a function of time, saturation, appropriate noise addition, pile-up. <br>
   The resulting signals are an input to the Preprocessor (which in turn performs digitization, filtering, bunch crossing id., noise suppression,...) <br>
   Since it needs hits, the simulation only takes "simul" datasets as input, NOT digitized datasets.

   @warning although the output is not digitized, LArSCL1Maker is part of the digitization simulation.

   @author Denis O. Damazio (BNL)
 */

class LArSCL1Maker : public AthAlgorithm,
                     public IIncidentListener 
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
  virtual StatusCode initialize();
  /**       Create  LArSCL1  object
      save in TES (2 containers: 1 EM, 1 hadronic)
  */
  virtual StatusCode execute();

  virtual StatusCode finalize();
  virtual void handle(const Incident&);


 private:

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKeySC{this,"ScCablingKey","LArOnOffIdMapSC","SG Key of SC LArOnOffIdMapping object"};

//
// >>>>>>>> private algorithm parts
//
  /** initialize hit map 
   */

  std::vector<float> computeSignal(const Identifier towerId, const int Ieta, const int specialCase,
				   std::vector<float> visEnergy, const int refTime) const;

  std::vector<float> computeNoise(const Identifier towerId, const int Ieta,
				  std::vector<float>& inputV) ;



  /** Method to update all conditions */
  StatusCode updateConditions();

  /** Method to print SuperCell Conditions */
  void printConditions(const HWIdentifier& hwSC);

  /** Method for converting Hits from samples (simplified version
 * of the same method in LarPileUpTool) */
 void ConvertHits2Samples(const HWIdentifier & hwSC, CaloGain::CaloGain igain,
       const std::vector<std::pair<float,float> >  *TimeE,
	std::vector<float>& samples);

//
// >>>>>>>> private data parts
//


  IChronoStatSvc*              m_chronSvc;
  PileUpMergeSvc*              m_mergeSvc;
  ServiceHandle<IAtRndmGenSvc> m_atRndmGenSvc;
  std::string                  m_rndmEngineName;
  CLHEP::HepRandomEngine*      m_rndmEngine;

  /** Alorithm property: use trigger time or not*/
  bool m_useTriggerTime;
  /** pointer to the TriggerTimeTool */
  ToolHandle<ITriggerTime> p_triggerTimeTool;
 
  int m_BeginRunPriority;

  ToolHandle<ICaloSuperCellIDTool>           m_scidtool;
  /** pointer to the offline TT helper */
  const CaloCell_SuperCell_ID*     m_scHelper;
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
  LArHitEMap* m_hitmap; // map of hits in cell 
  /** list of hit containers */
  std::vector <std::string> m_HitContainer;

/** algorithm property: sub-detectors to be simulated */
  std::string m_SubDetectors;          
/** algorithm property: container name for the EM TTL1s */
  std::string m_SCL1ContainerName;   

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

/** Conditions (shape) of SuperCell */
  const ILArShape* m_shapes;
/** Conditions (fSamples) of SuperCell */
  const ILArfSampl* m_fracS;
/** Conditions (LArPedestalSC) of SuperCell */
  const ILArPedestal* m_PedestalSC;
/** Conditions (LArNoiseSC) of SuperCell */
  const ILArNoise* m_NoiseSC;
/** Special Tool for AutoCorrelation sqrt */
  ToolHandle<ILArAutoCorrNoiseTool> m_autoCorrNoiseTool;
/** Special Tool for AutoCorrelation sqrt */
  ToolHandle<ILArADC2MeVTool> m_adc2mevTool;
/** key for fSampl conditions */
  std::string m_fSamplKey;
/** key for Shape conditions */
  std::string m_shapesKey;
/** key for Noise conditions */
  std::string m_noiseKey;
/** key for Pedestal conditions */
  std::string m_pedestalKey;

  /** output number of samples */
  unsigned int m_nSamples;
  /** output first samples */
  unsigned int m_firstSample;
  /** m_first */
  bool m_first;

   std::string m_saveHitsContainer;

   const CaloSuperCellDetDescrManager* m_sem_mgr;

};

#endif


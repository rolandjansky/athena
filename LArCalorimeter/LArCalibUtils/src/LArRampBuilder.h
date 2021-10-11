//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/



/** This algorithm collects data for the Ramp-fitting
    from a LArCalibDigitContainer. They key of the
    containers that are to be processed  is defined 
    by the jobOption 'KeyList'. The resulting 
    containers are put in the DetectorStore with the 
    same keys. 
   * @authors W. Lampl, S. Laplace
   * 20. 1. 2004
   * Modifications:
   * - S. Laplace, Oct. 2004: add bias-corrected parabola reconstruction of ADCMax
   *                          as well as pseudo-OF method
   * - S. Laplace, March 2005: re-order operations of reconstruction:
   *       - first average samples
   *       - then reconstruct maximum of averaged samples
   * - S. Laplace, spring 2005: use one calibration wave per DAC
   *        
   */
  

#ifndef LARRAMPBUILDER_H
#define LARRAMPBUILDER_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "LArRawConditions/LArRawRampContainer.h"
#include "LArRecUtils/LArParabolaPeakRecoTool.h"
#include "LArRecUtils/LArShapePeakRecoTool.h"
#include "LArRecUtils/LArOFPeakRecoTool.h"

#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArEM_SuperCell_ID.h"

#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArRinj.h"
#include "LArRawConditions/LArCaliWaveContainer.h"

#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArBadChannelMask.h"

#include "LArRecConditions/LArCalibLineMapping.h"

#include "LArCalibTriggerAccumulator.h"
#include <vector>
#include <string>
#include <map>
#include <memory>

class LArOnlineID_Base;
class LArCalibTriggerAccumulator;

class LArRampBuilder : public AthAlgorithm//, public IIncidentListener 
{
 
public:
  using AthAlgorithm::AthAlgorithm;
  ~LArRampBuilder() {};

  //standard algorithm methods
  StatusCode initialize();
  StatusCode execute();
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}

  typedef std::map<uint32_t,LArCalibTriggerAccumulator> ACCRAMP;


private:
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this, "OnOffMap", "LArOnOffIdMap", "SG key for mapping object"};
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKeySC{this,"ScCablingKey","LArOnOffIdMapSC","SG Key of SC LArOnOffIdMapping object"};

  //Private member functions
  // choose reconstruction mode (i.e. OF or Parabola)
  void chooseRecoMode() ;
  //Does the fitting of the raw ramp. Result is returned as LArRampDB_t.
  StatusCode rampfit(unsigned deg, const std::vector<LArRawRamp::RAMPPOINT_t>& data, 
		     std::vector<float>& rampCoeffs, std::vector<int>& vSat, 
		     const HWIdentifier chid, const LArOnOffIdMapping* cabling,
                     const LArBadChannelCont* bcCont);

  std::unique_ptr<LArConditionsContainer<ACCRAMP> > m_ramps;


  PublicToolHandle<LArParabolaPeakRecoTool> m_peakParabolaTool{this,"LArParabolaPeakRecoTool","LArParabolaPeakRecoTool"};
  PublicToolHandle<LArShapePeakRecoTool>    m_peakShapeTool{this,"LArShapePeakRecoTool","LArShapePeakRecoTool"};
  ToolHandle<LArOFPeakRecoTool>       m_peakOFTool{this,"PeakOFTool","LArOFPeakRecoTool"};

  unsigned m_event_counter=0; 
  int m_delay=-1;
  int m_ipassShape=0; 

  std::vector< std::vector< std::vector< std::vector<double> > > >m_CaliWaves;
 // vector (gain) of vector(HashCell) of vector (DAC) of DACs
  std::vector< std::vector< std::vector< unsigned int > > >m_CaliDACs;
 // vector (gain) of vector(HashCell) of DAC0 index
  std::vector< std::vector< int> > m_IndexDAC0;
  std::vector< std::vector< int> > m_IndexHighestDAC;

  std::vector<float> m_thePedestal;    

  
  
  enum recoType {PARABOLA,SHAPE,OF};  
  recoType m_recoType{OF};


  //Algorithm-Properties:
  Gaudi::Property<std::vector<std::string> > m_keylist{this, "KeyList",{},"List if input SG keys"};
  Gaudi::Property<std::string> m_keyoutput{this,"KeyOutput","LArRamp","SG Key of output object"};
    
  IntegerProperty m_degree{this,"Polynom",1,"Degree of ramp-polynom"};
  IntegerProperty m_maxADC{this,"RampRange",0,"Ignore ADC values higher than this (0: do nothing)"};
  BooleanProperty m_dac0sub{this,"SubtractDac0",true,"Take first DAC value as pedestal"};
  UnsignedIntegerProperty m_DAC0{this,"DAC0",0,"DAC value considered DAC0"};
  BooleanProperty m_saveRawRamp{this,"StoreRawRamp",false,"Save raw ramp obj in SG"};
  BooleanProperty m_saveRecRamp{this,"StoreRecRamp",true,"Save reconstructed ramp obj in SG"};
  BooleanProperty m_satSlope{this,"doSatSlope","Ignore points subject to saturation"};
  UnsignedIntegerProperty m_minDAC{this,"minDAC",0,"Ignore DAC values smaller that this"};
  StringProperty m_recoTypeProp{this,"RecoType","OF","One of 'Parabola', 'Shape' or 'OF'"}; 
  IntegerProperty m_deadChannelCut{this,"DeadChannelCut",1300,"Complain about channels with max-ADC below this value"};
  BooleanProperty m_correctBias{this,"correctBias",false,"For Parabola method: Correction yes/no"};
  BooleanProperty m_withIntercept{this,"WithIntercept",true,"False: Force fit to go through 0/0"};
  FloatProperty m_delayShift{this,"DelayShift",23};  //Only for OF peak reco
  
  Gaudi::Property<bool> m_doBadChannelMask{this,"IgnoreBadChannels",true,"Don't complain about known bad channels"};
  LArBadChannelMask m_bcMask;
  SG::ReadCondHandleKey<LArBadChannelCont> m_bcContKey {this, "BadChanKey", "LArBadChannel", "SG key for LArBadChan object"};
  Gaudi::Property<std::vector<std::string> > m_problemsToMask{this,"ProblemsToMask",{}, "Bad-Channel categories to mask"};
  Gaudi::Property<std::string> m_groupingType{this,"GroupingType","ExtendedFeedThrough","Grouping of the output conditions-container"};
  Gaudi::Property<std::string> m_hec_key{this,"HECKey","","SG Key of injection-resistor obj used for HEC Ramps"};

  const LArOnlineID_Base* m_onlineHelper=nullptr;
  const LArEM_Base_ID* m_emId=nullptr;


  const DataHandle<ILArRinj> m_dd_rinj;
  
  Gaudi::Property<bool>  m_isSC{this,"isSC",false,"Processing SC data yes/no"};
  Gaudi::Property<bool>  m_ishec{this,"isHEC",false,"Processing HEC data yes/no"};
  Gaudi::Property<bool>  m_iterate{this,"Iterate",false,"Iterative OF peak reco"};

  uint16_t m_fatalFebErrorPattern=0xffff;

};

#endif

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

#include "LArRecConditions/LArCalibLineMapping.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include <vector>
#include <string>
#include <map>

class ILArBadChannelMasker;
class LArOnlineID_Base;
class LArCalibTriggerAccumulator;

class LArRampBuilder : public AthAlgorithm//, public IIncidentListener 
{
 
public:
  LArRampBuilder(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArRampBuilder();

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
		     const HWIdentifier chid, const LArOnOffIdMapping* cabling);

  LArConditionsContainer<ACCRAMP>* m_ramps;


  ToolHandle<LArParabolaPeakRecoTool> m_peakParabolaTool;
  ToolHandle<LArShapePeakRecoTool>    m_peakShapeTool;
  ToolHandle<LArOFPeakRecoTool>       m_peakOFTool;
  ToolHandle<ICaloSuperCellIDTool> m_sc2ccMappingTool;

  unsigned m_event_counter; 
  int m_delay;
  int m_ipassShape; 
  int m_ipassPedestal; 

  std::vector< std::vector< std::vector< std::vector<double> > > >m_CaliWaves;
 // vector (gain) of vector(HashCell) of vector (DAC) of DACs
  std::vector< std::vector< std::vector< unsigned int > > >m_CaliDACs;
 // vector (gain) of vector(HashCell) of DAC0 index
  std::vector< std::vector< int> > m_IndexDAC0;
  std::vector< std::vector< int> > m_IndexHighestDAC;

  std::vector<float> m_thePedestal;    

  std::string m_recoTypeProp; // ( "Parabola", "Shape" or "OF" ) 
  
  enum recoType {PARABOLA,SHAPE,OF};  
  recoType m_recoType;


  //Algorithm-Properties:
  std::vector<std::string> m_keylist;//, m_keylistproperty;
  std::string m_keyoutput;
    
  IntegerProperty m_degree;
  IntegerProperty m_maxADC;
  IntegerProperty m_consADC;
  bool m_dac0sub, m_saveRawRamp,m_saveRecRamp, m_longNtuple, m_satSlope;
  UnsignedIntegerProperty m_minDAC;

  int m_DeadChannelCut;
  std::string m_folderName;
  int m_shapeMethodDAC;
  unsigned int m_DAC0;

  bool m_correctBias;
  bool m_withIntercept;
  float m_delayShift;
  //  hashID     sample
  std::vector<std::vector<short> > m_adc0;

  ToolHandle< ILArBadChannelMasker> m_badChannelMask;
  bool m_doBadChannelMask;

  const LArOnlineID_Base* m_onlineHelper;
  const LArEM_Base_ID* m_emId;

  // Grouping type
  std::string m_groupingType;

  const DataHandle<ILArRinj> m_dd_rinj;
  std::string m_hec_key;
  bool        m_isSC;
  bool        m_ishec;
  bool        m_iterate;

  uint16_t m_fatalFebErrorPattern;

};

#endif

//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/*
 * LArRTMParamExtractor
 *
 * This algorithm reads LArCaliWaveContainers and extract from each of the
 * LArCaliWaves the LArCaliPulseParams and LArDetCellParams parameters 
 * according to the corresponding RTM algorithms.
 *
 * Author:      Marco.Delmastro@cen.ch
 * 
 */

#ifndef LArRTMPARAMEXTRACTOR_H
#define LArRTMPARAMEXTRACTOR_H

#include <string>
#include <vector>
#include <atomic> 

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "tbb/blocked_range.h"
#include "LArRawConditions/LArWFParams.h"
#include "LArCalibUtils/LArWFParamTool.h" 
#include "LArRawConditions/LArCaliWave.h"
#include <memory>

class LArWFParamTool;

class LArRTMParamExtractor : public AthAlgorithm
{

 public:

  LArRTMParamExtractor(const std::string & name, ISvcLocator * pSvcLocator);
  
  ~LArRTMParamExtractor();

  StatusCode initialize();
  StatusCode execute()    {return StatusCode::SUCCESS;}
  StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
  
 private:

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKeySC{this,"ScCablingKey","LArOnOffIdMapSC","SG Key of SC LArOnOffIdMapping object"};

  ToolHandle<LArWFParamTool> m_larWFParamTool{this,"LArWFParamTool","LArWFParamTool"};

  std::vector<std::string> m_keylist;
  bool m_isSC;
  bool m_testmode, m_dumpOmegaScan, m_dumpResOscill;
  std::string m_omegaScanKey, m_resOscillKeyBefore, m_resOscillKeyAfter;
  
  bool m_ignoreDACselection;
  std::vector<int> m_DAC;
  
  bool  m_extractTcal, m_extractFstep, m_extractOmega0, m_extractTaur ;
  float m_defaultTcal, m_defaultFstep, m_defaultOmega0, m_defaultTaur ;
  
  bool m_recoverEmptyDB;
  
  std::string m_suffixRetrievedCaliPulse, m_suffixRetrievedDetCell;
  std::string m_keyRetrievedCaliPulse, m_keyRetrievedDetCell ;
  
  std::string m_suffixExtractedCaliPulse, m_suffixExtractedDetCell;
  std::string m_keyExtractedCaliPulse, m_keyExtractedDetCell ;
  
  // Grouping type
  std::string m_groupingType;

  // FT selection
  std::vector<int> m_FT;
  int m_PosNeg;
  // Slot selection
  std::vector<int> m_Slot;
  // Calib line selection
  bool m_Calibselection;
  int m_Cline;


   //Elements for TBB

  bool m_useTBB;
  mutable std::atomic<unsigned> m_counter{0};

  class helperParams {
  public:
    helperParams(const LArCaliWave* cw, const HWIdentifier id, const unsigned g) : 
      caliWave(cw), chid(id),gain(g) {};

    const LArCaliWave* caliWave;                  //Input object
    std::optional<LArCaliWave> omegaScan;       //optional output object
    std::optional<LArCaliWave> resOscill0;	  //optional output object		
    std::optional<LArCaliWave> resOscill1;      //optional output object
    LArWFParams wfParams;                         //Outut object
    HWIdentifier chid;
    unsigned gain;
    bool success=true;
  };

  class Looper
  { 
  public:
    //Looper() = delete;
    Looper(std::vector<helperParams>* p, const LArOnOffIdMapping* cabling, const LArWFParamTool* t, 
	   MsgStream& ms,std::atomic<unsigned>& cnt ) : 
      m_tbbparams(p), m_cabling(cabling), m_tool(t), 
      m_msg(ms), m_counter(cnt) {};

    void operator() (const tbb::blocked_range<size_t>& r) const;

  private:
    std::vector<helperParams>* m_tbbparams;
    const LArOnOffIdMapping* m_cabling;
    const LArWFParamTool* m_tool;
    MsgStream& m_msg;
    std::atomic<unsigned>& m_counter;
  };



};

#endif

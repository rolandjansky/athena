//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/** 
   * @author D. Prieur prieur@lapp.in2p3.fr
   * 5. 4. 2004
   */

#ifndef LARTCMPHYSWAVEPREDICTOR
#define LARTCMPHYSWAVEPREDICTOR

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "LArRawConditions/LArWFParams.h"
#include "LArRawConditions/LArPhysWave.h"
#include "LArRawConditions/LArCaliWave.h"

#include "LArCalibUtils/LArWFParamTool.h"
#include "LArCalibUtils/LArPhysWaveTool.h" 
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <vector>
#include <string>

class LArTCMPhysWavePredictor : public AthAlgorithm
{
 public:
  LArTCMPhysWavePredictor(const std::string & name, ISvcLocator * pSvcLocator);

  ~LArTCMPhysWavePredictor();

  //standard algorithm methods
  StatusCode initialize() ; 
  StatusCode execute() {return StatusCode::SUCCESS;} //empty method
  StatusCode stop();
  StatusCode finalize(){ATH_CHECK(m_cablingKey.initialize()); return StatusCode::SUCCESS;}
 private:

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this, "OnOffMap", "LArOnOffIdMap", "SG key for mapping object"};

  bool        m_testmode;
  bool	      m_datafromfile; //switch to take data from file/db
  int         m_minuitoutputlevel;
  bool        m_rootrawdump;
  std::string m_rootoutputfile;
  
  bool 		  m_filter_cells;    
  int 		  m_filter_layer;
  
  bool		  m_filter_phi;
  int		  m_filter_phi_min;
  int		  m_filter_phi_max;
  
  bool		  m_filter_eta;
  int		  m_filter_eta_min;
  int		  m_filter_eta_max;
    
  
  std::vector<std::string> m_keylist;
  std::string m_groupingType;
  
};

#endif

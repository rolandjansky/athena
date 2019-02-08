/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ReadCalibDataTestAlg.h Header file for SCT_ReadCalibDataTestAlg.
    @author: Per Johansson, based on Jorgen Dalmau TestReadSCT_CalibData example
    adapted first to test a tool and the a service
*/

// Multiple inclusion protection
#ifndef SCT_READ_CALIB_CHIP_DATA_TEST_ALG
#define SCT_READ_CALIB_CHIP_DATA_TEST_ALG

// Include Athena stuff
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "Identifier/Identifier.h"
#include "SCT_ConditionsTools/ISCT_ReadCalibChipDataTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

#include "GaudiKernel/ToolHandle.h"

// Include STL stuff
#include <string>

// Forward declarations
class SCT_ID;

/** This class acts as a test/sample client to the SCT_ReadSCalibChipDataSvc class.*/
class SCT_ReadCalibChipDataTestAlg : public AthReentrantAlgorithm 
{
 public:
  //----------Public Member Functions----------//
  // Structors
  SCT_ReadCalibChipDataTestAlg(const std::string& name, ISvcLocator* pSvcLocator); //!< Constructor
  virtual ~SCT_ReadCalibChipDataTestAlg() = default; //!< Destructor
  
  // Standard Gaudi functions
  StatusCode initialize() override; //!< Gaudi initialiser
  StatusCode execute(const EventContext& ctx) const override;    //!< Gaudi executer
  StatusCode finalize() override;   //!< Gaudi finaliser
  
 private:
  //----------Private Member Functions----------//
  StatusCode processProperties();             //!< Process jobOpt properties
  
  //----------Private Attributes----------//
  const SCT_ID*                       m_id_sct;        //!< ID helper for SCT
  SG::ReadHandleKey<xAOD::EventInfo>  m_currentEventKey{this, "EventInfoKey", "EventInfo", "EventInfoKey"};
  Identifier                          m_moduleId;      //!< Module identifier
  Identifier                          m_waferId;       //!< Wafer identifier
  Identifier                          m_stripId;       //!< Strip identifier
 
  // Get Tool Handle
  ToolHandle<ISCT_ReadCalibChipDataTool> m_ReadCalibChipDataTool{this, "SCT_ReadCalibChipDataTool", "SCT_ReadCalibChipDataTool", "Tool to retrieve chip calibration information"};

  BooleanProperty m_doTestmyConditionsSummary{this, "DoTestmyConditionsSummary", false, "Test return bool conditions summary?"};
  BooleanProperty m_doTestmyDataSummary{this, "DoTestmyDataSummary", false, "Test return data summary?"};
  IntegerArrayProperty m_moduleOfflinePosition{this, "ModuleOfflinePosition", {}, "Offline pos. as: B-EC,layer-disk,phi,eta"};
};
//---------------------------------------------------------------------- 

#endif // SCT_READ_CALIB_CHIP_DATA_TEST_ALG

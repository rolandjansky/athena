/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ReadCalibDataTestAlg.h Header file for SCT_ReadCalibDataTestAlg.
    @author: Per Johansson, based on Jorgen Dalmau TestReadSCT_CalibData example
    adapted first to test a tool and the a service
*/

// Multiple inclusion protection
#ifndef SCT_READ_CALIB_DATA_TEST_ALG
#define SCT_READ_CALIB_DATA_TEST_ALG

//local and indet stuff
#include "SCT_ConditionsTools/ISCT_ReadCalibDataTool.h"

// Include Athena stuff
#include "AthenaBaseComps/AthAlgorithm.h"
#include "Identifier/Identifier.h"
#include "SCT_Cabling/ISCT_CablingTool.h"

// Include Gaudi stuff
#include "GaudiKernel/ToolHandle.h"

// Include STL stuff
#include <string>

// Forward declarations
class ISvcLocator;
class SCT_ID;

/** This class acts as a test/sample client to the SCT_ReadSCalibDataSvc class.*/
class SCT_ReadCalibDataTestAlg:public AthAlgorithm 
{
 public:
  //----------Public Member Functions----------//
  // Structors
  SCT_ReadCalibDataTestAlg(const std::string& name, ISvcLocator* pSvcLocator); //!< Constructor
  virtual ~SCT_ReadCalibDataTestAlg() = default; //!< Destructor
  
  // Standard Gaudi functions
  StatusCode initialize() override; //!< Gaudi initialiser
  StatusCode execute() override;    //!< Gaudi executer
  StatusCode finalize() override;   //!< Gaudi finaliser
  
 private:
  //----------Private Member Functions----------//
  StatusCode processProperties();             //!< Process jobOpt properties
  
  //----------Private Attributes----------//
  const SCT_ID*                       m_id_sct;        //!< ID helper for SCT
  Identifier                          m_moduleId;      //!< Module identifier
  Identifier                          m_waferId;       //!< Wafer identifier
  Identifier                          m_stripId;       //!< Strip identifier
 
  // Get Tool Handle
  ToolHandle<ISCT_ReadCalibDataTool> m_ReadCalibDataTool{this, "SCT_ReadCalibDataTool", "SCT_ReadCalibDataTool", "Tool to retrieve calibration information"};
  ToolHandle<ISCT_CablingTool>       m_cabling{this, "SCT_CablingTool", "SCT_CablingTool", "Tool to retrieve SCT Cabling"};

  BooleanProperty                  m_doTestmyConditionsSummary;   //!< Test return bool conditions summary?
  BooleanProperty                  m_doTestmyDefectIsGood;        //!< Test isGood for all strips?
  BooleanProperty                  m_doTestmyDefectType;          //!< Test return defect type?
  BooleanProperty                  m_doTestmyDefectsSummary;      //!< Test return module defect summary?
  BooleanProperty                  m_doTestmyDefectList;          //!< Test return defectList?
  IntegerArrayProperty             m_moduleOfflinePosition;       //!< Offline pos. as: B-EC,layer-disk,phi,eta
};
//---------------------------------------------------------------------- 

#endif // SCT_READ_CALIB_DATA_TEST_ALG

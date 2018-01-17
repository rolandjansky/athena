/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ReadCalibDataTestAlg.h Header file for SCT_ReadCalibDataTestAlg.
    @author: Per Johansson, based on Jorgen Dalmau TestReadSCT_CalibData example
    adapted first to test a tool and the a service
*/

// Multiple inclusion protection
#ifndef SCT_READ_CALIB_DATA_TEST_ALG
#define SCT_READ_CALIB_DATA_TEST_ALG

//local and indet stuff


// Include Athena stuff
#include "AthenaBaseComps/AthAlgorithm.h"
#include "Identifier/Identifier.h"
#include "GaudiKernel/ServiceHandle.h"

// Include STL stuff
#include <string>

// Forward declarations
class ISvcLocator;
class ISCT_ReadCalibDataSvc;
class SCT_ID;
class ISCT_CablingSvc;

/** This class acts as a test/sample client to the SCT_ReadSCalibDataSvc class.*/
class SCT_ReadCalibDataTestAlg:public AthAlgorithm 
{
 public:
  //----------Public Member Functions----------//
  // Structors
  SCT_ReadCalibDataTestAlg(const std::string& name, ISvcLocator* pSvcLocator); //!< Constructor
  ~SCT_ReadCalibDataTestAlg();                                                 //!< Destructor
  
  // Standard Gaudi functions
  StatusCode initialize(); //!< Gaudi initialiser
  StatusCode execute();    //!< Gaudi executer
  StatusCode finalize();   //!< Gaudi finaliser
  
 private:
  //----------Private Member Functions----------//
  StatusCode processProperties();             //!< Process jobOpt properties
  
  //----------Private Attributes----------//
  StatusCode                          m_sc;            //!< To check return codes
  const SCT_ID*                       m_id_sct;        //!< ID helper for SCT
  Identifier                          m_moduleId;      //!< Module identifier
  Identifier                          m_waferId;       //!< Wafer identifier
  Identifier                          m_stripId;       //!< Strip identifier
 
  // Get Service Handle
  ServiceHandle<ISCT_ReadCalibDataSvc> m_ReadCalibDataSvc;
  ServiceHandle<ISCT_CablingSvc>       m_cabling;           //!< Handle to SCT cabling service

  BooleanProperty                  m_doTestmyConditionsSummary;   //!< Test return bool conditions summary?
  BooleanProperty                  m_doTestmyDefectIsGood;        //!< Test isGood for all strips?
  BooleanProperty                  m_doTestmyDefectType;          //!< Test return defect type?
  BooleanProperty                  m_doTestmyDefectsSummary;      //!< Test return module defect summary?
  BooleanProperty                  m_doTestmyDefectList;          //!< Test return defectList?
  IntegerArrayProperty             m_moduleOfflinePosition;       //!< Offline pos. as: B-EC,layer-disk,phi,eta
};
//---------------------------------------------------------------------- 

#endif // SCT_READ_CALIB_DATA_TEST_ALG

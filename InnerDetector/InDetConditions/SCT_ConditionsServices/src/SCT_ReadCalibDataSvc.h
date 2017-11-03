/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ReadCalibDataTool.h Header file for SCT_ReadCalibDataSvc.
    @author Per Johansson, 17/05/08, based on ReadSCT_CalibData.
*/

// Multiple inclusion protection
#ifndef SCT_READ_CALIB_DATA_SVC
#define SCT_READ_CALIB_DATA_SVC

// Include interface class
#include "SCT_ConditionsServices/ISCT_ReadCalibDataSvc.h"

// Include SCT calibration data map objects
//needed for typedef
#include "SCT_ConditionsData/SCT_CalibDefectData.h"

// Include top level interface
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"

// Include Athena stuff 
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"
#include "AthenaKernel/IIOVDbSvc.h" 

// Include boost stuff
#include "boost/array.hpp"

// Forward declarations
class CondAttrListCollection;
class ISvcLocator;
class StoreGateSvc;
class ISCT_CablingSvc;
class SCT_ID;
namespace InDetDD{ class SCT_DetectorManager; }

/** This class contains a Service that reads SCT calibration data and makes it available to 
    other algorithms. The current implementation reads the data from a COOL database. 
*/

class SCT_ReadCalibDataSvc: virtual public ISCT_ReadCalibDataSvc, virtual public AthService {

 public:
  enum {STRIPS_PER_WAFER=768, NUMBER_OF_WAFERS=8176};  
  //----------Public Member Functions----------//
  // Structors
  SCT_ReadCalibDataSvc(const std::string& name, ISvcLocator* pSvcLocator); //!< Constructor
  virtual ~SCT_ReadCalibDataSvc();                                           //!< Destructor
  
  // Retrive interface ID
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  static const InterfaceID& interfaceID();
  
  // Standard Gaudi functions
  virtual StatusCode initialize(); //!< Gaudi initialiser
  virtual StatusCode finalize(); //!< Gaudi finaliser
  
  /// @name Methods to be implemented from virtual baseclass methods, when introduced
  ///Return whether this service can report on the hierarchy level (e.g. module, chip...)
  virtual bool canReportAbout(InDetConditions::Hierarchy h);
  ///Summarise the result from the service as good/bad
  virtual bool isGood(const Identifier& elementId,InDetConditions::Hierarchy h=InDetConditions::DEFAULT);
  ///same thing with id hash, introduced by shaun with dummy method for now
  virtual bool isGood(const IdentifierHash& /*hashId*/) { return true; }
  // Fill the data structures
  StatusCode fillData() { return StatusCode::FAILURE; }
  // Fill the data structures from a Callback 
  StatusCode fillData(int& /*i*/, std::list<std::string>& l);
  // Report whether the map was filled
  bool filled() const;
  // Report whether the service can fill its data during the initialize phase
  virtual bool canFillDuringInitialize() { return false ; } //PJ need to know IOV/run#
  //@}
  
  //Define methods
  virtual StatusCode fillCalibDefectData(std::list<std::string>& keys); //!< Callback for retriving defect data
  
  // Methods to return calibration defect type and summary
  virtual SCT_ReadCalibDataSvc::CalibDefectType defectType(const Identifier& stripId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT); //!<Return summary of defect type and values for a strip
  virtual SCT_CalibDefectData::CalibModuleDefects defectsSummary(const Identifier& moduleId, const std::string& scan); //!<Returns module summary of defect  
  virtual std::list<Identifier> defectList(const std::string& defect); //!<Returns module summary of defect  

 private:
  
  //----------Private Attributes----------//
  ServiceHandle<StoreGateSvc>         m_storeGateSvc;      //!< Handle to StoreGate service
  ServiceHandle<StoreGateSvc>         m_detStoreSvc;       //!< Handle to detector store
  ServiceHandle<IIOVDbSvc>            m_IOVDbSvc;          //!< Handle on the IOVDb service
  ServiceHandle<ISCT_CablingSvc>      m_cabling;           //!< Handle to SCT cabling service
  const InDetDD::SCT_DetectorManager* m_SCTdetMgr;         //!< Handle to SCT detector manager
  const CondAttrListCollection*       m_attrListColl;      //!< Pointer to AL collection
  const SCT_ID*                       m_id_sct;            //!< Handle to SCT ID helper
  
  // If data filled correctly
  bool m_dataFilled;  
  // List folders to be read as CondAttrListCollection*
  StringArrayProperty m_atrcollist;
  // Calib defect maps
  SCT_CalibDefectData m_NPGDefects;
  SCT_CalibDefectData m_NODefects;
  BooleanProperty m_printCalibDefectMaps; //!< Print the calib defect maps?
  // Flag to set true to be able to use all methods not just isGood
  bool m_recoOnly;
  // Calib defect arrays
  typedef boost::array<bool, STRIPS_PER_WAFER> WaferIsGoodInfo_t; // [768b]
  typedef boost::array<WaferIsGoodInfo_t, NUMBER_OF_WAFERS> AllWaferIsGoodInfo_t; // [8176][768b]
  AllWaferIsGoodInfo_t m_isGoodAllWafersInfo;
  // Arrays to hold defects to ignore/limits
  std::vector<std::string> m_ignoreDefects;
  std::vector<float> m_ignoreDefectParameters;
  
  // DataHandles for callback
  const DataHandle<CondAttrListCollection> m_CalibDefects_NP;
  const DataHandle<CondAttrListCollection> m_CalibDefects_NO;
  // Key for DataHandle
  std::string m_key;
};

//---------------------------------------------------------------------- 
#endif // SCT_READ_CALIB_DATA_SVC

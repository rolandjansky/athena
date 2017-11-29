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

// Include STL
#include <mutex>

// Include Gaudi classes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ContextSpecificPtr.h"

// Include SCT calibration data map objects
#include "SCT_ConditionsData/SCT_CalibDefectData.h"
#include "SCT_ConditionsData/SCT_WaferGoodStripInfo.h"
#include "SCT_ConditionsData/SCT_AllGoodStripInfo.h"

// Include top level interface
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"

// Include Athena stuff 
#include "AthenaBaseComps/AthService.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

// Forward declarations
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
  enum {STRIPS_PER_WAFER=768};
  //----------Public Member Functions----------//
  // Structors
  SCT_ReadCalibDataSvc(const std::string& name, ISvcLocator* pSvcLocator); //!< Constructor
  virtual ~SCT_ReadCalibDataSvc();                                         //!< Destructor
  
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
  
  // Methods to return calibration defect type and summary
  virtual SCT_ReadCalibDataSvc::CalibDefectType defectType(const Identifier& stripId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT); //!<Return summary of defect type and values for a strip
  virtual SCT_CalibDefectData::CalibModuleDefects defectsSummary(const Identifier& moduleId, const std::string& scan); //!<Returns module summary of defect  
  virtual std::list<Identifier> defectList(const std::string& defect); //!<Returns module summary of defect  

 private:
  // Mutex to protect the contents.
  mutable std::mutex m_mutex;
  // Cache to store events for slots
  mutable std::vector<EventContext::ContextEvt_t> m_cacheGain;
  mutable std::vector<EventContext::ContextEvt_t> m_cacheNoise;
  mutable std::vector<EventContext::ContextEvt_t> m_cacheInfo;
  // Pointers of SCT_CalibDefectData
  mutable Gaudi::Hive::ContextSpecificPtr<const SCT_CalibDefectData> m_condDataGain;
  mutable Gaudi::Hive::ContextSpecificPtr<const SCT_CalibDefectData> m_condDataNoise;
  // Pointer of SCT_AllGoodStripInfo
  mutable Gaudi::Hive::ContextSpecificPtr<const SCT_AllGoodStripInfo> m_condDataInfo;
  // Read Cond Handles
  SG::ReadCondHandleKey<SCT_CalibDefectData> m_condKeyGain;
  SG::ReadCondHandleKey<SCT_CalibDefectData> m_condKeyNoise;
  SG::ReadCondHandleKey<SCT_AllGoodStripInfo> m_condKeyInfo;

  const SCT_CalibDefectData* getCondDataGain(const EventContext& ctx) const;
  const SCT_CalibDefectData* getCondDataNoise(const EventContext& ctx) const;
  const SCT_AllGoodStripInfo* getCondDataInfo(const EventContext& ctx) const;
  
  //----------Private Attributes----------//
  ServiceHandle<StoreGateSvc>         m_detStoreSvc; //!< Handle to detector store
  ServiceHandle<ISCT_CablingSvc>      m_cabling;     //!< Handle to SCT cabling service
  const InDetDD::SCT_DetectorManager* m_SCTdetMgr;   //!< Handle to SCT detector manager
  const SCT_ID*                       m_id_sct;      //!< Handle to SCT ID helper
  
  // Flag to set true to be able to use all methods not just isGood
  bool m_recoOnly;
};

//---------------------------------------------------------------------- 
#endif // SCT_READ_CALIB_DATA_SVC

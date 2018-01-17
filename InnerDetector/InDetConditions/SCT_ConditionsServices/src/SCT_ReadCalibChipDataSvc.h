/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ReadCalibChipDataSvc.h Header file for SCT_ReadCalibChipDataSvc.
    @author Per Johansson, 23/03/09
*/

// Multiple inclusion protection
#ifndef SCT_READ_CALIB_CHIP_DATA_SVC
#define SCT_READ_CALIB_CHIP_DATA_SVC

// Include interface class
#include "SCT_ConditionsServices/ISCT_ReadCalibChipDataSvc.h"

// Include STL
#include <mutex>

// Include Gaudi classes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ContextSpecificPtr.h"

// Include Athena stuff 
#include "AthenaBaseComps/AthService.h"
#include "SCT_ConditionsServices/SCT_ConditionsParameters.h"
#include "SCT_ConditionsData/SCT_GainCalibData.h"
#include "SCT_ConditionsData/SCT_NoiseCalibData.h"

// Forward declarations
class ISvcLocator;
class StoreGateSvc;
class SCT_ID;

/** This class contains a Service that reads SCT calibration data and makes it available to 
    other algorithms. The current implementation reads the data from a COOL database. 
*/

class SCT_ReadCalibChipDataSvc: virtual public ISCT_ReadCalibChipDataSvc, virtual public AthService {

 public:
  //----------Public Member Functions----------//
  // Structors
  SCT_ReadCalibChipDataSvc(const std::string& name, ISvcLocator* pSvcLocator); //!< Constructor
  virtual ~SCT_ReadCalibChipDataSvc();                                           //!< Destructor
  
  // Retrive interface ID
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  static const InterfaceID& interfaceID();
  
  // Standard Gaudi functions
  virtual StatusCode initialize(); //!< Gaudi initialiser
  virtual StatusCode finalize();   //!< Gaudi finaliser
  
  /// @name Methods to be implemented from virtual baseclass methods, when introduced
  ///Return whether this service can report on the hierarchy level (e.g. module, chip...)
  virtual bool canReportAbout(InDetConditions::Hierarchy h);
  ///Summarise the result from the service as good/bad
  virtual bool isGood(const Identifier& elementId,InDetConditions::Hierarchy h=InDetConditions::DEFAULT);
  ///same thing with id hash, introduced by shaun with dummy method for now
  virtual bool isGood(const IdentifierHash& hashId);
  // Fill the data structures
  StatusCode fillData() { return StatusCode::FAILURE; }
  // Fill the data structures from a Callback 
  StatusCode fillData(int& /*i*/, std::list<std::string>& l);
  /// Report whether the map was filled
  bool filled() const ;
  // Report whether the service can fill its data during the initialize phase
  virtual bool canFillDuringInitialize() { return false; } //PJ need to know IOV/run#
  //@}
  
  // Methods to return calibration data
  virtual std::vector<float> getNPtGainData(const Identifier& moduleId, const int side, const std::string& datatype); //!<Get NPtGain data per wafer
  virtual std::vector<float> getNoiseOccupancyData(const Identifier& moduleId, const int side, const std::string& datatype); //!<Get NoiseOccupancy data wafer

 private:
  // Private enums
  enum FolderType {NPTGAIN, NOISEOCC, UNKNOWN_FOLDER, N_FOLDERTYPES};

  // Private methods
  int nPtGainIndex(const std::string& dataName) const;
  int noiseOccIndex(const std::string& dataName) const;

  const SCT_GainCalibData* getCondDataGain(const EventContext& ctx) const;
  const SCT_NoiseCalibData* getCondDataNoise(const EventContext& ctx) const;

  //----------Private Attributes----------//
  ServiceHandle<StoreGateSvc>         m_detStoreSvc;       //!< Handle to detector store
  const SCT_ID*                       m_id_sct;            //!< Handle to SCT ID helper
  
  // Mutex to protect the contents.
  mutable std::mutex m_mutex;
  // Cache to store events for slots
  mutable std::vector<EventContext::ContextEvt_t> m_cacheGain;
  mutable std::vector<EventContext::ContextEvt_t> m_cacheNoise;
  // Pointers of SCT_GainCalibData and SCT_NoiseCalibData
  mutable Gaudi::Hive::ContextSpecificPtr<const SCT_GainCalibData> m_condDataGain;
  mutable Gaudi::Hive::ContextSpecificPtr<const SCT_NoiseCalibData> m_condDataNoise;
  // Read Cond Handles
  SG::ReadCondHandleKey<SCT_GainCalibData> m_condKeyGain;
  SG::ReadCondHandleKey<SCT_NoiseCalibData> m_condKeyNoise;

  // Noise level for isGood::Side
  float m_noiseLevel;
};

//---------------------------------------------------------------------- 
#endif // SCT_READ_CALIB_CHIP_DATA_SVC

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ReadCalibChipDataTool.h Header file for SCT_ReadCalibChipDataTool.
    @author Per Johansson, 23/03/09
*/

// Multiple inclusion protection
#ifndef SCT_READ_CALIB_CHIP_DATA_TOOL
#define SCT_READ_CALIB_CHIP_DATA_TOOL

// Include interface class
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsTools/ISCT_ReadCalibChipDataTool.h"

// Include Athena stuff
#include "SCT_ConditionsData/SCT_ConditionsParameters.h"
#include "SCT_ConditionsData/SCT_GainCalibData.h"
#include "SCT_ConditionsData/SCT_NoiseCalibData.h"

// Include Gaudi classes
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ContextSpecificPtr.h"

// Include STL
#include <mutex>

// Forward declarations
class SCT_ID;

/** This class contains a Tool that reads SCT calibration data and makes it available to 
    other algorithms. The current implementation reads the data from a COOL database. 
*/

class SCT_ReadCalibChipDataTool: public extends<AthAlgTool, ISCT_ReadCalibChipDataTool> {

 public:
  //----------Public Member Functions----------//
  // Structors
  SCT_ReadCalibChipDataTool(const std::string& type, const std::string& name, const IInterface* parent); //!< Constructor
  virtual ~SCT_ReadCalibChipDataTool() = default; //!< Destructor

  // Standard Gaudi functions
  virtual StatusCode initialize() override; //!< Gaudi initialiser
  virtual StatusCode finalize() override; //!< Gaudi finaliser

  /// @name Methods to be implemented from virtual baseclass methods, when introduced
  ///Return whether this service can report on the hierarchy level (e.g. module, chip...)
  virtual bool canReportAbout(InDetConditions::Hierarchy h) const override;
  ///Summarise the result from the service as good/bad
  virtual bool isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  ///same thing with id hash, introduced by shaun with dummy method for now
  virtual bool isGood(const IdentifierHash& hashId, const EventContext& ctx) const override;
  virtual bool isGood(const IdentifierHash& hashId) const override;
  //@}
  
  // Methods to return calibration data
  virtual std::vector<float> getNPtGainData(const Identifier& moduleId, const int side, const std::string& datatype, const EventContext& ctx) const override; //!<Get NPtGain data per wafer
  virtual std::vector<float> getNPtGainData(const Identifier& moduleId, const int side, const std::string& datatype) const override; //!<Get NPtGain data per wafer
  virtual std::vector<float> getNoiseOccupancyData(const Identifier& moduleId, const int side, const std::string& datatype, const EventContext& ctx) const override; //!<Get NoiseOccupancy data wafer
  virtual std::vector<float> getNoiseOccupancyData(const Identifier& moduleId, const int side, const std::string& datatype) const override; //!<Get NoiseOccupancy data wafer

 private:
  // Private enums
  enum FolderType {NPTGAIN, NOISEOCC, UNKNOWN_FOLDER, N_FOLDERTYPES};

  // Private methods
  int nPtGainIndex(const std::string& dataName) const;
  int noiseOccIndex(const std::string& dataName) const;

  const SCT_GainCalibData* getCondDataGain(const EventContext& ctx) const;
  const SCT_NoiseCalibData* getCondDataNoise(const EventContext& ctx) const;

  //----------Private Attributes----------//
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
  SG::ReadCondHandleKey<SCT_GainCalibData> m_condKeyGain{this, "CondKeyGain", "SCT_GainCalibData", "SCT calibration data of gains of chips"};
  SG::ReadCondHandleKey<SCT_NoiseCalibData> m_condKeyNoise{this, "CondKeyNoise", "SCT_NoiseCalibData", "SCT calibration data of noises of chips"};

  // Noise level for isGood::Side
  float m_noiseLevel;
};

//---------------------------------------------------------------------- 
#endif // SCT_READ_CALIB_CHIP_DATA_TOOL

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_CONFIGURATIONCONDALG
#define SCT_CONFIGURATIONCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "AthenaPoolUtilities/CondAttrListVec.h"
#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SCT_Cabling/ISCT_CablingTool.h"
#include "SCT_ConditionsData/SCT_ConfigurationCondData.h"
#include "SCT_ConditionsTools/ISCT_ReadoutTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Forward declarations
class EventIDRange;
class SCT_ID;

class SCT_ConfigurationCondAlg : public AthReentrantAlgorithm 
{  
 public:
  SCT_ConfigurationCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_ConfigurationCondAlg() = default;
  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;

 private:
  /** enum for constants*/
  enum {badLink=255, stripsPerChip=128, lastStrip=767};

  StatusCode fillChannelData(SCT_ConfigurationCondData* writeCdo, EventIDRange& rangeChannel, EventIDRange& rangeMur, EventIDRange& rangeDetEle, const EventContext& ctx) const;
  StatusCode fillModuleData(SCT_ConfigurationCondData* writeCdo, EventIDRange& rangeModule, const EventContext& ctx) const;
  StatusCode fillLinkStatus(SCT_ConfigurationCondData* writeCdo, EventIDRange& rangeMur, const EventContext& ctx) const;
  Identifier getStripId(const unsigned int truncatedSerialNumber, const unsigned int chipNumber, const unsigned int stripNumber,
                        const InDetDD::SiDetectorElementCollection* elements, const EventContext& ctx) const;

  static const std::string s_coolChannelFolderName;
  static const std::string s_coolChannelFolderName2;
  static const std::string s_coolModuleFolderName;
  static const std::string s_coolModuleFolderName2;
  static const std::string s_coolMurFolderName;
  static const std::string s_coolMurFolderName2;

  SG::ReadCondHandleKey<CondAttrListVec> m_readKeyChannel{this, "ReadKeyChannel", "/SCT/DAQ/Configuration/Chip", "Key of input (raw) conditions folder of chips"};
  SG::ReadCondHandleKey<CondAttrListVec> m_readKeyModule{this, "ReadKeyModule", "/SCT/DAQ/Config/Module", "Key of input (raw) conditions folder of modules"};
  SG::ReadCondHandleKey<CondAttrListVec> m_readKeyMur{this, "ReadKeyMur", "/SCT/DAQ/Config/MUR", "Key of input (raw) conditions folder of Murs"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  SG::WriteCondHandleKey<SCT_ConfigurationCondData> m_writeKey{this, "WriteKey", "SCT_ConfigurationCondData", "Key of output (derived) conditions data"};
  ServiceHandle<ICondSvc> m_condSvc;
  ToolHandle<ISCT_CablingTool> m_cablingTool{this, "SCT_CablingTool", "SCT_CablingTool", "Tool to retrieve SCT Cabling"}; //!< Handle on SCT cabling service
  ToolHandle<ISCT_ReadoutTool> m_readoutTool{this, "SCT_ReadoutTool", "SCT_ReadoutTool", "Handle on readout tool"}; //!< Handle on readout tool
  const SCT_ID* m_pHelper; //!< ID helper for SCT
};

#endif // SCT_CONFIGURATIONCONDALG

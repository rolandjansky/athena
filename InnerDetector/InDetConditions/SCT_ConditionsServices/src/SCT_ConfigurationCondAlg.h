/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_CONFIGURATIONCONDALG
#define SCT_CONFIGURATIONCONDALG

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/EventIDRange.h"

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListVec.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "SCT_ConditionsData/SCT_ConfigurationCondData.h"

#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "SCT_ConditionsServices/ISCT_ReadoutTool.h"

// Forward declarations
class ISCT_ConfigurationConditionsSvc;
class SCT_ID;
namespace InDetDD {class SCT_DetectorManager; }

class SCT_ConfigurationCondAlg : public AthAlgorithm 
{  
 public:
  SCT_ConfigurationCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~SCT_ConfigurationCondAlg();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  /** enum for constants*/
  enum {badLink=255, stripsPerChip=128, lastStrip=767};

  StatusCode fillChannelData();
  StatusCode fillModuleData();
  StatusCode fillLinkStatus();
  Identifier getStripId(const unsigned int truncatedSerialNumber, const unsigned int chipNumber, const unsigned int stripNumber) const;

  SCT_ConfigurationCondData* m_writeCdo;
  EventIDRange m_rangeChannel;
  EventIDRange m_rangeModule;
  EventIDRange m_rangeMur;

  static const std::string s_coolChannelFolderName;
  static const std::string s_coolChannelFolderName2;
  static const std::string s_coolModuleFolderName;
  static const std::string s_coolModuleFolderName2;
  static const std::string s_coolMurFolderName;
  static const std::string s_coolMurFolderName2;

  SG::ReadCondHandleKey<CondAttrListVec> m_readKeyChannel;
  SG::ReadCondHandleKey<CondAttrListVec> m_readKeyModule;
  SG::ReadCondHandleKey<CondAttrListVec> m_readKeyMur;
  SG::WriteCondHandleKey<SCT_ConfigurationCondData> m_writeKey;
  ServiceHandle<ICondSvc> m_condSvc;
  ServiceHandle<ISCT_CablingSvc> m_cablingSvc; //!< Handle on SCT cabling service
  ServiceHandle<StoreGateSvc> m_detStore; //!< Handle on the detector store
  const SCT_ID* m_pHelper; //!< ID helper for SCT
  const InDetDD::SCT_DetectorManager* m_pManager; //!< SCT detector manager
  ToolHandle<ISCT_ReadoutTool> m_readoutTool{this, "SCT_ReadoutTool", "SCT_ReadoutTool", "Handle on readout tool"}; //!< Handle on readout tool
};

#endif // SCT_CONFIGURATIONCONDALG

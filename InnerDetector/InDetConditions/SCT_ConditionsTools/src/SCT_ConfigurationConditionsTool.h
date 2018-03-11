/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConfigurationConditionsTool.h
 * header file for service which reads SCT configuration from database
 * @author shaun.roe@cern.ch, gwilliam@mail.cern.ch
**/

#ifndef SCT_ConfigurationConditionsTool_h
#define SCT_ConfigurationConditionsTool_h

// STL includes
#include <string>
#include <mutex>

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ContextSpecificPtr.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"
#include "SCT_ConditionsData/SCT_ConfigurationCondData.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadCondHandleKey.h"

// Forward declarations
class SCT_ID;
namespace InDetDD { class SCT_DetectorManager; }

/**
 * @class SCT_ConfigurationConditionsTool
 * Service which reads SCT_Configuration from the database
 * Deals with bad modules, bad links, strips out of the readout and bad strips
**/

class SCT_ConfigurationConditionsTool: public extends<AthAlgTool, ISCT_ConfigurationConditionsTool> {
public:

  //@name Service methods
  //@{
  SCT_ConfigurationConditionsTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SCT_ConfigurationConditionsTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  //@}
  
  /**Can the service report about the given component? (chip, module...)*/
  virtual bool                          canReportAbout(InDetConditions::Hierarchy h) override;
  
  /**Is the detector element good?*/
  virtual bool                          isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) override;
  
  /**Is it good?, using wafer hash*/
  virtual bool                          isGood(const IdentifierHash& hashId) override;

  /**List of bad modules*/
  virtual const std::set<Identifier>*   badModules() override;
  /**List of bad strips*/
  virtual void                          badStrips(std::set<Identifier>& strips, bool ignoreBadModules=false, bool ignoreBadChips=false) override;
  /**List of bad strips for a given module*/
  virtual void                          badStrips(const Identifier& moduleId, std::set<Identifier>& strips, bool ignoreBadModules=false, bool ignoreBadChips=false) override;
  /**List of bad links*/
  virtual std::pair<bool, bool>         badLinks(const Identifier& moduleId) override;
  /**Bad links for a given module*/
  virtual const std::map<Identifier, std::pair<bool, bool>>* badLinks() override;
  /**List of bad chips*/
  virtual const std::map<Identifier, unsigned int>* badChips() override;
  /**Bad chips for a given module*/
  virtual unsigned int                  badChips(const Identifier& moduleId) const override;
  /** Get the chip number containing a particular strip*/
  int                                   getChip(const Identifier& stripId) const;

private:
  // Mutex to protect the contents.
  mutable std::mutex m_mutex;
  // Cache to store events for slots
  mutable std::vector<EventContext::ContextEvt_t> m_cache;
  // Pointer of SCT_MonitorConditionsCondData
  mutable Gaudi::Hive::ContextSpecificPtr<const SCT_ConfigurationCondData> m_condData;
  SG::ReadCondHandleKey<SCT_ConfigurationCondData> m_condKey;

  const SCT_ID*                         m_pHelper;                  //!< ID helper for SCT
  const InDetDD::SCT_DetectorManager*   m_pManager;                 //!< SCT detector manager
  bool                                  m_checkStripsInsideModules; //!< Do we want to check if a strip is bad because it is inside a bad module

  /** Is a strip within a bad module*/
  bool                                  isStripInBadModule(const Identifier& stripId);
  /** Is a wafer in a bad module*/
  bool                                  isWaferInBadModule(const Identifier& waferId);
  
  /**Is a chip with this Identifier good?*/
  bool isGoodChip(const Identifier& stripId) const;

  /** enum for constants*/
  enum {stripsPerChip=128, lastStrip=767, invalidChipNumber=-1};

  const SCT_ConfigurationCondData* getCondData(const EventContext& ctx) const;
};

#endif // SCT_ConfigurationConditionsTool_h

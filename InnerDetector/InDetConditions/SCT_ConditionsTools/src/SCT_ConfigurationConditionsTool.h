/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConfigurationConditionsTool.h
 * header file for service which reads SCT configuration from database
 * @author shaun.roe@cern.ch, gwilliam@mail.cern.ch
**/

#ifndef SCT_ConfigurationConditionsTool_h
#define SCT_ConfigurationConditionsTool_h

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SCT_ConditionsData/SCT_ConfigurationCondData.h"
#include "StoreGate/ReadCondHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ContextSpecificPtr.h"

// STL includes
#include <mutex>
#include <string>

// Forward declarations
class SCT_ID;

/**
 * @class SCT_ConfigurationConditionsTool
 * Tool which reads SCT_Configuration from the database
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
  virtual bool                          canReportAbout(InDetConditions::Hierarchy h) const override;
  
  /**Is the detector element good?*/
  virtual bool                          isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual bool                          isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  
  /**Is it good?, using wafer hash*/
  virtual bool                          isGood(const IdentifierHash& hashId) const override;
  virtual bool                          isGood(const IdentifierHash& hashId, const EventContext& ctx) const override;

  /**List of bad modules*/
  virtual const std::set<Identifier>*   badModules(const EventContext& ctx) const override;
  virtual const std::set<Identifier>*   badModules() const override;
  /**List of bad strips*/
  virtual void                          badStrips(std::set<Identifier>& strips, const EventContext& ctx, bool ignoreBadModules=false, bool ignoreBadChips=false) const override;
  virtual void                          badStrips(std::set<Identifier>& strips, bool ignoreBadModules=false, bool ignoreBadChips=false) const override;
  /**List of bad strips for a given module*/
  virtual void                          badStrips(const Identifier& moduleId, std::set<Identifier>& strips, const EventContext& ctx, bool ignoreBadModules=false, bool ignoreBadChips=false) const override;
  virtual void                          badStrips(const Identifier& moduleId, std::set<Identifier>& strips, bool ignoreBadModules=false, bool ignoreBadChips=false) const override;
  /**List of bad links*/
  virtual std::pair<bool, bool>         badLinks(const IdentifierHash& hash, const EventContext& ctx) const override;
  virtual std::pair<bool, bool>         badLinks(const IdentifierHash& hash) const override;
  /**Bad links for a given module*/
  virtual const std::map<IdentifierHash, std::pair<bool, bool>>* badLinks(const EventContext& ctx) const override;
  virtual const std::map<IdentifierHash, std::pair<bool, bool>>* badLinks() const override;
  /**List of bad chips*/
  virtual const std::map<Identifier, unsigned int>* badChips(const EventContext& ctx) const override;
  virtual const std::map<Identifier, unsigned int>* badChips() const override;
  /**Bad chips for a given module*/
  virtual unsigned int                  badChips(const Identifier& moduleId, const EventContext& ctx) const override;
  virtual unsigned int                  badChips(const Identifier& moduleId) const override;
  /** Get the chip number containing a particular strip*/
  virtual int                           getChip(const Identifier& stripId, const EventContext& ctx) const override;
  virtual int                           getChip(const Identifier& stripId) const override;

 private:
  // Mutex to protect the contents.
  mutable std::mutex m_mutex;
  // Cache to store events for slots
  mutable std::vector<EventContext::ContextEvt_t> m_cache;
  mutable std::vector<EventContext::ContextEvt_t> m_cacheElements;
  // Pointer of SCT_ConfigurationCondData
  mutable Gaudi::Hive::ContextSpecificPtr<const SCT_ConfigurationCondData> m_condData;
  // Pointer of InDetDD::SiDetectorElementCollection
  mutable Gaudi::Hive::ContextSpecificPtr<const InDetDD::SiDetectorElementCollection> m_detectorElements;
  SG::ReadCondHandleKey<SCT_ConfigurationCondData> m_condKey{this, "CondKey", "SCT_ConfigurationCondData", "SCT DAQ configuration"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  const SCT_ID*                         m_pHelper;                  //!< ID helper for SCT
  bool                                  m_checkStripsInsideModules; //!< Do we want to check if a strip is bad because it is inside a bad module

  /** Is a strip within a bad module*/
  bool                                  isStripInBadModule(const Identifier& stripId, const EventContext& ctx) const;
  /** Is a wafer in a bad module*/
  bool                                  isWaferInBadModule(const Identifier& waferId, const EventContext& ctx) const;
  
  /**Is a chip with this Identifier good?*/
  bool isGoodChip(const Identifier& stripId, const EventContext& ctx) const;

  /** enum for constants*/
  enum {stripsPerChip=128, lastStrip=767, invalidChipNumber=-1};

  const SCT_ConfigurationCondData* getCondData(const EventContext& ctx) const;
  const InDetDD::SiDetectorElement* getDetectorElement(const IdentifierHash& waferHash, const EventContext& ctx) const;
};

#endif // SCT_ConfigurationConditionsTool_h

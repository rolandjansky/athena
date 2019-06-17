// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ByteStreamErrorsTool.h
 * header file for tool that keeps track of errors in the bytestream.
 * @author Susumu.Oda@cern.ch
**/

#ifndef SCT_ByteStreamErrorsTool_h
#define SCT_ByteStreamErrorsTool_h

///Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsTools/ISCT_ByteStreamErrorsTool.h"

#include "AthenaKernel/SlotSpecificObj.h"
#include "Identifier/IdContext.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetByteStreamErrors/InDetBSErrContainer.h"
#include "InDetByteStreamErrors/SCT_ByteStreamFractionContainer.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

///Gaudi includes
#include "GaudiKernel/EventContext.h"

///STL includes
#include <array>
#include <atomic>
#include <map>
#include <mutex>
#include <vector>

/** forward declarations */
class SCT_ID;

/**
 * @class SCT_ByteStreamErrorsTool
 * Tool that keeps track of modules that give rise to errors in the bytestram.
**/

class SCT_ByteStreamErrorsTool: public extends<AthAlgTool, ISCT_ByteStreamErrorsTool> {

public:
  //@name Tool methods
  //@{
  SCT_ByteStreamErrorsTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SCT_ByteStreamErrorsTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  //@}
  
  virtual bool canReportAbout(InDetConditions::Hierarchy h) const override;
  
  ///Is the detector element good?
  virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual bool isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual bool isGood(const IdentifierHash& elementIdHash) const override;
  virtual bool isGood(const IdentifierHash& elementIdHash, const EventContext& ctx) const override;
  
  const std::set<IdentifierHash>* getErrorSet(int errorType, const EventContext& ctx) const override; // Used by SCTRawDataProviderTool and others
  const std::set<IdentifierHash>* getErrorSet(int errorType) const override; // Used by SCTRawDataProviderTool and others

  const std::array<std::set<IdentifierHash>, SCT_ByteStreamErrors::NUM_ERROR_TYPES>* getErrorSets(const EventContext& ctx) const override; // Used by SCTRawDataProviderTool and others
  const std::array<std::set<IdentifierHash>, SCT_ByteStreamErrors::NUM_ERROR_TYPES>* getErrorSets() const override; // Used by SCTRawDataProviderTool and others

  virtual unsigned int tempMaskedChips(const Identifier& moduleId, const EventContext& ctx) const override; // Internally used
  virtual unsigned int tempMaskedChips(const Identifier& moduleId) const override;
  virtual unsigned int abcdErrorChips(const Identifier& moduleId) const override; // Internally used
  virtual unsigned int abcdErrorChips(const Identifier& moduleId, const EventContext& ctx) const override; // Internally used
  virtual bool isRODSimulatedData(const EventContext& ctx) const override; // Internally used
  virtual bool isRODSimulatedData() const override; // Internally used
  virtual bool isRODSimulatedData(const IdentifierHash& elementIdHash, const EventContext& ctx,
                                  const std::set<IdentifierHash>* errorSet=nullptr) const override;
  virtual bool isRODSimulatedData(const IdentifierHash& elementIdHash) const override;
  virtual bool isHVOn(const EventContext& ctx) const override; // Not used
  virtual bool isHVOn() const override; // Not used
  virtual bool isCondensedReadout(const EventContext& ctx) const override; // Not used
  virtual bool isCondensedReadout() const override; // Not used

private:

  ToolHandle<ISCT_ConfigurationConditionsTool> m_config{this, "ConfigTool",
      "SCT_ConfigurationConditionsTool/InDetSCT_ConfigurationConditionsTool", "Tool to retrieve SCT Configuration Tool"};
  const SCT_ID* m_sct_id{nullptr};
  IdContext m_cntx_sct;

  SG::ReadHandleKey<InDetBSErrContainer> m_bsErrContainerName{this, "ContainerName", "SCT_ByteStreamErrs", "Key of InDetBSErrContainer for SCT"};
  SG::ReadHandleKey<SCT_ByteStreamFractionContainer> m_bsFracContainerName{this, "FracContainerName", "SCT_ByteStreamFrac", "Key of SCT_ByteStreamFractionContainer"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  BooleanProperty m_checkRODSimulatedData{this, "CheckRODSimulatedData", true, "Flag to check RODSimulatedData flag."};

  // Mutex to protect the contents.
  mutable std::recursive_mutex m_mutex{};
  struct CacheEntry {
    EventContext::ContextEvt_t m_evt{EventContext::INVALID_CONTEXT_EVT};
    std::array<std::set<IdentifierHash>, SCT_ByteStreamErrors::NUM_ERROR_TYPES> m_bsErrors; // Used by getErrorSet, addError, resetSets
    std::map<Identifier, unsigned int> m_tempMaskedChips;
    std::map<Identifier, unsigned int> m_abcdErrorChips;
  };
  mutable SG::SlotSpecificObj<CacheEntry> m_cache ATLAS_THREAD_SAFE; // Guarded by m_mutex

  mutable std::atomic_uint m_nRetrievalFailure{0};

  StatusCode fillData(const EventContext& ctx) const;

  void addError(const IdentifierHash& id, int errorType, const EventContext& ctx) const;
  void resetSets(const EventContext& ctx) const;

  bool isGoodChip(const Identifier& stripId, const EventContext& ctx) const;
  int getChip(const Identifier& stripId, const EventContext& ctx) const;

  // For isRODSimulatedData, HVisOn and isCondensedReadout
  const SCT_ByteStreamFractionContainer* getFracData(const EventContext& ctx) const;
  const InDetDD::SiDetectorElement* getDetectorElement(const IdentifierHash& waferHash, const EventContext& ctx) const;

  const std::map<Identifier, unsigned int>& getTempMaskedChips(const EventContext& ctx) const;
  const std::map<Identifier, unsigned int>& getAbcdErrorChips(const EventContext& ctx) const;
};

#endif // SCT_ByteStreamErrorsTool_h

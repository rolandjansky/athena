/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ByteStreamErrorsSvc.h
 * header file for service that keeps track of errors in the bytestream.
 * @author nbarlow@cern.ch
**/

#ifndef SCT_ByteStreamErrorsSvc_h
#define SCT_ByteStreamErrorsSvc_h

///STL includes
#include <set>
#include <map>
#include <list>
#include <mutex>

///Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/EventContext.h"

///Athena includes
#include "AthenaBaseComps/AthService.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ByteStreamErrorsSvc.h"
#include "InDetByteStreamErrors/InDetBSErrContainer.h"
#include "InDetByteStreamErrors/SCT_ByteStreamFractionContainer.h"

/** needs to be included here for gcc 4.3 compatibility */
#include "StoreGate/StoreGateSvc.h"

#include "Identifier/IdContext.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

/** Read Handle Key */
#include "StoreGate/ReadHandleKey.h"

/** forward declarations */
template <class TYPE> class SvcFactory;
class ISvcLocator;
class SCT_ID;
class ISCT_ConfigurationConditionsSvc;
namespace InDetDD {
  class SCT_DetectorManager;
}

/**
 * @class SCT_ByteStreamErrorsSvc
 * Service that keeps track of modules that give rise to errors in the bytestram.
**/

class SCT_ByteStreamErrorsSvc: virtual public ISCT_ByteStreamErrorsSvc, virtual public AthService {
  friend class SvcFactory<SCT_ByteStreamErrorsSvc>;
public:
  //@name Service methods
  //@{
  SCT_ByteStreamErrorsSvc( const std::string& name, ISvcLocator* svc);
  virtual ~SCT_ByteStreamErrorsSvc() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  //@}
  
  virtual bool canReportAbout(InDetConditions::Hierarchy h) override;
  
  ///Is the detector element good?
  virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) override;
  virtual bool isGood(const IdentifierHash& elementIdHash) override;
  
  ///Manually get the data in the structure before proceding
  virtual StatusCode fillData() override;
  virtual StatusCode fillData(int&, std::list<std::string>&) override {
    return StatusCode::FAILURE;
  }
  virtual bool filled() const override;
  ///Can the data be filled during the initialize phase?
  virtual bool canFillDuringInitialize() override { return false; }

  const std::set<IdentifierHash>* getErrorSet(int errorType) override; // Used by SCTRawDataProviderTool and others

  virtual unsigned int tempMaskedChips(const Identifier& moduleId) override; // Internally used
  virtual unsigned int abcdErrorChips(const Identifier& moduleId) override; // Internally used
  virtual bool isRODSimulatedData() const override; // Internally used
  virtual bool HVisOn() const override; // Internally used
  virtual bool isCondensedReadout() const override; // Not used

private:

  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<ISCT_ConfigurationConditionsSvc> m_config;
  const SCT_ID* m_sct_id;
  IdContext m_cntx_sct;
  const InDetDD::SCT_DetectorManager* m_pManager; //!< SCT detector manager

  SG::ReadHandleKey<InDetBSErrContainer> m_bsErrContainerName;
  SG::ReadHandleKey<SCT_ByteStreamFractionContainer> m_bsFracContainerName;

  std::vector<std::set<IdentifierHash> > m_bsErrors[SCT_ByteStreamErrors::NUM_ERROR_TYPES]; // Used by getErrorSet, addError, resetSets

  std::vector<std::map<Identifier, unsigned int> > m_tempMaskedChips;
  std::vector<std::map<Identifier, unsigned int> > m_abcdErrorChips;

  // Mutex to protect the contents.
  mutable std::mutex m_mutex;
  // Cache to store events for slots
  mutable std::vector<EventContext::ContextEvt_t> m_cache;

  StatusCode fillData(const EventContext& ctx);

  void addError(IdentifierHash id, int errorType, const EventContext& ctx);
  void resetSets(const EventContext& ctx);

  bool isGoodChip(const Identifier& stripId);
  int getChip(const Identifier& stripId) const;

  // For isRODSimulatedData, HVisOn and isCondensedReadout
  const SCT_ByteStreamFractionContainer* getFracData() const;

  const std::set<IdentifierHash>& getErrorSet(SCT_ByteStreamErrors::errorTypes errorType, const EventContext& ctx);
  const std::map<Identifier, unsigned int>& getTempMaskedChips(const EventContext& ctx);
  const std::map<Identifier, unsigned int>& getAbcdErrorChips(const EventContext& ctx);
};

#endif // SCT_ByteStreamErrorsSvc_h

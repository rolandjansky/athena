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

///Gaudi includes
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
///Athena includes
#include "AthenaBaseComps/AthService.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "SCT_ConditionsServices/ISCT_ByteStreamErrorsSvc.h"
#include "InDetByteStreamErrors/InDetBSErrContainer.h"

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

class SCT_ByteStreamErrorsSvc: virtual public ISCT_ByteStreamErrorsSvc, virtual public IIncidentListener, virtual public AthService {
  friend class SvcFactory<SCT_ByteStreamErrorsSvc>;
public:
  //@name Service methods
  //@{
  SCT_ByteStreamErrorsSvc( const std::string& name, ISvcLocator* svc);
  virtual ~SCT_ByteStreamErrorsSvc() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

  virtual void handle(const Incident& inc) override;
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
  void addError(IdentifierHash id, int errorType) override; // Used by SCT_RodDecoder
  virtual void resetSets() override; // Internally used
  virtual int getNumberOfErrors(int errorType) override; /** for HLT */ // Used by SCT_TrgClusterization and SCT_ClusterCacheTool
  void addErrorCount(int errorType) override; // Used by SCT_RodDecoder
  virtual void resetCounts() override; /** for the counts used by HLT */ // Used internally and by SCT_ClusterCacheTool

  virtual void setFirstTempMaskedChip(const IdentifierHash& hashId, const unsigned int firstTempMaskedChip) override; // Used by SCT_RodDecoder
  virtual unsigned int getFirstTempMaskedChip(const IdentifierHash& hashId) const override; // Internally used
  virtual const std::map<Identifier, unsigned int>* tempMaskedChips() const override {return &m_tempMaskedChips;} // Internally used
  virtual unsigned int tempMaskedChips(const Identifier & moduleId) const override; // Internally used

  virtual unsigned int abcdErrorChips(const Identifier & moduleId) const override; // Internally used

  void setRODSimulatedData() override; // Used by SCT_RodDecoder
  bool isRODSimulatedData() override; // Internally used

  virtual void addRODHVCounter(bool HVisOn) override; // Used by SCT_RodDecoder
  virtual bool HVisOn() override; // Internally used

  virtual bool isCondensedReadout() override; // Not used
  virtual void setCondensedReadout(bool isCondensed) override; // Used by SCT_RodDecoder

private:

  ServiceHandle<StoreGateSvc> m_evtStore;
  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<ISCT_CablingSvc> m_cabling;
  ServiceHandle<ISCT_ConfigurationConditionsSvc> m_config;
  const SCT_ID* m_sct_id;
  IdContext m_cntx_sct;
  const InDetDD::SCT_DetectorManager* m_pManager; //!< SCT detector manager

  bool m_filled;

  bool m_useRXredundancy;

  bool m_lookForSGErrContainer;
  SG::ReadHandleKey<InDetBSErrContainer> m_bsErrContainerName;

  // Used by getErrorSet, addError, resetSets
  std::set<IdentifierHash> m_bsErrors[SCT_ByteStreamErrors::NUM_ERROR_TYPES];
  // Used by getNumberOfErrors, addErrorCount, resetCounts
  int m_numBsErrors[SCT_ByteStreamErrors::NUM_ERROR_TYPES];

  // Used by setFirstTempMaskedChip, getFirstTempMaskedChip, tempMaskedChips
  std::map<IdentifierHash, unsigned int> m_firstTempMaskedChips;
  std::map<Identifier, unsigned int> m_tempMaskedChips;

  // Used by setRODSimulatedData and isRODSimulatedData
  bool m_isRODSimulatedData; // This can be ROD dependent.

  // Used by addRODHVCounter and HVisOn
  int m_numRODsHVon;
  int m_numRODsTotal;

  // Used by isCondensedReadout and setCondensedReadout
  bool m_condensedMode; // This can be ROD dependent.

  bool isGoodChip(const Identifier& stripId) const;
  int getChip(const Identifier& stripId) const;
};

#endif // SCT_ByteStreamErrorsSvc_h

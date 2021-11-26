/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGNAVTOOLS_RUN2TORUN3TRIGNAVCONVERTERV2_H
#define TRIGNAVTOOLS_RUN2TORUN3TRIGNAVCONVERTERV2_H

// Framework includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTrigger/TrigNavigation.h"
#include "TrigConfInterfaces/IHLTConfigSvc.h"
#include "GaudiKernel/IClassIDSvc.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigNavStructure/Types.h"
#include "TrigNavStructure/TypedHolder.h"
#include "TrigNavStructure/TrigNavStructure.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

// STL includes
#include <string>
#include <set>
#include <map>


// class of temporary objects used to integrate informations needed in conversion process
struct ConvProxy {
  ConvProxy(const HLT::TriggerElement* te);

  bool mergeAllowed(const ConvProxy* other) const;
  void merge(ConvProxy* other); // this will change the also the "other" so it knows it has been merged
  const HLT::TriggerElement* te = nullptr;
  std::vector<HLT::te_id_type> teIDs; // post merging will contain IDs of all merged TEs

  std::vector<ConvProxy*> children;
  std::vector<ConvProxy*> parents;
  std::set<HLT::Identifier> runChains;
  std::set<HLT::Identifier> passChains;
  static const uint64_t MissingFEA = 0;
  uint64_t feaHash = MissingFEA;

  std::vector<HLT::TriggerElement::FeatureAccessHelper> features;
  std::vector<HLT::TriggerElement::FeatureAccessHelper> rois;

  TrigCompositeUtils::Decision* l1Node { nullptr };
  TrigCompositeUtils::Decision* imNode; // for checks only
  TrigCompositeUtils::Decision* hNode;
  std::string description() const;
};

using ConvProxySet_t = std::set<ConvProxy*>;
using TEIdToChainsMap_t = std::map<HLT::te_id_type, std::set<HLT::Identifier>>;

/**
 * @class Run2ToRun3TrigNavConverterV2
 * @brief
 **/
class Run2ToRun3TrigNavConverterV2 : public AthReentrantAlgorithm {
public:
  Run2ToRun3TrigNavConverterV2(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~Run2ToRun3TrigNavConverterV2() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;


private:
  // configurable properties & services
  SG::ReadHandleKey<xAOD::TrigNavigation> m_trigNavKey{ this, "TrigNavReadKey", "TrigNavigation" };
  PublicToolHandle<Trig::TrigDecisionTool> m_tdt{ this, "TrigDecisionTool","", "When enabled read navigation from TDT/off by default" };
  ServiceHandle< TrigConf::IHLTConfigSvc > m_configSvc{ this, "TrigConfigSvc", "TrigConf::xAODConfigSvc/xAODConfigSvc", "Trigger configuration service" };
  ServiceHandle<IClassIDSvc> m_clidSvc{ this, "ClassIDSvc", "ClassIDSvc", "Service to translate CLID to class name" };


  Gaudi::Property<bool> m_doSelfValidation{ this, "doSelfValidation", false, "Run consistency checks after stages of conversion (slows down the alg)" };
  Gaudi::Property<bool> m_doCompression{ this, "doCompression", false, "Collapse navigation elements to save ouput space" };
  Gaudi::Property<bool> m_doLinkFeatures{ this, "doLinkFeatures", true, "Add links to objects, setting it false makes sense when running tests" };
  Gaudi::Property<size_t> m_hNodesPerProxyThreshold{ this, "hNodesPerProxyThreshhold", 15, "Limit number of H nodes per TE (if exceeded conversion results in an error)" };
  Gaudi::Property<std::vector<std::string>> m_chainsToSave{ this, "Chains", {}, "If not specified, all chains are handled" };
  Gaudi::Property<std::vector<std::string>> m_collectionsToSave{ this, "Collections", {} };
  Gaudi::Property<std::vector<std::string>> m_roisToSave{ this, "Rois", {} };

  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_trigOutputNavKey{ this, "OutputNavKey", "HLTNav_Summary" };

  StatusCode extractTECtoChainMapping(TEIdToChainsMap_t& allTES, TEIdToChainsMap_t& finalTEs) const;
  mutable std::mutex m_configUpdateMutex;
  TEIdToChainsMap_t m_allTEIdsToChains, m_finalTEIdsToChains;
  

  StatusCode mirrorTEsStructure(ConvProxySet_t&, const HLT::TrigNavStructure& run2NavPtr) const;

  StatusCode associateChainsToProxies(ConvProxySet_t&, const TEIdToChainsMap_t&) const;

  StatusCode cureUnassociatedProxies(ConvProxySet_t&) const;

  StatusCode removeUnassociatedProxies(ConvProxySet_t&) const;

  StatusCode doCompression(ConvProxySet_t&) const;

  StatusCode collapseFeaturesProxies( ConvProxySet_t& ) const;

  StatusCode collapseFeaturelessProxies(ConvProxySet_t&) const;

  template<typename MAP>
  StatusCode collapseProxies(ConvProxySet_t&, MAP&) const;


  StatusCode fillRelevantFeatures(ConvProxySet_t& convProxies) const;
  StatusCode fillRelevantRois(ConvProxySet_t& convProxies, const HLT::TrigNavStructure& run2NavPtr) const;

  StatusCode createIMHNodes(ConvProxySet_t&, xAOD::TrigCompositeContainer&, const EventContext&) const;

  StatusCode createL1Nodes(const ConvProxySet_t& convProxies, xAOD::TrigCompositeContainer& decisions, const EventContext& context) const;
  StatusCode createFSNodes(const ConvProxySet_t&, xAOD::TrigCompositeContainer&, const TEIdToChainsMap_t& finalTEs,  const EventContext& context) const;

  StatusCode linkTopNode(xAOD::TrigCompositeContainer&) const;

  StatusCode linkFeaNode(ConvProxySet_t& convProxies, const HLT::TrigNavStructure& navigationDecoderrun2NavPtr, const EventContext& context) const;
  StatusCode linkRoiNode(ConvProxySet_t& convProxies, const HLT::TrigNavStructure& run2NavPtr) const;

  // helpers
  //!< both method skip TrigPassBits
  uint64_t feaToHash(const std::vector<HLT::TriggerElement::FeatureAccessHelper>&) const;
  bool feaEqual(const std::vector<HLT::TriggerElement::FeatureAccessHelper>& a, const std::vector<HLT::TriggerElement::FeatureAccessHelper>& b ) const;

  //!< returns true if this particular feature is to be saved (linked)
  bool feaToSave(const HLT::TriggerElement::FeatureAccessHelper& fea) const;
  
  bool roiToSave(const HLT::TrigNavStructure& navigationDecoder, const HLT::TriggerElement::FeatureAccessHelper& fea) const;

  // self validators
  // they return failure if something is not ok 
  StatusCode allProxiesHaveChain(const ConvProxySet_t&) const;

  StatusCode allProxiesConnected(const ConvProxySet_t&) const;


  StatusCode numberOfHNodesPerProxyNotExcessive(const ConvProxySet_t&) const;

  StatusCode noUnconnectedHNodes(const xAOD::TrigCompositeContainer&) const;

  std::tuple<uint32_t, CLID, std::string> getSgKey(const HLT::TrigNavStructure& navigationDecoder, const HLT::TriggerElement::FeatureAccessHelper& helper) const;
  const std::vector<HLT::TriggerElement::FeatureAccessHelper> getTEfeatures(const HLT::TriggerElement *te_ptr, const HLT::TrigNavStructure& navigationDecoder, bool filterOnCLID=true) const;
  const std::vector<HLT::TriggerElement::FeatureAccessHelper> getTEROIfeatures(const HLT::TriggerElement* te_ptr, const HLT::TrigNavStructure& navigationDecoder) const;

  std::map<CLID, std::set<std::string>> m_collectionsToSaveDecoded;

  std::vector<std::string> m_setRoiName;

  CLID m_roIDescriptorCLID{0};
  CLID m_roIDescriptorCollectionCLID{0};
  CLID m_TrigRingerRingsCLID{0};
  CLID m_TrigRingerRingsContainerCLID{0};
  CLID m_TrigEMClusterCLID{0};
  CLID m_TrigEMClusterContainerCLID{0};
  CLID m_CaloClusterCLID{0};
  CLID m_CaloClusterContainerCLID{0};
  CLID m_TrackParticleContainerCLID{0};
  CLID m_TauTrackContainerCLID{0};

};

#endif // TRIGNAVTOOLS_RUN2TORUN3TRIGNAVCONVERTERV2_H
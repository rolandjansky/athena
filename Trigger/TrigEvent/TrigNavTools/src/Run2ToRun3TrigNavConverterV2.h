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

// STL includes
#include <string>
#include <set>
#include <map>


// class of temporary objects used to integrate informations needed in conversion process
struct ConvProxy {
  ConvProxy(const HLT::TriggerElement* te);

  void merge(ConvProxy* other); // this will change the also the "other" so it knows it has been merged

  const HLT::TriggerElement* te = nullptr;
  std::vector<ConvProxy*> children;
  std::vector<ConvProxy*> parents;
  std::set<HLT::Identifier> runChains;
  std::set<HLT::Identifier> passChains;
  uint64_t feaHash;

  std::vector<TrigCompositeUtils::Decision*> imNodes; // for checks only
  std::vector<TrigCompositeUtils::Decision*> hNodes;
};

using ConvProxySet_t = std::set<ConvProxy*>;
using TEIdToChainsMap_t = std::map<HLT::te_id_type, std::set<HLT::Identifier>>;
using FEAToConvProxySet_t = std::map<uint64_t, ConvProxySet_t>;


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


  Gaudi::Property<bool> m_doSelfValidation{ this, "doSelfValidation", false, "Run consistency checks after stages of conversion (slows down the alg)" };
  Gaudi::Property<bool> m_doCompression{ this, "doCompression", true, "Collapse navigation elements to save ouput space" };
  Gaudi::Property<bool> m_doLinkFeatures{ this, "doLinkFeatures", true, "Add links to objects, setting it false makes sense when running tests" };
  Gaudi::Property<size_t> m_hNodesPerProxyThreshold{ this, "hNodesPerProxyThreshhold", 15, "Limit number of H nodes per TE (if exceeded conversion results in an error)" };
  Gaudi::Property<std::vector<std::string>> m_chainsToSave{ this, "Chains", {}, "If not specified, all chains are handled" };


  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_trigOutputNavKey{ this, "OutputNavKey", "HLTNav_Summary" };



  StatusCode extractTECtoChainMapping(TEIdToChainsMap_t& allTES, TEIdToChainsMap_t& finalTEs) const;

  StatusCode mirrorTEsStructure(ConvProxySet_t&, const EventContext& context) const;

  StatusCode associateChainsToProxies(ConvProxySet_t&, const TEIdToChainsMap_t&) const;

  StatusCode cureUnassociatedProxies(ConvProxySet_t&) const;

  StatusCode removeUnassociatedProxies(ConvProxySet_t&) const;

  StatusCode doCompression(ConvProxySet_t&) const;

  StatusCode fillFEAHashes(ConvProxySet_t&) const;

  StatusCode findSharedFEAHashes(const ConvProxySet_t&, FEAToConvProxySet_t&) const;

  StatusCode collapseConvProxies(ConvProxySet_t&, const FEAToConvProxySet_t&) const;

  StatusCode cureFeaturelessProxies(ConvProxySet_t&) const;

  StatusCode fillRelevantFeatures(ConvProxySet_t&) const;

  StatusCode createIMHNodes(ConvProxySet_t&, xAOD::TrigCompositeContainer&) const;

  StatusCode createFSNodes(const ConvProxySet_t&, xAOD::TrigCompositeContainer&, const TEIdToChainsMap_t& finalTEs) const;

  StatusCode linkTopNode(xAOD::TrigCompositeContainer&) const;

  // helpers
  uint64_t feaToHash(const std::vector<HLT::TriggerElement::FeatureAccessHelper>&) const;

  // self validators
  // they return failure if something is not ok 
  StatusCode allProxiesHaveChain(const ConvProxySet_t&) const;

  StatusCode allProxiesConnected(const ConvProxySet_t&) const;

  StatusCode allFEAHashesAreUnique(const FEAToConvProxySet_t&) const;

  StatusCode numberOfHNodesPerProxyNotExcessive(const ConvProxySet_t&) const;

  StatusCode noUnconnectedHNodes(const xAOD::TrigCompositeContainer&) const;

  //Gaudi::Property<int> m_myInt{this, "MyInt", 0, "An Integer"};
};

#endif // TRIGNAVTOOLS_RUN2TORUN3TRIGNAVCONVERTERV2_H
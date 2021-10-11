/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGNAVTOOLS_RUN2TORUN3TRIGNAVCONVERTER_H
#define TRIGNAVTOOLS_RUN2TORUN3TRIGNAVCONVERTER_H

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
#include "TrigNavStructure/TypedHolder.h"
#include "TrigNavStructure/TrigNavStructure.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

// STL includes
#include <string>
#include <map>
#include <set>
#include <vector>

/**
 * @class Run2ToRun3TrigNavConverter
 * @brief 
 **/
class Run2ToRun3TrigNavConverter : public AthReentrantAlgorithm {
public:
  Run2ToRun3TrigNavConverter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~Run2ToRun3TrigNavConverter() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:

    SG::ReadHandleKey<xAOD::TrigNavigation> m_trigNavKey { this, "TrigNavReadKey", "TrigNavigation"};
    PublicToolHandle<Trig::TrigDecisionTool> m_tdt {this, "TrigDecisionTool","", "When enabled read navigation from TDT/off by default"};

    Gaudi::Property<bool> m_doPrint{ this, "doPrint", false };
    Gaudi::Property<std::vector<std::string>> m_collectionsToSave{ this, "Collections", {} };
    Gaudi::Property<std::vector<std::string>> m_chainsToSave{ this, "Chains", {} };
    Gaudi::Property<std::vector<std::string>> m_roisToSave{ this, "Rois", {} };

    ServiceHandle< TrigConf::IHLTConfigSvc > m_configSvc { this, "TrigConfigSvc", "TrigConf::xAODConfigSvc/xAODConfigSvc", "Trigger configuration service" };
    ServiceHandle<IClassIDSvc> m_clidSvc{ this, "ClassIDSvc", "ClassIDSvc", "Service to translate CLID to class name" };
    SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_trigNavWriteKey { this, "TrigNavWriteKey", "HLTNav_Summary" };
    SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_trigSummaryWriteKey { this, "TrigSummaryWriteKey", "HLTNav_Summary" };

    Gaudi::Property<bool> m_onlyFeaturePriting { this, "onlyFeaturePrinting", false, "When enabled do not do conversion but scan all chains in all events for features attached to related TEs"};
    Gaudi::Property<bool> m_convertAllChains { this, "convertAllChains", false, "When enabled ignore chains list and covert all chains"};
    Gaudi::Property<bool> m_recursiveAlgoSelector { this, "recursiveAlgoSelector", true, "When enabled recursive feature collection searched"};


    std::set<CLID> m_setCLID;
    std::set<std::string> m_setChainName;
    std::vector<std::string> m_setRoiName;

    using DecisionPair = std::pair<TrigCompositeUtils::Decision*,TrigCompositeUtils::Decision*>;
    using DecisionObjMap = std::map<uint32_t,DecisionPair>; // decisionFeature, decision
    using DecisionObjStringMap = std::map<std::string,DecisionPair>; // decisionFeature, decision but key=string
    using TEObjMap = std::map<const HLT::TriggerElement*,DecisionPair>; // decisionFeature, decision
    using FElessObjMap = std::map<TrigCompositeUtils::Decision*,std::pair<TrigCompositeUtils::Decision*,const HLT::TriggerElement*>>; // decisionFeature-H no FE ->, decision,TEptr
    using L1ObjMap = std::map<uint32_t,TrigCompositeUtils::Decision*>; // L1decision
    using TEMap = std::map<const HLT::TriggerElement*, bool>; // map of TEs (true FE, false FE-less)
    using TEDecisionMap = std::map<const HLT::TriggerElement*, std::vector<TrigCompositeUtils::Decision*>>;



    std::tuple<uint32_t,CLID,std::string> getSgKey(const HLT::TrigNavStructure &navigationDecoder, const HLT::TriggerElement::FeatureAccessHelper& helper) const;
    StatusCode addTEfeatures(const HLT::TrigNavStructure &navigationDecoder, const HLT::TriggerElement::FeatureAccessHelper& helper, DecisionPair&  decisionPtr, int idx, DecisionObjMap* om) const;
    StatusCode addTEROIfeatures(const HLT::TrigNavStructure &navigationDecoder, const HLT::TriggerElement::FeatureAccessHelper& helper, TrigCompositeUtils::Decision*&  decisionPtr) const;
    StatusCode addROIfeatures(const HLT::TrigNavStructure &navigationDecoder, const HLT::TriggerElement::FeatureAccessHelper& helper, TrigCompositeUtils::Decision*&  decisionPtr, int idx, L1ObjMap* om) const;
    StatusCode addTRACKfeatures(const HLT::TrigNavStructure &navigationDecoder, const HLT::TriggerElement::FeatureAccessHelper& helper, TrigCompositeUtils::Decision*&  decisionPtr, ElementLink<TrigRoiDescriptorCollection>& rLink) const;
    const std::vector<HLT::TriggerElement::FeatureAccessHelper> getTEfeatures(const HLT::TriggerElement *te_ptr, const HLT::TrigNavStructure &navigationDecoder, bool filterOnCLID=true) const;
    const std::vector<HLT::TriggerElement::FeatureAccessHelper> getTEROIfeatures(const HLT::TriggerElement *te_ptr, const HLT::TrigNavStructure &navigationDecoder) const;
    const std::vector<HLT::TriggerElement::FeatureAccessHelper> getROIfeatures(const HLT::TriggerElement *te_ptr, const HLT::TrigNavStructure &navigationDecoder) const;
    const std::vector<HLT::TriggerElement::FeatureAccessHelper> getTRACKfeatures(const HLT::TriggerElement *te_ptr) const;

    StatusCode insertDecisionToTEMap( TrigCompositeUtils::Decision* d, const HLT::TriggerElement* te, TEDecisionMap& mapToFill) const;


    CLID m_roIDescriptorCLID { 0 };
    CLID m_roIDescriptorCollectionCLID { 0 };
    CLID m_TrigRingerRingsCLID { 0 };
    CLID m_TrigRingerRingsContainerCLID { 0 };
    CLID m_TrigEMClusterCLID { 0 };
    CLID m_TrigEMClusterContainerCLID { 0 };
    CLID m_CaloClusterCLID { 0 };
    CLID m_CaloClusterContainerCLID { 0 };
    CLID m_TrackParticleContainerCLID { 0 };
    CLID m_TrackParticleAuxContainerCLID { 0 };
    CLID m_TauTrackContainerCLID { 0 };
    CLID m_TauTrackAuxContainerCLID { 0 };


    StatusCode printFeatures(const HLT::TrigNavStructure& ) const;
};

#endif // TRIGNAVTOOLS_RUN2TORUN3TRIGNAVCONVERTER_H

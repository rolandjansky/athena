/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGNAVTOOLS_RUN2TORUN3TRIGNAVCONVERTER_H
#define TRIGNAVTOOLS_RUN2TORUN3TRIGNAVCONVERTER_H

// Framework includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTrigger/TrigNavigation.h"
#include "TrigConfInterfaces/IHLTConfigSvc.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigNavStructure/TypedHolder.h"
#include "TrigNavStructure/StandaloneNavigation.h"

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
    Gaudi::Property<bool> m_doPrint{ this, "doPrint", true };
    Gaudi::Property<std::vector<std::string>> m_collectionsToSave{ this, "setConfig", {} };
    ServiceHandle< TrigConf::IHLTConfigSvc > m_configSvc { this, "HLTConfigSvc", "HLTConfigSvc", "HLTConfig Service" };
    ServiceHandle<IClassIDSvc> m_clidSvc{ this, "ClassIDSvc", "ClassIDSvc", "Service to translate CLID to class name" };
    SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_trigNavWriteKey { this, "TrigNavWriteKey", "HLTNav_all" };
    SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_trigSummaryWriteKey { this, "TrigSummaryWriteKey", "HLTNav_Summary" };

    std::set<CLID> m_setCLID;
    StatusCode addTEfeatures(const HLT::StandaloneNavigation &navigationDecoder, HLT::TriggerElement::FeatureAccessHelper helper, TrigCompositeUtils::Decision *decisionPtr, bool kRoI=false) const;
    const std::vector<HLT::TriggerElement::FeatureAccessHelper> vectorTEfeatures(const HLT::TriggerElement *te_ptr) const;

    CLID m_roIDescriptorCLID;
    CLID m_roIDescriptorCollectionCLID;
    const std::vector<HLT::TriggerElement::FeatureAccessHelper> vectorROIfeatures(const HLT::TriggerElement *te_ptr) const;
    
    using TE_Decision_map = std::map<HLT::TriggerElement*, std::vector<TrigCompositeUtils::Decision*>>;
};

#endif // TRIGNAVTOOLS_RUN2TORUN3TRIGNAVCONVERTER_H

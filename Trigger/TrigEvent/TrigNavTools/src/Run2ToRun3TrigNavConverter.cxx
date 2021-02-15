/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "Run2ToRun3TrigNavConverter.h"
//#include "TrigNavStructure/StandaloneNavigation.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigNavStructure/TriggerElement.h"


Run2ToRun3TrigNavConverter::Run2ToRun3TrigNavConverter(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

Run2ToRun3TrigNavConverter::~Run2ToRun3TrigNavConverter()
{
}

StatusCode Run2ToRun3TrigNavConverter::initialize()
{

  ATH_CHECK( m_trigNavKey.initialize() );
  ATH_CHECK( m_trigNavWriteKey.initialize() );
  ATH_CHECK( m_trigSummaryWriteKey.initialize() );
  ATH_CHECK( m_configSvc.retrieve() );
  ATH_CHECK( m_clidSvc.retrieve() );

  // retrievig CLID from names and storing to set
  for (const auto& name : m_collectionsToSave) {
    CLID id {0};
    ATH_CHECK( m_clidSvc->getIDOfTypeName(name, id) );
    ATH_MSG_DEBUG("CLID NAME: " << name << " ID: " << id);
    m_setCLID.insert(id);
  }

  ATH_CHECK( m_clidSvc->getIDOfTypeName("TrigRoiDescriptor",   m_roIDescriptorCLID) );
  ATH_CHECK( m_clidSvc->getIDOfTypeName("TrigRoiDescriptorCollection",   m_roIDescriptorCollectionCLID) );

  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverter::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverter::execute(const EventContext& context) const
{
  SG::ReadHandle navReadHandle(m_trigNavKey, context);

  ATH_CHECK( navReadHandle.isValid() );

  HLT::StandaloneNavigation navDecoder = HLT::StandaloneNavigation();
  
  navDecoder.deserialize( navReadHandle->serialized() );

  SG::WriteHandle< TrigCompositeUtils::DecisionContainer > outputNavigation = TrigCompositeUtils::createAndStore( m_trigNavWriteKey, context );
  auto decisionOutput = outputNavigation.ptr();
  SG::WriteHandle< TrigCompositeUtils::DecisionContainer > outputSummary = TrigCompositeUtils::createAndStore( m_trigSummaryWriteKey, context );
  auto decisionSummary = outputSummary.ptr();
  TrigCompositeUtils::Decision* passRawOutput = TrigCompositeUtils::newDecisionIn( decisionSummary, "HLTPassRaw" );


    if (m_doPrint) {
      std::string dump;
      navDecoder.printASCIIArt(dump);
      ATH_MSG_INFO(dump);
    }

    // example chains used for testing
    const std::string chainName{"HLT_e9_etcut"};
    //const std::string chainName{"HLT_e5_lhvloose_nod0"};
    //const std::string chainName{"HLT_e26_lhtight_nod0"};
    //const std::string chainName{"HLT_e28_lhtight_nod0"};

    std::vector<HLT::TriggerElement*> tes;
    TE_Decision_map mapTEtoDecision; // TE - Decision (xAOD::TrigComposite)   
    TE_Decision_map mapTEtoDecisionActive; // TE Active - Decision (xAOD::TrigComposite)   
    std::vector<TrigCompositeUtils::Decision*> decisionLast; // storing "last" decision in a chain

    std::vector<HLT::TriggerElement::FeatureAccessHelper> featureRoI; // for keeping predecessing TE with RoI


    auto c = m_configSvc->chains().chain(chainName);
    // create HLT Identifier aka decision ID
    HLT::Identifier chainId = HLT::Identifier(chainName);

    TrigCompositeUtils::Decision *passActive = TrigCompositeUtils::newDecisionIn(decisionSummary, "HLTActive");

    ATH_MSG_DEBUG("CHAIN SIZE [signatures]: " << c->signatures().size());
    //for (auto s : c->signatures()) {
    for (auto s_iter = c->signatures().begin(), first_s_iter = s_iter; s_iter != c->signatures().end();  ++s_iter) {
    decisionLast.clear();
      for (auto te : (*s_iter)->outputTEs()) {
        tes.clear();
        navDecoder.getAllOfType(te->id(), tes, false);
        
        TrigCompositeUtils::Decision* lptr = nullptr;

        for (auto teptr : tes) {
          auto vectorTEfeatures_ptr = vectorTEfeatures( teptr );
          lptr = nullptr;
          if ( vectorTEfeatures_ptr.empty() ) {
            auto decision = TrigCompositeUtils::newDecisionIn(decisionOutput);
            mapTEtoDecision[teptr].push_back( decision );  
            if (teptr->getActiveState())
            {
              auto decisionFeature = TrigCompositeUtils::newDecisionIn(decisionOutput);
              TrigCompositeUtils::linkToPrevious(decisionFeature, decision, context);
              mapTEtoDecisionActive[teptr].push_back( decisionFeature );
              TrigCompositeUtils::addDecisionID(chainId, decisionFeature);
              lptr = decisionFeature;
            }
          }

          for ( auto elemFE : vectorTEfeatures_ptr ) {
            auto decision = TrigCompositeUtils::newDecisionIn(decisionOutput);
            mapTEtoDecision[teptr].push_back( decision );  
            if (teptr->getActiveState())
            {
              auto decisionFeature = TrigCompositeUtils::newDecisionIn(decisionOutput);
              TrigCompositeUtils::linkToPrevious(decisionFeature, decision, context);
              mapTEtoDecisionActive[teptr].push_back( decisionFeature );
              ATH_CHECK(addTEfeatures( navDecoder, elemFE, decisionFeature ));
              if (s_iter == first_s_iter)
              {
                for ( const auto& rNodes : HLT::TrigNavStructure::getRoINodes( teptr ) ) {
                  if ( HLT::TrigNavStructure::isRoINode(rNodes) ) {
                    for ( auto featureRoI : vectorROIfeatures( rNodes ) ) { 
                      ATH_CHECK(addTEfeatures( navDecoder, featureRoI, decisionFeature, true ));
                    }
                  }
                }
              }
              TrigCompositeUtils::addDecisionID(chainId, decisionFeature);
              lptr = decisionFeature;
            }
          }
          if (lptr != nullptr) decisionLast.push_back( lptr );

        }
      }
    }
    for (auto last : decisionLast) {
      TrigCompositeUtils::linkToPrevious(passActive, last, context);
      TrigCompositeUtils::decisionIDs(passActive).push_back(chainId);
      TrigCompositeUtils::linkToPrevious(passRawOutput, passActive, context);
    }
    TrigCompositeUtils::decisionIDs( passRawOutput ).push_back(chainId);

    for ( auto &[teptr, decisions] : mapTEtoDecision ) {
      for ( auto prep_ptr : navDecoder.getDirectPredecessors(teptr) ) {
        if ( mapTEtoDecisionActive.find(prep_ptr) != mapTEtoDecisionActive.end() ) {
          for (auto d : decisions) {
            for (auto pd : mapTEtoDecisionActive[prep_ptr]) {
              TrigCompositeUtils::linkToPrevious(d, pd, context);
            }
          }
        }
      }
    }

  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverter::addTEfeatures(const HLT::StandaloneNavigation &navigationDecoder, HLT::TriggerElement::FeatureAccessHelper helper, TrigCompositeUtils::Decision *decisionPtr, bool kRoI) const
{
    std::string sgKeyString = navigationDecoder.label(helper.getCLID(), helper.getIndex().subTypeIndex());
    std::string type_name;
    const CLID saveCLID =  (helper.getCLID() == m_roIDescriptorCLID ? m_roIDescriptorCollectionCLID : helper.getCLID());
    ATH_CHECK(m_clidSvc->getTypeNameOfID(saveCLID, type_name));

    auto sgKey = evtStore()->stringToKey(HLTNavDetails::formatSGkey("HLT", type_name, sgKeyString), saveCLID);
 
    if ( kRoI ) {
      decisionPtr->typelessSetObjectLink(TrigCompositeUtils::roiString(), sgKey, saveCLID, helper.getIndex().objectsBegin());
    } else {
    decisionPtr->typelessSetObjectLink(TrigCompositeUtils::featureString(), sgKey, saveCLID, helper.getIndex().objectsBegin(), helper.getIndex().objectsEnd());
    }  

  return StatusCode::SUCCESS;
}

const std::vector<HLT::TriggerElement::FeatureAccessHelper> Run2ToRun3TrigNavConverter::vectorTEfeatures(const HLT::TriggerElement *te_ptr) const
{
  std::vector<HLT::TriggerElement::FeatureAccessHelper> ptrFAHelper; 
  for (HLT::TriggerElement::FeatureAccessHelper helper : te_ptr->getFeatureAccessHelpers())
  {
       if (m_setCLID.find(helper.getCLID()) == m_setCLID.end()) {
         continue;
       }
      ptrFAHelper.push_back( helper );
  }
  return ptrFAHelper;
}

const std::vector<HLT::TriggerElement::FeatureAccessHelper> Run2ToRun3TrigNavConverter::vectorROIfeatures(const HLT::TriggerElement *te_ptr) const
{
  std::vector<HLT::TriggerElement::FeatureAccessHelper> ptrFAHelper;
  for (HLT::TriggerElement::FeatureAccessHelper helper : te_ptr->getFeatureAccessHelpers())
  {
       if (helper.getCLID() != m_roIDescriptorCLID)  {
         continue;
       }
      ptrFAHelper.push_back( helper );
  }
  return ptrFAHelper;
}



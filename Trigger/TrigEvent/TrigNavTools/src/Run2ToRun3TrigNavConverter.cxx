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
  for (const auto& name : m_setConfig) {
    CLID id {0};
    ATH_CHECK( m_clidSvc->getIDOfTypeName(name, id) );
    ATH_MSG_DEBUG("CLID NAME: " << name << " ID: " << id);
    m_setCLID.insert(id);
  }

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
    const std::string chainName{"HLT_e26_lhtight_nod0"};
    //const std::string chainName{"HLT_e28_lhtight_nod0"};

    std::vector<HLT::TriggerElement*> tes;
    TE_Decision_map mapTEtoDecision; // TE - Decision (xAOD::TrigComposite)   
    TE_Decision_map mapTEtoDecisionActive; // TE Active - Decision (xAOD::TrigComposite)   
    std::vector<TrigCompositeUtils::Decision*> decisionLast; // storing "last" decision in a chain

    std::vector<HLT::TriggerElement::FeatureAccessHelper> featureRoI; // for keeping predecessing TE with RoI


    auto c = m_configSvc->chains().chain(chainName);
    // create HLT Identifier aka decision ID
    HLT::Identifier chainId = HLT::Identifier(chainName);


    TrigCompositeUtils::Decision *passActive = TrigCompositeUtils::newDecisionIn(decisionOutput, "HLTActive");

    ATH_MSG_DEBUG("CHAIN SIZE [signatures]: " << c->signatures().size());
    //for (auto s : c->signatures()) {
    for (auto s_iter = c->signatures().begin(), first_s_iter = s_iter; s_iter != c->signatures().end();  ++s_iter) {
      for (auto te : (*s_iter)->outputTEs()) {
        tes.clear();
        decisionLast.clear();
        navDecoder.getAllOfType(te->id(), tes, false);
        
        for (auto teptr : tes) {

          for ( auto elemFE : vectorTEfeatures( teptr ) ) {
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
                    for ( auto featureRoI : vectorTEfeatures( rNodes ) ) { 
                      ATH_CHECK(addTEfeatures( navDecoder, featureRoI, decisionFeature, true ));
                    }
                  }
                }
              }
              TrigCompositeUtils::addDecisionID(chainId, decisionFeature);
              decisionLast.push_back( decisionFeature );
            }
          }

         
          for ( auto prep_ptr : navDecoder.getDirectPredecessors(teptr) ) {
            if (mapTEtoDecision.find(prep_ptr) != mapTEtoDecision.end())
            {
              for (auto d : mapTEtoDecision[teptr])
              {
                for (auto pd : mapTEtoDecision[prep_ptr])
                {
                  TrigCompositeUtils::linkToPrevious(d, pd, context);
                  ATH_MSG_DEBUG("decision - previous decision");
                }
              }
              for (auto d : mapTEtoDecisionActive[teptr])
              {
                for (auto pd : mapTEtoDecisionActive[prep_ptr])
                {
                  TrigCompositeUtils::linkToPrevious(d, pd, context);
                  ATH_MSG_DEBUG("ACTIVE decision - previous decision");
                }
              }
            }
          }

        }
      }
    }
    for ( auto last: decisionLast ) {
      TrigCompositeUtils::Decision* filter = TrigCompositeUtils::newDecisionIn( decisionSummary, passActive, "F", context );
      TrigCompositeUtils::linkToPrevious(filter, last, context);
      TrigCompositeUtils::decisionIDs(filter).push_back(chainId);
      TrigCompositeUtils::linkToPrevious( passRawOutput, filter, context );
    }
    TrigCompositeUtils::decisionIDs( passRawOutput ).push_back(chainId);


  return StatusCode::SUCCESS;
}

StatusCode Run2ToRun3TrigNavConverter::addTEfeatures(const HLT::StandaloneNavigation &navigationDecoder, HLT::TriggerElement::FeatureAccessHelper helper, TrigCompositeUtils::Decision *decisionPtr, bool kRoI) const
{
    std::string sgKeyString = navigationDecoder.label(helper.getCLID(), helper.getIndex().subTypeIndex());
    std::string type_name;
    ATH_CHECK(m_clidSvc->getTypeNameOfID(helper.getCLID(), type_name));
    ATH_MSG_DEBUG("----------------Attached collection " << sgKeyString << ", " << helper << " NAME: " << type_name);

    ATH_MSG_DEBUG(" SG KEY NAME from formatSGkey: " << HLTNavDetails::formatSGkey("HLT", type_name, sgKeyString));

    auto sgKey = evtStore()->stringToKey(HLTNavDetails::formatSGkey("HLT", type_name, sgKeyString), helper.getCLID());
    const CLID saveCLID =  (helper.getCLID() == 6455 ? 1097199488 : helper.getCLID());

    if ( kRoI ) {
      decisionPtr->typelessSetObjectLink(TrigCompositeUtils::roiString(), sgKey, saveCLID, helper.getIndex().objectsBegin());
    } else {
      decisionPtr->typelessSetObjectLink(TrigCompositeUtils::featureString(), sgKey, helper.getCLID(), helper.getIndex().objectsBegin(), helper.getIndex().objectsEnd());
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
  ATH_MSG_DEBUG(" **************** SIZE OF TCs: " << ptrFAHelper.size());
  return ptrFAHelper;
}

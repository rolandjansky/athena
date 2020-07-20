/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TDTExample.cxx 779433 2016-10-20 15:22:56Z rwhite $
// Updated August 2016 for Trigger for Physics Workshop by rwhite
//
// STL include(s):
#include <cmath>
#include <algorithm>
#include <iomanip>

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"

// TDT include(s):
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/TDTUtilities.h"
// Helper include
#include "TriggerAnalysisHelper.h"
//TrigConf include(s)
#include "TrigConfHLTData/HLTTriggerElement.h"
// Trigger EDM include(s):
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODTrigger/TrigPassBits.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

// Trigger include(s):
#include "TrigDecisionTool/TrigDecisionTool.h"
// Local include(s):
#include "TDTExample.h"

/**
 *  @short Internal class for collecting information about jet RoIs through the levels
 *
 * @author Tomasz Bold     <tomasz.bold@cern.ch>     - UC Irvine, AGH-UST Krakow
 *
 * $Revision: 779433 $
 * $Date: 2016-10-20 17:22:56 +0200 (Thu, 20 Oct 2016) $
 */
class JetInfo {
  
public:
   JetInfo() : l1roi(0), jet(0), efcluster(0) {}
   const TrigRoiDescriptor *l1roi;
   const xAOD::JetContainer *jet;
   const xAOD::CaloClusterContainer *efcluster;  
   std::vector<std::string> passed_chains;
   std::vector<std::string> failed_chains;

}; // class JetInfo


Trig::TDTExample::TDTExample(const std::string &name, ISvcLocator *pSvcLocator)
   : AthAlgorithm( name, pSvcLocator ),
     m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" ),
     m_tah( "Trig::TriggerAnalysisHelper/TriggerAnalysisHelper",this ),
     m_evtNr( 0 ),
     m_all( 0 ), m_allL1( 0 ), m_allHLT( 0 ) {

         declareProperty( "TrigDecisionTool", m_trigDec, "The tool to access TrigDecision" );
         declareProperty( "TriggerList", m_chain_names, "List of triggers to analyze");
}



StatusCode Trig::TDTExample::initialize() {

   // Retrieve the TDT:
   CHECK( m_trigDec.retrieve() );
   m_trigDec->ExperimentalAndExpertMethods()->enable();

   CHECK( m_tah.retrieve() );
   ATH_MSG_INFO( "Initialization successful" );

   // we define such big chain groups relying on the name convention (i.e. all L1 items start their name from L1_ etc)
   // in principle we would not have to do so because TDT defined this default groups, no hurt anyway
   m_all   = m_trigDec->getChainGroup( ".*" );
   m_allL1 = m_trigDec->getChainGroup( "L1_.*" ); 
   m_allHLT = m_trigDec->getChainGroup( "HLT_.*" );

   
   return StatusCode::SUCCESS;
}

StatusCode Trig::TDTExample::execute() {

    ++m_evtNr;
    ATH_MSG_DEBUG("Checking bits collection " );
    const xAOD::TrigPassBits *bits(0);
    if ( evtStore()->retrieve(bits).isSuccess() ) {
        ATH_MSG_DEBUG("Size of the bits collection is: " << bits->size() );
    } else {
        ATH_MSG_DEBUG("The bits collection is missing " );
    }

    REPORT_MESSAGE( MSG::DEBUG ) << "Testing on new event";

    if(m_evtNr==1){
        CHECK( checkLevels() );
        CHECK( printLevels() );
        CHECK( printChainConfiguration() );
    }
    CHECK( checkTriggerDecision() );
    CHECK( printHelpers() );
    CHECK( matchRoIs() );
    CHECK( featuresOfCompositeChain() );
    CHECK( printBGInformation() );
    CHECK( electronsAndFlags() );

    // run jet example
    std::vector< JetInfo > triggerjets;
    CHECK( jetRoItoChains( triggerjets ) );

    return StatusCode::SUCCESS;
}

StatusCode Trig::TDTExample::checkTriggerDecision(){
    // Provide examples for analyzing Trigger Decision Bits
    // Relies on the definition of Condition Masks and Bit Masks
    // See http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Trigger/TrigAnalysis/TrigDecisionTool/TrigDecisionTool/Conditions.h
    // Condition Mask elements are really enums for requesting a certain decision type when calling tdt()->isPassed(chain,condition)
    // Bit Masks are constants that define the position of the passedBits
    // passedBits fully define the trigger decision of a particular chain
    // More details see inside the definition of Chain
    // http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Trigger/TrigAnalysis/TrigDecisionTool/Root/ChainGroup.cxx
    // Expert methods updates the cache object
    ExpertMethods *em = m_trigDec->ExperimentalAndExpertMethods();
    
    for(const auto& chain:m_cfg_chains){
        //  By default, this returns the "Physics condition
        ATH_MSG_INFO(chain << " Passed " << m_trigDec->isPassed(chain));
        ATH_MSG_INFO(chain << " Passed Physics " << m_trigDec->isPassed(chain,TrigDefs::Physics));
        // RequireDecision -- trigger not prescaled
        ATH_MSG_INFO(chain << " Passed RequireDecision " << m_trigDec->isPassed(chain,TrigDefs::requireDecision));
        // Get RAW decision -- any difference from requireDecision
        ATH_MSG_INFO(chain << " Passed RequireDecision | allowResurrected " << 
                m_trigDec->isPassed(chain,TrigDefs::allowResurrectedDecision | TrigDefs::requireDecision));
        //Passes or is passThrough, includes resurrected
        ATH_MSG_INFO(chain << " Passed eventAccepted " << m_trigDec->isPassed(chain,TrigDefs::eventAccepted));

        // Get the bits, this tell us more 
        const unsigned int bits = m_trigDec->isPassedBits(chain);
        bool efprescale=bits & TrigDefs::EF_prescaled;
        bool rerun=bits&TrigDefs::EF_resurrected;
        bool passThrough=bits&TrigDefs::EF_passThrough;
        bool passedRaw=bits&TrigDefs::EF_passedRaw;

        // What about the L1?
        bool tbp = bits&TrigDefs::L1_isPassedBeforePrescale;
        bool tap = bits&TrigDefs::L1_isPassedAfterPrescale;
        bool tav = bits&TrigDefs::L1_isPassedAfterVeto;

        // What is the combined prescale result
        bool l1prescale=tbp && !tap;
        bool prescale=efprescale || l1prescale;


        ATH_MSG_INFO(chain << " Decision Summary: ");
        ATH_MSG_INFO(chain << " Passed " << passedRaw);
        ATH_MSG_INFO(chain << " Rerun " << rerun);
        ATH_MSG_INFO(chain << " Passthrough " << passThrough);
        ATH_MSG_INFO(chain << " HLT Prescale " << efprescale);
        ATH_MSG_INFO(chain << " L1 after prescale " << tav);
        ATH_MSG_INFO(chain << " L1 Prescale " << l1prescale);
        ATH_MSG_INFO(chain << " Prescale " << prescale);
        ATH_MSG_INFO(chain << " ============= ");

        // Create a Chain object from configuration
        const HLT::Chain *aChain=em->getChainDetails(chain);
        if(!aChain) 
            ATH_MSG_WARNING("Cannot create chain object");
        else
            ATH_MSG_INFO("Passed Raw " << aChain->chainPassedRaw() 
                    << " Passed Raw no prescale " << (aChain->chainPassedRaw() && !aChain->isPrescaled())
                    << " Passed physics " << aChain->chainPassed()
                    << " Prescaled " << aChain->isPrescaled()
                    << " Passthrough " << aChain->isPassedThrough());
        
        
        // Two cases where isPassed will return true if the rerun decision is true 
        if( ( bits & TrigDefs::EF_passedRaw ) && ( bits & TrigDefs::EF_resurrected ) ) {
            ATH_MSG_INFO("Trigger fired for event and decision was rerun");
        }
        if( !( bits & TrigDefs::EF_prescaled ) && ( bits & TrigDefs::EF_resurrected ) ) {
            ATH_MSG_INFO("Trigger ran in this event and decision was rerun");
        }
        
        // This case should always be false
        // trigger ran in this event but does fire, rerun decision is true
        // if trigger did not fire first time, why does it fire in rerun?
        if( !( bits & TrigDefs::EF_prescaled) && !( bits & TrigDefs::EF_passedRaw ) && ( bits & TrigDefs::EF_resurrected ) ) {
            ATH_MSG_INFO("Cannot happen, trigger failed to fire but rerun is true");
        }

    
    }
    return StatusCode::SUCCESS;
}


StatusCode Trig::TDTExample::checkLevels() {

   //
   // make some basic check, i.e. we should always have L1 passed if HLT is passed etc...
   //
   if( m_allHLT->isPassed() && (! m_allL1->isPassed()) ){
      REPORT_ERROR( StatusCode::FAILURE ) << "HLT passed w/o L1 passing"; 
      return StatusCode::FAILURE;
   }

   return StatusCode::SUCCESS;
}

StatusCode Trig::TDTExample::printLevels() {

   // print all L1 chains and their decision
   // get List of triggers on first event and set list of chains to analyze
    std::vector<std::string> allL1 =m_allL1->getListOfTriggers();
    std::vector<std::string> allHLT = m_allHLT->getListOfTriggers();
    // Create list of configured chains from input list
    for(const std::string& chain:m_allHLT->getListOfTriggers()){
        if(std::find(m_chain_names.begin(), m_chain_names.end(), chain) != m_chain_names.end()){ 
            ATH_MSG_INFO("Found corresponding chain in list " << chain); 
            m_cfg_chains.push_back(chain);
        }
    }

    ATH_MSG_INFO( "REGTEST L1 Item: " << allL1); 
    ATH_MSG_INFO( "REGTEST HLT Chain: " << allHLT);

    ATH_MSG_INFO( "REGTEST Chains to analyze " << m_cfg_chains);
    ATH_MSG_INFO( "REGTEST L1: " << m_allL1->isPassed() );
    ATH_MSG_INFO( "REGTEST HLT: " << m_allHLT->isPassed() );

    // this creates on the fly soem chain groups and check if they passed
    ATH_MSG_INFO( "REGTEST HLT_mu.*" << m_trigDec->isPassed( "HLT_mu.*" ) );
    ATH_MSG_INFO( "REGTEST HLT_j.*" << m_trigDec->isPassed( "HLT_j.*" ) );
    ATH_MSG_INFO( "REGTEST HLT_e.*" << m_trigDec->isPassed( "HLT_e.*" ) );
    ATH_MSG_INFO( "REGTEST HLT_g.*" << m_trigDec->isPassed( "HLT_g.*" ) );
    ATH_MSG_INFO( "REGTEST HLT_tau.*" << m_trigDec->isPassed( "HLT_tau.*" ) );

   return StatusCode::SUCCESS;
}

StatusCode Trig::TDTExample::printChainConfiguration() {
    
    // Expert TDT method
    // print the trigger chain configuration
    // See TrigConfHLTData/HLTChain.h
    for(const std::string& trigger : m_cfg_chains){

        const auto trig_conf = m_trigDec->ExperimentalAndExpertMethods()->getChainConfigurationDetails(trigger);
        ATH_MSG_INFO("Chain passes " << m_trigDec->isPassed(trigger));
        if(!trig_conf) {
            ATH_MSG_WARNING("Cannot retrieve trigger configuration for " << trigger << " " << typeid(trigger).name());
            continue; 
        }
        
        ATH_MSG_INFO("Chain " << trig_conf->chain_name() 
                << " Counter " << trig_conf->chain_counter()
                << " L1 item " << trig_conf->lower_chain_name()
                << " Level " << trig_conf->level()
                << " Prescale " << trig_conf->prescale() 
                << " PassThrough " << trig_conf->pass_through());

    }
    return StatusCode::SUCCESS;

}

// Following shows some examples of using the helper
// to print Navigation information 
StatusCode Trig::TDTExample::printHelpers()
{
    // Using the Helper, easily change the trigger and object type to examine
    // the same analysis is also done in electronAndFlags
    const std::string elTrigger="HLT_e26_lhtight_nod0_ivarloose";
    
    // The following uses the TDT to retrieve a FeatureContainer and a 
    // Feature w/ class T attached and prints information about the container and TriggerElement
    m_tah->getFeatureAndTEInfo<xAOD::ElectronContainer>(elTrigger, TrigDefs::alsoDeactivateTEs);
    // Loops over the objects in a container of class T and prints which objects have been selected
    m_tah->getSelectedObjects<xAOD::ElectronContainer>(elTrigger, TrigDefs::alsoDeactivateTEs);
   
    const std::string combined="HLT_e24_vloose_L1EM18VH_3j20noL1";
    // The following loops over the combinations and features of each combination
    // printing info about the features with class T1 and class T2 attached
    // For more examples see featuresOfCompositeChain
    m_tah->getCombinationAndTEInfo<xAOD::ElectronContainer,xAOD::JetContainer>(combined, TrigDefs::alsoDeactivateTEs);

    return StatusCode::SUCCESS;
}
StatusCode Trig::TDTExample::electronsAndFlags() {

    const std::string trigger="HLT_e26_lhtight_nod0_ivarloose";
   ATH_MSG_INFO( trigger << " passed: " << m_trigDec->isPassed( trigger ) );

   // Features can be retrieved even when trigger fails
   // The following condition returns the features for all TriggerElements, irregardless whether they are deactivated (i.e., the hypo fails)
   FeatureContainer f = m_trigDec->features( trigger, TrigDefs::alsoDeactivateTEs );

   // Retrieve a vector of Trigger Element (TE) features, i.e. collect all the TEs that contain ElectronContainers
   std::vector<Feature<xAOD::ElectronContainer> > vec = f.get<xAOD::ElectronContainer>("egamma_Electrons");
   
   // Loop over the features of type ElectornContainer
   for( const Trig::Feature<xAOD::ElectronContainer>& feat : vec ) {
       // Feature label, is the TriggerElement active w/ this feature?
       // For a given feature, feat.te() gives the TriggerElement
       // From the TE, can retrieve other features in the trigger chain
       
       // Some more advanced information, like the TE name that is stored in TrigConf
       // Trigger Element name is part of the configuration created from the TriggerMenu
       // Helper method from HLTTriggerElement to convert id to string, will match what we find in TM
       std::string label;
       TrigConf::HLTTriggerElement::getLabel (feat.te()->getId(), label );
       ATH_MSG_INFO("... Electron label: " << feat.label() << " Active " 
               << feat.te()->getActiveState() 
               << " Id " << feat.te()->getId() 
               << " label " << label);

       // Get a pointer to the container from the TriggerElement feature
       //
       const xAOD::ElectronContainer *cont=feat.cptr();
       if(!cont) {
           ATH_MSG_WARNING("ElectronContainer is Null");
           continue;
       }
       if (cont->empty()) continue;
       // now we use "ancestor" to get to object preseeding (in processing history) the final refainement of TrigRoiDescriptor 
       
       // Retrieve the PassBits
       // Objects in containers associated to a TriggerElement are marked as passed/failed
       const xAOD::TrigPassBits *bits=(m_trigDec->ancestor<xAOD::TrigPassBits>(feat.te())).cptr();
       if(!bits) 
           ATH_MSG_WARNING("TrigPassBits null");
       
       for(const xAOD::Electron *e : *cont){
           if(!e) {
               ATH_MSG_WARNING("Electron object is null");
               continue;
           }
           // Check whether the electron was selected by the hypothesis
           if(bits) 
               ATH_MSG_INFO("Electron selected " << bits->isPassing(e,cont) 
                   << " eta: " << e->eta() << " phi: " << e->phi() << "et: " << e->e()/cosh(e->eta()));
       }

       // With a TE, retrieve ancestors and activeState (did the hypo pass/fail?)
       // Which step is active?
       // Following example uses the helper
       ATH_MSG_INFO( " L2Calo Active " << m_tah->ancestorPassed<xAOD::TrigEMCluster>(feat.te())
           << " L2Electron Active " << m_tah->ancestorPassed<xAOD::TrigElectronContainer>(feat.te())
           << " EFCalo Active " << m_tah->ancestorPassed<xAOD::CaloClusterContainer>(feat.te()) );

   }

   return StatusCode::SUCCESS;
}


StatusCode Trig::TDTExample::matchRoIs() {

   FeatureContainer f = m_trigDec->features( "HLT_tau35_perf_tracktwo_tau25_perf_tracktwo");
   FeatureContainer::combination_const_iterator cIt;
   // see how many of this combinations we have
   // the combination in this chain (i.e. chain with multiplicity == 1) the combination is actually RoI
   // in case of chain of multiplicty == 2 (say L2_2e10) the combination is actually combination of two RoIs 
   // etc...
   ATH_MSG_INFO( "REGTEST  checking RoIs " << f.getCombinations().size() );

   for( cIt = f.getCombinations().begin(); cIt != f.getCombinations().end(); ++cIt ) {
      ATH_MSG_INFO( "REGTEST  checking specific RoI " );

      // combination can be printed
      ATH_MSG_INFO( *cIt ); 
      //
      // get various RoiDescriptor objects produced by Tau slice algorithms (refainements)
      // I know of lables after looking into the file with checkSG.py
      // or looking into Trigger/TriggerCommon/TriggerJobOpts/python/TriggerEDM.py
      // initialRoI is typically what we are interested in, this is the RoI defined by the RoIBuilder and sent to the HLT
      // additional RoIs are defined at the HLT with improved coordinates measured at the HLT
      std::vector< Feature<TrigRoiDescriptor> > initRois = cIt->get<TrigRoiDescriptor>("initialRoI");
      std::vector< Feature<TrigRoiDescriptor> > caloRois = cIt->get<TrigRoiDescriptor>("secondaryRoI_L2");
      std::vector< Feature<TrigRoiDescriptor> > tauRois  = cIt->get<TrigRoiDescriptor>("TrigJetRec");

      //
      // this is in fact wrapper which knows about:
      // object
      // TE to which it was attached to
      // label with which the object was attached in HLT algorithm (something like SG label)
      //
      // the object may be missing. 
      // This can be bug (i.e. objects not recorded, or real software bug).
      // In this cases you will get messages. 
      //
      // But this also can be result of the natural to HLT early rejection. 
      // In this case it may happen that last refainment is not there because roi was rejected.
      //
      Feature<TrigRoiDescriptor> d;
      if( !initRois.empty() ){
         d = initRois[0];
         ATH_MSG_INFO( "REGTEST init " << *d.cptr() ); // cptr() is returing const pointer stored in the wraper
      }
      if( !caloRois.empty() ){
         d = caloRois[0];
         ATH_MSG_INFO( "REGTEST calo " << *d.cptr() );
      }
      if( !tauRois.empty() ){
         d = tauRois[0];
         ATH_MSG_INFO( "REGTEST tau  " << *d.cptr() );
      }

      // 
      // There is another way of accessing objects in RoIs though "ancestor" method.
      //
      ATH_MSG_INFO( "Using maching" );
      std::vector<Feature<TrigRoiDescriptor> > final = cIt->get<TrigRoiDescriptor>("T2TauFinal");
      if ( ! final.empty() ) {
         d = final[0]; 
         if( !d.empty() ) // that still can be empty needs to be checked
            ATH_MSG_INFO( "REGTEST          tau  " << *d.cptr() );

         //
         // now we use "ancestor" to get to object preseeding (in processing history) the final refainement of TrigRoiDescriptor 
         //
         Feature<TrigRoiDescriptor> di = m_trigDec->ancestor<TrigRoiDescriptor>(d, "initialRoI");
         if( ! di.empty() )
            ATH_MSG_INFO( "REGTEST matching init " << *di.cptr() );

         //
         // with the use of ancestor method we can get to other types of obejcts (including L1)
         // 
         Feature<xAOD::EmTauRoI> emtau = m_trigDec->ancestor<xAOD::EmTauRoI>(d);
         if( ! emtau.empty() )
            ATH_MSG_INFO( "REGTEST matching EmTau roi passed this thresholds: "
                          << emtau.cptr()->thrNames() );
         // match other objects
         ATH_MSG_INFO( "Match other objects" );

         //
         // so we can do with the "get" methods 
         // this is a bit more laborious as size of returned vector has to bec checked too
         //
         std::vector<Feature<xAOD::TauJetContainer> > ttdv = cIt->get<xAOD::TauJetContainer>();
         if ( ! ttdv.empty() ) {
            Feature<xAOD::TauJetContainer> ttd = ttdv[0];
            const xAOD::TauJetContainer *cont=ttd.cptr();
            if (!ttd.empty()) {
               for(const xAOD::TauJet *tau : *cont)
                   ATH_MSG_INFO( "REGTEST TauJet: eta: " << tau->eta()
                           << " phi: " << tau->phi() );
            }
         }
      }
   }

   return StatusCode::SUCCESS;
}

StatusCode Trig::TDTExample::featuresOfCompositeChain() {

    //
   // These are more advanced example of looking into complex chains (multiplicty > 1)
   // Now combination contains 2 TEs but has multiple L1 combinations
   //
   // Use a tau chain that is seeded from L1 Tau (EmTauRoI) and L1 Jet (JetRoi)
   // How to access the EmTauRoi which seeds the HLT and the L1 jets
   // Following item seeded by L1_TAU20IM_2TAU12IM_J25_2J20_3J12
    const std::string chain("HLT_tau35_perf_tracktwo_tau25_perf_tracktwo");

   ATH_MSG_INFO( "REGTEST chain: " << chain << " is passed " << m_trigDec->isPassed( chain ) );

   FeatureContainer f = m_trigDec->features( chain );

   // 
   // Simple approach, no need to know about which object pairs with which.
   // Just need flat list of tau jets.
   //
  
   std::vector<Feature<xAOD::TauJetContainer> > flatjets = f.get<xAOD::TauJetContainer>();
   ATH_MSG_INFO( "REGTEST flat list of Jets has size: " << flatjets.size() );
   std::vector<Feature<xAOD::TauJetContainer> >::const_iterator fIt;
   for( fIt = flatjets.begin(); fIt < flatjets.end(); ++fIt ) {
       const xAOD::TauJetContainer *taus=fIt->cptr();

       for(const xAOD::TauJet *tau:*taus)
           ATH_MSG_INFO( "REGTEST TauJet e:" << tau->e() );
      //
      // This objects are back navigable
      //
      Feature<xAOD::JetRoI> jRoI =  m_trigDec->ancestor<xAOD::JetRoI>(*fIt);
      if ( !jRoI.empty() ) {
         ATH_MSG_INFO( "REGTEST  got l1 jet: " << jRoI.cptr()->thrNames() ); 
      } 
      Feature<xAOD::EmTauRoI> emRoI =  m_trigDec->ancestor<xAOD::EmTauRoI>(*fIt);
      if ( !jRoI.empty() ) {
         ATH_MSG_INFO( "REGTEST  got l1 emtau: " << emRoI.cptr()->thrNames() ); 
      } 
   }

   std::vector<Combination>::const_iterator i;
   for( i = f.getCombinations().begin(); i != f.getCombinations().end(); ++i ) {
      //
      // we can get flat vectors from objects like this
      //
       const std::vector< Trig::Feature<xAOD::TauJetContainer> >  taus = i->get<xAOD::TauJetContainer>("TrigTauRecPreselection");
       ATH_MSG_INFO( "REGTEST combination flat list of TauJets has size: " << taus.size() );
       //
      // Get the tau(s) container 
      // Always 2? Each combination should have at least 2 TauJet RoIs?
      //
      const xAOD::TauJetContainer* tau(0);
      if( taus.size() == 2 ) {
         tau = taus[0];  // this is realy useless here but shows that w/o call to cptr() implicit conversions happen 
      } else {
         ATH_MSG_WARNING("Our expectations were wrong");
         
      }

      //
      // Get RoI for it with te use of "ancestor".
      //
      Feature<TrigRoiDescriptor> initialRoI = 
         m_trigDec->ancestor< TrigRoiDescriptor >( taus[ 0 ], "initialRoI" );
      if( ! initialRoI.empty() ) {
         ATH_MSG_INFO( "REGTEST from chain: " << chain << " got tau: " << tau
                       << " and initialRoI: " << *initialRoI.cptr() ); 
      }

      //
      // Through ancestor we can also get lvl1 rois objects.
      //
      Feature<xAOD::JetRoI> j1L1RoI =  m_trigDec->ancestor<xAOD::JetRoI>(taus[0]);
      if ( !j1L1RoI.empty() ) {
         ATH_MSG_INFO( "REGTEST from chain: " << chain << " got l1 jet: "
                       << j1L1RoI.cptr()->thrNames() ); 
      }

   }

   // Second example
   
   const std::string chain2="HLT_e24_vloose_L1EM18VH_3j20noL1";
   ATH_MSG_INFO( "REGTEST chain: " << chain2 << " is passed " << m_trigDec->isPassed( chain2 ) );
   FeatureContainer f2 = m_trigDec->features( chain2 );
   ATH_MSG_INFO(chain2 << " has " << f2.getCombinations().size() << " combinations, " 
           << f2.get<xAOD::ElectronContainer>().size() << " Electron features, "
           << f2.get<xAOD::JetContainer>().size() << " Jet features");
           
           
           
   // Range based loop over Combination container
   std::string label;
   for( const Combination& comb : f2.getCombinations()){ 
       std::vector< Trig::Feature <xAOD::ElectronContainer> > elfeats = comb.get<xAOD::ElectronContainer>();
       ATH_MSG_INFO("Combination has " << elfeats.size() << " Electron features "); 
       
       for(const Trig::Feature<xAOD::ElectronContainer>& els:elfeats){
           const xAOD::ElectronContainer *cont = els.cptr(); 
           TrigConf::HLTTriggerElement::getLabel (els.te()->getId(), label );
           ATH_MSG_INFO("Electron feature: " << els.label() 
                   << " TE Id: "<< els.te()->getId() 
                   << " TE label: " << label 
                   << " has " << els.cptr()->size() << " electrons ");
           for(const xAOD::Electron *e : *cont){
               if(!e) {
                   ATH_MSG_WARNING("Electron object is null");
                   continue;
               }
               ATH_MSG_INFO("Electron " 
                   << " eta: " << e->eta() 
                   << " phi: " << e->phi() << " et: " << e->e()/cosh(e->eta()));
           }
       }
       
       std::vector< Trig::Feature <xAOD::JetContainer> > jetfeats = comb.get<xAOD::JetContainer>();
       ATH_MSG_INFO("Combination has " << jetfeats.size() << " Jet features ");
       for(const Trig::Feature<xAOD::JetContainer>& jets:jetfeats){
           TrigConf::HLTTriggerElement::getLabel (jets.te()->getId(), label );
           ATH_MSG_INFO("Jet feature label " << jets.label() 
                   << " TE Id: " << jets.te()->getId() 
                   << " TE label: " << label
                   << " has " << jets.cptr()->size() << " jets ");
       }
       
   }

   return StatusCode::SUCCESS;
}

StatusCode Trig::TDTExample::printBGInformation() {

   ATH_MSG_INFO( "REGTEST Bunchgroup code: " << ( int ) m_trigDec->getBGCode() );

   return StatusCode::SUCCESS;
}

void add( std::vector< std::string >& dest, const std::vector< std::string >& s ) {
   dest.insert( dest.end(), s.begin(), s.end() );
   return;
}

template<class T>
void set( T*& dest, T* src ) {
   if( ! dest )
      dest = src;
   return;
}

StatusCode  Trig::TDTExample::addChainInfo( std::vector< JetInfo >& jetRoIs, const ChainGroup* cg ) {
  
   Trig::FeatureContainer f = cg->features(TrigDefs::alsoDeactivateTEs);
   for( const Trig::Combination& comb : f.getCombinations() ) {
      for( const HLT::TriggerElement* te : comb.tes() ) {

         // get bunch of objects
         const TrigRoiDescriptor *l1roi = m_trigDec->ancestor<TrigRoiDescriptor>(te, "initialRoI");
         // CaloClusters not always kept
         const xAOD::CaloClusterContainer* efcluster = nullptr;//m_trigDec->ancestor<xAOD::CaloClusterContainer>(te);
         const xAOD::JetContainer *jet = m_trigDec->ancestor<xAOD::JetContainer>(te);
         
         bool alreadypresent = false;
         for( JetInfo& j : jetRoIs ) {
            if ( j.l1roi == l1roi ) {
               // we got new reco path
               alreadypresent = true;
               set( j.l1roi, l1roi );
               set( j.efcluster, efcluster );
               set( j.jet, jet );
               if( comb.active() ) {
                  add( j.passed_chains, cg->getListOfTriggers() );
               } else {
                  add( j.failed_chains, cg->getListOfTriggers() );
               }
            }
         }

         if( ! alreadypresent ) {
            JetInfo j;
            set( j.l1roi, l1roi );
            set( j.efcluster, efcluster );
            set( j.jet, jet );
            if ( comb.active() ) {
               add( j.passed_chains, cg->getListOfTriggers() );
            } else {
               add( j.failed_chains, cg->getListOfTriggers() );
            }
            jetRoIs.push_back( j );
         }
      }
   }

   return StatusCode::SUCCESS;
}

StatusCode Trig::TDTExample::jetRoItoChains( std::vector< JetInfo >& jetRoIs ) {

   // pick all jet chains
   std::string chains_regex( "(HLT)_[1-4]?j[0-9]+" );
   std::vector< std::string > chains = m_trigDec->getChainGroup( chains_regex )->getListOfTriggers();

   for( const std::string& ch : chains ) {
      ATH_MSG_DEBUG( "Iterating over " << ch );
      const ChainGroup *cg = m_trigDec->getChainGroup( ch );
      CHECK( addChainInfo( jetRoIs, cg ) );
   }

   for( const JetInfo& j : jetRoIs ) {
      if( j.l1roi )     ATH_MSG_INFO( "L1 RoI : " << *( j.l1roi ) );
      if( j.efcluster ) ATH_MSG_INFO( "EF Cluster : " << j.efcluster );
      if( j.jet ) ATH_MSG_INFO( "Jet : " << j.jet );
      ATH_MSG_INFO( "Failed : " << j.failed_chains );
      ATH_MSG_INFO( "Passed : " << j.passed_chains );
   }

   return StatusCode::SUCCESS;
}




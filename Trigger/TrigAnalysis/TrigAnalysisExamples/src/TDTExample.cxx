/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TDTExample.cxx 551321 2013-06-16 19:51:49Z stelzer $

// STL include(s):
#include <cmath>
#include <algorithm>
#include <iomanip>

// Boost include(s):
#include <boost/foreach.hpp>

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"

// TDT include(s):
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/TDTUtilities.h"

// Trigger EDM include(s):
#include "TrigParticle/TrigTau.h"
#include "TrigParticle/TrigTauContainer.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauDetailsContainer.h"
#include "tauEvent/Tau1P3PDetails.h"
#include "tauEvent/TauRecDetails.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigSteeringEvent/TrigPassFlags.h"
#include "TrigSteeringEvent/TrigPassFlagsCollection.h"
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigSteeringEvent/TrigPassBitsCollection.h"

#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "egammaEvent/egammaContainer.h"

#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticle/TrigL2Bphys.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEvent/TrigT2JetContainer.h"

// Local include(s):
#include "TDTExample.h"

/**
 *  @short Internal class for collecting information about jet RoIs through the levels
 *
 * @author Tomasz Bold     <tomasz.bold@cern.ch>     - UC Irvine, AGH-UST Krakow
 *
 * $Revision: 551321 $
 * $Date: 2013-06-16 21:51:49 +0200 (Sun, 16 Jun 2013) $
 */
class JetInfo {
  
public:
   JetInfo() : l1roi(0), l2cluster(0), l2roi(0), efcluster(0) {}
   const TrigRoiDescriptor *l1roi;
   const TrigT2Jet         *l2cluster;
   const TrigRoiDescriptor *l2roi;
   const CaloClusterContainer *efcluster;  
   std::vector<std::string> passed_chains;
   std::vector<std::string> failed_chains;

}; // class JetInfo


Trig::TDTExample::TDTExample(const std::string &name, ISvcLocator *pSvcLocator)
   : AthAlgorithm( name, pSvcLocator ),
     m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" ),
     m_all( 0 ), m_allL1( 0 ), m_allL2( 0 ), m_allEF( 0 ) {

   declareProperty( "TrigDecisionTool", m_trigDec, "The tool to access TrigDecision" );
}



StatusCode Trig::TDTExample::initialize() {

   // Retrieve the TDT:
   CHECK( m_trigDec.retrieve() );

   ATH_MSG_INFO( "Initialization successful" );

   // we define such big chain groups relying on the name convention (i.e. all L1 items start their name from L1_ etc)
   // in principle we woudl not have to do so because TDT defined this default groups, no hurt anyway
   m_all   = m_trigDec->getChainGroup( ".*" );
   m_allL1 = m_trigDec->getChainGroup( "L1_.*" ); 
   m_allL2 = m_trigDec->getChainGroup( "L2_.*" );
   m_allEF = m_trigDec->getChainGroup( "EF_.*" );

   return StatusCode::SUCCESS;
}

StatusCode Trig::TDTExample::execute() {



   ATH_MSG_DEBUG("Checking bits collection " );
   const TrigPassBitsCollection *bits(0);
   if ( evtStore()->retrieve(bits, "HLT_passbits").isSuccess() ) {
     ATH_MSG_DEBUG("Size of the bits collection is: " << bits->size() );
   } else {
     ATH_MSG_DEBUG("The bits collection is missing " );
   }

   

      
   REPORT_MESSAGE( MSG::DEBUG ) << "Testing on new event";

   //    CHECK( checkLevels() );
   //    CHECK( printLevels() );
   //    CHECK( matchRoIs() );
   //    CHECK( featuresOfCompositeChain() );
   //    CHECK( printBGInformation() );

   CHECK( electronsAndFlags() );

   // run jet example
   // std::vector< JetInfo > triggerjets;
   //   CHECK( jetRoItoChains( triggerjets ) );



   
   FeatureContainer f = m_trigDec->features("L2_MU4_DiMu_FS");
   //   f->get<>();
   f.get<TrigL2Bphys>();
   




   return StatusCode::SUCCESS;
}

StatusCode Trig::TDTExample::checkLevels() {

   //
   // make some basic check, i.e. we shoudl always have L1 passed if L2 is passed etc...
   //
   if( m_allEF->isPassed() && (! m_allL2->isPassed()) ){
      REPORT_ERROR( StatusCode::FAILURE ) << "EF passed w/o L2 passing";
      return StatusCode::FAILURE;
   }

   if( m_allL2->isPassed() && (! m_allL1->isPassed()) ){
      REPORT_ERROR( StatusCode::FAILURE ) << "L2 passed w/o L1 passing"; 
      return StatusCode::FAILURE;
   }

   return StatusCode::SUCCESS;
}

StatusCode Trig::TDTExample::printLevels() {

   // print all L1 chains and thier decision
   ATH_MSG_INFO( "REGTEST L1 Item: " <<  m_allL1->getListOfTriggers() );
   ATH_MSG_INFO( "REGTEST L2 Chain: " << m_allL2->getListOfTriggers() );
   ATH_MSG_INFO( "REGTEST EF Chain: " << m_allEF->getListOfTriggers() );

   ATH_MSG_INFO( "REGTEST L1: " << m_allL1->isPassed() );
   ATH_MSG_INFO( "REGTEST L2: " << m_allL2->isPassed() );
   ATH_MSG_INFO( "REGTEST EF: " << m_allEF->isPassed() );

   // this creates on the fly soem chain groups and check if they passed
   ATH_MSG_INFO( "REGTEST L2_mu.*" << m_trigDec->isPassed( "L2_mu.*" ) );
   ATH_MSG_INFO( "REGTEST L2_j.*" << m_trigDec->isPassed( "L2_j.*" ) );
   ATH_MSG_INFO( "REGTEST L2_e.*" << m_trigDec->isPassed( "L2_e.*" ) );
   ATH_MSG_INFO( "REGTEST L2_g.*" << m_trigDec->isPassed( "L2_g.*" ) );
   ATH_MSG_INFO( "REGTEST L2_tau.*" << m_trigDec->isPassed( "L2_tau.*" ) );

   return StatusCode::SUCCESS;
}

StatusCode Trig::TDTExample::electronsAndFlags() {

   ATH_MSG_INFO( "EF_e22_loose passed: " << m_trigDec->isPassed( "EF_e22_loose" ) );

   FeatureContainer f = m_trigDec->features( "EF_e22_loose" );

   std::vector<Feature<egamma> > electrons = f.get<egamma>("egamma_Electrons");
   BOOST_FOREACH( const Trig::Feature<egamma>& elec, electrons ) {
      if (elec.empty()) continue;
      const egamma* e = elec;
      ATH_MSG_INFO("... Electron    te-label: " << elec.label() << "eta: " << e->eta() << " phi: " << e->phi() << "et: " << e->et());
   }

   std::vector<Feature<TrigPassFlags> > isEMflags = f.get<TrigPassFlags>("isEM");
   BOOST_FOREACH( const Trig::Feature<TrigPassFlags>& isEM, isEMflags ) {
      if (isEM.empty()) continue;
      const TrigPassFlags* flags = isEM;
      ATH_MSG_INFO("... Flag        te-label: " << isEM.label() << "N-flags: " << flags->size());
      for(unsigned int i=0; i<flags->size(); i++) {
         ATH_MSG_INFO("        " << i << ": isEM = " << HLT::FlagAs<int>(flags->getFlag(i)));
      }
   }

   return StatusCode::SUCCESS;
}


StatusCode Trig::TDTExample::matchRoIs() {

   FeatureContainer f = m_trigDec->features( "L2_tauNoCut" ); // take one chain as example and take it's objects
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
      //
      std::vector< Feature<TrigRoiDescriptor> > initRois = cIt->get<TrigRoiDescriptor>("initialRoI");
      std::vector< Feature<TrigRoiDescriptor> > caloRois = cIt->get<TrigRoiDescriptor>("TrigT2CaloTau");
      std::vector< Feature<TrigRoiDescriptor> > tauRois  = cIt->get<TrigRoiDescriptor>("T2TauFinal");

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
         Feature<EmTau_ROI> emtau = m_trigDec->ancestor<EmTau_ROI>(d);
         if( ! emtau.empty() )
            ATH_MSG_INFO( "REGTEST matching EmTau roi passed this thresholds: "
                          << emtau.cptr()->getThresholdNames() );
         // match other objects
         ATH_MSG_INFO( "Match other objects" );

         //
         // so we can do with the "get" methods 
         // this is a bit more laborious as size of returned vector has to bec checked too
         //
         std::vector<Feature<TrigTau> > ttdv = cIt->get<TrigTau>();
         if ( ! ttdv.empty() ) {
            Feature<TrigTau> ttd = ttdv[0];
            if (!ttd.empty()) {
               const TrigTau* tau = ttd;
               ATH_MSG_INFO( "REGTEST TrigTau: etCalibCluster: " << tau->etCalibCluster()
                             << " nMatchedTracks: " << tau->nMatchedTracks() );
            }
         }
      }
   }

   return StatusCode::SUCCESS;
}

StatusCode Trig::TDTExample::featuresOfCompositeChain() {

   //
   // This is more advanced example of looking into complex chain (multiplicty == 3)
   // Now combination contains 3 TEs
   //
   const std::string chain( "L2_tau16i_2j23" );
  
   ATH_MSG_INFO( "REGTEST chain: " << chain << " is passed " << m_trigDec->isPassed( chain ) );

   FeatureContainer f = m_trigDec->features( chain );

   // 
   // Simple approach, no need to know about which object pairs with which.
   // Just need flat list of jets.
   //
   std::vector<Feature<TrigT2Jet> > flatjets = f.get<TrigT2Jet>();
   ATH_MSG_INFO( "REGTEST flat list of Jets has size: " << flatjets.size() );
   std::vector<Feature<TrigT2Jet> >::const_iterator fIt;
   for( fIt = flatjets.begin(); fIt < flatjets.end(); ++fIt ) {
      ATH_MSG_INFO( "REGTEST Jet e:" << fIt->cptr()->e() );
      //
      // This objects are back navigable
      //
      Feature<Jet_ROI> jRoI =  m_trigDec->ancestor<Jet_ROI>(*fIt);
      if ( !jRoI.empty() ) {
         ATH_MSG_INFO( "REGTEST  got l1 jet: " << jRoI.cptr()->getThresholdNames() ); 
      } 
   }

   std::vector<Combination>::const_iterator i;
   for( i = f.getCombinations().begin(); i != f.getCombinations().end(); ++i ) {
      //
      // we can get flat vectors from objects like this
      //
      std::vector<Feature<TrigTau> > taus = i->get<TrigTau>();
      std::vector<Feature<TrigT2Jet> > jets = i->get<TrigT2Jet>();
      //
      // Get the tau (well we know there is only one but we check)
      //
      const TrigTau* tau(0);
      if( taus.size() == 1 ) {
         tau = taus[0];  // this is realy useless here but shows that w/o call to cptr() implicit conversions happen 
      } else {
         REPORT_ERROR( StatusCode::FAILURE ) << "Our expectations were wrong";
         return StatusCode::FAILURE;
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
      // Check if there is right number of jets.
      //
      if( jets.size() != 2 ) {
         REPORT_ERROR( StatusCode::FAILURE ) << "Our expectations were wrong";
         return StatusCode::FAILURE;
      }

      //
      // Now get the 2 jets and then lvl1 RoIs for them. 
      //
      const TrigT2Jet* jet = jets[0];
      Feature<TrigRoiDescriptor> j1RoI = m_trigDec->ancestor<TrigRoiDescriptor>(jets[0], "initialRoI");
      if( ! j1RoI.empty() ) {
         ATH_MSG_INFO( "REGTEST from chain: " << chain << " got jet: " << jet << " and initialRoI: "
                       << *j1RoI.cptr() ); 
      }
    
      //
      // Through ancestor we can also get lvl1 rois objects.
      //
      Feature<Jet_ROI> j1L1RoI =  m_trigDec->ancestor<Jet_ROI>(jets[0]);
      if ( !j1L1RoI.empty() ) {
         ATH_MSG_INFO( "REGTEST from chain: " << chain << " got l1 jet: "
                       << j1L1RoI.cptr()->getThresholdNames() ); 
      }

      //
      // And the same for second jet.
      //
      jet = jets[1];
      Feature<TrigRoiDescriptor> j2RoI = m_trigDec->ancestor<TrigRoiDescriptor>(jets[1], "initialRoI");
      if( ! initialRoI.empty() ) {
         ATH_MSG_INFO( "REGTEST from chain: " << chain << " got jet: " << jet << " and initialRoI: "
                       << *j2RoI.cptr() ); 
      }

      Feature<Jet_ROI> j2L1RoI =  m_trigDec->ancestor<Jet_ROI>(jets[1]);
      if( ! j2L1RoI.empty() ) {
         ATH_MSG_INFO( "REGTEST from chain: " << chain << " got l1 jet: "
                       << j2L1RoI.cptr()->getThresholdNames() ); 
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
   BOOST_FOREACH( const Trig::Combination& comb, f.getCombinations() ) {
      BOOST_FOREACH( const HLT::TriggerElement* te, comb.tes() ) {

         // get bunch of objects
         const TrigRoiDescriptor *l1roi = m_trigDec->ancestor<TrigRoiDescriptor>(te, "initialRoI");
         const TrigT2Jet *l2cluster = m_trigDec->ancestor<TrigT2Jet>(te);
         const TrigRoiDescriptor *l2roi = m_trigDec->ancestor<TrigRoiDescriptor>(te, "TrigT2CaloJet");
         const CaloClusterContainer* efcluster = m_trigDec->ancestor<CaloClusterContainer>(te);

         bool alreadypresent = false;
         BOOST_FOREACH( JetInfo& j, jetRoIs ) {
            if ( j.l1roi == l1roi ) {
               // we got new reco path
               alreadypresent = true;
               set( j.l1roi, l1roi );
               set( j.l2cluster, l2cluster );
               set( j.l2roi, l2roi );
               set( j.efcluster, efcluster );

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
            set( j.l2cluster, l2cluster );
            set( j.l2roi, l2roi );
            set( j.efcluster, efcluster );

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
   std::string chains_regex( "(L2|EF)_[1-4]?J[0-9]+" );
   std::vector< std::string > chains = m_trigDec->getChainGroup( chains_regex )->getListOfTriggers();

   BOOST_FOREACH( const std::string& ch, chains ) {
      ATH_MSG_INFO( "Iterating over " << ch );
      const ChainGroup *cg = m_trigDec->getChainGroup( ch );
      CHECK( addChainInfo( jetRoIs, cg ) );
   }

   BOOST_FOREACH( const JetInfo& j, jetRoIs ) {
      if( j.l1roi )     ATH_MSG_INFO( "L1 RoI : " << *( j.l1roi ) );
      if( j.l2roi )     ATH_MSG_INFO( "L2 RoI : " << *( j.l2roi ) );
      if( j.l2cluster ) ATH_MSG_INFO( "L2 Cluser : " << j.l2cluster );
      if( j.efcluster ) ATH_MSG_INFO( "EF Cluser : " << j.efcluster );
      ATH_MSG_INFO( "Failed : " << j.failed_chains );
      ATH_MSG_INFO( "Passed : " << j.passed_chains );
   }

   return StatusCode::SUCCESS;
}




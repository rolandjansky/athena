/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** This example is based on TrigDecisionTest by Till, Nicolas, Ricardo.
 Example by Simon George.
 Instructions:
 1) Set up 13.0.20
 2) Check out PhysicsAnalysis/AnalysisCommon/UserAnalysis 
    https://twiki.cern.ch/twiki/bin/view/Atlas/UserAnalysis
    and check you can make it and run an example so it is working
 2a) workaround for bugs: check out and build latest TrigDecision (compilation error in GetPassedFeatures and changes to configuration methods), TrigNavigation (reset problem), TrigDecisionMaker (compatability with TrigDecision), TrigSteering (to get TrigRoiDescriptor::roiWord set)
 3) Copy requirements, TrigAnalysisExample (.h and .cxx); make
 4) find an AOD file & register it:
      e.g. RTT: http://atlas-project-rtt-results.web.cern.ch/atlas-project-rtt-results/rel_6/val/build/i686-slc4-gcc34-opt/offline/TrigAnalysisTest/AthenaTrigAnalysisTestIOTests/testAthenaRDOtoESDAOD/291/AOD.pool.root
      or one made by hand - may be necessary, see below.
      pool_insertFileToCatalog /afs/cern.ch/user/s/sgeorge/scratch1/analysis/run/AOD.pool.root
      FCregisterLFN -p /afs/cern.ch/user/s/sgeorge/scratch1/analysis/run/AOD.pool.root -l myAOD.pool.root
    Tip: run checkFile.py to check the contents of the AOD file. You need to know what you are looking for, especially if it is a bare class, std::vector or another container. 
    Need to create an AOD file with some fixes (pkgs above) w.r.t. 13.0.20:
    athena -c "EvtMax=10;OutputLevel=WARNING;" TrigAnalysisTest/testAthenaRDOtoESDAOD.py 

 5) Modify AnalysisSkeleton_topOptions.py for input AOD file and:
# Trigger example algorithm
TrigAnalysisExample = Algorithm( "TrigAnalysisExample" )
TrigAnalysisExample.OutputLevel = INFO
theApp.TopAlg += [ "TrigAnalysisExample" ]

Reference for TrigDecisionTool:
http://atlas-computing.web.cern.ch/atlas-computing/links/nightlyDevDirectory/AtlasOffline/latest_doxygen/InstallArea/doc//TrigDecision/html/classTrigDec_1_1TrigDecisionTool.html

Coding notes:
templated code => you will get lots of DEBUG messages about unpacking and navigating which you did not write, but appear to be from your algorithm.

KNOWN PROBLEMS
1. LVL2 data not accessible through TrigDecision without HEAD of TrigNavigation.

*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Trigger include(s):
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "JetEvent/JetCollection.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"

// Local include(s):
#include "TrigAnalysisExample.h"

TrigAnalysisExample::TrigAnalysisExample( const std::string& name,
                                          ISvcLocator* pSvcLocator )
   : AthAlgorithm( name, pSvcLocator ),
     m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" ) {

   declareProperty( "TrigDecisionTool", m_trigDec );

}

StatusCode TrigAnalysisExample::initialize(){

   ATH_MSG_INFO( "in intialize()" );

   // Retrieve the TDT:
   CHECK( m_trigDec.retrieve() );

   return StatusCode::SUCCESS;
}


StatusCode TrigAnalysisExample::execute() {

   ATH_MSG_INFO( "in execute()" );

   // Example 1: Obtain overall trigger status for this event.
   // Beware that there may be special low-threshold triggers that make this
   // meaningless. Best to look for a specific signature.
   ATH_MSG_INFO( "Pass state    = " << m_trigDec->isPassed( "EF_.*" ) );
   ATH_MSG_INFO( "Pass state L1 = " << m_trigDec->isPassed( "L1_.*" ) );
   ATH_MSG_INFO( "Pass state L2 = " << m_trigDec->isPassed( "L2_.*" ) );
   ATH_MSG_INFO( "Pass state EF = " << m_trigDec->isPassed( "EF_.*" ) );

   // Example 2: check if a specific chain passed.
   std::string mychain( "EF_j40" );
   if( m_trigDec->getListOfTriggers( mychain ).empty() ){
      ATH_MSG_INFO( "Chain " << mychain << " is not defined" );
   } else {
      ATH_MSG_INFO( "Chain " << mychain << " passed: "
                    << ( m_trigDec->isPassed( mychain ) ? "yes" : "no" ) );
   }

   /* Example 3: Advanced navigation/matching example.
      - pick a chain (e.g. EF_j40)
      - get TriggerElements for chain
      - select active TriggerElements
      - for each RoI (=TE)
      - get JetCollections (EF)
      - print properties of jets in collections
      - get initial TrigRoiDescriptor for RoI (eta, phi, roiWord)
      - loop over LVL1_ROI::std::vector<Jet_ROI>
      - find Jet_ROI with matching roiWord and print properties
   */

   Trig::FeatureContainer fc = m_trigDec->features( mychain );

   // get JetCollection.
   std::vector< Trig::Feature< JetCollection > > jets = fc.get< JetCollection >();

   if( jets.empty() ) {
      ATH_MSG_INFO( "Chain " << mychain  << " produced no JetCollection." );
   } else {
      ATH_MSG_INFO( "Chain " << mychain  << " produced " << jets.size()
                    << " JetCollection(s)." );
   }

   std::vector< Trig::Feature< JetCollection > >::const_iterator jet_itr = jets.begin();
   std::vector< Trig::Feature< JetCollection > >::const_iterator jet_end = jets.end();
   for( ; jet_itr != jet_end; ++jet_itr ) {

      if( jet_itr->te()->getActiveState() ) {
         ATH_MSG_INFO( "For chain " << mychain << ", feature did activate the TE "
                       << jet_itr->label() );
      }
      const JetCollection* jetcoll = *jet_itr;
      if( ! jetcoll ) {
         ATH_MSG_WARNING( "problem getting JetCollection" );
         continue;
      }

      if( ! jetcoll->size() ) {
         ATH_MSG_INFO( "JetCollection found but empty." );
         continue;
      }

      ATH_MSG_INFO( "JetCollection found. Get jets." );
      JetCollection::const_iterator jet_obj_itr = jetcoll->begin();
      JetCollection::const_iterator jet_obj_end = jetcoll->end();
      for( ; jet_obj_itr != jet_obj_end; ++jet_obj_itr ) {
         ATH_MSG_INFO( "  jet: eta " << ( *jet_obj_itr )->eta()
                       << " phi " << ( *jet_obj_itr )->phi()
                       << " et " << ( *jet_obj_itr )->et() );
      }
   }


   // now get initial RoIDescriptor for the same TE as the jets
   std::vector< Trig::Feature< TrigRoiDescriptor > > rois =
      fc.get< TrigRoiDescriptor >( "initialRoI" );

   if( ! rois.empty() ) {

      const TrigRoiDescriptor* roi = rois.front();
      ATH_MSG_INFO( "Initial TrigRoiDescriptor:"
                    << " eta " << roi->eta()
                    << " phi " << roi->phi()
                    << " roiWord " << roi->roiWord()
                    << " l1id " << roi->l1Id()
                    << " roiId " << roi->roiId() << " " << *roi );

      // get LVL1 jet RoIs for comparison and to identify the one with the same roiWord.
      // cache map of roiWord to LVL1_ROI in TrigDecisionTool?
      const LVL1_ROI *lvl1ROI;
      CHECK( evtStore()->retrieve( lvl1ROI ) );

      LVL1_ROI::jets_type::const_iterator jet_itr = lvl1ROI->getJetROIs().begin();
      LVL1_ROI::jets_type::const_iterator jet_end = lvl1ROI->getJetROIs().end();
      for( ; jet_itr != jet_end; ++jet_itr ) {

         ATH_MSG_INFO( "Jet RoI Word: " << jet_itr->getROIWord() );
         if( roi->roiWord() == jet_itr->getROIWord() ) {

            ATH_MSG_INFO( "matching LVL1 jet: " 
                          << " eta " << jet_itr->getEta() 
                          << " phi " << jet_itr->getPhi()
                          << " pattern " << jet_itr->getThrPattern()
                          << " ET 4x4 " << jet_itr->getET4x4()
                          << " ET 6x6 " << jet_itr->getET6x6()
                          << " ET 8x8 " << jet_itr->getET8x8() );
         }
      }
   }


   // for comparison and debug, get the JetCollections directly from SG
   ATH_MSG_INFO( "get the JetCollections directly from SG for comparison" );
   const DataHandle <JetCollection> jetcoll, jetcollLast;
   CHECK( evtStore()->retrieve( jetcoll, jetcollLast ) );

   ATH_MSG_INFO( "found  JetCollections" );
   for( ; jetcoll != jetcollLast; ++jetcoll ) {

      ATH_MSG_INFO( " Container " << jetcoll.key() );

      JetCollection::const_iterator jet_itr = jetcoll->begin();
      JetCollection::const_iterator jet_end = jetcoll->end();
      for( ; jet_itr != jet_end; ++jet_itr ) {

         ATH_MSG_INFO( "  jet: eta " << ( *jet_itr )->eta() << " phi "
                       << ( *jet_itr )->phi() << " et "
                       << ( *jet_itr )->et() );
      }
      
   }

   return StatusCode::SUCCESS;
}


StatusCode TrigAnalysisExample::finalize() {

   ATH_MSG_INFO( "in finalize()" );

   return StatusCode::SUCCESS;
}

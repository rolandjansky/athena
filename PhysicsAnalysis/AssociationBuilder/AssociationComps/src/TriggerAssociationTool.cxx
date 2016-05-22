/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Name: TriggerAssociationTool.cxx
//
/**
   @class TriggerAssociationTool

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date March 2010

   @brief Tool to match an object to every object from an input container
   within a given DeltaR range.
   WARNING: Currently, the link to the matched trigger object is NOT saved, 
   but rather a link to the object itself. This way, you only know THAT a 
   match was found, but not which trigger object matched!

*/
//=============================================================================

// This classes header
#include "TriggerAssociationTool.h"

// STL includes
#include <string>
#include <vector>
#include <utility>
#include <cfloat>
#include <climits>

// INav4Mom includes
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomLink.h"
#include "NavFourMom/INav4MomLinkContainer.h"
#include "NavFourMom/INav4MomAssocs.h"

// Particle includes
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/PhotonContainer.h"
#include "egammaEvent/Photon.h"
#include "muonEvent/MuonContainer.h"
#include "muonEvent/Muon.h"
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauJet.h"
#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"

// L2 includes
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigParticle/TrigTau.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"

// matching tool include
#include "TrigObjectMatching/TrigMatchTool.h"

// The needed ElementLink stuff
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h" 





//=============================================================================
// Constructor
//=============================================================================
TriggerAssociationTool::TriggerAssociationTool( const std::string& type, 
                                                const std::string& name,
                                                const IInterface* parent ) :
  AssociationBaseTool( type, name, parent ),
  m_matchTool( "TrigMatchTool/TrigMatchTool" ),
  m_triggerLevel (-1),
  m_nCalls(0)
{
  // The matching tool
  declareProperty("trigMatchTool",             m_matchTool,                      "The trigger matching tool" );

  declareProperty("triggerToMatchTo",          m_triggerName="",                 "The trigger to which to match to" );

  declareProperty("deltaR",                    m_deltaR = 0.1,                   "Cone size of the DeltaR match" );
  declareProperty("matchToPassedTriggersOnly", m_matchToPassedTriggersOnly=true, "Use only passed triggers to match to" );
}

//=============================================================================
// Destructor
//=============================================================================
TriggerAssociationTool::~TriggerAssociationTool()
{
}


//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode TriggerAssociationTool::initialize()
{
  // Print the used configuration
  ATH_MSG_DEBUG ( "==> initialize " << name() << "..." );
  ATH_MSG_DEBUG ( " using trigMatchTool     = " << m_matchTool );
  ATH_MSG_DEBUG ( " using triggerToMatchTo  = " << m_triggerName );

  
  // Test if we want to match to a Level 1 trigger item
  if( m_triggerName.find( "L1_" ) != std::string::npos )
    {
      m_triggerLevel = 1;
    }
  // Test if we want to match to a Level 1 trigger item
  else if( m_triggerName.find( "L2_" ) != std::string::npos )
    {
      m_triggerLevel = 2;
    }
  // Test if we want to match to a Level 1 trigger item
  else if( m_triggerName.find( "EF_" ) != std::string::npos )
    {
      m_triggerLevel = 3;
    }


  // load the matching tool
  ATH_CHECK( m_matchTool.retrieve() );


  // Do some sanity checks on the user configuration
  bool problemFound(false);
  if ( m_triggerName.empty() )
    {
      ATH_MSG_ERROR ( "Empty triggerToMatchTo! Please configure it properly!" );
      problemFound = true;
    }
  if ( m_triggerLevel == -1 )
    {
      ATH_MSG_ERROR ( "Unrecognized trigger level!" );
      problemFound = true;
    }
  if ( problemFound )
    {
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;
}



//=============================================================================
// The standard method to be called for each particle/inav4mom
//=============================================================================

StatusCode TriggerAssociationTool::calculateAssociations( const INavigable4MomentumCollection* objectContainer,
                                                          const size_t objectIndex,
                                                          INav4MomAssocs& inav4MomAssocs )
{
  //-----------------------------------------
  // Simple status message at the beginning of each call to this method
  //-----------------------------------------
  ATH_MSG_DEBUG ( "==> calculateAssociations " << name() );

  // Update counter
  m_nCalls += 1;


  //----------------------------------------------------------
  // Loop over container and find the matches
  //----------------------------------------------------------
  // Make sure we have a valid pointer
  if ( objectContainer != NULL )
    {
      // Try to find out if we have a particle that the MCTruthClassifier can understand
      const ElectronContainer*         myElectronContainer = dynamic_cast< const ElectronContainer* >(objectContainer);
      const PhotonContainer*           myPhotonContainer   = dynamic_cast< const PhotonContainer* >(objectContainer);
      const Analysis::MuonContainer*   myMuonContainer     = dynamic_cast< const Analysis::MuonContainer* >(objectContainer);
      const Analysis::TauJetContainer* myTauContainer      = dynamic_cast< const Analysis::TauJetContainer* >(objectContainer);
      const JetCollection*             myJetContainer      = dynamic_cast< const JetCollection* >(objectContainer);

      if ( myElectronContainer )
        {
          return calculateAssociations( myElectronContainer, objectIndex, inav4MomAssocs );
        }
      else if ( myPhotonContainer )
        {
          return calculateAssociations( myPhotonContainer, objectIndex, inav4MomAssocs );
        } 
      else if ( myMuonContainer )
        {
          return calculateAssociations( myMuonContainer, objectIndex, inav4MomAssocs );
        } 
      else if ( myTauContainer )
        {
          return calculateAssociations( myTauContainer, objectIndex, inav4MomAssocs );
        } 
      else if ( myJetContainer )
        {
          return calculateAssociations( myJetContainer, objectIndex, inav4MomAssocs );
        } 

    } // End: if ( objectContainer != NULL )
  else
    {
      ATH_MSG_WARNING ( "No valid objectContainer pointer!" );
    } // End: if/else ( objectContainer != NULL )

  return StatusCode::SUCCESS;
}








//=============================================================================
// The method to be called for electrons
//=============================================================================
StatusCode TriggerAssociationTool::calculateAssociations( const ElectronContainer* electronContainer,
                                                          const size_t electronIndex,
                                                          INav4MomAssocs& inav4MomAssocs )
{
  //-----------------------------------------
  // Simple status message at the beginning of each call to this method
  //-----------------------------------------
  ATH_MSG_DEBUG ( "==> calculateAssociations " << name() );


  //-----------------------------------------
  // Get the current object
  //-----------------------------------------
  const Analysis::Electron* electron(NULL);
  electron = (*electronContainer)[electronIndex];


  //----------------------------------------------------------
  // Loop over container and find the matches
  //----------------------------------------------------------
  // Make sure we have a valid pointer
  if ( electron != NULL )
    {
      // Test if we want to match to a Level 1 trigger item
      if( m_triggerLevel == 1 )
        {
          // Call the trigger matching tool
          const TrigMatch::TrigElectronL1 *match = 
            m_matchTool->matchToTriggerObject< TrigMatch::TrigElectronL1 >( electron,
                                                                            m_triggerName,
                                                                            m_deltaR,
                                                                            m_matchToPassedTriggersOnly );
          // Print out what match we have found
          if ( match )
            {
              // Add the current match to the association map
              // THIS IS A HACK!!! NOT A LINK TO THE MATCHED-TO OBJECT, BUT RATHER TO ITSELF!
              // ONLY GOOD TO KNOW IF A MATCH WAS FOUND!
              inav4MomAssocs.addAssociation( electronContainer,
                                             electronIndex,
                                             electronContainer,
                                             electronIndex );
              
              ATH_MSG_DEBUG ( "Found in trigger with name " << m_triggerName
                              << " a matching trigger electron with eta=" << match->eta()
                              << ", phi=" << match->phi()
                              << ", and pt=" << match->pt() );
            } // End: if ( match )
   
        } // End: L1
      // Test if we want to match to a Level 2 trigger item
      else if( m_triggerLevel == 2 )
        {
          // Call the trigger matching tool
          const TrigMatch::TrigElectronL2 *match = 
            m_matchTool->matchToTriggerObject< TrigMatch::TrigElectronL2 >( electron,
                                                                            m_triggerName,
                                                                            m_deltaR,
                                                                            m_matchToPassedTriggersOnly );
          // Print out what match we have found
          if ( match )
            {
              // Add the current match to the association map
              // THIS IS A HACK!!! NOT A LINK TO THE MATCHED-TO OBJECT, BUT RATHER TO ITSELF!
              // ONLY GOOD TO KNOW IF A MATCH WAS FOUND!
              inav4MomAssocs.addAssociation( electronContainer,
                                             electronIndex,
                                             electronContainer,
                                             electronIndex );
              
              ATH_MSG_DEBUG ( "Found in trigger with name " << m_triggerName
                              << " a matching trigger electron with eta=" << match->eta()
                              << ", phi=" << match->phi()
                              << ", and pt=" << match->pt() );
            } // End: if ( match )
   
        } // End: L2
      // Test if we want to match to a Level 2 trigger item
      else if( m_triggerLevel == 3 )
        {
          // Call the trigger matching tool
          const TrigMatch::TrigElectronEF *match = 
            m_matchTool->matchToTriggerObject< TrigMatch::TrigElectronEF >( electron,
                                                                            m_triggerName,
                                                                            m_deltaR,
                                                                            m_matchToPassedTriggersOnly );
          // Print out what match we have found
          if ( match )
            {
              // Add the current match to the association map
              // THIS IS A HACK!!! NOT A LINK TO THE MATCHED-TO OBJECT, BUT RATHER TO ITSELF!
              // ONLY GOOD TO KNOW IF A MATCH WAS FOUND!
              inav4MomAssocs.addAssociation( electronContainer,
                                             electronIndex,
                                             electronContainer,
                                             electronIndex );

              ATH_MSG_DEBUG ( "Found in trigger with name " << m_triggerName
                              << " a matching trigger electron with eta=" << match->eta()
                              << ", phi=" << match->phi()
                              << ", and pt=" << match->pt() );
            } // End: if ( match )
   
        } // End: EF

    } // End: if ( electron != NULL )
  else
    {
      ATH_MSG_WARNING ( "No valid electron pointer!" );
    } // End: if/else ( electron != NULL )
  
  return StatusCode::SUCCESS;
}








//=============================================================================
// The method to be called for photons
//=============================================================================
StatusCode TriggerAssociationTool::calculateAssociations( const PhotonContainer* photonContainer,
                                                          const size_t photonIndex,
                                                          INav4MomAssocs& inav4MomAssocs )
{
  //-----------------------------------------
  // Simple status message at the beginning of each call to this method
  //-----------------------------------------
  ATH_MSG_DEBUG ( "==> calculateAssociations " << name() );


  //-----------------------------------------
  // Get the current object
  //-----------------------------------------
  const Analysis::Photon* photon(NULL);
  photon = (*photonContainer)[photonIndex];


  //----------------------------------------------------------
  // Loop over container and find the matches
  //----------------------------------------------------------
  // Make sure we have a valid pointer
  if ( photon != NULL )
    {
      // Test if we want to match to a Level 1 trigger item
      if( m_triggerLevel == 1 )
        {
          // Call the trigger matching tool
          const TrigMatch::TrigPhotonL1 *match = 
            m_matchTool->matchToTriggerObject< TrigMatch::TrigPhotonL1 >( photon,
                                                                          m_triggerName,
                                                                          m_deltaR,
                                                                          m_matchToPassedTriggersOnly );
          // Print out what match we have found
          if ( match )
            {
              // Add the current match to the association map
              // THIS IS A HACK!!! NOT A LINK TO THE MATCHED-TO OBJECT, BUT RATHER TO ITSELF!
              // ONLY GOOD TO KNOW IF A MATCH WAS FOUND!
              inav4MomAssocs.addAssociation( photonContainer,
                                             photonIndex,
                                             photonContainer,
                                             photonIndex );

              ATH_MSG_DEBUG ( "Found in trigger with name " << m_triggerName
                              << " a matching trigger photon with eta=" << match->eta()
                              << ", phi=" << match->phi()
                              << ", and pt=" << match->pt() );
            } // End: if ( match )
   
        } // End: L1
      // Test if we want to match to a Level 2 trigger item
      else if( m_triggerLevel == 2 )
        {
          // Call the trigger matching tool
          const TrigMatch::TrigPhotonL2 *match = 
            m_matchTool->matchToTriggerObject< TrigMatch::TrigPhotonL2 >( photon,
                                                                          m_triggerName,
                                                                          m_deltaR,
                                                                          m_matchToPassedTriggersOnly );
          // Print out what match we have found
          if ( match )
            {
              // Add the current match to the association map
              // THIS IS A HACK!!! NOT A LINK TO THE MATCHED-TO OBJECT, BUT RATHER TO ITSELF!
              // ONLY GOOD TO KNOW IF A MATCH WAS FOUND!
              inav4MomAssocs.addAssociation( photonContainer,
                                             photonIndex,
                                             photonContainer,
                                             photonIndex );

              ATH_MSG_DEBUG ( "Found in trigger with name " << m_triggerName
                              << " a matching trigger photon with eta=" << match->eta()
                              << ", phi=" << match->phi()
                              << ", and pt=" << match->pt() );
            } // End: if ( match )
   
        } // End: L2
      // Test if we want to match to a Level 2 trigger item
      else if( m_triggerLevel == 3 )
        {
          // Call the trigger matching tool
          const TrigMatch::TrigPhotonEF *match = 
            m_matchTool->matchToTriggerObject< TrigMatch::TrigPhotonEF >( photon,
                                                                          m_triggerName,
                                                                          m_deltaR,
                                                                          m_matchToPassedTriggersOnly );
          // Print out what match we have found
          if ( match )
            {
              // Add the current match to the association map
              // THIS IS A HACK!!! NOT A LINK TO THE MATCHED-TO OBJECT, BUT RATHER TO ITSELF!
              // ONLY GOOD TO KNOW IF A MATCH WAS FOUND!
              inav4MomAssocs.addAssociation( photonContainer,
                                             photonIndex,
                                             photonContainer,
                                             photonIndex );
              
              ATH_MSG_DEBUG ( "Found in trigger with name " << m_triggerName
                              << " a matching trigger photon with eta=" << match->eta()
                              << ", phi=" << match->phi()
                              << ", and pt=" << match->pt() );
            } // End: if ( match )
   
        } // End: EF

    } // End: if ( photon != NULL )
  else
    {
      ATH_MSG_WARNING ( "No valid photon pointer!" );
    } // End: if/else ( photon != NULL )
  
  return StatusCode::SUCCESS;
}






//=============================================================================
// The method to be called for muons
//=============================================================================
StatusCode TriggerAssociationTool::calculateAssociations( const Analysis::MuonContainer* muonContainer,
                                                          const size_t muonIndex,
                                                          INav4MomAssocs& inav4MomAssocs )
{
  //-----------------------------------------
  // Simple status message at the beginning of each call to this method
  //-----------------------------------------
  ATH_MSG_DEBUG ( "==> calculateAssociations " << name() );


  //-----------------------------------------
  // Get the current object
  //-----------------------------------------
  const Analysis::Muon* muon(NULL);
  muon = (*muonContainer)[muonIndex];


  //----------------------------------------------------------
  // Loop over container and find the matches
  //----------------------------------------------------------
  // Make sure we have a valid pointer
  if ( muon != NULL )
    {
      // Test if we want to match to a Level 1 trigger item
      if( m_triggerLevel == 1 )
        {
          // Call the trigger matching tool
          const TrigMatch::TrigMuonL1 *match = 
            m_matchTool->matchToTriggerObject< TrigMatch::TrigMuonL1 >( muon,
                                                                        m_triggerName,
                                                                        m_deltaR,
                                                                        m_matchToPassedTriggersOnly );
          // Print out what match we have found
          if ( match )
            {
              // Add the current match to the association map
              // THIS IS A HACK!!! NOT A LINK TO THE MATCHED-TO OBJECT, BUT RATHER TO ITSELF!
              // ONLY GOOD TO KNOW IF A MATCH WAS FOUND!
              inav4MomAssocs.addAssociation( muonContainer,
                                             muonIndex,
                                             muonContainer,
                                             muonIndex );

              ATH_MSG_DEBUG ( "Found in trigger with name " << m_triggerName
                              << " a matching trigger muon with eta=" << match->eta()
                              << ", phi=" << match->phi()
                              << ", and pt=" << match->pt() );
            } // End: if ( match )
   
        } // End: L1
      // Test if we want to match to a Level 2 trigger item
      else if( m_triggerLevel == 2 )
        {
          // Call the trigger matching tool
          const TrigMatch::TrigMuonL2 *match = 
            m_matchTool->matchToTriggerObject< TrigMatch::TrigMuonL2 >( muon,
                                                                        m_triggerName,
                                                                        m_deltaR,
                                                                        m_matchToPassedTriggersOnly );
          // Print out what match we have found
          if ( match )
            {
              // Add the current match to the association map
              // THIS IS A HACK!!! NOT A LINK TO THE MATCHED-TO OBJECT, BUT RATHER TO ITSELF!
              // ONLY GOOD TO KNOW IF A MATCH WAS FOUND!
              inav4MomAssocs.addAssociation( muonContainer,
                                             muonIndex,
                                             muonContainer,
                                             muonIndex );

              ATH_MSG_DEBUG ( "Found in trigger with name " << m_triggerName
                              << " a matching trigger muon with eta=" << match->eta()
                              << ", phi=" << match->phi()
                              << ", and pt=" << match->pt() );
            } // End: if ( match )
   
        } // End: L2
      // Test if we want to match to a Level 2 trigger item
      else if( m_triggerLevel == 3 )
        {
          // Call the trigger matching tool
          const TrigMuonEFInfo *match = 
            m_matchTool->matchToTriggerObject< TrigMuonEFInfo >( muon,
                                                                 m_triggerName,
                                                                 m_deltaR,
                                                                 m_matchToPassedTriggersOnly );
          // Print out what match we have found
          if ( match )
            {
              // Add the current match to the association map
              // THIS IS A HACK!!! NOT A LINK TO THE MATCHED-TO OBJECT, BUT RATHER TO ITSELF!
              // ONLY GOOD TO KNOW IF A MATCH WAS FOUND!
              inav4MomAssocs.addAssociation( muonContainer,
                                             muonIndex,
                                             muonContainer,
                                             muonIndex );

              ATH_MSG_DEBUG ( "Found in trigger with name " << m_triggerName );
            } // End: if ( match )
   
        } // End: EF

    } // End: if ( muon != NULL )
  else
    {
      ATH_MSG_WARNING ( "No valid muon pointer!" );
    } // End: if/else ( muon != NULL )
  
  return StatusCode::SUCCESS;
}







//=============================================================================
// The method to be called for taus
//=============================================================================
StatusCode TriggerAssociationTool::calculateAssociations( const Analysis::TauJetContainer* tauContainer,
                                                          const size_t tauIndex,
                                                          INav4MomAssocs& inav4MomAssocs )
{
  //-----------------------------------------
  // Simple status message at the beginning of each call to this method
  //-----------------------------------------
  ATH_MSG_DEBUG ( "==> calculateAssociations " << name() );


  //-----------------------------------------
  // Get the current object
  //-----------------------------------------
  const Analysis::TauJet* tau(NULL);
  tau = (*tauContainer)[tauIndex];


  //----------------------------------------------------------
  // Loop over container and find the matches
  //----------------------------------------------------------
  // Make sure we have a valid pointer
  if ( tau != NULL )
    {
      // Test if we want to match to a Level 1 trigger item
      if( m_triggerLevel == 1 )
        {
          // Call the trigger matching tool
          const TrigMatch::TrigTauL1 *match = 
            m_matchTool->matchToTriggerObject< TrigMatch::TrigTauL1 >( tau,
                                                                       m_triggerName,
                                                                       m_deltaR,
                                                                       m_matchToPassedTriggersOnly );
          // Print out what match we have found
          if ( match )
            {
              // Add the current match to the association map
              // THIS IS A HACK!!! NOT A LINK TO THE MATCHED-TO OBJECT, BUT RATHER TO ITSELF!
              // ONLY GOOD TO KNOW IF A MATCH WAS FOUND!
              inav4MomAssocs.addAssociation( tauContainer,
                                             tauIndex,
                                             tauContainer,
                                             tauIndex );

              ATH_MSG_DEBUG ( "Found in trigger with name " << m_triggerName
                              << " a matching trigger tau with eta=" << match->eta()
                              << ", phi=" << match->phi()
                              << ", and pt=" << match->pt() );
            } // End: if ( match )
   
        } // End: L1
      // Test if we want to match to a Level 2 trigger item
      else if( m_triggerLevel == 2 )
        {
          // Call the trigger matching tool
          const TrigMatch::TrigTauL2 *match = 
            m_matchTool->matchToTriggerObject< TrigMatch::TrigTauL2 >( tau,
                                                                       m_triggerName,
                                                                       m_deltaR,
                                                                       m_matchToPassedTriggersOnly );
          // Print out what match we have found
          if ( match )
            {
              // Add the current match to the association map
              // THIS IS A HACK!!! NOT A LINK TO THE MATCHED-TO OBJECT, BUT RATHER TO ITSELF!
              // ONLY GOOD TO KNOW IF A MATCH WAS FOUND!
              inav4MomAssocs.addAssociation( tauContainer,
                                             tauIndex,
                                             tauContainer,
                                             tauIndex );

              ATH_MSG_DEBUG ( "Found in trigger with name " << m_triggerName
                              << " a matching trigger tau with eta=" << match->eta()
                              << ", phi=" << match->phi()
                              << ", and pt=" << match->pt() );
            } // End: if ( match )
   
        } // End: L2
      // Test if we want to match to a Level 2 trigger item
      else if( m_triggerLevel == 3 )
        {
          // Call the trigger matching tool
          const TrigMatch::TrigTauEF *match = 
            m_matchTool->matchToTriggerObject< TrigMatch::TrigTauEF >( tau,
                                                                       m_triggerName,
                                                                       m_deltaR,
                                                                       m_matchToPassedTriggersOnly );
          // Print out what match we have found
          if ( match )
            {
              // Add the current match to the association map
              // THIS IS A HACK!!! NOT A LINK TO THE MATCHED-TO OBJECT, BUT RATHER TO ITSELF!
              // ONLY GOOD TO KNOW IF A MATCH WAS FOUND!
              inav4MomAssocs.addAssociation( tauContainer,
                                             tauIndex,
                                             tauContainer,
                                             tauIndex );

              ATH_MSG_DEBUG ( "Found in trigger with name " << m_triggerName
                              << " a matching trigger tau with eta=" << match->eta()
                              << ", phi=" << match->phi()
                              << ", and pt=" << match->pt() );
            } // End: if ( match )
   
        } // End: EF

    } // End: if ( tau != NULL )
  else
    {
      ATH_MSG_WARNING ( "No valid tau pointer!" );
    } // End: if/else ( tau != NULL )
  
  return StatusCode::SUCCESS;
}







//=============================================================================
// The method to be called for jets
//=============================================================================
StatusCode TriggerAssociationTool::calculateAssociations( const JetCollection* jetContainer,
                                                          const size_t jetIndex,
                                                          INav4MomAssocs& inav4MomAssocs )
{
  //-----------------------------------------
  // Simple status message at the beginning of each call to this method
  //-----------------------------------------
  ATH_MSG_DEBUG ( "==> calculateAssociations " << name() );


  //-----------------------------------------
  // Get the current object
  //-----------------------------------------
  const Jet* jet(NULL);
  jet = (*jetContainer)[jetIndex];


  //----------------------------------------------------------
  // Loop over container and find the matches
  //----------------------------------------------------------
  // Make sure we have a valid pointer
  if ( jet != NULL )
    {
      // Test if we want to match to a Level 1 trigger item
      if( m_triggerLevel == 1 )
        {
          // Call the trigger matching tool
          const TrigMatch::TrigJetL1 *match = 
            m_matchTool->matchToTriggerObject< TrigMatch::TrigJetL1 >( jet,
                                                                       m_triggerName,
                                                                       m_deltaR,
                                                                       m_matchToPassedTriggersOnly );
          // Print out what match we have found
          if ( match )
            {
              // Add the current match to the association map
              // THIS IS A HACK!!! NOT A LINK TO THE MATCHED-TO OBJECT, BUT RATHER TO ITSELF!
              // ONLY GOOD TO KNOW IF A MATCH WAS FOUND!
              inav4MomAssocs.addAssociation( jetContainer,
                                             jetIndex,
                                             jetContainer,
                                             jetIndex );

              ATH_MSG_DEBUG ( "Found in trigger with name " << m_triggerName
                              << " a matching trigger jet with eta=" << match->eta()
                              << ", phi=" << match->phi()
                              << ", and pt=" << match->pt() );
            } // End: if ( match )
   
        } // End: L1
      // Test if we want to match to a Level 2 trigger item
      else if( m_triggerLevel == 2 )
        {
          // Call the trigger matching tool
          const TrigMatch::TrigJetL2 *match = 
            m_matchTool->matchToTriggerObject< TrigMatch::TrigJetL2 >( jet,
                                                                       m_triggerName,
                                                                       m_deltaR,
                                                                       m_matchToPassedTriggersOnly );
          // Print out what match we have found
          if ( match )
            {
              // Add the current match to the association map
              // THIS IS A HACK!!! NOT A LINK TO THE MATCHED-TO OBJECT, BUT RATHER TO ITSELF!
              // ONLY GOOD TO KNOW IF A MATCH WAS FOUND!
              inav4MomAssocs.addAssociation( jetContainer,
                                             jetIndex,
                                             jetContainer,
                                             jetIndex );

              ATH_MSG_DEBUG ( "Found in trigger with name " << m_triggerName
                              << " a matching trigger jet with eta=" << match->eta()
                              << ", phi=" << match->phi()
                              << ", and pt=" << match->pt() );
            } // End: if ( match )
   
        } // End: L2
      // Test if we want to match to a Level 2 trigger item
      else if( m_triggerLevel == 3 )
        {
          // Call the trigger matching tool
          const TrigMatch::TrigJetEF *match = 
            m_matchTool->matchToTriggerObject< TrigMatch::TrigJetEF >( jet,
                                                                       m_triggerName,
                                                                       m_deltaR,
                                                                       m_matchToPassedTriggersOnly );
          // Print out what match we have found
          if ( match )
            {
              // Add the current match to the association map
              // THIS IS A HACK!!! NOT A LINK TO THE MATCHED-TO OBJECT, BUT RATHER TO ITSELF!
              // ONLY GOOD TO KNOW IF A MATCH WAS FOUND!
              inav4MomAssocs.addAssociation( jetContainer,
                                             jetIndex,
                                             jetContainer,
                                             jetIndex );

              ATH_MSG_DEBUG ( "Found in trigger with name " << m_triggerName
                              << " a matching trigger jet with eta=" << match->eta()
                              << ", phi=" << match->phi()
                              << ", and pt=" << match->pt() );
            } // End: if ( match )
   
        } // End: EF

    } // End: if ( jet != NULL )
  else
    {
      ATH_MSG_WARNING ( "No valid jet pointer!" );
    } // End: if/else ( jet != NULL )
  
  return StatusCode::SUCCESS;
}




//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode TriggerAssociationTool::finalize()
{
  ATH_MSG_DEBUG ( "FINALIZING AFTER ALL EVENTS ARE PROCESSED" );
  return StatusCode::SUCCESS;
}




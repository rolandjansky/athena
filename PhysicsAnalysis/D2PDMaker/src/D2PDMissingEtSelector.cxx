/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Name: D2PDMissingEtSelector.cxx
//
/**
   @class D2PDMissingEtSelector

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Select events based on the missing Et and create a neutrino container.

*/
//=============================================================================

// This classes header
#include "D2PDMaker/D2PDMissingEtSelector.h"

// STL includes
#include <string>
#include <vector>
#include <cfloat>
#include <climits>
#include <math.h>

// Gaudi stuff
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

// The AthenaBarCode, used to check if two particles are identical
#include "Navigation/IAthenaBarCode.h"

// The needed ElementLink stuff
#include "DataModel/DataVector.h"
#include "DataModel/ElementLink.h" 

// Different particles inherit from this
#include "NavFourMom/INav4MomLink.h"
#include "NavFourMom/INav4MomLinkContainer.h"

// neutrino includes
#include "ParticleEvent/Neutrino.h"
#include "ParticleEvent/NeutrinoContainer.h"
#include "Particle/TrackParticle.h"
#include "MissingETEvent/MissingET.h"

// Neutrino helper tools
#include "SpecialUtils/NeutrinoUtils.h"


using CLHEP::HepLorentzVector;


//=============================================================================
// Constructor
//=============================================================================
D2PDMissingEtSelector::D2PDMissingEtSelector(const std::string& name,
                                             ISvcLocator* pSvcLocator) :
  AthFilterAlgorithm ( name,     pSvcLocator ),
  m_kinSelectorTool( "KinematicSelector/MissingEtKinematicSelectorForMissingEtSelector", this )
{
  declareProperty("kinematicSelectorTool",    m_kinSelectorTool, "Kinematic selection tool" );

  declareProperty("printSetup",              m_printSetup   = false, "Printing the setup if true" );
  declareProperty("printSummary",            m_printSummary = false, "Printing the summary if true" );

  declareProperty("inputCollection",          m_inCollKey="" );
  declareProperty("outputCollection",         m_outCollKey="" );
  declareProperty("outputLinkCollection",     m_outLinkCollKey="" );

  declareProperty("minNumberPassed",          m_minNumberPassed=0,
                  "Min number of selected neutrinos" );
  declareProperty("maxNumberPassed",          m_maxNumberPassed=INT_MAX,
                  "Max number of selected neutrinos" );

  declareProperty("missingEtMin",             m_missingEtMin=-DBL_MAX, "Min missing E_T" );
  declareProperty("missingEtMax",             m_missingEtMax=DBL_MAX, "Max missing E_T" );

  declareProperty("sumEtMin",                 m_sumEtMin=-DBL_MAX, "Min sumEt" );
  declareProperty("sumEtMax",                 m_sumEtMax=DBL_MAX, "Max sumEt" );

  declareProperty("missingEtSignificanceMin", m_missingEtSignifMin=-DBL_MAX, "Min missing E_T significance" );
  declareProperty("missingEtSignificanceMax", m_missingEtSignifMax=DBL_MAX,  "Max missing E_T significance" );

  declareProperty("missingEtPhiMin",          m_phiMin=-DBL_MAX, "Min phi" );
  declareProperty("missingEtPhiMax",          m_phiMax=DBL_MAX,  "Max phi" );

  declareProperty("missingEtPhiVetoRanges",   m_phiVetoRanges="", "Phi-ranges to veto" );

  declareProperty("dumpStoreGate",
                  m_dumpStoreGate=false, 
                  "Flag to dump StoreGate content after each event" );

  // Initialize the counters to zero
  m_nEventsProcessed = 0 ;
}



//=============================================================================
// Destructor
//=============================================================================
D2PDMissingEtSelector::~D2PDMissingEtSelector()
{
}


//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode D2PDMissingEtSelector::initialize()
{
  // Print the used configuration
  if ( m_printSetup || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "==> initializeMore " << name() << "..." );
      
      // Print out the used configuration
      ATH_MSG_INFO ( " using printSetup                      = " << m_printSetup );
      ATH_MSG_INFO ( " using printSummary                    = " << m_printSummary );

      ATH_MSG_INFO ( " using inputCollection          = " << m_inCollKey );
      ATH_MSG_INFO ( " using outputCollection         = " << m_outCollKey );
      ATH_MSG_INFO ( " using outputLinkCollection     = " << m_outLinkCollKey );

      ATH_MSG_INFO ( " using kinematicSelectorTool    = " << m_kinSelectorTool );

      ATH_MSG_INFO ( " using minNumberPassed          = " << m_minNumberPassed );
      ATH_MSG_INFO ( " using maxNumberPassed          = " << m_maxNumberPassed );

      ATH_MSG_INFO ( " using missingEtMin             = " << m_missingEtMin );
      ATH_MSG_INFO ( " using missingEtMax             = " << m_missingEtMax );

      ATH_MSG_INFO ( " using sumEtMin                 = " << m_sumEtMin );
      ATH_MSG_INFO ( " using sumEtMax                 = " << m_sumEtMax );

      ATH_MSG_INFO ( " using missingEtSignificanceMin = " << m_missingEtSignifMin );
      ATH_MSG_INFO ( " using missingEtSignificanceMax = " << m_missingEtSignifMax );

      ATH_MSG_INFO ( " using missingEtPhiMin          = " << m_phiMin );
      ATH_MSG_INFO ( " using missingEtPhiMax          = " << m_phiMax );

      ATH_MSG_INFO ( " using missingEtPhiVetoRanges   = " << m_phiVetoRanges );

      ATH_MSG_INFO ( " using dumpStoreGate            = " << m_dumpStoreGate );
    } // End: if ( m_printSetup )

  
  // get the kinematic selector tool (derived class from ISelectorCore)
  ATH_CHECK( m_kinSelectorTool.retrieve() );

  // get the FilterTool 
  ATH_CHECK( toolSvc()->retrieveTool("FilterTool", m_filterTool) );


  // Do some sanity checks on the user configuration
  if ( m_inCollKey.empty() )
    {
      ATH_MSG_ERROR ( "Empty input collection! Please configure it properly!" );
      return StatusCode::FAILURE ;
    }
  if ( m_outCollKey.empty() && !(m_outLinkCollKey.empty()) )
    {
      ATH_MSG_ERROR ( "Empty output collection and non-empty outputLinkCollection!"
                      << " This doesn't work! The outputLinkCollection must be able to point somewheree!"
                      << " Please configure it properly!" );
      return StatusCode::FAILURE ;
    }


  
  //--------------------------------------------------------------------------
  // Set the cut values for the selection tools
  //--------------------------------------------------------------------------
  m_kinSelectorTool->getTool()->setminPhi( m_phiMin );
  m_kinSelectorTool->getTool()->setmaxPhi( m_phiMax );

  m_kinSelectorTool->getTool()->setPhiVetoRanges( m_phiVetoRanges );


  // Initialize the counters to zero
  m_nEventsProcessed = 0 ;
  
  return StatusCode::SUCCESS;

}



//=============================================================================
// Athena execute method
//=============================================================================
StatusCode D2PDMissingEtSelector::execute()
{
  //-----------------------------------------
  // Increase the event counter
  //-----------------------------------------
  ++m_nEventsProcessed ;


  //-----------------------------------------
  // Simple status message at the beginning of each event execute,
  //-----------------------------------------
  ATH_MSG_DEBUG ( "==> execute " << name() << " on " << m_nEventsProcessed << ". event..." );



  //-----------------------------------------
  //Retrieve the neutrino container
  //-----------------------------------------
  // The missing ET object
  const MissingET* inMissingEtObject(0);
  if ( !(evtStore()->retrieve( inMissingEtObject, m_inCollKey )).isSuccess() || !inMissingEtObject )
    {
      if ( m_nEventsProcessed <= 10 )
        {
          ATH_MSG_WARNING ( "Input collection = '" << m_inCollKey
                            << "' could not be retrieved from StoreGate! "
                            << " This message will only be repeated 10 times..." );
        }
      else
        {
          ATH_MSG_DEBUG ( "Input collection = '" << m_inCollKey
                          << "' could not be retrieved from StoreGate! " );
        }
      return StatusCode::SUCCESS;
    } 
  ATH_MSG_DEBUG ( "Input collection = '" << m_inCollKey << "' retrieved from StoreGate" );



  //-----------------------------------------
  // Create the output container for selected 
  // neutrinos and record it in StoreGate
  //-----------------------------------------
  NeutrinoContainer* outContainer(NULL);
  if ( !(m_outCollKey.empty()) )
    {
      outContainer = new NeutrinoContainer( SG::OWN_ELEMENTS );  
      ATH_CHECK( evtStore()->record ( outContainer, m_outCollKey ) );
    }



  //-----------------------------------------
  // Create the output container for the ElementLinks
  // to the selected neutrinos and record it in StoreGate
  //-----------------------------------------
  INav4MomLinkContainer* outLinkContainer(NULL);
  if ( !(m_outCollKey.empty()) && !(m_outLinkCollKey.empty()) )
    {
      outLinkContainer = new INav4MomLinkContainer();
      ATH_CHECK( evtStore()->record ( outLinkContainer, m_outLinkCollKey ) );
    } // End: if ( m_outLinkCollKey != "" )



  // Create a counter for the number of selected objects
  unsigned int nSelected(0);


  //-----------------------------------------
  // Create the neutrino object
  //-----------------------------------------
  double etMiss          = inMissingEtObject->et();
  double etxMiss         = inMissingEtObject->etx();
  double etyMiss         = inMissingEtObject->ety();
  double sumEt           = inMissingEtObject->sumet();
  double etMissGeV       = 0.001*etMiss;
  double sumEtGeV        = 0.001*sumEt;
  double missingEtSignif = 0.0;
  if ( sumEtGeV != 0.0 )
    {
      if ( sumEtGeV > 0.0 )
        {
          missingEtSignif = etMissGeV / ( 0.5 * sqrt( sumEtGeV ) ) ;
        }
      else if ( sumEtGeV < 0.0 )
        {
          missingEtSignif = -1.0*etMissGeV / ( 0.5 * sqrt( -1.0*sumEtGeV ) ) ;
        }
    }


  //--------------------------
  // Do the actual selections
  //--------------------------
  bool passAll = true;
  // missing transverse energy selection
  if ( passAll )
    {
      passAll = m_filterTool->doubleFilter( etMiss, m_missingEtMin, m_missingEtMax );
    }
  // sum Et selection
  if ( passAll )
    {
      passAll = m_filterTool->doubleFilter( sumEt, m_sumEtMin, m_sumEtMax );
    }
  // missing Et significance selection
  if ( passAll )
    {
      passAll = m_filterTool->doubleFilter( missingEtSignif, m_missingEtSignifMin, m_missingEtSignifMax );
    }


  //----------------------------------------------------------
  // Write out the neutrino (links) if all cuts are passed
  //----------------------------------------------------------
  Neutrino* neutrino = new Neutrino();

  // Make sure we have an neutrino
  if ( neutrino )
    {
      neutrino->set4Mom(HepLorentzVector( etxMiss, etyMiss, 0.0, etMiss ));
      neutrino->set_charge(0);
      neutrino->set_pdgId(PDG::nu_e);

      // phi selection
      if ( passAll )
        {
          passAll = m_kinSelectorTool->getTool()->passPhi( neutrino );
        }
      // phi veto selection
      if ( passAll )
        {
          passAll = m_kinSelectorTool->getTool()->passPhiVetoRanges( neutrino );
        }

      // Add the neutrinos that pass all selections 
      // to the output container
      if ( passAll )
        {
          // Update the counter of number of selected objects
          nSelected++;

          // Actually put the neutrino into the output container
          if ( !(m_outCollKey.empty()) && outContainer != NULL )
            {
              outContainer->push_back( neutrino );
            }
          else
            {
              delete neutrino;
              neutrino = 0;
            }
        }
      else
        {
          delete neutrino;
          neutrino = 0;
        }
      
    }// End: if neutrino
  else
    {
      //delete neutrino;
      neutrino = 0;
    }


  // Some debug statements for the log file
  if ( !(m_outCollKey.empty()) && !(m_outLinkCollKey.empty()) && outContainer != NULL && outLinkContainer != NULL )
    {
      ATH_MSG_DEBUG ( "In event number " << m_nEventsProcessed
                      << "  Input collection size = 1'"
                      << "  Output collection size = '" << outContainer->size()
                      << "  Output link collection size = '" << outLinkContainer->size() );
    }
  else
    {
      ATH_MSG_DEBUG ( "In event number " << m_nEventsProcessed
                      << "  Input collection size = 1"
                      << "  Number of seleced objects = 1" << nSelected );
    }



  //-----------------------------------------
  // Set the output container of selected neutrinos as const
  //-----------------------------------------
  if ( !(m_outCollKey.empty()) && outContainer != NULL )
    {
      ATH_CHECK( evtStore()->setConst( outContainer ) );
    }


  //-----------------------------------------
  // Create the output link container of selected neutrinos
  // and set it to const
  //-----------------------------------------
  if ( !(m_outLinkCollKey.empty()) && outContainer != NULL && outLinkContainer != NULL )
    {
      // Loop over the newly created output container 
      for ( unsigned int i=0; i < outContainer->size(); ++i )
        {
          // Actually create an ElementLink to the selected neutrino
          // and add this ElementLink to the output link container
          INav4MomLink neutrinoLink( *outContainer, i );
          outLinkContainer->push_back( neutrinoLink );
        }
      ATH_CHECK( evtStore()->setConst( outLinkContainer ) );
    } // End: if ( m_outLinkCollKey != "" )





  //-----------------------------------------
  // Determine if this event is accepted
  //-----------------------------------------
  if ( nSelected >= m_minNumberPassed 
       && nSelected <= m_maxNumberPassed )
    {
      ATH_MSG_DEBUG ( " Event " << m_nEventsProcessed
                      << " passed, found " << nSelected
                      << " neutrinos. Required were minNumber=" << m_minNumberPassed
                      << " and maxNumber=" << m_maxNumberPassed );
      setFilterPassed(true);
    }
  else
    {
      ATH_MSG_DEBUG ( " Event " << m_nEventsProcessed
                      << " failed, found " << nSelected
                      << " neutrinos. Required were minNumber=" << m_minNumberPassed
                      << " and maxNumber=" << m_maxNumberPassed );
      setFilterPassed(false);
    }


  // Writing out all containers in StoreGate to the log file
  if ( m_dumpStoreGate )
    {
      ATH_MSG_INFO ( "StoreGateDump: \n" << evtStore()->dump() );
    } // End: if ( m_dumpStoreGate )
  
  return StatusCode::SUCCESS;
}





//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode D2PDMissingEtSelector::finalize()
{
  if ( m_printSummary || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "FINALIZING AFTER ALL EVENTS ARE PROCESSED" );
    }

  return StatusCode::SUCCESS;
}




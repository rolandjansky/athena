/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Name: D2PDParticleCombiner.cxx
//
/**
   @class D2PDParticleCombiner

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Combine particles to composite particles

   Class to combine two or more particles from a given list of
   input containers. All combinations will be tried and only the
   ones passing the used selections will be written to UserData.
*/
//=============================================================================

// This classes header
#include "D2PDMaker/D2PDParticleCombiner.h"

// STL includes
#include <string>
#include <vector>
#include <cfloat>
#include <climits>

// The needed ElementLink stuff
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h" 

// The composite particle
#include "ParticleEvent/CompositeParticle.h"
#include "ParticleEvent/CompositeParticleContainer.h"

// Different particles inherit from this
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomLink.h"
#include "NavFourMom/INav4MomLinkContainer.h"
#include "EventKernel/IParticle.h"

// MC Truth includes
#include "GeneratorObjects/McEventCollection.h"
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// The AthenaBarCode, used to check if two particles are identical
#include "Navigation/IAthenaBarCode.h"

// include the Odometer Algorithm from  EventViewCombiners/EVComboHelper.h
#include "PATCore/CombinatoricsOdometer.h"



//=============================================================================
// Constructor
//=============================================================================
D2PDParticleCombiner::D2PDParticleCombiner(const std::string& name,
                                           ISvcLocator* pSvcLocator) :
  AthFilterAlgorithm ( name,     pSvcLocator ),
  m_mcEventColl(0),
  m_kinSelectorTool( "KinematicSelector/KinematicSelectorForD2PDParticleCombiner", this ),
  m_filterTool(nullptr),
  m_selectionTools(this),
  m_associationTools(this),
  m_userDataCalcTools(this),
  m_isTwoBodyDecay(false)
{
  declareProperty("printSetup",              m_printSetup   = false, "Printing the setup if true" );
  declareProperty("printSummary",            m_printSummary = false, "Printing the summary if true" );

  declareProperty("inputCollectionList",     m_inCollKeyList, "List of input collection keys" );
  declareProperty("inputCollection1",        m_inCollKey1="", "Key of first input collection" );
  declareProperty("inputCollection2",        m_inCollKey2="", "Key of second input collection" );
  declareProperty("inputCollection3",        m_inCollKey3="", "Optional: key of third input collection" );

  declareProperty("outputCollection",        m_outCollKey="MyCompositeCollection" );

  declareProperty("mcEventCollection",       m_mcEventCollKey="GEN_AOD", "The name of the MCEventCollection" );

  declareProperty("compositeParticlePDG_ID", m_pdgId=0, "PDG_ID of composite particle" );

  declareProperty("minNumberPassed",         m_minNumberPassed=0,
                  "Min number of selected composite particles" );
  declareProperty("maxNumberPassed",         m_maxNumberPassed=INT_MAX,
                  "Max number of selected composite particles" );

  declareProperty("ptMin",                   m_ptMin=-1.0*DBL_MAX, "Min P_T" );
  declareProperty("ptMax",                   m_ptMax=DBL_MAX,      "Max P_T" );

  declareProperty("pMin",                    m_pMin=-1.0*DBL_MAX, "Min momentum" );
  declareProperty("pMax",                    m_pMax=DBL_MAX,      "Max momentum" );

  declareProperty("etMin",                   m_etMin=-DBL_MAX, "Min E_T" );
  declareProperty("etMax",                   m_etMax=DBL_MAX,  "Max E_T" );

  declareProperty("eMin",                    m_eMin=-DBL_MAX,  "Min E" );
  declareProperty("eMax",                    m_eMax=DBL_MAX,   "Max E" );

  declareProperty("etaMin",                  m_etaMin=-DBL_MAX, "Min eta" );
  declareProperty("etaMax",                  m_etaMax=DBL_MAX,  "Max eta" );

  declareProperty("absEtaMin",               m_absEtaMin=-DBL_MAX, "Min |eta|" );
  declareProperty("absEtaMax",               m_absEtaMax=DBL_MAX,  "Max |eta|" );

  declareProperty("etaVetoRanges",           m_etaVetoRanges="", "Eta-ranges to veto" );

  declareProperty("phiMin",                  m_phiMin=-DBL_MAX, "Min phi" );
  declareProperty("phiMax",                  m_phiMax=DBL_MAX,  "Max phi" );

  declareProperty("phiVetoRanges",           m_phiVetoRanges="", "Phi-ranges to veto" );

  declareProperty("massMin",                 m_massMin=-DBL_MAX, "Min mass" );
  declareProperty("massMax",                 m_massMax=DBL_MAX,  "Max mass" );

  declareProperty("kinematicSelectorTool",   m_kinSelectorTool, "Kinematic selection tool" );

  declareProperty("deltaEtaMin",             m_deltaEtaMin=-DBL_MAX, "Min deltaEta" );
  declareProperty("deltaEtaMax",             m_deltaEtaMax=DBL_MAX, "Max deltaEta" );

  declareProperty("deltaPhiMin",             m_deltaPhiMin=-DBL_MAX, "Min deltaPhi" );
  declareProperty("deltaPhiMax",             m_deltaPhiMax=DBL_MAX, "Max deltaPhi" );

  declareProperty("deltaRMin",               m_deltaRMin=-DBL_MAX, "Min deltaR" );
  declareProperty("deltaRMax",               m_deltaRMax=DBL_MAX, "Max deltaR" );

  declareProperty("chargeMin",               m_chargeMin=-INT_MAX,  "Min charge" );
  declareProperty("chargeMax",               m_chargeMax=INT_MAX,   "Max charge" );
  declareProperty("allowChargeConjugate",    m_allowChargeConj=true, "Allow charge conjugate" );

  declareProperty("mcTruthRequireSameMotherPdgID",
                  m_mcTruthRequireSameMotherPdgID=false,
                  "Require that the truth mother has the same PDG_ID defined with compositeParticlePDG_ID" );
  declareProperty("mcTruthRequireSameMotherBarcode",
                  m_mcTruthRequireSameMotherBarcode=false,
                  "Require that the truth mother has the identical MC Truth barcode" );

  declareProperty("iSelectorToolList",       m_selectionTools, "List of ISelector tools to be used for selection" );

  declareProperty("associationToolList",     m_associationTools, "List of AssociationTools to be used for matching" );
  declareProperty("outputAssociationContainerList", m_outAssoCollKeys,
                  "List of the names of the output association maps (INav4MomAssocs)" );

  declareProperty("userDataCalcToolList",    m_userDataCalcTools, "List of UserDataCalcTools to be used" );

  declareProperty("dumpStoreGate", m_dumpStoreGate=false, "Flag to dump StoreGate content after each event" );

  m_nEventsProcessed = 0 ;
}







//=============================================================================
// Destructor
//=============================================================================
D2PDParticleCombiner::~D2PDParticleCombiner()
{
}






//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode D2PDParticleCombiner::initialize()
{
  // Print the used configuration
  if ( m_printSetup || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "==> initialize " << name() << "..." );
      
      // Print out the used configuration
      ATH_MSG_INFO ( " using printSetup                      = " << m_printSetup );
      ATH_MSG_INFO ( " using printSummary                    = " << m_printSummary );

      ATH_MSG_INFO ( " using outputCollection                = " << m_outCollKey );

      ATH_MSG_INFO ( " using inputCollection1                = " << m_inCollKey1 );
      ATH_MSG_INFO ( " using inputCollection2                = " << m_inCollKey2 );
      ATH_MSG_INFO ( " using inputCollection3                = " << m_inCollKey3 );
      ATH_MSG_INFO ( " using inputCollectionList             = " << m_inCollKeyList );

      ATH_MSG_INFO ( " using mcEventCollection               = " << m_mcEventCollKey );

      ATH_MSG_INFO ( " using compositeParticlePDG_ID         = " << m_pdgId );

      ATH_MSG_INFO ( " using minNumberPassed                 = " << m_minNumberPassed );
      ATH_MSG_INFO ( " using maxNumberPassed                 = " << m_maxNumberPassed );

      ATH_MSG_INFO ( " using ptMin                           = " << m_ptMin );
      ATH_MSG_INFO ( " using ptMax                           = " << m_ptMax );

      ATH_MSG_INFO ( " using pMin                            = " << m_pMin );
      ATH_MSG_INFO ( " using pMax                            = " << m_pMax );

      ATH_MSG_INFO ( " using etMin                           = " << m_etMin );
      ATH_MSG_INFO ( " using etMax                           = " << m_etMax );

      ATH_MSG_INFO ( " using eMin                            = " << m_eMin );
      ATH_MSG_INFO ( " using eMax                            = " << m_eMax );

      ATH_MSG_INFO ( " using etaMin                          = " << m_etaMin );
      ATH_MSG_INFO ( " using etaMax                          = " << m_etaMax );
      ATH_MSG_INFO ( " using etaVetoRanges                   = " << m_etaVetoRanges );

      ATH_MSG_INFO ( " using phiMin                          = " << m_phiMin );
      ATH_MSG_INFO ( " using phiMax                          = " << m_phiMax );
      ATH_MSG_INFO ( " using phiVetoRanges                   = " << m_phiVetoRanges );

      ATH_MSG_INFO ( " using massMin                         = " << m_massMin );
      ATH_MSG_INFO ( " using massMax                         = " << m_massMax );

      ATH_MSG_INFO ( " using kinematicSelectorTool           = " << m_kinSelectorTool );

      ATH_MSG_INFO ( " using deltaEtaMin                     = " << m_deltaEtaMin );
      ATH_MSG_INFO ( " using deltaEtaMax                     = " << m_deltaEtaMax );

      ATH_MSG_INFO ( " using deltaPhiMin                     = " << m_deltaPhiMin );
      ATH_MSG_INFO ( " using deltaPhiMax                     = " << m_deltaPhiMax );

      ATH_MSG_INFO ( " using deltaRMin                       = " << m_deltaRMin );
      ATH_MSG_INFO ( " using deltaRMax                       = " << m_deltaRMax );

      ATH_MSG_INFO ( " using chargeMin                       = " << m_chargeMin );
      ATH_MSG_INFO ( " using chargeMax                       = " << m_chargeMax );
      ATH_MSG_INFO ( " using allowChargeConjugate            = " << m_allowChargeConj );

      ATH_MSG_INFO ( " using mcTruthRequireSameMotherPdgID   = " << m_mcTruthRequireSameMotherPdgID );
      ATH_MSG_INFO ( " using mcTruthRequireSameMotherBarcode = " << m_mcTruthRequireSameMotherBarcode );

      ATH_MSG_INFO ( " using iSelectorToolList               = " << m_selectionTools );

      ATH_MSG_INFO ( " using associationToolList             = " << m_associationTools );
      ATH_MSG_INFO ( " using outputAssociationContainerList  = " << m_outAssoCollKeys );

      ATH_MSG_INFO ( " using userDataCalcToolList            = " << m_userDataCalcTools );

      ATH_MSG_INFO ( " using dumpStoreGate                   = " << m_dumpStoreGate );
    } // End: if ( m_printSetup )



  // get the kinematic selector tool (derived class from ISelectorCore)
  ATH_CHECK( m_kinSelectorTool.retrieve() );

  // get the FilterTool 
  ATH_CHECK( toolSvc()->retrieveTool("FilterTool", m_filterTool) );

  // Get all the ISelector tools
  ATH_CHECK( m_selectionTools.retrieve() );

  // Get all the Association tools
  ATH_CHECK( m_associationTools.retrieve() );
  // Reserve the memory for the needed objects
  m_vecINav4MomAssocs.reserve( m_associationTools.size() );

  // Get all the UserDataCalcTools
  ATH_CHECK( m_userDataCalcTools.retrieve() );



  // Transfer the individual input keys to the vector of input keys...
  // This is for backwards compatibility
  if ( m_inCollKeyList.size() == 0 && !(m_inCollKey1.empty()) && !(m_inCollKey2.empty())  && !(m_inCollKey3.empty()) )
    {
      m_inCollKeyList.push_back( m_inCollKey1 );
      m_inCollKeyList.push_back( m_inCollKey2 );
      m_inCollKeyList.push_back( m_inCollKey3 );
    }
  else if ( m_inCollKeyList.size() == 0 && !(m_inCollKey1.empty()) && !(m_inCollKey2.empty())  && m_inCollKey3.empty() )
    {
      m_inCollKeyList.push_back( m_inCollKey1 );
      m_inCollKeyList.push_back( m_inCollKey2 );
    }
  else if ( m_inCollKeyList.size() >= 2 && m_inCollKey1.empty() && m_inCollKey2.empty()  && m_inCollKey3.empty() )
    {
      // This is expected
    }
  else
    {
      ATH_MSG_ERROR ( "Inconsistent configuration of the input container keys! Check your configuration:"
                      << " using inputCollection1    = " << m_inCollKey1
                      << " using inputCollection2    = " << m_inCollKey2
                      << " using inputCollection3    = " << m_inCollKey3
                      << " using inputCollectionList = " << m_inCollKeyList );
      return StatusCode::FAILURE;
    }




  // Do some sanity checks on the user configuration
  bool problemFound(false);
  if ( m_outCollKey.empty() )
    {
      ATH_MSG_ERROR ( "Empty output collection! Please configure it properly!" );
      problemFound = true;
    }
  if ( m_outAssoCollKeys.size() != m_associationTools.size() )
    {
      ATH_MSG_ERROR ( "The outputAssociationContainerList has size = " << m_outAssoCollKeys.size()
                      << " and the associationToolList has size = " << m_associationTools.size()
                      << " but they have to have the same size!"
                      << " Please configure it properly..." );
      problemFound = true;
    }
  if ( problemFound )
    {
      return StatusCode::FAILURE;
    }



  // Check if we have a two-body decay
  if ( m_inCollKeyList.size() == 2 )  
    {
      m_isTwoBodyDecay = true;
    }


  // Initialize the counters to zero
  m_nEventsProcessed = 0 ;


  //--------------------------------------------------------------------------
  // Set the cut values for the selection tools
  //--------------------------------------------------------------------------
  m_kinSelectorTool->getTool()->setminPt( m_ptMin );
  m_kinSelectorTool->getTool()->setmaxPt( m_ptMax );

  m_kinSelectorTool->getTool()->setminP( m_pMin );
  m_kinSelectorTool->getTool()->setmaxP( m_pMax );

  m_kinSelectorTool->getTool()->setminEt( m_etMin );
  m_kinSelectorTool->getTool()->setmaxEt( m_etMax );

  m_kinSelectorTool->getTool()->setminE( m_eMin );
  m_kinSelectorTool->getTool()->setmaxE( m_eMax );

  m_kinSelectorTool->getTool()->setminEta( m_etaMin );
  m_kinSelectorTool->getTool()->setmaxEta( m_etaMax );

  m_kinSelectorTool->getTool()->setminAbsEta( m_absEtaMin );
  m_kinSelectorTool->getTool()->setmaxAbsEta( m_absEtaMax );

  m_kinSelectorTool->getTool()->setEtaVetoRanges( m_etaVetoRanges );

  m_kinSelectorTool->getTool()->setminPhi( m_phiMin );
  m_kinSelectorTool->getTool()->setmaxPhi( m_phiMax );

  m_kinSelectorTool->getTool()->setPhiVetoRanges( m_phiVetoRanges );

  m_kinSelectorTool->getTool()->setminMass( m_massMin );
  m_kinSelectorTool->getTool()->setmaxMass( m_massMax );

  ATH_MSG_DEBUG ( "KinematicSelectorTool:" << m_kinSelectorTool );
  
  return StatusCode::SUCCESS;

}



//=============================================================================
// Athena execute method
//=============================================================================
StatusCode D2PDParticleCombiner::execute()
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
  // Retrieve and store the input containers
  //-----------------------------------------

  // Create some objects that are needed later on
  bool AllInputContainersAreFull = true;

  // Clear the vector of LinkContainers and a map of their names.
  //  use a map to determine if a given input container
  //  has already been seen, and simply point to the
  //  link container corresponding to  that already-used
  //  input container
  m_inputLinkContainerListDelete.clear();
  m_inputLinkContainerList.clear();
  m_inputLinkContainerNames.clear();
  m_alreadyUsedInputContainers.clear();

  // Loop over the list of input containers
  std::vector< std::string >::const_iterator inCollKeyList_begin(m_inCollKeyList.begin());
  std::vector< std::string >::const_iterator inCollKeyList_end(m_inCollKeyList.end());
  for( ; inCollKeyList_begin != inCollKeyList_end; inCollKeyList_begin++ )
    {
      std::string anInputCollName = *inCollKeyList_begin;

      // Check if the current input container name is a LinkContainer
      if ( evtStore()->contains< INav4MomLinkContainer >( anInputCollName ) )
        {
          // Actually retrieve the LinkContainer from StoreGate
          const INav4MomLinkContainer* aLinkContainer = NULL;
          ATH_CHECK( evtStore()->retrieve( aLinkContainer , anInputCollName ) );
          ATH_MSG_DEBUG ( "Input link collection = '" << anInputCollName
                          << "' retrieved from StoreGate which has " << aLinkContainer->size() << " entries." );

          if ( aLinkContainer->size() == 0 )
            {
              AllInputContainersAreFull &= false;
            }

          if ( m_inputLinkContainerNames.find( (INav4MomLinkContainer*) aLinkContainer ) == m_inputLinkContainerNames.end() )
            {
              m_inputLinkContainerNames[ (INav4MomLinkContainer*) aLinkContainer ] = anInputCollName;
            }
          m_inputLinkContainerList.push_back( (INav4MomLinkContainer*) aLinkContainer );
          m_inputLinkContainerListDelete.push_back( false );
        }
      else if ( evtStore()->contains< INavigable4MomentumCollection >( anInputCollName ) )
        {
          // This container holds and INavigable4MomentumCollection
          const INavigable4MomentumCollection* aContainer = NULL;
          ATH_CHECK( evtStore()->retrieve( aContainer, anInputCollName ) );
          ATH_MSG_DEBUG ( "Input collection = '" << anInputCollName
                          << "' retrieved from StoreGate which has " << aContainer->size() << " entries." );
          
          if (aContainer->size() == 0)
            {
              AllInputContainersAreFull &= false;
            }

          // Now, create the container of ElementLinks pointing to the real container
          if ( m_alreadyUsedInputContainers.find( (INavigable4MomentumCollection*) aContainer ) == m_alreadyUsedInputContainers.end() ) 
            {
              INav4MomLinkContainer* newLinkContainer = new INav4MomLinkContainer;
              newLinkContainer->reserve( aContainer->size() );

              for ( unsigned int i=0; i < aContainer->size(); i++ )
                {
                  INav4MomLink iNav4MomLink( *aContainer, i );
                  newLinkContainer->push_back( iNav4MomLink );
                }

              m_alreadyUsedInputContainers[ (INavigable4MomentumCollection*) aContainer ] = newLinkContainer;
	  

              if ( m_inputLinkContainerNames.find( newLinkContainer ) == m_inputLinkContainerNames.end() )
                {
                  m_inputLinkContainerNames[ newLinkContainer ] = anInputCollName;
                }
              // Add this new container of ElementLinks to the vector
              m_inputLinkContainerList.push_back( newLinkContainer );
              m_inputLinkContainerListDelete.push_back( true );
            } 
          else 
            {
              // Add an already existing container of ElementLinks again to the vector
              m_inputLinkContainerList.push_back( m_alreadyUsedInputContainers[ (INavigable4MomentumCollection*) aContainer ] );
              m_inputLinkContainerListDelete.push_back( false );
            }
        }
      else
        {
          if ( m_nEventsProcessed <= 10 )
            {
              ATH_MSG_WARNING ( "Input link collection  = '" << anInputCollName
                                << "' could not be retrieved from StoreGate! "
                                << " This message will only be repeated 10 times..." );
            }
          else
            {
              ATH_MSG_DEBUG ( "Input link collection  = '" << anInputCollName
                              << "' could not be retrieved from StoreGate! " );
            } 
          return StatusCode::SUCCESS;
        } // End: if/elif/else is link container
    } // End: Loop over the list of input containers
  

  // Double check if we have a two-body decay
  if ( m_inputLinkContainerList.size() == 2 )
    {
      m_isTwoBodyDecay = true;
      ATH_MSG_DEBUG ( "This is a two-body decay!" );
    }


  // One more sanity check that we didn't loose a container in between
  if ( m_inCollKeyList.size() != m_inputLinkContainerList.size() )  
    {
      ATH_MSG_ERROR ( "Inconsistent number of input containers after some processing! Exiting..." );
      return StatusCode::FAILURE;
    }


  //-----------------------------------------
  // Retrieve the MCEventCollection, if needed
  //-----------------------------------------
  if ( !m_mcEventCollKey.empty() && (
                                     m_mcTruthRequireSameMotherPdgID  ||
                                     m_mcTruthRequireSameMotherBarcode
                                     )
       )
    {
      ATH_CHECK( evtStore()->retrieve( m_mcEventColl, m_mcEventCollKey ) );
      ATH_MSG_DEBUG ( "MCEventCollection = '" << m_mcEventCollKey << "' retrieved from StoreGate" );
    } // End: need to retrieve MCEventCollection




  //-----------------------------------------
  // Create the output composite particle container and record it in StoreGate
  //-----------------------------------------
  CompositeParticleContainer* outContainer 
    = new CompositeParticleContainer( SG::OWN_ELEMENTS );  
  ATH_CHECK( evtStore()->record ( outContainer, m_outCollKey ) );



  //-----------------------------------------
  // Create the output INav4MomAssocs map container
  //-----------------------------------------
  // Empty the vector from the previous event
  m_vecINav4MomAssocs.clear();

  // Create the same amount of INav4MomAssocs maps as we have AssociationTools
  for ( unsigned int toolIdx=0; toolIdx < m_associationTools.size(); ++toolIdx )
    {
      if ( !(m_outAssoCollKeys[toolIdx]).empty() )
        {
          INav4MomAssocs* inav4MomAssocs = new INav4MomAssocs;
          ATH_CHECK( evtStore()->record ( inav4MomAssocs, m_outAssoCollKeys[toolIdx] ) );
          m_vecINav4MomAssocs.push_back( inav4MomAssocs );
        } // End: if ( m_outAssoCollKey != "" )
    } // End: Add all needed INav4MomAssocs to a vector




  //-----------------------------------------
  // In case at least one of the input containers has zero size,
  //  no CompositeParticle can be found. So only try building
  //  CompositeParticles if every input container has at least
  //  one entry.
  //-----------------------------------------
  if ( AllInputContainersAreFull )
    {

      //-----------------------------------------
      // Do the combinations of particles
      //-----------------------------------------
      
      // Use the odometer algorithm to perform the combination of particles
      //  m_inputLinkContainerList is the std::vector of links
      
      // get the label and number of elements in each link container.
      // pass those to the OdoMeter, and ask the OdoMeter to return 
      // the unique sets of indices for objects in the containers
      
      // To create the odometer object, we need to assign each container pointer
      // a string name that uniquely idenfies that container. The same container
      // (even used multiple times) will always have the same name. The name/entries 
      // info will be used to initialize the odometer.
      ATH_MSG_DEBUG ( "Input LinkContainer list contains " << m_inputLinkContainerList.size() << " entries." );

      // Clear needed vectors and maps
      m_containerLabels.clear();
      m_containerMaxN.clear();
      m_alreadyUsedContainers.clear();
  
      std::vector< INav4MomLinkContainer* >::const_iterator container_begin(m_inputLinkContainerList.begin());
      std::vector< INav4MomLinkContainer* >::const_iterator container_end(m_inputLinkContainerList.end());
      for ( ; container_begin != container_end; container_begin++ )
        {
          INav4MomLinkContainer* aLinkContainer = *container_begin;
          std::string containerName = m_inputLinkContainerNames[ aLinkContainer ];

          // determine if this container has already been used in the list. 
          // if not, assign it a unique name. If so, get the name of the existing container
          if ( m_containerLabels.size() == 0 || 
               m_alreadyUsedContainers.find( aLinkContainer ) == m_alreadyUsedContainers.end() )
            {
              m_containerLabels.push_back( containerName );
              m_containerMaxN[ containerName ] = aLinkContainer->size();
              m_alreadyUsedContainers[ aLinkContainer ] = containerName;
            }
          else
            {
              // already used this container. Use the old container's name
              m_containerLabels.push_back( containerName );
            }
          ATH_MSG_DEBUG ( "A container/size pair for the Odometer: "
                          << m_containerLabels.at( m_containerLabels.size() - 1) << "/"
                          << m_containerMaxN[ m_containerLabels.at( m_containerLabels.size() - 1) ] );
        }
  

      // Create and initialize an instance of the odometer
      OdoMeter anOdometer(m_containerLabels, m_containerMaxN);
  
      // find all unique groupings and build composites
      while ( anOdometer.increment() ) 
        {
          // Clear the vector
          m_anINav4MomLinkList.clear();

          // Get the current combinatoric 
          std::vector<int> aUniqueChoice = anOdometer.getVector();
          int nContainers = aUniqueChoice.size();

          if (msgLvl(MSG::DEBUG))
            {
              msg(MSG::DEBUG) << "Odometer - using this unique combination: ";
              for ( unsigned int ichoice = 0; ichoice < aUniqueChoice.size(); ichoice++ )
                {
                  msg(MSG::DEBUG)  << aUniqueChoice.at(ichoice) << ",";
                }
              msg(MSG::DEBUG) << endmsg;
            }

          // Loop over all containers
          for( int aContainerIndex = 0; aContainerIndex < nContainers; aContainerIndex++ )
            {
              INav4MomLinkContainer* aLinkContainer = m_inputLinkContainerList.at( aContainerIndex );
              int indexInContainer = aUniqueChoice.at( aContainerIndex );
          
              const INav4MomLink aNav4MomLink = aLinkContainer->at( indexInContainer );

              const INavigable4MomentumCollection* ptrINav4MomCollection =
                dynamic_cast< const INavigable4MomentumCollection* >( aNav4MomLink.getStorableObjectPointer() );

              if ( ptrINav4MomCollection != NULL )
                {
                  const INav4MomLink aParticleLink(*ptrINav4MomCollection, aNav4MomLink.index());
                  m_anINav4MomLinkList.push_back( aParticleLink );
                }
            } // end the loop over the available containers to build INav4MomLink objects
      
          // Now, actually build the CompositeParticle from the list of ElementLinks to INavigable4Momentum
          ATH_CHECK( buildComposite( outContainer, m_anINav4MomLinkList ) );

        } // End: while ( anOdometer.increment() )

    } // End: Found an empty input collection; thus no CompositeParticle can be possible... do nothing.



  //-----------------------------------------
  // Set the output container of composite particles as const
  //-----------------------------------------
  ATH_CHECK( evtStore()->setConst( outContainer ) );
  ATH_MSG_DEBUG ( "Output collection = '" << m_outCollKey << "' set to const in StoreGate" );
  


  //--------------------------------------------------------------------------
  // Now that all CompositeParticles for this event are found, loop over them
  // and try to find associations and also calculate UserData.
  //--------------------------------------------------------------------------
  // Get the CompoisteParticleContainer and loop over it
  for ( unsigned long i=0; i < outContainer->size(); ++i )
    {
      CompositeParticle* compPart = (*outContainer)[i];
      if ( compPart )
        {
          //--------------------------
          // Calculate the Associations (matching)
          //--------------------------
          for ( unsigned int toolIdx(0); toolIdx < m_associationTools.size(); ++toolIdx )
            {
              if ( !(m_outAssoCollKeys[toolIdx]).empty() )
                {
                  ATH_CHECK( m_associationTools[toolIdx]->calculateAssociations( outContainer,
                                                                             i,
                                                                             *(m_vecINav4MomAssocs[toolIdx]) ) );
                } // End: if asso key not empty
            } // End: loop over association tools

      
          //--------------------------
          // Calculate the UserData
          //--------------------------
          ToolHandleArray<IUserDataCalcTool>::iterator toolItr    = m_userDataCalcTools.begin();
          ToolHandleArray<IUserDataCalcTool>::iterator toolItrEnd = m_userDataCalcTools.end();
          for ( ; toolItr != toolItrEnd; ++toolItr )
            {
              ATH_CHECK( (*toolItr)->calculateElementUserData( compPart ) );
            }// End: loop over UserDataCalc tools
          
        } // End: if compPart
    } // End: Loop over CompositeParticles




  // Print a final message about the composite particles
  ATH_MSG_DEBUG ( "Found " << outContainer->size() << " composite particles in event number " << m_nEventsProcessed );
  


  //-----------------------------------------
  // Determine if this event is accepted
  //-----------------------------------------
  if ( outContainer->size() >= m_minNumberPassed
       && outContainer->size() <= m_maxNumberPassed )
    {
      if (msgLvl(MSG::DEBUG))
        {
          msg(MSG::DEBUG) 
            << " Event " << m_nEventsProcessed
            << " passed, found " << outContainer->size()
            << " composite particles."
            << " Required were minNumber=" << m_minNumberPassed
            << " and maxNumber=" << m_maxNumberPassed 
            << ". Names and sizes of input collections: "; 
          for ( unsigned int i(0); i < m_inputLinkContainerList.size(); ++i )
            {
              msg(MSG::DEBUG) 
                << m_inputLinkContainerNames[ m_inputLinkContainerList.at(i) ] 
                << ":" 
                << (m_inputLinkContainerList.at(i))->size()
                << ", " ;
            }
          msg(MSG::DEBUG) << endmsg;
        }
      setFilterPassed(true);
    }
  else
    {    
      if (msgLvl(MSG::DEBUG))
        {
          msg(MSG::DEBUG) 
            << " Event " << m_nEventsProcessed
            << " failed, found " << outContainer->size()
            << " composite particles."
            << " Required were minNumber=" << m_minNumberPassed
            << " and maxNumber=" << m_maxNumberPassed
            << ". Names and sizes of input collections: "; 
          for ( unsigned int i(0); i < m_inputLinkContainerList.size(); ++i )
            {
              msg(MSG::DEBUG) 
                << m_inputLinkContainerNames[ m_inputLinkContainerList.at(i) ] 
                << ":" 
                << (m_inputLinkContainerList.at(i))->size()
                << ", " ;
            }
          msg(MSG::DEBUG) << endmsg;
        }
      setFilterPassed(false);
    }


  // Output message: Dump all content of StoreGate
  if ( m_dumpStoreGate )
    {
      ATH_MSG_INFO ( "StoreGateDump: \n" << evtStore()->dump() );
    } // End: if ( m_dumpStoreGate )


  // Delete needed things
  if ( m_inputLinkContainerListDelete.size() != m_inputLinkContainerList.size() )
    {
      ATH_MSG_ERROR ( "Something went horrobly wrong! Sizes don't match up!" );
      return StatusCode::FAILURE;
    }
  for ( unsigned int i=0; i<m_inputLinkContainerList.size(); ++i )
    {
      if ( m_inputLinkContainerListDelete[i] )
        {
          delete m_inputLinkContainerList[i];
        }
    }

  
  return StatusCode::SUCCESS;
}





//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode D2PDParticleCombiner::finalize()
{
  // Output message 
  if ( m_printSummary || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "FINALIZING AFTER ALL EVENTS ARE PROCESSED" );
    }

  return StatusCode::SUCCESS;
}






//=============================================================================
// Building the composite particle
//=============================================================================
StatusCode D2PDParticleCombiner::buildComposite( CompositeParticleContainer* outContainer,
                                                 INav4MomLinkContainer& anINav4MomLinkList )
{
  // Check if the vector does not have zero size
  if ( anINav4MomLinkList.size() == 0 )
    {
      return StatusCode::SUCCESS;
    }

  // point to the INavigable4Momenta at each INav4MomLink
  std::vector< INavigable4Momentum* > theParticles;

  bool ParticlesAreValid = true;

  //Loop over all ElementLinks to INavigable4Momenta and get the INavigable4Momenta
  INav4MomLinkContainer::const_iterator particlelinks_begin(anINav4MomLinkList.begin());
  INav4MomLinkContainer::const_iterator particlelinks_end(anINav4MomLinkList.end());
  for ( ; particlelinks_begin != particlelinks_end; particlelinks_begin++ )
    {
      const INav4MomLink aParticleLink = *particlelinks_begin;

      // Check if this ElementLink is valid
      if ( aParticleLink.isValid() )
        {
          // Get the particle from the ElementLink
          const INavigable4Momentum* aParticle = dynamic_cast< const INavigable4Momentum* >(*aParticleLink);

          if (aParticle)
            {
              // determine if the particle shares all daughters in common with a previous particle
              // in the composite:
              std::vector< INavigable4Momentum* >::const_iterator theParticles_begin(theParticles.begin());
              std::vector< INavigable4Momentum* >::const_iterator theParticles_end(theParticles.end());
              for ( ; theParticles_begin != theParticles_end; theParticles_begin++ ) 
                {
                  if ( shareSameConstituents( aParticle, *theParticles_begin ) )
                    {
                      // Output message 
                      ATH_MSG_DEBUG ( "Found aParticle overlaps with another INavigable4Momentum in the composite!" );
                      ParticlesAreValid &= false;
                      break;
                    }
                }
              
              // push the particle onto the candidate's input list
              if ( ParticlesAreValid )
                {
                  theParticles.push_back( (INavigable4Momentum*) aParticle );
                }
            }
          else 
            {
              // Output message 
              ATH_MSG_DEBUG ( "Found non-valid particle at an ElementLink location!" );
              ParticlesAreValid &= false;
            } // check if aParticle exists at all
        }
      else
        {
          // Output message 
          ATH_MSG_DEBUG ( "Found non-valid ElementLink for a  particle!" );
          ParticlesAreValid &= false;
        } // check the link to a particle
      
      if (false == ParticlesAreValid)
        {
          break;
        }
    } // End: loop over vector of ElementLinks


  //-----------------------------------------
  // Do the combination
  //-----------------------------------------
  if (ParticlesAreValid)
    {
      // Iterate over all INavigable4Momentum to find the combined charge and find the dataType
      ParticleDataType::DataType compDataType( ParticleDataType::Data );
      int chargeCompPart(0);
      std::vector< INavigable4Momentum* >::const_iterator theParticles_begin(theParticles.begin());
      std::vector< INavigable4Momentum* >::const_iterator theParticles_end(theParticles.end());
      for ( ; theParticles_begin != theParticles_end; theParticles_begin++)
        {
          const IParticle* aParticle = dynamic_cast< const IParticle* >(*theParticles_begin);
          if ( aParticle )
            {
              compDataType = aParticle->dataType();
              if (aParticle->hasCharge())
                {
                  chargeCompPart += (int)aParticle->charge();
                } 
            }
        }

      // Change the PDG_ID depending on the found charge
      int pdgId = m_pdgId;
      if ( chargeCompPart < 0 && m_pdgId > 0 )
        {
          pdgId = -1*m_pdgId;
        }
      else if ( chargeCompPart > 0 && m_pdgId < 0 )
        {
          pdgId = -1*m_pdgId;
        }
      
      // Actually create the composite particle
      //--------------------------------------------------------------
      particlelinks_begin = anINav4MomLinkList.begin();
      particlelinks_end   = anINav4MomLinkList.end();
      CompositeParticle* compPart = new CompositeParticle;
      for ( ; particlelinks_begin != particlelinks_end; particlelinks_begin++ )
        {
          compPart->add( *particlelinks_begin );
        }
      compPart->set_charge( chargeCompPart );
      compPart->set_pdgId( (PDG::pidType)pdgId );
      compPart->set_dataType( compDataType );
          
      // Check if this composite particle has been found before
      if ( !compositeParticleAlreadyFound( outContainer, compPart ) )
        {      
          // Add this composite particle to the output container
          // if it passes all the selections
          //--------------------------------------------------------------
          bool passAll = true;
          
          // transverse momentum selection
          if ( passAll )
            {
              passAll = m_kinSelectorTool->getTool()->passPt( compPart );
            }
          // momentum selection
          if ( passAll )
            {
              passAll = m_kinSelectorTool->getTool()->passP( compPart );
            }
          // transverse energy selection
          if ( passAll )
            {
              passAll = m_kinSelectorTool->getTool()->passEt( compPart );
            }
          // energy selection
          if ( passAll )
            {
              passAll = m_kinSelectorTool->getTool()->passE( compPart );
            }
          // eta selection
          if ( passAll )
            {
              passAll = m_kinSelectorTool->getTool()->passEta( compPart );
            }
          // |eta| selection
          if ( passAll )
            {
              passAll = m_kinSelectorTool->getTool()->passAbsEta( compPart );
            }
          // eta veto selection
          if ( passAll )
            {
              passAll = m_kinSelectorTool->getTool()->passEtaVetoRanges( compPart );
            }
          // phi selection
          if ( passAll )
            {
              passAll = m_kinSelectorTool->getTool()->passPhi( compPart );
            }
          // phi veto selection
          if ( passAll )
            {
              passAll = m_kinSelectorTool->getTool()->passPhiVetoRanges( compPart );
            }
          // mass selection
          if ( passAll )
            {
              passAll = m_kinSelectorTool->getTool()->passMass( compPart );
            }
          
          const INavigable4Momentum* part1 = theParticles.at(0);
          const INavigable4Momentum* part2 = theParticles.at(1);

          // deltaEta selection
          if ( m_isTwoBodyDecay && passAll )
            {
              passAll = m_filterTool->absDeltaEtaFilter( part1, part2, 
                                                         m_deltaEtaMin,
                                                         m_deltaEtaMax );
            }
          // deltaPhi selection
          if ( m_isTwoBodyDecay && passAll )
            {
              passAll = m_filterTool->deltaPhiFilter( part1, part2, 
                                                      m_deltaPhiMin,
                                                      m_deltaPhiMax );
            }
          // deltaR selection
          if ( m_isTwoBodyDecay && passAll )
            {
              passAll = m_filterTool->deltaRFilter( part1, part2, 
                                                    m_deltaRMin,
                                                    m_deltaRMax );
            }
          // charge selection
          if ( passAll )
            {
              passAll = m_filterTool->chargeFilter( compPart,
                                                    m_chargeMin,
                                                    m_chargeMax,
                                                    m_allowChargeConj );
            }

          // MC Truth selection
          // ONLY run this if MCTruth information is available AND
          // MCTruth selections are configured!
          if ( !m_mcEventCollKey.empty() && (
                                             m_mcTruthRequireSameMotherPdgID  ||
                                             m_mcTruthRequireSameMotherBarcode
                                             )
               )
            {
              if ( passAll )
                {
                  passAll = mcTruthSelections( compPart );
                }
            }



          //----------------------------------------------------------
          // Now, execute the selections based on provided ISelector tools
          //----------------------------------------------------------
          if ( passAll )
            {
              // Loop over all ISelector tools that the user provided
              // and check if this inav is accepted by the ISelector tools
              ToolHandleArray<ISelector>::iterator toolItr    = m_selectionTools.begin();
              ToolHandleArray<ISelector>::iterator toolItrEnd = m_selectionTools.end();
              for ( ; toolItr != toolItrEnd; ++toolItr )
                {
                  if ( passAll )
                    {
                      passAll = passAll && (*toolItr)->getTool()->accept( compPart );
                    }
                } // End: loop over ISelector tools
            } // End: if passAll




          // Write out the composite particles if all cuts are passed
          if ( passAll )
            {
              outContainer->push_back( compPart );
            }
          else
            {
              delete compPart;
            }
        }
      else
        {
          // Output message 
          ATH_MSG_DEBUG ( "Found this composite particle already before..." );
          delete compPart;
        } // End: if compositeParticleAlreadyFound

    } // End: Saveguard check if all particles are valide

  return StatusCode::SUCCESS;
}






//=============================================================================
// Check if the composite particle at hand was already found before
//=============================================================================
bool D2PDParticleCombiner::compositeParticleAlreadyFound( CompositeParticleContainer* compContainer, 
                                                          CompositeParticle* compPart )
{
  // default return
  bool foundIdentical = false;
  
  // Count the number of constituents of the test particle
  int nConstituentsTest = (int)( compPart->navigableBase().size() );

  // Get the first input container and loop over it
  CompositeParticleContainer::const_iterator contItr    = compContainer->begin();
  CompositeParticleContainer::const_iterator contItrEnd = compContainer->end();
  for( ; contItr != contItrEnd; ++contItr )
    {
      // Check if an identical composite particle is already found
      if ( !foundIdentical )
        {
          // Get the INav4MomLink and INavigable4Momentum for this iterator
          const CompositeParticle* contCompPart = *contItr ;

          // Count the number of constituents of the reference particle
          int nConstituentsReference
            = (int)( contCompPart->navigableBase().size() );
	  
          // Check that both have the same number of constituents
          if ( nConstituentsTest==nConstituentsReference )
            {
              // Loop over all constituents of the composite particle to be tested
              bool allConstituentsSame = true;
              CompositeParticle::ConstituentsIter_t cpItr
                = compPart->constituents_begin();
              CompositeParticle::ConstituentsIter_t cpItrEnd
                = compPart->constituents_end();
              for( ; cpItr != cpItrEnd; ++cpItr )
                {
                  if ( allConstituentsSame
                       && contCompPart->contains(*cpItr) )
                    {
                      allConstituentsSame = true;
                    }
                  else
                    {
                      allConstituentsSame = false;
                    }
                } // End: loop over constituents
	      
              // Now, propagete the decission
              if ( allConstituentsSame )
                {
                  foundIdentical = true;
                }
              else
                {
                  foundIdentical = false;
                }
            } // End: if ( nConstituentsTest==nConstituentsReference )
          else
            {
              foundIdentical = false;
            } // End: if/else ( nConstituentsTest==nConstituentsReference )
        } // End: if ( !foundIdentical )
    } // End: loop over container

  // Output message 
  ATH_MSG_DEBUG ( "Checking if this composite particle was already found before..."
                  << " foundIdentical=" << foundIdentical );

  return foundIdentical;
}






//=============================================================================
// Check that two particles are not the same or, if they are 
// composite particles, that they don't share the same constitutents 
//=============================================================================
bool D2PDParticleCombiner::shareSameConstituents( const INavigable4Momentum* part1,
                                                  const INavigable4Momentum* part2 )
{
  const CompositeParticle* compPart1 = 
    dynamic_cast<const CompositeParticle*> (part1) ;
  const CompositeParticle* compPart2 = 
    dynamic_cast<const CompositeParticle*> (part2) ;

  // Neither of the two is a composite particle
  if ( !compPart1 && !compPart2 )
    {
      if ( part1->hasSameAthenaBarCodeExceptVersion(*part2) )
        {
          return true;
        }
      else
        {
          return false;
        }
    }
  // One of them is a composite
  else if ( compPart1 && !compPart2 )
    {
      return shareSameConstituents( part2, compPart1 );
    }
  else if ( !compPart1 && compPart2 )
    {
      return shareSameConstituents( part1, compPart2 );
    }
  // Both are composite candidates
  else if ( compPart1 && compPart2 )
    {
      return shareSameConstituents( compPart1, compPart2 );
    }

  return false;
}



//=============================================================================
// Check that two particles are not the same or, if they are 
// composite particles, that they don't share the same constitutents 
// -------------------------- HELPER ---------------------
//=============================================================================
bool D2PDParticleCombiner::shareSameConstituents( const INavigable4Momentum* part1,
                                                  const CompositeParticle* compPart2 )
{
  // Default return
  bool isConstituent = false;

  // Loop over all constituents of the composite particle to be tested
  CompositeParticle::ConstituentsIter_t cpItr =
    compPart2->constituents_begin();
  CompositeParticle::ConstituentsIter_t cpItrEnd =
    compPart2->constituents_end();
  for( ; cpItr != cpItrEnd; ++cpItr )
    {
      const INavigable4Momentum* part2 = dynamic_cast<const INavigable4Momentum*>(*cpItr);

      // Check if this constituent itself is a composite particle
      const CompositeParticle* constitCP = 
        dynamic_cast<const CompositeParticle*> (part2) ;

      if ( !constitCP )
        {
          isConstituent = part1->hasSameAthenaBarCodeExceptVersion(*part2);
        }
      else
        {
          isConstituent = shareSameConstituents( part1, constitCP );
        }
      if ( isConstituent )
        {
          return true;
        }
    } // End: loop over constituents
  
  
  return isConstituent;
}



//=============================================================================
// Check that two particles are not the same or, if they are 
// composite particles, that they don't share the same constitutents 
// -------------------------- HELPER ---------------------
//=============================================================================
bool D2PDParticleCombiner::shareSameConstituents( const CompositeParticle* compPart1,
                                                  const CompositeParticle* compPart2 )
{
  // Default return
  bool isConstituent = false;

  // Loop over all constituents of the composite particle to be tested
  CompositeParticle::ConstituentsIter_t cp1Itr =
    compPart1->constituents_begin();
  CompositeParticle::ConstituentsIter_t cp1ItrEnd =
    compPart1->constituents_end();
  for( ; cp1Itr != cp1ItrEnd; ++cp1Itr )
    {
      const INavigable4Momentum* part1 = dynamic_cast<const INavigable4Momentum*>(*cp1Itr);

      // Check if this constituent itself is a composite particle
      const CompositeParticle* constitCP1 = 
        dynamic_cast<const CompositeParticle*> (part1) ;

      CompositeParticle::ConstituentsIter_t cp2Itr
        = compPart2->constituents_begin();
      CompositeParticle::ConstituentsIter_t cp2ItrEnd
        = compPart2->constituents_end();
      for( ; cp2Itr != cp2ItrEnd; ++cp2Itr )
        {
          const INavigable4Momentum* part2 = dynamic_cast<const INavigable4Momentum*>(*cp2Itr);

          // Check if this constituent itself is a composite particle
          const CompositeParticle* constitCP2 = 
            dynamic_cast<const CompositeParticle*> (part2) ;

          if ( !constitCP1 && !constitCP2 )
            {
              isConstituent = part1->hasSameAthenaBarCodeExceptVersion(*part2);
            }
          if ( !constitCP1 && constitCP2 ) 
            {
              isConstituent = shareSameConstituents( part1, constitCP2 );
            }
          if ( constitCP1 && !constitCP2 ) 
            {
              isConstituent = shareSameConstituents( part2, constitCP1 );
            }
          if ( constitCP1 && constitCP2 ) 
            {
              isConstituent = shareSameConstituents( constitCP1, constitCP2 );
            }
          if ( isConstituent )
            {
              return true;
            }
        } // End: loop over constituents 2
    } // End: loop over constituents 1


  return isConstituent;
}


//=============================================================================
// Do the selection based on the MC Truth record
// -------------------------- HELPER ---------------------
//=============================================================================
bool D2PDParticleCombiner::mcTruthSelections( const CompositeParticle* compPart )
{
  // Default return
  bool isPassed = true;

  // If the MCEventCollection is not available, I can't do anything
  if ( !m_mcEventColl )
    {
      ATH_MSG_WARNING ( "No MCEventCollection available... skipping!" );
      return true;
    }


  // Get the daughters of the composite particle and check if all of them are TruthParticles
  unsigned int counter(0);
  std::vector<const TruthParticle*> truthParticles;
  CompositeParticle::ConstituentsIter_t cpItr    = compPart->constituents_begin();
  CompositeParticle::ConstituentsIter_t cpItrEnd = compPart->constituents_end();
  for( ; cpItr != cpItrEnd; ++cpItr )
    {
      ++counter;
      const TruthParticle* part = dynamic_cast<const TruthParticle*>(*cpItr);
      
      // if the cast was successfull, add it to the vector of truth particles
      if ( part )
        {
          truthParticles.push_back( part );
        }
    }


  // If all of the constituents are TruthParticles...
  if ( truthParticles.size() != counter )
    {
      ATH_MSG_WARNING ( "Not all of the constituents of this CompositeParticle are TruthParticles... skipping!" );
      return true;
    }


  //------------------------------------------------
  // Now, do the actual selections
  //------------------------------------------------

  std::vector<int> pdgIDList;
  std::vector<int> barcodeList;

  // Loop over the TruthParticles
  std::vector<const TruthParticle*>::const_iterator partItr    = truthParticles.begin();
  std::vector<const TruthParticle*>::const_iterator partItrEnd = truthParticles.end();
  for ( ; partItr != partItrEnd; ++partItr )
    {
      // Get the GenParticle from the TruthParticle
      const TruthParticle* part = (*partItr);
      const HepMC::GenParticle* genPart = part->genParticle();
      const int pdgID = genPart->pdg_id();
      
      // Now, get the origin of this generated particle
      McEventCollection::const_iterator mcEventItr = m_mcEventColl->begin();
      const int primaryBarcode = genPart->barcode()%1000000;
      const HepMC::GenParticle* primaryPart = (*mcEventItr)->barcode_to_particle(primaryBarcode);

      // Check that we really have the primaryPart
      if ( !primaryPart )
        {
          ATH_MSG_WARNING ( "Could not get the primaryParticle... skipping!" );
          return true;
        }

      // Now get the production vertex
      const HepMC::GenVertex*  prodVert = primaryPart->production_vertex();
      if ( !prodVert )
        {
          ATH_MSG_WARNING ( "Could not get the ProductionVertex... skipping!" );
          return true;
        }

      // Check that we have only one mother
      if ( prodVert->particles_in_size() > 1 )
        {
          ATH_MSG_WARNING ( "The ProductionVertex has more than one incomming particles... skipping!" );
          return true;
        }


      // Loop over the mother particles
      // Make sure that we ignore bremsstrahlung and decays into itself
      const HepMC::GenVertex* originVert = prodVert ;
      //const HepMC::GenVertex* tmpVert(0);
      int originPdgID = pdgID;
      int originBarcode(0);
      int counter(0);
      do
        {
          ++counter;
          HepMC::GenVertex::particles_in_const_iterator motherItr    = originVert->particles_in_const_begin();
          HepMC::GenVertex::particles_in_const_iterator motherItrEnd = originVert->particles_in_const_end();
          for ( ; motherItr != motherItrEnd; ++motherItr )
            {	     
              originPdgID   = (*motherItr)->pdg_id();
              originVert    = (*motherItr)->production_vertex();
              originBarcode = (*motherItr)->barcode();
            }

          // Protect against infinite loop
          if ( counter > 100 )
            {
              ATH_MSG_WARNING ( "Stuck in an infinite while loop... breaking out!" );
              break;
            }
        } while ( abs(originPdgID) == abs(pdgID) && originVert != 0 );

      // Attach the PDG_ID and barcode of the origin particle to the vectors
      pdgIDList.push_back( originPdgID );
      barcodeList.push_back( originBarcode );

    } // End: loop over all the daughter TruthParticles from the CompositeParticle

  // Get the decissions
  bool isSamePdgID(true);
  bool isSameBarcode(true);
  std::vector<int>::const_iterator pdgItr    = pdgIDList.begin();
  std::vector<int>::const_iterator pdgItrEnd = pdgIDList.end();
  for ( ; pdgItr != pdgItrEnd; ++pdgItr )
    {
      if ( m_pdgId != (*pdgItr) )
        {
          isSamePdgID = false;
        }
    } // End: Loop over pdg_ID list

  std::vector<int>::const_iterator barcodeItr    = barcodeList.begin();
  std::vector<int>::const_iterator barcodeItrEnd = barcodeList.end();
  for ( ; barcodeItr != barcodeItrEnd; ++barcodeItr )
    {
      if ( barcodeList[0] != (*barcodeItr) )
        {
          isSameBarcode = false;
        }
    } // End: Loop over barcode list



  // Now, do the final decission
  if ( m_mcTruthRequireSameMotherPdgID && !isSamePdgID )
    {
      isPassed = false;
    }
  if ( m_mcTruthRequireSameMotherBarcode && !isSameBarcode )
    {
      isPassed = false;
    }


  return isPassed;
}



//       const HepMC::GenVertex* MothVert(0);
//       long  NumOfParents(-1);
//       int MotherBarcode(0);
//       m_MotherPDG=0;
//       m_PhotonMotherPDG=0;

//       const HepMC::GenVertex* oriMothVert(0);
//       HepMC::GenVertex::particles_in_const_iterator itrMother = PriOriVert->particles_in_const_begin();
//       for ( ; itrMother != PriOriVert->particles_in_const_end();
//             ++itrMother)
//         {	     
//           m_MotherPDG   = (*itrMother)->pdg_id();
//           MothVert      = (*itrMother)->production_vertex();
//           oriMothVert   = (*itrMother)->production_vertex();
//           MotherBarcode = (*itrMother)->barcode();
//         }// cycle itrMother
	
//       // radiation in the final state
//       if ( abs(m_MotherPDG)==11 && PriBarcode<200000 && oriMothVert!=0 )
//         {
//           int itr=0;
//           do
//             {
//               PriOriVert = oriMothVert;
//               PriBarcode = MotherBarcode;
//               for (HepMC::GenVertex::particles_in_const_iterator
//                      itrMother = PriOriVert->particles_in_const_begin();
//                    itrMother!=PriOriVert->particles_in_const_end(); ++itrMother){
	     
//                 m_MotherPDG = (*itrMother)->pdg_id();
//                 MothVert    = (*itrMother)->production_vertex();
//                 oriMothVert = (*itrMother)->production_vertex();
//                 MotherBarcode=(*itrMother)->barcode();
//               }// cycle itrMother
//               itr++;
//               if(itr>100) {std::cout<<"infinite while"<<std::endl;break;}
//             }  while ( oriMothVert!=0 && abs(m_MotherPDG)==11 );
//         }

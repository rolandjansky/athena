/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Name: D2PDLeptonNeutrinoCombiner.cxx
//
/**
   @class D2PDLeptonNeutrinoCombiner

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Select events based on the missing Et and create a neutrino container.

*/
//=============================================================================

// This classes header
#include "D2PDMaker/D2PDLeptonNeutrinoCombiner.h"

// STL includes
#include <string>
#include <vector>
#include <cfloat>
#include <climits>

// Gaudi stuff
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

// Different particles inherit from this
#include "NavFourMom/INav4MomLink.h"
#include "NavFourMom/INav4MomLinkContainer.h"
#include "EventKernel/IParticle.h"

// The AthenaBarCode, used to check if two particles are identical
#include "Navigation/IAthenaBarCode.h"

// The composite particle
#include "ParticleEvent/CompositeParticle.h"
#include "ParticleEvent/CompositeParticleContainer.h"

// The needed ElementLink stuff
#include "DataModel/DataVector.h"
#include "DataModel/ElementLink.h" 

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
D2PDLeptonNeutrinoCombiner::D2PDLeptonNeutrinoCombiner(const std::string& name,
                                                       ISvcLocator* pSvcLocator) :
  AthFilterAlgorithm ( name,     pSvcLocator ),
  m_kinSelectorTool( "KinematicSelector/KinematicSelectorForLeptonNeutrinoCombiner", this ),
  m_neutrinoKinSelectorTool( "KinematicSelector/NeutrinoKinematicSelectorForLeptonNeutrinoCombiner", this ),
  m_missingEtKinSelectorTool( "KinematicSelector/MissingEtKinematicSelectorForLeptonNeutrinoCombiner", this )
{
  declareProperty("printSetup",              m_printSetup   = false, "Printing the setup if true" );
  declareProperty("printSummary",            m_printSummary = false, "Printing the summary if true" );

  declareProperty("kinematicSelectorTool",         m_kinSelectorTool, "Kinematic selection tool" );
  declareProperty("neutrinoKinematicSelectorTool", m_neutrinoKinSelectorTool, "Neutrino Kinematic selection tool" );
  declareProperty("missingEtKinematicSelectorTool", m_missingEtKinSelectorTool, "Missing Et Kinematic selection tool" );

  declareProperty("inputCollection",        m_inCollKey="", "Input missing Et collection" );
  declareProperty("inputLeptonCollection",  m_inLepCollKey="", "Input lepton (link) collection" );

  declareProperty("outputNeutrinoCollection",     m_outNeuCollKey="", "Output collection of Neutrinos" );
  declareProperty("outputNeutrinoLinkCollection", m_outNeuLinkCollKey="", "Output link collection of Neutrinos" );

  declareProperty("outputCollection",       m_outCollKey="",
                  "Output collection of the CompositeParticle, e.g., W boson" );

  declareProperty("WMass",                  m_wMass=80400.0,
                  "mass to be used for the mass constraint in MeV" );
  declareProperty("dealWithNegativeSolution",
                  m_dealWithNegSol=true,
                  "deal with possible negative solutions of the mass constraint" );

  declareProperty("minNumberPassed",        m_minNumberPassed=0,
                  "Min number of selected CompositeParticles, e.g., W bosons" );
  declareProperty("maxNumberPassed",        m_maxNumberPassed=INT_MAX,
                  "Max number of selected CompositeParticles, e.g., W bosons" );


  declareProperty("missingEtMin",           m_missingEtMin=-DBL_MAX, "Min missing E_T" );
  declareProperty("missingEtMax",           m_missingEtMax=DBL_MAX, "Max missing E_T" );

  declareProperty("sumEtMin",               m_sumEtMin=-DBL_MAX, "Min sumEt" );
  declareProperty("sumEtMax",               m_sumEtMax=DBL_MAX, "Max sumEt" );

  declareProperty("missingEtPhiMin",        m_missingEtPhiMin=-DBL_MAX, "MissingEt Min phi" );
  declareProperty("missingEtPhiMax",        m_missingEtPhiMax=DBL_MAX,  "MissingEt Max phi" );

  declareProperty("missingEtPhiVetoRanges", m_missingEtPhiVetoRanges="", "MissingEt Phi-ranges to veto" );


  declareProperty("neutrinoPtMin",          m_neutrinoPtMin=-DBL_MAX, "Neutrino Min P_T" );
  declareProperty("neutrinoPtMax",          m_neutrinoPtMax=DBL_MAX,  "Neutrino Max P_T" );

  declareProperty("neutrinoPMin",           m_neutrinoPMin=-DBL_MAX, "Neutrino Min P" );
  declareProperty("neutrinoPMax",           m_neutrinoPMax=DBL_MAX,  "Neutrino Max P" );

  declareProperty("neutrinoEtMin",          m_neutrinoEtMin=-DBL_MAX, "Neutrino Min E_T" );
  declareProperty("neutrinoEtMax",          m_neutrinoEtMax=DBL_MAX,  "Neutrino Max E_T" );

  declareProperty("neutrinoEMin",           m_neutrinoEMin=-DBL_MAX,  "Neutrino Min E" );
  declareProperty("neutrinoEMax",           m_neutrinoEMax=DBL_MAX,   "Neutrino Max E" );

  declareProperty("neutrinoEtaMin",         m_neutrinoEtaMin=-DBL_MAX, "Neutrino Min eta" );
  declareProperty("neutrinoEtaMax",         m_neutrinoEtaMax=DBL_MAX,  "Neutrino Max eta" );

  declareProperty("neutrinoEtaVetoRanges",  m_neutrinoEtaVetoRanges="", "Neutrino Eta-ranges to veto" );

  declareProperty("neutrinoPhiMin",         m_neutrinoPhiMin=-DBL_MAX, "Neutrino Min phi" );
  declareProperty("neutrinoPhiMax",         m_neutrinoPhiMax=DBL_MAX,  "Neutrino Max phi" );

  declareProperty("neutrinoPhiVetoRanges",  m_neutrinoPhiVetoRanges="", "Neutrino Phi-ranges to veto" );


  declareProperty("compositeParticlePDG_ID", m_pdgId=0, "PDG_ID of composite particle" );

  declareProperty("ptMin",                   m_ptMin=-DBL_MAX, "Min P_T" );
  declareProperty("ptMax",                   m_ptMax=DBL_MAX,  "Max P_T" );

  declareProperty("pMin",                    m_pMin=-DBL_MAX,  "Min P" );
  declareProperty("pMax",                    m_pMax=DBL_MAX,   "Max P" );

  declareProperty("etMin",                   m_etMin=-DBL_MAX, "Min E_T" );
  declareProperty("etMax",                   m_etMax=DBL_MAX,  "Max E_T" );

  declareProperty("eMin",                    m_eMin=-DBL_MAX,  "Min E" );
  declareProperty("eMax",                    m_eMax=DBL_MAX,   "Max E" );

  declareProperty("etaMin",                  m_etaMin=-DBL_MAX, "Min eta" );
  declareProperty("etaMax",                  m_etaMax=DBL_MAX,  "Max eta" );

  declareProperty("etaVetoRanges",           m_etaVetoRanges="", "Eta-ranges to veto" );

  declareProperty("phiMin",                  m_phiMin=-DBL_MAX, "Min phi" );
  declareProperty("phiMax",                  m_phiMax=DBL_MAX,  "Max phi" );

  declareProperty("phiVetoRanges",           m_phiVetoRanges="", "Phi-ranges to veto" );

  declareProperty("massMin",                 m_massMin=-DBL_MAX, "Min mass" );
  declareProperty("massMax",                 m_massMax=DBL_MAX,  "Max mass" );

  declareProperty("deltaEtaMin",             m_deltaEtaMin=-DBL_MAX, "Min deltaEta" );
  declareProperty("deltaEtaMax",             m_deltaEtaMax=DBL_MAX, "Max deltaEta" );

  declareProperty("deltaPhiMin",             m_deltaPhiMin=-DBL_MAX, "Min deltaPhi" );
  declareProperty("deltaPhiMax",             m_deltaPhiMax=DBL_MAX, "Max deltaPhi" );

  declareProperty("deltaRMin",               m_deltaRMin=-DBL_MAX, "Min deltaR" );
  declareProperty("deltaRMax",               m_deltaRMax=DBL_MAX, "Max deltaR" );

  declareProperty("chargeMin",               m_chargeMin=-INT_MAX,  "Min charge" );
  declareProperty("chargeMax",               m_chargeMax=INT_MAX,   "Max charge" );
  declareProperty("allowChargeConjugate",    m_allowChargeConj=true, "Allow charge conjugate" );


  declareProperty("dumpStoreGate",          m_dumpStoreGate=false, 
                  "Flag to dump StoreGate content after each event" );

  // Initialize the counters to zero
  m_nEventsProcessed = 0 ;
}



//=============================================================================
// Destructor
//=============================================================================
D2PDLeptonNeutrinoCombiner::~D2PDLeptonNeutrinoCombiner()
{
}



//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode D2PDLeptonNeutrinoCombiner::initialize()
{
  // Print the used configuration
  if ( m_printSetup || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "==> initializeMore " << name() << "..." );
      
      // Print out the used configuration
      ATH_MSG_INFO ( " using printSetup                     = " << m_printSetup );
      ATH_MSG_INFO ( " using printSummary                   = " << m_printSummary );

      ATH_MSG_INFO ( " using inputCollection                = " << m_inCollKey );
      ATH_MSG_INFO ( " using inputLeptonCollection          = " << m_inLepCollKey );

      ATH_MSG_INFO ( " using outputNeutrinoCollection       = " << m_outNeuCollKey );
      ATH_MSG_INFO ( " using outputNeutrinoLinkCollection   = " << m_outNeuLinkCollKey );

      ATH_MSG_INFO ( " using outputCollection               = " << m_outCollKey );

      ATH_MSG_INFO ( " using kinematicSelectorTool          = " << m_kinSelectorTool );
      ATH_MSG_INFO ( " using neutrinoKinematicSelectorTool  = " << m_neutrinoKinSelectorTool );
      ATH_MSG_INFO ( " using missingEtKinematicSelectorTool = " << m_missingEtKinSelectorTool );

      ATH_MSG_INFO ( " using minNumberPassed                = " << m_minNumberPassed );
      ATH_MSG_INFO ( " using maxNumberPassed                = " << m_maxNumberPassed );


      ATH_MSG_INFO ( " using missingEtMin                   = " << m_missingEtMin );
      ATH_MSG_INFO ( " using missingEtMax                   = " << m_missingEtMax );

      ATH_MSG_INFO ( " using sumEtMin                       = " << m_sumEtMin );
      ATH_MSG_INFO ( " using sumEtMax                       = " << m_sumEtMax );

      ATH_MSG_INFO ( " using missingEtPhiMin                = " << m_missingEtPhiMin );
      ATH_MSG_INFO ( " using missingEtPhiMax                = " << m_missingEtPhiMax );

      ATH_MSG_INFO ( " using missingEtPhiVetoRanges         = " << m_missingEtPhiVetoRanges );

      
      ATH_MSG_INFO ( " using neutrinoPtMin                  = " << m_neutrinoPtMin );
      ATH_MSG_INFO ( " using neutrinoPtMax                  = " << m_neutrinoPtMax );

      ATH_MSG_INFO ( " using neutrinoPMin                   = " << m_neutrinoPMin );
      ATH_MSG_INFO ( " using neutrinoPMax                   = " << m_neutrinoPMax );

      ATH_MSG_INFO ( " using neutrinoEtMin                  = " << m_neutrinoEtMin );
      ATH_MSG_INFO ( " using neutrinoEtMax                  = " << m_neutrinoEtMax );

      ATH_MSG_INFO ( " using neutrinoEMin                   = " << m_neutrinoEMin );
      ATH_MSG_INFO ( " using neutrinoEMax                   = " << m_neutrinoEMax );

      ATH_MSG_INFO ( " using neutrinoEtaMin                 = " << m_neutrinoEtaMin );
      ATH_MSG_INFO ( " using neutrinoEtaMax                 = " << m_neutrinoEtaMax );

      ATH_MSG_INFO ( " using neutrinoEtaVetoRanges          = " << m_neutrinoEtaVetoRanges );

      ATH_MSG_INFO ( " using neutrinoPhiMin                 = " << m_neutrinoPhiMin );
      ATH_MSG_INFO ( " using neutrinoPhiMax                 = " << m_neutrinoPhiMax );

      ATH_MSG_INFO ( " using neutrinoPhiVetoRanges          = " << m_neutrinoPhiVetoRanges );


      ATH_MSG_INFO ( " using compositeParticlePDG_ID        = " << m_pdgId );

      ATH_MSG_INFO ( " using ptMin                          = " << m_ptMin );
      ATH_MSG_INFO ( " using ptMax                          = " << m_ptMax );

      ATH_MSG_INFO ( " using pMin                           = " << m_pMin );
      ATH_MSG_INFO ( " using pMax                           = " << m_pMax );

      ATH_MSG_INFO ( " using etMin                          = " << m_etMin );
      ATH_MSG_INFO ( " using etMax                          = " << m_etMax );

      ATH_MSG_INFO ( " using eMin                           = " << m_eMin );
      ATH_MSG_INFO ( " using eMax                           = " << m_eMax );

      ATH_MSG_INFO ( " using etaMin                         = " << m_etaMin );
      ATH_MSG_INFO ( " using etaMax                         = " << m_etaMax );

      ATH_MSG_INFO ( " using etaVetoRanges                  = " << m_etaVetoRanges );

      ATH_MSG_INFO ( " using phiMin                         = " << m_phiMin );
      ATH_MSG_INFO ( " using phiMax                         = " << m_phiMax );

      ATH_MSG_INFO ( " using phiVetoRanges                  = " << m_phiVetoRanges );

      ATH_MSG_INFO ( " using massMin                        = " << m_massMin );
      ATH_MSG_INFO ( " using massMax                        = " << m_massMax );

      ATH_MSG_INFO ( " using deltaEtaMin                    = " << m_deltaEtaMin );
      ATH_MSG_INFO ( " using deltaEtaMax                    = " << m_deltaEtaMax );

      ATH_MSG_INFO ( " using deltaPhiMin                    = " << m_deltaPhiMin );
      ATH_MSG_INFO ( " using deltaPhiMax                    = " << m_deltaPhiMax );

      ATH_MSG_INFO ( " using deltaRMin                      = " << m_deltaRMin );
      ATH_MSG_INFO ( " using deltaRMax                      = " << m_deltaRMax );

      ATH_MSG_INFO ( " using chargeMin                      = " << m_chargeMin );
      ATH_MSG_INFO ( " using chargeMax                      = " << m_chargeMax );
      ATH_MSG_INFO ( " using allowChargeConjugate           = " << m_allowChargeConj );


      ATH_MSG_INFO ( " using dumpStoreGate                  = " << m_dumpStoreGate );
    } // End: if ( m_printSetup )


  
  // get the kinematic selector tool (derived class from ISelectorCore)
  ATH_CHECK( m_kinSelectorTool.retrieve() );

  // get the kinematic selector tool for neutrinos (derived class from ISelectorCore)
  ATH_CHECK( m_neutrinoKinSelectorTool.retrieve() );

  // get the kinematic selector tool for missing et (derived class from ISelectorCore)
  ATH_CHECK( m_missingEtKinSelectorTool.retrieve() );

  // get the FilterTool 
  ATH_CHECK( toolSvc()->retrieveTool("FilterTool", m_filterTool) );



  // Do some sanity checks on the user configuration
  if ( m_inCollKey.empty() )
    {
      ATH_MSG_ERROR ( "Empty input collection! Please configure it properly!" );
      return StatusCode::FAILURE ;
    }
  if ( m_outNeuCollKey.empty() )
    {
      ATH_MSG_ERROR ( "Empty output collection! The output collection key is REQUIRED!"
                      << " Please configure it properly!" );
      return StatusCode::FAILURE ;
    }
  if ( m_inLepCollKey.empty() )
    {
      ATH_MSG_ERROR ( " No lepton input (link) collection is specified! Please configure it properly!" );
      return StatusCode::FAILURE ;
    }



  
  //--------------------------------------------------------------------------
  // Set the cut values for the selection tools
  //--------------------------------------------------------------------------


  // for missing et
  //-----------------
  m_missingEtKinSelectorTool->getTool()->setminPhi( m_missingEtPhiMin );
  m_missingEtKinSelectorTool->getTool()->setmaxPhi( m_missingEtPhiMax );

  m_missingEtKinSelectorTool->getTool()->setPhiVetoRanges( m_missingEtPhiVetoRanges );



  // for neutrinos
  //-----------------
  m_neutrinoKinSelectorTool->getTool()->setminPt( m_neutrinoPtMin );
  m_neutrinoKinSelectorTool->getTool()->setmaxPt( m_neutrinoPtMax );

  m_neutrinoKinSelectorTool->getTool()->setminP( m_neutrinoPMin );
  m_neutrinoKinSelectorTool->getTool()->setmaxP( m_neutrinoPMax );

  m_neutrinoKinSelectorTool->getTool()->setminEt( m_neutrinoEtMin );
  m_neutrinoKinSelectorTool->getTool()->setmaxEt( m_neutrinoEtMax );

  m_neutrinoKinSelectorTool->getTool()->setminE( m_neutrinoEMin );
  m_neutrinoKinSelectorTool->getTool()->setmaxE( m_neutrinoEMax );

  m_neutrinoKinSelectorTool->getTool()->setminEta( m_neutrinoEtaMin );
  m_neutrinoKinSelectorTool->getTool()->setmaxEta( m_neutrinoEtaMax );

  m_neutrinoKinSelectorTool->getTool()->setEtaVetoRanges( m_neutrinoEtaVetoRanges );

  m_neutrinoKinSelectorTool->getTool()->setminPhi( m_neutrinoPhiMin );
  m_neutrinoKinSelectorTool->getTool()->setmaxPhi( m_neutrinoPhiMax );

  m_neutrinoKinSelectorTool->getTool()->setPhiVetoRanges( m_neutrinoPhiVetoRanges );



  // for the composite particles
  //-----------------
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

  m_kinSelectorTool->getTool()->setEtaVetoRanges( m_etaVetoRanges );

  m_kinSelectorTool->getTool()->setminPhi( m_phiMin );
  m_kinSelectorTool->getTool()->setmaxPhi( m_phiMax );

  m_kinSelectorTool->getTool()->setPhiVetoRanges( m_phiVetoRanges );

  m_kinSelectorTool->getTool()->setminMass( m_massMin );
  m_kinSelectorTool->getTool()->setmaxMass( m_massMax );



  // Create the temporary neutrino object
  m_tmpNeutrino = new Neutrino;


  // Initialize the counters to zero
  m_nEventsProcessed = 0 ;
  
  return StatusCode::SUCCESS;

}



//=============================================================================
// Athena execute method
//=============================================================================
StatusCode D2PDLeptonNeutrinoCombiner::execute()
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
  // Retrieve the input lepton container
  // to be used for the mass constaint
  //-----------------------------------------
  const INavigable4MomentumCollection* inLepContainer(0);
  const INav4MomLinkContainer* inLepLinkContainer(0);
  if ( !(m_inLepCollKey.empty()) )
    {
      if ( evtStore()->contains< INav4MomLinkContainer >( m_inLepCollKey ) )
        {
          ATH_CHECK( evtStore()->retrieve( inLepLinkContainer, m_inLepCollKey ) );
          ATH_MSG_DEBUG ( "Input lepton link collection = '" << m_inLepCollKey
                          << "' retrieved from StoreGate" );
        }
      else if ( evtStore()->contains< INavigable4MomentumCollection >( m_inLepCollKey ) )
        {
          ATH_CHECK( evtStore()->retrieve( inLepContainer, m_inLepCollKey ) );
          ATH_MSG_DEBUG ( "Input lepton collection = '" << m_inLepCollKey );
        }
      else
        {
          if ( m_nEventsProcessed <= 10 )
            {
              ATH_MSG_WARNING ( "Input lepton (link)collection = '" << m_inLepCollKey
                                << "' could not be retrieved from StoreGate! "
                                << " This message will only be repeated 10 times..." );
            }
          else
            {
              ATH_MSG_DEBUG ( "Input lepton (link)collection = '" << m_inLepCollKey
                              << "' could not be retrieved from StoreGate! " );
            }
          return StatusCode::SUCCESS;
        } // End: if/elif/else is lepton link container
    } // End: if m_inLepCollKey != ""


  //-----------------------------------------
  // Create link container for the lepton input
  // if no link container was retreived
  //-----------------------------------------
  bool mustDeleteLepLinkCont(false);
  if ( inLepContainer && !inLepLinkContainer )
    {
      // Create a new link container
      INav4MomLinkContainer* tmpLinkContainer = new INav4MomLinkContainer;
      mustDeleteLepLinkCont = true;

      // Loop over the lepton input container
      for ( unsigned int i=0; i < inLepContainer->size(); ++i )
        {
          INav4MomLink iNav4MomLink( *inLepContainer, i );
          tmpLinkContainer->push_back( iNav4MomLink );
        } // End: for loop over input lepton container
      inLepLinkContainer = tmpLinkContainer ;
      tmpLinkContainer = 0;
    } // End: if ( inLepContainer && !inLepLinkContainer )




  //-----------------------------------------
  // Create the output container for selected 
  // neutrinos and record it in StoreGate
  //-----------------------------------------
  NeutrinoContainer* outNeutrinoContainer = 
    new NeutrinoContainer( SG::OWN_ELEMENTS );  
  ATH_CHECK( evtStore()->record ( outNeutrinoContainer, m_outNeuCollKey ) );
  ATH_MSG_DEBUG ( "Output collection = '" << m_outNeuCollKey << "' recorded into StoreGate" );


  //-----------------------------------------
  // Create the output container for the ElementLinks
  // to the selected neutrinos and record it in StoreGate
  //-----------------------------------------
  INav4MomLinkContainer* outNeutrinoLinkContainer(NULL);
  if ( !(m_outNeuLinkCollKey.empty()) )
    {
      outNeutrinoLinkContainer = new INav4MomLinkContainer();
      ATH_CHECK( evtStore()->record ( outNeutrinoLinkContainer, m_outNeuLinkCollKey ) );
      ATH_MSG_DEBUG ( "Output link collection = '" << m_outNeuLinkCollKey << "' recorded into StoreGate" );
    } // End: if ( m_outNeuLinkCollKey != "" )



  //-----------------------------------------
  // Create the output composite particle container and record it in StoreGate
  //-----------------------------------------
  CompositeParticleContainer* outContainer 
    = new CompositeParticleContainer( SG::OWN_ELEMENTS );  
  ATH_CHECK( evtStore()->record ( outContainer, m_outCollKey ) );
  ATH_MSG_DEBUG ( "Output collection = '" << m_outCollKey << "' recorded into StoreGate" );




  //-----------------------------------------
  // Create the neutrino object
  //-----------------------------------------
  double etMiss  = inMissingEtObject->et();
  double etxMiss = inMissingEtObject->etx();
  double etyMiss = inMissingEtObject->ety();
  double sumEt   = inMissingEtObject->sumet();


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
  // Use the temporary neutrino
  m_tmpNeutrino->set4Mom(HepLorentzVector( etxMiss, etyMiss, 0.0, etMiss ));
  // phi selection
  if ( passAll )
    {
      passAll = m_missingEtKinSelectorTool->getTool()->passPhi( m_tmpNeutrino );
    }
  // phi veto selection
  if ( passAll )
    {
      passAll = m_missingEtKinSelectorTool->getTool()->passPhiVetoRanges( m_tmpNeutrino );
    }


  //----------------------------------------------------------
  // create neutrino(s) with pz!=0 using a mass constraint if all cuts are passed
  //----------------------------------------------------------
  if ( passAll )
    {
      // Loop over the lepton container
      if ( inLepLinkContainer )
        {

          // Get the input lepton container and loop over it
          INav4MomLinkContainer::const_iterator inLepItr
            = inLepLinkContainer->begin();
          INav4MomLinkContainer::const_iterator inLepItrEnd
            = inLepLinkContainer->end();
          unsigned int countOne(0);
          for( ; inLepItr != inLepItrEnd; ++inLepItr )
            {
              ++countOne;
              // Get the INav4MomLink for this iterator
              const INav4MomLink nav4momLepLink = *inLepItr ;

              // Check if the link points to a container that can be cast to an INavigable4MomentumCollection
              const INavigable4MomentumCollection* tmpCont(NULL);
              if ( nav4momLepLink.isValid() )
                {
                  // Get the object for this iterator
                  const INavigable4Momentum* lep = (*nav4momLepLink);

                  // Use the temporary INavigable4MomentumCollection
                  tmpCont = dynamic_cast< const INavigable4MomentumCollection* >( nav4momLepLink.getStorableObjectPointer() );
                  if ( tmpCont != NULL )
                    {
                      // Now, go ahead and find the neutrinos that satisfy the Boson Mass Constraint
                      if ( lep && outNeutrinoContainer )
                        {
                          // Create a temporary neutrino container
                          NeutrinoContainer* tmpNeutrinoContainer = new NeutrinoContainer(SG::VIEW_ELEMENTS);

                          // Do the actual mass constraint neutrino finding
                          bool findNeutrino = NeutrinoUtils::candidatesFromWMass( lep,
                                                                                  etxMiss, 
                                                                                  etyMiss,
                                                                                  (*tmpNeutrinoContainer),
                                                                                  m_dealWithNegSol );
                          if ( findNeutrino )
                            {
                              while ( !tmpNeutrinoContainer->empty() )
                                {
                                  Neutrino* neutrino = tmpNeutrinoContainer->back();
                                  tmpNeutrinoContainer->pop_back();
                                  if ( neutrino )
                                    {
                                      // Check if this neutrino passes the selection criteria

                                      // transverse momentum selection
                                      if ( passAll )
                                        {
                                          passAll = m_neutrinoKinSelectorTool->getTool()->passPt( neutrino );
                                        }
                                      // momentum selection
                                      if ( passAll )
                                        {
                                          passAll = m_neutrinoKinSelectorTool->getTool()->passP( neutrino );
                                        }
                                      // transverse energy selection
                                      if ( passAll )
                                        {
                                          passAll = m_neutrinoKinSelectorTool->getTool()->passEt( neutrino );
                                        }
                                      // energy selection
                                      if ( passAll )
                                        {
                                          passAll = m_neutrinoKinSelectorTool->getTool()->passE( neutrino );
                                        }
                                      // eta selection
                                      if ( passAll )
                                        {
                                          passAll = m_neutrinoKinSelectorTool->getTool()->passEta( neutrino );
                                        }
                                      // eta veto selection
                                      if ( passAll )
                                        {
                                          passAll = m_neutrinoKinSelectorTool->getTool()->passEtaVetoRanges( neutrino );
                                        }
                                      // phi selection
                                      if ( passAll )
                                        {
                                          passAll = m_neutrinoKinSelectorTool->getTool()->passPhi( neutrino );
                                        }
                                      // phi veto selection
                                      if ( passAll )
                                        {
                                          passAll = m_neutrinoKinSelectorTool->getTool()->passPhiVetoRanges( neutrino );
                                        }

                                      // Add the neutrinos that pass all selections 
                                      // to the output container
                                      if ( selectNeutrinos(neutrino) )
                                        {
                                          outNeutrinoContainer->push_back( neutrino );
                                        }
                                      else
                                        {
                                          delete neutrino;
                                        }

                                      // Now, build the composite particle from the lepton on the neutrino
                                      CompositeParticle* compPart = new CompositeParticle;
                                      const INav4MomLink leptonLink( *tmpCont, nav4momLepLink.index() );
                                      const INav4MomLink neutrinoLink( *outNeutrinoContainer, (outNeutrinoContainer->size()-1) );
                                      compPart->add( leptonLink, neutrinoLink );
                                      // Set the charge
                                      const INavigable4Momentum* tmpLep =  dynamic_cast< const INavigable4Momentum* >( lep );
                                      int charge(0);
                                      if ( tmpLep )
                                        {
                                          const IParticle* aParticle = dynamic_cast< const IParticle* >(tmpLep);
                                          if ( aParticle )
                                            {
                                              if ( aParticle->hasCharge() )
                                                {
                                                  charge = (int)aParticle->charge();
                                                }
                                              compPart->set_dataType( aParticle->dataType() );
                                            }
                                        }
                                      else
                                        {
                                          ATH_MSG_WARNING ( "Couldn't cast to INavigable4Momentum!" );
                                        }
                                      compPart->set_charge( charge );
                                      // Change the PDG_ID depending on the found charge
                                      int pdgId = m_pdgId;
                                      if ( charge < 0 && m_pdgId > 0 )
                                        {
                                          pdgId = -1*m_pdgId;
                                        }
                                      compPart->set_pdgId( (PDG::pidType)pdgId );
                                      
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

                                      if ( tmpLep )
                                        {
                                          // deltaEta selection
                                          if ( passAll )
                                            {
                                              passAll = m_filterTool->absDeltaEtaFilter( tmpLep, neutrino, 
                                                                                         m_deltaEtaMin,
                                                                                         m_deltaEtaMax );
                                            }
                                          // deltaPhi selection
                                          if ( passAll )
                                            {
                                              passAll = m_filterTool->deltaPhiFilter( tmpLep, neutrino, 
                                                                                      m_deltaPhiMin,
                                                                                      m_deltaPhiMax );
                                            }
                                          // deltaR selection
                                          if ( passAll )
                                            {
                                              passAll = m_filterTool->deltaRFilter( tmpLep, neutrino, 
                                                                                    m_deltaRMin,
                                                                                    m_deltaRMax );
                                            }
                                        }
                                      // charge selection
                                      if ( passAll )
                                        {
                                          passAll = m_filterTool->chargeFilter( compPart,
                                                                                m_chargeMin,
                                                                                m_chargeMax,
                                                                                m_allowChargeConj );
                                        }


                                      // Write out the composite particles if all cuts are passed
                                      if ( passAll )
                                        {
                                          outContainer->push_back( compPart );
                                        }
                                      else
                                        {
                                          delete compPart;
                                        }

                                    } // End: if ( neutrino ) 
                                } // End: loop over neutrino container
                            } // End: if ( findNeutrino ) 

                          // Delete temoprary container
                          delete tmpNeutrinoContainer;

                        } // End: if ( lep )
                    } //End: if cast to INavigable4MomentumCollection successful
                } // End:  if ( nav4momLepLink.isValid() )
            } // End: loop over INav4MomLinkContainer
        } // End: if ( inLepLinkContainer )
      else
        {
          delete inLepLinkContainer;
        }
    } // End: if passAll

  // If inLepLinkContainer is NOT managed by StoreGate, we must delete it here
  if ( mustDeleteLepLinkCont )
    {
      delete inLepLinkContainer;
    }


  // Some debug statements for the log file
  if ( !(m_outNeuCollKey.empty()) && !(m_outNeuLinkCollKey.empty()) )
    {
      ATH_MSG_DEBUG ( "In event number " << m_nEventsProcessed
                      << "  Input collection size = 1'"
                      << "  Output collection size = '" << outNeutrinoContainer->size()
                      << "  Output link collection size = '" << outNeutrinoLinkContainer->size() );
    }


  //-----------------------------------------
  // Set the output container of selected neutrinos as const
  //-----------------------------------------
  ATH_CHECK( evtStore()->setConst( outNeutrinoContainer ) );
  ATH_MSG_DEBUG ( "Output collection = '" << m_outNeuCollKey << "' set to const in StoreGate" );



  //-----------------------------------------
  // Set the output container of composite particles as const
  //-----------------------------------------
  ATH_CHECK( evtStore()->setConst( outContainer ) );
  ATH_MSG_DEBUG ( "Output collection = '" << m_outCollKey << "' set to const in StoreGate" );



  //-----------------------------------------
  // Create the output link container of selected neutrinos
  // and set it to const
  //-----------------------------------------
  if ( !(m_outNeuLinkCollKey.empty()) )
    {
      // Loop over the newly created output container 
      for ( unsigned int i=0; i < outNeutrinoContainer->size(); ++i )
        {
          // Actually create an ElementLink to the selected neutrino
          // and add this ElementLink to the output link container
          INav4MomLink neutrinoLink( *outNeutrinoContainer, i );
          outNeutrinoLinkContainer->push_back( neutrinoLink );
        }
	  
      ATH_CHECK( evtStore()->setConst( outNeutrinoLinkContainer ) );
      ATH_MSG_DEBUG ( "Output link collection = '" << m_outNeuLinkCollKey << "' set to const in StoreGate" );
    } // End: if ( m_outNeuLinkCollKey != "" )





  //-----------------------------------------
  // Determine if this event is accepted
  //-----------------------------------------
  unsigned int contSize = outNeutrinoContainer->size();
  if ( contSize >= m_minNumberPassed 
       && contSize <= m_maxNumberPassed )
    {
      ATH_MSG_DEBUG ( " Event " << m_nEventsProcessed
                      << " passed, found " << contSize
                      << " neutrinos. Required were minNumber=" << m_minNumberPassed
                      << " and maxNumber=" << m_maxNumberPassed );
      setFilterPassed(true);
    }
  else
    {
      ATH_MSG_DEBUG ( " Event " << m_nEventsProcessed
                      << " failed, found " << contSize
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
StatusCode D2PDLeptonNeutrinoCombiner::finalize()
{
  if ( m_printSummary || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "FINALIZING AFTER ALL EVENTS ARE PROCESSED" );
    }

  // Clean up, this temporary neutrino is not needed any more
  if ( m_tmpNeutrino ) delete m_tmpNeutrino;

  return StatusCode::SUCCESS;
}










//=============================================================================
// Helper method
//=============================================================================
StatusCode D2PDLeptonNeutrinoCombiner::findNeutrinosWithMassConstraint( const INavigable4Momentum* lep,
                                                                        double etxMiss, double etyMiss,
                                                                        NeutrinoContainer* outNeutrinoContainer )
{
  if ( lep && outNeutrinoContainer )
    {
      // Create a temporary neutrino container
      NeutrinoContainer* tmpNeutrinoContainer = new NeutrinoContainer(SG::VIEW_ELEMENTS);  
      bool findNeutrino = NeutrinoUtils::candidatesFromWMass(lep,
                                                             etxMiss, 
                                                             etyMiss,
                                                             (*tmpNeutrinoContainer),
                                                             m_dealWithNegSol);
      if ( findNeutrino )
        {
          while ( !tmpNeutrinoContainer->empty() )
            {
              Neutrino* neutrino = tmpNeutrinoContainer->back();
              tmpNeutrinoContainer->pop_back();
              if ( neutrino )
                {
                  // Add the neutrinos that pass all selections 
                  // to the output container
                  if ( selectNeutrinos(neutrino) )
                    {
                      outNeutrinoContainer->push_back( neutrino );
                    }
                  else
                    {
                      delete neutrino;
                    }
                } // End: if ( neutrino ) 
            } // End: loop over neutrino container
        } // End: if ( findNeutrino ) 
      
      // Delete temoprary container
      delete tmpNeutrinoContainer;
      
    } // End: if ( lep )
  
  return StatusCode::SUCCESS;
}







//=============================================================================
// select the neutrinos and set their properties
//=============================================================================
bool D2PDLeptonNeutrinoCombiner::selectNeutrinos( Neutrino* neutrino )
{
  neutrino->set_charge(0);
  neutrino->set_pdgId(PDG::nu_e);

  bool passAll(true);
  // transverse energy selection
  if ( passAll )
    {
      //passAll = m_filterTool->etFilter( neutrino, m_etMin, m_etMax );
    }
  // eta selection
  if ( passAll )
    {
      //passAll = m_filterTool->etaFilter( neutrino, m_etaMin, m_etaMax );
    }
  // eta veto selection
  if ( passAll )
    {
      //passAll = m_filterTool->etaVetoRangesFilter( neutrino, m_etaVetoRanges );
    }
  
  return passAll;
}





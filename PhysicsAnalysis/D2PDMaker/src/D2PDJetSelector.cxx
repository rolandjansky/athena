/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Name: D2PDJetSelector.cxx
//
/**
   @class D2PDJetSelector

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Select jets and write out good ones as ElementLinks

   Class to select good jets from an input container (which can either
   be an jet container or a container of ElementLinks poining to an
   jet container) and write out the good ones as a new container of
   ElementLinks pointing to the good jets in the original input container.
*/
//=============================================================================

// This classes header
#include "D2PDMaker/D2PDJetSelector.h"

// STL includes
#include <string>
#include <vector>
#include <cfloat>

// Gaudi stuff
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

// jet includes
#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"
//#include "JetUtils/JetCaloQualityUtils.h" // Not in 15.6.4!
#include "Particle/TrackParticle.h"
#include "EventKernel/SignalStateHelper.h"
#include "MissingETPerformance/JetVariablesTool.h"


//=============================================================================
// Constructor
//=============================================================================
D2PDJetSelector::D2PDJetSelector( const std::string& name,
                                  ISvcLocator* pSvcLocator ) :
  D2PDSelectorBase< Jet, JetCollection >( name, pSvcLocator ),
  m_signalStateKinSelectorTool( "KinematicSelector/SignalStateKinematicSelectorForJetSelector", this ),
  m_jetVariablesTool("JetVariablesTool",this),
  m_doJetShapeList(false),
  m_idxJetShapeList(0),
  m_doJVF(false),
  m_idxJVF(0),
  m_doFlavorTag(false),
  m_idxFlavorTag(0),
  m_doEMFrac(false),
  m_idxEMFrac(0),
  m_doPSFrac(false),
  m_idxPSFrac(0),
  m_doN90(false),
  m_idxN90(0),
  m_doSamplingMax(false),
  m_idxSamplingMax(0),
  m_doFCor(false),
  m_idxFCor(0),
  m_doScaledJESEt(false),
  m_idxScaledJESEt(0),
  m_doScaledJESE(false),
  m_idxScaledJESE(0),
  m_doScaledJESPt(false),
  m_idxScaledJESPt(0),
  m_doScaledJESP(false),
  m_idxScaledJESP(0),
  m_doScaledJESMass(false),
  m_idxScaledJESMass(0),
  m_doSignalStateEt(false),
  m_idxSignalStateEt(0),
  m_doSignalStateE(false),
  m_idxSignalStateE(0),
  m_doSignalStatePt(false),
  m_idxSignalStatePt(0),
  m_doSignalStateP(false),
  m_idxSignalStateP(0),
  m_doSignalStateEta(false),
  m_idxSignalStateEta(0),
  m_doSignalStateAbsEta(false),
  m_idxSignalStateAbsEta(0),
  m_doSignalStateEtaVeto(false),
  m_idxSignalStateEtaVeto(0),
  m_doSignalStatePhi(false),
  m_idxSignalStatePhi(0),
  m_doSignalStatePhiVeto(false),
  m_idxSignalStatePhiVeto(0),
  m_doSignalStateMass(false),
  m_idxSignalStateMass(0)
{
  declareProperty("jetShapeList",                m_jetShapeList,                  "List of jet shapes to use for selection" );
  declareProperty("jetShapeMinList",             m_jetShapeMinList,               "List of jet shape lower cut values" );
  declareProperty("jetShapeMaxList",             m_jetShapeMaxList,               "List of jet shape upper cut values" );

  declareProperty("jetVertexFractionAbsMin",     m_absJVFMin=-DBL_MAX,            "Minimum |JetVertexFraction|" );
  m_absJVFMin.declareUpdateHandler( &D2PDJetSelector::setupJetVertexFraction, this );
  declareProperty("jetVertexFractionAbsMax",     m_absJVFMax=DBL_MAX,             "Maximum |JetVertexFraction|" );
  m_absJVFMax.declareUpdateHandler( &D2PDJetSelector::setupJetVertexFraction, this );
  declareProperty("jetVertexFractionName",       m_JVFName = "JVF",               "Name of the jet vertex fraction moment to be used." );

  declareProperty("jetFlavourTagWeightMin",      m_flavorTagWeightMin=-DBL_MAX,   "Minimum required flavor tag weight" );
  m_flavorTagWeightMin.declareUpdateHandler( &D2PDJetSelector::setupFlavorTag, this );
  declareProperty("jetFlavourTagWeightMax",      m_flavorTagWeightMax=DBL_MAX,    "Maximum required flavor tag weight" );
  m_flavorTagWeightMax.declareUpdateHandler( &D2PDJetSelector::setupFlavorTag, this );
  declareProperty("jetFlavourTagName",           m_flavorTaggerName="",
                  "Name of the flavor tagger to be used. If none is give, the default tagger will be used." );

  declareProperty("jetEMFractionMin",            m_emFractionMin=-DBL_MAX,        "Minimum required jet EM fraction" );
  m_emFractionMin.declareUpdateHandler( &D2PDJetSelector::setupEMFrac, this );
  declareProperty("jetEMFractionMax",            m_emFractionMax=DBL_MAX,         "Maximum required jet EM fraction" );
  m_emFractionMax.declareUpdateHandler( &D2PDJetSelector::setupEMFrac, this );

  declareProperty("jetPSFractionMin",            m_psFractionMin=-DBL_MAX,        "Minimum required jet Presampler fraction");
  m_psFractionMin.declareUpdateHandler( &D2PDJetSelector::setupPSFrac, this );
  declareProperty("jetPSFractionMax",            m_psFractionMax=DBL_MAX,         "Maximum required jet Presampler fraction");
  m_psFractionMax.declareUpdateHandler( &D2PDJetSelector::setupPSFrac, this );

  declareProperty("jetN90Min",                   m_n90Min=-DBL_MAX,               "Minimum required jet n90 cut" );
  m_n90Min.declareUpdateHandler( &D2PDJetSelector::setupN90, this );
  declareProperty("jetN90Max",                   m_n90Max=DBL_MAX,                "Maximum required jet n90 cut" );
  m_n90Max.declareUpdateHandler( &D2PDJetSelector::setupN90, this );
  declareProperty("jetN90Name",                  m_n90Name="n90",                 "Actual jet n90 moment name" );

  declareProperty("jetVetoSamplingMaxList",      m_vetoSamplingMaxList,
                  "List of integer values that should be vetoed for the SamplingMax jet moment/shape" );
  declareProperty("jetSamplingMaxName",          m_samplingMaxName="SamplingMax", "SamplingMax jet shape/moment name" );

  declareProperty("jetFCorMin",                  m_fCorMin=-DBL_MAX,              "fcor=BAD_CELLS_CORR_E/E_jet(emscale) minimum cut value " );
  m_fCorMin.declareUpdateHandler( &D2PDJetSelector::setupFCor, this );
  declareProperty("jetFCorMax",                  m_fCorMax=DBL_MAX,               "fcor=BAD_CELLS_CORR_E/E_jet(emscale) maximum cut value " );
  m_fCorMax.declareUpdateHandler( &D2PDJetSelector::setupFCor, this );


  declareProperty("useJetScalingMoment",         m_jetScaleMoment="EMJES",
                  "Use this jet moment to retrieve the scale factor for the jet energy" );

  declareProperty("jetScaledEtMin",              m_scaledJESEtMin=-DBL_MAX,       "Scaled jet min E_T" );
  m_scaledJESEtMin.declareUpdateHandler( &D2PDJetSelector::setupScaledJESEt, this );
  declareProperty("jetScaledEtMax",              m_scaledJESEtMax=DBL_MAX,        "Scaled jet max E_T" );
  m_scaledJESEtMax.declareUpdateHandler( &D2PDJetSelector::setupScaledJESEt, this );

  declareProperty("jetScaledEMin",               m_scaledJESEMin=-DBL_MAX,        "Scaled jet min E" );
  m_scaledJESEMin.declareUpdateHandler( &D2PDJetSelector::setupScaledJESE, this );
  declareProperty("jetScaledEMax",               m_scaledJESEMax=DBL_MAX,         "Scaled jet max E" );
  m_scaledJESEMax.declareUpdateHandler( &D2PDJetSelector::setupScaledJESE, this );

  declareProperty("jetScaledPtMin",              m_scaledJESPtMin=-DBL_MAX,       "Scaled jet min P_T" );
  m_scaledJESPtMin.declareUpdateHandler( &D2PDJetSelector::setupScaledJESPt, this );
  declareProperty("jetScaledPtMax",              m_scaledJESPtMax=DBL_MAX,        "Scaled jet max P_T" );
  m_scaledJESPtMax.declareUpdateHandler( &D2PDJetSelector::setupScaledJESPt, this );

  declareProperty("jetScaledPMin",               m_scaledJESPMin=-DBL_MAX,        "Scaled jet min P" );
  m_scaledJESPMin.declareUpdateHandler( &D2PDJetSelector::setupScaledJESP, this );
  declareProperty("jetScaledPMax",               m_scaledJESPMax=DBL_MAX,         "Scaled jet max P" );
  m_scaledJESPMax.declareUpdateHandler( &D2PDJetSelector::setupScaledJESP, this );

  declareProperty("jetScaledMassMin",            m_scaledJESMassMin=-DBL_MAX,     "Scaled jet min mass" );
  m_scaledJESMassMin.declareUpdateHandler( &D2PDJetSelector::setupScaledJESMass, this );
  declareProperty("jetScaledMassMax",            m_scaledJESMassMax=DBL_MAX,      "Scaled jet max mass" );
  m_scaledJESMassMax.declareUpdateHandler( &D2PDJetSelector::setupScaledJESMass, this );


  declareProperty("useJetSignalState",           m_signalState=0,
                  "Use this jet SignalState for the jetSignalStateXXX selections" );

  declareProperty("jetSignalStateKinematicSelectorTool",
                  m_signalStateKinSelectorTool,
                  "Kinematic selection tool for jets with the other SignalState" );

  declareProperty("jetSignalStateEtMin",         m_signalStateEtMin=-DBL_MAX,     "SignalState jet min E_T" );
  m_signalStateEtMin.declareUpdateHandler( &D2PDJetSelector::setupSignalStateEt, this );
  declareProperty("jetSignalStateEtMax",         m_signalStateEtMax=DBL_MAX,      "SignalState jet max E_T" );
  m_signalStateEtMax.declareUpdateHandler( &D2PDJetSelector::setupSignalStateEt, this );

  declareProperty("jetSignalStateEMin",          m_signalStateEMin=-DBL_MAX,      "SignalState jet min E" );
  m_signalStateEMin.declareUpdateHandler( &D2PDJetSelector::setupSignalStateE, this );
  declareProperty("jetSignalStateEMax",          m_signalStateEMax=DBL_MAX,       "SignalState jet max E" );
  m_signalStateEMax.declareUpdateHandler( &D2PDJetSelector::setupSignalStateE, this );

  declareProperty("jetSignalStatePtMin",         m_signalStatePtMin=-DBL_MAX,     "SignalState jet min P_T" );
  m_signalStatePtMin.declareUpdateHandler( &D2PDJetSelector::setupSignalStatePt, this );
  declareProperty("jetSignalStatePtMax",         m_signalStatePtMax=DBL_MAX,      "SignalState jet max P_T" );
  m_signalStatePtMax.declareUpdateHandler( &D2PDJetSelector::setupSignalStatePt, this );

  declareProperty("jetSignalStatePMin",          m_signalStatePMin=-DBL_MAX,      "SignalState jet min P" );
  m_signalStatePMin.declareUpdateHandler( &D2PDJetSelector::setupSignalStateP, this );
  declareProperty("jetSignalStatePMax",          m_signalStatePMax=DBL_MAX,       "SignalState jet max P" );
  m_signalStatePMax.declareUpdateHandler( &D2PDJetSelector::setupSignalStateP, this );

  declareProperty("jetSignalStateEtaMin",        m_signalStateEtaMin=-DBL_MAX,    "SignalState min eta" );
  m_signalStateEtaMin.declareUpdateHandler( &D2PDJetSelector::setupSignalStateEta, this );
  declareProperty("jetSignalStateEtaMax",        m_signalStateEtaMax=DBL_MAX,     "SignalState jet max eta" );
  m_signalStateEtaMax.declareUpdateHandler( &D2PDJetSelector::setupSignalStateEta, this );

  declareProperty("jetSignalStateAbsEtaMin",     m_signalStateAbsEtaMin=-DBL_MAX, "SignalState min |eta|" );
  m_signalStateAbsEtaMin.declareUpdateHandler( &D2PDJetSelector::setupSignalStateAbsEta, this );
  declareProperty("jetSignalStateAbsEtaMax",     m_signalStateAbsEtaMax=DBL_MAX,  "SignalState jet max |eta|" );
  m_signalStateAbsEtaMax.declareUpdateHandler( &D2PDJetSelector::setupSignalStateAbsEta, this );

  declareProperty("jetSignalStateEtaVetoRanges", m_signalStateEtaVetoRanges="",   "SignalState jet eta-ranges to veto" );
  m_signalStateEtaVetoRanges.declareUpdateHandler( &D2PDJetSelector::setupSignalStateEtaVeto, this );

  declareProperty("jetSignalStatePhiMin",        m_signalStatePhiMin=-DBL_MAX,    "SignalState jet min phi" );
  m_signalStatePhiMin.declareUpdateHandler( &D2PDJetSelector::setupSignalStatePhi, this );
  declareProperty("jetSignalStatePhiMax",        m_signalStatePhiMax=DBL_MAX,     "SignalState jet max phi" );
  m_signalStatePhiMax.declareUpdateHandler( &D2PDJetSelector::setupSignalStatePhi, this );

  declareProperty("jetSignalStatePhiVetoRanges", m_signalStatePhiVetoRanges="",   "SignalState jet phi-ranges to veto" );
  m_signalStatePhiVetoRanges.declareUpdateHandler( &D2PDJetSelector::setupSignalStatePhiVeto, this );

  declareProperty("jetSignalStateMassMin",       m_signalStateMassMin=-DBL_MAX,   "SignalState jet min mass" );
  m_signalStateMassMin.declareUpdateHandler( &D2PDJetSelector::setupSignalStateMass, this );
  declareProperty("jetSignalStateMassMax",       m_signalStateMassMax=DBL_MAX,    "SignalState jet max mass" );
  m_signalStateMassMax.declareUpdateHandler( &D2PDJetSelector::setupSignalStateMass, this );

  declareProperty("jetVariablesTool",            m_jetVariablesTool );

  m_nJetsProcessed=0;
  m_nJetsPassed=0;
}


//=============================================================================
// Destructor
//=============================================================================
D2PDJetSelector::~D2PDJetSelector()
{
}


//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode D2PDJetSelector::initializeMore()
{
  // Print the used configuration
  if ( m_printSetup || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "==> initializeMore " << name() << "..." );
      
      // Print out the used configuration
      ATH_MSG_INFO ( " using jetShapeList                = " << m_jetShapeList    );
      ATH_MSG_INFO ( " using jetShapeMinList             = " << m_jetShapeMinList );
      ATH_MSG_INFO ( " using jetShapeMaxList             = " << m_jetShapeMaxList );

      ATH_MSG_INFO ( " using jetVertexFractionAbsMin     = " << m_absJVFMin );
      ATH_MSG_INFO ( " using jetVertexFractionAbsMax     = " << m_absJVFMax );
      ATH_MSG_INFO ( " using jetVertexFractionName       = " << m_JVFName   );

      ATH_MSG_INFO ( " using jetFlavourTagWeightMin      = " << m_flavorTagWeightMin );
      ATH_MSG_INFO ( " using jetFlavourTagWeightMax      = " << m_flavorTagWeightMax );
      ATH_MSG_INFO ( " using jetFlavourTagName           = " << m_flavorTaggerName   );

      ATH_MSG_INFO ( " using jetEMFractionMin            = " << m_emFractionMin );
      ATH_MSG_INFO ( " using jetEMFractionMax            = " << m_emFractionMax );

      ATH_MSG_INFO ( " using jetPSFractionMin            = " << m_psFractionMin );
      ATH_MSG_INFO ( " using jetPSFractionMax            = " << m_psFractionMax );

      ATH_MSG_INFO ( " using jetN90Min                   = " << m_n90Min  );
      ATH_MSG_INFO ( " using jetN90Max                   = " << m_n90Max  );
      ATH_MSG_INFO ( " using jetN90Name                  = " << m_n90Name );

      ATH_MSG_INFO ( " using jetVetoSamplingMaxList      = " << m_vetoSamplingMaxList );
      ATH_MSG_INFO ( " using jetVetoSamplingMaxName      = " << m_samplingMaxName     );

      ATH_MSG_INFO ( " using jetFCorMin                  = " << m_fCorMin     );
      ATH_MSG_INFO ( " using jetFCorMax                  = " << m_fCorMax     );


      ATH_MSG_INFO ( " using useJetScalingMoment         = " << m_jetScaleMoment     );

      ATH_MSG_INFO ( " using jetScaledEtMin              = " << m_scaledJESEtMin );
      ATH_MSG_INFO ( " using jetScaledEtMax              = " << m_scaledJESEtMax );

      ATH_MSG_INFO ( " using jetScaledEMin               = " << m_scaledJESEMin );
      ATH_MSG_INFO ( " using jetScaledEMax               = " << m_scaledJESEMax );

      ATH_MSG_INFO ( " using jetScaledPtMin              = " << m_scaledJESPtMin );
      ATH_MSG_INFO ( " using jetScaledPtMax              = " << m_scaledJESPtMax );

      ATH_MSG_INFO ( " using jetScaledPMin               = " << m_scaledJESPMin );
      ATH_MSG_INFO ( " using jetScaledPMax               = " << m_scaledJESPMax );

      ATH_MSG_INFO ( " using jetScaledMassMin            = " << m_scaledJESMassMin );
      ATH_MSG_INFO ( " using jetScaledMassMax            = " << m_scaledJESMassMax );


      ATH_MSG_INFO ( " using useJetSignalState           = " << m_signalState     );

      ATH_MSG_INFO ( " using jetSignalStateKinematicSelectorTool = " << m_signalStateKinSelectorTool    );

      ATH_MSG_INFO ( " using jetSignalStateEtMin         = " << m_signalStateEtMin );
      ATH_MSG_INFO ( " using jetSignalStateEtMax         = " << m_signalStateEtMax );

      ATH_MSG_INFO ( " using jetSignalStateEMin          = " << m_signalStateEMin );
      ATH_MSG_INFO ( " using jetSignalStateEMax          = " << m_signalStateEMax );

      ATH_MSG_INFO ( " using jetSignalStatePtMin         = " << m_signalStatePtMin );
      ATH_MSG_INFO ( " using jetSignalStatePtMax         = " << m_signalStatePtMax );

      ATH_MSG_INFO ( " using jetSignalStatePMin          = " << m_signalStatePMin );
      ATH_MSG_INFO ( " using jetSignalStatePMax          = " << m_signalStatePMax );

      ATH_MSG_INFO ( " using jetSignalStateEtaMin        = " << m_signalStateEtaMin );
      ATH_MSG_INFO ( " using jetSignalStateEtaMax        = " << m_signalStateEtaMax );

      ATH_MSG_INFO ( " using jetSignalStateAbsEtaMin     = " << m_signalStateAbsEtaMin );
      ATH_MSG_INFO ( " using jetSignalStateAbsEtaMax     = " << m_signalStateAbsEtaMax );

      ATH_MSG_INFO ( " using jetSignalStateEtaVetoRanges = " << m_signalStateEtaVetoRanges );

      ATH_MSG_INFO ( " using jetSignalStatePhiMin        = " << m_signalStatePhiMin );
      ATH_MSG_INFO ( " using jetSignalStatePhiMax        = " << m_signalStatePhiMax );

      ATH_MSG_INFO ( " using jetSignalStatePhiVetoRanges = " << m_signalStatePhiVetoRanges );

      ATH_MSG_INFO ( " using jetSignalStateMassMin       = " << m_signalStateMassMin );
      ATH_MSG_INFO ( " using jetSignalStateMassMax       = " << m_signalStateMassMax );
    } // End: if ( m_printSetup )


  // Do some consistency checks
  if ( m_jetShapeList.size() != m_jetShapeMinList.size()
       || m_jetShapeList.size() != m_jetShapeMaxList.size()
       || m_jetShapeMinList.size() != m_jetShapeMaxList.size()
       )
    {
      ATH_MSG_ERROR ( "The length of the jet shape lists do not agree"
                      << "! Length of jetShapeList=" << m_jetShapeList.size()
                      << ". Length of jetShapeMinList=" << m_jetShapeMinList.size()
                      << ". Length of jetShapeMaxList=" << m_jetShapeMaxList.size() );
      return StatusCode::FAILURE;
    }


  // get the kinematic selector tool (derived class from ISelectorCore)
  // for the jets with the alternate SignalState
  ATH_CHECK( m_signalStateKinSelectorTool.retrieve() );
  if ( m_printSetup || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "jetSignalStateKinematicSelectorTool:" << m_signalStateKinSelectorTool );
    }

  ATH_CHECK( m_jetVariablesTool.retrieve() );
  if ( m_printSetup || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "jetVariablesTool:" << m_jetVariablesTool );
    }
  


  //--------------------------------------------------------------------------
  // Set the cut values for the selection tools
  //--------------------------------------------------------------------------

  //-----------------------
  // jets with different signal state quantities
  //-----------------------
  m_signalStateKinSelectorTool->getTool()->setminEt( m_signalStateEtMin );
  m_signalStateKinSelectorTool->getTool()->setmaxEt( m_signalStateEtMax );

  m_signalStateKinSelectorTool->getTool()->setminE( m_signalStateEMin );
  m_signalStateKinSelectorTool->getTool()->setmaxE( m_signalStateEMax );

  m_signalStateKinSelectorTool->getTool()->setminPt( m_signalStatePtMin );
  m_signalStateKinSelectorTool->getTool()->setmaxPt( m_signalStatePtMax );

  m_signalStateKinSelectorTool->getTool()->setminP( m_signalStatePMin );
  m_signalStateKinSelectorTool->getTool()->setmaxP( m_signalStatePMax );

  m_signalStateKinSelectorTool->getTool()->setminEta( m_signalStateEtaMin );
  m_signalStateKinSelectorTool->getTool()->setmaxEta( m_signalStateEtaMax );

  m_signalStateKinSelectorTool->getTool()->setminAbsEta( m_signalStateAbsEtaMin );
  m_signalStateKinSelectorTool->getTool()->setmaxAbsEta( m_signalStateAbsEtaMax );

  m_signalStateKinSelectorTool->getTool()->setEtaVetoRanges( (std::string&)m_signalStateEtaVetoRanges.value() );

  m_signalStateKinSelectorTool->getTool()->setminPhi( m_signalStatePhiMin );
  m_signalStateKinSelectorTool->getTool()->setmaxPhi( m_signalStatePhiMax );

  m_signalStateKinSelectorTool->getTool()->setPhiVetoRanges( (std::string&)m_signalStatePhiVetoRanges.value() );

  m_signalStateKinSelectorTool->getTool()->setminMass( m_signalStateMassMin );
  m_signalStateKinSelectorTool->getTool()->setmaxMass( m_signalStateMassMax );

  m_nJetsProcessed=0;
  m_nJetsPassed=0;


  if ( m_jetShapeList.size() > 0 ) { m_doJetShapeList = true; }
  if ( m_vetoSamplingMaxList.size() > 0 ) { m_doSamplingMax = true; }

  // Setup the internal cutflow stuff
  if ( m_printInternalCutflow )
    {
      if ( m_doJetShapeList )
        {
          m_idxJetShapeList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doJetShapeList=1", std::make_pair(0,0) ) );
        }
      if ( m_doJVF )
        {
          m_idxJVF = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < |%s| < %g",
                                                          m_absJVFMin.value(), m_JVFName.c_str(), m_absJVFMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doFlavorTag )
        {
          m_idxFlavorTag = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < flavor tag < %g",
                                                          m_flavorTagWeightMin.value(), m_flavorTagWeightMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doEMFrac )
        {
          m_idxEMFrac = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < EM fraction < %g",
                                                          m_emFractionMin.value(), m_emFractionMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doPSFrac )
        {
          m_idxPSFrac = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < PS fraction < %g",
                                                          m_psFractionMin.value(), m_psFractionMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doN90 )
        {
          m_idxN90 = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < n90 < %g",
                                                          m_n90Min.value(), m_n90Max.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doSamplingMax )
        {
          m_idxSamplingMax = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doSamplingMax=1", std::make_pair(0,0) ) );
        }
      if ( m_doFCor )
        {
          m_idxFCor = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < FCor < %g",
                                                          m_fCorMin.value(), m_fCorMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doScaledJESEt )
        {
          m_idxScaledJESEt = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < scaled JES et < %g",
                                                          m_scaledJESEtMin.value(), m_scaledJESEtMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doScaledJESE )
        {
          m_idxScaledJESE = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < scaled JES e < %g",
                                                          m_scaledJESEMin.value(), m_scaledJESEMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doScaledJESPt )
        {
          m_idxScaledJESPt = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < scaled JES pt < %g",
                                                          m_scaledJESPtMin.value(), m_scaledJESPtMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doScaledJESP )
        {
          m_idxScaledJESP = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < scaled JES p < %g",
                                                          m_scaledJESPMin.value(), m_scaledJESPMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doScaledJESMass )
        {
          m_idxScaledJESMass = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < scaled JES mass < %g",
                                                          m_scaledJESMassMin.value(), m_scaledJESMassMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doSignalStateEt )
        {
          m_idxSignalStateEt = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < signal state et < %g",
                                                          m_signalStateEtMin.value(), m_signalStateEtMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doSignalStateE )
        {
          m_idxSignalStateE = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < signal state e < %g",
                                                          m_signalStateEMin.value(), m_signalStateEMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doSignalStatePt )
        {
          m_idxSignalStatePt = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < signal state pt < %g",
                                                          m_signalStatePtMin.value(), m_signalStatePtMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doSignalStateP )
        {
          m_idxSignalStateP = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < signal state p < %g",
                                                          m_signalStatePMin.value(), m_signalStatePMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doSignalStateEta )
        {
          m_idxSignalStateEta = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < signal state eta < %g",
                                                          m_signalStateEtaMin.value(), m_signalStateEtaMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doSignalStateAbsEta )
        {
          m_idxSignalStateAbsEta = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < signal state |eta| < %g",
                                                          m_signalStateAbsEtaMin.value(), m_signalStateAbsEtaMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doSignalStateEtaVeto )
        {
          m_idxSignalStateEtaVeto = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( m_signalStateEtaVetoRanges, std::make_pair(0,0) ) );
        }
      if ( m_doSignalStatePhi )
        {
          m_idxSignalStatePhi = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < signal state phi < %g",
                                                          m_signalStatePhiMin.value(), m_signalStatePhiMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doSignalStatePhiVeto )
        {
          m_idxSignalStatePhiVeto = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( m_signalStatePhiVetoRanges, std::make_pair(0,0) ) );
        }
      if ( m_doSignalStateMass )
        {
          m_idxSignalStateMass = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < signal state mass < %g",
                                                          m_signalStateMassMin.value(), m_signalStateMassMax.value() ), std::make_pair(0,0) ) );
        }
    }



  return StatusCode::SUCCESS;
}



//=============================================================================
// Athena execute method
//=============================================================================
StatusCode D2PDJetSelector::processObject( const Jet* jet,
                                           bool &isPassed )
{
  m_nJetsProcessed++;
  // Make sure we have a jet
  if ( jet )
    {
	      
      //--------------------------
      // Do the selections
      //--------------------------

      // Get all requested jet shapes and perform the selection
      if ( m_doJetShapeList && isPassed )
        {
          for ( unsigned int i=0; i < m_jetShapeList.size(); ++i )
            {
              JetKeyDescriptor::key_t currentShape = (JetKeyDescriptor::key_t)m_jetShapeList[i];
              double lowerCut = m_jetShapeMinList[i];
              double upperCut = m_jetShapeMaxList[i];
              isPassed = isPassed && ( ((double)(jet->getShape(currentShape))) >= lowerCut ) && ( ((double)(jet->getShape(currentShape))) <= upperCut );
            } // End: loop over jet shapes
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxJetShapeList] += 1; }
        }


      // Jet vertex fraction selection
      if ( m_doJVF && isPassed )
        {
          ATH_MSG_VERBOSE( "Available jet moment keys: " << jet->getMomentKeys() );
          double absJVF = fabs((double)(jet->getMoment(m_JVFName)));
          isPassed = ( absJVF >= m_absJVFMin && absJVF <= m_absJVFMax )  ?  true  :  false;
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxJVF] += 1; }
        }


      // flavor tag weight
      if ( m_doFlavorTag && isPassed )
        {
          if ( m_flavorTaggerName == "" )
            {
              if ( jet->getFlavourTagWeight() >= m_flavorTagWeightMin
                   && jet->getFlavourTagWeight() <= m_flavorTagWeightMax
                   )
                {
                  isPassed = true;
                }
              else
                {
                  isPassed = false;
                }
            }
          else
            {
              if ( jet->getFlavourTagWeight(m_flavorTaggerName) >= m_flavorTagWeightMin
                   && jet->getFlavourTagWeight(m_flavorTaggerName) <= m_flavorTagWeightMax
                   )
                {
                  isPassed = true;
                }
              else
                {
                  isPassed = false;
                }
            }
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxFlavorTag] += 1; }
        }


      // Jet EM Fraction selection
      if ( m_doEMFrac && isPassed )
        {
           /*
          double emfrac = jetEMFraction(jet);
          if ( emfrac >= m_emFractionMin && emfrac <= m_emFractionMax )
            {
              isPassed = true;
            }
          else
            {
              isPassed = false;
            }
           */
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxEMFrac] += 1; }
        }

     // Jet Presampler Fraction selection
      if ( m_doPSFrac && isPassed )
        {
          double psfrac = m_jetVariablesTool->SamplingFracPS(jet);
          if ( psfrac >= m_psFractionMin && psfrac <= m_psFractionMax )
            {
              isPassed = true;
            }
          else
            {
              isPassed = false;
            }
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxPSFrac] += 1; }
        }



      // Jet n90 selection
      if ( m_doN90 && isPassed )
        {
          //double n90 = JetCaloQualityUtils::nLeadingCells( jet, m_n90 ); 
          JetKeyDescriptor::key_t n90Shape = (JetKeyDescriptor::key_t)m_n90Name;
          double n90 = (double)(jet->getShape(n90Shape));
          if ( n90 >= m_n90Min && n90 <= m_n90Max )
            {
              isPassed = true;
            }
          else
            {
              isPassed = false;
            }
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxN90] += 1; }
        }


      // SamplingMax veto selection
      if ( m_doSamplingMax && isPassed )
        {
          JetKeyDescriptor::key_t samplingMaxShape = (JetKeyDescriptor::key_t)m_samplingMaxName;
          int samplingMax = (int)(jet->getShape(samplingMaxShape));
          bool vetoSamplingMax(false);
          for ( unsigned int i=0; i < m_vetoSamplingMaxList.size(); ++i )
            {
              if ( samplingMax == m_vetoSamplingMaxList[i] )
                {
                  vetoSamplingMax = true;
                }
            }
          isPassed = !vetoSamplingMax;
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxSamplingMax] += 1; }
        }


      // for quality selection
      if ( m_doFCor && isPassed )
        {
          std::string badCellsCorrEName("BAD_CELLS_CORR_E");
          JetKeyDescriptor::key_t badCellsCorrEShape = (JetKeyDescriptor::key_t)badCellsCorrEName;
          double badCellsCorrE(-99.0);
          badCellsCorrE = (double)(jet->getShape(badCellsCorrEShape));

          double jetEMScaleE(-99.0);
          jetEMScaleE = jet->e(P4SignalState::JETEMSCALE);

          double fcor(-99.0);
          if ( jetEMScaleE != 0.0 )
            {
              fcor = badCellsCorrE / jetEMScaleE;
            }
          if ( fcor >= m_fCorMin && fcor <= m_fCorMax )
            {
              isPassed = true;
            }
          else
            {
              isPassed = false;
            }
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxFCor] += 1; }
        }

      //----------------------------------------------------------
      // Do selections for jets with a rescaled jet energy scale
      //----------------------------------------------------------
      double emJESfactor    = jet->getMoment( m_jetScaleMoment );
      Jet::hlv_t jet4MomJES =  emJESfactor * jet->hlv( P4SignalState::JETEMSCALE );
      // transverse energy selection
      if ( m_doScaledJESEt && isPassed )
        {
          isPassed = ( jet4MomJES.et() > m_scaledJESEtMin  && jet4MomJES.et() < m_scaledJESEtMax );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxScaledJESEt] += 1; }
        }
      // energy selection
      if ( m_doScaledJESE && isPassed )
        {
          isPassed = ( jet4MomJES.e() > m_scaledJESEMin  && jet4MomJES.e() < m_scaledJESEMax );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxScaledJESE] += 1; }
        }
      // transverse momentum selection
      if ( m_doScaledJESPt &&  isPassed )
        {
          isPassed = ( jet4MomJES.perp() > m_scaledJESPtMin  && jet4MomJES.perp() < m_scaledJESPtMax );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxScaledJESPt] += 1; }
        }
      // momentum selection
      if ( m_doScaledJESP &&  isPassed )
        {
          isPassed = ( jet4MomJES.rho() > m_scaledJESPMin  && jet4MomJES.rho() < m_scaledJESPMax );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxScaledJESP] += 1; }
        }
      // mass selection
      if ( m_doScaledJESMass &&  isPassed )
        {
          isPassed = ( jet4MomJES.m() > m_scaledJESMassMin  && jet4MomJES.m() < m_scaledJESMassMax );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxScaledJESMass] += 1; }
        }



      //----------------------------------------------------------
      // Do selections for jets with a different SignalState
      //----------------------------------------------------------
      P4SignalState::State originalSignalState = jet->signalState();
      SignalStateHelper sigstateH( jet, (P4SignalState::State)m_signalState );
      ATH_MSG_DEBUG ( "Setting a new SignalState for this jet! Will apply the selections for the new SignalState!" );
      
      // transverse energy selection
      if ( m_doSignalStateEt && isPassed )
        {	  
          isPassed = m_signalStateKinSelectorTool->getTool()->passEt( jet );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxSignalStateEt] += 1; }
        }
      // energy selection
      if ( m_doSignalStateE && isPassed )
        {
          isPassed = m_signalStateKinSelectorTool->getTool()->passE( jet );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxSignalStateE] += 1; }
        }
      // transverse momentum selection
      if ( m_doSignalStatePt && isPassed )
        {
          isPassed = m_signalStateKinSelectorTool->getTool()->passPt( jet );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxSignalStatePt] += 1; }
        }
      // momentum selection
      if ( m_doSignalStateP && isPassed )
        {
          isPassed = m_signalStateKinSelectorTool->getTool()->passP( jet );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxSignalStateP] += 1; }
        }
      // eta selection
      if ( m_doSignalStateEta && isPassed )
        {
          isPassed = m_signalStateKinSelectorTool->getTool()->passEta( jet );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxSignalStateEta] += 1; }
        }
      // |eta| selection
      if ( m_doSignalStateAbsEta && isPassed )
        {
          isPassed = m_signalStateKinSelectorTool->getTool()->passAbsEta( jet );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxSignalStateAbsEta] += 1; }
        }
      // eta veto selection
      if ( m_doSignalStateEtaVeto && isPassed )
        {
          isPassed = m_signalStateKinSelectorTool->getTool()->passEtaVetoRanges( jet );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxSignalStateEtaVeto] += 1; }
        }
      // phi selection
      if ( m_doSignalStatePhi && isPassed )
        {
          isPassed = m_signalStateKinSelectorTool->getTool()->passPhi( jet );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxSignalStatePhi] += 1; }
        }
      // phi veto selection
      if ( m_doSignalStatePhiVeto && isPassed )
        {
          isPassed = m_signalStateKinSelectorTool->getTool()->passPhiVetoRanges( jet );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxSignalStatePhiVeto] += 1; }
        }
      // mass selection
      if ( m_doSignalStateMass && isPassed )
        {
          isPassed = m_signalStateKinSelectorTool->getTool()->passMass( jet );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxSignalStateMass] += 1; }
        }
      // Now, revert the jet SignalState to the original one
      // done automatically when sigstateH go away.
      SignalStateHelper sigstateH2( jet, originalSignalState );
      ATH_MSG_DEBUG ( "Reverting the jet to the original SignalState!" );
      if (isPassed) m_nJetsPassed++;
    }
  else
    {
      // Return false 
      isPassed = false;
      ATH_MSG_WARNING ( "Could not dynamic_cast to an jet! Failing this jet!" );
    } // End: if/else have jet


  return StatusCode::SUCCESS;
}





//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode D2PDJetSelector::finalizeMore()
{
  return StatusCode::SUCCESS;
}




/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//=============================================================================
/**
   @Project: D2PDMaker
 
   @class D2PDElectronSelector
   
   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>
   
   @date June 2009
   
   @brief Select electrons and write out good ones as ElementLinks.
   
   Class to select good electrons from an input container (which can either
   be an electron container or a container of ElementLinks poining to an
   electron container) and write out the good ones as a new container of
   ElementLinks pointing to the good electrons in the original input container.
   
   This class inherits from D2PDMaker/BaseSelector. Thus, all standard
   selection criteria are already defined there.
*/
//=============================================================================


// This classes header
#include "D2PDMaker/D2PDElectronSelector.h"

// STL includes
#include <string>
#include <vector>
#include <cfloat>

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ListItem.h"

// The needed ElementLink stuff
#include "DataModel/DataVector.h"
#include "DataModel/ElementLink.h" 

// egamma includes
#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/egammaParamDefs.h"
#include "egammaEvent/EMShower.h"
#include "egammaEvent/EMTrackMatch.h"
#include "Particle/TrackParticle.h"
#include "CaloEvent/CaloCluster.h"





//=============================================================================
// Constructor
//=============================================================================
D2PDElectronSelector::D2PDElectronSelector( const std::string& name,
                                            ISvcLocator* pSvcLocator ) :
  D2PDSelectorBase< Analysis::Electron, ElectronContainer >( name, pSvcLocator ),
  m_clusterKinSelectorTool( "KinematicSelector/ClusterKinematicSelectorForElectronSelector", this ),
  m_clusterCaloLayerKinSelectorTool( "KinematicSelector/ClusterCaloLayerKinematicSelectorForElectronSelector", this ),
  m_trackKinSelectorTool( "KinematicSelector/TrackKinematicSelectorForElectronSelector", this ),
  m_doIsEM(false),
  m_idxIsEM(0),
  m_doPassID(false),
  m_idxPassID(0),
  m_doAuthor(false),
  m_idxAuthor(0),
  m_doAcceptAuthorList(false),
  m_idxAcceptAuthorList(0),
  m_doRequireAuthorList(false),
  m_idxRequireAuthorList(0),
  m_doVetoAuthorList(false),
  m_idxVetoAuthorList(0),
  m_doUncombinedEt(false),
  m_idxUncombinedEt(0),
  m_doUncombinedE(false),
  m_idxUncombinedE(0),
  m_doUncombinedEta(false),
  m_idxUncombinedEta(0),
  m_doUncombinedAbsEta(false),
  m_idxUncombinedAbsEta(0),
  m_doUncombinedPhi(false),
  m_idxUncombinedPhi(0),
  m_doUncombinedMass(false),
  m_idxUncombinedMass(0),
  m_doClusterEt(false),
  m_idxClusterEt(0),
  m_doClusterE(false),
  m_idxClusterE(0),
  m_doClusterEta(false),
  m_idxClusterEta(0),
  m_doClusterAbsEta(false),
  m_idxClusterAbsEta(0),
  m_doClusterEtaVeto(false),
  m_idxClusterEtaVeto(0),
  m_doClusterPhi(false),
  m_idxClusterPhi(0),
  m_doClusterPhiVeto(false),
  m_idxClusterPhiVeto(0),
  m_doClusterMass(false),
  m_idxClusterMass(0),
  m_doClusterLayerE(false),
  m_idxClusterLayerE(0),
  m_doClusterLayerEta(false),
  m_idxClusterLayerEta(0),
  m_doClusterLayerAbsEta(false),
  m_idxClusterLayerAbsEta(0),
  m_doClusterLayerEtaVeto(false),
  m_idxClusterLayerEtaVeto(0),
  m_doClusterLayerPhi(false),
  m_idxClusterLayerPhi(0),
  m_doClusterLayerPhiVeto(false),
  m_idxClusterLayerPhiVeto(0),
  m_doTrackPt(false),
  m_idxTrackPt(0),
  m_doTrackP(false),
  m_idxTrackP(0),
  m_doTrackEta(false),
  m_idxTrackEta(0),
  m_doTrackAbsEta(false),
  m_idxTrackAbsEta(0),
  m_doTrackEtaVeto(false),
  m_idxTrackEtaVeto(0),
  m_doTrackPhi(false),
  m_idxTrackPhi(0),
  m_doTrackPhiVeto(false),
  m_idxTrackPhiVeto(0),
  m_doTrackMass(false),
  m_idxTrackMass(0),
  m_doClusterTrackEt(false),
  m_idxClusterTrackEt(0),
  m_doTrackPtIso(false),
  m_idxTrackPtIso(0),
  m_doAbsTrackPtIso(false),
  m_idxAbsTrackPtIso(0),
  m_doCaloEtIso(false),
  m_idxCaloEtIso(0),
  m_doAbsCaloEtIso(false),
  m_idxAbsCaloEtIso(0),
  m_doEGammaParam(false),
  m_idxEGammaParam(0),
  m_tmp4Mom(0)
{
  declareProperty("clusterKinematicSelectorTool",
                  m_clusterKinSelectorTool,
                  "Kinematic selection tool for clusters" );

  declareProperty("clusterCaloLayerKinematicSelectorTool",
                  m_clusterCaloLayerKinSelectorTool,
                  "Kinematic selection tool for clusters for specific layer in the EM calorimeter" );

  declareProperty("trackKinematicSelectorTool",
                  m_trackKinSelectorTool,
                  "Kinematic selection tool for tracks" );

  declareProperty("electronIsEM",         m_isEM=0,    "EGamma IsEM Bitmask" );
  m_isEM.declareUpdateHandler( &D2PDElectronSelector::setupIsEM, this );

  declareProperty("electronID",           m_passID=-1, "EGamma ID quality" );
  m_passID.declareUpdateHandler( &D2PDElectronSelector::setupPassID, this );

  declareProperty("electronAuthor",       m_author=0,  "EGamma author" );
  m_author.declareUpdateHandler( &D2PDElectronSelector::setupAuthor, this );

  declareProperty("electronAcceptAuthorList",  m_acceptAuthorList,  "EGamma author list for accept (logical OR combination)" );
  declareProperty("electronRequireAuthorList", m_requireAuthorList, "EGamma author list for require (logical AND combination)" );
  declareProperty("electronVetoAuthorList",    m_vetoAuthorList,    "EGamma author list for veto (logical NOT combination)" );


  declareProperty("uncombinedEtMin",         m_uncombinedEtMin=-DBL_MAX, "Uncombined min E_T" );
  m_uncombinedEtMin.declareUpdateHandler( &D2PDElectronSelector::setupUncombinedEt, this );
  declareProperty("uncombinedEtMax",         m_uncombinedEtMax=DBL_MAX,  "Uncombined max E_T" );
  m_uncombinedEtMax.declareUpdateHandler( &D2PDElectronSelector::setupUncombinedEt, this );

  declareProperty("uncombinedEMin",          m_uncombinedEMin=-DBL_MAX,  "Uncombined min E" );
  m_uncombinedEMin.declareUpdateHandler( &D2PDElectronSelector::setupUncombinedE, this );
  declareProperty("uncombinedEMax",          m_uncombinedEMax=DBL_MAX,   "Uncombined max E" );
  m_uncombinedEMax.declareUpdateHandler( &D2PDElectronSelector::setupUncombinedE, this );

  declareProperty("uncombinedEtaMin",        m_uncombinedEtaMin=-DBL_MAX, "Uncombined min eta" );
  m_uncombinedEtaMin.declareUpdateHandler( &D2PDElectronSelector::setupUncombinedEta, this );
  declareProperty("uncombinedEtaMax",        m_uncombinedEtaMax=DBL_MAX,  "Uncombined max eta" );
  m_uncombinedEtaMax.declareUpdateHandler( &D2PDElectronSelector::setupUncombinedEta, this );

  declareProperty("uncombinedAbsEtaMin",     m_uncombinedAbsEtaMin=-DBL_MAX, "Uncombined min |eta|" );
  m_uncombinedAbsEtaMin.declareUpdateHandler( &D2PDElectronSelector::setupUncombinedAbsEta, this );
  declareProperty("uncombinedAbsEtaMax",     m_uncombinedAbsEtaMax=DBL_MAX,  "Uncombined max |eta|" );
  m_uncombinedAbsEtaMax.declareUpdateHandler( &D2PDElectronSelector::setupUncombinedAbsEta, this );

  declareProperty("uncombinedPhiMin",        m_uncombinedPhiMin=-DBL_MAX, "Uncombined min phi" );
  m_uncombinedPhiMin.declareUpdateHandler( &D2PDElectronSelector::setupUncombinedPhi, this );
  declareProperty("uncombinedPhiMax",        m_uncombinedPhiMax=DBL_MAX,  "Uncombined max phi" );
  m_uncombinedPhiMax.declareUpdateHandler( &D2PDElectronSelector::setupUncombinedPhi, this );

  declareProperty("uncombinedMassMin",       m_uncombinedMassMin=-DBL_MAX, "Uncombined min mass" );
  m_uncombinedMassMin.declareUpdateHandler( &D2PDElectronSelector::setupUncombinedMass, this );
  declareProperty("uncombinedMassMax",       m_uncombinedMassMax=DBL_MAX,  "Uncombined max mass" );
  m_uncombinedMassMax.declareUpdateHandler( &D2PDElectronSelector::setupUncombinedMass, this );


  declareProperty("clusterEtMin",         m_clusterEtMin=-DBL_MAX, "Cluster min E_T" );
  m_clusterEtMin.declareUpdateHandler( &D2PDElectronSelector::setupClusterEt, this );
  declareProperty("clusterEtMax",         m_clusterEtMax=DBL_MAX,  "Cluster max E_T" );
  m_clusterEtMax.declareUpdateHandler( &D2PDElectronSelector::setupClusterEt, this );

  declareProperty("clusterEMin",          m_clusterEMin=-DBL_MAX,  "Cluster min E" );
  m_clusterEMin.declareUpdateHandler( &D2PDElectronSelector::setupClusterE, this );
  declareProperty("clusterEMax",          m_clusterEMax=DBL_MAX,   "Cluster max E" );
  m_clusterEMax.declareUpdateHandler( &D2PDElectronSelector::setupClusterE, this );

  declareProperty("clusterEtaMin",        m_clusterEtaMin=-DBL_MAX, "Cluster min eta" );
  m_clusterEtaMin.declareUpdateHandler( &D2PDElectronSelector::setupClusterEta, this );
  declareProperty("clusterEtaMax",        m_clusterEtaMax=DBL_MAX,  "Cluster max eta" );
  m_clusterEtaMax.declareUpdateHandler( &D2PDElectronSelector::setupClusterEta, this );

  declareProperty("clusterAbsEtaMin",     m_clusterAbsEtaMin=-DBL_MAX, "Cluster min |eta|" );
  m_clusterAbsEtaMin.declareUpdateHandler( &D2PDElectronSelector::setupClusterAbsEta, this );
  declareProperty("clusterAbsEtaMax",     m_clusterAbsEtaMax=DBL_MAX,  "Cluster max |eta|" );
  m_clusterAbsEtaMax.declareUpdateHandler( &D2PDElectronSelector::setupClusterAbsEta, this );

  declareProperty("clusterEtaVetoRanges", m_clusterEtaVetoRanges="", "Cluster eta-ranges to veto" );
  m_clusterEtaVetoRanges.declareUpdateHandler( &D2PDElectronSelector::setupClusterEtaVeto, this );

  declareProperty("clusterPhiMin",        m_clusterPhiMin=-DBL_MAX, "Cluster min phi" );
  m_clusterPhiMin.declareUpdateHandler( &D2PDElectronSelector::setupClusterPhi, this );
  declareProperty("clusterPhiMax",        m_clusterPhiMax=DBL_MAX,  "Cluster max phi" );
  m_clusterPhiMax.declareUpdateHandler( &D2PDElectronSelector::setupClusterPhi, this );

  declareProperty("clusterPhiVetoRanges", m_clusterPhiVetoRanges="", "Cluster phi-ranges to veto" );
  m_clusterPhiVetoRanges.declareUpdateHandler( &D2PDElectronSelector::setupClusterPhiVeto, this );

  declareProperty("clusterMassMin",       m_clusterMassMin=-DBL_MAX, "Cluster min mass" );
  m_clusterMassMin.declareUpdateHandler( &D2PDElectronSelector::setupClusterMass, this );
  declareProperty("clusterMassMax",       m_clusterMassMax=DBL_MAX,  "Cluster max mass" );
  m_clusterMassMax.declareUpdateHandler( &D2PDElectronSelector::setupClusterMass, this );


  declareProperty("clusterEMCaloLayer",              m_emCaloLayer=-1,
                  "Layer in EM calorimeter to use (default -1: skip selections)" );

  declareProperty("clusterEMCaloLayerEMin",          m_clusterCaloLayerEMin=-DBL_MAX,
                  "Cluster min E in specific EM calorimeter layer" );
  m_clusterCaloLayerEMin.declareUpdateHandler( &D2PDElectronSelector::setupClusterLayerE, this );
  declareProperty("clusterEMCaloLayerEMax",          m_clusterCaloLayerEMax=DBL_MAX,
                  "Cluster max E in specific EM calorimeter layer" );
  m_clusterCaloLayerEMax.declareUpdateHandler( &D2PDElectronSelector::setupClusterLayerE, this );

  declareProperty("clusterEMCaloLayerEtaMin",        m_clusterCaloLayerEtaMin=-DBL_MAX,
                  "Cluster min eta in specific EM calorimeter layer" );
  m_clusterCaloLayerEtaMin.declareUpdateHandler( &D2PDElectronSelector::setupClusterLayerEta, this );
  declareProperty("clusterEMCaloLayerEtaMax",        m_clusterCaloLayerEtaMax=DBL_MAX,
                  "Cluster max eta in specific EM calorimeter layer" );
  m_clusterCaloLayerEtaMax.declareUpdateHandler( &D2PDElectronSelector::setupClusterLayerEta, this );

  declareProperty("clusterEMCaloLayerAbsEtaMin",     m_clusterCaloLayerAbsEtaMin=-DBL_MAX,
                  "Cluster min |eta| in specific EM calorimeter layer" );
  m_clusterCaloLayerAbsEtaMin.declareUpdateHandler( &D2PDElectronSelector::setupClusterLayerAbsEta, this );
  declareProperty("clusterEMCaloLayerAbsEtaMax",     m_clusterCaloLayerAbsEtaMax=DBL_MAX,
                  "Cluster max |eta| in specific EM calorimeter layer" );
  m_clusterCaloLayerAbsEtaMax.declareUpdateHandler( &D2PDElectronSelector::setupClusterLayerAbsEta, this );

  declareProperty("clusterEMCaloLayerEtaVetoRanges", m_clusterCaloLayerEtaVetoRanges="",
                  "Cluster eta-ranges to veto in specific EM calorimeter layer" );
  m_clusterCaloLayerEtaVetoRanges.declareUpdateHandler( &D2PDElectronSelector::setupClusterLayerEtaVeto, this );

  declareProperty("clusterEMCaloLayerPhiMin",        m_clusterCaloLayerPhiMin=-DBL_MAX,
                  "Cluster min phi in specific EM calorimeter layer" );
  m_clusterCaloLayerPhiMin.declareUpdateHandler( &D2PDElectronSelector::setupClusterLayerPhi, this );
  declareProperty("clusterEMCaloLayerPhiMax",        m_clusterCaloLayerPhiMax=DBL_MAX,
                  "Cluster max phi in specific EM calorimeter layer" );
  m_clusterCaloLayerPhiMax.declareUpdateHandler( &D2PDElectronSelector::setupClusterLayerPhi, this );

  declareProperty("clusterEMCaloLayerPhiVetoRanges", m_clusterCaloLayerPhiVetoRanges="",
                  "Cluster phi-ranges to veto in specific EM calorimeter layer" );
  m_clusterCaloLayerPhiVetoRanges.declareUpdateHandler( &D2PDElectronSelector::setupClusterLayerPhiVeto, this );


  declareProperty("trackPtMin",         m_trackPtMin=-DBL_MAX, "Track min P_T" );
  m_trackPtMin.declareUpdateHandler( &D2PDElectronSelector::setupTrackPt, this );
  declareProperty("trackPtMax",         m_trackPtMax=DBL_MAX,  "Track max P_T" );
  m_trackPtMax.declareUpdateHandler( &D2PDElectronSelector::setupTrackPt, this );

  declareProperty("trackPMin",          m_trackPMin=-DBL_MAX,  "Track min P" );
  m_trackPMin.declareUpdateHandler( &D2PDElectronSelector::setupTrackP, this );
  declareProperty("trackPMax",          m_trackPMax=DBL_MAX,   "Track max P" );
  m_trackPMax.declareUpdateHandler( &D2PDElectronSelector::setupTrackP, this );

  declareProperty("trackEtaMin",        m_trackEtaMin=-DBL_MAX, "Track min eta" );
  m_trackEtaMin.declareUpdateHandler( &D2PDElectronSelector::setupTrackEta, this );
  declareProperty("trackEtaMax",        m_trackEtaMax=DBL_MAX,  "Track max eta" );
  m_trackEtaMax.declareUpdateHandler( &D2PDElectronSelector::setupTrackEta, this );

  declareProperty("trackAbsEtaMin",     m_trackAbsEtaMin=-DBL_MAX, "Track min |eta|" );
  m_trackAbsEtaMin.declareUpdateHandler( &D2PDElectronSelector::setupTrackAbsEta, this );
  declareProperty("trackAbsEtaMax",     m_trackAbsEtaMax=DBL_MAX,  "Track max |eta|" );
  m_trackAbsEtaMax.declareUpdateHandler( &D2PDElectronSelector::setupTrackAbsEta, this );

  declareProperty("trackEtaVetoRanges", m_trackEtaVetoRanges="", "Track eta-ranges to veto" );
  m_trackEtaVetoRanges.declareUpdateHandler( &D2PDElectronSelector::setupTrackEtaVeto, this );

  declareProperty("trackPhiMin",        m_trackPhiMin=-DBL_MAX, "Track min phi" );
  m_trackPhiMin.declareUpdateHandler( &D2PDElectronSelector::setupTrackPhi, this );
  declareProperty("trackPhiMax",        m_trackPhiMax=DBL_MAX,  "Track max phi" );
  m_trackPhiMax.declareUpdateHandler( &D2PDElectronSelector::setupTrackPhi, this );

  declareProperty("trackPhiVetoRanges", m_trackPhiVetoRanges="", "Track phi-ranges to veto" );
  m_trackPhiVetoRanges.declareUpdateHandler( &D2PDElectronSelector::setupTrackPhiVeto, this );

  declareProperty("trackMassMin",       m_trackMassMin=-DBL_MAX, "Track min mass" );
  m_trackMassMin.declareUpdateHandler( &D2PDElectronSelector::setupTrackMass, this );
  declareProperty("trackMassMax",       m_trackMassMax=DBL_MAX,  "Track max mass" );
  m_trackMassMax.declareUpdateHandler( &D2PDElectronSelector::setupTrackMass, this );


  declareProperty("clusterTrackEtMin",  m_clusterTrackEtMin=-DBL_MAX,  "ClusterE/cosh(trackEta) min" );
  m_clusterTrackEtMin.declareUpdateHandler( &D2PDElectronSelector::setupClusterTrackEt, this );
  declareProperty("clusterTrackEtMax",  m_clusterTrackEtMax=DBL_MAX,   "ClusterE/cosh(trackEta) max" );
  m_clusterTrackEtMax.declareUpdateHandler( &D2PDElectronSelector::setupClusterTrackEt, this );


  declareProperty("egDetailContainer",  m_egammaDetailKey="egDetailAOD", "egDetailContainer key" );

  declareProperty("egammaTrackIsolationParameter", m_egammaTrackIsoParam=-1,
                  "the egammaParameter for tracking isolation" );
  declareProperty("egammaCaloIsolationParameter",  m_egammaCaloIsoParam=-1,
                  "the egammaParameter for calorimeter isolation" );

  declareProperty("relativeTrackPtIsolationMin",   m_relTrackPtIsoMin=-DBL_MAX,
                  "minimum relative track pt isolation cut" );
  m_relTrackPtIsoMin.declareUpdateHandler( &D2PDElectronSelector::setupTrackPtIso, this );
  declareProperty("relativeTrackPtIsolationMax",   m_relTrackPtIsoMax=DBL_MAX,
                  "maximum relative track pt isolation cut" );
  m_relTrackPtIsoMax.declareUpdateHandler( &D2PDElectronSelector::setupTrackPtIso, this );

  declareProperty("absoluteTrackPtIsolationMin",   m_absTrackPtIsoMin=-DBL_MAX,
                  "minimum absolute track pt isolation cut" );
  m_absTrackPtIsoMin.declareUpdateHandler( &D2PDElectronSelector::setupAbsTrackPtIso, this );
  declareProperty("absoluteTrackPtIsolationMax",   m_absTrackPtIsoMax=DBL_MAX,
                  "maximum absolute track pt isolation cut" );
  m_absTrackPtIsoMax.declareUpdateHandler( &D2PDElectronSelector::setupAbsTrackPtIso, this );

  declareProperty("relativeCaloEtIsolationMin",    m_relCaloEtIsoMin=-DBL_MAX,
                  "minimum relative calorimeter et isolation cut" );
  m_relCaloEtIsoMin.declareUpdateHandler( &D2PDElectronSelector::setupCaloEtIso, this );
  declareProperty("relativeCaloEtIsolationMax",    m_relCaloEtIsoMax=DBL_MAX,
                  "maximum relative calorimeter et isolation cut" );
  m_relCaloEtIsoMax.declareUpdateHandler( &D2PDElectronSelector::setupCaloEtIso, this );

  declareProperty("absoluteCaloEtIsolationMin",    m_absCaloEtIsoMin=-DBL_MAX,
                  "minimum absolute calorimeter et isolation cut" );
  m_absCaloEtIsoMin.declareUpdateHandler( &D2PDElectronSelector::setupAbsCaloEtIso, this );
  declareProperty("absoluteCaloEtIsolationMax",    m_absCaloEtIsoMax=DBL_MAX,
                  "maximum absolute calorimeter et isolation cut" );
  m_absCaloEtIsoMax.declareUpdateHandler( &D2PDElectronSelector::setupAbsCaloEtIso, this );


  declareProperty("egammaParameterList",       m_egammaParameterList,       "egammaParameter list for selection" );
  declareProperty("egammaParameterCutMinList", m_egammaParameterCutMinList, "egammaParameter lower cut list" );
  declareProperty("egammaParameterCutMaxList", m_egammaParameterCutMaxList, "egammaParameter upper cut list" );

}





//=============================================================================
// Destructor
//=============================================================================
D2PDElectronSelector::~D2PDElectronSelector()
{
}






//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode D2PDElectronSelector::initializeMore()
{
  // Print the used configuration
  if ( m_printSetup || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "==> initializeMore " << name() << "..." );
      
      // Print out the used configuration
      ATH_MSG_INFO ( " using electronIsEM                          = " << m_isEM );
      ATH_MSG_INFO ( " using electronID                            = " << m_passID );
      ATH_MSG_INFO ( " using electronAuthor                        = " << m_author );

      ATH_MSG_INFO ( " using electronAcceptAuthorList              = " << m_acceptAuthorList  );
      ATH_MSG_INFO ( " using electronRequireAuthorList             = " << m_requireAuthorList );
      ATH_MSG_INFO ( " using electronVetoAuthorList                = " << m_vetoAuthorList    );


      ATH_MSG_INFO ( " using uncombinedEtMin                       = " << m_uncombinedEtMin );
      ATH_MSG_INFO ( " using uncombinedEtMax                       = " << m_uncombinedEtMax );

      ATH_MSG_INFO ( " using uncombinedEMin                        = " << m_uncombinedEMin );
      ATH_MSG_INFO ( " using uncombinedEMax                        = " << m_uncombinedEMax );

      ATH_MSG_INFO ( " using uncombinedEtaMin                      = " << m_uncombinedEtaMin );
      ATH_MSG_INFO ( " using uncombinedEtaMax                      = " << m_uncombinedEtaMax );

      ATH_MSG_INFO ( " using uncombinedAbsEtaMin                   = " << m_uncombinedAbsEtaMin );
      ATH_MSG_INFO ( " using uncombinedAbsEtaMax                   = " << m_uncombinedAbsEtaMax );

      ATH_MSG_INFO ( " using uncombinedPhiMin                      = " << m_uncombinedPhiMin );
      ATH_MSG_INFO ( " using uncombinedPhiMax                      = " << m_uncombinedPhiMax );

      ATH_MSG_INFO ( " using uncombinedMassMin                     = " << m_uncombinedMassMin );
      ATH_MSG_INFO ( " using uncombinedMassMax                     = " << m_uncombinedMassMax );


      ATH_MSG_INFO ( " using clusterEtMin                          = " << m_clusterEtMin );
      ATH_MSG_INFO ( " using clusterEtMax                          = " << m_clusterEtMax );

      ATH_MSG_INFO ( " using clusterEMin                           = " << m_clusterEMin );
      ATH_MSG_INFO ( " using clusterEMax                           = " << m_clusterEMax );

      ATH_MSG_INFO ( " using clusterEtaMin                         = " << m_clusterEtaMin );
      ATH_MSG_INFO ( " using clusterEtaMax                         = " << m_clusterEtaMax );

      ATH_MSG_INFO ( " using clusterAbsEtaMin                      = " << m_clusterAbsEtaMin );
      ATH_MSG_INFO ( " using clusterAbsEtaMax                      = " << m_clusterAbsEtaMax );

      ATH_MSG_INFO ( " using clusterEtaVetoRanges                  = " << m_clusterEtaVetoRanges );

      ATH_MSG_INFO ( " using clusterPhiMin                         = " << m_clusterPhiMin );
      ATH_MSG_INFO ( " using clusterPhiMax                         = " << m_clusterPhiMax );

      ATH_MSG_INFO ( " using clusterPhiVetoRanges                  = " << m_clusterPhiVetoRanges );

      ATH_MSG_INFO ( " using clusterMassMin                        = " << m_clusterMassMin );
      ATH_MSG_INFO ( " using clusterMassMax                        = " << m_clusterMassMax );


      ATH_MSG_INFO ( " using clusterEMCaloLayer                    = " << m_emCaloLayer );

      ATH_MSG_INFO ( " using clusterEMCaloLayerEMin                = " << m_clusterCaloLayerEMin );
      ATH_MSG_INFO ( " using clusterEMCaloLayerEMax                = " << m_clusterCaloLayerEMax );

      ATH_MSG_INFO ( " using clusterEMCaloLayerEtaMin              = " << m_clusterCaloLayerEtaMin );
      ATH_MSG_INFO ( " using clusterEMCaloLayerEtaMax              = " << m_clusterCaloLayerEtaMax );

      ATH_MSG_INFO ( " using clusterEMCaloLayerAbsEtaMin           = " << m_clusterCaloLayerAbsEtaMin );
      ATH_MSG_INFO ( " using clusterEMCaloLayerAbsEtaMax           = " << m_clusterCaloLayerAbsEtaMax );

      ATH_MSG_INFO ( " using clusterEMCaloLayerEtaVetoRanges       = " << m_clusterCaloLayerEtaVetoRanges );

      ATH_MSG_INFO ( " using clusterEMCaloLayerPhiMin              = " << m_clusterCaloLayerPhiMin );
      ATH_MSG_INFO ( " using clusterEMCaloLayerPhiMax              = " << m_clusterCaloLayerPhiMax );

      ATH_MSG_INFO ( " using clusterEMCaloLayerPhiVetoRanges       = " << m_clusterCaloLayerPhiVetoRanges );


      ATH_MSG_INFO ( " using trackPtMin                            = " << m_trackPtMin );
      ATH_MSG_INFO ( " using trackPtMax                            = " << m_trackPtMax );

      ATH_MSG_INFO ( " using trackPMin                             = " << m_trackPMin );
      ATH_MSG_INFO ( " using trackPMax                             = " << m_trackPMax );

      ATH_MSG_INFO ( " using trackEtaMin                           = " << m_trackEtaMin );
      ATH_MSG_INFO ( " using trackEtaMax                           = " << m_trackEtaMax );

      ATH_MSG_INFO ( " using trackAbsEtaMin                        = " << m_trackAbsEtaMin );
      ATH_MSG_INFO ( " using trackAbsEtaMax                        = " << m_trackAbsEtaMax );

      ATH_MSG_INFO ( " using trackEtaVetoRanges                    = " << m_trackEtaVetoRanges );

      ATH_MSG_INFO ( " using trackPhiMin                           = " << m_trackPhiMin );
      ATH_MSG_INFO ( " using trackPhiMax                           = " << m_trackPhiMax );

      ATH_MSG_INFO ( " using trackPhiVetoRanges                    = " << m_trackPhiVetoRanges );

      ATH_MSG_INFO ( " using trackMassMin                          = " << m_trackMassMin );
      ATH_MSG_INFO ( " using trackMassMax                          = " << m_trackMassMax );


      ATH_MSG_INFO ( " using egDetailContainer                     = " << m_egammaDetailKey );

      ATH_MSG_INFO ( " using egammaTrackIsolationParameter         = " << m_egammaTrackIsoParam );
      ATH_MSG_INFO ( " using egammaCaloIsolationParameter          = " << m_egammaCaloIsoParam );

      ATH_MSG_INFO ( " using relativeTrackPtIsolationMin           = " << m_relTrackPtIsoMin );
      ATH_MSG_INFO ( " using relativeTrackPtIsolationMax           = " << m_relTrackPtIsoMax );

      ATH_MSG_INFO ( " using absoluteTrackPtIsolationMin           = " << m_absTrackPtIsoMin );
      ATH_MSG_INFO ( " using absoluteTrackPtIsolationMax           = " << m_absTrackPtIsoMax );

      ATH_MSG_INFO ( " using relativeCaloEtIsolationMin            = " << m_relCaloEtIsoMin );
      ATH_MSG_INFO ( " using relativeCaloEtIsolationMax            = " << m_relCaloEtIsoMax );

      ATH_MSG_INFO ( " using absoluteCaloEtIsolationMin            = " << m_absCaloEtIsoMin );
      ATH_MSG_INFO ( " using absoluteCaloEtIsolationMax            = " << m_absCaloEtIsoMax );


      ATH_MSG_INFO ( " using clusterKinematicSelectorTool          = " << m_clusterKinSelectorTool );
      ATH_MSG_INFO ( " using clusterCaloLayerKinematicSelectorTool = " << m_clusterCaloLayerKinSelectorTool );
      ATH_MSG_INFO ( " using trackKinematicSelectorTool            = " << m_trackKinSelectorTool );


      ATH_MSG_INFO ( " using egammaParameterList                   = " << m_egammaParameterList );
      ATH_MSG_INFO ( " using egammaParameterCutMinList             = " << m_egammaParameterCutMinList );
      ATH_MSG_INFO ( " using egammaParameterCutMaxList             = " << m_egammaParameterCutMaxList );
    } // End: if ( m_printSetup )



  // Perform some consistency checks
  bool allOK(true);
  if ( m_egammaParameterCutMinList.size() != 0 && m_egammaParameterList.size() != m_egammaParameterCutMinList.size() )
    {
      allOK = false;
      ATH_MSG_ERROR ( "The egammaParameterCutMinList has not zero size, but the size is different from the egammaParameterList!"
                      << " Please fix your configuration..." );
    }
  if ( m_egammaParameterCutMaxList.size() != 0 && m_egammaParameterList.size() != m_egammaParameterCutMaxList.size() )
    {
      allOK = false;
      ATH_MSG_ERROR ( "The egammaParameterCutMaxList has not zero size, but the size is different from the egammaParameterList!"
                      << " Please fix your configuration..." );
    }
  if ( !allOK )
    {
      return StatusCode::FAILURE;
    }




  // get the kinematic selector tool (derived class from ISelectorCore)
  // for the calorimeter clusters
  ATH_CHECK( m_clusterKinSelectorTool.retrieve() );
  if ( m_printSetup || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "ClusterKinSelectorTool:" << m_clusterKinSelectorTool );
    }


  // get the kinematic selector tool (derived class from ISelectorCore)
  // for the calorimeter clusters in certain EM calorimeter samplings
  ATH_CHECK( m_clusterCaloLayerKinSelectorTool.retrieve() );
  if ( m_printSetup || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "ClusterEMCaloLayerKinSelectorTool:" << m_clusterCaloLayerKinSelectorTool );
    }


  // get the kinematic selector tool (derived class from ISelectorCore)
  // for the tracks
  ATH_CHECK( m_trackKinSelectorTool.retrieve() );
  if ( m_printSetup || msgLvl(MSG::DEBUG) )
    {
      ATH_MSG_INFO ( "TrackKinSelectorTool:" << m_trackKinSelectorTool );
    }



  //--------------------------------------------------------------------------
  // Set the cut values for the selection tools
  //--------------------------------------------------------------------------

  //-----------------------
  // Electron quantities
  //-----------------------


  //-----------------------
  // Cluster quantities
  //-----------------------
  m_clusterKinSelectorTool->getTool()->setminEt( m_clusterEtMin );
  m_clusterKinSelectorTool->getTool()->setmaxEt( m_clusterEtMax );

  m_clusterKinSelectorTool->getTool()->setminE( m_clusterEMin );
  m_clusterKinSelectorTool->getTool()->setmaxE( m_clusterEMax );

  m_clusterKinSelectorTool->getTool()->setminEta( m_clusterEtaMin );
  m_clusterKinSelectorTool->getTool()->setmaxEta( m_clusterEtaMax );

  m_clusterKinSelectorTool->getTool()->setminAbsEta( m_clusterAbsEtaMin );
  m_clusterKinSelectorTool->getTool()->setmaxAbsEta( m_clusterAbsEtaMax );

  m_clusterKinSelectorTool->getTool()->setEtaVetoRanges( (std::string&)m_clusterEtaVetoRanges.value() );

  m_clusterKinSelectorTool->getTool()->setminPhi( m_clusterPhiMin );
  m_clusterKinSelectorTool->getTool()->setmaxPhi( m_clusterPhiMax );

  m_clusterKinSelectorTool->getTool()->setPhiVetoRanges( (std::string&)m_clusterPhiVetoRanges.value() );

  m_clusterKinSelectorTool->getTool()->setminMass( m_clusterMassMin );
  m_clusterKinSelectorTool->getTool()->setmaxMass( m_clusterMassMax );


  //-----------------------
  // Cluster quantities in certain EM calorimeter samplings
  //-----------------------
  m_clusterCaloLayerKinSelectorTool->getTool()->setminE( m_clusterCaloLayerEMin );
  m_clusterCaloLayerKinSelectorTool->getTool()->setmaxE( m_clusterCaloLayerEMax );

  m_clusterCaloLayerKinSelectorTool->getTool()->setminEta( m_clusterCaloLayerEtaMin );
  m_clusterCaloLayerKinSelectorTool->getTool()->setmaxEta( m_clusterCaloLayerEtaMax );

  m_clusterCaloLayerKinSelectorTool->getTool()->setminAbsEta( m_clusterCaloLayerAbsEtaMin );
  m_clusterCaloLayerKinSelectorTool->getTool()->setmaxAbsEta( m_clusterCaloLayerAbsEtaMax );

  m_clusterCaloLayerKinSelectorTool->getTool()->setEtaVetoRanges( (std::string&)m_clusterCaloLayerEtaVetoRanges.value() );

  m_clusterCaloLayerKinSelectorTool->getTool()->setminPhi( m_clusterCaloLayerPhiMin );
  m_clusterCaloLayerKinSelectorTool->getTool()->setmaxPhi( m_clusterCaloLayerPhiMax );

  m_clusterCaloLayerKinSelectorTool->getTool()->setPhiVetoRanges( (std::string&)m_clusterCaloLayerPhiVetoRanges.value() );


  //-----------------------
  // Track quantities
  //-----------------------
  m_trackKinSelectorTool->getTool()->setminPt( m_trackPtMin );
  m_trackKinSelectorTool->getTool()->setmaxPt( m_trackPtMax );

  m_trackKinSelectorTool->getTool()->setminP( m_trackPMin );
  m_trackKinSelectorTool->getTool()->setmaxP( m_trackPMax );

  m_trackKinSelectorTool->getTool()->setminEta( m_trackEtaMin );
  m_trackKinSelectorTool->getTool()->setmaxEta( m_trackEtaMax );

  m_trackKinSelectorTool->getTool()->setminAbsEta( m_trackAbsEtaMin );
  m_trackKinSelectorTool->getTool()->setmaxAbsEta( m_trackAbsEtaMax );

  m_trackKinSelectorTool->getTool()->setEtaVetoRanges( (std::string&)m_trackEtaVetoRanges.value() );

  m_trackKinSelectorTool->getTool()->setminPhi( m_trackPhiMin );
  m_trackKinSelectorTool->getTool()->setmaxPhi( m_trackPhiMax );

  m_trackKinSelectorTool->getTool()->setPhiVetoRanges( (std::string&)m_trackPhiVetoRanges.value() );

  m_trackKinSelectorTool->getTool()->setminMass( m_trackMassMin );
  m_trackKinSelectorTool->getTool()->setmaxMass( m_trackMassMax );



  //--------------------------------------------------------------------------
  // Create the needed objects
  //--------------------------------------------------------------------------
  m_tmp4Mom = new CaloCluster();



  if ( m_egammaParameterList.size() > 0 ) { m_doEGammaParam = true; }
  if ( m_acceptAuthorList.size() > 0 ) { m_doAcceptAuthorList = true; }
  if ( m_requireAuthorList.size() > 0 ) { m_doRequireAuthorList = true; }
  if ( m_vetoAuthorList.size() > 0 ) { m_doVetoAuthorList = true; }



  // Setup the internal cutflow stuff
  if ( m_printInternalCutflow )
    {

      if ( m_doIsEM )
        {
          m_idxIsEM = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "IsEM=%ld", m_isEM.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doPassID )
        {
          m_idxPassID = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "passID=%ld", m_passID.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doAuthor )
        {
          m_idxAuthor = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "author=%ld", m_author.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doAcceptAuthorList )
        {
          m_idxAcceptAuthorList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doAcceptAuthorList=1", std::make_pair(0,0) ) );
        }
      if ( m_doRequireAuthorList )
        {
          m_idxRequireAuthorList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doRequireAuthorList=1", std::make_pair(0,0) ) );
        }
      if ( m_doVetoAuthorList )
        {
          m_idxVetoAuthorList = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doVetoAuthorList=1", std::make_pair(0,0) ) );
        }
      if ( m_doUncombinedEt )
        {
          m_idxUncombinedEt = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < uncombined et < %g",
                                                          m_uncombinedEtMin.value(), m_uncombinedEtMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doUncombinedE )
        {
          m_idxUncombinedE = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < uncombined e < %g",
                                                          m_uncombinedEMin.value(), m_uncombinedEMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doUncombinedEta )
        {
          m_idxUncombinedEta = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < uncombined eta < %g",
                                                          m_uncombinedEtaMin.value(), m_uncombinedEtaMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doUncombinedAbsEta )
        {
          m_idxUncombinedAbsEta = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < uncombined |eta| < %g",
                                                          m_uncombinedAbsEtaMin.value(), m_uncombinedAbsEtaMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doUncombinedPhi )
        {
          m_idxUncombinedPhi = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < uncombined phi < %g",
                                                          m_uncombinedPhiMin.value(), m_uncombinedPhiMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doUncombinedMass )
        {
          m_idxUncombinedMass = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < uncombined mass < %g",
                                                          m_uncombinedMassMin.value(), m_uncombinedMassMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doClusterEt )
        {
          m_idxClusterEt = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < cluster et < %g",
                                                          m_clusterEtMin.value(), m_clusterEtMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doClusterE )
        {
          m_idxClusterE = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < cluster e < %g",
                                                          m_clusterEMin.value(), m_clusterEMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doClusterEta )
        {
          m_idxClusterEta = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < cluster eta < %g",
                                                          m_clusterEtaMin.value(), m_clusterEtaMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doClusterAbsEta )
        {
          m_idxClusterAbsEta = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < cluster |eta| < %g",
                                                          m_clusterAbsEtaMin.value(), m_clusterAbsEtaMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doClusterEtaVeto )
        {
          m_idxClusterEtaVeto = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( m_clusterEtaVetoRanges.value(), std::make_pair(0,0) ) );
        }
      if ( m_doClusterPhi )
        {
          m_idxClusterPhi = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < cluster phi < %g",
                                                          m_clusterPhiMin.value(), m_clusterPhiMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doClusterMass )
        {
          m_idxClusterMass = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < cluster mass < %g",
                                                          m_clusterMassMin.value(), m_clusterMassMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doClusterLayerE )
        {
          m_idxClusterLayerE = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < cluster layer e < %g",
                                                          m_clusterCaloLayerEMin.value(), m_clusterCaloLayerEMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doClusterLayerEta )
        {
          m_idxClusterLayerEta = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < cluster layer eta < %g",
                                                          m_clusterCaloLayerEtaMin.value(), m_clusterCaloLayerEtaMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doClusterLayerAbsEta )
        {
          m_idxClusterLayerAbsEta = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < cluster layer |eta| < %g",
                                                          m_clusterCaloLayerAbsEtaMin.value(), m_clusterCaloLayerAbsEtaMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doClusterLayerEtaVeto )
        {
          m_idxClusterLayerEtaVeto = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( m_clusterCaloLayerEtaVetoRanges.value(), std::make_pair(0,0) ) );
        }
      if ( m_doClusterLayerPhi )
        {
          m_idxClusterLayerPhi = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < cluster layer phi < %g",
                                                          m_clusterCaloLayerPhiMin.value(), m_clusterCaloLayerPhiMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doClusterLayerPhiVeto )
        {
          m_idxClusterLayerPhiVeto = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( m_clusterCaloLayerPhiVetoRanges.value(), std::make_pair(0,0) ) );
        }
      if ( m_doTrackPt )
        {
          m_idxTrackPt = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < track pt < %g",
                                                          m_trackPtMin.value(), m_trackPtMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doTrackP )
        {
          m_idxTrackP = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < track p < %g",
                                                          m_trackPMin.value(), m_trackPMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doTrackEta )
        {
          m_idxTrackEta = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < track eta < %g",
                                                          m_trackEtaMin.value(), m_trackEtaMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doTrackAbsEta )
        {
          m_idxTrackAbsEta = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < track |eta| < %g",
                                                          m_trackAbsEtaMin.value(), m_trackAbsEtaMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doTrackEtaVeto )
        {
          m_idxTrackEtaVeto = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( m_trackEtaVetoRanges.value(), std::make_pair(0,0) ) );
        }
      if ( m_doTrackPhi )
        {
          m_idxTrackPhi = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < track phi < %g",
                                                          m_trackPhiMin.value(), m_trackPhiMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doTrackPhiVeto )
        {
          m_idxTrackPhiVeto = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( m_trackPhiVetoRanges.value(), std::make_pair(0,0) ) );
        }
      if ( m_doTrackMass )
        {
          m_idxTrackMass = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < track mass < %g",
                                                          m_trackMassMin.value(), m_trackMassMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doClusterTrackEt )
        {
          m_idxClusterTrackEt = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < cluster-track et < %g",
                                                          m_clusterTrackEtMin.value(), m_clusterTrackEtMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doTrackPtIso )
        {
          m_idxTrackPtIso = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < track pt iso < %g",
                                                          m_relTrackPtIsoMin.value(), m_relTrackPtIsoMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doAbsTrackPtIso )
        {
          m_idxAbsTrackPtIso = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < absolute track pt iso < %g",
                                                          m_absTrackPtIsoMin.value(), m_absTrackPtIsoMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doCaloEtIso )
        {
          m_idxCaloEtIso = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < calo et iso < %g",
                                                          m_relCaloEtIsoMin.value(), m_relCaloEtIsoMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doAbsCaloEtIso )
        {
          m_idxAbsCaloEtIso = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( Form( "%g < absolute calo et iso < %g",
                                                          m_absCaloEtIsoMin.value(), m_absCaloEtIsoMax.value() ), std::make_pair(0,0) ) );
        }
      if ( m_doEGammaParam )
        {
          m_idxEGammaParam = m_objCutFlow.size();
          m_objCutFlow.push_back(0);
          m_eventCutFlow.push_back( std::make_pair( "doEGammaParam=1", std::make_pair(0,0) ) );
        }
    }


  return StatusCode::SUCCESS;

}








//=============================================================================
// D2PDMaker processObject method
// This method will be called ONCE PER OBJECT!
//=============================================================================
StatusCode D2PDElectronSelector::processObject( const Analysis::Electron* electron,
                                                bool &isPassed )
{
  //-----------------------------------------
  // Do the selection
  //-----------------------------------------

  // Make sure we have an electron returned by the dynamic_cast
  if ( electron )
    {
      //--------------------------
      // Do the actual selections
      //--------------------------
      // IsEM selection
      if ( isPassed && m_doIsEM )
        {
          isPassed = ( electron->isem(m_isEM) == 0 );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxIsEM] += 1; }
        }

      // Identification selection
      if ( isPassed && m_doPassID && m_passID >= 0 )
        {
          isPassed = electron->passID( ((egammaPID::egammaIDQuality)m_passID.value()) );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxPassID] += 1; }
        }

      // Electron author selection
      if ( isPassed && m_doAuthor && m_author != 0 )
        {
          isPassed = ( electron->author(m_author) == true );
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxAuthor] += 1; }
        }

      // Electron author accept list selection
      if ( isPassed && m_doAcceptAuthorList )
        {
          // Deal with the logical OR combination from the m_acceptAuthorList
          bool passAccept(false);
          if ( m_acceptAuthorList.size() == 0 )
            {
              passAccept = true;
            }
          std::vector<long>::iterator acceptItr    = m_acceptAuthorList.begin();
          std::vector<long>::iterator acceptItrEnd = m_acceptAuthorList.end();
          for ( ; acceptItr != acceptItrEnd; ++acceptItr )
            {
              passAccept = passAccept || ( electron->author((*acceptItr)) == true );
            } // End: loop over acceptAuthorList
          isPassed = passAccept;
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxAcceptAuthorList] += 1; }
        }

      // Electron author require list selection
      if ( isPassed && m_doRequireAuthorList )
        {
          // Deal with the logical AND combination from the m_requireAuthorList
          std::vector<long>::iterator requireItr    = m_requireAuthorList.begin();
          std::vector<long>::iterator requireItrEnd = m_requireAuthorList.end();
          for ( ; requireItr != requireItrEnd; ++requireItr )
            {
              isPassed = isPassed && ( electron->author((*requireItr)) == true );
            } // End: loop over requireAuthorList
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxRequireAuthorList] += 1; }
        }

      // Electron author veto list selection
      if ( isPassed && m_doVetoAuthorList )
        {
          // Deal with the logical NOT combination from the m_vetoAuthorList
          bool passVeto(false);
          std::vector<long>::iterator vetoItr    = m_vetoAuthorList.begin();
          std::vector<long>::iterator vetoItrEnd = m_vetoAuthorList.end();
          for ( ; vetoItr != vetoItrEnd; ++vetoItr )
            {
              passVeto = passVeto || ( electron->author((*vetoItr)) == true );
            } // End: loop over vetoAuthorList
          isPassed = !passVeto;
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxVetoAuthorList] += 1; }
        }



      //----------------------------------------------------------
      // Get the EMShower for this electron (needed later on)
      //----------------------------------------------------------
      const EMShower* emshower(NULL);
      if ( m_doCaloEtIso || m_doAbsCaloEtIso
           || m_doTrackPtIso || m_doAbsTrackPtIso
           || m_doEGammaParam 
           )
        {
          emshower = electron->detail<EMShower>(m_egammaDetailKey);
        }


      //----------------------------------------------------------
      // Get the uncombined 4-momentum for this electron
      //----------------------------------------------------------
      const egamma::momentum_type& uncombined = electron->get4Mom(egamma::Uncombined);

      //----------------------------------------------------------
      // Do the uncombined 4-momentum selections
      //----------------------------------------------------------
      // transverse energy selection
      if ( isPassed && m_doUncombinedEt )
        {
          isPassed = m_uncombinedEtMin < uncombined.et() && uncombined.et() < m_uncombinedEtMax;
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxUncombinedEt] += 1; }
        }
      // energy selection
      if ( isPassed && m_doUncombinedE )
        {
          isPassed = m_uncombinedEMin < uncombined.e() && uncombined.e() < m_uncombinedEMax;
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxUncombinedE] += 1; }
        }
      // eta selection
      if ( isPassed && m_doUncombinedEta )
        {
          isPassed = m_uncombinedEtaMin < uncombined.eta() && uncombined.eta() < m_uncombinedEtaMax;
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxUncombinedEta] += 1; }
        }
      // |eta| selection
      if ( isPassed && m_doUncombinedAbsEta )
        {
          isPassed = m_uncombinedAbsEtaMin < fabs(uncombined.eta()) && fabs(uncombined.eta()) < m_uncombinedAbsEtaMax;
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxUncombinedAbsEta] += 1; }
        }
      // phi selection
      if ( isPassed && m_doUncombinedPhi )
        {
          isPassed = m_uncombinedPhiMin < uncombined.phi() && uncombined.phi() < m_uncombinedPhiMax;
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxUncombinedPhi] += 1; }
        }
      // mass selection
      if ( isPassed && m_doUncombinedMass )
        {
          isPassed = m_uncombinedMassMin < uncombined.m() && uncombined.m() < m_uncombinedMassMax;
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxUncombinedMass] += 1; }
        }




      //----------------------------------------------------------
      // Get the cluster for this electron
      //----------------------------------------------------------
      const CaloCluster* cluster(NULL);

      if ( m_doClusterEt || m_doClusterE || m_doClusterEta || m_doClusterAbsEta || m_doClusterEtaVeto
           || m_doClusterPhi || m_doClusterPhiVeto || m_doClusterMass 
           || m_doClusterLayerE || m_doClusterLayerEta || m_doClusterLayerAbsEta
           || m_doClusterLayerEtaVeto || m_doClusterLayerPhi || m_doClusterLayerPhiVeto
           || m_doCaloEtIso || m_doAbsCaloEtIso
           || m_doTrackPtIso || m_doClusterTrackEt )
        {
          cluster = electron->cluster();
        }
      if ( cluster )
        {

          //----------------------------------------------------------
          // Do cluster selections
          //----------------------------------------------------------
          // transverse energy selection
          if ( isPassed && m_doClusterEt )
            {
              isPassed = m_clusterKinSelectorTool->getTool()->passEt( cluster );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxClusterEt] += 1; }
            }
          // energy selection
          if ( isPassed && m_doClusterE )
            {
              isPassed = m_clusterKinSelectorTool->getTool()->passE( cluster );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxClusterE] += 1; }
            }
          // eta selection
          if ( isPassed && m_doClusterEta )
            {
              isPassed = m_clusterKinSelectorTool->getTool()->passEta( cluster );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxClusterEta] += 1; }
            }
          // |eta| selection
          if ( isPassed && m_doClusterAbsEta )
            {
              isPassed = m_clusterKinSelectorTool->getTool()->passAbsEta( cluster );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxClusterAbsEta] += 1; }
            }
          // eta veto selection
          if ( isPassed && m_doClusterEtaVeto )
            {
              isPassed = m_clusterKinSelectorTool->getTool()->passEtaVetoRanges( cluster );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxClusterEtaVeto] += 1; }
            }
          // phi selection
          if ( isPassed && m_doClusterPhi )
            {
              isPassed = m_clusterKinSelectorTool->getTool()->passPhi( cluster );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxClusterPhi] += 1; }
            }
          // phi veto selection
          if ( isPassed && m_doClusterPhiVeto )
            {
              isPassed = m_clusterKinSelectorTool->getTool()->passPhiVetoRanges( cluster );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxClusterPhiVeto] += 1; }
            }
          // mass selection
          if ( isPassed && m_doClusterMass )
            {
              isPassed = m_clusterKinSelectorTool->getTool()->passMass( cluster );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxClusterMass] += 1; }
            }

          //---------------------------------------
          // Determine the cluster selection in a certain EM calorimeter sampling
          //---------------------------------------
          if ( isPassed && ( m_emCaloLayer == 0 || m_emCaloLayer == 1 || m_emCaloLayer == 2 || m_emCaloLayer == 3 ) )
            {
              // First, use the temporary CaloCluster to pass to the selection tool
              double energy(0.0);
              double eta(0.0);
              double phi(0.0);
              energy = cluster->energyBE( m_emCaloLayer );
              eta    = cluster->etaBE( m_emCaloLayer );
              phi    = cluster->phiBE( m_emCaloLayer );
              m_tmp4Mom->setE( energy );
              m_tmp4Mom->setEta( eta );
              m_tmp4Mom->setPhi( phi );
              m_tmp4Mom->setM( 0.0 );

              // energy selection
              if ( isPassed && m_doClusterLayerE )
                {
                  isPassed = m_clusterCaloLayerKinSelectorTool->getTool()->passE( m_tmp4Mom );
                  if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxClusterLayerE] += 1; }
                }
              // eta selection
              if ( isPassed && m_doClusterLayerEta )
                {
                  isPassed = m_clusterCaloLayerKinSelectorTool->getTool()->passEta( m_tmp4Mom );
                  if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxClusterLayerEta] += 1; }
                }
              // |eta| selection
              if ( isPassed && m_doClusterLayerAbsEta )
                {
                  isPassed = m_clusterCaloLayerKinSelectorTool->getTool()->passAbsEta( m_tmp4Mom );
                  if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxClusterLayerAbsEta] += 1; }
                }
              // eta veto selection
              if ( isPassed && m_doClusterLayerEtaVeto )
                {
                  isPassed = m_clusterCaloLayerKinSelectorTool->getTool()->passEtaVetoRanges( m_tmp4Mom );
                  if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxClusterLayerEtaVeto] += 1; }
                }
              // phi selection
              if ( isPassed && m_doClusterLayerPhi )
                {
                  isPassed = m_clusterCaloLayerKinSelectorTool->getTool()->passPhi( m_tmp4Mom );
                  if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxClusterLayerPhi] += 1; }
                }
              // phi veto selection
              if ( isPassed && m_doClusterLayerPhiVeto )
                {
                  isPassed = m_clusterCaloLayerKinSelectorTool->getTool()->passPhiVetoRanges( m_tmp4Mom );
                  if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxClusterLayerPhiVeto] += 1; }
                }

            } // End: if ( isPassed and emCaloLayer is ok )



          //---------------------------------------
          // Determine the egamma calorimeter isolation
          //---------------------------------------
          if ( isPassed && ( m_doCaloEtIso || m_doAbsCaloEtIso ) && m_egammaCaloIsoParam != -1 )
            {
              if (emshower)
                {
                  double coneEt(-99999.0);
                  coneEt = emshower->parameter( (egammaParameters::ParamDef)m_egammaCaloIsoParam );

                  // egamma relative calo isolation
                  if ( isPassed && m_doCaloEtIso )
                    {
                      if ( cluster->et() != 0.0 )
                        {
                          isPassed = ( ( (coneEt/(cluster->et())) <= m_relCaloEtIsoMax ) 
                                       && ( m_relCaloEtIsoMin  <= (coneEt/(cluster->et())) ) ); 
                        }
                      if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxCaloEtIso] += 1; }
                    }
                  // egamma absolute calo isolation
                  if ( isPassed && m_doAbsCaloEtIso )
                    {
                      isPassed = ( ( coneEt <= m_absCaloEtIsoMax ) && ( m_absCaloEtIsoMin <= coneEt ) );
                      if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxAbsCaloEtIso] += 1; }
                    }
                }
              else
                {
                  ATH_MSG_WARNING ( "Null pointer for EMShower object" );
                }

            } // End: if ( isPassed && m_egammaCaloIsoParam != -1 )

        } // End: if cluster
	  



      //----------------------------------------------------------
      // Get the TrackParticle for this electron
      //----------------------------------------------------------
      const Rec::TrackParticle* trackPart(NULL);
      if ( m_doTrackPt || m_doTrackP || m_doTrackEta || m_doTrackAbsEta || m_doTrackEtaVeto
           || m_doTrackPhi || m_doTrackPhiVeto || m_doTrackMass
           || m_doTrackPtIso || m_doAbsTrackPtIso
           || m_doClusterTrackEt
           )
        {
          trackPart = electron->trackParticle();
        }
      if ( trackPart )
        {
          //----------------------------------------------------------
          // Do track selections
          //----------------------------------------------------------
          // transverse energy selection
          if ( m_doTrackPt && isPassed )
            {
              isPassed = m_trackKinSelectorTool->getTool()->passEt( trackPart );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxTrackPt] += 1; }
            }
          // energy selection
          if ( m_doTrackP && isPassed )
            {
              isPassed = m_trackKinSelectorTool->getTool()->passE( trackPart );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxTrackP] += 1; }
            }
          // eta selection
          if ( m_doTrackEta && isPassed )
            {
              isPassed = m_trackKinSelectorTool->getTool()->passEta( trackPart );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxTrackEta] += 1; }
            }
          // |eta| selection
          if ( m_doTrackAbsEta && isPassed )
            {
              isPassed = m_trackKinSelectorTool->getTool()->passAbsEta( trackPart );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxTrackAbsEta] += 1; }
            }
          // eta veto selection
          if ( m_doTrackEtaVeto && isPassed )
            {
              isPassed = m_trackKinSelectorTool->getTool()->passEtaVetoRanges( trackPart );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxTrackEtaVeto] += 1; }
            }
          // phi selection
          if ( m_doTrackPhi && isPassed )
            {
              isPassed = m_trackKinSelectorTool->getTool()->passPhi( trackPart );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxTrackPhi] += 1; }
            }
          // phi veto selection
          if ( m_doTrackPhiVeto && isPassed )
            {
              isPassed = m_trackKinSelectorTool->getTool()->passPhiVetoRanges( trackPart );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxTrackPhiVeto] += 1; }
            }
          // mass selection
          if ( m_doTrackMass && isPassed )
            {
              isPassed = m_trackKinSelectorTool->getTool()->passMass( trackPart );
              if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxTrackMass] += 1; }
            }

          //---------------------------------------
          // Determine the egamma tracking isolation
          //---------------------------------------
          if ( isPassed && ( m_doTrackPtIso || m_doAbsTrackPtIso ) && m_egammaTrackIsoParam != -1 )
            {
              if (emshower)
                {
                  double conePt(-99999.0);
                  conePt = emshower->parameter( (egammaParameters::ParamDef)m_egammaTrackIsoParam );

                  // egamma relative track isolation
                  if ( m_doTrackPtIso && isPassed )
                    {
                      if ( cluster && cluster->et() != 0.0 )
                        {
                          const double clusEt = cluster->et();
                          const double relcone = conePt / clusEt;
                          isPassed = ( ( relcone <= m_relTrackPtIsoMax ) 
                                       && ( m_relTrackPtIsoMin  <= relcone ) ); 
                        }
                      if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxTrackPtIso] += 1; }
                    }
                  // egamma absolute track isolation
                  if ( m_doAbsTrackPtIso && isPassed )
                    {
                      isPassed = ( ( conePt <= m_absTrackPtIsoMax ) && ( m_absTrackPtIsoMin <= conePt ) );
                      if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxAbsTrackPtIso] += 1; }
                    }
                }
              else
                {
                  ATH_MSG_WARNING ( "Null pointer for EMShower object" );
                }

            } // End: if ( isPassed && m_egammaTrackIsoParam != -1 )
      
          // Do the Cluster-track (by-hand) combined quantities
          if ( cluster )
            {
              if ( m_doClusterTrackEt && isPassed )
                {
                  double clusTrackEt(0.0);
                  double trackCoshEta = cosh( trackPart->eta() );
                  if ( trackCoshEta != 0.0 )
                    {
                      clusTrackEt = cluster->e()/trackCoshEta;
                    }
                  isPassed = (  m_clusterTrackEtMin <= clusTrackEt  &&  clusTrackEt <=  m_clusterTrackEtMax  );
                  if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxClusterTrackEt] += 1; }
                }
            } // End: if cluster
          
        } // End: if trackPart



      // Add the cuts on the egammaParameters
      if ( isPassed && m_doEGammaParam && emshower )
        {
          for ( unsigned int i=0; i < m_egammaParameterList.size(); ++i )
            {
              double tmpParam = (double)emshower->parameter( (egammaParameters::ParamDef)m_egammaParameterList[i] );
              bool passLowerCut(true);
              bool passUpperCut(true);
              if ( m_egammaParameterList.size() == m_egammaParameterCutMinList.size() )
                {
                  passLowerCut = passLowerCut && ( m_egammaParameterCutMinList[i] <= tmpParam );
                }
              if ( m_egammaParameterList.size() == m_egammaParameterCutMaxList.size() )
                {
                  passUpperCut = passUpperCut && ( m_egammaParameterCutMaxList[i] >= tmpParam );
                }
              if ( isPassed && passLowerCut && passUpperCut )
                {
                  isPassed = true;
                }
              else
                {
                  isPassed = false;
                }
            } // End: loop over egammaParameters
          if ( isPassed && m_printInternalCutflow ) { m_objCutFlow[m_idxEGammaParam] += 1; }
        } // End: if ( isPassed )

    }
  else
    {
      // Return false 
      isPassed = false;
      ATH_MSG_WARNING ( "Could not dynamic_cast to an electron! Failing this electron!" );
    } // End: if/else have electron
  
  
  return StatusCode::SUCCESS;
}







//=============================================================================
// D2PDMaker finalizeMore method
// This method will be called ONCE PER EVENT!
//=============================================================================
StatusCode D2PDElectronSelector::finalizeMore()
{
  // Releasing the used tools
  ATH_CHECK( m_clusterKinSelectorTool.release() );
  ATH_CHECK( m_clusterCaloLayerKinSelectorTool.release() );
  ATH_CHECK( m_trackKinSelectorTool.release() );

  // Deleting the pointers
  if ( m_tmp4Mom ) delete m_tmp4Mom;


  return StatusCode::SUCCESS;
}



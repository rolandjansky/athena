/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     SoftElectronTag.cxx
PACKAGE:  offline/PhysicsAnalysis/JetTagging/JetTagTools

AUTHORS:  F. Derue, A. Kaczmarska, M.Wolter
CREATED:  Jan 2005

PURPOSE:  b-tagging based on soft lepton identification

COMMENTS: evolved from LifetimeTag

UPDATE: AK,MW - 10.02.2006 - some track quality cuts added
        FD    - 06.02.2008 - migration to COOL
        FD    - 10.04.2008 - track quality cuts + m_Db m_Du normalized
        FD    - 17.05.2008 - add access to Photons (for conversions)
        FD    - 23.06.2008 - m_originalElCollection absent  = warning only + d0
        FD    - 27.10.2008 - reorder calls of weights, pTrel and d0
        FD    - 15.01.2009 - use of AthAlgTool and other changes +
                             adapt to log10(weight) given by egamma
********************************************************************/
#include "JetTagTools/SoftElectronTag.h"

#include "GeoPrimitives/GeoPrimitives.h"

//#include "JetEvent/Jet.h"
#include "JetTagInfo/SoftElectronInfo.h"
#include "Navigation/NavigationToken.h" 

#include "egammaEvent/Photon.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronAssociation.h"
#include "egammaEvent/PhotonAssociation.h"

#include "JetTagTools/TrackSelector.h"
#include "JetTagTools/SVForIPTool.h"
#include "JetTagTools/ITrackGradeFactory.h"
#include "Navigation/NavigationToken.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"

#include "ParticleTruth/TrackParticleTruth.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

#include "JetTagInfo/TruthInfo.h"
#include "JetTagInfo/SoftLeptonTruthInfo.h"
#include "JetTagInfo/SLTrueInfo.h"

#include "JetTagTools/HistoHelperRoot.h"
#include "JetTagTools/NewLikelihoodTool.h"
#include "JetTagCalibration/CalibrationBroker.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"

namespace Analysis 
{
  
SoftElectronTag::SoftElectronTag(const std::string& t, const std::string& n, const IInterface* p)
  : AthAlgTool(t,n,p),
    m_trackToVertexTool("Reco::TrackToVertex"),
    m_trackSelectorTool("Analysis::TrackSelector"),
    m_trackToVertexIPEstimator("Trk::TrackToVertexIPEstimator"),
    m_SVForIPTool("Analysis::SVForIPTool"),
    m_unbiasIPEstimation(false),
    m_likelihoodTool("Analysis::NewLikelihoodTool"),
    m_histoHelper(0),
    m_calibrationTool("BTagCalibrationBroker"),
    m_secVxFinderNameForV0Removal("InDetVKalVxInJetTool"),
    m_secVxFinderNameForIPSign("InDetVKalVxInJetTool")
{

  // declare interface
  declareInterface<ITagTool>(this);
  
  //
  declareProperty("Runmodus",     m_runModus     = "analysis");
  //
  declareProperty("writeInfoPlus", m_writeInfoPlus = false);

  // Name of the electron input collection
  declareProperty("originalElCollectionName", 
		  m_originalElCollectionName = "ElectronAODCollection",
		  "Name of the electron input collection");
  // Boolean to use photon collection
  declareProperty("usePhoton", m_usePhoton = true);
  // Name of the photon input collection
  declareProperty("originalPhCollectionName", 
		  m_originalPhCollectionName = "PhotonAODCollection",
		  "Name of the photon input collection");
  // Name of the jet input collection
  declareProperty("jetCollectionList", m_jetCollectionList,
		  "Name of the jet input collection");
  // Name of the track particle container
  declareProperty("TPContainer",        
		  m_TPContainerName      = "TrackParticleCandidate",
		  "Name of the track particle container");
  // Name of the track particle truth container
  declareProperty("TPTruthContainer",
		  m_TPTruthContainerName = "TrackParticleTruthCollection",
		  "Name of the track particle truth container");
  // Type of data to analyse in reference mode (B, UDSG, ALL)
  declareProperty("referenceType",      m_referenceType        = "B",
		  "Type of data to analyse in reference mode (B, UDSG, ALL)"); 
  // Distance for purification
  declareProperty("purificationDeltaR", m_purificationDeltaR   = 0.8,
		  "Distance for purification");
  // Distance of isolation to electrons
  declareProperty("elecIsolDeltaR",     m_elecIsolDeltaR      = 0.7,
		  "Distance of isolation to electrons");
  // Cut on minimum pT for jets
  declareProperty("jetPtMinRef",        m_jetPtMinRef          = 15.*Gaudi::Units::GeV,
		  "Cut on minimum pT for jets");
  // Cut on maximum eta for jets
  declareProperty("jetEtaMaxRef",       m_jetEtaMaxRef         = 2.5,
		  "Cut on maximum eta for jets");

  // global configuration:
  declareProperty("SVForIPTool", m_SVForIPTool);
  // tools:
  declareProperty("trackSelectorTool", m_trackSelectorTool);
  declareProperty("trackToVertexTool", m_trackToVertexTool);
  // Threshold for high-threshold hits in TRT
  declareProperty("HTRTThr", m_thrTRT=0.05);
  declareProperty("LikelihoodTool", m_likelihoodTool);
  // Tools for calibration
  declareProperty("calibrationTool",	m_calibrationTool);
  declareProperty("useForcedCalibration",  m_doForcedCalib   = false);
  declareProperty("ForcedCalibrationName", m_ForcedCalibName = "Cone4H1Tower");
  // Boolean to use transverse impact parameter
  declareProperty("UseTransverseIP", m_useTransverseIP = true,
		  "Boolean to use transverse impact parameter");
  // Boolean to have unbiased IP estimator
  declareProperty("unbiasIPEstimation",m_unbiasIPEstimation,
		  "Boolean to have unbiased IP estimator");
  // tool for IP estimator
  declareProperty("TrackToVertexIPEstimator",m_trackToVertexIPEstimator,
		  "tool for IP estimator");
  // Boolean to use cut-based identification of electrons
  // if not use ratio of likelihood (default)
  declareProperty("UseCutBased", m_useCutBased = false,
		  "Boolean to use cut-based identification of electrons - if not use ratio of likelihood (default)");
  
  // hypotheses 
  m_hypotheses.push_back("Sig");
  m_hypotheses.push_back("Bkg");
}

  
SoftElectronTag::~SoftElectronTag()
{
}

// ==================================================
StatusCode SoftElectronTag::initialize()
{
  // retrieving ToolSvc: 
  IToolSvc* toolSvc;
  StatusCode sc = service("ToolSvc", toolSvc);
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_ERROR(" Can't get ToolSvc");
    return StatusCode::FAILURE;
  }
    
  // retrieving TrackToVertex: 
  if (m_runModus == "reference" || m_writeInfoPlus) {
    if ( m_trackToVertexTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_trackToVertexTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("Retrieved tool " << m_trackToVertexTool);
    }
  } 

  // retrieving trackToVertexIPEstimator
  if (m_trackToVertexIPEstimator.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_trackToVertexIPEstimator);
  } else {
    ATH_MSG_DEBUG("Retrieved tool " << m_trackToVertexIPEstimator);
  }

  // creation of TrackSelector: (private instance) 
  if ( m_trackSelectorTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_trackSelectorTool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved tool " << m_trackSelectorTool);
  }
  
  // retrieving calibrationTool
  if ( m_calibrationTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_calibrationTool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved tool " << m_calibrationTool);
  }

  // If the jet author is not known (or one wants a calibration not 
  // corresponding to the author), can force the calibration. 
  // Check that this calibration has been loaded
  if (m_doForcedCalib) {
    if (std::find( m_jetCollectionList.begin(), 
		   m_jetCollectionList.end(), 
		   m_ForcedCalibName ) == m_jetCollectionList.end()) {
      ATH_MSG_ERROR("Error, forced calibration to an unloaded one");
      return StatusCode::FAILURE;
    }
  }
  
  // book histograms in reference mode 
  if (m_runModus == "reference") {
    sc = InitReferenceMode();
    if (sc==StatusCode::FAILURE) return StatusCode::FAILURE;
  }
  
  // read in calibration histograms 
  if( m_runModus == "analysis" ) {
    sc = InitAnalysisMode();
    if (sc==StatusCode::FAILURE) return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

// ==================================================================
StatusCode SoftElectronTag::InitReferenceMode() 
{
  // ===========================================
  // Booking of histograms in reference mode
  // ===========================================
  
  StatusCode sc = StatusCode::SUCCESS;

  ITHistSvc* myHistoSvc;
  if( service( "THistSvc", myHistoSvc ).isSuccess() ) {
    ATH_MSG_DEBUG("HistoSvc loaded successfully."); 
    // define Histo Helper
    m_histoHelper = new HistoHelperRoot(myHistoSvc);
    // Check for overflows ?
    m_histoHelper->setCheckOverflows(false);
    // loop on all Jet collection
    for(uint ijc=0;ijc<m_jetCollectionList.size();ijc++) {
      // loop on hypotheses
      for(uint ih=0;ih<m_hypotheses.size();ih++) {
	std::string hName = "/RefFile/SoftEl/"+m_jetCollectionList[ijc]+"/"+
	  m_hypotheses[ih]+"/";
	
	m_histoHelper->bookHisto(hName+"d0","|Transverse Impact Parameter|",
				 100,0.,1.);
	m_histoHelper->bookHisto(hName+"pTrel","Electron pT wrt jet axis",
				 100,0.,5.);
	m_histoHelper->bookHisto(hName+"D","Discriminant",
				 50,-30.,20.);
	m_histoHelper->bookHisto(hName+"DpT","Discriminant vs pTrel",
				 25,0.,5.,25,-30.,20.);
      }
    }
    m_histoHelper->print();
  } else {
    ATH_MSG_ERROR("HistoSvc could NOT bo loaded.");
  }
  
  return sc;
}

// ==================================================================
StatusCode SoftElectronTag::InitAnalysisMode() 
{
  // ===========================================
  // Retrieve histograms in analysis mode
  // ===========================================

  StatusCode sc = StatusCode::SUCCESS;
   
  // configure likelihood
  if ( m_likelihoodTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_likelihoodTool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved tool " << m_likelihoodTool);
  }
  
  m_likelihoodTool->defineHypotheses(m_hypotheses);
  std::string hDir;

  // loop on hypotheses
  for(uint ih=0;ih<m_hypotheses.size();ih++) {
    
    //hDir="/RefFile/"+m_jetCollectionList[ijc]+"/"+m_hypotheses[ih]+"/";
    hDir=m_hypotheses[ih]+"/";
    
    // use of pTrel
    m_likelihoodTool->defineHistogram(hDir+"pTrel");
    
    // use of transverse impact parameter
    if (m_useTransverseIP) 
      m_likelihoodTool->defineHistogram(hDir+"d0");
    
    // Weight of the likelihood (from electron-id only)
    //m_likelihoodTool->defineHistogram(hDir+"D");
    
    // Weight of the likelihood vs pTrel
    //m_likelihoodTool->defineHistogram(hDir+"DpT");
    
    // }
  }
  m_likelihoodTool->printStatus();

  return sc;
}

// ==========================================================
StatusCode SoftElectronTag::finalize()
{
  return StatusCode::SUCCESS;
}

// ==========================================================
void SoftElectronTag::tagJet(xAOD::Jet& jetToTag)
{
  // If one wants to add a pointer to the best Electron, needs the container. 
  // Retrieved for each jet. Does another way to do this exist ? 
  if(m_writeInfoPlus) {
    bool ppb = true;
    StatusCode sc = evtStore()->retrieve(m_originalElCollection, m_originalElCollectionName);
    
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Electron collection " << m_originalElCollectionName 
		      << " not found");
    } else {
      ATH_MSG_VERBOSE("Electron collection " << m_originalElCollectionName 
		      << " found");
      ppb = false;
    }
    if(ppb) {
      ATH_MSG_VERBOSE("Not able to persistify electron infos ! Exiting...");
      return;
    }
    
    // retrieve the photon collection
    if (m_usePhoton) {
      sc = evtStore()->retrieve(m_originalPhCollection, m_originalPhCollectionName);
      if (sc.isFailure()) {
	ATH_MSG_WARNING("Photon collection " << m_originalPhCollectionName 
			<< " not found");
      } else {
	ATH_MSG_VERBOSE("Photon collection " << m_originalPhCollectionName 
			<< " found");
	ppb = false;
      }
    }
  
    if(ppb) {
      ATH_MSG_VERBOSE("Not able to persistify photon infos ! Exiting...");
      return;
    }
  }

  // author to know which jet algorithm: 
  m_author = jetToTag.jetAuthor();
  if (m_doForcedCalib) {
    m_author = m_ForcedCalibName;
  } else { 
    //Check that this author is known in the calibration
    if (std::find( m_jetCollectionList.begin(), 
		   m_jetCollectionList.end(), 
		   m_author ) == m_jetCollectionList.end()) {
      ATH_MSG_DEBUG("Jet Algorithm not found in the standard list"); 
      ATH_MSG_DEBUG("Trying to find a similar one...");
      if      (m_author.find("Cone4",0) != std::string::npos) 
	m_author = "Cone4H1Tower";
      else if (m_author.find("Cone7",0) != std::string::npos) 
	m_author = "Cone7H1Tower";
      else if (m_author.find("Kt4",0)   != std::string::npos) 
	m_author = "Kt4H1Tower";
      else if (m_author.find("Kt6",0)   != std::string::npos) 
	m_author = "Kt6H1Tower";
      else {
	ATH_MSG_DEBUG("None found, taking " << m_ForcedCalibName << " calibration");
	m_author = m_ForcedCalibName;
      }
    }
  }
  
  // Create the info class and append it to the Jet 
  m_softeInfo = 0;
  m_bestSoftE = 0;

  // look for the best electron candidate
  LookForBestElectron(jetToTag);
  // Return if there are not enough electrons left after preselection
  if (m_eleCounter<1) {
    ATH_MSG_VERBOSE("Jet does not contain any good electrons");
    return;
  }
  ATH_MSG_DEBUG(" Max. probability for jet: "  << m_totalproMax);
  
  // retrieve information from the best electron
  RetrieveInfoBestElectron(jetToTag);
  
  // run the part of the tagging algorithm done only in analysis mode
  if (m_runModus == "analysis") 
    JetTagAnalysisMode(jetToTag);

  // run the part of the tagging algorithm done only in reference mode
  if (m_runModus == "reference" && m_bestSoftE) 
    JetTagReferenceMode(jetToTag);
  
  return;
}


// ======================================================
void SoftElectronTag::LookForBestElectron(xAOD::Jet& jetToTag)
{
  //
  // Extract the Electrons from the jet and apply preselection on the track.
  // Ignore other constituents for now
  //

  m_eleCounter       = 0;
  m_bestSoftIsPhoton = false;
  m_totalproMax      = 0.;

  // initialisation
  m_d0wrtPvx    = 0.;
  m_d0ErrwrtPvx = 0.;
  m_signOfIP    = 1.;
  m_ptrel       = 0.;
  m_Db          = 0.; 
  m_Du          = 0.;

  // retrieve the electron associated to the jet
  UseElectrons(jetToTag);

  // Is the softE misidentified as a member of photon conversion
  if (m_usePhoton) UsePhotons(jetToTag);
}

// ======================================================
void SoftElectronTag::UseElectrons(xAOD::Jet& jetToTag)
{
  //
  // Extract the Electrons from the jet
  //
  
  // retrieve the electron associated to the jet
  const ElectronAssociation *ec = jetToTag.getAssociation<ElectronAssociation>("Electrons");
  if (ec == 0) {
    ATH_MSG_DEBUG("No electron assocation");
  } else {
    // loop on objects
    for(Navigable<ElectronContainer,double>::object_iter j = ec->begin(); j!= ec->end(); ++j) {
      const Electron *eTemp = *j;
      ATH_MSG_DEBUG("eTemp = " << eTemp 
		    << " author " << eTemp->author() 
		    << " pt = " << eTemp->pt() 
		    << " eta = " << eTemp->eta() 
		    << " phi = " << eTemp->phi());
      if (eTemp) {
	// if no track is associated
	if (!eTemp->trackParticle()) {
	  //ATH_MSG_WARNING("Electron without a track ! This should not exist !");
	} else {
	  if ( m_preselect(eTemp->trackParticle()) && 
	       eTemp->author(egammaParameters::AuthorSofte)) {
	    m_eleCounter++;
	    ATH_MSG_DEBUG("Looking at " << m_eleCounter 
			  << " electron, isemse " << eTemp->isemse() 
			  <<" author "<<eTemp->author());
	    
	    // in case of use of the electron-id based on weights
	    double totalpro = 0.;
	    if (!m_useCutBased) {
	      // combine electron weights + pTrel + d0
	      totalpro = CombineWeights(jetToTag,eTemp,1);
	    } else
	      // in case of use of the cut based selection 
	      if (eTemp->isemse()&egammaPID::ElectronTight) {
		// combine weights of pTrel + d0
		totalpro = CombineWeights(jetToTag,eTemp,2);
	      }
	    ATH_MSG_DEBUG(" total prob "<<totalpro);
	    // use by default the ratio of likelihood selection
	    m_totalproVector.push_back(totalpro);
	    if ( totalpro > m_totalproMax ) {
	      m_totalproMax = totalpro;		  
	      m_isemse    = eTemp->isemse();
	      m_bestSoftE = eTemp;
	    }
	  }
	}
      }
    }
  }
}

// ======================================================
void SoftElectronTag::UsePhotons(xAOD::Jet& jetToTag)
{
  //
  // Extract the Photons from the jet to use electrons from conversions
  // or electrons mis-identififed as photons
  //

  // retrieve association between photons and jets
  const PhotonAssociation *ph = jetToTag.getAssociation<PhotonAssociation>("Photons");
  if (ph == 0) {
    ATH_MSG_DEBUG("No photon assocation");
    return;
  }
  ATH_MSG_DEBUG("Now looking for photons in the photon container");
  // loop on photons in jet container
  for(Navigable<PhotonContainer,double>::object_iter j = ph->begin(); j!= ph->end(); ++j) {
    const Photon *eTemp = *j;
    ATH_MSG_DEBUG("eTemp = " << eTemp 
		  << " author " << eTemp->author() 
		  << " pt = " << eTemp->pt() 
		  << " eta = " << eTemp->eta() 
		  << " phi = " << eTemp->phi() 
		  << " TrackMatch = " << eTemp->trackParticle());
    //
    if (eTemp) {
      // if no track is associated it cannot be an electron from conversion
      if (eTemp->trackParticle()) {
	if ( m_preselect(eTemp->trackParticle()) && 
	     eTemp->author(egammaParameters::AuthorSofte)) {
	  m_eleCounter++;
	  ATH_MSG_DEBUG("Looking at " << m_eleCounter 
			<< " photon, isemse " << eTemp->isemse() 
			<<" author "<<eTemp->author());
	  
	  double totalpro = 0.;
	  // in case of use of the electron-id based on weights
	  if (!m_useCutBased) {
	    // combine electron weights + pTrel + d0
	    totalpro = CombineWeights(jetToTag,eTemp,1);
	  } else
	    // in case of use of the cut based selection 
	    if (eTemp->isemse()&egammaPID::ElectronTight) {
	      // combine weights of pTrel + d0
	      totalpro = CombineWeights(jetToTag,eTemp,2);
	    }
	  ATH_MSG_DEBUG(" total prob "<<totalpro);
	  // use by default the ratio of likelihood selection
	  m_totalproVector.push_back(totalpro);
	  if ( totalpro > m_totalproMax ) {
	    m_totalproMax = totalpro;		  
	    m_isemse    = eTemp->isemse();
	    m_bestSoftE = eTemp;
	    m_bestSoftIsPhoton = true;
	  }
	}
      }
    }
  }
}

// ======================================================
double SoftElectronTag::CombineWeights(xAOD::Jet& jetToTag, const egamma *eTemp, int choice)
{
  //
  // Combine weights filled at electron level + pT rel + d0
  // eTemp = Electron or Photon
  // choice = 1 combine all
  //        = 2 use only pTrel and d0 (no electron weight but use "isem")
  //

  double totalpro = 0.;

  //std::cout << " CombineWeights : " << choice << std::endl;

  /** jet direction: */
  Amg::Vector3D jetDirection(jetToTag.px(),jetToTag.py(),jetToTag.pz());
  Amg::Vector3D unit = jetDirection.unit();

  // weight to be signal from electron pid
  // NB: SofteElectronWeight is a log10(weight) !
  double s = eTemp->egammaID(egammaPID::SofteElectronWeight);
  // weight to be background from electron pid
  double b = eTemp->egammaID(egammaPID::SofteBgWeight);
  //std::cout << " s1 = " << s << " b = " << b << std::endl;
  s = pow(10.,s);
  b = pow(10.,b);
  // NB: SofteBgWeight is a log10(weight) !
  //std::cout << " s = " << s << " b = " << b << std::endl;
  
  if ( s+b != 0.) { 
    // weight to be signal
    m_Db   = s/(s+b);
    // weight to be background
    m_Du   = b/(s+b);
  } else {
    ATH_MSG_DEBUG("Info for Soft E : both hypotheses with 0 weight"); 
  }

  //std::cout << " m_Db = " << m_Db << " " << m_Du << std::endl;
  
  std::vector<Slice> slices;
  // pT of electron relative to jet axis
  m_ptrel = eTemp->p4().Vect().Dot(jetToTag.p4().Vect());
  AtomicProperty atom1(m_ptrel/1.e3,"pTrel");
  std::string compoName(m_author+"#");
  Composite compo1(compoName+"pTrel");
  compo1.atoms.push_back(atom1);
  Slice slice1("SoftEl");
  slice1.composites.push_back(compo1);
  
  //std::cout << " pTrel = " << m_ptrel << std::endl;
  // transverse impact parameter at perigee
  /* use new Tool for "unbiased" IP estimation */
  const Trk::ImpactParametersAndSigma* myIPandSigma(0);
  if (m_trackToVertexIPEstimator) { 
    myIPandSigma = m_trackToVertexIPEstimator->estimate(eTemp->trackParticle(),m_priVtx,m_unbiasIPEstimation);
  }
  if(0==myIPandSigma) {
    ATH_MSG_WARNING("SET: trackToVertexIPEstimator failed !");
  } else {
    m_d0wrtPvx=myIPandSigma->IPd0;
    m_d0ErrwrtPvx=myIPandSigma->sigmad0;
    delete myIPandSigma;
    myIPandSigma=0;
  }


  //std::cout << " m_d0 = " << m_d0wrtPvx << " " << m_d0ErrwrtPvx << std::endl;
  // sign of the impact parameter 
  if (m_trackToVertexIPEstimator) { 
    m_signOfIP=m_trackToVertexIPEstimator->get2DLifetimeSignOfTrack(eTemp->trackParticle()->definingParameters(),unit,m_priVtx->recVertex());
  }
  //std::cout << " m_signOfIP = " << m_signOfIP << std::endl;
  // signed ip and significances 
  //double sd0             = m_signOfIP*fabs(m_d0wrtPvx);
  //double sd0significance = m_signOfIP*fabs(m_d0wrtPvx/m_d0ErrwrtPvx);
  //std::cout << " sd0 = " << sd0 << " " << sd0significance << std::endl;
  
  /*m_d0wrtPvx = eTemp->trackParticle()->measuredPerigee()->parameters()[Trk::d0];
  const Trk::MeasuredPerigee* perigee = m_trackToVertexTool->perigeeAtVertex(*(m_bestSoftE->trackParticle()), m_priVtx->position());
  if (perigee) {
    m_d0wrtPvx = perigee->parameters()[Trk::d0];
    delete perigee;
    }*/
  // if transverse impact parameter is used
  if (m_useTransverseIP) {
    AtomicProperty atom2(fabs(m_d0wrtPvx),"d0");
    Composite compo2(compoName+"d0");
    compo2.atoms.push_back(atom2);
    slice1.composites.push_back(compo2);
  }
  slices.push_back(slice1);
  // fill the likelihood method
  m_likelihoodTool->setLhVariableValue(slices);
  //std::cout << " ici 0" << std::endl;
  // retrieve the weights
  //m_Weight[0] = 0.;
  //m_Weight[1] = 0.;
  m_Weight = m_likelihoodTool->calculateLikelihood();
  // add to tmp the weights to be electrons 
  // only if not use a cut based analysis
  if (choice==1) {
    // std::cout << " m_Weight.size() " << m_Weight.size() << std::endl;

    if (m_Weight.size() >= 2) {
      m_Weight[0] *= m_Db;
      m_Weight[1] *= m_Du;
    }
  }

  //std::cout << " m_Weight = " << m_Weight[0] << " " << m_Weight[1] << std::endl;
  // calculate totalpro as the "electron" hypothesis
  totalpro = m_Weight[0];

  //std::cout << " totalpro = " << totalpro << std::endl;
  return totalpro;
}

// ======================================================
void SoftElectronTag::RetrieveInfoBestElectron(xAOD::Jet& jetToTag)
{
  //
  // retrieve information (pTrel,d0,weights) from best electron
  //  
  // initialisation
  m_d0wrtPvx = 0.;
  m_ptrel    = 0.;
  m_Db       = 0; 
  m_Du       = 0;

  if (m_bestSoftE && (m_runModus == "reference" || m_writeInfoPlus)) {
    // transverse impact parameter at perigee
    m_d0wrtPvx = m_bestSoftE->trackParticle()->measuredPerigee()->parameters()[Trk::d0];
    const Trk::Perigee* perigee = m_trackToVertexTool->perigeeAtVertex(*(m_bestSoftE->trackParticle()), m_priVtx->recVertex().position());
    if (perigee) {
      m_d0wrtPvx = perigee->parameters()[Trk::d0];
      delete perigee;
    }
    // pT relative to jet axis
    m_ptrel = m_bestSoftE->p4().Vect().Dot(jetToTag.p4().Vect());
    //
    double s = m_bestSoftE->egammaID(egammaPID::SofteElectronWeight);
    double b = m_bestSoftE->egammaID(egammaPID::SofteBgWeight);
    s = pow(10.,s);
    b = pow(10.,b);
    if ( s+b != 0.) { 
      // weight to be signal
      m_Db   = s/(s+b);
      // weight to be background
      m_Du   = b/(s+b);
    } else {
      ATH_MSG_DEBUG("Info for best Soft E : both hypotheses with 0 weight"); 
    }
  }
  ATH_MSG_VERBOSE("Info for bestSoftE " << m_d0wrtPvx 
		  << " " << m_ptrel << " " << m_Db << " " << m_Du);
}

// ======================================================
void SoftElectronTag::JetTagReferenceMode(xAOD::Jet& jetToTag)
{
  // ===============================================================
  // part of the jet tagging algorithm done only in reference mode
  // ===============================================================

  // check value of pT and eta of the jet
  if ( jetToTag.pt()<=m_jetPtMinRef ) return;
  if ( fabs(jetToTag.eta())>=m_jetEtaMaxRef ) return;

  ATH_MSG_VERBOSE("A jet " << jetToTag.pt() 
		  << " " << jetToTag.eta());

  // check for a truth match
  const TruthInfo* mcinfo = jetToTag.tagInfo<TruthInfo>("TruthInfo");
  double deltaRmin(0.);
  double deltaRminC(0.);
  std::string label = "N/A";
  if( mcinfo ) {
    label = mcinfo->jetTruthLabel();
    // for purification: require no b or c quark closer than dR=m_purificationDeltaR
    double deltaRtoClosestB = mcinfo->deltaRMinTo("B");
    double deltaRtoClosestC = mcinfo->deltaRMinTo("C");
    deltaRmin = deltaRtoClosestB < deltaRtoClosestC ? deltaRtoClosestB : deltaRtoClosestC;
    double deltaRtoClosestT = mcinfo->deltaRMinTo("T");
    deltaRmin = deltaRtoClosestT < deltaRmin ? deltaRtoClosestT : deltaRmin;
    deltaRminC = deltaRtoClosestB;
  } else {
    ATH_MSG_ERROR("No TruthInfo ! Cannot run on reference mode !");
    return;
  }

  //
  ATH_MSG_VERBOSE("A jet " << label);
  if ( (    "B"==m_referenceType &&   "B"==label ) ||  // b-jets    
       ( "UDSG"==m_referenceType && "N/A"==label ) ||  // light jets
       (  "ALL"==m_referenceType && // all jets: b + purified light jets
	  ( ( "B"==label ) || 
	    ( "N/A"==label && deltaRmin > m_purificationDeltaR ) ||
	    ( "C"==label   && deltaRminC > m_purificationDeltaR ) ) ) ) {
    ATH_MSG_VERBOSE("Will add to calibration ! for a " << label << " jet");;
    std::string pref = "N/A";
    if ("N/A"==label) pref = m_hypotheses[1];
    if ("B"==label) {
      const SoftLeptonTruthInfo* sltinfo = 
	jetToTag.tagInfo<SoftLeptonTruthInfo>("SoftLeptonTruthInfo");
      if (sltinfo) {
	ATH_MSG_VERBOSE("SLT info exist "); 
	int nslt = sltinfo->numSLTrueInfo();
	bool gotSLT = false;
	if (nslt > 0) {
	  ATH_MSG_VERBOSE("lepton(s) in the jets :" << nslt);
	  // Get StoreGate to match the bestSoftE and the true lepton... seems complicated !
	  //StoreGateSvc* m_storeGate;
	  //StatusCode sc = service("StoreGateSvc", m_storeGate);
	  //if (sc.isFailure()) {
	  //ATH_MSG_ERROR("StoreGate service not found ! Cannot build calibration");
	  //return;
	  //}
	  const xAOD::TrackParticleContainer * tpContainer(0);
	  const TrackParticleTruthCollection* tpTruthColl(0);
	  //sc = m_storeGate->retrieve(tpContainer,m_TPContainerName);
	  StatusCode sc = evtStore()->retrieve(tpContainer,m_TPContainerName);
	  if ( sc.isFailure() ) {
	    ATH_MSG_DEBUG("No TrackParticleCandidate ! Cannot build calibration");
	    return;
	  }
	  //sc = m_storeGate->retrieve(tpTruthColl,m_TPTruthContainerName);
	  sc = evtStore()->retrieve(tpTruthColl,m_TPTruthContainerName);
	  if (sc.isFailure() ) {
	    ATH_MSG_DEBUG("No TrackParticleTruthCollection ! Cannot build calibration");
	    return;
	  }
	  long theBarcode = 0;
	  ElementLink<xAOD::TrackParticleContainer> trackPrtlink;
	  trackPrtlink.setElement(const_cast<xAOD::TrackParticle*>(m_bestSoftE->trackParticle()));
	  trackPrtlink.setStorableObject(*tpContainer);
	  TrackParticleTruthCollection::const_iterator tempTrackPrtTruthItr = tpTruthColl->find(Rec::TrackParticleTruthKey(trackPrtlink));
	  if (tempTrackPrtTruthItr != tpTruthColl->end()) {
	    const HepMcParticleLink & temHepMcLink = (*tempTrackPrtTruthItr).second.particleLink();
	    if (temHepMcLink.eventIndex()==0) theBarcode=temHepMcLink.barcode();
	    const HepMC::GenParticle * thePart(0);
	    if (theBarcode!=0 && theBarcode<100000) {
	      ATH_MSG_VERBOSE("The barcode of the match for the bestSoftE " << theBarcode);
	      thePart = temHepMcLink.cptr();
	      if (thePart) {
		for (int islt = 0; islt < nslt; islt++) {
		  const SLTrueInfo slt = sltinfo->getSLTrueInfo(islt);
		  int barc = slt.barcode();
		  if (barc == theBarcode && abs(slt.pdgId()) == 11) {
		    if ("B"==label) {
		      int pdgM = slt.pdgIdMother();
		      if (  isBMeson(pdgM) || isDMeson(pdgM) || isBBaryon(pdgM) || isDBaryon(pdgM) ) {gotSLT = true; pref = m_hypotheses[0];} 
		      if ( slt.FromB() && !slt.FromD()) {gotSLT = true; }
		      if ( slt.FromB() &&  slt.FromD()) {gotSLT = true; } 
		      if (!slt.FromB() &&  slt.FromD()) {gotSLT = true; } // What is this ??
		    }
		    ATH_MSG_VERBOSE("Found a truth match in the SLT info" << slt);
		  }
		}
	      }
	    } else {
	      ATH_MSG_VERBOSE("The barcode of the match for the bestSoftE is too high " << theBarcode);
	    }
	  }
	}
	if (!gotSLT) pref = m_hypotheses[1];
      }
    }
    if (pref == m_hypotheses[0] || pref == m_hypotheses[1]) {
      std::string hDir = "/RefFile/SoftEl/"+m_author+"/"+pref+"/";
      m_histoHelper->fillHisto(hDir+"d0",fabs(m_d0wrtPvx));
      m_histoHelper->fillHisto(hDir+"pTrel",m_ptrel/1.e3);
      m_histoHelper->fillHisto(hDir+"D",m_totalproMax);
      m_histoHelper->fillHisto(hDir+"DpT",m_ptrel/1.e3,m_totalproMax);
    }
  }
}

// ======================================================
bool SoftElectronTag::IsHardEle(xAOD::Jet& jetToTag) 
{

  std::vector<Amg::Vector3D> hardMus;
  bool hasHardEle(false);

  const SoftLeptonTruthInfo* sltinfo = jetToTag.tagInfo<SoftLeptonTruthInfo>("SoftLeptonTruthInfo");

  if (sltinfo) {
    int nslt = sltinfo->numSLTrueInfo();
    ATH_MSG_DEBUG("SL truth info exist. Found " << nslt << " true leptons in jet"); 
    for (int islt = 0; islt < nslt; islt++) {
      const SLTrueInfo slt = sltinfo->getSLTrueInfo(islt);
      ATH_MSG_DEBUG("SLT info " << slt.pdgId() 
		    << " " << slt.momentum().perp() 
		    << " " << slt.FromB() << " " << slt.FromD() << " " << slt.FromGH());
      if ( (abs(slt.pdgId()) == 13 || abs(slt.pdgId()) == 11 || abs(slt.pdgId()) == 15) && // Lepton from direct decay of W/Z/H
          !(slt.FromB()) &&
          !(slt.FromD()) &&
      (abs(slt.pdgIdMother())<100) && // not from light hadron decay-in-flight
          slt.FromGH()

	   //if ( abs(slt.pdgId()) == 11 && // Electron from direct decay of W/Z/H
	   //!(slt.FromB()) &&
	   //!(slt.FromD()) &&
	   //slt.FromGH()
	   ) {
	hardMus.push_back(slt.momentum());

	//hack to avoid calling heplorentzvector on Amg::Vector3D
	xAOD::IParticle::FourMom_t tempVec(slt.momentum().x(),slt.momentum().y(),slt.momentum().z());
	//xAOD::IParticle::FourMom_t tempVec(v.x(),v.y(),v.z());
	double dR = tempVec.DeltaR(jetToTag.p4());

	ATH_MSG_DEBUG("DR info " 
		      << v.eta() << " " << jetToTag.eta() << " "
		      << v.phi() << " " << jetToTag.phi() << " "
		      << dR);
	if(dR<m_elecIsolDeltaR) {	    
	  hasHardEle = true;
	}
      }
    }
  }
  return hasHardEle;
}

// ======================================================
void SoftElectronTag::JetTagAnalysisMode(xAOD::Jet& jetToTag)
{
  // ===============================================================
  // part of the jet tagging algorithm done only in analysis mode
  // ===============================================================
  
  std::string instanceName(name());
  // Create the Info only if there is at least one electron (or photon)
  if (m_bestSoftE && m_softeInfo == 0) {
    m_softeInfo = new SoftElectronInfo(instanceName.erase(0,8));
    jetToTag.addInfo(m_softeInfo);
    
    // Store maximum probability as Weight and NTrackProb 
    // and a vector of probabilities as TrackProb
    m_softeInfo->setNTrackProb(m_totalproMax);
    m_softeInfo->setWeight(m_totalproMax);
    m_softeInfo->setTrackProb(m_totalproVector);
      
    // Add the SETrackInfo
    if (m_writeInfoPlus) {
      if (m_bestSoftE) {
	if (!m_bestSoftIsPhoton) {
	  // if best candidate is an electron
	  SETrackInfo tinfo(m_originalElCollection,dynamic_cast<const Electron *>(m_bestSoftE),
			    m_d0wrtPvx,m_ptrel,m_totalproVector);
	  m_softeInfo->addTrackInfo(tinfo);
	} else {
	  // if best candidate is a photon
	  SETrackInfo tinfo(m_originalPhCollection,dynamic_cast<const Photon*>(m_bestSoftE),
			    m_d0wrtPvx,m_ptrel,m_totalproVector);
	  m_softeInfo->addTrackInfo(tinfo);
	}
      }
    }
  }

  if (m_bestSoftE) {
    if(m_softeInfo) m_softeInfo->setTagLikelihood(m_Weight);
    // Tagging done. Make info object valid, i.e. tag was ok. 
    // Fill the JetTag and return ... 
    m_softeInfo->makeValid();
    
    ATH_MSG_DEBUG("Weight of the jet (max. probability): " 
		  <<m_softeInfo->nTrackProb());
    for (unsigned jj=0; jj< (m_softeInfo->vectorTrackProb()).size(); jj++){
      ATH_MSG_DEBUG("Probability of track "<<jj<<" "
		    <<(m_softeInfo->vectorTrackProb())[jj]);
    }
  }
  m_likelihoodTool->clear();
  m_totalproVector.clear();
}

// ===================================================
void SoftElectronTag::finalizeHistos()
{
}


/* ------------------------------------------------------------------- */
/*                        Private Helper Functions                     */
/* ------------------------------------------------------------------- */
bool SoftElectronTag::m_preselect(const xAOD::TrackParticle *t)
{
  // 
  // performs track selection 
  // NB: should be similar to what is done in Reconstruction/egamma/egammaRec
  // softeBuilder.cxx
  // But some extra-cuts can be applied like on the transverse impact parameter
  //
  
  ATH_MSG_DEBUG("m_preselect(): trkPrt->pt(): " << t->pt());
  
  m_trackSelectorTool->primaryVertex(m_priVtx->recVertex().position());
  m_trackSelectorTool->prepare();
  // apply track selection
  if( m_trackSelectorTool->selectTrack(t) ) {
    // apply further cuts like on TRT high threshold
    // pseudo-rapidity
    double eta = fabs(t->eta());

    // fraction of high threshold hits in TRT (with outliers)
    int nTRThigh, nTRThighOutliers, nTRT, nTRTOutliers;
    t->summaryValue(nTRThigh, xAOD::numberOfTRTHighThresholdHits);
    t->summaryValue(nTRThighOutliers, xAOD::numberOfTRTHighThresholdOutliers);
    t->summaryValue(nTRT, xAOD::numberOfTRTHits);
    t->summaryValue(nTRTOutliers, xAOD::numberOfTRTOutliers);


    double rTRT = (nTRT+nTRTOutliers) > 0 ? ((double) (nTRThigh+nTRThighOutliers)/(nTRT+nTRTOutliers) ) : 0.;
    
    //std::cout << " tTRT = " << rTRT << std::endl;
    if ((rTRT>m_thrTRT && eta<2.) || eta>=2.) return true;
  }
  return false;
}

// =============================================================
bool SoftElectronTag::isBBaryon(const int pID) const 
{
  //
  // PdgID of B-baryon is of form ...xxx5xxx
  //
  std::string idStr = longToStr( abs(pID) );
  char digit4 = idStr[ idStr.length() - 4 ];
  if( digit4=='5' ) 
    return true;
  else 
    return false;
}

// ============================================================
bool SoftElectronTag::isBMeson(const int pID) const 
{
  //
  // PdgID of B-meson is of form ...xxx05xx
  //
  std::string idStr = longToStr( abs(pID) );
  char digit3 = idStr[ idStr.length() - 3 ];
  char digit4;
  if( idStr.length() < 4 ) { digit4 = '0'; }
  else { digit4 = idStr[ idStr.length() - 4 ]; };
  if( (digit4=='0') && (digit3=='5') ) 
    return true;
  else 
    return false;
}

// ============================================================
bool SoftElectronTag::isDBaryon(const int pID) const 
{
  //
  // PdgID of D-baryon is of form ...xxx4xxx
  //
  std::string idStr = longToStr( abs(pID) );
  char digit4 = idStr[ idStr.length() - 4 ];
  if( digit4=='4' ) 
    return true;
  else 
    return false;
}

// =============================================================
bool SoftElectronTag::isDMeson(const int pID) const 
{
  //
  // PdgID of D-meson is of form ...xxx04xx
  //
  std::string idStr = longToStr( abs(pID) );
  char digit3 = idStr[ idStr.length() - 3 ];
  char digit4;
  if( idStr.length() < 4 ) { digit4 = '0'; }
  else { digit4 = idStr[ idStr.length() - 4 ]; };
  if( (digit4=='0') && (digit3=='4') ) 
    return true;
  else 
    return false;
}

// ==============================================================
std::string SoftElectronTag::longToStr( const long n ) const 
{
  if (0==n) return "0"; 
  std::string str = "";
  for ( long m = n; m!=0; m/=10 )
    str = char( '0' + abs(m%10) ) + str;
  if ( n<0 )
    str = "-" + str;
  return str;
}


}

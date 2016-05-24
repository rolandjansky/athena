/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     SoftMuonTag.cxx
PACKAGE:  offline/PhysicsAnalysis/JetTagging/JetTagTools

PURPOSE:  b-tagging based on soft muon identification
********************************************************************/
#include "JetTagTools/SoftMuonTag.h"

//#include "FourMom/P4PxPyPzE.h"
//#include "JetEvent/Jet.h"
#include "GaudiKernel/MsgStream.h"
#include "Navigation/NavigationToken.h"
#include "GaudiKernel/IToolSvc.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "MuonSelectorTools/IMuonSelectionTool.h" 
#include "JetTagTools/JetTagUtils.h"

#include "JetTagInfo/TruthInfo.h"
#include "JetTagInfo/SoftMuonInfo.h"
#include "JetTagInfo/SoftLeptonTruthInfo.h"
#include "JetTagInfo/SLTrueInfo.h"

#include "JetTagTools/NewLikelihoodTool.h"
#include "JetTagTools/HistoHelperRoot.h"
#include "GaudiKernel/ITHistSvc.h"
#include "JetTagCalibration/CalibrationBroker.h"

#include "xAODMuon/MuonContainer.h"
//////#include "MuonIDEvent/MuonAssociation.h"
//////#include "StoreGate/StoreGateSvc.h"
//////#include "muonEvent/Muon.h"

namespace Analysis
{

SoftMuonTag::SoftMuonTag(const std::string& t, const std::string& n, const IInterface* p)
        : AthAlgTool(t,n,p),
          m_trackToVertexTool("Reco::TrackToVertex"),
	  m_muonSelectorTool("JVC_MuonSelectorTool"),
          m_likelihoodTool("Analysis::NewLikelihoodTool"),
	  m_histoHelper(0)
{
    declareInterface<ITagTool>(this);
    declareProperty("Runmodus",       m_runModus               = "analysis");
    declareProperty("RefType",        m_refType                = "ALL");
    declareProperty("TaggingAlgType", m_algMode                = "L1D");
    declareProperty("BTagJetPTmin",   m_pTjetmin               = 15.*Gaudi::Units::GeV);
    declareProperty("BTagJetEtamin",  m_etajetmin              = 2.7);
    declareProperty("LikelihoodTool", m_likelihoodTool);
    declareProperty("TrackToVertexTool"       , m_trackToVertexTool);
    declareProperty("TrackToVertexIPEstimator", m_trackToVertexIPEstimator);   
    declareProperty("muonSelectorTool", 	m_muonSelectorTool);
    declareProperty("checkOverflows", m_checkOverflows         = false);
    declareProperty("purificationDeltaR", m_purificationDeltaR = 0.8);
    declareProperty("muonIsolDeltaR", m_muonIsolDeltaR         = 0.7);
    declareProperty("UseBinInterpol", m_UseBinInterpol         = true);
    
    declareProperty("jetCollectionList"    , m_jetCollectionList);
    declareProperty("useForcedCalibration" , m_doForcedCalib   = false);
    declareProperty("ForcedCalibrationName", m_ForcedCalibName = "Cone4H1Tower");

    // VD: not sure which of these is needed ....
    declareProperty("RecAlgorithm",   m_alg                    = 1);
    declareProperty("CutD0",          m_d0cut                  = 4.*Gaudi::Units::mm);
    declareProperty("CutPT",          m_pTcut                  = 4.*Gaudi::Units::GeV);
    declareProperty("CutDR",          m_DRcut                  = 0.5);
    declareProperty("CutMatchChi2",   m_MatchChi2cut           = 10);

    declareProperty("writeInfoPlus",  m_writeInfoPlus          = true);

    declareProperty("originalMuCollectionName", m_originalMuCollectionName = "StacoMuonCollection");
    declareProperty("MuonAssociationName"     , m_muonAssociationName="Muons");

    /** number of hypotheses = 3 : b,l,c */
    m_hypothese.push_back("SoftMb");
    m_hypothese.push_back("SoftMl");
    m_hypothese.push_back("SoftMc");

    // List of histogram names
    m_histoname.push_back("pT");
    m_histoname.push_back("pTrel");
    m_histoname.push_back("pTLowPt");
    m_histoname.push_back("pTrelLowPt");
    m_histoname.push_back("pTpTrel");
    m_histoname.push_back("pTpTrelLowPt");
    m_histoname.push_back("JetETEffL1D");
    m_histoname.push_back("JetETLowPtEffL1D");
    m_histoname.push_back("JetETNormL1D");
    m_histoname.push_back("JetETLowPtNormL1D");
    m_histoname.push_back("JetETEffL1DL1D");
    m_histoname.push_back("JetETLowPtEffL1DL1D");
    m_histoname.push_back("JetETNormL1DL1D");
    m_histoname.push_back("JetETLowPtNormL1DL1D");
    m_histoname.push_back("JetETEffL2D");
    m_histoname.push_back("JetETLowPtEffL2D");
    m_histoname.push_back("JetETNormL2D");
    m_histoname.push_back("JetETLowPtNormL2D");
    
    
    declareProperty("MuonQuality",		m_muonQualityCut = xAOD::Muon::Medium ); 
}

SoftMuonTag::~SoftMuonTag()
{
}

StatusCode SoftMuonTag::initialize()
{
  
  ATH_MSG_INFO("#BTAG# Initializing..."); 
  printParameterSettings();

  /** retrieving ToolSvc: */
  IToolSvc* toolSvc;
  StatusCode sc = service("ToolSvc", toolSvc);
  if (StatusCode::SUCCESS != sc) {
    ATH_MSG_ERROR( "#BTAG# Can't get ToolSvc");
    return StatusCode::FAILURE;
  }
  
  //Retrieve the Muon Selectot tool
  sc = m_muonSelectorTool.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_muonSelectorTool);
    return sc;
  } else {
    ATH_MSG_INFO("#BTAG# Retrieved tool " << m_muonSelectorTool);  
  }

  /** retrieving TrackToVertex: */
  /*
  if ( m_trackToVertexTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL( "#BTAG# Failed to retrieve tool " << m_trackToVertexTool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "#BTAG# Retrieved tool " << m_trackToVertexTool);
  }
  */

  if (m_trackToVertexIPEstimator.retrieve().isFailure() ) {
    ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_trackToVertexIPEstimator);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_trackToVertexIPEstimator);
  }

  // If the jet author is not known 
  // (or one wants a calibration not corresponding to the author), can force the calibration. 
  // Check that this calibration has been loaded
  if (m_doForcedCalib) {
    if (std::find( m_jetCollectionList.begin(), 
		   m_jetCollectionList.end(), 
		   m_ForcedCalibName ) == m_jetCollectionList.end()) {
      ATH_MSG_ERROR( "#BTAG# Error, forced calibration to an unloaded one");
      return StatusCode::FAILURE;
    }
  }

  /* ------------------------------------------------------------------------- */
  /*                 READ IN REFHISTOS IF IN ANALYSIS MODE                     */
  /* ------------------------------------------------------------------------- */
  /* // VD: commenting out
  if (m_runModus == "analysis") {
    ATH_MSG_INFO("#BTAG# Reading histos...");
    // retrieve the Likelihood tool
    if ( m_likelihoodTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL( "#BTAG# Failed to retrieve tool " << m_likelihoodTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("#BTAG# Retrieved tool " << m_likelihoodTool);
    }
    //    m_likelihoodTool->setInterpolFlag(m_UseBinInterpol);
    if (3!=m_hypothese.size()) {
      ATH_MSG_ERROR( "#BTAG# There should be 3 hypotheses instead of " << m_hypothese.size());
      return StatusCode::FAILURE;
    }
    m_likelihoodTool->defineHypotheses(m_hypothese);
    for(uint ih=0;ih<m_hypothese.size();ih++) {
      // define histos for efficiency:
      std::string hName = m_hypothese[ih]+"/JetETEff"+m_algMode;
      m_likelihoodTool->defineHistogram(hName);
      hName = m_hypothese[ih]+"/JetETNorm"+m_algMode;
      m_likelihoodTool->defineHistogram(hName);
      hName = m_hypothese[ih]+"/JetETLowPtEff"+m_algMode;
      m_likelihoodTool->defineHistogram(hName);
      hName = m_hypothese[ih]+"/JetETLowPtNorm"+m_algMode;
      m_likelihoodTool->defineHistogram(hName);
      // define histos for pdf's:
      std::string hBase = m_hypothese[ih]+"/";
      if(m_algMode == "L1D"){
	m_likelihoodTool->defineHistogram(hBase+"pTrel");
	m_likelihoodTool->defineHistogram(hBase+"pTrelLowPt");
      }
      else if(m_algMode == "L1DL1D"){
	m_likelihoodTool->defineHistogram(hBase+"pT");
	m_likelihoodTool->defineHistogram(hBase+"pTLowPt");
	m_likelihoodTool->defineHistogram(hBase+"pTrel");
	m_likelihoodTool->defineHistogram(hBase+"pTrelLowPt");
      }
      else if(m_algMode == "L2D"){
	m_likelihoodTool->defineHistogram(hBase+"pTpTrel");
	m_likelihoodTool->defineHistogram(hBase+"pTpTrelLowPt");
      }
    }
    //       for(uint ih=0;ih<m_hypothese.size();ih++) {
    // 	m_likelihoodTool->prepareHistosFromFile("/BTAG/CALIB/"+m_hypothese[ih]+"/");
    //       }
    //      m_likelihoodTool->resetLhVariableToUse();
    m_likelihoodTool->printStatus();	
  }
  */

  /* ----------------------------------------------------------------------------------- */
  /*                         BOOK HISTOS IF IN REFERENCE MODE                            */
  /* ----------------------------------------------------------------------------------- */
  if (m_runModus=="reference") {
    //
    // Book the histos
    // 
    ITHistSvc* myHistoSvc;
    if( service( "THistSvc", myHistoSvc ).isSuccess() ) {
      ATH_MSG_DEBUG( "#BTAG# "<< name() << ": HistoSvc loaded successfully.");
      m_histoHelper = new HistoHelperRoot(myHistoSvc);
      m_histoHelper->setCheckOverflows(m_checkOverflows);
    } else ATH_MSG_ERROR( "#BTAG# " << name() << ": HistoSvc could NOT bo loaded.");
    //
    ATH_MSG_INFO( "#BTAG# Booking histos...");
    for(uint ijc=0;ijc<m_jetCollectionList.size();ijc++) {
      for(uint ih=0;ih<m_hypothese.size();ih++) {
	if(ih==0) {
	  // Control:
	  std::string hDir = "/RefFile/SoftMu/"+m_jetCollectionList[ijc]+"/controlSoftMu/";
	  m_histoHelper->bookHisto(hDir+"eta","eta",60,-3.,3.);
	  m_histoHelper->bookHisto(hDir+"phi","phi",64,-3.2,3.2);
	  m_histoHelper->bookHisto(hDir+"pt","pt",50,0.,300.);
	  m_histoHelper->bookHisto(hDir+"smpt","Soft Muon pT",100,0.,100.);
	}
	std::string hDir = "/RefFile/SoftMu/"+m_jetCollectionList[ijc]+"/"+m_hypothese[ih]+"/";
	// variables:
	m_histoHelper->bookHisto(hDir+"pT",           "pT/(pT+5)",100,0.,1.);
	m_histoHelper->bookHisto(hDir+"pTrel",        "pTrel/(pTrel+0.5)",100,0.,1.);
	m_histoHelper->bookHisto(hDir+"pTLowPt",      "pT/(pT+5)",100,0.,1.);
	m_histoHelper->bookHisto(hDir+"pTrelLowPt",   "pTrel/(pTrel+0.5)",100,0.,1.);
	m_histoHelper->bookHisto(hDir+"pTpTrel",      "pT/(pT+5) vs pTrel/(pTrel+0.5)" ,100,0.,1.,100,0.,1.);
	m_histoHelper->bookHisto(hDir+"pTpTrelLowPt", "pT/(pT+5) vs pTrel/(pTrel+0.5)" ,100,0.,1.,100,0.,1.);
	// normalization:
	// 1D:
	m_histoHelper->bookHisto(hDir+"JetETEffL1D",  "Jet Et",100,0,500);
	m_histoHelper->bookHisto(hDir+"JetETLowPtEffL1D",  "Jet Et",100,0,500);
	m_histoHelper->bookHisto(hDir+"JetETNormL1D", "Jet Et",100,0,500);
	m_histoHelper->bookHisto(hDir+"JetETLowPtNormL1D", "Jet Et",100,0,500);
	// 1Dx1D:
	m_histoHelper->bookHisto(hDir+"JetETEffL1DL1D",  "Jet Et",100,0,500);
	m_histoHelper->bookHisto(hDir+"JetETLowPtEffL1DL1D",  "Jet Et",100,0,500);
	m_histoHelper->bookHisto(hDir+"JetETNormL1DL1D", "Jet Et",100,0,500);
	m_histoHelper->bookHisto(hDir+"JetETLowPtNormL1DL1D", "Jet Et",100,0,500);
	// 2D:
	m_histoHelper->bookHisto(hDir+"JetETEffL2D",       "Jet Et",100,0,500);
	m_histoHelper->bookHisto(hDir+"JetETLowPtEffL2D",  "Jet Et",100,0,500);
	m_histoHelper->bookHisto(hDir+"JetETNormL2D",      "Jet Et",100,0,500);
	m_histoHelper->bookHisto(hDir+"JetETLowPtNormL2D", "Jet Et",100,0,500);
      }
    }
    m_histoHelper->print();
	
  }
  
  return StatusCode::SUCCESS;
}


StatusCode SoftMuonTag::finalize()
{
    return StatusCode::SUCCESS;
}


StatusCode SoftMuonTag::tagJet(xAOD::Jet& jetToTag, xAOD::BTagging* BTag) {

  ATH_MSG_DEBUG( "#BTAG# Starting tagJet");

  /** author to know which jet algorithm: */
  std::string author = JetTagUtils::getJetAuthor(jetToTag);
  if (m_doForcedCalib) author = m_ForcedCalibName;
  ATH_MSG_VERBOSE("#BTAG# Using jet type " << author << " for calibrations.");

  /* The jet */
  double jeteta = jetToTag.eta(), jetphi = jetToTag.phi(), jetpt = jetToTag.pt();
  ATH_MSG_DEBUG( "#BTAG# Jet properties : eta = "<<jeteta
		 <<" phi = "<<jetphi
		 <<" pT  = "<<jetpt/1.e3 
		 <<" author = " <<author);

  // Reference only: Fill control histograms and get jet label
  std::string pref = "";
  std::string label = "N/A";
  std::vector<Amg::Vector3D> hardMus;
  if (m_runModus=="reference") {
    // Veto jets containing a muon from direct decay of W/Z/H:
    //bool hasHardMu(false);
    //const SoftLeptonTruthInfo* sltinfo = jetToTag.tagInfo<SoftLeptonTruthInfo>("SoftLeptonTruthInfo");
    /*
    if (sltinfo) {
      int nslt = sltinfo->numSLTrueInfo();
      ATH_MSG_DEBUG( "#BTAG# SL truth info exist. Found " << nslt << " true leptons in jet");
      for (int islt = 0; islt < nslt; islt++) {
	const SLTrueInfo slt = sltinfo->getSLTrueInfo(islt);
	ATH_MSG_DEBUG ( "#BTAG# SLT info " << slt.pdgId() 
			<< " " << slt.momentum().perp() 
			<< " " << slt.FromB() << " " << slt.FromD() << " " << slt.FromGH()); 
	if ( abs(slt.pdgId()) == 13 && // Muon from direct decay of W/Z/H
	     !(slt.FromB()) &&
	     !(slt.FromD()) &&
	     slt.FromGH()
	     ) {
	  Amg::Vector3D v(slt.momentum());
	  hardMus.push_back(v);
	  double dR = v.deltaR(Amg::Vector3D(jetToTag.p4().Px(), jetToTag.p4().Py(), jetToTag.p4().Pz()));
	  ATH_MSG_DEBUG ( "#BTAG# DR info " 
			  << v.eta() << " " << jetToTag.eta() << " "
			  << v.phi() << " " << jetToTag.phi() << " "
			  << dR);
	  if(dR<m_muonIsolDeltaR) {	    
	    hasHardMu = true;
	  }
	}
      }
    }
    */
    //
    /////////////if(hasHardMu)return; // skip jet
    //
    m_histoHelper->fillHisto("/RefFile/SoftMu/"+author+"/controlSoftMu/eta",(double)jeteta);
    if (fabs(jeteta) <= m_etajetmin) {
      m_histoHelper->fillHisto("/RefFile/SoftMu/"+author+"/controlSoftMu/phi",(double)jetphi);
      m_histoHelper->fillHisto("/RefFile/SoftMu/"+author+"/controlSoftMu/pt",(double)jetpt/1.e3);
      //
      /*
      if(jetpt>m_pTjetmin) {
	const TruthInfo* mcTrueInfo = jetToTag.tagInfo<TruthInfo>("TruthInfo");
	if (mcTrueInfo) {
	  label = mcTrueInfo->jetTruthLabel();
	  ATH_MSG_DEBUG ( "#BTAG# Jet label="<<label);
	  // for purification: require no b or c quark closer than dR=m_purificationDeltaR
	  double deltaRtoClosestB = mcTrueInfo->deltaRMinTo("B");
	  double deltaRtoClosestC = mcTrueInfo->deltaRMinTo("C");
	  double deltaRmin = deltaRtoClosestB < deltaRtoClosestC ? deltaRtoClosestB : deltaRtoClosestC;
	  //
	  if ( (    "B"==m_refType &&   "B"==label ) ||  // b-jets    
	       (    "C"==m_refType &&   "C"==label ) ||  // c-jets
	       (    "L"==m_refType && "N/A"==label ) ||  // light jets
	       (  "ALL"==m_refType &&                    // all jets: purify
		  ( 
		   ( "B"==label   && deltaRtoClosestC > m_purificationDeltaR ) || 
		   ( "C"==label   && deltaRtoClosestB > m_purificationDeltaR ) ||
		   ( "N/A"==label && deltaRmin > m_purificationDeltaR ) ) )
	       ) {
	    if ("B"==label) {
	      pref = m_hypothese[0];
	    } else if ("N/A"==label) {
	      pref = m_hypothese[1];
	    } else if ("C"==label) {
	      pref = m_hypothese[2];
	    }
	    std::string hDir = "/RefFile/SoftMu/"+author+"/"+pref+"/";
	    m_histoHelper->fillHisto(hDir+"JetETNormL1D",(double)jetpt/1.e3);
	    m_histoHelper->fillHisto(hDir+"JetETNormL1DL1D",(double)jetpt/1.e3);
	    m_histoHelper->fillHisto(hDir+"JetETNormL2D",(double)jetpt/1.e3);
	    m_histoHelper->fillHisto(hDir+"JetETLowPtNormL1D",(double)jetpt/1.e3);
	    m_histoHelper->fillHisto(hDir+"JetETLowPtNormL1DL1D",(double)jetpt/1.e3);
	    m_histoHelper->fillHisto(hDir+"JetETLowPtNormL2D",(double)jetpt/1.e3);
	  }
	  else return; // unwanted label
	} 
	else {
	  ATH_MSG_ERROR( "#BTAG# No Label ! Cannot run on reference mode !");
	  return;
	}
      }
      else return; // failed pt cut
      */
    }
    else return StatusCode::SUCCESS; // failed eta cut
  }

  
  /////// VD: THIS is the part that matters
  /*
  if(m_runModus=="analysis" && m_writeInfoPlus) {
    bool ppb = true;
    StoreGateSvc* m_StoreGate;
    StatusCode sc = service("StoreGateSvc", m_StoreGate);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "#BTAG# StoreGate service not found !");
    } else {
      sc = m_StoreGate->retrieve(m_originalMuCollection, m_originalMuCollectionName);
      if (sc.isFailure()) {
	ATH_MSG_WARNING( "#BTAG# " << m_originalMuCollectionName << " not found in StoreGate.");
      } else {
	ATH_MSG_DEBUG( "#BTAG# MuonContainer " << m_originalMuCollectionName << " found.");
	ppb = false;
      }
    }
    if(ppb) {
      ATH_MSG_WARNING( "#BTAG# Not able to persistify infos ! Exiting...");
      return;
    }
  }
  */

  //const MuonAssociation *mc = jetToTag.getAssociation<MuonAssociation>(m_muonAssociationName);
  /*
  if (mc == 0) {
    ATH_MSG_INFO( "#BTAG# No muon constituent");
    return;
  }
  */
  std::vector<ElementLink<xAOD::MuonContainer> > assocMuons;
  assocMuons= BTag->auxdata<std::vector<ElementLink<xAOD::MuonContainer> > >("Muons");
  if ( assocMuons.size()==0 ) {
    ATH_MSG_DEBUG( "#BTAG# Found no associated muons to the jet");
    ///return StatusCode::SUCCESS; /// need to go untill the end to decorate
  }

  //SoftMuonInfo* softmInfo(0);
  // LOOP OVER MUONS ASSOCIATED WITH THE JET:
  //int muCounter         = 0;       // number of muons passing basic selection
  //int muCounterL1D      = 0;       // number of muons tagged by L1D algorithm
  //int muCounterLowPt    = 0;       // number of muons passing basic selection
  //int muCounterL1DLowPt = 0;       // number of muons tagged by L1D algorithm
  //std::vector<double> bestMuProbi; // likelihood (b, c, l) of the best muon
  //double bestMuWeight = 0;         // weight of the best muon
  //if(m_algMode == "CHI2") bestMuWeight = 1000.;
  //double highestPT = 0;       // for reference: if several, use
  //double highestPTrel = 0;    // only the muon with highest pt
  //bool   highestIsLowP = 0;

  
  /// variables used to decorate the Btagging object
  float jet_mu_dRmin_pt=0;
  float jet_mu_dRmin_dR=10;
  float jet_mu_dRmin_qOverPratio    = 0;
  float jet_mu_dRmin_mombalsignif   = 0;
  float jet_mu_dRmin_scatneighsignif= 0;
  float jet_mu_dRmin_pTrel=0;
  float jet_mu_dRmin_d0   =0;
  float jet_mu_dRmin_z0   =0;
  float jet_mu_dRmin_ID_qOverP_var  = 0;   
  int muonIndex=-1;

  for (unsigned int iT=0; iT<assocMuons.size(); iT++) {
    if ( !assocMuons.at(iT).isValid() ) continue;
    const xAOD::Muon* tmpMuon= *(assocMuons.at(iT));
    if ( tmpMuon==0 ) continue;
    
    if ( tmpMuon->muonType() != xAOD::Muon::Combined) continue;

    // Veto muons close to muons from W/Z/H decay in reference mode
    //bool closeToHardMu(false);
    
    /*
    for(uint i=0;i<hardMus.size();i++) {
      Amg::Vector3D v = hardMus[i];
      double dR = v.deltaR(Amg::Vector3D(m->p4().Px(),m->p4().Py(),m->p4().Pz()));
      ATH_MSG_DEBUG( "#BTAG# DR(mu-mu) info " 
		     << v.eta() << " " <<  m->eta() << " "
		     << v.phi() << " " << m->phi() << " "
		     << dR);
      if(dR<0.1) {	    
	closeToHardMu = true;
      }	  
    }
    if(closeToHardMu) {
      ATH_MSG_DEBUG( "#BTAG# skipping this muon" );
      continue;
    }
    */

    // muon selection here:
    float dR = jetToTag.p4().DeltaR(tmpMuon->p4());
    if(dR>=0.4) continue;
    
    const ElementLink< xAOD::TrackParticleContainer >& pMuIDTrack=tmpMuon->inDetTrackParticleLink();
    const ElementLink< xAOD::TrackParticleContainer >& pMuMSTrack=tmpMuon->muonSpectrometerTrackParticleLink();
    //const xAOD::Vertex * pVtx   = (*pMuIDTrack)->vertex();
    if ( !pMuIDTrack.isValid() || !pMuMSTrack.isValid()) continue;

    const std::vector<float>&cov= (*pMuIDTrack)->definingParametersCovMatrixVec();
    float momBalSignif0=0.;
    tmpMuon->parameter(momBalSignif0, xAOD::Muon::momentumBalanceSignificance);
    if( momBalSignif0==0 )          continue;
    if( (*pMuMSTrack)->qOverP()==0) continue;
    
    xAOD::Muon::Quality quality = m_muonSelectorTool->getQuality(*tmpMuon);
    //just added this cut         
    if( quality > m_muonQualityCut ) continue; 

    float scatNeighSignif=0.;
    tmpMuon->parameter(scatNeighSignif, xAOD::Muon::scatteringNeighbourSignificance);
    TLorentzVector myjet, mymu;
    myjet.SetPtEtaPhiM(jetToTag.pt(),jetToTag.eta(),jetToTag.phi(),0);
    mymu.SetPtEtaPhiM(tmpMuon->pt(),tmpMuon->eta(),tmpMuon->phi(),0);
    float pTrel      =myjet.Vect().Perp(mymu.Vect()); // VD: everything MUST be in MeV
    float qOverPratio=(*pMuIDTrack)->qOverP()/(*pMuMSTrack)->qOverP();
    
    float d0 = tmpMuon->primaryTrackParticle()->d0();
    float z0 = tmpMuon->primaryTrackParticle()->z0()+(tmpMuon->primaryTrackParticle()->vz())-(m_priVtx->z()) ;
    //std::cout << " trk: " <<  tmpMuon->primaryTrackParticle()->z0() << "  PVZ: " << m_priVtx->z() << "  and z: " << z0 << std::endl;

    ATH_MSG_DEBUG( "#BTAG# Found muon " << iT << " with PT= " << tmpMuon->pt() 
		  << " dR= " << dR 
		  << " qOverPratio= " << qOverPratio
		  << " ScatNeiSig= " << scatNeighSignif
		  << " MomBalSig= " <<  momBalSignif0
		  << " ptrel= " << pTrel
		  << " z0= " << z0 
		  << " d0= " << d0
		  );

    if(dR<jet_mu_dRmin_dR){
      jet_mu_dRmin_pt=tmpMuon->pt();
      jet_mu_dRmin_dR=dR;
      jet_mu_dRmin_qOverPratio    = qOverPratio;
      jet_mu_dRmin_mombalsignif   = momBalSignif0;
      jet_mu_dRmin_scatneighsignif= scatNeighSignif;
      jet_mu_dRmin_pTrel=pTrel;
      jet_mu_dRmin_d0   =d0;
      jet_mu_dRmin_z0   =z0;
      jet_mu_dRmin_ID_qOverP_var  = cov[14];  
      muonIndex=iT;
    }

    /*
    const MuonParameters::Author mAuthor = m->author();
    ATH_MSG_DEBUG( "#BTAG# Muon Author=" << mAuthor << " " 
		   << MuonParameters:: mediumPt << " ");
    // do not use MuTagMedium:
    if(mAuthor == MuonParameters::mediumPt ) continue;
    bool isComb(m->isCombinedMuon());
    bool isLowP(m->isSegmentTaggedMuon() && m->inDetTrackParticle()!=0);
    bool acceptAlg(true);
    if( 0==m_alg && ( isLowP || !isComb ) ) acceptAlg = false;     // Use only combined muons
    else if( 1==m_alg && (!isLowP && !isComb) ) acceptAlg = false; // Use only low-pT and combined muons
    if(!acceptAlg)continue;
   
    double matchChi2 = m->matchChi2OverDoF();
    ATH_MSG_DEBUG( "#BTAG# Muon Match Chi2=" << matchChi2 << " " << m_MatchChi2cut);
    if( m->matchChi2OverDoF()>m_MatchChi2cut && (isComb||isLowP) )continue;
    //
    bool passD0cut(true);
    double d0wrtPriVtx(999.);
    if( isLowP || isComb ) {
      const xAOD::TrackParticle* trk = m->inDetTrackParticle();
      d0wrtPriVtx = trk->d0();
      const Trk::Perigee* perigee =
	m_trackToVertexTool->perigeeAtVertex(*trk, m_priVtx->recVertex().position());
      if (perigee) {
	d0wrtPriVtx = perigee->parameters()[Trk::d0];
	delete perigee;
      }
      if(fabs(d0wrtPriVtx)>m_d0cut)passD0cut = false;
    }
    if(!passD0cut)continue;
    
    double dR = jetToTag.p4().DeltaR(m->p4());
    double pt(1./m->iPt());
    double ptrel = m->p4().Vect().Dot((jetToTag.p4()+m->p4()).Vect());
    double ptN    ( pt    / (pt   +5.*Gaudi::Units::GeV) );
    double ptrelN ( ptrel / (ptrel+ 0.5*Gaudi::Units::GeV) );
    */
    

    //
    /*
    if(dR<m_DRcut) { // basic cut applied to all algMode's
  
      if(isLowP)muCounterLowPt++;
      else muCounter++;
      
      if(m_runModus == "analysis") {
	std::vector<double> probi;
	if(m_algMode == "CHI2") {
	  if(fabs(pt)>m_pTcut) {
	    probi.push_back(matchChi2);
	    if( matchChi2<bestMuWeight ) {
	      bestMuWeight = matchChi2;
	      bestMuProbi  = probi;
	    }
	  }
	} else {
	  std::vector<Slice> slices;
	  AtomicProperty atom2(ptrelN,"pTrel");
	  if(m_algMode == "L2D") {
	    AtomicProperty atom1(ptN,"pT");
	    Slice slice("L2D");
	    if(isLowP){
	      Composite compo(author+"#pTpTrelLowPt");
	      compo.atoms.push_back(atom1);
	      compo.atoms.push_back(atom2);
	      slice.composites.push_back(compo);
	    } else{
	      Composite compo(author+"#pTpTrel");
	      compo.atoms.push_back(atom1);
	      compo.atoms.push_back(atom2);
	      slice.composites.push_back(compo);
	    }
	    slices.push_back(slice);
	  } else if(m_algMode == "L1DL1D") {
	    AtomicProperty atom1(ptN,"pT");
	    Slice slice("L1DL1D");
	    if(isLowP){
	      Composite compo1(author+"#pTLowPt");
	      compo1.atoms.push_back(atom1);
	      slice.composites.push_back(compo1);
	      Composite compo2(author+"#pTrelLowPt");
	      compo2.atoms.push_back(atom2);
	      slice.composites.push_back(compo2);
	    } else{
	      Composite compo1(author+"#pT");
	      compo1.atoms.push_back(atom1);
	      slice.composites.push_back(compo1);
	      Composite compo2(author+"#pTrel");
	      compo2.atoms.push_back(atom2);
	      slice.composites.push_back(compo2);
	    }
	    slices.push_back(slice);
	  } else if(m_algMode == "L1D" && fabs(pt)>m_pTcut) {
	    Slice slice("L1D");
	    if(isLowP){
	      muCounterL1DLowPt++;
	      Composite compo(author+"#pTrelLowPt");
	      compo.atoms.push_back(atom2);
	      slice.composites.push_back(compo);
	    } else{
	      muCounterL1D++;
	      Composite compo(author+"#pTrel");
	      compo.atoms.push_back(atom2);
	      slice.composites.push_back(compo);
	    }
	    slices.push_back(slice);
	  }
	  if(slices.size()) {
	    m_likelihoodTool->setLhVariableValue(slices);
	    probi = m_likelihoodTool->calculateLikelihood();
	    double w(1);
	    if (probi.size() >= 3) {
	      ATH_MSG_VERBOSE( "#BTAG# SoftMu probabilities "
			       <<" p_b = "<<probi[0]
			       <<" p_c = "<<probi[2]
			       <<" p_l = "<<probi[1]);
	      double effb = 0.5, effl = 0.5, effc = 0.5;
	      if(isLowP){
		effb = m_likelihoodTool->getEff(m_hypothese[0],author+"#JetETLowPt",m_algMode);
		effl = m_likelihoodTool->getEff(m_hypothese[1],author+"#JetETLowPt",m_algMode);
		effc = m_likelihoodTool->getEff(m_hypothese[2],author+"#JetETLowPt",m_algMode);
		ATH_MSG_VERBOSE( "#BTAG# SoftMu Low PT efficiencies for jetColl "<<author
				 <<" eps_b = "<<effb<<" eps_c = "<<effc<<" eps_l = "<<effl);
		
	      } else{
		effb = m_likelihoodTool->getEff(m_hypothese[0],author+"#JetET",m_algMode);
		effl = m_likelihoodTool->getEff(m_hypothese[1],author+"#JetET",m_algMode);
		effc = m_likelihoodTool->getEff(m_hypothese[2],author+"#JetET",m_algMode);
		ATH_MSG_VERBOSE( "#BTAG# SoftMu efficiencies for jetColl "<<author
				 <<" eps_b = "<<effb<<" eps_c = "<<effc<<" eps_l = "<<effl);
	      }
	      probi[0] *= effb;
	      probi[1] *= effl;
	      probi[2] *= effc;
	      w = probi[0];
	    }
	    else {
	      ATH_MSG_ERROR( "#BTAG# Missing number in jet probabilities ! "<<probi.size());
	    }
	    if(w>bestMuWeight) {
	      bestMuWeight = w;
	      bestMuProbi  = probi;
	    } 
	    m_likelihoodTool->clear();
	  }
	}
	
	if( 0==softmInfo && probi.size() ) { // create a SoftMuonInfo only if >0 selected muon in jet
	//std::string instanceName(name());
	softmInfo = new SoftMuonInfo(instanceName.erase(0,8));
	jetToTag.addInfo(softmInfo);
  }
  // Add the SMTrackInfo
	if(m_writeInfoPlus && probi.size() ) {
	  SMTrackInfo tinfo(m_originalMuCollection,m,d0wrtPriVtx,ptrel,probi);
	  softmInfo->addTrackInfo(tinfo);
	}
      }
    }
    */

  }
  
  if (muonIndex!=-1) ATH_MSG_DEBUG(" #BTAG: choosing muon: " <<  muonIndex);

  // now decorate the b-tagging object
  std::string xAODBaseName="SMT";
  BTag->setVariable<float>(xAODBaseName, "mu_pt"           , jet_mu_dRmin_pt);
  BTag->setVariable<float>(xAODBaseName, "dR"              , jet_mu_dRmin_dR);
  BTag->setVariable<float>(xAODBaseName, "qOverPratio"     , jet_mu_dRmin_qOverPratio);
  BTag->setVariable<float>(xAODBaseName, "mombalsignif"    , jet_mu_dRmin_mombalsignif);
  BTag->setVariable<float>(xAODBaseName, "scatneighsignif" , jet_mu_dRmin_scatneighsignif);
  BTag->setVariable<float>(xAODBaseName, "pTrel"           , jet_mu_dRmin_pTrel);
  BTag->setVariable<float>(xAODBaseName, "mu_d0"           , jet_mu_dRmin_d0);
  BTag->setVariable<float>(xAODBaseName, "mu_z0"           , jet_mu_dRmin_z0);
  BTag->setVariable<float>(xAODBaseName, "ID_qOverP"       , jet_mu_dRmin_ID_qOverP_var);
  
  ElementLink<xAOD::MuonContainer> theLink; 
  if (muonIndex!=-1) theLink=assocMuons.at(muonIndex);
  
  BTag->auxdata< ElementLink<xAOD::MuonContainer> >("SMT_mu_link")=theLink; 

  /*
  if (softmInfo) {
    softmInfo->setTagLikelihood(bestMuProbi);
    double w(-100);
    if(m_algMode == "CHI2") w = bestMuWeight;
    else if(bestMuProbi.size()>1) {
      double pb = bestMuProbi[0];
      double pu = bestMuProbi[1];
      if(pb<=0.)       w = -30.;
      else if (pu<=0.) w = +100.;
      else             w = log(pb/pu);	  
    }
    softmInfo->setWeight(w);
    softmInfo->makeValid();
  }
  */

  //ATH_MSG_DEBUG( "#BTAG# tagJet is done" );

  return  StatusCode::SUCCESS;;
}



void SoftMuonTag::finalizeHistos() 
{
  if (m_runModus=="reference") {
    for(uint ijc=0;ijc<m_jetCollectionList.size();ijc++) {
      for(uint ih=0;ih<m_hypothese.size();ih++) {
	std::string hDir = "/BTAG/CALIB/"+m_hypothese[ih]+"/";
	m_likelihoodTool
	  ->smoothAndNormalizeHistogram( m_histoHelper->getHisto1D(hDir+"pT") , "" );
	m_likelihoodTool
	  ->smoothAndNormalizeHistogram( m_histoHelper->getHisto1D(hDir+"pTrel") , "" );
	m_likelihoodTool
	  ->smoothAndNormalizeHistogram( m_histoHelper->getHisto1D(hDir+"pTLowPt") , "" );
	m_likelihoodTool
	  ->smoothAndNormalizeHistogram( m_histoHelper->getHisto1D(hDir+"pTrelLowPt") , "" );
	m_likelihoodTool
	  ->smoothAndNormalizeHistogram( m_histoHelper->getHisto1D(hDir+"pTpTrel") , "" );
	m_likelihoodTool
	  ->smoothAndNormalizeHistogram( m_histoHelper->getHisto1D(hDir+"pTpTrelLowPt") , "" );
      }
    }
  }
  return;
}


void SoftMuonTag::printParameterSettings()
{
  ATH_MSG_INFO( "#BTAG# " << name() << "Parameter settings " );
  ATH_MSG_INFO( "#BTAG# I am in " << m_runModus << " modus." );
  ATH_MSG_INFO( "#BTAG# The method is "<<m_algMode);
  if (m_runModus == "reference") {
    ATH_MSG_INFO( "#BTAG# Preparing "<< m_refType<< "-jet probability density functions...");
  }
}

}


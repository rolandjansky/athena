/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          BJetBuilder.cxx  -  Description
                            -------------------
   begin   : 10.03.04
   authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
   email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
 
   changes : 13.01.05 (FD) introduce soft lepton case
 
***************************************************************************/

#include "BTagging/BJetBuilder.h"
#include "BTagging/BTagTool.h"

#include "CLHEP/Vector/LorentzVector.h"

// For Soft Lepton Tag
#include "egammaEvent/ElectronAssociation.h"
#include "MuonIDEvent/MuonAssociation.h"
#include "egammaEvent/PhotonAssociation.h"
#include "BTagging/BTagTrackAssociation.h"

// migration to new ParticleJet class
#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"
#include "JetTagEvent/TrackAssociation.h"

// Needed in case AOD --> AOD and keeping old TruthInfo 
#include "JetTagInfo/TruthInfo.h"
#include "JetTagInfo/SoftLeptonTruthInfo.h"

namespace Analysis {

  BJetBuilder::BJetBuilder( const std::string &n, ISvcLocator *p ) :
    AthAlgorithm( n, p ),
    m_runModus("analysis"),
    m_bTagTool( "Analysis::BTagTool" ),
    m_JetCollectionName( "JetCollection" ),
    m_jetFinderBasedOn("Cells"),
    m_BTagAssociation(true),
    m_BTagTrackAssocTool("Analysis::BTagTrackAssociation"),
      
    m_jetSelectionPtMin( 15.* CLHEP::GeV ),
    m_jetSelectionEtaMax( 2.5 ),
    m_jetSelectionNbTrackMin( 1 ),
    
    m_nbcall(0),
    m_nbjettagged(0),
    m_nbbjettagged(0),
    m_nbcjettagged(0),
    m_nbtjettagged(0),
    m_nctk(0.),
    m_ncmu(0.),
    m_ncmu2(0.),
    m_ncel(0.),
    m_wbip2d(0.),
    m_wuip2d(0.),
    m_waip2d(0.),
    m_wbcmb(0.),
    m_wucmb(0.),
    m_wacmb(0.),
    m_wbjetp(0.),
    m_wujetp(0.),
    m_wajetp(0.),
    m_wbsv0(0.),
    m_wusv0(0.),
    m_wasv0(0.),
    m_wbtrkc(0.),
    m_wutrkc(0.),
    m_watrkc(0.)
  {
    declareProperty( "Runmodus",                            m_runModus); // The run modus (reference/analysis)
    declareProperty( "BTagTool",                            m_bTagTool);
    declareProperty( "JetCollectionName",                   m_JetCollectionName );
    declareProperty( "jetFinderBasedOn",                    m_jetFinderBasedOn);
    declareProperty( "BTagAssociation",                     m_BTagAssociation);
    declareProperty( "BTagTrackAssocTool",          	    m_BTagTrackAssocTool);

    declareProperty( "JetSelectionPtMin",                   m_jetSelectionPtMin );
    declareProperty( "JetSelectionEtaMax",                  m_jetSelectionEtaMax );
    declareProperty( "JetSelectionNbTrackMin",              m_jetSelectionNbTrackMin );
    declareProperty( "TagJetIfNotSelected",                 m_tagJetIfNotSelected = true );
    declareProperty( "KeepTruthInfo",                       m_keepTruthInfo = true);
    declareProperty( "CollectionAppendix",                  m_collectionAppendix = "ReTagged" );
    //    declareProperty( "CollectionAppendix",                  m_collectionAppendix = "AOD" );
  }

  BJetBuilder::~BJetBuilder() {
  }

  StatusCode BJetBuilder::initialize() {
   
    /// retrieve the main BTagTool
    if ( m_bTagTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_bTagTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_bTagTool);
    }

    // retrieve the track association tool
    if ( !m_BTagTrackAssocTool.empty() ) {
      if ( m_BTagTrackAssocTool.retrieve().isFailure() ) {
        ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_BTagTrackAssocTool);
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_BTagTrackAssocTool);
      }
    }

    m_nbjettagged++;
    return StatusCode::SUCCESS;
  }


  StatusCode BJetBuilder::execute() {

    StatusCode sc;

    m_nbcall++;

    /* ----------------------------------------------------------------------------------- */
    /*      RETRIEVE NEW JET CONTAINER FROM STOREGATE                                      */
    /* ----------------------------------------------------------------------------------- */

    const JetCollection* tmpSGParticleJetContainer( 0 );
    sc = evtStore()->retrieve( tmpSGParticleJetContainer, m_JetCollectionName );
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR("#BTAG# Failed to retrieve collection " << m_JetCollectionName);
      return StatusCode::SUCCESS;
    }
    ATH_MSG_VERBOSE("#BTAG# Input jet collection contains " << tmpSGParticleJetContainer->size() << " jets.");

    jetcollection_t* JetsList = new jetcollection_t();
    for ( JetCollection::const_iterator i = tmpSGParticleJetContainer->begin(); 
	  i != tmpSGParticleJetContainer->end(); i++ ) {
      // Calo constituents are not necessarily in the AOD for the time being :
      // if the pointer to the first constituent is null, assume one cannot copy them
      Jet* newJet(0);
      if ( 0 != (*i)->size() )  {
	if ( 0 == (*((*i)->begin())) ) {
	  newJet = new Jet(*i,Jet::CopyMoments);
	} else
	  newJet = new Jet(*i,Jet::CopyMoments,Jet::CopyConstituents);
      } else {
	newJet = new Jet(*i,Jet::CopyMoments);
      }

      if (m_keepTruthInfo) {
	const Analysis::TruthInfo* Tinfo = (*i)->tagInfo<Analysis::TruthInfo>("TruthInfo");
	if (Tinfo) {
	  TruthInfo* truthInfo = new TruthInfo(*Tinfo);
	  newJet->addInfo(truthInfo);
	}
	const Analysis::TruthInfo* TinfoPU = (*i)->tagInfo<Analysis::TruthInfo>("TruthInfoPU");
	if (TinfoPU) {
	  TruthInfo* truthInfoPU = new TruthInfo(*TinfoPU);
	  newJet->addInfo(truthInfoPU);
	}
	const Analysis::SoftLeptonTruthInfo* SLTinfo = (*i)->tagInfo<Analysis::SoftLeptonTruthInfo>("SoftLeptonTruthInfo");
	if (SLTinfo) {
	  SoftLeptonTruthInfo* sltruthInfo = new SoftLeptonTruthInfo(*SLTinfo);
	  newJet->addInfo(sltruthInfo);
	}
      }
      JetsList->push_back(newJet);
    }

    /* ----------------------------------------------------------------------------------- */
    /*               Particle to Jet Associations                                          */
    /* ----------------------------------------------------------------------------------- */
    unsigned int ijet = 0;

    StatusCode jetIsAssociated;
    if (!m_BTagTrackAssocTool.empty()) {
       jetIsAssociated = m_BTagTrackAssocTool->BTagTrackAssociation_exec(JetsList);
    }
    if ( jetIsAssociated.isFailure() ) {
      ATH_MSG_ERROR("#BTAG# Failed to associate tracks to jet ");
      return StatusCode::FAILURE;
    }

    JetCollection* particleJetContainer = new JetCollection(SG::OWN_ELEMENTS);
    std::string newJetColl = m_JetCollectionName+m_collectionAppendix;
    sc = evtStore()->record( particleJetContainer, newJetColl );
    if ( sc.isFailure() ) { 
      ATH_MSG_ERROR("#BTAG# Failed to record new jet collection " << newJetColl);
      return StatusCode::SUCCESS;
    }

    for ( jetcollection_t::iterator jetOfCol = JetsList->begin();
          jetOfCol != JetsList->end(); jetOfCol++ ) {
          particleJetContainer->push_back(*jetOfCol);
    }
    delete JetsList;
    /* ----------------------------------------------------------------------------------- */
    /*                                  B-TAGGING                                          */
    /* ----------------------------------------------------------------------------------- */
    /* Loop over all jets in the event after jet preselection. */
    ijet = 0;
    if ( particleJetContainer->size() > 0 ) {
     
      for ( JetCollection::iterator jetOfCol = particleJetContainer->begin(); 
	    jetOfCol != particleJetContainer->end(); jetOfCol++ ) {

        Jet& jetToTag = ( **jetOfCol );

        /** Apply jet selection: */
        // the P4... interface does not inherit from LorentzVector :-(
	CLHEP::HepLorentzVector p4( jetToTag.px(), jetToTag.py(), jetToTag.pz(), jetToTag.e() );
        bool pass( true );
        if ( p4.perp() < m_jetSelectionPtMin )                     pass = false;
        if ( fabs( p4.pseudoRapidity() ) > m_jetSelectionEtaMax )  pass = false;
	const Analysis::TrackAssociation *trkInJet = jetToTag.getAssociation<TrackAssociation>("Tracks");
	if(!trkInJet){
	  ATH_MSG_WARNING("#BTAG#  Could not find tracks in jets");	
	  pass = false;
	}
        else if ( (int)jetToTag.getAssociation<TrackAssociation>("Tracks")->size() < m_jetSelectionNbTrackMin )  pass = false;
        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "#BTAG# Jet " << ijet 
	                                            << " Eta= " << p4.pseudoRapidity()
                                                    << " Phi= " << p4.phi() << " Et= " << p4.et() 
                                                    << " passed: " << pass << endreq;

	/** All jets are passed through the taggers (if requested).  */
        if ( ( pass || m_tagJetIfNotSelected ) ) {

          ATH_MSG_VERBOSE("#BTAG# Calling taggers...");

          sc = m_bTagTool->tagJet( jetToTag );
          if(sc.isFailure()) {}

	  m_nbjettagged++;

	  // monitor a bit the results (averages printed at end of run):
	  // a/ weights:
	  m_waip2d += jetToTag.getFlavourTagWeight("IP2D");
	  m_wacmb  += jetToTag.getFlavourTagWeight();
	  m_wajetp += jetToTag.getFlavourTagWeight("JetProb");
	  m_wasv0  += jetToTag.getFlavourTagWeight("SV0");
	  m_watrkc += jetToTag.getFlavourTagWeight("TrackCounting2D");
	  std::string label = "N/A";
	  const Analysis::TruthInfo* mcinfo = jetToTag.tagInfo<Analysis::TruthInfo>("TruthInfo");
	  if(mcinfo) {
	    label = mcinfo->jetTruthLabel();
	    if("B"==label) {
	      m_nbbjettagged++;
	      m_wbip2d += jetToTag.getFlavourTagWeight("IP2D");
	      m_wbcmb  += jetToTag.getFlavourTagWeight();
	      m_wbjetp += jetToTag.getFlavourTagWeight("JetProb");
	      m_wbsv0  += jetToTag.getFlavourTagWeight("SV0");
	      m_wbtrkc += jetToTag.getFlavourTagWeight("TrackCounting2D");
	    } else if("C"==label) {
	      m_nbcjettagged++;
	    } else if("T"==label) {
	      m_nbtjettagged++;
	    } else {
	      m_wuip2d += jetToTag.getFlavourTagWeight("IP2D");
	      m_wucmb  += jetToTag.getFlavourTagWeight();
	      m_wujetp += jetToTag.getFlavourTagWeight("JetProb");
	      m_wusv0  += jetToTag.getFlavourTagWeight("SV0");
	      m_wutrkc += jetToTag.getFlavourTagWeight("TrackCounting2D");
	    }
	  }
	  // b/ constituents:
	  int ntrk = 0;
	  int nele = 0;
	  int npho = 0;
	  int nmuo = 0;
	  int nmuo2 = 0;
	  if (jetToTag.getAssociation<TrackAssociation>("Tracks")) 
	    ntrk = jetToTag.getAssociation<TrackAssociation>("Tracks")->size();
	  if (jetToTag.getAssociation<ElectronAssociation>("Electrons")) 
	    nele = jetToTag.getAssociation<ElectronAssociation>("Electrons")->size();
	  if (jetToTag.getAssociation<PhotonAssociation>("Photons")) 
	    npho = jetToTag.getAssociation<PhotonAssociation>("Photons")->size();
	  if (jetToTag.getAssociation<MuonAssociation>("Muons")) 
	    nmuo = jetToTag.getAssociation<MuonAssociation>("Muons")->size();
	  if (!m_isMC || (m_isMC && "B" == label)) {
	    m_nctk += ntrk;
	    m_ncmu += nmuo;
	    m_ncmu2+= nmuo2;
	    m_ncel += nele;
	  }
	  msg(MSG::DEBUG) << "#BTAG# Jet " << ijet << " has after association: "
			  << ntrk << " tracks, " 
			  << nmuo << " muons, "
			  << nmuo2<< " second muons, "
			  << nele << " electrons and "
			  << npho << " potential softE photons." << endreq;
	  msg(MSG::VERBOSE) << "#BTAG# -> TAGRESULT for JET " << ijet << " : "
	                    << " label= " << label
	                    << " IP2D= " << jetToTag.getFlavourTagWeight("IP2D")
	                    << " IP3D+SV1= " << jetToTag.getFlavourTagWeight()
	                    << " JetProb= " << jetToTag.getFlavourTagWeight("JetProb")
	                    << " SV0= " << jetToTag.getFlavourTagWeight("SV0")
	                    << " TC2= " << jetToTag.getFlavourTagWeight("TrackCounting2D")
	                    << endreq;
	  
	  ijet++;
	  
        }
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode BJetBuilder::finalize() {
    if (m_runModus=="reference") m_bTagTool->finalizeHistos();
    if (msgLvl(MSG::INFO)) {
      int nbl = 0;
      if (m_isMC) nbl = m_nbjettagged - m_nbbjettagged - m_nbcjettagged - m_nbtjettagged;
      msg(MSG::INFO) << "#BTAG#  b-tagging summary: called "
		     << m_nbcall << " times on " 
		     << m_nbjettagged << " jets";
      if (m_isMC) msg(MSG::INFO) << " ("
				 << "b:" << m_nbbjettagged << ", "
				 << "c:" << m_nbcjettagged << ", "
				 << "tau:" << m_nbtjettagged << ", "
				 << "light:" << nbl << ") " << endreq;
      else msg(MSG::INFO) << endreq;
      std::string ConstitSumm = m_isMC ? 
	"#BTAG#  b-tagging summary for b-jet constituents: " : 
	"#BTAG#  b-tagging summary for jet constituents: ";
      int nalljet = m_isMC ? m_nbbjettagged : m_nbjettagged;
      msg(MSG::INFO) << ConstitSumm
		     << "<#trk>=" << (nalljet>0 ? m_nctk/(float)nalljet : 0)
		     << " <#mu>=" << (nalljet>0 ? m_ncmu/(float)nalljet : 0)
		     << " <#mu2>="<< (nalljet>0 ? m_ncmu2/(float)nalljet : 0)
		     << " <#el>=" << (nalljet>0 ? m_ncel/(float)nalljet : 0)
		     << endreq;
      msg(MSG::INFO) << "#BTAG#  b-tagging summary for TrackCounting: "
		     << "<w_all>=" << (m_nbjettagged>0 ? m_watrkc/(float)m_nbjettagged : 0);
      if (m_isMC) msg(MSG::INFO) 
                     << " <w_b>=" << (m_nbbjettagged>0 ? m_wbtrkc/(float)m_nbbjettagged : 0)
		     << " <w_u>=" << (nbl>0 ? m_wutrkc/(float)nbl : 0)
		     << endreq;
      else msg(MSG::INFO) << endreq;
      msg(MSG::INFO) << "#BTAG#  b-tagging summary for JetProb: "
		     << "<w_all>=" << (m_nbjettagged>0 ? m_wajetp/(float)m_nbjettagged : 0);
      if (m_isMC) msg(MSG::INFO) 
		     << " <w_b>=" << (m_nbbjettagged>0 ? m_wbjetp/(float)m_nbbjettagged : 0)
		     << " <w_u>=" << (nbl>0 ? m_wujetp/(float)nbl : 0)
		     << endreq;
      else msg(MSG::INFO) << endreq;
      msg(MSG::INFO) << "#BTAG#  b-tagging summary for SV0: "
		     << "<w_all>=" << (m_nbjettagged>0 ? m_wasv0/(float)m_nbjettagged : 0);
      if (m_isMC) msg(MSG::INFO) 
		     << " <w_b>=" << (m_nbbjettagged>0 ? m_wbsv0/(float)m_nbbjettagged : 0)
		     << " <w_u>=" << (nbl>0 ? m_wusv0/(float)nbl : 0)
		     << endreq;
      else msg(MSG::INFO) << endreq;
    /*
      msg(MSG::INFO) << "#BTAG#  b-tagging summary for IP2D: "
      << "<w_all>=" << (m_nbjettagged>0 ? m_waip2d/(float)m_nbjettagged : 0);
      if (m_isMC) msg(MSG::INFO) 
      << " <w_b>=" << (m_nbbjettagged>0 ? m_wbip2d/(float)m_nbbjettagged : 0)
      << " <w_u>=" << (nbl>0 ? m_wuip2d/(float)nbl : 0)
      << endreq;
      else msg(MSG::INFO) << endreq;
    */
      msg(MSG::INFO) << "#BTAG#  b-tagging summary for IP3D+SV1: "
		     << "<w_all>=" << (m_nbjettagged>0 ? m_wacmb/(float)m_nbjettagged : 0);
      if (m_isMC) msg(MSG::INFO) 
		     << " <w_b>=" << (m_nbbjettagged>0 ? m_wbcmb/(float)m_nbbjettagged : 0)
		     << " <w_u>=" << (nbl>0 ? m_wucmb/(float)nbl : 0)
		     << endreq;
      else msg(MSG::INFO) << endreq;
    }

    return StatusCode::SUCCESS;
  }

}


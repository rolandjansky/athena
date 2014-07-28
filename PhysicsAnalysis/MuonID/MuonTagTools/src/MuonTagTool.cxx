/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : MuonTagTool.cxx
Package : offline/PhysicsAnalysis/MuonID/MuonTagTools
Created : January 2006
Purpose : create a collection of MuonTag

*****************************************************************************/

#include "GaudiKernel/Property.h"

//#include "muonEvent/MuonContainer.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "MuonTagTools/MuonTagTool.h"
//#include "xAODTracking/VertexContainer.h"
//#include "xAODTracking/TrackParticleContainer.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "TagEvent/MuonAttributeNames.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
#include <sstream>


/** the constructor */
MuonTagTool::MuonTagTool (const std::string& type, const std::string& name, 
                                  const IInterface* parent) : 
  AthAlgTool( type, name, parent ),
  m_trackToVertexTool("Reco::TrackToVertex"),
  m_trackToVertexIPEstimator(""),
  m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool")
{

  /** Muon AOD Container Name */
  declareProperty("Container",          m_containerNames);

  /** selection cut of Pt */
  declareProperty("EtCut",              m_cut_Et = 6.0*CLHEP::GeV);

  /** overlap removal deltaR cut */
  declareProperty("OverlapDeltaRCut",   m_DRcut = 0.01);
  
  /** Calo Isolation cut values */
  declareProperty("CaloIsoCutValues",   m_caloisocutvalues, "Cut values for calo isolation");

  /** Calo Isolation cut values */
  declareProperty("TrackIsoCutValues",  m_trackisocutvalues, "Cut values for track isolation");

  /** Calo Isolation cut values */
  declareProperty("CaloRelIsoCutValues",   m_caloisorelcutvalues, "Cut values for relative calo isolation");

  /** Calo Isolation cut values */
  declareProperty("TrackRelIsoCutValues",  m_trackisorelcutvalues, "Cut values for relative track isolation");

  /** D0 preselection cut value */
  declareProperty("maxD0PreSelection",  m_maxD0preselection = 5*CLHEP::mm, "Cut values for track impact parameter");

  /** cosmic flag */
  declareProperty("isCosmics", m_cosmics = false);

  /** inner detector flag */ 
  declareProperty("doInDet", m_doInDet = true);

  /** key for primary vertex container */
  declareProperty ("PrimaryVertexKey", m_vxCandidate = "VxPrimaryCandidate");
  /** tool handle for track to vertex */
  declareProperty ("TrackToVertexTool",  m_trackToVertexTool);
  declareProperty ("TrackToVertexIPEstimator",  m_trackToVertexIPEstimator);
  /** D0 tight veto cut */
  declareProperty("maxD0tight",  m_maxD0tight = 0.3*CLHEP::mm);
  /** Z0 tight veto cut */
  declareProperty("maxZ0tight",  m_maxZ0tight = 2*CLHEP::mm);
  /** D0 significance loose veto cut */
  declareProperty("maxD0SignificanceLoose",  m_maxD0signLoose = 5 );
  /** D0 significance tight veto cut */
  declareProperty("maxD0SignificanceTight",  m_maxD0signTight = 3.5 );

  declareInterface<MuonTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode  MuonTagTool::initialize() {

  AthAlgTool::initialize().ignore();


  /** get TrackToVertexTool if Inner Detector is available */
  if(m_doInDet) {
    StatusCode sc = m_trackToVertexTool.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_FATAL ("Unable to retrieve handle on TrackToVertexTool");
      return sc;
    }
    if( !m_trackToVertexIPEstimator.empty() ){
      sc = m_trackToVertexIPEstimator.retrieve();
      if (sc.isFailure()) {
	ATH_MSG_FATAL ("Unable to retrieve " << m_trackToVertexIPEstimator);
	return sc;
      }
    }
  }

  if ( !m_idHelper.retrieve().isSuccess() ) {
    ATH_MSG_ERROR ("Unable to retrieve " << m_idHelper);
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


/** build the attribute list - called in initialize */
StatusCode MuonTagTool::attributeSpecification(std::map<std::string, AthenaAttributeType>& attrMap, const int max) {


  /** Muon Attributes */
  attrMap[ MuonAttributeNames[MuonTAG::NMuon] ]  = AthenaAttributeType("unsigned int", MuonAttributeUnitNames[MuonTAG::NMuon],  MuonAttributeGroupNames[MuonTAG::NMuon]);

  std::ostringstream os;
  for (int i=1; i<= max; ++i) {

    /** pt */
    os.str("");
    os << MuonAttributeNames[MuonTAG::Pt] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float", MuonAttributeUnitNames[MuonTAG::Pt], MuonAttributeGroupNames[MuonTAG::Pt]);
    m_ptStr.push_back( os.str() );

    /** eta */
    os.str("");
    os << MuonAttributeNames[MuonTAG::Eta] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float", MuonAttributeUnitNames[MuonTAG::Eta], MuonAttributeGroupNames[MuonTAG::Eta]);
    m_etaStr.push_back( os.str() );

    /** phi */
    os.str("");
    os << MuonAttributeNames[MuonTAG::Phi] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float", MuonAttributeUnitNames[MuonTAG::Phi], MuonAttributeGroupNames[MuonTAG::Phi]);
    m_phiStr.push_back( os.str() );

    /** Tightness */
    os.str("");
    os << MuonAttributeNames[MuonTAG::Tight] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("unsigned int", MuonAttributeUnitNames[MuonTAG::Tight], MuonAttributeGroupNames[MuonTAG::Tight]);
    m_tightStr.push_back( os.str() );

    /** isolation for Muons  */
    os.str("");
    os << MuonAttributeNames[MuonTAG::Isol] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("unsigned int", MuonAttributeUnitNames[MuonTAG::Isol], MuonAttributeGroupNames[MuonTAG::Isol]);
    m_isoStr.push_back( os.str() );

    /** precision layers for Muons  */
    os.str("");
    os << MuonAttributeNames[MuonTAG::NPrecLay] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("unsigned int", MuonAttributeUnitNames[MuonTAG::NPrecLay], MuonAttributeGroupNames[MuonTAG::NPrecLay]);
    m_nprecLayStr.push_back( os.str() );

    /** precision hole layers for Muons  */
    os.str("");
    os << MuonAttributeNames[MuonTAG::NPrecHoleLay] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("unsigned int", MuonAttributeUnitNames[MuonTAG::NPrecHoleLay], MuonAttributeGroupNames[MuonTAG::NPrecHoleLay]);
    m_nprecHoleLayStr.push_back( os.str() );

    /** phi layers for Muons  */
    os.str("");
    os << MuonAttributeNames[MuonTAG::NPhiLay] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("unsigned int", MuonAttributeUnitNames[MuonTAG::NPhiLay], MuonAttributeGroupNames[MuonTAG::NPhiLay]);
    m_nphiLayStr.push_back( os.str() );

    /** momentum balance significance for Muons  */
    os.str("");
    os << MuonAttributeNames[MuonTAG::MomBalSign] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("unsigned int", MuonAttributeUnitNames[MuonTAG::MomBalSign], MuonAttributeGroupNames[MuonTAG::MomBalSign]);
    m_momBalStr.push_back( os.str() );

  }

  return StatusCode::SUCCESS;
}

/** execute - called on every event */
StatusCode MuonTagTool::execute(TagFragmentCollection & muonTagCol, const int max) {
  
  ATH_MSG_INFO ("in execute()");

  std::vector<const xAOD::Muon*> unique_muons; // would be IParticle if mixed lepton collection
  
  /** now loop over the containers and merge STACCO and MuID containers */
  
  for ( unsigned int j=0; j<m_containerNames.size(); ++j ) {
    
    const xAOD::MuonContainer *muonContainer=0;
    StatusCode sc = evtStore()->retrieve( muonContainer, m_containerNames[j] );
    if (sc.isFailure()) {
      ATH_MSG_INFO ("Muon container not found in SG: " << m_containerNames[j]);
      continue;
    }

    /** loop over muons in container */
    xAOD::MuonContainer::const_iterator cm_it     = muonContainer->begin();
    xAOD::MuonContainer::const_iterator cm_it_end = muonContainer->end();
    for ( ; cm_it != cm_it_end ; ++cm_it) {
    
      ATH_MSG_INFO ("next Muon has pt = " <<  (*cm_it)->pt()/1000. << " Gev, eta = " <<  (*cm_it)->eta() << ", phi = " <<  (*cm_it)->phi());
      
      /** preselection of good muon candidate for tags */
      if ( (*cm_it)->pt() > m_cut_Et && (((*cm_it)->quality() == xAOD::Muon::Medium )|| ((*cm_it)->quality() == xAOD::Muon::Tight)) ) {//if it does not work use 1 instead of Medium// checking if the muon is at least medium
	
	ATH_MSG_INFO ("-> Muon passes preselection");

	/** not first container, do overlap removal */
	if ( j!=0 ) {

	  std::vector<const xAOD::Muon*>::const_iterator cm2_it = unique_muons.begin();
	  for ( ; cm2_it != unique_muons.end() ; ++cm2_it) {

	    /* now do a deltaR check... */
	    //using the DR from the p4 TLorentzVector
	    double dR= (*cm_it)->p4().DeltaR((*cm2_it)->p4());
	    if (dR<m_DRcut) {
	      ATH_MSG_INFO ("-> Muon fails dR overlap cut, dR = " << dR);
	      continue;
	    }
	  }
	  ATH_MSG_INFO ("-> Muon passes overlap check !");
	}
	  
	/** we apply impact preselection if no cosmics and no standalone muon */
	if (!m_cosmics && !((*cm_it)->muonType()==xAOD::Muon::MuonStandAlone) ) {
	  ATH_MSG_INFO ("-> Muon is combined or segment tagged, try impact veto against cosmics");

	  //double d0=0,z0=0, dd0=0,dz0=0,theta=0;
	  // getMuonImpactParameter (*cm_it, d0, z0, dd0, dz0, theta, true); not called still not updated.

	  // if ( abs(d0) > m_maxD0preselection ) {
	  //   ATH_MSG_INFO ("-> Muon fails impact veto, has d0 = " << d0 << " mm, for cosmics, reject it");//this cut is applied but d0 is always zero!	    
	  //   continue;
	  // } else {
	  //   ATH_MSG_INFO ("-> Muon passes impact veto, has d0 = " << d0 << " mm");	    
	  // }
	}

	ATH_MSG_INFO ("-> copy Muon for output !");
	unique_muons.push_back( *cm_it );
      } else {
	ATH_MSG_INFO ("-> Muon failed preselection !");
      }
    }
  }

  if ( unique_muons.size() > 1) {
    ATH_MSG_INFO ("sorting muon container");
    //    AnalysisUtils::Sort::pT( userMuonContainer );
    AnalysisUtils::Sort::pT( &unique_muons );
  }

  /** now fill muon TAG info */
  int i=0;
  std::vector<const xAOD::Muon*>::const_iterator muonItr  = unique_muons.begin();
  for (; muonItr != unique_muons.end() && i < max; ++muonItr, ++i) { //i<max

    const xAOD::Muon& muon = **muonItr;

    ATH_MSG_INFO("Muon: pt " << muon.pt()
		  //* muon.charge()
		  << "  eta " << muon.eta() << "  phi " << muon.phi() 
		  << " px " << muon.p4().Px() << " py " << muon.p4().Py() );
    
    /** pt */
    muonTagCol.insert( m_ptStr[i], muon.pt() );
		       //* muon.charge() );
    
    /** eta */
    muonTagCol.insert( m_etaStr[i], muon.eta() );

    /** phi */
    muonTagCol.insert( m_phiStr[i], muon.phi() );
    
    //comment out iso,track quality cuts

    unsigned int iso = 0x0;
    /* Calo relative Isolation in bits from 0 to 7 */
    float etcone = 0.0;
    if( ! muon.isolation(etcone,xAOD::Iso::etcone20) ){
      ATH_MSG_ERROR("No etcone defined");
    }
    etcone/=muon.pt();
    ATH_MSG_INFO("etcone20/pt = "<< etcone);
    
    //muon.parameter(MuonParameters::etcone20)/muon.pt();
    for (unsigned int j=0; j<m_caloisorelcutvalues.size(); j++) {
      if( j > 7 ) break; 
      if( etcone < m_caloisorelcutvalues[j] ) {
	ATH_MSG_INFO("Setting relative Calo Isolation: bit " << j );
	iso |= 1 << j;
      }
    }
    
    /* Calo Isolation in bits from 8 to 15 */
    
    if( ! muon.isolation(etcone,xAOD::Iso::etcone20) ){
      ATH_MSG_ERROR("No etcone defined");
    }
    
    ATH_MSG_INFO("etcone20 = "<< etcone);
    
    //muon.parameter(MuonParameters::etcone20);
    for (unsigned int j=0; j<m_caloisocutvalues.size(); j++) {
      if( j > 7 ) break; 
      if ( etcone < m_caloisocutvalues[j] ) {
	ATH_MSG_INFO("Setting absolute Calo Isolation: bit " << j+8 );
	iso |= 1 << (8+j);
      }
    } 
    
    /* Track Isolation in bits from 16 to 23 */ 
    
    float ptcone = 0.0 ;
    if( ! muon.isolation(ptcone,xAOD::Iso::ptcone20) ){
      ATH_MSG_ERROR("No ptcone defined");
    }
    ptcone/=muon.pt();
    ATH_MSG_INFO("ptcone20/pt = "<< ptcone);

    //     muon.parameter(MuonParameters::ptcone20)/muon.pt();
    for (unsigned int j=0; j<m_trackisorelcutvalues.size(); j++) {
      if( j > 7 ) break; 
      if ( ptcone < m_trackisorelcutvalues[j] ){
	ATH_MSG_INFO("Setting relative track Isolation: bit " << j+16 );
	iso |= 1 << (16+j);
      }
    } 
    
    /* Track Isolation in bits from 24 to 31 */ 
    
    //    ptcone = muon.parameter(MuonParameters::ptcone20);
    if( ! muon.isolation(ptcone,xAOD::Iso::ptcone20) ){
      ATH_MSG_ERROR("No ptcone defined");
    }
    
    ATH_MSG_INFO("ptcone20 = "<< ptcone);

    for (unsigned int j=0; j<m_trackisocutvalues.size(); j++) {
      if( j > 7 ) break; 
      if ( ptcone < m_trackisocutvalues[j] ) {
	ATH_MSG_INFO("Setting absolute track Isolation: bit " << j+24 );
	iso |= 1 << (24+j);
      }     
    }
    if( msgLvl(MSG::INFO) ){
      std::bitset<32> bits(iso);
      ATH_MSG_INFO("Isolation: " << iso << " bits: " << bits.to_string() );
    }

    muonTagCol.insert( m_isoStr[i], iso );
    
    /** varying levels of tighness cuts - to be defined and implemented */
    
    unsigned int tightness = 0x0; 
    if ( muon.muonType() == xAOD::Muon::MuonStandAlone )               tightness = tightness | bit2int(0);
    if ( muon.muonType() == xAOD::Muon::Combined )                 tightness = tightness | bit2int(1);
    if ( muon.muonType() == xAOD::Muon::SiliconAssociatedForwardMuon ) tightness = tightness | bit2int(2);
    if ( muon.muonType() == xAOD::Muon::SegmentTagged )            tightness = tightness | bit2int(3);
    if ( muon.quality() == xAOD::Muon::Medium ||  muon.quality() == xAOD::Muon::Tight )                       tightness = tightness | bit2int(4);

    //    ATH_MSG_VERBOSE("Muon types: SA " << muon.isStandAloneMuon() << " CB " << muon.isCombinedMuon() << " TAG " << muon.isSegmentTaggedMuon()		    << " FW " << muon.isSiliconAssociatedForwardMuon() << " Medium " << muon.isMedium() );
    
    /** no add the impact parameter bits for cosmic veto */
    
    double d0=0,z0=0,dd0=0;//,dz0=0,theta=0;
    //getMuonImpactParameter ((*muonItr), d0, z0, dd0, dz0, theta, true);
    double d0sig = dd0 != 0 ? fabs(d0/dd0) : 9999.;
    if ( d0sig < m_maxD0signLoose) tightness = tightness | bit2int(5);
    if ( d0sig < m_maxD0signTight) tightness = tightness | bit2int(6);

    if( !m_trackToVertexIPEstimator.empty() ){ 
      /** calculate unbiased impact parameter */
    
      //  getMuonImpactParameter ((*muonItr), d0, z0, dd0, dz0, theta, false);
      d0sig = dd0 != 0 ? fabs(d0/dd0) : 9999.;
      if ( d0sig < m_maxD0signLoose) tightness = tightness | bit2int(7);
      if ( d0sig < m_maxD0signTight) tightness = tightness | bit2int(8);
      
      if ( fabs(d0) < m_maxD0tight)  tightness = tightness | bit2int(9);
      if ( fabs(z0) < m_maxZ0tight)  tightness = tightness | bit2int(10);
    }
    
    
    
    /** ID track quality cuts */
    
    //    const Rec::TrackParticle* tp_id = muon.inDetTrackParticle();
    //retrieve the primaryTrackParticle
    //not all Muons has it.
    const ElementLink<xAOD::TrackParticleContainer> &  tp_p = muon.primaryTrackParticleLink();
    if(!tp_p){
      ATH_MSG_INFO("primary track particle not retrieved");
      continue;}
    if(tp_p){
      ATH_MSG_INFO("primary track particle retrieved");
    }
    //    const ElementLink<xAOD::TrackParticleContainer> &  tp_id = muon.inDetTrackParticle();
    const xAOD::TrackParticle* tp = 0;
    if( tp_p.isValid() ) {
      tp = *tp_p;
      ATH_MSG_INFO("tp_p IS VALID");
    }
    if( tp ){
      ATH_MSG_INFO("tp is created");
      uint8_t nblh   = 0x0;
      uint8_t eblh   = 0x0;
      uint8_t nblo   = 0x0;
      uint8_t nphi   = 0x0;
      uint8_t npds   = 0x0;
      uint8_t nsctds = 0x0;
      uint8_t nscthi = 0x0;
      uint8_t nsctho = 0x0;
      uint8_t npho   = 0x0;
      uint8_t ntrthi = 0x0;
      uint8_t ntrtol = 0x0;
      

      //WRITE ALL THE MESSAGE ERROR
      //build a function
      //if( tp_id && tp_id->trackSummary() ){//->????
      
      // check blayer 
      if( !tp->summaryValue(eblh,xAOD::expectBLayerHit) ||
	  tp->summaryValue(nblh,xAOD::numberOfBLayerHits) + tp->summaryValue(nblo,xAOD::numberOfBLayerOutliers) > 0 )   tightness = tightness | bit2int(11);
      
      // pixel hit counts
      if( tp->summaryValue(nphi,xAOD::numberOfPixelHits) + tp->summaryValue(npds,xAOD::numberOfPixelDeadSensors) >= 2 ) tightness = tightness | bit2int(12);

      // sct hit counts
      if( tp->summaryValue(nscthi,xAOD::numberOfSCTHits) + tp->summaryValue(nsctds,xAOD::numberOfSCTDeadSensors) > 5 )      tightness = tightness | bit2int(13);

      // hole cuts
      if( tp->summaryValue(npho,xAOD::numberOfPixelHoles) + tp->summaryValue(nsctho,xAOD::numberOfSCTHoles) < 2 )         tightness = tightness | bit2int(14);

      // trt cuts
      int ntrt = tp->summaryValue(ntrthi,xAOD::numberOfTRTHits) + tp->summaryValue(ntrtol,xAOD::numberOfTRTOutliers );
      bool pass = true;
      if(  ntrt > 5 && tp->summaryValue(ntrtol,xAOD::numberOfTRTOutliers )/(double)ntrt > 0.9 ) pass = false;                             
      if( (fabs(tp->eta()) > 0.1 && fabs(tp->eta()) < 1.9 ) && ntrt <= 5 )                  pass = false;                          
      if( pass ) tightness = tightness | bit2int(15);
    
    
    uint8_t nprecisionLayers=0;
    uint8_t nprecisionHoleLayers=0;
    uint8_t nphiLayers=0;
    
    if(!tp->summaryValue(nprecisionLayers,xAOD::numberOfPrecisionLayers)){
      ATH_MSG_ERROR("number of precision layers not retrieved");
    }
    
    if(!tp->summaryValue(nprecisionHoleLayers,xAOD::numberOfPrecisionHoleLayers)){
      ATH_MSG_ERROR("number of precision hole layers not retrieved");
    }
    
    if(!tp->summaryValue(nphiLayers,xAOD::numberOfPhiLayers)){
      ATH_MSG_ERROR("number of phi layers not retrieved");
    }
 
    muonTagCol.insert( m_nprecLayStr[i],     nprecisionLayers );       
    muonTagCol.insert( m_nprecHoleLayStr[i], nprecisionHoleLayers );       
    muonTagCol.insert( m_nphiLayStr[i],      nphiLayers );
    }//the if statement of ##tp# ends here.
    
    int nbar = 0;
    int nend = 0;
    int nbis78 = 0;
    int nee = 0;
    int nbee = 0;
    if( muon.muonType() == xAOD::Muon::MuonStandAlone || muon.muonType() == xAOD::Muon::Combined){
      ATH_MSG_INFO("Muon is StandAlone or Combined: try to retrieve the MuonSegmentContainer");
    
    //const Trk::MuonTrackSummary* muonSummary = muon.track() && muon.track()->trackSummary() ? muon.track()->trackSummary()->muonTrackSummary() : 0;
    // if( muonSummary ){
      // loop over chambers
      
     const xAOD::MuonSegmentContainer *muonSegContainer=0;
    StatusCode sc_one = evtStore()->retrieve( muonSegContainer, "MuonSegments" );
    if (sc_one.isFailure()) {
      ATH_MSG_INFO ("MuonSegment container not found in SG: ");
      continue;
    }
    if(!(sc_one.isFailure())){
      ATH_MSG_INFO("MuonSegment container found!");
    }

    /** loop over muons in container */
    xAOD::MuonSegmentContainer::const_iterator chit     = muonSegContainer->begin();
    xAOD::MuonSegmentContainer::const_iterator chit_end = muonSegContainer->end();
    //    for ( ; chi_it != csm_it_end ; ++csm_it) {

    //  std::vector<Trk::MuonTrackSummary::ChamberHitSummary>::const_iterator chit = muonSummary->chamberHitSummary().begin();
    // std::vector<Trk::MuonTrackSummary::ChamberHitSummary>::const_iterator chit_end = muonSummary->chamberHitSummary().end();
      for( ;chit!=chit_end;++chit ) {
	ATH_MSG_INFO("Starting iteration over chambers");
	//const Identifier& chId = chit->chamberId();
	Muon::MuonStationIndex::ChIndex chIndex = static_cast< Muon::MuonStationIndex::ChIndex >( (*chit)->chamberIndex() );
	  //m_idHelper->chamberIndex(chId);
	if( chIndex == Muon::MuonStationIndex::BIS ){
	  int eta = (*chit)->etaIndex();
	    //m_idHelper->stationEta(chId);
	  if( abs(eta) >= 7 ) nbis78 += (*chit)->nPrecisionHits();
	}
	if( chIndex == Muon::MuonStationIndex::BEE ){
	  nbee += (*chit)->nPrecisionHits();
	}
	//	if( m_idHelper->isEndcap(chId) ){
	//  nend += chit->nPrecisionHits();
	//  Muon::MuonStationIndex::StIndex stIndex = m_idHelper->stationIndex(chId);
	//	  if( stIndex == Muon::MuonStationIndex::EE ) nee += chit->nhits();
	//	}else{
	//  nbar += chit->nhits();
	//	}
	// }
      }
    }//put it back when you can find the muonquality
    if( nbar > 0 )   tightness = tightness | bit2int(16);
    if( nend > 0 )   tightness = tightness | bit2int(17);
    if( nbis78 > 0 ) tightness = tightness | bit2int(18);
    if( nee > 0 )    tightness = tightness | bit2int(19);
    if( nbee > 0 )   tightness = tightness | bit2int(20);
    // if( summary.nprecisionLayers > 2 && nbis78 == 0 && summary.nphiLayers > 0 && nbee == 0 && (nbar==0||nend==0) ) tightness = tightness | bit2int(21);
    // if( muon.fitChi2OverDoF() < 5 ) tightness = tightness | bit2int(22);
    // if( muon.fitChi2OverDoF() < 3 ) tightness = tightness | bit2int(23);
    // ATH_MSG_VERBOSE("Muon hits: nbar " << nbar << " nend " << nend << " nbis78 " << nbis78 << " nee " << nee	<< " nbee " << nbee << " nprec " << summary.nprecisionLayers << " nphi " << summary.nphiLayers << " chi2/ndof " << muon.fitChi2OverDoF() );

    if( msgLvl(MSG::DEBUG) ){
      std::bitset<32> bits(tightness);
      ATH_MSG_VERBOSE("Tightness: " << tightness << " bits: " << bits.to_string() );
    }

    
    ATH_MSG_INFO(""<< m_tightStr[i] << "tightness = " << tightness);
    muonTagCol.insert( m_tightStr[i], tightness );       
    
    /** Number of hits on  track */

    // momentumBalanceSignificance
    /* const uint8_t m_momentumBal = 0.0;
    if(! tp->parameter(m_momentumBal, xAOD::Muon::momentumBalanceSignificance)){
      ATH_MSG_ERROR("momentumBalanceSignificance not retrieved!");
    }
    
    muonTagCol.insert( m_momBalStr[i], m_momentumBal );
    */
    ATH_MSG_INFO ("writing muon number: " << i << " pt: " <<  muon.pt() / 1000. << " GeV, tightness: " << tightness << " iso " << iso);
    
  }
  /** insert the number of loose muons */
  muonTagCol.insert(MuonAttributeNames[MuonTAG::NMuon], i);

  /* manage memory yourself if the contain was not recorded into StoreGate
  if ( !evtStore()->contains<Analysis::MuonContainer>(key) ) {
     userMuonContainer->clear();
     delete userMuonContainer;     
     } */

  return StatusCode::SUCCESS;
}

/** finalize - called once at the end */
StatusCode MuonTagTool::finalize() {
  ATH_MSG_INFO ("in finalize()");
  return StatusCode::SUCCESS;
}

/** destructor */
MuonTagTool::~MuonTagTool() { 
}
 

//comment out the function
/** private function to get impact parameter */
/*void MuonTagTool::getMuonImpactParameter (const xAOD::Muon* muon, double& d0, double& z0, double& dd0, double& dz0, double& theta, bool biased ) {


  const ElementLink<xAOD::TrackParticleContainer> &  tp_prime = muon.primaryTrackParticleLink();
  if(!tp_prime){
  ATH_MSG_INFO("primary track particle not retrieved");
  continue;}
  if(tp_prime){
  ATH_MSG_INFO("primary track particle retrieved");
  }
  
  
  const xAOD::TrackParticle* tp = 0;
  if( tp_prime.isValid() ) {
  tp = *tp_prime;
  ATH_MSG_INFO("tp_prime IS VALID");
  }
  if(!tp){
  continue;
  }

  d0    = 0;
  z0    = 0;
  dd0   = 0;
  dz0   = 0;
  theta = 0;

  // If Inner Detector information is not available, we cannot use perigee with respect to vertex.  
  //if(!m_doInDet) return;
 
  const VxContainer* vxContainer(0);
  StatusCode sc = evtStore()->retrieve(vxContainer, m_vxCandidate);
  if (sc.isFailure() || !vxContainer) {
    ATH_MSG_DEBUG ("Could not retrieve primary vertex info: " << m_vxCandidate <<", return 0 impact parameter.");
    return;
  }
  if (vxContainer->size()<1) {
    ATH_MSG_DEBUG ("No primary vertices reconstructed, return 0 impact parameter.");
    return;
  } 
  VxContainer::const_iterator vxI = vxContainer->begin();

  ATH_MSG_DEBUG ("---> vertex at (x/y/z) = " << 
		 ((*vxI)->recVertex().position())[0] << " / " <<
		 ((*vxI)->recVertex().position())[1] << " / " <<
		 ((*vxI)->recVertex().position())[2]);

  // primary track
  const Rec::TrackParticle* track = muon->track();
  bool trackok = (track && track->measuredPerigee());
  if(!trackok) {
    ATH_MSG_DEBUG ("Muon (primary) has no perigee or TrackParticle, return 0 impact parameter.");
    return;
  }

  if( biased ){
    const Trk::TrackParameters* perigee 
      = m_trackToVertexTool->perigeeAtVertex((*track), (*vxI)->recVertex().position());
    if(!perigee) {
      ATH_MSG_DEBUG ("Extrapolation failed, return 0 impact parameter.");
      return;
    }
    
    d0    = (perigee->parameters())[Trk::d0];
    z0    = (perigee->parameters())[Trk::z0];
    theta = (perigee->parameters())[Trk::theta];
    dd0   = dz0 = -99.;
    if (perigee->covariance()) {
      dd0   = sqrt((*perigee->covariance())(Trk::d0,Trk::d0));
      dz0   = sqrt((*perigee->covariance())(Trk::z0,Trk::z0));
    }
    delete perigee;

  }else{
    
    const Trk::ImpactParametersAndSigma* myIPandSigma = !m_trackToVertexIPEstimator.empty() ? m_trackToVertexIPEstimator->estimate( track, track, *vxContainer->begin(), true ) : 0;
    if( !myIPandSigma ){
      ATH_MSG_WARNING ( "trackToVertexIPEstimator at PV failed !");
    }else{
      z0 = myIPandSigma->IPz0;
      d0 = myIPandSigma->IPd0;
      dd0 = myIPandSigma->sigmad0;
      dz0 = myIPandSigma->sigmaz0;
    }
    if ( myIPandSigma ) delete myIPandSigma;
  }
  ATH_MSG_VERBOSE ("---> Impact Parameters: d0 = " << d0 << " mm, z0 = " << z0 << " mm"); 

  return;
}
*/

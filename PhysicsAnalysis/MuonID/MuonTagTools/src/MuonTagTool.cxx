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

#include "xAODMuon/MuonContainer.h"
#include "MuonTagTools/MuonTagTool.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TagEvent/MuonAttributeNames.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"
#include <sstream>


/** the constructor */
MuonTagTool::MuonTagTool (const std::string& type, const std::string& name, 
                                  const IInterface* parent) : 
  AthAlgTool( type, name, parent )
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
  declareProperty ("PrimaryVertexKey", m_vxCandidate = "PrimaryVertices");

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
    attrMap[ os.str() ] = AthenaAttributeType("float", MuonAttributeUnitNames[MuonTAG::MomBalSign], MuonAttributeGroupNames[MuonTAG::MomBalSign]);
    m_momBalStr.push_back( os.str() );

  }

  return StatusCode::SUCCESS;
}

/** execute - called on every event */
StatusCode MuonTagTool::execute(TagFragmentCollection & muonTagCol, const int max) {
  
  ATH_MSG_DEBUG ("in execute()");

  std::vector<const xAOD::Muon*> unique_muons; // would be IParticle if mixed lepton collection
  
  /** now loop over the containers and merge STACCO and MuID containers */
  
  for ( unsigned int j=0; j<m_containerNames.size(); ++j ) {
    
    const xAOD::MuonContainer *muonContainer=0;
    StatusCode sc = evtStore()->retrieve( muonContainer, m_containerNames[j] );
    if (sc.isFailure()) {
      ATH_MSG_DEBUG ("Muon container not found in SG: " << m_containerNames[j]);
      continue;
    }

    /** loop over muons in container */
    xAOD::MuonContainer::const_iterator cm_it     = muonContainer->begin();
    xAOD::MuonContainer::const_iterator cm_it_end = muonContainer->end();
    for ( ; cm_it != cm_it_end ; ++cm_it) {
    
      ATH_MSG_DEBUG ("next Muon has pt = " <<  (*cm_it)->pt()/1000. << " Gev, eta = " <<  (*cm_it)->eta() << ", phi = " <<  (*cm_it)->phi());
      
      /** preselection of good muon candidate for tags */
      if ( (*cm_it)->pt() > m_cut_Et && (((*cm_it)->quality() == xAOD::Muon::Medium )|| ((*cm_it)->quality() == xAOD::Muon::Tight)) ) {//if it does not work use 1 instead of Medium// checking if the muon is at least medium
	
	ATH_MSG_DEBUG ("-> Muon passes preselection");

	/** not first container, do overlap removal */
	if ( j!=0 ) {

	  std::vector<const xAOD::Muon*>::const_iterator cm2_it = unique_muons.begin();
	  for ( ; cm2_it != unique_muons.end() ; ++cm2_it) {

	    /* now do a deltaR check... */
	    //using the DR from the p4 TLorentzVector
	    double dR= (*cm_it)->p4().DeltaR((*cm2_it)->p4());
	    if (dR<m_DRcut) {
	      ATH_MSG_DEBUG ("-> Muon fails dR overlap cut, dR = " << dR);
	      continue;
	    }
	  }
	  ATH_MSG_DEBUG ("-> Muon passes overlap check !");
	}
	  
	/** we apply impact preselection if no cosmics and no standalone muon */
	if (!m_cosmics && !((*cm_it)->muonType()==xAOD::Muon::MuonStandAlone) ) {
	  ATH_MSG_DEBUG ("-> Muon is combined or segment tagged, try impact veto against cosmics");

	  double d0=0,z0=0, dd0=0,dz0=0,theta=0;
	  getMuonImpactParameter (*cm_it, d0, z0, dd0, dz0, theta); 
	  
	  if ( abs(d0) > m_maxD0preselection ) {
	    ATH_MSG_DEBUG ("-> Muon fails impact veto, ha d0 = " << d0 << " mm, for cosmics, reject it");
	    continue;
	  } else {
	    ATH_MSG_DEBUG ("-> Muon passes impact veto, has d0 = " << d0 << " mm");	    
	  }
	}

	ATH_MSG_DEBUG ("-> copy Muon for output !");
	unique_muons.push_back( *cm_it );
      } else {
	ATH_MSG_DEBUG ("-> Muon failed preselection !");
      }
    }
  }

  if ( unique_muons.size() > 1) {
    ATH_MSG_DEBUG ("sorting muon container");
    //    AnalysisUtils::Sort::pT( userMuonContainer );
    AnalysisUtils::Sort::pT( &unique_muons );
  }
  
  /** now fill muon TAG info */
  int i=0;
  std::vector<const xAOD::Muon*>::const_iterator muonItr  = unique_muons.begin();
  for (; muonItr != unique_muons.end() && i < max; ++muonItr, ++i) { //i<max
    
    const xAOD::Muon& muon = **muonItr;
    
    ATH_MSG_DEBUG("Muon: pt " << muon.pt()
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
    else{
      float etcone_rel= etcone/muon.pt();
      ATH_MSG_DEBUG("etcone20/pt = "<< etcone_rel);
      
      
      for (unsigned int j=0; j<m_caloisorelcutvalues.size(); j++) {
	if( j > 7 ) break; 
	if( etcone_rel < m_caloisorelcutvalues[j] ) {
	  ATH_MSG_DEBUG("Setting relative Calo Isolation: bit " << j );
	  iso |= 1 << j;
	}
      }
      
      /* Calo Isolation in bits from 8 to 15 */
      
      for (unsigned int j=0; j<m_caloisocutvalues.size(); j++) {
	if( j > 7 ) break; 
	if ( etcone < m_caloisocutvalues[j] ) {
	  ATH_MSG_DEBUG("Setting absolute Calo Isolation: bit " << j+8 );
	  iso |= 1 << (8+j);
	}
      } 
    }
    if( ! muon.isolation(etcone,xAOD::Iso::topoetcone20) ){
      ATH_MSG_ERROR("No topoetcone20 defined");
    }
    else{
      float etcone_rel= etcone/muon.pt();
      ATH_MSG_DEBUG("topoetcone20/pt = "<< etcone_rel);
      
      
      for (unsigned int j=0; j<m_caloisorelcutvalues.size(); j++) {
	if( j > 7 ) break; 
	if( etcone_rel < m_caloisorelcutvalues[j] ) {
	  ATH_MSG_DEBUG("Setting relative Calo Isolation: bit " << j );
	  iso |= 1 << (2 + j);
	}
      }
      
      /* Calo Isolation in bits from 8 to 15 */
      
      

      for (unsigned int j=0; j<m_caloisocutvalues.size(); j++) {
	if( j > 7 ) break; 
	if ( etcone < m_caloisocutvalues[j] ) {
	  ATH_MSG_DEBUG("Setting absolute Calo Isolation: bit " << j+8 );
	  iso |= 1 << (10+j);
	}
      } 
    }
    if( ! muon.isolation(etcone,xAOD::Iso::topoetcone30) ){
      ATH_MSG_ERROR("No topoetcone30 defined");
    }
    else{
      float etcone_rel= etcone/muon.pt();
      ATH_MSG_DEBUG("topoetcone30/pt = "<< etcone_rel);
      
      
      for (unsigned int j=0; j<m_caloisorelcutvalues.size(); j++) {
	if( j > 7 ) break; 
	if( etcone_rel < m_caloisorelcutvalues[j] ) {
	  ATH_MSG_DEBUG("Setting relative Calo Isolation: bit " << j );
	  iso |= 1 << (4 + j);
	}
      }
      
      /* Calo Isolation in bits from 8 to 15 */
      
      for (unsigned int j=0; j<m_caloisocutvalues.size(); j++) {
	if( j > 7 ) break; 
	if ( etcone < m_caloisocutvalues[j] ) {
	  ATH_MSG_DEBUG("Setting absolute Calo Isolation: bit " << j+8 );
	  iso |= 1 << (12+j);
	}
      } 
    }

    if( ! muon.isolation(etcone,xAOD::Iso::topoetcone40) ){
      ATH_MSG_ERROR("No topoetcone40 defined");
    }
    else{
      float etcone_rel= etcone/muon.pt();
      ATH_MSG_DEBUG("topoetcone40/pt = "<< etcone_rel);
      
      
      for (unsigned int j=0; j<m_caloisorelcutvalues.size(); j++) {
	if( j > 7 ) break; 
	if( etcone_rel < m_caloisorelcutvalues[j] ) {
	  ATH_MSG_DEBUG("Setting relative Calo Isolation: bit " << j );
	  iso |= 1 << (6 + j);
	}
      }
      
      /* Calo Isolation in bits from 8 to 15 */
      
      for (unsigned int j=0; j<m_caloisocutvalues.size(); j++) {
	if( j > 7 ) break; 
	if ( etcone < m_caloisocutvalues[j] ) {
	  ATH_MSG_DEBUG("Setting absolute Calo Isolation: bit " << j+8 );
	  iso |= 1 << (14+j);
	}
      } 
    }

    /* Track Isolation in bits from 16 to 23 */ 
    
    float ptcone = 0.0 ;
    if( ! muon.isolation(ptcone,xAOD::Iso::ptcone20) ){
      ATH_MSG_ERROR("No ptcone20 defined");
    }
    else{
      float ptcone_rel= ptcone/muon.pt();
      ATH_MSG_DEBUG("ptcone20/pt = "<< ptcone_rel);
      
      for (unsigned int j=0; j<m_trackisorelcutvalues.size(); j++) {
	if( j > 7 ) break; 
	if ( ptcone_rel < m_trackisorelcutvalues[j] ){
	  ATH_MSG_DEBUG("Setting relative track Isolation: bit " << j+16 );
	  iso |= 1 << (16+j);
	}
      } 
      
      /* Track Isolation in bits from 24 to 31 */ 
      
      
      ATH_MSG_DEBUG("ptcone20 = "<< ptcone);
      
      for (unsigned int j=0; j<m_trackisocutvalues.size(); j++) {
	if( j > 7 ) break; 
	if ( ptcone < m_trackisocutvalues[j] ) {
	  ATH_MSG_DEBUG("Setting absolute track Isolation: bit " << j+24 );
	  iso |= 1 << (24+j);
	}     
      }
    }

    if( ! muon.isolation(ptcone,xAOD::Iso::ptcone30) ){
      ATH_MSG_ERROR("No ptcone30 defined");
    }
    else{
      float ptcone_rel= ptcone/muon.pt();
      ATH_MSG_DEBUG("ptcone30/pt = "<< ptcone_rel);
      
      for (unsigned int j=0; j<m_trackisorelcutvalues.size(); j++) {
	if( j > 7 ) break; 
	if ( ptcone_rel < m_trackisorelcutvalues[j] ){
	  ATH_MSG_DEBUG("Setting relative track Isolation: bit " << j+16 );
	  iso |= 1 << (18+j);
	}
      } 
      
      /* Track Isolation in bits from 24 to 31 */ 
      
      
      ATH_MSG_DEBUG("ptcone30 = "<< ptcone);
      
      for (unsigned int j=0; j<m_trackisocutvalues.size(); j++) {
	if( j > 7 ) break; 
	if ( ptcone < m_trackisocutvalues[j] ) {
	  ATH_MSG_DEBUG("Setting absolute track Isolation: bit " << j+24 );
	  iso |= 1 << (26+j);
	}     
      }
    }

    if( ! muon.isolation(ptcone,xAOD::Iso::ptcone40) ){
      ATH_MSG_ERROR("No ptcone40 defined");
    }
    else{
      float ptcone_rel= ptcone/muon.pt();
      ATH_MSG_DEBUG("ptcone40/pt = "<< ptcone_rel);
      
      for (unsigned int j=0; j<m_trackisorelcutvalues.size(); j++) {
	if( j > 7 ) break; 
	if ( ptcone_rel < m_trackisorelcutvalues[j] ){
	  ATH_MSG_DEBUG("Setting relative track Isolation: bit " << j+16 );
	  iso |= 1 << (20+j);
	}
      } 
      
      /* Track Isolation in bits from 24 to 31 */ 
      
      
      ATH_MSG_DEBUG("ptcone40 = "<< ptcone);
      
      for (unsigned int j=0; j<m_trackisocutvalues.size(); j++) {
	if( j > 7 ) break; 
	if ( ptcone < m_trackisocutvalues[j] ) {
	  ATH_MSG_DEBUG("Setting absolute track Isolation: bit " << j+24 );
	  iso |= 1 << (28+j);
	}     
      }
    }
    if( msgLvl(MSG::DEBUG) ){
      std::bitset<32> bits(iso);
      ATH_MSG_DEBUG("Isolation: " << iso << " bits: " << bits.to_string() );
    }
    
    muonTagCol.insert( m_isoStr[i], iso );
    
    /** varying levels of tighness cuts - to be defined and implemented */
    
    unsigned int tightness = 0x0; 
    if ( muon.muonType() == xAOD::Muon::MuonStandAlone )               tightness = tightness | bit2int(0);
    if ( muon.muonType() == xAOD::Muon::Combined )                 tightness = tightness | bit2int(1);
    if ( muon.muonType() == xAOD::Muon::SiliconAssociatedForwardMuon ) tightness = tightness | bit2int(2);
    if ( muon.muonType() == xAOD::Muon::SegmentTagged )            tightness = tightness | bit2int(3);
    if ( muon.quality() == xAOD::Muon::Medium ||  muon.quality() == xAOD::Muon::Tight )                       tightness = tightness | bit2int(4);
    
    /** Add the impact parameter bits for cosmic veto */
    
    double d0=0,z0=0,dd0=0,dz0=0,theta=0;
    getMuonImpactParameter ((*muonItr), d0, z0, dd0, dz0, theta);
    double d0sig = dd0 != 0 ? fabs(d0/dd0) : 9999.;
    if ( d0sig < m_maxD0signLoose) tightness = tightness | bit2int(5);
    if ( d0sig < m_maxD0signTight) tightness = tightness | bit2int(6);
    if ( fabs(d0) < m_maxD0tight)  tightness = tightness | bit2int(9);
    if ( fabs(z0) < m_maxZ0tight)  tightness = tightness | bit2int(10);

    /** ID track quality cuts */
   
    const ElementLink<xAOD::TrackParticleContainer> &  tp_p = muon.primaryTrackParticleLink();
    if(!tp_p){
      ATH_MSG_DEBUG("primary track particle not retrieved");
      continue;}
    if(tp_p){
      ATH_MSG_DEBUG("primary track particle retrieved");
    }
   
   const xAOD::TrackParticle* tp = 0;
    if( tp_p.isValid() ) {
      tp = *tp_p;
      ATH_MSG_DEBUG("tp_p IS VALID");
    }
    if( tp ){
      ATH_MSG_DEBUG("tp is created");
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
      
      if( !tp->summaryValue(eblh,xAOD::expectBLayerHit)){
      	ATH_MSG_WARNING("No expectBLayerHit");
      }
      if( !tp->summaryValue(eblh,xAOD::expectBLayerHit)){
	ATH_MSG_WARNING("No expectBLayerHit");
      }
      if( !tp->summaryValue(nblh,xAOD::numberOfBLayerHits)){
	ATH_MSG_WARNING("No nBLayerHits");
      }
      if( !tp->summaryValue(nblo,xAOD::numberOfBLayerOutliers)){
	ATH_MSG_WARNING("No nofBLayerOutliers");
      }
      if( !tp->summaryValue(nphi,xAOD::numberOfPixelHits)){
	ATH_MSG_WARNING("No numberOfPixelHits");
      }
      if( !tp->summaryValue(npds,xAOD::numberOfPixelDeadSensors)){
	ATH_MSG_WARNING("No numberOfPixelDeadSensors");
      }
      if( !tp->summaryValue(npho,xAOD::numberOfPixelHoles)){
	ATH_MSG_WARNING("No numberOfPixelHoles");
      }
      if( !tp->summaryValue(nsctho,xAOD::numberOfSCTHoles)){
	ATH_MSG_WARNING("No numberOfSCTHoles");
      }

      if( !tp->summaryValue(nscthi,xAOD::numberOfSCTHits)){
	ATH_MSG_WARNING("No numberOfSCTHits");
      }

      if( !tp->summaryValue(nsctds,xAOD::numberOfSCTDeadSensors)){
	ATH_MSG_WARNING("No numberOfSCTDeadSensors");
      }

      if( !tp->summaryValue(ntrthi,xAOD::numberOfTRTHits)){
	ATH_MSG_WARNING("No numberOfTRTHits");
      }
      if( !tp->summaryValue(ntrtol,xAOD::numberOfTRTOutliers)){
	ATH_MSG_WARNING("No numberOfTRTHits");
      }

      // check blayer 
      if( !tp->summaryValue(eblh,xAOD::expectBLayerHit) ||
	  (nblh + nblo > 0) )   tightness = tightness | bit2int(11);
      
      // pixel hit counts
      if( nphi + npds >= 2 ) tightness = tightness | bit2int(12);
      
      // sct hit counts
      if( nscthi + nsctds > 5 )      tightness = tightness | bit2int(13);
      
      // hole cuts
      if( npho + nsctho < 2 )         tightness = tightness | bit2int(14);
      
      // trt cuts
      int ntrt = ntrthi+ ntrtol;
      bool pass = true;
      if(  ntrt > 5 && ntrtol/(double)ntrt > 0.9 ) pass = false;                             
      if( (fabs(tp->eta()) > 0.1 && fabs(tp->eta()) < 1.9 ) && ntrt <= 5 )                  pass = false;                          
      if( pass ) tightness = tightness | bit2int(15);

      //check ID cuts and HightPtCuts

      if( muon.passesIDCuts() == 1 )   tightness = tightness | bit2int(16);
      if( muon.passesHighPtCuts() == 1  )   tightness = tightness | bit2int(21);
      
      float m_segChi2OverDoF = 0.0;
      if(! muon.parameter(m_segChi2OverDoF, xAOD::Muon::segmentChi2OverDoF)){
      	ATH_MSG_ERROR("segmentChi2OverDoF not retrieved!");
      }
      if( m_segChi2OverDoF < 5 ) tightness = tightness | bit2int(22);
      if( m_segChi2OverDoF < 3 ) tightness = tightness | bit2int(23);
      
      if( msgLvl(MSG::DEBUG) ){
	std::bitset<32> bits(tightness);
	ATH_MSG_VERBOSE("Tightness: " << tightness << " bits: " << bits.to_string() );
      }
      
      
      muonTagCol.insert( m_tightStr[i], tightness );       
      
      
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
      
      
      /** Number of hits on  track */
      
      // momentumBalanceSignificance
      float m_momentumBal = 0.0;
      if(! muon.parameter(m_momentumBal, xAOD::Muon::momentumBalanceSignificance)){
	ATH_MSG_WARNING("momentumBalanceSignificance not retrieved!");
      }
      
      muonTagCol.insert( m_momBalStr[i], m_momentumBal );
      
      //ATH_MSG_DEBUG ("writing muon number: " << i << " pt: " <<  muon.pt() / 1000. << " GeV, tightness: " << tightness << " iso " << iso);
      
    }
    /** insert the number of loose muons */
    muonTagCol.insert(MuonAttributeNames[MuonTAG::NMuon], i);
  }
  /* manage memory yourself if the contain was not recorded into StoreGate
     if ( !evtStore()->contains<Analysis::MuonContainer>(key) ) {
     userMuonContainer->clear();
     delete userMuonContainer;     
     } */
  
  return StatusCode::SUCCESS;
}

/** finalize - called once at the end */
StatusCode MuonTagTool::finalize() {
  ATH_MSG_DEBUG ("in finalize()");
  return StatusCode::SUCCESS;
}

/** destructor */
MuonTagTool::~MuonTagTool() { 
}
 

//comment out the function
/** private function to get impact parameter */
void MuonTagTool::getMuonImpactParameter (const xAOD::Muon* muon, double& d0, double& z0, double& dd0, double& dz0, double& theta) {


  
  d0    = 0;
  z0    = 0;
  dd0   = -99;
  dz0   = -99;
  theta = 0;

  // If Inner Detector information is not available, we cannot use perigee with respect to vertex.  
  //if(!m_doInDet) return;
 
  const xAOD::VertexContainer* vxContainer=0;
  StatusCode sc = evtStore()->retrieve(vxContainer, m_vxCandidate);
  if (sc.isFailure() || !vxContainer) {
    ATH_MSG_DEBUG ("Could not retrieve primary vertex info: " << m_vxCandidate <<", return 0 impact parameter.");
    return;
  }
  if (vxContainer->size()<1) {
    ATH_MSG_DEBUG ("No primary vertices reconstructed, return 0 impact parameter.");
    return;
  } 
  xAOD::VertexContainer::const_iterator vxI     = vxContainer->begin();

  ATH_MSG_DEBUG ("---> vertex at (x/y/z) = " << 
		 (*vxI)->x() << " / " <<
		 (*vxI)->y() << " / " <<
		 (*vxI)->z() );

  // primary track
  
  const ElementLink<xAOD::TrackParticleContainer> &  tp_prime = muon->primaryTrackParticleLink();
  if(!tp_prime){
  ATH_MSG_DEBUG("primary track particle not retrieved");
  }
  if(tp_prime){
  ATH_MSG_DEBUG("primary track particle retrieved");
  }
  
  
  const xAOD::TrackParticle* tp = 0;
  if( tp_prime.isValid() ) {
  tp = *tp_prime;
  ATH_MSG_DEBUG("tp_prime IS VALID");

  d0    = tp->d0();
  z0    = tp->z0();
  theta = tp->theta();
  dd0   = dz0 = -99.;

 
  dd0 = sqrt(tp->definingParametersCovMatrix()(0,0));
  dz0 = sqrt(tp->definingParametersCovMatrix()(1,1));

  ATH_MSG_DEBUG("d0 = " <<  d0 << " z0 = " <<z0 << " theta = " << theta << " dd0 = " << dd0 << " dz0 = " << dz0); 
  }
  if(!tp){
     ATH_MSG_DEBUG("tp_prime IS NOT VALID");
  }

  return;
}


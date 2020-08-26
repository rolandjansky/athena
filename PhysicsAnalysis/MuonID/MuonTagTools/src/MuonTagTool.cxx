/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : MuonTagTool.cxx
Package : offline/PhysicsAnalysis/MuonID/MuonTagTools
Purpose : create a collection of MuonTag

*****************************************************************************/

#include "Gaudi/Property.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODParticleEvent/IParticleLink.h"
#include "AthContainers/ConstDataVector.h"
#include "xAODMuon/MuonContainer.h"
#include "MuonTagTools/MuonTagTool.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "TagEvent/MuonAttributeNames.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"
#include "xAODEventInfo/EventInfo.h"
#include <sstream>
#include <cmath>


/** the constructor */
MuonTagTool::MuonTagTool (const std::string& type, const std::string& name, 
                          const IInterface* parent) : 
  AthAlgTool( type, name, parent ),
  /**Initializing private member for the likelihood and isolation tool*/
  m_loose_trackonly_isolation(""),
  m_loose_isolation(""),
  m_tight_isolation(""),
  m_gradient_loose_isolation(""),
  m_gradient_isolation(""),
  m_fixedcut_tight_trackonly_isolation(""),
  m_fixedcut_loose_isolation("") {
  
  /** Muon AOD Container Name */
  declareProperty("Container",            m_containerNames);

  /** Muon MET input container name*/
  declareProperty("MuonMETContainerName", m_muon_met_container_name);
  
  /** selection cut of Pt */
  declareProperty("PtCut",                m_cut_Pt = 6.0*CLHEP::GeV);
  
  /** overlap removal deltaR cut */
  declareProperty("OverlapDeltaRCut",     m_DRcut = 0.01);
  
  /** Calo Isolation cut values */
  declareProperty("EtconeIsoCutValues",   m_etconeisocutvalues, "Cut values for Etcone isolation");
  
  /** Calo Isolation cut values */
  declareProperty("PtconeIsoCutValues",   m_ptconeisocutvalues, "Cut values for Ptcone isolation");
  
  /** Calo Isolation cut values */
  declareProperty("EtconeRelIsoCutValues",m_etconeisorelcutvalues, "Cut values for Etcone/pt isolation");
  
  /** Calo Isolation cut values */
  declareProperty("PtconeRelIsoCutValues",m_ptconeisorelcutvalues, "Cut values for Ptcone/pt isolation");
  
  /** D0 preselection cut value */
  declareProperty("maxD0PreSelection",    m_maxD0preselection = 5*CLHEP::mm, "Cut values for track impact parameter");
  
  /** cosmic flag */
  declareProperty("isCosmics",            m_cosmics = false);
  
  /** inner detector flag */ 
  declareProperty("doInDet",              m_doInDet = true);
  
  /** key for primary vertex container */
  declareProperty ("PrimaryVertexKey",    m_vxCandidate = "PrimaryVertices");
  
  /** D0 tight veto cut */
  declareProperty("maxD0tight",              m_maxD0tight = 0.3*CLHEP::mm);
  /** Z0 tight veto cut */
  declareProperty("maxZ0tight",              m_maxZ0tight = 2*CLHEP::mm);
  /** Z0 loose veto cut */
  declareProperty("maxZ0loose",              m_maxZ0loose = 10*CLHEP::mm);
  /** D0 significance loose veto cut */
  declareProperty("maxD0SignificanceLoose",  m_maxD0signLoose = 5 );
  /** D0 significance tight veto cut */
  declareProperty("maxD0SignificanceTight",  m_maxD0signTight = 3.5 );
  /**Muon Calibration Tool*/
  declareProperty("MuonCalibrationTool",     m_muon_calibration_tool );
  /**Muon Selection Tool*/
  declareProperty("MuonSelectionTool",       m_muon_selection_tool );
  /** Muon Isolation Tool names */
  declareProperty("LooseTrackOnlyIsolation", m_loose_trackonly_isolation);
  declareProperty("LooseIsolation",          m_loose_isolation);
  declareProperty("TightIsolation",          m_tight_isolation);
  declareProperty("GradientLooseIsolation",  m_gradient_loose_isolation);
  declareProperty("GradientIsolation",       m_gradient_isolation);
  declareProperty("FixedCutTightTrackOnlyIsolation", m_fixedcut_tight_trackonly_isolation);
  declareProperty("FixedCutLooseIsolation",          m_fixedcut_loose_isolation);
  declareProperty("DoIsolation", m_doIso=true);

  declareInterface<MuonTagTool>( this );
}

/** initialization - called once at the begginning */
StatusCode  MuonTagTool::initialize() {
  
  AthAlgTool::initialize().ignore();
  /** retrieve and check the muon calibration tool*/
  ATH_CHECK ( m_muon_calibration_tool.retrieve() );
  /** retrieve and check the muon selector tool*/
  ATH_CHECK ( m_muon_selection_tool.retrieve() );
  /** retrieve and check the muon isolation tool*/
  if(m_doIso){
    CHECK(m_loose_trackonly_isolation.retrieve());
    CHECK(m_loose_isolation.retrieve());
    CHECK(m_tight_isolation.retrieve());
    CHECK(m_gradient_loose_isolation.retrieve());
    CHECK(m_gradient_isolation.retrieve());
    CHECK(m_fixedcut_tight_trackonly_isolation.retrieve());
    CHECK(m_fixedcut_loose_isolation.retrieve());
    
    if (m_etconeisorelcutvalues.size() > 2) {
      ATH_MSG_FATAL ("More than to EtconeRel values are not permitted");
      return StatusCode::FAILURE;
    }
    if (m_etconeisocutvalues.size() > 2) {
      ATH_MSG_FATAL ("More than to Etcone values are not permitted");
      return StatusCode::FAILURE; 
    }
    if (m_ptconeisorelcutvalues.size() > 2) {
      ATH_MSG_FATAL ("More than to PtconeRel values are not permitted");
      return StatusCode::FAILURE;
    }
    if (m_ptconeisocutvalues.size() > 2) {
      ATH_MSG_FATAL ("More than to Ptcone values are not permitted");
      return StatusCode::FAILURE;
    }
  }
  else {
    m_loose_trackonly_isolation.disable();
    m_loose_isolation.disable();
    m_tight_isolation.disable();
    m_gradient_loose_isolation.disable();
    m_gradient_isolation.disable();
    m_fixedcut_tight_trackonly_isolation.disable();
    m_fixedcut_loose_isolation.disable();
  }
  return StatusCode::SUCCESS;
}


/** build the attribute list - called in initialize */
StatusCode MuonTagTool::attributeSpecification(std::map<std::string, AthenaAttributeType>& attrMap, const int max) {
  
  
  /** Muon Attributes */
  attrMap[ MuonAttributeNames[MuonTAG::NMuon] ]  = AthenaAttributeType("unsigned int",
                                                                       MuonAttributeUnitNames[MuonTAG::NMuon],
                                                                       MuonAttributeGroupNames[MuonTAG::NMuon]);
  
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
  
  std::vector<const xAOD::Muon*> unique_muons; 
  
  /** now loop over the Muon containers */
  
  for ( unsigned int j=0; j<m_containerNames.size(); ++j ) {
    
    const xAOD::MuonContainer *muonContainer=0;
    StatusCode sc = evtStore()->retrieve( muonContainer, m_containerNames[j] );
    if (sc.isFailure()) {
      ATH_MSG_DEBUG ("Muon container not found in SG: " << m_containerNames[j]);
      continue;
    }
    
    // create a shallow copy of the muon container
    std::pair< xAOD::MuonContainer*, xAOD::ShallowAuxContainer* >  shallowCopy = xAOD::shallowCopyContainer(*muonContainer);
    xAOD::MuonContainer       *muonContainerShallowCopy    = shallowCopy.first;
    xAOD::ShallowAuxContainer *muonAuxContainerShallowCopy = shallowCopy.second;
    
    CHECK( evtStore()->record(muonContainerShallowCopy,    "MuonShallowTAG"));
    CHECK( evtStore()->record(muonAuxContainerShallowCopy, "MuonShallowTAGAux."));

    static SG::AuxElement::Accessor< xAOD::IParticleLink > accSetOriginLink ("originalObjectLink");
    for ( xAOD::Muon *shallowCopyMuon : * muonContainerShallowCopy ) {

      /** fix calibration using tool */
      ATH_MSG_DEBUG("Un-Calibrated pt = " << shallowCopyMuon->pt());
      if(m_muon_calibration_tool->applyCorrection(*shallowCopyMuon) != CP::CorrectionCode::Ok){
	ATH_MSG_WARNING("Cannot calibrate muon");
      }
      ATH_MSG_DEBUG("Calibrated pt = " << shallowCopyMuon->pt()); 

      const xAOD::IParticleLink originLink( *muonContainer, shallowCopyMuon->index() );
      accSetOriginLink(*shallowCopyMuon) = originLink;
    }
    CHECK(evtStore()->setConst(muonContainerShallowCopy ));
    CHECK(evtStore()->setConst(muonAuxContainerShallowCopy ));

    /** create a new copy for MET calculation */
    ConstDataVector< xAOD::MuonContainer >* selectedMuons = new ConstDataVector< xAOD::MuonContainer >( SG::VIEW_ELEMENTS );
    ATH_CHECK( evtStore()->record( selectedMuons, m_muon_met_container_name ) );

    /** loop over the muon container */
    xAOD::MuonContainer::const_iterator cm_it     = muonContainerShallowCopy->begin();
    xAOD::MuonContainer::const_iterator cm_it_end = muonContainerShallowCopy->end();
    for ( ; cm_it != cm_it_end ; ++cm_it) {
      
      ATH_MSG_DEBUG ("next Muon has pt = " <<  (*cm_it)->pt()/1000. << " Gev, eta = " <<  (*cm_it)->eta() << ", phi = " <<  (*cm_it)->phi());

      /** now preselection of good muon candidate for tags */

      /** use the muon selector tool to get the tightness */
      xAOD::Muon::Quality my_quality = m_muon_selection_tool->getQuality(**cm_it);
      /** require that the muon is combined */
      bool muonType                  = (*cm_it)->muonType();

      /** we require at least Loose, not Standalone, passes Pt cut */
      if ( my_quality <= xAOD::Muon::Loose && muonType != xAOD::Muon::MuonStandAlone &&
	   (*cm_it)->pt() > m_cut_Pt ) {

        ATH_MSG_DEBUG ("-> Muon passes preselection");
      
        /** not first container, do overlap removal (obsolete) */
        if ( j!=0 ) {

          std::vector<const xAOD::Muon*>::const_iterator cm2_it = unique_muons.begin();
          for ( ; cm2_it != unique_muons.end() ; ++cm2_it) {
	    
            /** now we apply a deltaR check*/
            double dR= (*cm_it)->p4().DeltaR((*cm2_it)->p4());
            if (dR<m_DRcut) {
              ATH_MSG_DEBUG ("-> Muon fails dR overlap cut, dR = " << dR);
              continue;
            }
	    
          }
          ATH_MSG_DEBUG ("-> Muon passes overlap check !");
        }
        
        /** we apply impact preselection if no cosmics and no standalone muon */
        if (!m_cosmics) {
	  ATH_MSG_DEBUG ("-> try impact veto against cosmics");
	  
	  double d0=0,z0=0, d0_significance=0;
          getMuonImpactParameter (*cm_it, d0, z0, d0_significance);
          
          if ( std::abs(d0) > m_maxD0preselection ) {
            ATH_MSG_DEBUG ("-> Muon fails impact veto, ha d0 = " << d0 << " mm, for cosmics, reject it");
            continue;
          } else {
            ATH_MSG_DEBUG ("-> Muon passes impact veto, has d0 = " << d0 << " mm");         
          }
        }
	
        ATH_MSG_DEBUG ("-> Muon passed, copy Muon for output !");
	/** Fill the Muon into output list for TAG file */
        unique_muons.push_back( *cm_it );
	/** Fill the Muon MET input container only if the muons are medium */
	selectedMuons->push_back( *cm_it );
      }
    }
  }
  
  /** in TAG files we save muons momentum sorted */
  if ( unique_muons.size() > 1) {
    ATH_MSG_DEBUG ("sorting muon container");
    AnalysisUtils::Sort::pT( &unique_muons );
  }
  
  /** now fill muon TAG info */
  int i=0;
  std::vector<const xAOD::Muon*>::const_iterator muonItr  = unique_muons.begin();
  for (; muonItr != unique_muons.end() && i < max; ++muonItr, ++i) {
    
    const xAOD::Muon& muon = **muonItr;
    
    ATH_MSG_DEBUG("Muon: pt " << muon.pt()
                  << " eta " << muon.eta() << " phi " << muon.phi() 
                  << " px " << muon.p4().Px() << " py " << muon.p4().Py() );
    
    /** pt */
    muonTagCol.insert( m_ptStr[i], muon.pt() * muon.charge() );
    
    /** eta */
    muonTagCol.insert( m_etaStr[i], muon.eta() );
    
    /** phi */
    muonTagCol.insert( m_phiStr[i], muon.phi() );
    
    unsigned int iso = 0x0;
    unsigned int tightness = 0x0; 

    /** now start filling the isolation information */
    if(m_doIso){
      /** let's compute the etcone20 isolation of the muon */

      float etcone     = 0.0;
      if( ! muon.isolation(etcone,xAOD::Iso::etcone20) ){
	ATH_MSG_ERROR("No etcone defined");
      }
      else{
      
	/** apply etcone/pt cuts first */
	
	float etcone_rel= etcone/muon.pt();
	ATH_MSG_DEBUG("etcone20/pt = "<< etcone_rel);
      
	/* Etcone20/pt are bits 0 and 1 */
      
	for (unsigned int j=0; j<m_etconeisorelcutvalues.size(); j++) {
	  if( etcone_rel < m_etconeisorelcutvalues[j] ) {
	    ATH_MSG_DEBUG("Setting Etcone20/pt isolation: bit " << j );
	    iso |= 1 << j;
	  }
	}
      
	/* Etcone20 are bits 8 and 9 */
      
	for (unsigned int j=0; j<m_etconeisocutvalues.size(); j++) {
	  if ( etcone < m_etconeisocutvalues[j] ) {
	    ATH_MSG_DEBUG("Setting Etcone20 isolation: bit " << j+8 );
	    iso |= 1 << (8+j);
	  }
	} 
      }
    
      /** let's compute the etcone20 isolation of the muon */
    
      if( ! muon.isolation(etcone,xAOD::Iso::topoetcone20) ){
	ATH_MSG_ERROR("No topoetcone20 defined");
      }
      else{
      
	/** apply topoEtcone20/pt cuts first */
      
	float etcone_rel= etcone/muon.pt();
	ATH_MSG_DEBUG("topoetcone20/pt = "<< etcone_rel);
      
	/* topoEtcone20/pt are bits 2 and 3 */
      
	for (unsigned int j=0; j<m_etconeisorelcutvalues.size(); j++) {
	  if( etcone_rel < m_etconeisorelcutvalues[j] ) {
	    ATH_MSG_DEBUG("Setting TopoEtcone20/pt isolation: bit " << j );
	    iso |= 1 << (2 + j);
	  }
	}
      
	/* topo20Etcone/pt are bits 10 and 11 */
      
	for (unsigned int j=0; j<m_etconeisocutvalues.size(); j++) {
	  if ( etcone < m_etconeisocutvalues[j] ) {
	    ATH_MSG_DEBUG("Setting TopoEtcone20: bit " << j+10 );
	    iso |= 1 << (10+j);
	  }
	} 
      }
    
      /** let's compute the etcone30 isolation of the muon */
    
      if( ! muon.isolation(etcone,xAOD::Iso::topoetcone30) ){
	ATH_MSG_ERROR("No topoetcone30 defined");
      }
      else{
      
	/** apply topoEtcone30/pt cuts first */
      
	float etcone_rel= etcone/muon.pt();
	ATH_MSG_DEBUG("topoetcone30/pt = "<< etcone_rel);
      
	/* topoEtcone30/pt are bits 4 and 5 */
      
	for (unsigned int j=0; j<m_etconeisorelcutvalues.size(); j++) {
	  if( etcone_rel < m_etconeisorelcutvalues[j] ) {
	    ATH_MSG_DEBUG("Setting topoEtcone30/pt isolation: bit " << j+4 );
	    iso |= 1 << (4 + j);
	  }
	}
      
	/* topo30Etcone are bits 12 and 13 */
      
	for (unsigned int j=0; j<m_etconeisocutvalues.size(); j++) {
	  if ( etcone < m_etconeisocutvalues[j] ) {
	    ATH_MSG_DEBUG("Setting topoEtcone30 isolation: bit " << j+12 );
	    iso |= 1 << (12+j);
	  }
	} 
      }
    
      /** let's compute the etcone40 isolation of the muon */
    
      if( ! muon.isolation(etcone,xAOD::Iso::topoetcone40) ){
	ATH_MSG_ERROR("No topoetcone40 defined");
      }
      else{
      
	/** apply topoEtcone40/pt cuts first */
      
	float etcone_rel= etcone/muon.pt();
	ATH_MSG_DEBUG("topoetcone40/pt = "<< etcone_rel);
      
	/* topo40Etcone/pt are bits 6 and 7 */
      
	for (unsigned int j=0; j<m_etconeisorelcutvalues.size(); j++) {
	  if( etcone_rel < m_etconeisorelcutvalues[j] ) {
	    ATH_MSG_DEBUG("Setting topoEtcone40/pt isolation: bit " << j+6 );
	    iso |= 1 << (6 + j);
	  }
	}
      
	/* topo40Etcone are bits 14 and 15 */
      
	for (unsigned int j=0; j<m_etconeisocutvalues.size(); j++) {
	  if ( etcone < m_etconeisocutvalues[j] ) {
	    ATH_MSG_DEBUG("Setting topoEtcone40 isolation: bit " << j+14 );
	    iso |= 1 << (14+j);
	  }
	} 
      }
    
      /** let's compute the ptcone20 isolation of the muon */
    
      float ptcone = 0.0 ;
      if( ! muon.isolation(ptcone,xAOD::Iso::ptcone20) ){
	ATH_MSG_ERROR("No ptcone20 defined");
      }
      else{
      
	/** apply ptcone20/pt cuts first */
      
	float ptcone_rel= ptcone/muon.pt();
	ATH_MSG_DEBUG("ptcone20/pt = "<< ptcone_rel);
      
	/* ptcone20/pt are bits 16 and 17 */
      
	for (unsigned int j=0; j<m_ptconeisorelcutvalues.size(); j++) {
	  if ( ptcone_rel < m_ptconeisorelcutvalues[j] ){
	    ATH_MSG_DEBUG("Setting ptcone20/pt isolation: bit " << j+16 );
	    iso |= 1 << (16+j);
	  }
	} 
      
	/* ptcone20 are bits 24 and 25 */
      
	ATH_MSG_DEBUG("ptcone20 = "<< ptcone);
      
	for (unsigned int j=0; j<m_ptconeisocutvalues.size(); j++) {
	  if ( ptcone < m_ptconeisocutvalues[j] ) {
	    ATH_MSG_DEBUG("Setting ptcone20 isolation: bit " << j+24 );
	    iso |= 1 << (24+j);
	  }     
	}
      }
    
      /** let's compute the ptcone30 isolation of the muon */
    
      if( ! muon.isolation(ptcone,xAOD::Iso::ptcone30) ){
	ATH_MSG_ERROR("No ptcone30 defined");
      }
      else{
      
	/** apply ptcone30/pt cuts first */
      
	float ptcone_rel= ptcone/muon.pt();
	ATH_MSG_DEBUG("ptcone30/pt = "<< ptcone_rel);
      
	/* ptcone30 are bits 18 and 19 */
      
	for (unsigned int j=0; j<m_ptconeisorelcutvalues.size(); j++) {
	  if ( ptcone_rel < m_ptconeisorelcutvalues[j] ){
	    ATH_MSG_DEBUG("Setting ptcone30/pt isolation: bit " << j+18 );
	    iso |= 1 << (18+j);
	  }
	} 
      
	/* ptcone30 are bits 26 and 27 */      
      
	ATH_MSG_DEBUG("ptcone30 = "<< ptcone);
      
	for (unsigned int j=0; j<m_ptconeisocutvalues.size(); j++) {
	  if ( ptcone < m_ptconeisocutvalues[j] ) {
	    ATH_MSG_DEBUG("Setting ptcone30 solation: bit " << j+26 );
	    iso |= 1 << (26+j);
	  }     
	}
      }
    
      /** let's compute the ptcone20 isolation of the muon */
    
      if( ! muon.isolation(ptcone,xAOD::Iso::ptcone40) ){
	ATH_MSG_ERROR("No ptcone40 defined");
      }
      else{
      
	/** apply ptcone40/pt cuts first */
      
	float ptcone_rel= ptcone/muon.pt();
	ATH_MSG_DEBUG("ptcone40/pt = "<< ptcone_rel);
      
	/* ptcone40/pt are bits 20 and 21 */
      
	for (unsigned int j=0; j<m_ptconeisorelcutvalues.size(); j++) {
	  if ( ptcone_rel < m_ptconeisorelcutvalues[j] ){
	    ATH_MSG_DEBUG("Setting ptcone40/pt isolation: bit " << j+20 );
	    iso |= 1 << (20+j);
	  }
	} 
      
	/* ptcone40 are bits 28 and 29 */
      
	ATH_MSG_DEBUG("ptcone40 = "<< ptcone);
      
	for (unsigned int j=0; j<m_ptconeisocutvalues.size(); j++) {
	  if ( ptcone < m_ptconeisocutvalues[j] ) {
	    ATH_MSG_DEBUG("Setting ptcone40 isolation: bit " << j+28 );
	    iso |= 1 << (28+j);
	  }     
	}
      }

      if( msgLvl(MSG::DEBUG) ){
	std::bitset<32> bits(iso);
	ATH_MSG_DEBUG("Isolation: " << iso << " bits: " << bits.to_string() );
      }
    
      muonTagCol.insert( m_isoStr[i], iso );
    

      /**  Using Isolation Tool to fill bit from 22,23,30,31  with loose_trackonly,loose,tight,gradientloose,gradient*/    
      if(m_loose_trackonly_isolation->accept(**muonItr))tightness |= (1 << 24);
      if(m_loose_isolation->accept(**muonItr))          tightness |= (1 << 25);
      if(m_tight_isolation->accept(**muonItr))          tightness |= (1 << 26);
      if(m_gradient_isolation->accept(**muonItr))       tightness |= (1 << 27);
      if(m_gradient_loose_isolation->accept(**muonItr)) tightness |= (1 << 28);
      if(m_fixedcut_tight_trackonly_isolation->accept(**muonItr))tightness |= (1 << 29);
      if(m_fixedcut_loose_isolation->accept(**muonItr))          tightness |= (1 << 30);
    }
    /** varying levels of tighness cuts - to be defined and implemented */
    
    if ( muon.muonType() == xAOD::Muon::MuonStandAlone )               tightness = tightness | bit2int(0);
    if ( muon.muonType() == xAOD::Muon::Combined )                     tightness = tightness | bit2int(1);
    if ( muon.muonType() == xAOD::Muon::SiliconAssociatedForwardMuon ) tightness = tightness | bit2int(2);
    if ( muon.muonType() == xAOD::Muon::SegmentTagged )                tightness = tightness | bit2int(3);
    
    xAOD::Muon::Quality my_quality = m_muon_selection_tool->getQuality(muon);
    if ( my_quality <= xAOD::Muon::Loose)                              tightness = tightness | bit2int(7);
    if ( my_quality <= xAOD::Muon::Medium)                             tightness = tightness | bit2int(4);
    if ( my_quality <= xAOD::Muon::Tight)                              tightness = tightness | bit2int(8);
    
    /** ID track quality cuts */
   
    const ElementLink<xAOD::TrackParticleContainer> &  tp_p = muon.primaryTrackParticleLink();
    if(!tp_p){
      ATH_MSG_DEBUG("no link to primary track particle");
    } else {
      ATH_MSG_DEBUG("found link to primary track particle");

      if( tp_p.isValid() ) {
        ATH_MSG_DEBUG("elementLink<TrackParticle> is valid");
        const xAOD::TrackParticle* tp = *tp_p;

        /** Add the impact parameter bits for cosmic veto */
        
        double d0=0,z0=0,d0sig=0;
        /**calling getMuonImpactParameter to get d0,z0 values*/
        getMuonImpactParameter ((*muonItr), d0, z0, d0sig);

        if ( d0sig < m_maxD0signLoose) tightness = tightness | bit2int(5);
        if ( d0sig < m_maxD0signTight) tightness = tightness | bit2int(6);
        if ( fabs(d0) < m_maxD0tight)  tightness = tightness | bit2int(9);
        if ( fabs(z0) < m_maxZ0tight)  tightness = tightness | bit2int(10);
        if ( fabs(z0) < m_maxZ0loose)  tightness = tightness | bit2int(20);
       
        // check blayer, if expected
        uint8_t nblh   = 0x0;
        uint8_t eblh   = 0x0;
        uint8_t nblo   = 0x0;
        if( !tp->summaryValue(nblh,xAOD::numberOfInnermostPixelLayerHits)){
          ATH_MSG_WARNING("No nBLayerHits");
        }
        if( !tp->summaryValue(nblo,xAOD::numberOfInnermostPixelLayerOutliers)){
          ATH_MSG_WARNING("No nofBLayerOutliers");
        }
        if( !tp->summaryValue(eblh,xAOD::expectInnermostPixelLayerHit) ||
            (nblh + nblo > 0) )        tightness = tightness | bit2int(11);
      
        // pixel hit counts
        uint8_t nphi   = 0x0;
        uint8_t npds   = 0x0;
        if( !tp->summaryValue(nphi,xAOD::numberOfPixelHits)){
          ATH_MSG_WARNING("No numberOfPixelHits");
        }
        if( !tp->summaryValue(npds,xAOD::numberOfPixelDeadSensors)){
          ATH_MSG_WARNING("No numberOfPixelDeadSensors");
        }
        if( nphi + npds >= 2 )         tightness = tightness | bit2int(12);
        
        // sct hit counts
        uint8_t nscthi = 0x0;
        uint8_t nsctds = 0x0;
        if( !tp->summaryValue(nscthi,xAOD::numberOfSCTHits)){
          ATH_MSG_WARNING("No numberOfSCTHits");
        }
        if( !tp->summaryValue(nsctds,xAOD::numberOfSCTDeadSensors)){
          ATH_MSG_WARNING("No numberOfSCTDeadSensors");
        }
        if( nscthi + nsctds > 5 )      tightness = tightness | bit2int(13);
      
        // hole cuts
        uint8_t npho   = 0x0;
        uint8_t nsctho = 0x0;
        if( !tp->summaryValue(npho,xAOD::numberOfPixelHoles)){
          ATH_MSG_WARNING("No numberOfPixelHoles");
        }
        if( !tp->summaryValue(nsctho,xAOD::numberOfSCTHoles)){
          ATH_MSG_WARNING("No numberOfSCTHoles");
        }
        if( npho + nsctho < 2 )         tightness = tightness | bit2int(14);
        
        // trt cuts
        uint8_t ntrthi = 0x0;
        uint8_t ntrtol = 0x0;
        if( !tp->summaryValue(ntrthi,xAOD::numberOfTRTHits)){
          ATH_MSG_WARNING("No numberOfTRTHits");
        }
        if( !tp->summaryValue(ntrtol,xAOD::numberOfTRTOutliers)){
          ATH_MSG_WARNING("No numberOfTRTHits");
        }
        int ntrt = ntrthi+ ntrtol;
        bool pass = true;
        if ( ntrt > 5 && ntrtol/(double)ntrt > 0.9 ) pass = false;                             
        if ( (fabs(tp->eta()) > 0.1 && fabs(tp->eta()) < 1.9 ) && ntrt <= 5 ) pass = false;                          
        if ( pass ) tightness = tightness | bit2int(15);
        
        /** get muon layer information */
        
        uint8_t nprecisionLayers=0;
        if(!tp->summaryValue(nprecisionLayers,xAOD::numberOfPrecisionLayers)){
          ATH_MSG_ERROR("number of precision layers not retrieved");
        }
        muonTagCol.insert( m_nprecLayStr[i],     nprecisionLayers );      
      
        uint8_t nprecisionHoleLayers=0;
        if(!tp->summaryValue(nprecisionHoleLayers,xAOD::numberOfPrecisionHoleLayers)){
          ATH_MSG_ERROR("number of precision hole layers not retrieved");
        }
        muonTagCol.insert( m_nprecHoleLayStr[i], nprecisionHoleLayers );       
        
        uint8_t nphiLayers=0;      
        if(!tp->summaryValue(nphiLayers,xAOD::numberOfPhiLayers)){
          ATH_MSG_ERROR("number of phi layers not retrieved");
        }
        muonTagCol.insert( m_nphiLayStr[i],      nphiLayers );
        
        /** get momentumBalanceSignificance */
        
      }
    }
    
    /** check ID cuts and HightPtCuts using the muon selector tool */
    
    if( m_muon_selection_tool->passedIDCuts(muon) == 1 )        tightness = tightness | bit2int(16);
    if( m_muon_selection_tool->passedHighPtCuts(muon) == 1  )   tightness = tightness | bit2int(21);
      
    /** segment information */
    
    float segChi2OverDoF = 0.0;
    if(! muon.parameter(segChi2OverDoF, xAOD::Muon::segmentChi2OverDoF)){
      ATH_MSG_ERROR("segmentChi2OverDoF not retrieved!");
    }
    if( segChi2OverDoF < 5 ) tightness = tightness | bit2int(22);
    if( segChi2OverDoF < 3 ) tightness = tightness | bit2int(23);

    if( msgLvl(MSG::DEBUG) ){
      std::bitset<32> bits(tightness);
      ATH_MSG_VERBOSE("Tightness: " << tightness << " bits: " << bits.to_string() );
    }
    
    /** now insert tightness */
    
    muonTagCol.insert( m_tightStr[i], tightness );       

    /** insert momentum balance */
        
    float momentumBal = 0.0;
    if(! muon.parameter(momentumBal, xAOD::Muon::momentumBalanceSignificance)){
      ATH_MSG_WARNING("momentumBalanceSignificance not retrieved!");
    }      
    muonTagCol.insert( m_momBalStr[i], momentumBal );
  }

  /** insert the number of loose muons */
  muonTagCol.insert(MuonAttributeNames[MuonTAG::NMuon], i);

  return StatusCode::SUCCESS;
}

/** finalize - called once at the end */
StatusCode MuonTagTool::finalize() {
  ATH_MSG_DEBUG ("in finalize()");
  return StatusCode::SUCCESS;
}

/** private function to get impact parameter */
void MuonTagTool::getMuonImpactParameter (const xAOD::Muon* muon, double& d0, double& z0, double& d0_significance) {
   
  /** let's initialize to 0 the impact parameters*/
  d0    = 0;
  z0    = 0;
  d0_significance = 0;

  StatusCode sc;
  
  /** retrieve the  EventInfo container*/
  const xAOD::EventInfo* eventInfo = 0;
  sc = evtStore()->retrieve( eventInfo, "EventInfo");
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "No AOD EventInfo container found in SG" );
    return;
  }

  /** get vertex container, we assume the first vertex is the primary by convention */
  const xAOD::VertexContainer* vxContainer=0;
  sc = evtStore()->retrieve(vxContainer, m_vxCandidate);
  if (sc.isFailure() || !vxContainer) {
    ATH_MSG_DEBUG ("Could not retrieve primary vertex info: " << m_vxCandidate <<", return 0 impact parameter.");
    return;
  }
  if (vxContainer->size()<1) {
    ATH_MSG_DEBUG ("No primary vertices reconstructed, return 0 impact parameter.");
    return;
  } 
  xAOD::VertexContainer::const_iterator vxI     = vxContainer->begin();
  if ((*vxI)->vertexType() != xAOD::VxType::PriVtx) {
    ATH_MSG_DEBUG ("---> no primary vertex reconstructed, return 0 impact parameters.");
    return;
  }
  ATH_MSG_DEBUG ("---> vertex at (x/y/z) = " << (*vxI)->x() << " / " << (*vxI)->y() << " / " << (*vxI)->z() );
  
  /** We try to find the link to the primary track and after we retrieve it*/
  const ElementLink<xAOD::TrackParticleContainer> &  tp_prime = muon->primaryTrackParticleLink();
  if(!tp_prime){
    ATH_MSG_DEBUG("found no link to primary track particle");
  } else {
    const xAOD::TrackParticle* trk = *tp_prime;
  
    /** d0 Significance w.r.t. Beam Spot covariance as recommended (even if it makes little sense) */
    double d0_significance = xAOD::TrackingHelpers::d0significance(trk,
								   eventInfo->beamPosSigmaX(),
								   eventInfo->beamPosSigmaY(),
								   eventInfo->beamPosSigmaXY() );
    /** get d0 impact parameter */
    d0    = trk->d0();
    /** z0 is an approximation, just do the diff */
    double z0 = fabs(trk->z0() + trk->vz() - (*vxI)->z());

    ATH_MSG_DEBUG("d0 = " <<  d0 << " z0 = " <<z0 << " d0_significance = " << d0_significance);
  }
  return;
}


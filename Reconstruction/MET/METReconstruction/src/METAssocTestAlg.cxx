/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "METAssocTestAlg.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "xAODMissingET/MissingETAssociationHelper.h"

using namespace xAOD;

// allow for differences due to float compression
static const float f_tolerance = 1;

namespace met {

  METAssocTestAlg::METAssocTestAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    ::AthAlgorithm( name, pSvcLocator ) {
    declareProperty(  "METMapSuffix",  m_mapsuffix="AntiKt4EMTopo"  );
    declareProperty(  "JetCollection", m_jetname=""                 );

    declareProperty(  "FailOnInconsistency", m_failOnInconsistency=false  );
  }

  StatusCode METAssocTestAlg::initialize()
  {
    if(m_mapsuffix.empty()) {
      ATH_MSG_FATAL( "METMapSuffix (e.g. AntiKt4EMTopo) must be set!" );
      return StatusCode::FAILURE;
    }
    m_mapname = "METAssoc_"+m_mapsuffix;
    if(m_jetname.empty()) { m_jetname = m_mapsuffix+"Jets"; }

    ATH_MSG_INFO( "Initialising " << name() );
    ATH_MSG_INFO( "MET map name:        " << m_mapname );

    return StatusCode::SUCCESS;
  }

  StatusCode METAssocTestAlg::finalize() { return StatusCode::SUCCESS; }

  StatusCode METAssocTestAlg::execute()
  {


    SG::ReadHandle<MissingETAssociationMap> map(m_mapname);
    if (!map.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve met association map");
      return StatusCode::FAILURE;
    } 

    for(const auto assoc : *map) {
      if(assoc->isMisc()) { // misc association gets special treatment
	ATH_MSG_VERBOSE( "  Now on MET misc association" );
	ATH_CHECK( checkMiscAssoc(*assoc) );
      } else {
	ATH_MSG_VERBOSE( "  Now on MET association for jet " << assoc->refJet()->index() );
	// Test jets for consistency with the map
	ATH_CHECK( checkJet(*assoc) );
	// Test associated objects for consistency with the map
	ATH_CHECK( checkObjects(*assoc) );
	// Test association for self-consistency based on stored keys
	ATH_CHECK( checkAssoc(*assoc) );
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode METAssocTestAlg::checkJet(const xAOD::MissingETAssociation& assoc)
  {
    const auto& jet = *assoc.refJet();
    JetFourMom_t jetconstp4 = jet.jetP4(JetConstitScaleMomentum);
    MissingETBase::Types::constvec_t jettrkvec = assoc.jetTrkVec();

    xAOD::MissingETAssociationHelper helper;

    // switch on all overlapping objects
    for(size_t iobj=0; iobj<assoc.objects().size(); ++iobj) {
      helper.setObjSelectionFlag(&assoc,iobj,true);
    }
    // get cluster/inclusive pflow and track/charged pflow constituent sums
    MissingETBase::Types::constvec_t overlapcalvec = assoc.overlapCalVec(helper);
    MissingETBase::Types::constvec_t overlaptrkvec = assoc.overlapTrkVec(helper);

    ATH_MSG_VERBOSE("  Jet constituent-scale pt: " << jetconstp4.pt()     << ", E: " << jetconstp4.e() );
    ATH_MSG_VERBOSE("  Max constituent overlap pt:   " << overlapcalvec.cpt() << ", E: " << overlapcalvec.ce() );

    ATH_MSG_VERBOSE("  Jet track pt:     " << jettrkvec.cpt()     << ", E: " << jettrkvec.ce() );
    ATH_MSG_VERBOSE("  Max track overlap pt: " << overlaptrkvec.cpt() << ", E: " << overlaptrkvec.ce() );

    // optionally quit and fail if overlap-removal exceeds original jet constituent-scale energy
    if(overlapcalvec.ce()-jetconstp4.e()>f_tolerance) {
      ATH_MSG_ERROR("Too-large overlapcalvec!");
      if(m_failOnInconsistency) return StatusCode::FAILURE;
    }
    if(overlaptrkvec.ce()-jettrkvec.ce()>f_tolerance) {
      ATH_MSG_ERROR("Too-large overlaptrkvec!");
      if(m_failOnInconsistency) return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  StatusCode METAssocTestAlg::checkObjects(const xAOD::MissingETAssociation& assoc)
  {
    const auto& jet = *assoc.refJet();
    JetFourMom_t jetconstp4 = jet.jetP4(JetConstitScaleMomentum);
    MissingETBase::Types::constvec_t jettrkvec = assoc.jetTrkVec();

    xAOD::MissingETAssociationHelper helper;

    // loop over individual objects and check that none of them has a larger
    // associated constituent sum than the jet
    for(size_t iobj=0; iobj<assoc.objects().size(); ++iobj) {
      ATH_MSG_VERBOSE("    Now on constituent " << iobj );
      const IParticle* obj = assoc.objects()[iobj];
      if(!obj) continue; // invalid ElementLink -- thinned?
      helper.resetObjSelectionFlags();
      helper.setObjSelectionFlag(&assoc,iobj,true);
      MissingETBase::Types::constvec_t overlapcalvec = assoc.overlapCalVec(helper);
      MissingETBase::Types::constvec_t overlaptrkvec = assoc.overlapTrkVec(helper);

      ATH_MSG_VERBOSE("    Object pt:       " << obj->pt() << ", E: " << obj->e() << ", type: " << obj->type());
      ATH_MSG_VERBOSE("    Constituent pt:  " << overlapcalvec.cpt() << ", E: " << overlapcalvec.ce() );
      ATH_MSG_VERBOSE("    Track pt:        " << overlaptrkvec.cpt() << ", E: " << overlaptrkvec.ce() );

      // optionally quit and fail if overlap-removal exceeds original jet constituent-scale energy
      if(overlapcalvec.ce()-jetconstp4.e()>f_tolerance) {
	ATH_MSG_ERROR("Too-large overlapcalvec!");
	ATH_MSG_ERROR("    Object pt:       " << obj->pt() << ", E: " << obj->e() << ", type: " << obj->type());
	if(m_failOnInconsistency) return StatusCode::FAILURE;
      }
      if(overlaptrkvec.ce()-jettrkvec.ce()>f_tolerance) {
	ATH_MSG_ERROR("Too-large overlaptrkvec!");
	ATH_MSG_ERROR("    Object pt:       " << obj->pt() << ", E: " << obj->e() << ", type: " << obj->type());
	if(m_failOnInconsistency) return StatusCode::FAILURE;
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode METAssocTestAlg::checkAssoc(const xAOD::MissingETAssociation& assoc)
  {
    const auto& jet = *assoc.refJet();
    JetFourMom_t jetconstp4 = jet.jetP4(JetConstitScaleMomentum);
    MissingETBase::Types::constvec_t jettrkvec = assoc.jetTrkVec();

    // loop over the encoded cluster/inclusive pflow constituent sums and check that these are
    // all smaller than the jet constituent-scale pt
    for(size_t ikey=0; ikey<assoc.calkey().size(); ++ikey) {
      ATH_MSG_VERBOSE("    Now on calkey " << ikey << ": " << assoc.calkey()[ikey] );
      MissingETBase::Types::constvec_t calvec = assoc.calVec(ikey);
      ATH_MSG_VERBOSE("    Constituent pt:  " << calvec.cpt() << ", E: " << calvec.ce() );

      if(calvec.ce()-jetconstp4.e()>f_tolerance) {
	ATH_MSG_ERROR("Too-large calvec!");
	for(size_t iobj=0; iobj<assoc.objects().size(); ++iobj) {
	  if( ((1<<iobj)&assoc.calkey()[ikey]) == 0 ) {
	    const IParticle* obj = assoc.objects()[iobj];
	    if(!obj) continue; // invalid ElementLink -- thinned?
	    ATH_MSG_ERROR("    Object pt:       " << obj->pt() << ", E: " << obj->e() << ", type: " << obj->type());
	  }
	}
	if(m_failOnInconsistency) return StatusCode::FAILURE;
      }
    }

    // loop over the encoded track/charged pflow constituent sums and check that these are
    // all smaller than the jet track sum
    for(size_t ikey=0; ikey<assoc.trkkey().size(); ++ikey) {
      ATH_MSG_VERBOSE("    Now on trkkey " << ikey << ": " << assoc.trkkey()[ikey] );
      MissingETBase::Types::constvec_t trkvec = assoc.trkVec(ikey);
      ATH_MSG_VERBOSE("    Track pt:        " << trkvec.cpt() << ", E: " << trkvec.ce() );

      if(trkvec.ce()-jettrkvec.ce()>f_tolerance) {
	ATH_MSG_ERROR("Too-large trkvec!");
	for(size_t iobj=0; iobj<assoc.objects().size(); ++iobj) {
	  if( ((1<<iobj)&assoc.trkkey()[ikey]) == 0 ) {
	    const IParticle* obj = assoc.objects()[iobj];
	    if(!obj) continue; // invalid ElementLink -- thinned?
	    ATH_MSG_ERROR("    Object pt:       " << obj->pt() << ", E: " << obj->e() << ", type: " << obj->type());
	  }
	}
	if(m_failOnInconsistency) return StatusCode::FAILURE;
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode METAssocTestAlg::checkMiscAssoc(const xAOD::MissingETAssociation& assoc)
  {
    // loop over the encoded cluster/inclusive pflow constituent sums
    for(size_t ikey=0; ikey<assoc.calkey().size(); ++ikey) {
      ATH_MSG_VERBOSE("    Now on calkey " << ikey << ": " << assoc.calkey()[ikey] );
      MissingETBase::Types::constvec_t calvec = assoc.calVec(ikey);
      ATH_MSG_VERBOSE("    Constituent pt:  " << calvec.cpt() << ", E: " << calvec.ce() );
    }

    // loop over the encoded track/charged pflow constituent sums
    for(size_t ikey=0; ikey<assoc.trkkey().size(); ++ikey) {
      ATH_MSG_VERBOSE("    Now on trkkey " << ikey << ": " << assoc.trkkey()[ikey] );
      MissingETBase::Types::constvec_t trkvec = assoc.trkVec(ikey);
      ATH_MSG_VERBOSE("    Track pt:        " << trkvec.cpt() << ", E: " << trkvec.ce() );
    }

    return StatusCode::SUCCESS;
  }
}

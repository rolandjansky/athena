/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackSystematicsTools/InDetTrackTruthOriginTool.h"
#include "InDetTrackSystematicsTools/InDetTrackTruthOriginDefs.h"

#include "xAODTruth/TruthParticleContainer.h"

#include <math.h>

namespace InDet {

  InDetTrackTruthOriginTool::InDetTrackTruthOriginTool(const std::string& name) :
    asg::AsgTool(name)
  {

#ifndef XAOD_STANDALONE
    declareInterface<IInDetTrackTruthOriginTool>(this);
#endif

    declareProperty("barcodeG4", m_barcodeG4);
    declareProperty("matchingProbabilityCut", m_matchingProbabilityCut);

    declareProperty("truthParticleLinkName", m_truthParticleLinkName);
    declareProperty("truthMatchProbabilityAuxName", m_truthMatchProbabilityAuxName);
  }

  InDetTrackTruthOriginTool::~InDetTrackTruthOriginTool() = default;

  StatusCode InDetTrackTruthOriginTool::initialize() {
    return StatusCode::SUCCESS;
  }

  int InDetTrackTruthOriginTool::getTrackOrigin(const xAOD::TrackParticle* track) const {

    const xAOD::TruthParticle* truth = nullptr;
    typedef ElementLink< xAOD::TruthParticleContainer > Link_t;
    if( track->isAvailable< Link_t >( m_truthParticleLinkName.data() ) ) {
      static SG::AuxElement::ConstAccessor< Link_t > linkAcc( m_truthParticleLinkName.data() );
      const Link_t& link = linkAcc( *track );
      if( link.isValid() ) {
	truth = *link;
      }
    }

    static SG::AuxElement::ConstAccessor< float > tmpAcc( m_truthMatchProbabilityAuxName.data() );
    float truthProb = tmpAcc( *track );

    int origin = 0;

    // not matched to truth: call it fake
    if(truthProb < m_matchingProbabilityCut) {
      origin = origin | (0x1 << InDet::TrkOrigin::Fake);
    }
    // matched to truth: find truth origin
    else{
      // matched but truth link is broken: call it from pileup (not true for < 100 MeV!)
      if (!truth){
	origin = origin | (0x1 << InDet::TrkOrigin::Pileup);
      }

      // matched and link is present: find trutn origin
      else{

	// is it fragmentation? yes until proven otherwise
	bool isFragmentation = true;

	// from B decay chain?
	if(isFrom(truth, 5)) {
	  origin = origin | (0x1 << InDet::TrkOrigin::BHadronDecay);
	  isFragmentation = false;
	}

	// from D decay chain?
	if(isFrom(truth, 4)) {
	  origin = origin | (0x1 << InDet::TrkOrigin::DHadronDecay);
	  isFragmentation = false;
	}

	// Secondary? check based on barcode: secondaries are produced by G4.
	int truthBarcode = truth->barcode();
	if (truthBarcode > m_barcodeG4) {
	  // sub-categorize secondaries...
	  int parentID = getParentID(truth);

	  // photon conversions
	  if(parentID == 22) {
	    origin = origin | (0x1 << InDet::TrkOrigin::GammaConversion);
	  }

	  // K-short
	  else if(parentID == 310) {
	    origin = origin | (0x1 << InDet::TrkOrigin::KshortDecay);
	  }

	  // Lambda
	  else if(abs(parentID) == 3122){
	    origin = origin | (0x1 << InDet::TrkOrigin::LambdaDecay);
	  }

	  // other long living particle decays
	  else if(abs(parentID) > 3) {
	    origin = origin | (0x1 << InDet::TrkOrigin::OtherDecay);
	  }

	  // hadronic interactions
	  else if(parentID == -1) {
	    origin = origin | (0x1 << InDet::TrkOrigin::HadronicInteraction);
	  }

	  // other secondaries? 
	  //  ---> Not sure what if anything should be here...
	  else if(parentID == -2) {
	    origin = origin | (0x1 << InDet::TrkOrigin::OtherSecondary);
	  }

	  // other unknown origin (e.g. parent not in the record:) 
	  //  ---> Not sure what if anything should be here...
	  else{
	    origin = origin | (0x1 << InDet::TrkOrigin::OtherOrigin);
	  }

	  isFragmentation = false;
	}

	// uncategorized: call it fragmentation
	if(isFragmentation) {
	  origin = origin | (0x1 << InDet::TrkOrigin::Fragmentation);
	}

      }
    }
  
    return origin;
  }

  bool InDetTrackTruthOriginTool::isFrom(const xAOD::TruthParticle* part, int flav) const {

    if ( part == nullptr ) return false;

    if( flav != 5 && flav != 4 ) return false;

    if( ! part->isHadron() ) return false;

    if( flav == 5 && part->isBottomHadron() ) return true;

    if( flav == 4 && part->isCharmHadron() ) return true;

    for(unsigned int p=0; p<part->nParents(); p++) {
      const xAOD::TruthParticle* parent = part->parent(p);
      if( isFrom(parent, flav) ) return true;
    }

    return false;
  }

  int InDetTrackTruthOriginTool::getParentID(const xAOD::TruthParticle* part) const {

    // no parents? is anything even there?
    if(part->nParents() != 1) return -3;

    int pdgId = part->pdgId();

    const xAOD::TruthParticle *parent = part->parent(0);

    // in some files, the particle has a parent but that parent's barcode is zero.
    // treat this as "other" (anything less than -2)
    if (parent == nullptr) return -4;

    int parentId = parent->pdgId();

    // photon conversions:
    if(parent->isPhoton() && part->isElectron()) return parentId;

    // Kshort:
    if(abs(pdgId) == 211 && parentId == 310 && parent->nChildren() == 2) return parentId;

    // Lambdas
    if((abs(pdgId) == 211 || abs(pdgId) == 2212) && abs(pdgId) == 3122 && parent->nChildren() == 2) return parentId;

    // other decays:
    if(parent->isHadron() && parent->nChildren() == 2) return parentId;

    // hadronic interactions
    if(parent->nChildren() > 2) return -1;

    // other secondaries: is anything even there??
    return -2;
  }
}

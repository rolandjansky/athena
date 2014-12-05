///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METMaker.cxx 
// Implementation file for class METMaker
// Author: T.J.Khoo<khoo@cern.ch>
/////////////////////////////////////////////////////////////////// 

// METUtilities includes
#include "METUtilities/METMaker.h"

// MET EDM
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAssociationMap.h"

// EDM includes
#include "xAODJet/JetAttributes.h"

// Tracking EDM
#include "xAODTracking/TrackParticle.h"

namespace met {

  using std::vector;

  using xAOD::MissingET;
  using xAOD::MissingETContainer;
  using xAOD::MissingETAssociation;
  using xAOD::MissingETAssociationMap;
  using xAOD::MissingETAuxContainer;
  using xAOD::MissingETComposition;
  //
  using xAOD::IParticle;
  using xAOD::IParticleContainer;
  //
  using xAOD::JetContainer;
  using xAOD::JetConstituentVector;
  //
  using xAOD::TrackParticle;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 

  // Constructors
  ////////////////
  METMaker::METMaker(const std::string& name) : 
    AsgTool(name),
    m_objLinkAcc("originalObjectLink")
  {
    //
    // Property declaration
    // 

    declareProperty("JetPtCut",         m_jetMinPt         = 20e3  );
    declareProperty("DoJetJvfCut",      m_doJetJvf         = true  );
    declareProperty("JetJvfCut",        m_jetMinAbsJvf     = 0.25  );
    declareProperty("JetMinEFrac",      m_jetMinEfrac      = 0.5   );
    declareProperty("JetMinWeightedPt", m_jetMinWeightedPt = 0.    );
    declareProperty("CorrectJetPhi",    m_jetCorrectPhi    = false );
    declareProperty("DoPFlow",          m_doPFlow          = false );
  }

  // Destructor
  ///////////////
  METMaker::~METMaker()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METMaker::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  StatusCode METMaker::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  // StatusCode METMaker::execute()
  // {
  //   ATH_MSG_DEBUG ( name() << " in execute...");

  //   return StatusCode::SUCCESS;
  // }

  // **** Rebuild generic MET term ****

  StatusCode METMaker::rebuildMET(xAOD::MissingET* met,
				  const xAOD::IParticleContainer* collection,
				  const xAOD::MissingETAssociationMap* map,
				  std::vector<const xAOD::IParticle*>& uniques)
  {
    MissingETBase::UsageHandler::Policy p = MissingETBase::UsageHandler::OnlyCluster;
    bool removeOverlap = true;
    if(m_doPFlow) {
      p = MissingETBase::UsageHandler::ParticleFlow;
    }
    if(!collection->empty()) {
      const IParticle* obj = collection->front();
      if(obj->type()==xAOD::Type::Muon) {
	if(!m_doPFlow) {
	  p = MissingETBase::UsageHandler::OnlyTrack;
	}
	removeOverlap = false;
      }
    }
    return rebuildMET(met,collection,map,uniques,p,removeOverlap);
  }

  StatusCode METMaker::rebuildMET(xAOD::MissingET* met,
				  const xAOD::IParticleContainer* collection,
				  const xAOD::MissingETAssociationMap* map,
				  std::vector<const xAOD::IParticle*>& uniques,
				  MissingETBase::UsageHandler::Policy p,
				  bool removeOverlap) {
    if(!met || !collection || !map) {
      ATH_MSG_WARNING("Invalid pointer supplied for "
		      << "MET (" << met << "), "
		      << "collection (" << collection << "), "
		      << " or map (" << map << ").");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_VERBOSE("Rebuilding MET term " << met->name());
    uniques.clear();
    if(!collection->empty()) {
      bool originalInputs = !collection->isAvailable<ElementLink<IParticleContainer> >("originalObjectLink");
      for(const auto& obj : *collection) {
	const MissingETAssociation* assoc = 0;
	if(originalInputs) {
	  assoc = MissingETComposition::getAssociation(map,obj);
	} else {
	  const IParticle* orig = *m_objLinkAcc(*obj);
	  assoc = MissingETComposition::getAssociation(map,orig);
	}
	if(assoc) {
	  bool selected(false);
	  // Don't overlap remove muons, but flag the non-overlapping muons to take out their tracks from jets
	  selected = !assoc->hasOverlaps(obj,p);
	  if(selected || !removeOverlap) {*met += obj;}
	  ATH_MSG_VERBOSE(obj->type() << " with pt " << obj->pt()
		       << " is " << ( selected ? "non-" : "") << "overlapping");
	  assoc->setObjSelectionFlag(obj,selected);
	  if(selected) {
	    uniques.push_back(obj);
	  }
	} else {
	  // special case for muons, which might not be associated to a jet (isolated)
	  if(obj->type()==xAOD::Type::Muon) {
	    *met += obj;
	  }
	}
      }
    }
    return StatusCode::SUCCESS;
  }

  // **** Rebuild jet & soft MET terms ****

  StatusCode METMaker::rebuildJetMET(xAOD::MissingET* metJet,
				     const xAOD::JetContainer* jets,
				     const xAOD::MissingETAssociationMap* map,
				     std::vector<const xAOD::IParticle*>& uniques,
				     xAOD::MissingET* metSoftClus,
				     const xAOD::MissingET* coreSoftClus,
				     xAOD::MissingET* metSoftTrk,
				     const xAOD::MissingET* coreSoftTrk) {
    if(!metJet || !jets || !map) {
      ATH_MSG_WARNING( "Invalid pointer supplied for "
		       << "RefJet term (" << metJet << "), "
		       << "jet collection (" << jets << "), "
		       << " or map (" << map << ")." );
      return StatusCode::SUCCESS;
    }
    ATH_MSG_VERBOSE("Rebuilding MET jet term " << metJet->name());
    if(!metSoftClus && !metSoftTrk) {
      ATH_MSG_WARNING("Neither soft cluster nor soft track term has been supplied!");
      return StatusCode::SUCCESS;
    }
    if(metSoftClus) {
      if(coreSoftClus) {
	ATH_MSG_VERBOSE("Rebuilding MET soft cluster term " << metSoftClus->name());
	*metSoftClus += *coreSoftClus;
      } else {
	ATH_MSG_WARNING("Soft cluster term provided without a core term!");
	return StatusCode::SUCCESS;
      }
    }
    if(metSoftTrk) {
      if(coreSoftTrk) {
	ATH_MSG_VERBOSE("Rebuilding MET soft track term " << metSoftTrk->name());
	*metSoftTrk += *coreSoftTrk;
      } else {
	ATH_MSG_WARNING("Soft track term provided without a core term!");
	return StatusCode::SUCCESS;
      }
    }
    uniques.clear();
    bool originalInputs = !jets->isAvailable<ElementLink<IParticleContainer> >("originalObjectLink");
    for(const auto& jet : *jets) {
      const MissingETAssociation* assoc = 0;
      if(originalInputs) {
	assoc = MissingETComposition::getAssociation(map,jet);
      } else {
	const IParticle* orig = *m_objLinkAcc(*jet);
	assoc = MissingETComposition::getAssociation(map,orig);
      }
      if(assoc) {
	ATH_MSG_VERBOSE( "Jet calib pt = " << jet->pt());
	bool selected = jet->pt()>m_jetMinPt;
	if(m_doJetJvf) {
	  vector<float> jvf;
	  jet->getAttribute<vector<float> >(xAOD::JetAttribute::JVF,jvf);
	  selected = (jet->pt()>50e3 || fabs(jet->eta())>2.4 || fabs(jvf[0])>m_jetMinAbsJvf);
	  ATH_MSG_VERBOSE("Jet " << (selected ? "passes" : "fails") <<" JVF selection");
	}
	bool hardJet(false);
	MissingETBase::Types::constvec_t calvec = assoc->overlapCalVec();
	bool caloverlap = false;
	caloverlap = calvec.ce()>0;
	ATH_MSG_DEBUG("Jet " << jet->index() << " is " << ( caloverlap ? "" : "non-") << "overlapping");
	
	xAOD::JetFourMom_t constjet = jet->jetP4("JetConstitScaleMomentum");
	double jpx = constjet.Px();
	double jpy = constjet.Py();
	double jpt = constjet.Pt();
	double opx = jpx - calvec.cpx();
	double opy = jpy - calvec.cpy();
	double opt = sqrt( opx*opx+opy*opy );
	double uniquefrac = 1. - (calvec.ce() / constjet.E());
	ATH_MSG_VERBOSE( "Jet constscale px, py, pt = " << jpx << ", " << jpy << ", " << jpt );
	ATH_MSG_VERBOSE( "Jet overlap E = " << calvec.ce() );
	ATH_MSG_VERBOSE( "Jet unique const E = " << constjet.E() - calvec.ce() );
	ATH_MSG_VERBOSE( "Jet OR px, py, pt = " << opx << ", " << opy << ", " << opt );
	if(selected) {
	  if(!caloverlap) {
	    // add jet full four-vector
	    hardJet = true;
	    *metJet += jet;
	  } else {
	    // check unique fraction
	    if(uniquefrac>m_jetMinEfrac && opt>m_jetMinWeightedPt) {
	      // add jet corrected for overlaps
	      hardJet = true;
	      if(m_jetCorrectPhi) {
		metJet->add(opx/jpx*jet->px(),opy/jpy*jet->py(),opt/jpt*jet->pt());
	      } else {
		metJet->add(uniquefrac*jet->px(),uniquefrac*jet->py(),uniquefrac*jet->pt());
	      }
	    }
	  }
	}  // hard jet selection

	if(hardJet){
	  ATH_MSG_VERBOSE("Jet added at full scale");
	  uniques.push_back(jet);
	} else {
	  if(metSoftClus) {
	    // add fractional contribution
	    // FIXME: extend to allow pileup-suppressed versions
	    ATH_MSG_VERBOSE("Jet added at const scale");
	    metSoftClus->add(opx,opy,opt);
	  }
	  if(metSoftTrk){
	    // use jet tracks
	    // remove any tracks already used by other objects
	    MissingETBase::Types::constvec_t trkvec = assoc->overlapTrkVec();
	    MissingETBase::Types::constvec_t jettrkvec = assoc->jetTrkVec();
	    if(jettrkvec.ce()>1e-9) {
	      jpx = jettrkvec.cpx();
	      jpy = jettrkvec.cpy();
	      jpt = jettrkvec.sumpt();
	      jettrkvec -= trkvec;
	      opx = jettrkvec.cpx();
	      opy = jettrkvec.cpy();
	      opt = jettrkvec.sumpt();
	      ATH_MSG_VERBOSE( "Jet track px, py, sumpt = " << jpx << ", " << jpy << ", " << jpt );
	      ATH_MSG_VERBOSE( "Jet OR px, py, sumpt = " << opx << ", " << opy << ", " << opt );
	    } else {
	      opx = opy = opt = 0;
	      ATH_MSG_VERBOSE( "This jet has no associated tracks" );
	    }
	    metSoftTrk->add(opx,opy,opt);
	  } // soft track
	} // soft jet
      } // association exists
      else {
	ATH_MSG_WARNING( "WARNING: Jet without association found!" );
      }
    } // jet loop

    if(metSoftTrk) {
      // supplement track term with any tracks associated to isolated muons
      // these are recorded in the misc association
      const MissingETAssociation* assoc = map->getMiscAssociation();
      if(assoc) {
	MissingETBase::Types::constvec_t trkvec = assoc->overlapTrkVec();
	double opx = trkvec.cpx();
	double opy = trkvec.cpy();
	double osumpt = trkvec.sumpt();;
	ATH_MSG_VERBOSE( "Misc track px, py, sumpt = " << opx << ", " << opy << ", " << osumpt );
	metSoftTrk->add(opx,opy,osumpt);
      }
    }

    if(metSoftClus) {
      // supplement cluster term with any clusters associated to isolated e/gamma
      // these are recorded in the misc association
      const MissingETAssociation* assoc = map->getMiscAssociation();
      if(assoc) {
	MissingETBase::Types::constvec_t calvec = assoc->overlapCalVec();
	double opx = calvec.cpx();
	double opy = calvec.cpy();
	double osumpt = calvec.sumpt();
	ATH_MSG_VERBOSE( "Misc cluster px, py, sumpt = " << opx << ", " << opy << ", " << osumpt );
	metSoftClus->add(opx,opy,osumpt);
      }
    }

    return StatusCode::SUCCESS;
  }

  // **** Sum up MET terms ****

  StatusCode METMaker::buildMETSum(const std::string& totalName,
				   xAOD::MissingETContainer* metCont,
				   MissingETBase::Types::bitmask_t softTermsSource)
  {
    ATH_MSG_DEBUG("Build MET total: " << totalName);

    MissingET* metFinal = new MissingET(0.,0.,0.,totalName,MissingETBase::Source::total());
    metCont->push_back(metFinal);

    for(const auto& met : *metCont) {
      if(met->source()==MissingETBase::Source::total()) continue;
      if(softTermsSource && MissingETBase::Source::isSoftTerm(met->source())) {
	if(!MissingETBase::Source::hasPattern(met->source(),softTermsSource)) continue;
      }
      ATH_MSG_VERBOSE("Add MET term " << met->name() );
      *metFinal += *met;
    }

    ATH_MSG_DEBUG( "Rebuilt MET Final --"
		   << " mpx: " << metFinal->mpx()
		   << " mpy: " << metFinal->mpy()
		   );
    
    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

} //> end namespace met

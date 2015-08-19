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

// Jet EDM
#include "xAODJet/JetAttributes.h"

// Tracking EDM
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"

// Shallow copy
#include "xAODCore/ShallowCopy.h"

// Muon EDM
#include "xAODMuon/Muon.h"

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
  // using xAOD::VertexContainer;
  // using xAOD::Vertex;

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

  // Constructors
  ////////////////
  METMaker::METMaker(const std::string& name) :
    AsgTool(name),
    m_objLinkAcc("originalObjectLink"),
    m_jetUsedDec("usedByMET")
  {
    //
    // Property declaration
    //

    // declareProperty("VxColl",             m_pvcoll             = "PrimaryVertices"   );

    declareProperty("JetPtCut",           m_jetMinPt           = 20e3                );
    declareProperty("JetJvtCut",          m_jetMinAbsJvt       = 0.64                );
    declareProperty("JetJvtMomentName",   m_jetJvtMomentName   = "Jvt"               );
    declareProperty("JetMinEFrac",        m_jetMinEfrac        = 0.5                 );
    declareProperty("JetMinWeightedPt",   m_jetMinWeightedPt   = 0.                  );
    //declareProperty("JetConstitScaleMom", m_jetConstitScaleMom = "JetLCScaleMomentum");
    declareProperty("JetConstitScaleMom", m_jetConstitScaleMom = "");
    declareProperty("CorrectJetPhi",      m_jetCorrectPhi      = false               );
    declareProperty("DoPFlow",            m_doPFlow            = false               );

    declareProperty("DoMuonEloss",        m_muEloss            = false               );
    declareProperty("DoIsolMuonEloss",    m_muIsolEloss        = false               );
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
				  const xAOD::MissingETAssociationMap* map) {

    std::vector<const xAOD::IParticle*> uniques;
    return rebuildMET(met,collection,map,uniques);

  }

  StatusCode METMaker::rebuildMET(const std::string& metKey,
                                  xAOD::Type::ObjectType metType,
                                  xAOD::MissingETContainer* metCont,
                                  const xAOD::IParticleContainer* collection,
                                  const xAOD::MissingETAssociationMap* map)
  {
    std::vector<const xAOD::IParticle*> uniques;
    return rebuildMET(metKey,metType,metCont,collection,map,uniques);
  }

  StatusCode METMaker::rebuildMET(const std::string& metKey,
                                  xAOD::Type::ObjectType metType,
                                  xAOD::MissingETContainer* metCont,
                                  const xAOD::IParticleContainer* collection,
                                  const xAOD::MissingETAssociationMap* map,
                                  std::vector<const xAOD::IParticle*>& uniques)
  {
    MissingETBase::Types::bitmask_t metSource;
    switch(metType) {
    case xAOD::Type::Electron:
      metSource = MissingETBase::Source::electron();
      break;
    case xAOD::Type::Photon:
      metSource = MissingETBase::Source::photon();
      break;
    case xAOD::Type::Tau:
      metSource = MissingETBase::Source::tau();
      break;
    case xAOD::Type::Muon:
      metSource = MissingETBase::Source::muon();
      break;
    case xAOD::Type::Jet:
      ATH_MSG_WARNING("Incorrect use of rebuildMET -- use rebuildJetMET for RefJet term");
      return StatusCode::FAILURE;
    default:
      ATH_MSG_WARNING("Invalid object type provided: " << metType);
      return StatusCode::FAILURE;
    }
    //    MissingET* met = new MissingET(metKey,metSource);
    MissingET* met = nullptr;
    if( fillMET(met,metCont, metKey , metSource) != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("failed to fill MET term");
      return StatusCode::FAILURE;
    }
    //    metCont->push_back(met);
    return rebuildMET(met,collection,map,uniques);
  }

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
    if(map->empty()) {
      ATH_MSG_WARNING("Incomplete association map received. Abort.");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_VERBOSE("Building MET term " << met->name());
    uniques.clear();
    if(!collection->empty()) {
      bool originalInputs = !collection->front()->isAvailable<ElementLink<IParticleContainer> >("originalObjectLink");
      bool isShallowCopy = dynamic_cast<const xAOD::ShallowAuxContainer*>(collection->front()->container()->getConstStore());
      ATH_MSG_VERBOSE("const store = " << collection->front()->container()->getConstStore());
      if(isShallowCopy && originalInputs) {
	ATH_MSG_WARNING("Shallow copy provided without \"originalObjectLinks\" decoration! "
			<< "Overlap removal cannot be done. "
			<< "Will not compute this term.");
	ATH_MSG_WARNING("Please apply xAOD::setOriginalObjectLinks() from xAODBase/IParticleHelpers.h");
	return StatusCode::SUCCESS;
      } else {
	ATH_MSG_VERBOSE("Original inputs? " << originalInputs);
      }
      for(const auto& obj : *collection) {
	const IParticle* orig = obj;
        bool selected = false;
        if(!originalInputs) { orig = *m_objLinkAcc(*obj); }

	// If the object has already been selected and processed, ignore it.
	if(MissingETComposition::objSelected(map,orig)) continue;
	selected = MissingETComposition::selectIfNoOverlaps(map,orig,p);
        ATH_MSG_VERBOSE(obj->type() << " (" << orig <<") with pt " << obj->pt()
                        << " is " << ( selected ? "non-" : "") << "overlapping");

        // Don't overlap remove muons, but flag the non-overlapping muons to take out their tracks from jets
	if(selected || !removeOverlap) {
	  if(m_muEloss && MissingETBase::Source::isMuonTerm(met->source())) {
	    const xAOD::Muon* muon = static_cast<const xAOD::Muon*>(obj);
	    bool isolated = MissingETComposition::getRefJet(map,orig)==0;
	    ATH_MSG_VERBOSE("Muon isolated? " << isolated);
	    // apply Eloss correction
	    if(!isolated || m_muIsolEloss) {
	      float eloss = muon->floatParameter(xAOD::Muon::EnergyLoss);
	      float eloss_corr = 1. - eloss / muon->e();
	      ATH_MSG_VERBOSE("Eloss: " << eloss << ", E: " << muon->e() << ", correction factor: " << eloss_corr);
	      if( eloss>muon->e()) {ATH_MSG_WARNING("Muon Eloss " << eloss << " > muon E " << muon->e() << "!"); }
	      else { met->add(muon,eloss_corr); }
	    }
	  } else {
	    *met += obj;
	  }
	}
        if(selected) {
          uniques.push_back(obj);
        }
      }
    }
    ATH_MSG_DEBUG("Built met term " << met->name() << ", with magnitude " << met->met());
    return StatusCode::SUCCESS;
  }

  // **** Rebuild jet & soft MET terms ****
  StatusCode METMaker::rebuildJetMET(const std::string& metJetKey,
				     const std::string& softKey,
				     xAOD::MissingETContainer* metCont,
				     const xAOD::JetContainer* jets,
				     const xAOD::MissingETContainer* metCoreCont,
				     const xAOD::MissingETAssociationMap* map,
				     bool doJetJVT)
  {
    std::vector<const xAOD::IParticle*> uniques;

    return rebuildJetMET(metJetKey,softKey,metCont,
                         jets,metCoreCont,map,doJetJVT,uniques);
  }

  // **** Rebuild jet & soft MET terms ****
  StatusCode METMaker::rebuildTrackMET(const std::string& metJetKey,
				       const std::string& softKey,
				       xAOD::MissingETContainer* metCont,
				       const xAOD::JetContainer* jets,
				       const xAOD::MissingETContainer* metCoreCont,
				       const xAOD::MissingETAssociationMap* map,
				       bool doJetJVT)
  {
    std::vector<const xAOD::IParticle*> uniques;

    return rebuildTrackMET(metJetKey,softKey,metCont,
			   jets,metCoreCont,map,doJetJVT,uniques);
  }

  StatusCode METMaker::rebuildJetMET(const std::string& metJetKey,
                                     const std::string& softKey,
                                     xAOD::MissingETContainer* metCont,
                                     const xAOD::JetContainer* jets,
                                     const xAOD::MissingETContainer* metCoreCont,
                                     const xAOD::MissingETAssociationMap* map,
                                     bool doJetJVT,
                                     std::vector<const xAOD::IParticle*>& uniques)
  {
    ATH_MSG_VERBOSE("Rebuild jet term: " << metJetKey << " and soft term: " << softKey);

    MissingET* metJet = nullptr; //= new MissingET(metJetKey,MissingETBase::Source::jet());
    if( fillMET(metJet,metCont, metJetKey, MissingETBase::Source::jet()) != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("failed to fill MET term");
      return StatusCode::FAILURE;
    }
    //    metCont->push_back(metJet);

    const MissingET *coreSoftClus(0), *coreSoftTrk(0);
    MissingET *metSoftClus(0), *metSoftTrk(0);

    const MissingET* coreSoft = (*metCoreCont)[softKey+"Core"];
    if(!coreSoft) {
      ATH_MSG_WARNING("Invalid soft term key supplied: " << softKey);
      return StatusCode::FAILURE;
    }
    if(MissingETBase::Source::isTrackTerm(coreSoft->source())) {
      coreSoftTrk = coreSoft;

      metSoftTrk = nullptr; //= new MissingET(softKey,coreSoftTrk->source());
      if( fillMET(metSoftTrk,metCont, softKey , coreSoftTrk->source() ) != StatusCode::SUCCESS) {
	ATH_MSG_ERROR("failed to fill MET term");
	return StatusCode::FAILURE;
      }
      //      metCont->push_back(metSoftTrk);
    } else {
      coreSoftClus = coreSoft;

      metSoftClus = nullptr; //= new MissingET(softKey,coreSoftTrk->source());
      if( fillMET(metSoftClus, metCont, softKey , coreSoftClus->source() ) != StatusCode::SUCCESS) {
	ATH_MSG_ERROR("failed to fill MET term");
	return StatusCode::FAILURE;
      }
      //      metCont->push_back(metSoftClus);
    }

    return rebuildJetMET(metJet, jets, map,
                         metSoftClus, coreSoftClus,
                         metSoftTrk,  coreSoftTrk,
                         doJetJVT,uniques);
  }

  StatusCode METMaker::rebuildTrackMET(const std::string& metJetKey,
				       const std::string& softKey,
				       xAOD::MissingETContainer* metCont,
				       const xAOD::JetContainer* jets,
				       const xAOD::MissingETContainer* metCoreCont,
				       const xAOD::MissingETAssociationMap* map,
				       bool doJetJVT,
				       std::vector<const xAOD::IParticle*>& uniques)
  {
    ATH_MSG_VERBOSE("Rebuild jet term: " << metJetKey << " and soft term: " << softKey);

    MissingET* metJet = nullptr; // new MissingET(metJetKey,MissingETBase::Source::jet() | MissingETBase::Source::track() );//jet track met
    if( fillMET(metJet , metCont,  metJetKey  , MissingETBase::Source::jet() | MissingETBase::Source::track() ) != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("failed to fill MET term");
      return StatusCode::FAILURE;
    }
    //    metCont->push_back(metJet);

    const MissingET *coreSoftTrk(0);
    MissingET *metSoftTrk(0);

    const MissingET* coreSoft = (*metCoreCont)[softKey+"Core"];
    if(!coreSoft) {
      ATH_MSG_WARNING("Invalid soft term key supplied: " << softKey);
      return StatusCode::FAILURE;
    }
    coreSoftTrk = coreSoft;

    //    metSoftTrk = new MissingET(softKey,coreSoftTrk->source());
    metSoftTrk = nullptr;
    if( fillMET(metSoftTrk , metCont, softKey  , coreSoftTrk->source()) != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("failed to fill MET term");
      return StatusCode::FAILURE;
    }
    //    metCont->push_back(metSoftTrk);

    return rebuildTrackMET(metJet, jets, map,
			   metSoftTrk,  coreSoftTrk,
			   doJetJVT,uniques);
  }

  StatusCode METMaker::rebuildJetMET(const std::string& metJetKey,
				     const std::string& softClusKey,
				     const std::string& softTrkKey,
				     xAOD::MissingETContainer* metCont,
				     const xAOD::JetContainer* jets,
				     const xAOD::MissingETContainer* metCoreCont,
				     const xAOD::MissingETAssociationMap* map,
				     bool doJetJVT)
  {
    std::vector<const xAOD::IParticle*> uniques;
    return rebuildJetMET(metJetKey,softClusKey,softTrkKey,metCont,
                         jets,metCoreCont,map,doJetJVT,uniques);
  }

  StatusCode METMaker::rebuildJetMET(const std::string& metJetKey,
                                     const std::string& softClusKey,
                                     const std::string& softTrkKey,
                                     xAOD::MissingETContainer* metCont,
                                     const xAOD::JetContainer* jets,
                                     const xAOD::MissingETContainer* metCoreCont,
                                     const xAOD::MissingETAssociationMap* map,
                                     bool doJetJVT,
                                     std::vector<const xAOD::IParticle*>& uniques)
  {

    ATH_MSG_VERBOSE("Create Jet MET " << metJetKey);
    MissingET* metJet = nullptr; // = new MissingET(metJetKey,MissingETBase::Source::jet());
    if( fillMET(metJet , metCont ,metJetKey , MissingETBase::Source::jet()) != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("failed to fill MET term");
      return StatusCode::FAILURE;
    }
    //    metCont->push_back(metJet);
    ATH_MSG_VERBOSE("Create SoftClus MET " << softClusKey);
    const MissingET* coreSoftClus = (*metCoreCont)[softClusKey+"Core"];
    ATH_MSG_VERBOSE("Create SoftTrk MET " << softTrkKey);
    const MissingET* coreSoftTrk = (*metCoreCont)[softTrkKey+"Core"];
    if(!coreSoftClus) {
      ATH_MSG_WARNING("Invalid cluster soft term key supplied: " << softClusKey);
      return StatusCode::FAILURE;
    }
    if(!coreSoftTrk) {
      ATH_MSG_WARNING("Invalid track soft term key supplied: " << softTrkKey);
      return StatusCode::FAILURE;
    }
    MissingET* metSoftClus = nullptr; // new MissingET(softClusKey,coreSoftClus->source());
    if( fillMET(metSoftClus, metCont, softClusKey,  coreSoftClus->source()) != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("failed to fill MET term");
      return StatusCode::FAILURE;
    }
    //    metCont->push_back(metSoftClus);

    MissingET* metSoftTrk = nullptr; // new MissingET(softTrkKey,coreSoftTrk->source());
    if( fillMET(metSoftTrk, metCont, softTrkKey,  coreSoftTrk->source()) != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("failed to fill MET term");
      return StatusCode::FAILURE;
    }
    //    metCont->push_back(metSoftTrk);

    return rebuildJetMET(metJet, jets, map,
                         metSoftClus, coreSoftClus,
                         metSoftTrk, coreSoftTrk,
                         doJetJVT,uniques);
  }

  StatusCode METMaker::rebuildJetMET(xAOD::MissingET* metJet,
                                     const xAOD::JetContainer* jets,
                                     const xAOD::MissingETAssociationMap* map,
                                     xAOD::MissingET* metSoftClus,
                                     const xAOD::MissingET* coreSoftClus,
                                     xAOD::MissingET* metSoftTrk,
                                     const xAOD::MissingET* coreSoftTrk,
                                     bool doJetJVT,
                                     std::vector<const xAOD::IParticle*>& uniques,
                                     bool tracksForHardJets) {
    if(!metJet || !jets || !map) {
      ATH_MSG_WARNING( "Invalid pointer supplied for "
		       << "RefJet term (" << metJet << "), "
		       << "jet collection (" << jets << "), "
		       << " or map (" << map << ")." );
      return StatusCode::SUCCESS;
    }
    if(map->empty()) {
      ATH_MSG_WARNING("Incomplete association map received. Abort.");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_VERBOSE("Building MET jet term " << metJet->name());
    if(!metSoftClus && !metSoftTrk) {
      ATH_MSG_WARNING("Neither soft cluster nor soft track term has been supplied!");
      return StatusCode::SUCCESS;
    }
    if(metSoftClus) {
      if(coreSoftClus) {
        ATH_MSG_VERBOSE("Building MET soft cluster term " << metSoftClus->name());
	ATH_MSG_VERBOSE("Core soft cluster mpx " << coreSoftClus->mpx()
			<< ", mpy " << coreSoftClus->mpy()
			<< " sumet " << coreSoftClus->sumet());
        *metSoftClus += *coreSoftClus;
      } else {
        ATH_MSG_WARNING("Soft cluster term provided without a core term!");
        return StatusCode::SUCCESS;
      }
    }
    if(metSoftTrk) {
      if(coreSoftTrk) {
        ATH_MSG_VERBOSE("Building MET soft track term " << metSoftTrk->name());
	ATH_MSG_VERBOSE("Core soft track mpx " << coreSoftTrk->mpx()
			<< ", mpy " << coreSoftTrk->mpy()
			<< " sumet " << coreSoftTrk->sumet());
        *metSoftTrk += *coreSoftTrk;
      } else {
        ATH_MSG_WARNING("Soft track term provided without a core term!");
        return StatusCode::SUCCESS;
      }
    }

    uniques.clear();
    bool originalInputs = jets->empty() ? false : !jets->front()->isAvailable<ElementLink<IParticleContainer> >("originalObjectLink");
    for(const auto& jet : *jets) {
      bool isUsedinMET=false;
      const MissingETAssociation* assoc = 0;
      if(originalInputs) {
        assoc = MissingETComposition::getAssociation(map,jet);
      } else {
        const IParticle* orig = *m_objLinkAcc(*jet);
        assoc = MissingETComposition::getAssociation(map,static_cast<const xAOD::Jet*>(orig));
      }
      if(assoc && !assoc->isMisc()) {
        ATH_MSG_VERBOSE( "Jet calib pt = " << jet->pt());
        bool selected = jet->pt()>m_jetMinPt;
        bool JVT_reject(false);
        if(doJetJVT) {
	  if(jet->pt()<50e3 && fabs(jet->eta())<2.4) {
	    float jvt;
	    jet->getAttribute<float>(m_jetJvtMomentName,jvt);
	    JVT_reject = jvt<m_jetMinAbsJvt;
	    ATH_MSG_VERBOSE("Jet " << (JVT_reject ? "fails" : "passes") <<" JVT selection");
	    // MissingETBase::Types::constvec_t trkvec = assoc->overlapTrkVec();
	    // MissingETBase::Types::constvec_t calvec = assoc->overlapCalVec();
	    // MissingETBase::Types::constvec_t jettrkvec = assoc->jetTrkVec();	    
	    // if(!JVT_reject && trkvec.sumpt()>0) {
	    //   ATH_MSG_VERBOSE("Jet passing JVT has trkvec sumpt = " << trkvec.sumpt()
	    // 		      << ", jettrkvec sumpt = " << jettrkvec.sumpt()
	    // 		      << ", calvec sumpt = " << calvec.sumpt()
	    // 		      << ", const scale pt = " << jet->jetP4("JetConstitScaleMomentum").Pt());
	    //   if(trkvec.sumpt()/jettrkvec.sumpt()>0.9 && calvec.sumpt()<1000) {
	    // 	// remove if jet tracks are mostly removed but no calo energy is removed
	    // 	ATH_MSG_DEBUG("Remove jet -- appears to pass JVT due to a muon. OR-ed track pt: " << jettrkvec.sumpt()-trkvec.sumpt()
	    // 		      << ", OR-ed calo pt: " << jet->jetP4("JetConstitScaleMomentum").Pt()-calvec.sumpt());
	    // 	JVT_reject = true;
	    //   }
	    // }
	  }
        }
        bool hardJet(false);
        MissingETBase::Types::constvec_t calvec = assoc->overlapCalVec();
        bool caloverlap = false;
        caloverlap = calvec.ce()>0;
        ATH_MSG_DEBUG("Jet " << jet->index() << " is " << ( caloverlap ? "" : "non-") << "overlapping");

	xAOD::JetFourMom_t constjet;
	if(m_jetConstitScaleMom.empty()){
	  constjet = (assoc->hasAlternateConstVec() ? assoc->getAlternateConstVec() : jet->jetP4("JetConstitScaleMomentum"));
	}else{
	  constjet = jet->jetP4(m_jetConstitScaleMom);//grab a constituent scale added by the JetMomentTool/JetConstitFourMomTool.cxx
	}
	calvec *= constjet.E()/(assoc->hasAlternateConstVec() ? assoc->getAlternateConstVec() : jet->jetP4("JetConstitScaleMomentum")).E();
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
        if(selected && !JVT_reject) {
          if(!caloverlap) {
            // add jet full four-vector
            hardJet = true;
            if (!tracksForHardJets) *metJet += jet;
          } else {
            // check unique fraction
            if(uniquefrac>m_jetMinEfrac && opt>m_jetMinWeightedPt) {
              // add jet corrected for overlaps
              hardJet = true;
              if(m_jetCorrectPhi && !tracksForHardJets) {
                metJet->add(opx/jpx*jet->px(),opy/jpy*jet->py(),opt/jpt*jet->pt());
              } else if (!tracksForHardJets) {
                metJet->add(uniquefrac*jet->px(),uniquefrac*jet->py(),uniquefrac*jet->pt());
              }
            }
          }
        }  // hard jet selection

        if(hardJet){
          ATH_MSG_VERBOSE("Jet added at full scale");
          uniques.push_back(jet);
          isUsedinMET=true;
        } else {
          if(metSoftClus && !JVT_reject) {
            // add fractional contribution
            ATH_MSG_VERBOSE("Jet added at const scale");
            metSoftClus->add(opx,opy,opt);
          }
        }
        if(metSoftTrk && (!hardJet || tracksForHardJets)){
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
          if (hardJet) metJet->add(opx,opy,opt);
          else metSoftTrk->add(opx,opy,opt);
        } // soft track
      } // association exists
      else {
        ATH_MSG_WARNING( "WARNING: Jet without association found!" );
      }
      m_jetUsedDec(*jet)=isUsedinMET;
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
	ATH_MSG_VERBOSE("Final soft track mpx " << metSoftTrk->mpx()
			<< ", mpy " << metSoftTrk->mpy()
			<< " sumet " << metSoftTrk->sumet());
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
	ATH_MSG_VERBOSE("Final soft cluster mpx " << metSoftClus->mpx()
			<< ", mpy " << metSoftClus->mpy()
			<< " sumet " << metSoftClus->sumet());
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode METMaker::rebuildTrackMET(xAOD::MissingET* metJet,
				       const xAOD::JetContainer* jets,
				       const xAOD::MissingETAssociationMap* map,
				       xAOD::MissingET* metSoftTrk,
				       const xAOD::MissingET* coreSoftTrk,
				       bool doJetJVT,
				       std::vector<const xAOD::IParticle*>& uniques) {
    return rebuildJetMET(metJet,jets,map,NULL,NULL,metSoftTrk,coreSoftTrk,doJetJVT,uniques,true);
  }

  // **** Remove objects and any overlaps from MET calculation ****

  StatusCode METMaker::markInvisible(const xAOD::IParticleContainer* collection,
				     const xAOD::MissingETAssociationMap* map,
				     MissingETBase::UsageHandler::Policy p)
  {
    if(!collection || !map) {
      ATH_MSG_WARNING("Invalid pointer supplied for "
                      << "collection (" << collection << "), "
                      << " or map (" << map << ").");
      return StatusCode::SUCCESS;
    }
    if(map->empty()) {
      ATH_MSG_WARNING("Incomplete association map received. Abort.");
      return StatusCode::SUCCESS;
    }
    if(!collection->empty()) {
      bool originalInputs = !collection->front()->isAvailable<ElementLink<IParticleContainer> >("originalObjectLink");
      bool isShallowCopy = dynamic_cast<const xAOD::ShallowAuxContainer*>(collection->front()->container()->getConstStore());
      if(isShallowCopy && originalInputs) {
	ATH_MSG_WARNING("Shallow copy provided without \"originalObjectLinks\" decoration! "
			<< "Overlap removal cannot be done. "
			<< "Will not mark as invisible.");
	ATH_MSG_WARNING("Please apply xAOD::setOriginalObjectLinks() from xAODBase/IParticleHelpers.h");
	return StatusCode::SUCCESS;
      } else {
	ATH_MSG_VERBOSE("Original inputs? " << originalInputs);
      }
      for(const auto& obj : *collection) {
	const IParticle* orig = obj;
        if(!originalInputs) { orig = *m_objLinkAcc(*obj); }
	ATH_MSG_VERBOSE("Current particle " << obj << ", orig " << orig);
	MissingETComposition::selectIfNoOverlaps(map,orig,p);
	ATH_MSG_VERBOSE("Marked object " << obj->type() << " (" << orig <<") "
			<< " with pt " << obj->pt() << " as invisible."
			<< " Selected ? " << MissingETComposition::objSelected(map,orig));
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

    MissingET* metFinal = nullptr;//  new MissingET(0.,0.,0.,totalName,MissingETBase::Source::total());
    if( fillMET(metFinal, metCont, totalName, MissingETBase::Source::total()) != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("failed to fill MET term");
      return StatusCode::FAILURE;
    }
    //    metCont->push_back(metFinal);

    for(const auto& met : *metCont) {
      if(MissingETBase::Source::isTotalTerm(met->source())) continue;
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

  //this is used to not create a private store
  //it puts the given new MET object into the container
  StatusCode METMaker::fillMET(xAOD::MissingET *& met,
			       xAOD::MissingETContainer * metCont,
			       const std::string& metKey,
			       const MissingETBase::Types::bitmask_t metSource){
    if(met != nullptr){
      ATH_MSG_ERROR("You can't fill a filled MET value");
      return StatusCode::FAILURE;
    }

    met = new xAOD::MissingET();
    metCont->push_back(met);

    met->setName  (metKey);
    met->setSource(metSource);

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

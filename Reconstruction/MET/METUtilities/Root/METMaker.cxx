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
#include "xAODMuon/MuonContainer.h"

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

  static const SG::AuxElement::ConstAccessor< std::vector<ElementLink<xAOD::IParticleContainer> > > acc_ghostMuons("GhostMuon");

  static const SG::AuxElement::ConstAccessor< std::vector<int> > acc_trkN("NumTrkPt500");
  static const SG::AuxElement::ConstAccessor< std::vector<float> > acc_trksumpt("SumPtTrkPt500");
  static const SG::AuxElement::ConstAccessor< std::vector<float> > acc_sampleE("EnergyPerSampling");

  static const SG::AuxElement::ConstAccessor<float> acc_emf("EMFrac");
  static const SG::AuxElement::ConstAccessor<float> acc_width("Width");
  static const SG::AuxElement::ConstAccessor<float> acc_Eloss("EnergyLoss");

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

  // Constructors
  ////////////////
  METMaker::METMaker(const std::string& name) :
    AsgTool(name),
    m_objLinkAcc("originalObjectLink"),
    m_objUsedDec("usedByMET"),
    m_trkseltool("")
  {
    //
    // Property declaration
    //

    // declareProperty("VxColl",             m_pvcoll             = "PrimaryVertices"   );

    declareProperty("JetJvtMomentName",   m_jetJvtMomentName   = "Jvt"               );
    declareProperty("JetMinEFrac",        m_jetMinEfrac        = 0.5                 );
    declareProperty("JetMinWeightedPt",   m_jetMinWeightedPt   = 0.                  );
    //declareProperty("JetConstitScaleMom", m_jetConstitScaleMom = "JetLCScaleMomentum");
    declareProperty("JetConstitScaleMom", m_jetConstitScaleMom = "JetConstitScaleMomentum");
    declareProperty("CorrectJetPhi",      m_jetCorrectPhi      = false               );
    declareProperty("DoPFlow",            m_doPFlow            = false               );
    declareProperty("DoSoftTruth",        m_doSoftTruth        = false               );
    declareProperty("DoJetTruth",         m_doConstJet         = false               );

    declareProperty("JetSelection",       m_jetSelection       = "Default"           );//Default, Tight or Expert
    declareProperty("CustomCentralJetPt", m_customCenJetPtCut  = 20e3                );
    declareProperty("CustomForwardJetPt", m_customFwdJetPtCut  = 20e3                );
    declareProperty("CustomJetJvtCut",    m_customJvtCut       = 0.64                );
    declareProperty("CustomJetJvtPtMax",  m_customJvtPtMax     = 50e3                );

    declareProperty("DoMuonEloss",        m_muEloss            = false               );
    declareProperty("ORCaloTaggedMuons",  m_orCaloTaggedMuon   = true                );

    declareProperty("UseGhostMuons",      m_useGhostMuons      = false               );
    declareProperty("DoRemoveMuonJets",   m_doRemoveMuonJets   = false               );
    declareProperty("DoSetMuonJetEMScale", m_doSetMuonJetEMScale = false             );

    declareProperty("TrackSelectorTool",  m_trkseltool                               );
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
    
    //default jet selection i.e. pre-recommendation
    ATH_MSG_VERBOSE("Use jet selection criterion: " << m_jetSelection);
    if (m_jetSelection == "Default")     { m_CenJetPtCut = 20e3; m_FwdJetPtCut = 20e3; m_JvtCut = 0.64; m_JvtPtMax = 50e3;}
    else if (m_jetSelection == "Tight")  { m_CenJetPtCut = 20e3; m_FwdJetPtCut = 30e3; m_JvtCut = 0.64; m_JvtPtMax = 50e3;}
    else if (m_jetSelection == "Tier0")  { m_CenJetPtCut = 0;    m_FwdJetPtCut = 0;    m_JvtCut = -1;   m_JvtPtMax = 0;}
    else if (m_jetSelection == "Expert")  { 
      ATH_MSG_INFO("Custom jet selection configured. *** FOR EXPERT USE ONLY ***");
      m_CenJetPtCut = m_customCenJetPtCut;
      m_FwdJetPtCut = m_customFwdJetPtCut;
      m_JvtCut = m_customJvtCut;
      m_JvtPtMax = m_customJvtPtMax; 
    }
    else { ATH_MSG_ERROR( "Error: No available jet selection found! Choose one: Default, Tight, Expert" ); return StatusCode::FAILURE; }

    if (!m_trkseltool.empty()) ATH_CHECK( m_trkseltool.retrieve() );

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
				  MissingETBase::UsageHandler::Policy objScale) {

    std::vector<const xAOD::IParticle*> uniques;
    return rebuildMET(met,collection,map,uniques,objScale);

  }

  StatusCode METMaker::rebuildMET(const std::string& metKey,
                                  xAOD::Type::ObjectType metType,
                                  xAOD::MissingETContainer* metCont,
                                  const xAOD::IParticleContainer* collection,
                                  const xAOD::MissingETAssociationMap* map,
				  MissingETBase::UsageHandler::Policy objScale) 
  {
    std::vector<const xAOD::IParticle*> uniques;
    return rebuildMET(metKey,metType,metCont,collection,map,uniques,objScale);
  }

  StatusCode METMaker::rebuildMET(const std::string& metKey,
                                  xAOD::Type::ObjectType metType,
                                  xAOD::MissingETContainer* metCont,
                                  const xAOD::IParticleContainer* collection,
                                  const xAOD::MissingETAssociationMap* map,
                                  std::vector<const xAOD::IParticle*>& uniques,
				  MissingETBase::UsageHandler::Policy objScale)
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

    MissingET* met = nullptr;
    if( fillMET(met,metCont, metKey , metSource) != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("failed to fill MET term");
      return StatusCode::FAILURE;
    }

    // If muon eloss corrections are required, create a new term to hold these if it doesn't already exist
    if(metType==xAOD::Type::Muon && (m_muEloss || m_doSetMuonJetEMScale) && !(*metCont)["MuonEloss"]) {
      MissingET* met_muEloss = nullptr;
      if( fillMET(met_muEloss,metCont,"MuonEloss",
		  MissingETBase::Source::Muon | MissingETBase::Source::Calo) != StatusCode::SUCCESS) {
	ATH_MSG_ERROR("failed to create Muon Eloss MET term");
	return StatusCode::FAILURE;
      }
    }

    return rebuildMET(met,collection,map,uniques,objScale);
  }

  StatusCode METMaker::rebuildMET(xAOD::MissingET* met,
                                  const xAOD::IParticleContainer* collection,
                                  const xAOD::MissingETAssociationMap* map,
                                  std::vector<const xAOD::IParticle*>& uniques,
				  MissingETBase::UsageHandler::Policy objScale) 
  {
    MissingETBase::UsageHandler::Policy p = MissingETBase::UsageHandler::OnlyCluster;
    bool removeOverlap = true;
    if(!collection->empty()) {
      const IParticle* obj = collection->front();
      if(obj->type()==xAOD::Type::Muon) {
        p = MissingETBase::UsageHandler::OnlyTrack;
        removeOverlap = false;
      }
    }
    if (m_doSoftTruth) p = MissingETBase::UsageHandler::TruthParticle;
    if (m_doPFlow) p = MissingETBase::UsageHandler::ParticleFlow;
    return rebuildMET(met,collection,map,uniques,p,removeOverlap,objScale);
  }

  StatusCode METMaker::rebuildMET(xAOD::MissingET* met,
                                  const xAOD::IParticleContainer* collection,
                                  const xAOD::MissingETAssociationMap* map,
                                  std::vector<const xAOD::IParticle*>& uniques,
                                  MissingETBase::UsageHandler::Policy p,
                                  bool removeOverlap,
				  MissingETBase::UsageHandler::Policy objScale) {
    if(!met || !collection || !map) {
      ATH_MSG_WARNING("Invalid pointer supplied for "
                      << "MET (" << met << "), "
                      << "collection (" << collection << "), "
                      << " or map (" << map << ").");
      return StatusCode::SUCCESS;
    }
    if(map->empty()) {
      ATH_MSG_WARNING("Incomplete association map received. Cannot rebuild MET.");
      ATH_MSG_WARNING("Note: METMaker should only be run on events containing at least one PV");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_VERBOSE("Building MET term " << met->name());
    uniques.clear();
    if(!collection->empty()) {
      bool originalInputs = !m_objLinkAcc.isAvailable(*collection->front());
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
	std::vector<const xAOD::MissingETAssociation*> assocs = xAOD::MissingETComposition::getAssociations(map,orig);
	if(assocs.empty()) {
	  ATH_MSG_WARNING("Object is not in association map. Did you make a deep copy but fail to set the \"originalObjectLinks\" decoration?");
	  ATH_MSG_WARNING("If not, Please apply xAOD::setOriginalObjectLinks() from xAODBase/IParticleHelpers.h");
	}

	// If the object has already been selected and processed, ignore it.
	if(MissingETComposition::objSelected(map,orig)) continue;
	selected = MissingETComposition::selectIfNoOverlaps(map,orig,p) || !removeOverlap;
        ATH_MSG_VERBOSE(obj->type() << " (" << orig <<") with pt " << obj->pt()
                        << " is " << ( selected ? "non-" : "") << "overlapping");

	//Do special overlap removal for calo tagged muons
	if(m_orCaloTaggedMuon && !removeOverlap && orig->type()==xAOD::Type::Muon && static_cast<const xAOD::Muon*>(orig)->muonType()==xAOD::Muon::CaloTagged) {
	  for (size_t i = 0; i < assocs.size(); i++) {
	    std::vector<size_t> ind = assocs[i]->overlapIndices(orig);
	    std::vector<const xAOD::IParticle*> allObjects = assocs[i]->objects();
	    for (size_t indi = 0; indi < ind.size(); indi++) {
	      if (allObjects[ind[indi]]->type()==xAOD::Type::Electron
		  && assocs[i]->objSelected(ind[indi])) {
		selected = false;
		break;
	      }
	    }
	  }
	}
        // Don't overlap remove muons, but flag the non-overlapping muons to take out their tracks from jets
	// Removed eloss from here -- clusters already flagged.
	// To be handled in rebuildJetMET
	if(selected) {
	  if(objScale==MissingETBase::UsageHandler::PhysicsObject) {
	    ATH_MSG_VERBOSE("Add object with pt " << obj->pt());
	    *met += obj;
	  } else {
	    MissingETBase::Types::constvec_t constvec = MissingETComposition::getConstVec(map,obj,objScale);
	    ATH_MSG_VERBOSE("Add truth object with pt " << constvec.cpt());
	    met->add(constvec.cpx(),constvec.cpy(),constvec.cpt());
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

    const MissingET *coreSoftClus(0), *coreSoftTrk(0);
    MissingET *metSoftClus(0), *metSoftTrk(0);

    const MissingET* coreSoft = (*metCoreCont)[softKey+"Core"];
    if(!coreSoft) {
      ATH_MSG_WARNING("Invalid soft term key supplied: " << softKey);
      return StatusCode::FAILURE;
    }
    if(MissingETBase::Source::isTrackTerm(coreSoft->source())) {
      coreSoftTrk = coreSoft;

      metSoftTrk = nullptr;
      if( fillMET(metSoftTrk,metCont, softKey , coreSoftTrk->source() ) != StatusCode::SUCCESS) {
	ATH_MSG_ERROR("failed to fill MET term");
	return StatusCode::FAILURE;
      }
    } else {
      coreSoftClus = coreSoft;

      metSoftClus = nullptr;
      if( fillMET(metSoftClus, metCont, softKey , coreSoftClus->source() ) != StatusCode::SUCCESS) {
	ATH_MSG_ERROR("failed to fill MET term");
	return StatusCode::FAILURE;
      }
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

    MissingET* metSoftTrk = nullptr; // new MissingET(softTrkKey,coreSoftTrk->source());
    if( fillMET(metSoftTrk, metCont, softTrkKey,  coreSoftTrk->source()) != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("failed to fill MET term");
      return StatusCode::FAILURE;
    }

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
                                     bool tracksForHardJets,
                                     std::vector<const xAOD::IParticle*>* softConst) {
    if(!metJet || !jets || !map) {
      ATH_MSG_WARNING( "Invalid pointer supplied for "
		       << "RefJet term (" << metJet << "), "
		       << "jet collection (" << jets << "), "
		       << " or map (" << map << ")." );
      return StatusCode::SUCCESS;
    }
    if(softConst && m_trkseltool.empty() && !m_doPFlow && !m_doSoftTruth) {
      ATH_MSG_WARNING( "Requested soft track element links, but no track selection tool supplied.");
    }
    const xAOD::Vertex *pv = softConst?getPV():0;

    if(map->empty()) {
      ATH_MSG_WARNING("Incomplete association map received. Cannot rebuild MET.");
      ATH_MSG_WARNING("Note: METMaker should only be run on events containing at least one PV");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_VERBOSE("Building MET jet term " << metJet->name());
    if(!metSoftClus && !metSoftTrk) {
      ATH_MSG_WARNING("Neither soft cluster nor soft track term has been supplied!");
      return StatusCode::SUCCESS;
    }
    static const SG::AuxElement::ConstAccessor<std::vector<ElementLink<IParticleContainer> > > acc_softConst("softConstituents");
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
      // Fill a vector with the soft constituents, if one was provided.
      // For now, only setting up to work with those corresponding to the jet constituents.
      // Can expand if needed.
      if(softConst && acc_softConst.isAvailable(*coreSoftClus)) {
	for(const auto& constit : acc_softConst(*coreSoftClus)) {
	  softConst->push_back(*constit);
	}
	ATH_MSG_DEBUG(softConst->size() << " soft constituents from core term");
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
      if(softConst && acc_softConst.isAvailable(*coreSoftTrk) && !m_doPFlow && !m_doSoftTruth) {
	for(const auto& constit : acc_softConst(*coreSoftTrk)) {
	  softConst->push_back(*constit);
	}
	ATH_MSG_DEBUG(softConst->size() << " soft constituents from trk core term");
      }
    }

    uniques.clear();
    bool originalInputs = jets->empty() ? false : !m_objLinkAcc.isAvailable(*jets->front());
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
        bool selected = (fabs(jet->eta())<2.4 && jet->pt()>m_CenJetPtCut) || (fabs(jet->eta())>=2.4 && jet->pt()>m_FwdJetPtCut );//jjj        
        bool JVT_reject(false);
	bool isMuFSRJet(false);
	
        if(doJetJVT) {
	  if(jet->pt()<m_JvtPtMax && fabs(jet->eta())<2.4) {
	    float jvt;
	    jet->getAttribute<float>(m_jetJvtMomentName,jvt);
	    JVT_reject = jvt<m_JvtCut;
	    ATH_MSG_VERBOSE("Jet " << (JVT_reject ? "fails" : "passes") <<" JVT selection");
	  }
        }
        bool hardJet(false);
        MissingETBase::Types::constvec_t calvec = assoc->overlapCalVec();
        bool caloverlap = false;
        caloverlap = calvec.ce()>0;
        ATH_MSG_DEBUG("Jet " << jet->index() << " is " << ( caloverlap ? "" : "non-") << "overlapping");

	xAOD::JetFourMom_t constjet;
	double constSF(1);
	if(m_jetConstitScaleMom.empty() && assoc->hasAlternateConstVec()){
	  constjet = assoc->getAlternateConstVec();
	} else {
	  constjet = jet->jetP4(m_jetConstitScaleMom);//grab a constituent scale added by the JetMomentTool/JetConstitFourMomTool.cxx
	  double denom = (assoc->hasAlternateConstVec() ? assoc->getAlternateConstVec() : jet->jetP4("JetConstitScaleMomentum")).E();
	  constSF = denom>1e-9 ? constjet.E()/denom : 0.;
	  ATH_MSG_VERBOSE("Scale const jet by factor " << constSF);
	  calvec *= constSF;
	}
	double jpx = constjet.Px();
        double jpy = constjet.Py();
        double jpt = constjet.Pt();
        double opx = jpx - calvec.cpx();
        double opy = jpy - calvec.cpy();

	MissingET* met_muonEloss(0);
	if(m_muEloss) {
	  // Get a term to hold the Eloss corrections
	  MissingETContainer* metCont = static_cast<MissingETContainer*>(metJet->container());
	  met_muonEloss = (*metCont)["MuonEloss"];
	  if(!met_muonEloss) {
	    ATH_MSG_WARNING("Attempted to apply muon Eloss correction, but corresponding MET term does not exist!");
	    return StatusCode::FAILURE;
	  }
	}

	float total_eloss(0);
	MissingETBase::Types::bitmask_t muons_selflags(0);
	std::vector<const xAOD::Muon*> muons_in_jet;
	if(m_useGhostMuons) { // for backwards-compatibility
	  if(acc_ghostMuons.isAvailable(*jet)) {
	    for(const auto& el : acc_ghostMuons(*jet)) {
	      if(el.isValid()) {
		muons_in_jet.push_back(static_cast<const xAOD::Muon*>(*el));
	      } else {
		ATH_MSG_WARNING("Invalid element link to ghost muon! Quitting.");
		return StatusCode::FAILURE;
	      }
	    }
	  } else {
	    ATH_MSG_WARNING("Ghost muons requested but not found!");
	    return StatusCode::FAILURE;
	  }
	} else { // should all be associated in r21
	  for(const auto& obj : assoc->objects()) {
	    if(obj->type()==xAOD::Type::Muon) {
	      const xAOD::Muon* mu_test(static_cast<const xAOD::Muon*>(obj));
	      ATH_MSG_VERBOSE("Muon " << mu_test->index() << " found in jet " << jet->index());
	      if(m_objLinkAcc.isAvailable(*mu_test)) mu_test = static_cast<const xAOD::Muon*>(*m_objLinkAcc(*mu_test));
	      if(MissingETComposition::objSelected(map,mu_test)) { // 
		muons_in_jet.push_back(mu_test);
		ATH_MSG_VERBOSE("Muon is selected by MET.");
	      }
	    }
	  }
	}
	for(const xAOD::Muon* mu_in_jet : muons_in_jet) {
	  float mu_Eloss = acc_Eloss(*mu_in_jet);

	  if(!JVT_reject && mu_in_jet) {
	    if (m_doRemoveMuonJets) {
	      // need to investigate how this is affected by the recording of muon clusters in the map
	      float mu_id_pt = mu_in_jet->trackParticle(xAOD::Muon::InnerDetectorTrackParticle) ? mu_in_jet->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)->pt() : 0.;
	      float jet_trk_sumpt = acc_trksumpt.isAvailable(*jet) && this->getPV() ? acc_trksumpt(*jet)[this->getPV()->index()] : 0.;
	      // missed the muon, so we should add it back
	      if(0.9999*mu_id_pt>jet_trk_sumpt)
		jet_trk_sumpt+=mu_id_pt;
	      float jet_trk_N = acc_trkN.isAvailable(*jet) && this->getPV() ? acc_trkN(*jet)[this->getPV()->index()] : 0.;
	      ATH_MSG_VERBOSE("Muon has ID pt " << mu_id_pt);
	      ATH_MSG_VERBOSE("Jet has pt " << jet->pt() << ", trk sumpt " << jet_trk_sumpt << ", trk N " << jet_trk_N);
	      bool jet_from_muon = mu_id_pt>1e-9 && jet_trk_sumpt>1e-9 && (jet->pt()/mu_id_pt < 2 && mu_id_pt/jet_trk_sumpt>0.8) && jet_trk_N<5;
	      if(jet_from_muon) {
		ATH_MSG_VERBOSE("Jet is from muon -- remove.");
		JVT_reject = true;
	      }
	    }

	    if (m_doSetMuonJetEMScale) {
	      // need to investigate how this is affected by the recording of muon clusters in the map
	      float mu_id_pt = mu_in_jet->trackParticle(xAOD::Muon::InnerDetectorTrackParticle) ? mu_in_jet->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)->pt() : 0.;
	      float jet_trk_sumpt = acc_trksumpt.isAvailable(*jet) && this->getPV() ? acc_trksumpt(*jet)[this->getPV()->index()] : 0.;
	      // missed the muon, so we should add it back
	      if(0.9999*mu_id_pt>jet_trk_sumpt)
		jet_trk_sumpt+=mu_id_pt;
	      float jet_trk_N = acc_trkN.isAvailable(*jet) && this->getPV() ? acc_trkN(*jet)[this->getPV()->index()] : 0.;
	      float jet_psE = acc_sampleE(*jet)[0] + acc_sampleE(*jet)[4];
	      bool jet_from_muon = jet_trk_sumpt>1e-9 && jet_trk_N<3 && mu_id_pt / jet_trk_sumpt > 0.8 && acc_emf(*jet)>0.9 && acc_width(*jet)<0.1 && jet_psE>2500;
	      ATH_MSG_VERBOSE("Muon has ID pt " << mu_id_pt);
	      ATH_MSG_VERBOSE("Jet has trk sumpt " << jet_trk_sumpt << ", trk N " << jet_trk_N << ", PS E " << jet_psE << ", width " << acc_width(*jet) << ", emfrac " << acc_emf(*jet));

	      if(jet_from_muon) {
		ATH_MSG_VERBOSE("Jet is from muon -- set to EM scale and subtract Eloss.");
		// Using constjet now because we focus on AntiKt4EMTopo.
		// Probably not a massive difference to LC, but PF needs some consideration
		ATH_MSG_VERBOSE("Jet e: " << constjet.E() << ", mu Eloss: " << mu_Eloss);
		float elosscorr = mu_Eloss >= constjet.e() ? 0. : 1.-mu_Eloss/constjet.e();
		// Effectively, take the unique fraction of the jet times the eloss-corrected fraction
		// This might in some cases oversubtract, but should err on the side of undercounting the jet contribution
		opx *= elosscorr;
		opy *= elosscorr;
		ATH_MSG_VERBOSE(" Jet eloss factor " << elosscorr << ", final pt: " << sqrt(opx*opx+opy*opy));
		// Don't treat this jet normally. Instead, just add to the Eloss term
		isMuFSRJet = true;
	      }
	    }
	  } // end muon-jet overlap-removal

	  //m_muEloss && 
	  if(!isMuFSRJet) {
	    switch(mu_in_jet->energyLossType()) {
	    case xAOD::Muon::Parametrized:
	    case xAOD::Muon::MOP:
	    case xAOD::Muon::Tail:
	    case xAOD::Muon::FSRcandidate:
	    case xAOD::Muon::NotIsolated:
	      // For now don't differentiate the behaviour
	      // Remove the Eloss assuming the parameterised value
	      // The correction is limited to the selected clusters
	      total_eloss += mu_Eloss;
	      muons_selflags |= (1<<assoc->findIndex(mu_in_jet));
	    }
	  }
	}
	ATH_MSG_VERBOSE("Muon selection flags: " << muons_selflags);
	ATH_MSG_VERBOSE("Muon total eloss: " << total_eloss);

	MissingETBase::Types::constvec_t mu_calovec;
	// borrowed from overlapCalVec
	for(size_t iKey = 0; iKey < assoc->sizeCal(); iKey++) {
	  bool selector = (muons_selflags & assoc->calkey()[iKey]);
	  if(selector) mu_calovec += assoc->calVec(iKey);
	  ATH_MSG_VERBOSE("This key: " << assoc->calkey()[iKey] << ", selector: " << selector);
	}
	ATH_MSG_VERBOSE("Mu calovec pt, no Eloss:   " << mu_calovec.cpt());
	if(m_muEloss) mu_calovec *= std::max(0.,1-(total_eloss/mu_calovec.ce()));
	ATH_MSG_VERBOSE("Mu calovec pt, with Eloss: " << mu_calovec.cpt());

	// re-add calo components of muons beyond Eloss correction
	ATH_MSG_VERBOSE("Jet " << jet->index() << " const pT before OR " << jpt);
	ATH_MSG_VERBOSE("Jet " << jet->index() << " const pT after OR " << sqrt(opx*opx+opy*opy));
	opx += mu_calovec.cpx();
	opy += mu_calovec.cpy();
        double opt = sqrt( opx*opx+opy*opy );
	ATH_MSG_VERBOSE("Jet " << jet->index() << " const pT diff after OR readding muon clusters " << opt-jpt);
        double uniquefrac = 1. - (calvec.ce() - mu_calovec.ce()) / constjet.E();
        ATH_MSG_VERBOSE( "Jet constscale px, py, pt, E = " << jpx << ", " << jpy << ", " << jpt << ", " << constjet.E() );
        ATH_MSG_VERBOSE( "Jet overlap E = " << calvec.ce() - mu_calovec.ce() );
        ATH_MSG_VERBOSE( "Jet OR px, py, pt, E = " << opx << ", " << opy << ", " << opt << ", " << constjet.E() - calvec.ce() );
	  
	if(isMuFSRJet) {
	  if(met_muonEloss) {
	    met_muonEloss->add(opx,opy,opt);
	  } else {
	    ATH_MSG_WARNING("Attempted to apply muon Eloss correction, but corresponding MET term does not exist!");
	    return StatusCode::FAILURE;
	  }
	} else {
	  if(selected && !JVT_reject) {
	    if(!caloverlap) {
	      // add jet full four-vector
	      hardJet = true;
	      if (!tracksForHardJets) {
		if(m_doConstJet) {
		  metJet->add(jpx,jpy,jpt);
		} else {*metJet += jet;}
	      }
	    } else {
	      // check unique fraction
	      if(uniquefrac>m_jetMinEfrac && opt>m_jetMinWeightedPt) {
		// add jet corrected for overlaps
		hardJet = true;
		if(!tracksForHardJets) {
		  if(m_jetCorrectPhi) {
		    if (m_doConstJet) metJet->add(opx,opy,opt);
		    else {
		      double jesF = jet->pt() / jpt;
		      metJet->add(opx*jesF,opy*jesF,opt*jesF);
		    }
		  } else {
		    if (m_doConstJet) metJet->add(uniquefrac*jpx,uniquefrac*jpy,uniquefrac*jpt);
		    else metJet->add(uniquefrac*jet->px(),uniquefrac*jet->py(),uniquefrac*jet->pt());
		  }
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
	      if (fabs(jet->eta())<2.5 || !(coreSoftClus->source()&MissingETBase::Source::Central))
		metSoftClus->add(opx,opy,opt);

	      // Fill a vector with the soft constituents, if one was provided.
	      // For now, only setting up to work with those corresponding to the jet constituents.
	      // Can expand if needed.
	      // This ignores overlap removal.
	      //
	      if(softConst) {
		for(size_t iConst=0; iConst<jet->numConstituents(); ++iConst) {
		  const IParticle* constit = jet->rawConstituent(iConst);
		  softConst->push_back(constit);
		}
	      }
	    }
	  } // hard jet or CST

	  if(metSoftTrk && (!hardJet || tracksForHardJets)) {
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
	    else if (fabs(jet->eta())<2.5 || !(coreSoftTrk->source()&MissingETBase::Source::Central)) {
	      metSoftTrk->add(opx,opy,opt);
	      if(softConst && !m_doPFlow && !m_doSoftTruth) {
		std::vector<const IParticle*> jettracks;
		jet->getAssociatedObjects<IParticle>(xAOD::JetAttribute::GhostTrack,jettracks);
		for(size_t iConst=0; iConst<jettracks.size(); ++iConst) {
		  const TrackParticle* pTrk = static_cast<const TrackParticle*>(jettracks[iConst]);
		  if (acceptTrack(pTrk,pv)) softConst->push_back(pTrk);
		}
	      }
	    }
	  } // soft track

	} // is not from muon FSR
      } // association exists
      else {
	ATH_MSG_WARNING( "WARNING: Jet without association found!" );
      }
      if(tracksForHardJets) m_objUsedDec(*jet)=isUsedinMET;
    } // jet loop

    if(softConst) ATH_MSG_DEBUG(softConst->size() << " soft constituents from core term + jets");

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
	float total_eloss(0.);
	MissingETBase::Types::bitmask_t muons_selflags(0);
        MissingETBase::Types::constvec_t calvec = assoc->overlapCalVec();
        double opx = calvec.cpx();
        double opy = calvec.cpy();
        double osumpt = calvec.sumpt();
	for(const auto& obj : assoc->objects()) {
	  if(obj->type()==xAOD::Type::Muon) {
	    const xAOD::Muon* mu_test(static_cast<const xAOD::Muon*>(obj));
	    if(m_objLinkAcc.isAvailable(*mu_test)) mu_test = static_cast<const xAOD::Muon*>(*m_objLinkAcc(*mu_test));
	    if(MissingETComposition::objSelected(map,mu_test)) { // 
	      float mu_Eloss = acc_Eloss(*mu_test);

	      switch(mu_test->energyLossType()) {
	      case xAOD::Muon::Parametrized:
	      case xAOD::Muon::MOP:
	      case xAOD::Muon::Tail:
	      case xAOD::Muon::FSRcandidate:
	      case xAOD::Muon::NotIsolated:
		// For now don't differentiate the behaviour
		// Remove the Eloss assuming the parameterised value
		// The correction is limited to the selected clusters
		total_eloss += mu_Eloss;
		muons_selflags |= (1<<assoc->findIndex(mu_test));
	      }
	      ATH_MSG_VERBOSE("Mu index " << mu_test->index());
	    }
	  }
	}
	ATH_MSG_VERBOSE("Mu selection flags " << muons_selflags);
	ATH_MSG_VERBOSE("Mu total eloss " << total_eloss);

	MissingETBase::Types::constvec_t mu_calovec;
	// borrowed from overlapCalVec
	for(size_t iKey = 0; iKey < assoc->sizeCal(); iKey++) {
	  bool selector = (muons_selflags & assoc->calkey()[iKey]);
	  ATH_MSG_VERBOSE("This key: " << assoc->calkey()[iKey] << ", selector: " << selector
		       << " this calvec E: " << assoc->calVec(iKey).ce());
	  if(selector) mu_calovec += assoc->calVec(iKey);
	}
	if(m_muEloss) mu_calovec *= std::max(0.,1-(total_eloss/mu_calovec.ce()));
	opx += mu_calovec.cpx();
	opy += mu_calovec.cpy();
	osumpt += mu_calovec.sumpt();
	ATH_MSG_VERBOSE("Mu cluster sumpt " << mu_calovec.sumpt());


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
      ATH_MSG_WARNING("Incomplete association map received. Cannot rebuild MET.");
      ATH_MSG_WARNING("Note: METMaker should only be run on events containing at least one PV");
      return StatusCode::SUCCESS;
    }
    if(!collection->empty()) {
      bool originalInputs = !m_objLinkAcc.isAvailable(*collection->front());
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

  // Accept Track
  ////////////////
  bool METMaker::acceptTrack(const xAOD::TrackParticle* trk, const xAOD::Vertex* vx) const
  {
    const Root::TAccept& accept = m_trkseltool->accept( *trk, vx );
    return accept;
  }

  const xAOD::Vertex* METMaker::getPV() const {
    const xAOD::VertexContainer *vxCont = 0;
    const xAOD::Vertex *pv = 0;
    if( evtStore()->retrieve(vxCont,"PrimaryVertices").isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve primary vertex container PrimaryVertices");
    } else if(vxCont->empty()) {
      ATH_MSG_WARNING("Event has no primary vertices!");
    } else {
      ATH_MSG_DEBUG("Successfully retrieved primary vertex container");
      for(const auto& vx : *vxCont) {
	if(vx->vertexType()==xAOD::VxType::PriVtx)
	  {pv = vx; break;}
      }
    }
    return pv;
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

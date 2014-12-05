///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METRebuilder.cxx 
// Implementation file for class METRebuilder
// Author: T.J.Khoo<khoo@cern.ch>
/////////////////////////////////////////////////////////////////// 

// METUtilities includes
#include "METUtilities/METRebuilder.h"

// MET EDM
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETComponentMap.h"

// EDM includes
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetAttributes.h"

// Egamma EDM
#include "xAODEgamma/ElectronxAODHelpers.h"

// Tracking EDM
#include "xAODTracking/TrackParticle.h"

// Track errors
#include "EventPrimitives/EventPrimitivesHelpers.h"

namespace met {

  using std::vector;

  using xAOD::MissingET;
  using xAOD::MissingETContainer;
  using xAOD::MissingETComponent;
  using xAOD::MissingETComponentMap;
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
  METRebuilder::METRebuilder(const std::string& name) : 
    AsgTool(name),
    m_doEle(false),
    m_doGamma(false),
    m_doTau(false),
    m_doMuon(false),
    m_rebuildEle(false),
    m_rebuildGamma(false),
    m_rebuildTau(false),
    m_rebuildMuon(false),
    m_objLinkAcc("originalObjectLink"),
    m_trkUsedDec("usedByMET"),
    m_pureTrkSoft(true)
  {
    //
    // Property declaration
    // 
    declareProperty( "EleColl",         m_eleColl    = "ElectronCollection" );
    declareProperty( "GammaColl",       m_gammaColl  = "PhotonCollection"   );
    declareProperty( "TauColl",         m_tauColl    = "TauRecContainer"    );
    declareProperty( "JetColl",         m_jetColl    = "AntiKt4LCTopoJets"  );
    declareProperty( "MuonColl",        m_muonColl   = "Muons"              );
    //
    declareProperty( "EleTerm",         m_eleTerm    = "RefEle"             );
    declareProperty( "GammaTerm",       m_gammaTerm  = "RefGamma"           );
    declareProperty( "TauTerm",         m_tauTerm    = "RefTau"             );
    declareProperty( "JetTerm",         m_jetTerm    = "RefJet"             );
    declareProperty( "MuonTerm",        m_muonTerm   = "Muons"              );
    declareProperty( "SoftTerm",        m_softTerm   = "PVSoftTrk"          );
    //
    declareProperty( "InputMap",        m_inputMap   = "METMap_RefFinal"    );
    declareProperty( "OutputContainer", m_outMETCont = "MET_MyRefFinal"     );
    declareProperty( "OutputTotal",     m_outMETTerm = "Final"              );
    declareProperty( "WarnIfDuplicate", m_warnOfDupes = true                );

    // migrate to new tool at some point
    declareProperty( "CalibJetPtCut",   m_jetPtCut   = 20e3                 );
    declareProperty( "DoJetJVFCut",     m_jetDoJvf   = true                 );
    declareProperty( "CalibJetJvfCut",  m_jetJvfCut  = 0.25                 );
    declareProperty( "SoftJetScale",  m_softJetScale = "JetConstitScaleMomentum" );

    // should be in a track tool -- also should use track isolation decorations
    declareProperty( "DoTrackPVSel",    m_trk_doPVsel = true                );
    declareProperty( "VertexColl",      m_vtxColl    = "PrimaryVertices"    );
    declareProperty( "ClusterColl",     m_clusColl   = "CaloCalTopoCluster" );
    declareProperty( "TrackD0Max",      m_trk_d0Max = 1.5                   );
    declareProperty( "TrackZ0Max",      m_trk_z0Max = 1.5                   );

  }

  // Destructor
  ///////////////
  METRebuilder::~METRebuilder()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METRebuilder::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    if( m_inputMap.size()==0 ) {
      ATH_MSG_FATAL("Input MissingETComponentMap name must be provided.");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO ("Input MET Map: " << m_inputMap);

    if( m_outMETCont.size()==0 ) {
      ATH_MSG_FATAL("Output MissingETContainer name must be provided.");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO ("Output MET Container: " << m_outMETCont);

    ATH_MSG_INFO ("Configured to rebuild following MET Terms:");
    if( m_eleTerm!="" ) {
      m_doEle = true;
      if( m_eleColl!="" ) {
	m_rebuildEle = true;
	ATH_MSG_INFO("  Electrons: " << m_eleColl
		     << " > " << m_eleTerm );
      }
    }
    if( m_gammaTerm!="" ) {
      m_doGamma = true;
      if( m_gammaColl!="" ) {
	m_rebuildGamma = true;
	ATH_MSG_INFO("  Photons:   " << m_gammaColl
		     << " > " << m_gammaTerm );
      }
    }
    if( m_tauTerm!="" ) {
      m_doTau = true;
      if( m_tauColl!="" ) {
	m_rebuildTau = true;
	ATH_MSG_INFO("  Taus:      " << m_tauColl
		     << " > " << m_tauTerm );
      }
    }
    if( m_muonTerm!="" ) {
      m_doMuon = true;
      if( m_muonColl!="" ) {
	m_rebuildMuon = true;
	ATH_MSG_INFO("  Muons:     " << m_muonColl
		     << " > " << m_muonTerm );
      }
    }
    if( m_jetColl!="" && m_jetTerm!="") {
      ATH_MSG_INFO("  Jets:      " << m_jetColl
		   << " > " << m_jetTerm );
    } else {
      ATH_MSG_FATAL("Error in configuration -- jet input and term keys must both be specified.");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO ("  Soft:      " << m_softTerm);

    m_trkUsedDec = SG::AuxElement::Decorator<char>("usedBy"+m_outMETCont);

    m_doTracks = (m_softTerm == "PVSoftTrk");

    m_pureTrkSoft = (m_softJetScale == "JetTrackScale");

    return StatusCode::SUCCESS;
  }

  StatusCode METRebuilder::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  StatusCode METRebuilder::execute()
  {
    ATH_MSG_DEBUG ( name() << " in execute...");

    const MissingETComponentMap* metMap = 0;
    if( evtStore()->retrieve(metMap, m_inputMap).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve MissingETComponentMap: " << m_inputMap);
      return StatusCode::SUCCESS;
    }

    if( evtStore()->contains<MissingETContainer>(m_outMETCont) ) {
      if(m_warnOfDupes)
	{ ATH_MSG_WARNING("MET container " << m_outMETCont << " already in StoreGate"); }
      return StatusCode::SUCCESS;
    }

    // Create a MissingETContainer with its aux store
    MissingETContainer* outCont = new MissingETContainer();
    if( evtStore()->record(outCont, m_outMETCont).isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETContainer: " << m_outMETCont);
      return StatusCode::SUCCESS;
    }
    MissingETAuxContainer* metAuxCont = new MissingETAuxContainer();
    if( evtStore()->record(metAuxCont, m_outMETCont+"Aux.").isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETAuxContainer: " << m_outMETCont+"Aux.");
      return StatusCode::SUCCESS;
    }
    outCont->setStore(metAuxCont);
       
    if(m_doEle) {
      if(m_rebuildEle) {
	const xAOD::ElectronContainer* elec = 0;
	if( evtStore()->retrieve(elec, m_eleColl).isFailure() ) {
	  ATH_MSG_WARNING("Unable to retrieve ElectronContainer: " << m_eleColl);
	  return StatusCode::SUCCESS;
	}
	ATH_CHECK( rebuildMET(m_eleTerm, outCont, elec, metMap, m_doTracks) );
      } else {
	MissingET* metele = new MissingET();
	const MissingET* metele_ref = MissingETComposition::getMissingET(metMap,m_eleTerm);
	outCont->push_back(metele);
	metele->setName(metele_ref->name());
	metele->setSource(metele_ref->source());
	*metele += *metele_ref;
      }
    }

    if(m_doGamma) {
    if(m_rebuildGamma) {
      const xAOD::PhotonContainer* gamma = 0;
      if( evtStore()->retrieve(gamma, m_gammaColl).isFailure() ) {
	ATH_MSG_WARNING("Unable to retrieve GammaContainer: " << m_gammaColl);
	return StatusCode::SUCCESS;
      }
      ATH_CHECK( rebuildMET(m_gammaTerm, outCont, gamma, metMap, m_doTracks) );
    } else {
	MissingET* metgamma = new MissingET();
	const MissingET* metgamma_ref = MissingETComposition::getMissingET(metMap,m_gammaTerm);
	outCont->push_back(metgamma);
	metgamma->setName(metgamma_ref->name());
	metgamma->setSource(metgamma_ref->source());
	*metgamma += *metgamma_ref;
      }
    }


    if(m_doTau) {
    if(m_rebuildTau) {
      const xAOD::TauJetContainer* taujet = 0;
      if( evtStore()->retrieve(taujet, m_tauColl).isFailure() ) {
	ATH_MSG_WARNING("Unable to retrieve TauJetContainer: " << m_tauColl);
	return StatusCode::SUCCESS;
      }
      ATH_CHECK( rebuildMET(m_tauTerm, outCont, taujet, metMap, m_doTracks) );
    } else {
	MissingET* mettau = new MissingET();
	const MissingET* mettau_ref = MissingETComposition::getMissingET(metMap,m_tauTerm);
	outCont->push_back(mettau);
	mettau->setName(mettau_ref->name());
	mettau->setSource(mettau_ref->source());
	*mettau += *mettau_ref;
      }
    }


    if(m_doMuon) {
    if(m_rebuildMuon) {
      // May need implementation of Eloss correction
      // Place in separate tool (?)
      const xAOD::MuonContainer* muon = 0;
      if( evtStore()->retrieve(muon, m_muonColl).isFailure() ) {
	ATH_MSG_WARNING("Unable to retrieve MuonContainer: " << m_muonColl);
	return StatusCode::SUCCESS;
      }
      ATH_CHECK( rebuildMET(m_muonTerm, outCont, muon, metMap, m_doTracks) );
    } else {
	MissingET* metmuon = new MissingET();
	const MissingET* metmuon_ref = MissingETComposition::getMissingET(metMap,m_muonTerm);
	outCont->push_back(metmuon);
	metmuon->setName(metmuon_ref->name());
	metmuon->setSource(metmuon_ref->source());
	*metmuon += *metmuon_ref;
      }
    }


    // Implementation of the jet/soft term rebuilding
    // Place in separate tool (?)
    const xAOD::JetContainer* jet = 0;
    if( evtStore()->retrieve(jet, m_jetColl).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve JetContainer: " << m_jetColl);
      return StatusCode::SUCCESS;
    }
    ATH_CHECK( rebuildJetMET(m_jetTerm, m_softTerm, outCont, jet, metMap, m_doTracks) );

    ATH_CHECK( buildMETSum(m_outMETTerm, outCont) );

    return StatusCode::SUCCESS;
  }

  // **** Rebuild generic MET term ****

  StatusCode METRebuilder::rebuildMET(const std::string& metKey,
				      xAOD::MissingETContainer* metCont,
				      const xAOD::IParticleContainer* collection,
				      const xAOD::MissingETComponentMap* metMap,
				      bool doTracks) {
    ATH_MSG_DEBUG("Rebuild MET term: " << metKey);
    const MissingETComponent* component = MissingETComposition::getComponent(metMap,metKey);
    if(!component) {
      ATH_MSG_WARNING("Could not find METComponent for " << metKey << " in MET Map!");
      return StatusCode::SUCCESS;
    }
    MissingET* met = new MissingET(0.,0.,0.,metKey,component->metObject()->source());
    metCont->push_back(met);
    return rebuildMET(met,collection,component,doTracks);
  }

  StatusCode METRebuilder::rebuildMET(xAOD::MissingET* met,
				      const xAOD::IParticleContainer* collection,
				      const xAOD::MissingETComponent* component,
				      bool doTracks) {

    if(component->size()==0) return StatusCode::SUCCESS;

    ATH_MSG_VERBOSE("Rebuilding MET term " << component->metObject()->name());

    const IParticleContainer* testCollection = dynamic_cast<const IParticleContainer*>(component->objects().front()->container());
    bool originalInputs = (testCollection == collection);
    bool matchCollection = true;
    if(collection->size()>0) {
      // Consistency test: check that the collection supplied is the original one
      // used for MET reconstruction, or is derived from this collection
      if(!originalInputs) {
	const IParticle* pObj = collection->front();
	if(!m_objLinkAcc.isAvailable(*pObj)) {
	  ATH_MSG_WARNING("Modified container provided without originalObjectLink -- cannot proceed.");
	  matchCollection = false;
	} else {
	  const IParticleContainer* sourceCollection = dynamic_cast<const IParticleContainer*>((*m_objLinkAcc(*pObj))->container());
	  matchCollection = (sourceCollection == testCollection);
	}
      }
      if(!matchCollection) {
	ATH_MSG_WARNING("Problem with input object container -- skipping this term.");
	return StatusCode::SUCCESS;
      }
    }

    // Method flow:
    // 1. Loop over the objects in the collection
    // 2. Find them or their originals in the METComponent
    // 3. Add to the MET term with appropriate weights
    
    for( IParticleContainer::const_iterator iObj=collection->begin();
	 iObj!=collection->end(); ++iObj ) {

      const IParticle* pObj = *iObj;
      // check if this is a copy - if so, get the original object pointer
      if(!originalInputs) pObj = *m_objLinkAcc(*pObj);

      if(component->findIndex(pObj) != MissingETBase::Numerical::invalidIndex()) {
	MissingETBase::Types::weight_t objWeight = component->weight(pObj);
	ATH_MSG_VERBOSE( "Object with pt " << (*iObj)->pt() << " has weight " << objWeight.wet() );

	if(doTracks) {
	  associateTracks(*iObj);
	}

	met->add((*iObj)->pt()*cos((*iObj)->phi())*objWeight.wpx(),
		 (*iObj)->pt()*sin((*iObj)->phi())*objWeight.wpy(),
		 (*iObj)->pt()*objWeight.wet());
      } // used object in MET
      else {
	ATH_MSG_VERBOSE( "Object with pt " << (*iObj)->pt() << " not found." );
      }
    }

    ATH_MSG_DEBUG( "Original " << component->metObject()->name() << " MET --"
		   << " mpx: " << component->metObject()->mpx()
		   << " mpy: " << component->metObject()->mpy()
		   );
    ATH_MSG_DEBUG( "Rebuilt " << component->metObject()->name() << " MET  --"
		   << " mpx: " << met->mpx()
		   << " mpy: " << met->mpy()
		   );

    return StatusCode::SUCCESS;
  }

  // **** Rebuild jet & soft MET terms ****

  StatusCode METRebuilder::rebuildJetMET(const std::string& jetKey,
					 const std::string& softKey,
					 xAOD::MissingETContainer* metCont,
					 const xAOD::JetContainer* jets,
					 const xAOD::MissingETComponentMap* metMap,
					 bool doTracks) {
    const MissingETComponent* component = MissingETComposition::getComponent(metMap,jetKey);
    if(!component) {
      ATH_MSG_WARNING("Could not find METComponent for " << jetKey << " in MET Map!");
      return StatusCode::SUCCESS;
    }
    MissingET* metJet = new MissingET(0.,0.,0.,jetKey,component->metObject()->source());
    metCont->push_back(metJet);
    const MissingET* oldSoft = xAOD::MissingETComposition::getMissingET(metMap,softKey);
    if(!component) {
      ATH_MSG_WARNING("Could not find METComponent for " << softKey << " in MET Map!");
      return StatusCode::SUCCESS;
    }
    // need -1 because this assumes adding a particle, not a MET
    // copy constructor needs correcting.
    MissingET* metSoft = new MissingET();
    metCont->push_back(metSoft);
    metSoft->setName(oldSoft->name());
    metSoft->setSource(oldSoft->source());
    *metSoft += *oldSoft;
    return rebuildJetMET(metJet,metSoft,oldSoft,jets,component,doTracks);
  }

  StatusCode METRebuilder::rebuildJetMET(xAOD::MissingET* metJet,
					 xAOD::MissingET* metSoft,
					 const xAOD::MissingET* oldSoft,
					 const xAOD::JetContainer* jets,
					 const xAOD::MissingETComponent* component,
					 bool doTracks) {

    if(component->size()==0) return StatusCode::SUCCESS;

    const xAOD::VertexContainer* vtx = 0;
    if(m_trk_doPVsel && doTracks) {
      if( evtStore()->retrieve( vtx, m_vtxColl).isFailure() ) {
        ATH_MSG_WARNING("Unable to retrieve input primary vertex container");
        return StatusCode::SUCCESS;
      }
      if(vtx->size()>0) {
	ATH_MSG_DEBUG("Main primary vertex has z = " << (*vtx)[0]->z());
      } else{
	ATH_MSG_WARNING("Event has no primary vertices");
	return StatusCode::SUCCESS;
      }
    }

    const IParticleContainer* testCollection = dynamic_cast<const IParticleContainer*>(component->objects().front()->container());
    const IParticleContainer* collcast = dynamic_cast<const IParticleContainer*>(jets);
    bool originalInputs = (testCollection == collcast);
    bool matchCollection = true;
    if(jets->size()>0) {
      // Consistency test: check that the collection supplied is the original one
      // used for MET reconstruction, or is derived from this collection
      if(!originalInputs) {
	const IParticle* pJet = jets->front();
	if(!m_objLinkAcc.isAvailable(*pJet)) {
	  ATH_MSG_WARNING("Modified container provided without originalObjectLink -- cannot proceed.");
	  matchCollection = false;
	} else {
	  const IParticleContainer* sourceCollection = dynamic_cast<const IParticleContainer*>((*m_objLinkAcc(*pJet))->container());
	  matchCollection = (sourceCollection == testCollection);
	}
      }
    }
    if(!matchCollection) {
      ATH_MSG_WARNING("Problem with input object container -- skipping these terms.");
      return StatusCode::SUCCESS;
    }
    // 1. Loop over the jets in the collection
    // 2. Find them or their originals in the METComponent
    // 3. Add to the MET term with appropriate weights
    for( JetContainer::const_iterator iJet=jets->begin();
	 iJet!=jets->end(); ++iJet ) {

      const xAOD::IParticle* pJet = *iJet;
      if(!originalInputs) pJet = *m_objLinkAcc(*pJet);

      if(component->findIndex(pJet) != MissingETBase::Numerical::invalidIndex()) {

	MissingETBase::Types::weight_t jetWeight = component->weight(pJet);
	bool passJVF = true;
	if(m_jetDoJvf) {
	  vector<float> jvf;
	  (*iJet)->getAttribute<vector<float> >(xAOD::JetAttribute::JVF,jvf);
	  passJVF = (*iJet)->pt()>50e3 || fabs((*iJet)->eta())>2.4 || fabs(jvf[0])>m_jetJvfCut;
	  ATH_MSG_VERBOSE("Jet with pt " << (*iJet)->pt() << " has jvf = " << jvf[0]);
	}

	if((*iJet)->pt()>m_jetPtCut && passJVF) {
	  
	  ATH_MSG_VERBOSE("Retain jet with pt " << (*iJet)->pt() << " at full scale.");
	  
	  metJet->add((*iJet)->px()*jetWeight.wpx(),
		      (*iJet)->py()*jetWeight.wpy(),
		      (*iJet)->pt()*jetWeight.wet());
	} // minimum pt cut for jet calibration
	else {
	  double trkjetpx(0), trkjetpy(0), trkjetpt(0);
	  if(doTracks) {
	    ATH_MSG_VERBOSE("Add tracks from jet with pt " << (*iJet)->pt());
	    vector<const TrackParticle*> jettracks = (*iJet)->getAssociatedObjects<TrackParticle>(xAOD::JetAttribute::GhostTrack);
	    ATH_MSG_VERBOSE("Got jet tracks");
	    for(vector<const TrackParticle*>::const_iterator iTrk = jettracks.begin();
		iTrk!=jettracks.end(); ++iTrk) {
	      if(!*iTrk) continue;
	      // duplicate ST track selection -- should be in a tool
	      if(fabs((*iTrk)->pt())>500/*MeV*/ && fabs((*iTrk)->eta())<2.5) {
		uint8_t nPixHits(0), nSctHits(0);
		(*iTrk)->summaryValue(nPixHits,xAOD::numberOfPixelHits);
		(*iTrk)->summaryValue(nSctHits,xAOD::numberOfSCTHits);
		if(nPixHits>=1 && nSctHits>=6) {
		  bool badTrack = false;
		  if( (fabs((*iTrk)->eta())<1.5 && (*iTrk)->pt()>200e3) ||
		      (fabs((*iTrk)->eta())>=1.5 && (*iTrk)->pt()>120e3) ) {
		    // Get relative error on qoverp
		    float Rerr = Amg::error((*iTrk)->definingParametersCovMatrix(),4)/fabs((*iTrk)->qOverP());
		    // Simplified cut -- remove tracks that are more energetic than the jet
		    if(Rerr>0.4 || (*iTrk)->pt()>2*(*iJet)->pt()) badTrack = true;
		  } // additional cuts against high pt mismeasured tracks
		  bool uniqueTrack = true;
		  uniqueTrack = !m_trkUsedDec(**iTrk);
		  if(!badTrack && uniqueTrack &&
		     (!m_trk_doPVsel||isPVTrack(*iTrk,(*vtx)[0]))) {
		    ATH_MSG_VERBOSE("  + track with pt " << (*iTrk)->pt());
		    trkjetpx += (*iTrk)->pt()*cos((*iTrk)->phi());
		    trkjetpy += (*iTrk)->pt()*sin((*iTrk)->phi());
		    trkjetpt += (*iTrk)->pt();
		} else { ATH_MSG_VERBOSE("  - track failed badtrack/uniqueness/PV"); }
	      } // PIX & SCT hits
	      else { ATH_MSG_VERBOSE("  - track failed hit requirement"); }
	    } // pt, eta
	    else { ATH_MSG_VERBOSE("  - track failed pt/eta"); }
	  } // track loop
	} // track-based soft term
	if(m_pureTrkSoft){
	  metSoft->add(trkjetpx,trkjetpy,trkjetpt);
	} else {
	  // just add the weighted constituent-scale jet
	    xAOD::JetFourMom_t jetP = (*iJet)->jetP4(m_softJetScale);
	    ATH_MSG_VERBOSE("Soft jet pt = " << jetP.Pt() << ", track pt = " << trkjetpt);
	    if(trkjetpt>jetP.Pt() || !passJVF) { // use tracks if higher scale than calo jet or fail JVF cut in central region
	      ATH_MSG_VERBOSE("Add jet with pt " << (*iJet)->pt()
			      << " at track scale (pt = " << trkjetpt << ").");
	      metSoft->add(trkjetpx,trkjetpy,trkjetpt);
	    } else {
	      ATH_MSG_VERBOSE("Add jet with pt " << (*iJet)->pt()
			      << " at constituent scale (pt = " << jetP.Pt() << ").");
	      metSoft->add(jetP.Px()*jetWeight.wpx(),
			   jetP.Py()*jetWeight.wpy(),
			   jetP.Pt()*jetWeight.wet());
	    } // otherwise, use calo jet at chosen scale
	  } // cluster-based soft term
	} // jets below threshold should be added to the soft terms
      } // used jet in MET
    }

    ATH_MSG_DEBUG( "Original jet MET --"
		   << " mpx: " << component->metObject()->mpx()
		   << " mpy: " << component->metObject()->mpy()
		   );
    ATH_MSG_DEBUG( "Rebuilt jet MET  --"
		   << " mpx: " << metJet->mpx()
		   << " mpy: " << metJet->mpy()
		   );

    ATH_MSG_DEBUG("Original MET Soft --"
		  << " mpx: " << oldSoft->mpx()
		  << " mpy: " << oldSoft->mpy());

    ATH_MSG_DEBUG( "Rebuilt MET soft --"
		   << " mpx: " << metSoft->mpx()
		   << " mpy: " << metSoft->mpy()
		   );

    return StatusCode::SUCCESS;
  }

  // **** Sum up MET terms ****

  StatusCode METRebuilder::buildMETSum(const std::string& totalName,
				       xAOD::MissingETContainer* metCont)
  {
    ATH_MSG_DEBUG("Build MET total: " << totalName);

    MissingET* metFinal = new MissingET(0.,0.,0.,"Final",MissingETBase::Source::total());
    metCont->push_back(metFinal);

    for(MissingETContainer::const_iterator iMET=metCont->begin();
	iMET!=metCont->end(); ++iMET) {
      if(*iMET==metFinal) continue;
      *metFinal += **iMET;
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

  // Implement for now, but should move to common tools when possible
  bool METRebuilder::isPVTrack(const xAOD::TrackParticle* trk,
			       const xAOD::Vertex* pv) const
  {

    if(trk->d0()>m_trk_d0Max) return false;
    if(fabs(trk->z0()+trk->vz() - pv->z()) > m_trk_z0Max) return false;

    return true;
  }

  void METRebuilder::associateTracks(const xAOD::IParticle* obj) {
    
    if(obj->type()==xAOD::Type::Electron) {
      const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*>(obj);
      for(size_t iTrk=0; iTrk<el->nTrackParticles(); ++iTrk) {
	const TrackParticle* eltrk = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(el->trackParticle(iTrk));
	if(eltrk) m_trkUsedDec(*eltrk) = true;
      }
    }
    if(obj->type()==xAOD::Type::Photon) {
      const xAOD::Photon* ph = dynamic_cast<const xAOD::Photon*>(obj);
      for(size_t iVtx=0; iVtx<ph->nVertices(); ++iVtx) {
	const xAOD::Vertex* phvx = ph->vertex(iVtx);

	if(phvx) {
	  for(size_t iTrk=0; iTrk<phvx->nTrackParticles(); ++iTrk) {
	    const TrackParticle* phtrk = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(phvx->trackParticle(iTrk));
	    if(phtrk) m_trkUsedDec(*phtrk) = true;
	  }
	}
      }
    }
    if(obj->type()==xAOD::Type::Tau) {
      const xAOD::TauJet* tau = dynamic_cast<const xAOD::TauJet*>(obj);
      // now find associated tracks
      for(size_t iTrk=0; iTrk<tau->nTracks(); ++iTrk) {
	m_trkUsedDec(*tau->track(iTrk)) = true;
      }
      for(size_t iTrk=0; iTrk<tau->nOtherTracks(); ++iTrk) {
	const xAOD::TrackParticle* trk = tau->otherTrack(iTrk);
	double dR = (*tau->jetLink())->p4().DeltaR(trk->p4());
	if(dR<0.2) {
	  m_trkUsedDec(*trk) = true;
	}
      }
    }
    if(obj->type()==xAOD::Type::Muon) {
      const xAOD::Muon* mu = dynamic_cast<const xAOD::Muon*>(obj);
      if(mu->inDetTrackParticleLink().isValid()) {
	m_trkUsedDec(**mu->inDetTrackParticleLink()) = true;
      }
    }
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

} //> end namespace met

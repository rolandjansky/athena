/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METAssocAlg.cxx

#include "METAssocAlg.h"

#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "xAODMissingET/MissingETAuxAssociationMap.h"

#include "xAODEgamma/EgammaxAODHelpers.h"
#include "AthContainers/ConstDataVector.h"

// Track errors
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "xAODEventInfo/EventInfo.h"

using std::string;
using std::vector;
using namespace xAOD;

using std::string;

namespace met {

  static inline bool greaterPt(const xAOD::IParticle* part1, const xAOD::IParticle* part2) {
    return part1->pt()>part2->pt();
  }
  static inline bool greaterPtPFO(const xAOD::PFO* part1, const xAOD::PFO* part2) {
    if (part1->charge()==0 && part2->charge()!=0) return false;
    if (part1->charge()!=0 && part2->charge()==0) return true;
    if (part1->charge()==0 && part2->charge()==0) return part1->ptEM()>part2->ptEM();
    return part1->pt()>part2->pt();
  }

  //**********************************************************************

  METAssocAlg::METAssocAlg(const std::string& name,
			   ISvcLocator* pSvcLocator )
    : ::AthAlgorithm( name, pSvcLocator ) {
    declareProperty( "METContainer",         m_contname  = "MET_Core" );
    declareProperty( "METAssociationMap",    m_mapname   = "METAssoc" );

    declareProperty( "JetColl",              m_jetcoll   = "AntiKt4LCTopoJets"      );
    declareProperty( "EleColl",              m_elecoll   = "ElectronCollection"     );
    declareProperty( "GammaColl",            m_gammacoll = "PhotonCollection"       );
    declareProperty( "TauColl",              m_taucoll   = "TauRecContainer"        );
    declareProperty( "MuonColl",             m_muoncoll  = "Muons"                  );
    declareProperty( "PrimVxColl",           m_pvcoll    = "PrimaryVertices"        );
    declareProperty( "TrkColl",              m_trkcoll   = "InDetTrackParticles"    );
    declareProperty( "ClusColl",             m_clcoll    = "CaloCalTopoCluster"     );
    declareProperty( "PFlow",                m_pflow     = false                    );
    declareProperty( "PFOTool",              m_pfotool                              );
    declareProperty( "TrackSelectorTool",     m_trkseltool );
  }

  //**********************************************************************

  METAssocAlg::~METAssocAlg() { }

  //**********************************************************************

  StatusCode METAssocAlg::initialize() {
    ATH_MSG_INFO("Initializing " << name() << "...");
  
    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode METAssocAlg::finalize() {
    ATH_MSG_INFO ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  //**********************************************************************

  StatusCode METAssocAlg::execute() { 
    ATH_MSG_VERBOSE("Executing " << name() << "...");
    // Loop over tools.

    // Create a MissingETAssociationMap with its aux store
    MissingETAssociationMap* metMap = new MissingETAssociationMap();
    if( evtStore()->record(metMap, m_mapname).isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETAssociationMap: " << m_mapname);
      return StatusCode::SUCCESS;
    }
    MissingETAuxAssociationMap* metAuxMap = new MissingETAuxAssociationMap();
    if( evtStore()->record(metAuxMap, m_mapname+"Aux.").isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETAuxAssociationMap: " << m_mapname+"Aux.");
      return StatusCode::SUCCESS;
    }
    metMap->setStore(metAuxMap);

    // Apply track selection filters
    const CaloClusterContainer* tcCont(0);
    if( evtStore()->retrieve(tcCont, m_clcoll).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve topocluster container " << m_clcoll << " for overlap removal");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Successfully retrieved topocluster collection");

    const VertexContainer* vxCont(0);
    if( evtStore()->retrieve(vxCont, m_pvcoll).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve primary vertex container " << m_pvcoll);
      return StatusCode::SUCCESS;
    } else if(vxCont->empty()) {
      ATH_MSG_WARNING("Event has no primary vertices!");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Successfully retrieved primary vertex container");

    const TrackParticleContainer* tracks=0;
    ATH_CHECK( evtStore()->retrieve(tracks, m_trkcoll) );

    filterTracks(tracks,(*vxCont)[0]);

    const PFOContainer* pfoCont = m_pfotool->retrievePFO(CP::EM, CP::all);
    if(!pfoCont) {
      ATH_MSG_WARNING("Unable to retrieve input pfo container");
      return StatusCode::SUCCESS;
    }

    ATH_CHECK(m_trkseltool.retrieve());

    // Generate association objects from the jets in the event ******************
    const JetContainer* jetCont(0);
    if( evtStore()->retrieve(jetCont, m_jetcoll).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input jet container " << m_jetcoll);
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Successfully retrieved jet collection");

    CHECK( setupJetAssoc(metMap, jetCont, (*vxCont)[0]) );

    /// Muons
    /// Do muons before electrons and photons to remove track overlaps

    const MuonContainer* muonCont(0);
    if( evtStore()->retrieve(muonCont, m_muoncoll).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input muon container " << m_muoncoll);
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Successfully retrieved muon collection");

    // vector<const IParticle*> mutracks;
    CHECK( setupMuons(metMap, muonCont, (*vxCont)[0], pfoCont));

    /// Electrons

    const ElectronContainer* elCont(0);
    if( evtStore()->retrieve(elCont, m_elecoll).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input electron container " << m_elecoll);
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Successfully retrieved electron collection");
    CHECK( setupElectrons(metMap, elCont, tcCont, (*vxCont)[0], pfoCont));

    /// Photons

    const PhotonContainer* phCont(0);
    if( evtStore()->retrieve(phCont, m_gammacoll).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input photon container " << m_gammacoll);
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Successfully retrieved photon collection");
    CHECK( setupPhotons(metMap, phCont, tcCont, (*vxCont)[0], pfoCont));

    /// Taus

    const TauJetContainer* tauCont(0);
    if( evtStore()->retrieve(tauCont, m_taucoll).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input tau container " << m_taucoll);
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Successfully retrieved tau collection");

    CHECK( setupTaus(metMap, tauCont, (*vxCont)[0], pfoCont));

    // Do internal overlap-finding **********************************************

    bool foundOverlaps = metMap->identifyOverlaps();
    if(foundOverlaps) ATH_MSG_DEBUG("Overlaps identified!");

    // Create a MissingETContainer with its aux store
    MissingETContainer* newMet = new MissingETContainer();
    if( evtStore()->record(newMet, m_contname).isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETContainer: " << m_contname);
      return StatusCode::SUCCESS;
    }
    MissingETAuxContainer* metAuxCont = new MissingETAuxContainer();
    if( evtStore()->record(metAuxCont, m_contname+"Aux.").isFailure() ) {
      ATH_MSG_WARNING("Unable to record MissingETAuxContainer: " << m_contname+"Aux.");
      return StatusCode::SUCCESS;
    }
    newMet->setStore(metAuxCont);

    MissingET* metCoreCl = new MissingET(0.,0.,0.,"SoftClusCore",MissingETBase::Source::softEvent() | MissingETBase::Source::cluster());
    newMet->push_back(metCoreCl);
    MissingET* metCoreTrk = new MissingET(0.,0.,0.,"PVSoftTrkCore",MissingETBase::Source::softEvent() | MissingETBase::Source::track());
    newMet->push_back(metCoreTrk);
    const IParticleContainer* uniqueClusters;
    if (m_pflow) uniqueClusters = metMap->getUniqueSignals(pfoCont,MissingETBase::UsageHandler::Policy::ParticleFlow);
    else uniqueClusters = metMap->getUniqueSignals(tcCont);
    for(IParticleContainer::const_iterator iSig=uniqueClusters->begin();iSig!=uniqueClusters->end();iSig++) {
      if (m_pflow) {
        const xAOD::PFO *pfo = dynamic_cast<const xAOD::PFO*>(*iSig);
        if (pfo->charge()==0) {
	  TLorentzVector corrected = pfo->GetVertexCorrectedEMFourVec(*((*vxCont)[0]));
          if (pfo->eEM()>0) metCoreCl->add(corrected.Px(),corrected.Py(),corrected.Pt());
          continue;
        } else if (!acceptChargedPFO(pfo->track(0),(*vxCont)[0])) continue;
        *metCoreTrk += *iSig;
      }
     if ((*iSig)->e()>0) *metCoreCl += *iSig;
    }
    if (m_pflow) return StatusCode::SUCCESS;

    const IParticleContainer* uniqueTracks = metMap->getUniqueSignals(tracks);
    for(const auto& trk : *uniqueTracks) {
//      if(acceptTrack(dynamic_cast<const TrackParticle*>(trk),(*vxCont)[0]) && isGoodEoverP(dynamic_cast<const TrackParticle*>(trk))) {
      if(acceptTrack(dynamic_cast<const TrackParticle*>(trk),(*vxCont)[0]) ) {
	ATH_MSG_VERBOSE("Add core track with pt " << trk->pt());
	*metCoreTrk += trk;
      }
    }

    delete uniqueClusters;
    delete uniqueTracks;
    
    return StatusCode::SUCCESS;
  }

  //*********************************************************************************************************
  // Create jet associations
  StatusCode METAssocAlg::setupJetAssoc(xAOD::MissingETAssociationMap* metMap,
					const xAOD::JetContainer* jets,
					const xAOD::Vertex* pv)
  {
    for(const auto& jet : *jets) {
      ATH_MSG_VERBOSE("Jet pt, eta, phi = " << jet->pt() << ", " << jet->eta() << "," << jet->phi() );
      std::vector<const IParticle*> selectedTracks;
      if (m_pflow) {
        for (size_t consti = 0; consti < jet->numConstituents(); consti++) {
          const PFO *pfo = dynamic_cast<const PFO*>(jet->rawConstituent(consti));
          if (pfo->charge()!=0) selectedTracks.push_back(pfo->track(0));
        }
      } else {
        std::vector<const IParticle*> jettracks;
        jet->getAssociatedObjects<IParticle>(JetAttribute::GhostTrack,jettracks);
        selectedTracks = selectTracks(jettracks,pv);
      }
      MissingETComposition::add(metMap,jet,selectedTracks);
      ATH_MSG_VERBOSE("Added association " << metMap->findIndex(jet) << " pointing to jet " << jet);
    }
    MissingETComposition::addMiscAssociation(metMap);
    ATH_MSG_VERBOSE("Added miscellaneous association");

    return StatusCode::SUCCESS;
  }

  //*********************************************************************************************************
  // Do muon track finding
  StatusCode METAssocAlg::setupMuons(xAOD::MissingETAssociationMap* metMap,
				     const xAOD::MuonContainer* muons,
				     const xAOD::Vertex* pv,
				     const xAOD::PFOContainer* pfoCont)
  //				     std::vector<const xAOD::IParticle*>& mutracks)
  {
    vector<const IParticle*> constlist;
    constlist.reserve(2);
    for(const auto& mu : *muons) {
      //      if(mu->pt()<5e3) continue;
      ATH_MSG_VERBOSE( "Muon px, py, pt = " << mu->p4().Px() << ", " << mu->p4().Py() << ", " << mu->pt() );
      constlist.clear();
      MissingETBase::Types::constvec_t tcvec;
      MissingETBase::Types::constvec_t trkvec;
      //      ATH_MSG_VERBOSE( "Muon pt, eta, phi = " << mu->pt() << ", " << mu->eta() << "," << mu->phi() );
      const TrackParticle* idtrack = mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
      if(idtrack) {
	//	if(acceptTrack(idtrack,pv) && isGoodEoverP(idtrack)) {
	if(acceptTrack(idtrack,pv) ) {
	  ATH_MSG_VERBOSE("Accept muon track " << idtrack << " px, py = " << idtrack->p4().Px() << ", " << idtrack->p4().Py());
	  ATH_MSG_VERBOSE("Muon ID track ptr: " << idtrack);
          if (m_pflow) {
            for(const auto& pfo : *pfoCont) {
              if (pfo->charge()!=0 && pfo->track(0) == idtrack) {
                constlist.push_back(pfo);
	        trkvec += *idtrack;
              }
              if (pfo->charge()==0 && mu->cluster() && mu->cluster()->p4().DeltaR(pfo->p4EM())<0.1 && pfo->eEM()>0) constlist.push_back(pfo);
            }
	  } else {
	    constlist.push_back(idtrack);
	    trkvec += *idtrack;
          }
	  // if(mu->pt()>10e3 && (mu->muonType()==xAOD::Muon::Combined || mu->muonType()==xAOD::Muon::SegmentTagged)) {
	  //   mutracks.push_back(idtrack);
	  // }
	}
      }
      ATH_MSG_VERBOSE( "Muon track px, py, pt = " << trkvec.cpx() << ", " << trkvec.cpy() << ", " << trkvec.cpt() );

      bool inserted(false);
      inserted = MissingETComposition::insert(metMap,mu,constlist,tcvec,trkvec);
      if(inserted) {
	const MissingETAssociation* assoc = MissingETComposition::getAssociation(metMap,mu);
	ATH_MSG_VERBOSE( "Muon is associated to jet " << assoc->refJet()->index() << " with pt " << assoc->refJet()->pt() );
	ATH_MSG_VERBOSE( "Muon-jet deltaR = " << mu->p4().DeltaR(assoc->refJet()->p4()) );
      } else {
	ATH_MSG_VERBOSE( "Add muon as miscellaneous object" );
	inserted = MissingETComposition::insertMisc(metMap,mu,constlist,tcvec,trkvec);
      }
      ATH_MSG_VERBOSE( (inserted ? "Successfully" : "Unsuccessfully" ) << " inserted muon into association map");
    }
    ATH_MSG_VERBOSE("Done with muon setup");
    return StatusCode::SUCCESS;
  }

  //*********************************************************************************************************
  // Do electron topocluster finding
  StatusCode METAssocAlg::setupElectrons(xAOD::MissingETAssociationMap* metMap,
					 const xAOD::ElectronContainer* electrons,
					 const xAOD::CaloClusterContainer* clusters,
					 const xAOD::Vertex* pv,
					 const xAOD::PFOContainer* pfoCont)
  //					 std::vector<const xAOD::IParticle*>& mutracks)
  {
    vector<const xAOD::IParticle*> constlist;
    constlist.reserve(10);    
    ConstDataVector<ElectronContainer> electrons_tmp(SG::VIEW_ELEMENTS);
    for(const auto& ph : *electrons) {
      electrons_tmp.push_back(ph);
    }
    std::sort(electrons_tmp.begin(),electrons_tmp.end(),greaterPt);

    for(const auto& el : electrons_tmp) {
      // if(el->pt()<5e3) continue;
      ATH_MSG_VERBOSE( "Electron px, py, pt = " << el->p4().Px() << ", " << el->p4().Py() << ", " << el->pt() );
      constlist.clear();
      MissingETBase::Types::constvec_t tcvec,trkvec;
      //      ATH_MSG_VERBOSE( "Electron pt, eta, phi = " << el->pt() << ", " << el->eta() << "," << el->phi() );
      if (m_pflow) this->extractPFO(el,pfoCont,constlist,tcvec,trkvec,pv);
      else {
        this->extractTopoClusters(el,clusters,constlist,tcvec);
        this->extractTracks(el,pv,constlist,trkvec);
      }
      ATH_MSG_VERBOSE( "Electron has " << constlist.size() << " topoclusters." );

      ATH_MSG_VERBOSE( "Electron tc px, py, pt, sumpt = " << tcvec.cpx() << ", " << tcvec.cpy() << ", " << tcvec.cpt() << ", " << tcvec.sumpt() );
      ATH_MSG_VERBOSE( "Electron track px, py, pt, sumpt = " << trkvec.cpx() << ", " << trkvec.cpy() << ", " << trkvec.cpt() << ", " << trkvec.sumpt() );

      bool inserted(false);
      inserted = MissingETComposition::insert(metMap,el,constlist,tcvec,trkvec);
      if(inserted) {
	const MissingETAssociation* assoc = MissingETComposition::getAssociation(metMap,el);
	ATH_MSG_VERBOSE( "Electron is associated to jet " << assoc->refJet()->index() << " with pt " << assoc->refJet()->pt() );
	ATH_MSG_VERBOSE( "Electron-jet deltaR = " << el->p4().DeltaR(assoc->refJet()->p4()) );
      } else {
	ATH_MSG_VERBOSE( "Add electron as miscellaneous object" );
	inserted = MissingETComposition::insertMisc(metMap,el,constlist,tcvec,trkvec);
      }
      ATH_MSG_VERBOSE( (inserted ? "Successfully" : "Unsuccessfully" ) << " inserted electron into association map");
    }  
    ATH_MSG_VERBOSE("Done with electron setup");
    return StatusCode::SUCCESS;
  }

  //*********************************************************************************************************
  // Do photon topocluster finding
  StatusCode METAssocAlg::setupPhotons(xAOD::MissingETAssociationMap* metMap,
				       const xAOD::PhotonContainer* photons,
				       const xAOD::CaloClusterContainer* clusters,
				       const xAOD::Vertex* pv,
				       const xAOD::PFOContainer* pfoCont)
  //				       std::vector<const xAOD::IParticle*>& mutracks)
  {
    vector<const xAOD::IParticle*> constlist;
    constlist.reserve(10);
    ConstDataVector<PhotonContainer> photons_tmp(SG::VIEW_ELEMENTS);
    for(const auto& ph : *photons) {
      photons_tmp.push_back(ph);
    }
    std::sort(photons_tmp.begin(),photons_tmp.end(),greaterPt);    

    for(const auto& ph : photons_tmp) {
      // if(ph->pt()<5e3) continue;
      ATH_MSG_VERBOSE( "Photon px, py, pt = " << ph->p4().Px() << ", " << ph->p4().Py() << ", " << ph->pt() );
      constlist.clear();
      MissingETBase::Types::constvec_t tcvec,trkvec;
      if (m_pflow) this->extractPFO(ph,pfoCont,constlist,tcvec,trkvec,pv);
      else {
        this->extractTopoClusters(ph,clusters,constlist,tcvec);
        ATH_MSG_VERBOSE( "Photon has " << constlist.size() << " topoclusters." );
        this->extractTracks(ph,pv,constlist,trkvec);
      }

      ATH_MSG_VERBOSE( "Photon tc px, py, pt, sumpt = " << tcvec.cpx() << ", " << tcvec.cpy() << ", " << tcvec.cpt() << ", " << tcvec.sumpt() );
      ATH_MSG_VERBOSE( "Photon track px, py, pt, sumpt = " << trkvec.cpx() << ", " << trkvec.cpy() << ", " << trkvec.cpt() << ", " << trkvec.sumpt() );

      bool inserted(false);
      inserted = MissingETComposition::insert(metMap,ph,constlist,tcvec,trkvec);
      if(inserted) {
	const MissingETAssociation* assoc = MissingETComposition::getAssociation(metMap,ph);
	ATH_MSG_VERBOSE( "Photon is associated to jet " << assoc->refJet()->index() << " with pt " << assoc->refJet()->pt() );
	ATH_MSG_VERBOSE( "Photon-jet deltaR = " << ph->p4().DeltaR(assoc->refJet()->p4()) );
      } else {
	ATH_MSG_VERBOSE( "Add photon as miscellaneous object" );
	inserted = MissingETComposition::insertMisc(metMap,ph,constlist,tcvec,trkvec);
      }
      ATH_MSG_VERBOSE( (inserted ? "Successfully" : "Unsuccessfully" ) << " inserted photon into association map");
    }
    ATH_MSG_VERBOSE("Done with photons setup");
    return StatusCode::SUCCESS;
  }

  //*********************************************************************************************************
  // Do tau topocluster finding
  StatusCode METAssocAlg::setupTaus(xAOD::MissingETAssociationMap* metMap,
				    const xAOD::TauJetContainer* taus,
				    const xAOD::Vertex* pv,
				    const xAOD::PFOContainer* pfoCont)
  //				    std::vector<const xAOD::IParticle*>& mutracks)
  {
    vector<const xAOD::IParticle*> constlist;
    constlist.reserve(20);
    ConstDataVector<TauJetContainer> taus_tmp(SG::VIEW_ELEMENTS);
    for(const auto& ph : *taus) {
      taus_tmp.push_back(ph);
    }
    std::sort(taus_tmp.begin(),taus_tmp.end(),greaterPt);    

    for(const auto& tau : taus_tmp) {
      // if(tau->pt()<5e3) continue;
      ATH_MSG_VERBOSE( "Tau px, py, pt = " << tau->p4().Px() << ", " << tau->p4().Py() << ", " << tau->pt() );
      constlist.clear();
      MissingETBase::Types::constvec_t tcvec,trkvec;
      if (m_pflow) this->extractPFO(tau,pfoCont,constlist,tcvec,trkvec,pv);
      else {
        this->extractTopoClusters(tau,constlist,tcvec);
        this->extractTracks(tau,pv,constlist,trkvec);
      }

      ATH_MSG_VERBOSE( "Tau tc px, py, pt, sumpt = " << tcvec.cpx() << ", " << tcvec.cpy() << ", " << tcvec.cpt() << ", " << tcvec.sumpt() );
      ATH_MSG_VERBOSE( "Tau track px, py, pt, sumpt = " << trkvec.cpx() << ", " << trkvec.cpy() << ", " << trkvec.cpt() << ", " << trkvec.sumpt() );

      bool inserted(false);
      inserted = MissingETComposition::insert(metMap,tau,constlist,tcvec,trkvec);
      ATH_MSG_VERBOSE( (inserted ? "Successfully" : "Unsuccessfully" ) << " inserted tau into association map");
      const MissingETAssociation* assoc = MissingETComposition::getAssociation(metMap,tau);
      if(assoc) {
	ATH_MSG_VERBOSE( "Tau is associated to jet " << assoc->refJet()->index() << " with pt " << assoc->refJet()->pt() );
	ATH_MSG_VERBOSE( "Tau-jet deltaR = " << tau->p4().DeltaR(assoc->refJet()->p4()) );
	const Jet* seedjet = *tau->jetLink();
	ATH_MSG_VERBOSE( "Tau has seed jet " << seedjet->index() << " with pt " << seedjet->pt() );
      }
    }  

    ATH_MSG_VERBOSE("Done with tau setup");
    return StatusCode::SUCCESS;
  }
  //**********************************************************************
  // Get Egamma constituents
  void METAssocAlg::extractPFO(const xAOD::Egamma* eg,
				       const xAOD::PFOContainer* pfoCont,
				       std::vector<const xAOD::IParticle*>& pfolist,
				       MissingETBase::Types::constvec_t& pfovec,
				       MissingETBase::Types::constvec_t& trkvec,
				       const xAOD::Vertex* pv)
  {
    // safe to assume a single SW cluster?
    // will do so for now...
    const CaloCluster* swclus = eg->caloCluster();
    double eg_cl_e = swclus->e();
    const xAOD::Electron* elec = (eg->type()==xAOD::Type::ObjectType::Electron) ? dynamic_cast<const xAOD::Electron*>(eg) : NULL;
    const xAOD::Photon* ph = (eg->type()==xAOD::Type::ObjectType::Photon) ? dynamic_cast<const xAOD::Photon*>(eg) : NULL;

    // the matching strategy depends on how the cluster container is sorted
    // easier if it's sorted in descending pt order
    // we'll worry about optimisation later
    vector<const xAOD::PFO*> nearbyPFO;
    nearbyPFO.reserve(10);
    for(const auto& pfo : *pfoCont) {
      std::vector<const IParticle*> cls;
      bool match = false;
      if (pfo->charge()==0) {
        if (swclus->p4().DeltaR(pfo->p4EM())<0.1 && pfo->eEM()>0) match = true;
        //pfo->associatedParticles(PFODetails::CaloCluster,cls);
        //for(const auto& cl : cls) {
        //  if (!cl) continue;
        //  double dR = swclus->p4().DeltaR(cl->p4());
        //  if(dR<0.1 && cl->e()>0) match = true;
        //}
      }
      if (elec) {
        for(size_t iTrk=0; iTrk<elec->nTrackParticles(); ++iTrk) {
          const TrackParticle* eltrk = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(elec->trackParticle(iTrk));
          if(pfo->charge()!=0 && acceptChargedPFO(eltrk,pv) && pfo->track(0) == eltrk) match = true;
       }
      }
      if (ph) {
        for(size_t iVtx=0; iVtx<ph->nVertices(); ++iVtx) {
          const Vertex* phvx = ph->vertex(iVtx);
          for(size_t iTrk=0; iTrk<phvx->nTrackParticles(); ++iTrk) {
            const TrackParticle* phtrk = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(phvx->trackParticle(iTrk));
            if(pfo->charge()!=0 && acceptChargedPFO(phtrk,pv) && pfo->track(0) == phtrk) match = true; 
          }
	}
      }
      if (match) nearbyPFO.push_back(pfo);
    }
    ATH_MSG_VERBOSE("Found " << nearbyPFO.size() << " nearby pfos");

    bool doSum = true;
    double sumE_pfo = 0.;
    std::sort(nearbyPFO.begin(),nearbyPFO.end(),greaterPtPFO);
    for(const auto& pfo : nearbyPFO) {
      double pfo_e = (pfo->charge()==0 ? pfo->eEM() : pfo->e());
      // skip cluster if it's above our bad match threshold
      if(pfo->eEM()>2*eg_cl_e) {
        ATH_MSG_VERBOSE("Reject topocluster in sum. Ratio vs eg cluster: " << (pfo->eEM()/eg_cl_e));
	continue;
      }

      if( (doSum = fabs(sumE_pfo+pfo->e()-eg_cl_e) < fabs(sumE_pfo - eg_cl_e)) ) {
	pfolist.push_back(pfo);
	sumE_pfo += pfo_e;
	pfovec += (pfo->charge()==0 ? MissingETBase::Types::constvec_t(pfo->ptEM()*cos(pfo->phiEM()),pfo->ptEM()*sin(pfo->phiEM()),pfo->ptEM()*cosh(pfo->etaEM()),pfo->eEM(),pfo->eEM()) : MissingETBase::Types::constvec_t(*pfo));
        trkvec += MissingETBase::Types::constvec_t(*pfo);
      } // if we will retain the topocluster
      else {break;}
    } // loop over nearby clusters
  }

  void METAssocAlg::extractTracks(const xAOD::TauJet* tau,
				  const xAOD::Vertex* pv,
				  std::vector<const xAOD::IParticle*>& constlist,
				  MissingETBase::Types::constvec_t& trkvec)
  {
    const Jet* jet = *tau->jetLink();
    for(size_t iTrk=0; iTrk<tau->nTracks(); ++iTrk) {
      const TrackParticle* tautrk = tau->track(iTrk);
      //	if(acceptTrack(tautrk,pv) && isGoodEoverP(tautrk)) {
      if(acceptTrack(tautrk,pv) ) {
	// bool matchedmu = false;
	// for(const auto& mutrk : mutracks) {
	//   if( (matchedmu = (tautrk == mutrk)) ) {
	//     ATH_MSG_VERBOSE("Veto track matched to muon");
	// 	break;
	//   }
	// }
	// if(!matchedmu) {
	ATH_MSG_VERBOSE("Accept tau track " << tautrk << " px, py = " << tautrk->p4().Px() << ", " << tautrk->p4().Py());
	constlist.push_back(tautrk);
	trkvec += *tautrk;
	// }
      }
    }
    for(size_t iTrk=0; iTrk<tau->nOtherTracks(); ++iTrk) {
      const xAOD::TrackParticle* tautrk = tau->otherTrack(iTrk);
      double dR = jet->p4().DeltaR(tautrk->p4());
      //	if(dR<0.2 && acceptTrack(tautrk,pv) && isGoodEoverP(tautrk)) {
      if(dR<0.2 && acceptTrack(tautrk,pv) ) {
	// bool matchedmu = false;
	// for(const auto& mutrk : mutracks) {
	//   if( (matchedmu = (tautrk == mutrk)) ) {
	//     ATH_MSG_VERBOSE("Veto track matched to muon");
	// 	break;
	//   }
	// }
	// if(!matchedmu) {
	ATH_MSG_VERBOSE("Accept track " << tautrk << " px, py = " << tautrk->p4().Px() << ", " << tautrk->p4().Py());
	constlist.push_back(tautrk);
	trkvec += *tautrk;
	// }
      }
    }
  }
  void METAssocAlg::extractTracks(const xAOD::Photon* ph,
				  const xAOD::Vertex* pv,
				  std::vector<const xAOD::IParticle*>& constlist,
				  MissingETBase::Types::constvec_t& trkvec)
  {
    vector<const xAOD::TrackParticle*> phtrks;
    for(size_t iVtx=0; iVtx<ph->nVertices(); ++iVtx) {
      const Vertex* phvx = ph->vertex(iVtx);
      for(size_t iTrk=0; iTrk<phvx->nTrackParticles(); ++iTrk) {
	// if(!phvx->trackParticle(iTrk)) {
	//   ATH_MSG_INFO("Invalid photon trackparticle pointer");
	// }
	const TrackParticle* phtrk = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(phvx->trackParticle(iTrk));
	//	  if(acceptTrack(phtrk,pv) && isGoodEoverP(phtrk) && ph->p4().DeltaR(phtrk->p4())<0.1) {
	if(acceptTrack(phtrk,pv) && ph->p4().DeltaR(phtrk->p4())<0.2) {
	  // bool matchedmu = false;
	  // for(const auto& mutrk : mutracks) {
	  //   if( (matchedmu = (phtrk == mutrk)) ) {
	  // 	ATH_MSG_VERBOSE("Veto track matched to muon");
	  // 	break;
	  //   }
	  // }
	  bool duplicate = false;
	  for(const auto& gamtrk : phtrks) {
	    if( (duplicate = (phtrk == gamtrk)) ) {
	      ATH_MSG_VERBOSE("Veto duplicate track");
	      break;
	    }
	  }
	  // if(!matchedmu && !duplicate) {
	  if(!duplicate) {
	    ATH_MSG_VERBOSE("Accept photon track " << phtrk << " px, py = " << phtrk->p4().Px() << ", " << phtrk->p4().Py());
	    ATH_MSG_VERBOSE("              track eta, phi = " << phtrk->p4().Eta() << ", " << phtrk->p4().Phi());
	    ATH_MSG_VERBOSE("Delta R = " << ph->p4().DeltaR(phtrk->p4()));
	    constlist.push_back(phtrk);
	    trkvec += *phtrk;
	    phtrks.push_back(phtrk);
	  }
	}
      }
    }
  }
  void METAssocAlg::extractTracks(const xAOD::Electron* el,
				  const xAOD::Vertex* pv,
				  std::vector<const xAOD::IParticle*>& constlist,
				  MissingETBase::Types::constvec_t& trkvec)
  {
    for(size_t iTrk=0; iTrk<el->nTrackParticles(); ++iTrk) {
      const TrackParticle* eltrk = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(el->trackParticle(iTrk));
      //	if(acceptTrack(eltrk,pv) && isGoodEoverP(eltrk) && el->p4().DeltaR(eltrk->p4())<0.1) {
      if(acceptTrack(eltrk,pv)  && el->p4().DeltaR(eltrk->p4())<0.1) {
	// bool matchedmu = false;
	// for(const auto& mutrk : mutracks) {
	//   if( (matchedmu=(eltrk == mutrk))) {
	//     ATH_MSG_VERBOSE("Veto track matched to muon");
	//     break;
	//   }
	// }
	// if(!matchedmu) {
	ATH_MSG_VERBOSE("Accept electron track " << eltrk << " px, py = " << eltrk->p4().Px() << ", " << eltrk->p4().Py());
	constlist.push_back(eltrk);
	trkvec += *eltrk;
      }
    }
  }
  //**********************************************************************
  // Get Egamma constituents
  void METAssocAlg::extractTopoClusters(const xAOD::Egamma* eg,
					const xAOD::CaloClusterContainer* tcCont,
					std::vector<const xAOD::IParticle*>& tclist,
					MissingETBase::Types::constvec_t& tcvec)
  {
    // safe to assume a single SW cluster?
    // will do so for now...
    const CaloCluster* swclus = eg->caloCluster();
    double eg_cl_e = swclus->e();

    // the matching strategy depends on how the cluster container is sorted
    // easier if it's sorted in descending pt order
    // we'll worry about optimisation later
    vector<const xAOD::CaloCluster*> nearbyTC;
    nearbyTC.reserve(10);
    for(const auto& cl : *tcCont) {
      // this can probably be done more elegantly
      double dR = swclus->p4().DeltaR(cl->p4());
      if(dR<0.1 && cl->e()>0) {
	// could consider also requirements on the EM fraction or depth
	nearbyTC.push_back(cl);
      } // match TC in a cone around SW cluster
    }
    ATH_MSG_VERBOSE("Found " << nearbyTC.size() << " nearby topoclusters");

    bool doSum = true;
    double sumE_tc = 0.;
    const CaloCluster* bestbadmatch = 0;
    std::sort(nearbyTC.begin(),nearbyTC.end(),greaterPt);
    for(const auto& cl : nearbyTC) {
      double tcl_e = cl->e();
      // skip cluster if it's above our bad match threshold
      // FIXME: What to do with these poor matches?
      if(tcl_e>1.5*eg_cl_e) {
	ATH_MSG_VERBOSE("Reject topocluster in sum. Ratio vs eg cluster: " << (tcl_e/eg_cl_e));
	if( !bestbadmatch || (fabs(tcl_e/eg_cl_e-1.) < fabs(bestbadmatch->e()/eg_cl_e-1.)) ) bestbadmatch = cl;
	continue;
      }

      ATH_MSG_VERBOSE("E match with new cluster: " << fabs(sumE_tc+tcl_e - eg_cl_e) / eg_cl_e);
      if( (doSum = (fabs(sumE_tc+tcl_e - eg_cl_e) < fabs(sumE_tc - eg_cl_e))) ) {
	ATH_MSG_VERBOSE("Accept topocluster with pt " << cl->pt() << ", e " << cl->e() << " in sum.");
	ATH_MSG_VERBOSE("E match with new cluster: " << fabs(sumE_tc+tcl_e - eg_cl_e) / eg_cl_e);
	ATH_MSG_VERBOSE("Energy ratio of TC to eg: " << tcl_e / eg_cl_e);
	tclist.push_back(cl);
	sumE_tc += tcl_e;
	tcvec += MissingETBase::Types::constvec_t(*cl);
      } // if we will retain the topocluster
    } // loop over nearby clusters
    if(sumE_tc<1e-9 && bestbadmatch) {
      ATH_MSG_VERBOSE("No better matches found -- add bad match topocluster with pt "
		      << bestbadmatch->pt() << ", e " << bestbadmatch->e() << ".");
      tclist.push_back(bestbadmatch);
      tcvec += MissingETBase::Types::constvec_t(*bestbadmatch);
    }
  }

  //*********************************************************************************************************
  // Get tau constituents
  void METAssocAlg::extractPFO(const xAOD::TauJet* tau,
				       const xAOD::PFOContainer* pfoCont,
				       std::vector<const xAOD::IParticle*>& pfolist,
				       MissingETBase::Types::constvec_t& pfovec,
				       MissingETBase::Types::constvec_t& trkvec,
				       const xAOD::Vertex* pv)
  {  
    const Jet* seedjet = *tau->jetLink();
    JetConstituentVector constit = seedjet->getConstituents();
    ATH_MSG_VERBOSE("Current tau has " << constit.size() << " constituents.");
    for(const auto& pfo : *pfoCont) {
      bool match = false;
      if (pfo->charge()==0 && seedjet->p4().DeltaR(pfo->p4EM())<0.2) match = true;;
      for(size_t iTrk=0; iTrk<tau->nTracks(); ++iTrk) {
	const TrackParticle* tautrk = tau->track(iTrk);
	if(acceptChargedPFO(tautrk,pv) && pfo->charge()!=0 && tautrk==pfo->track(0)) match = true; 
      }
      for(size_t iTrk=0; iTrk<tau->nOtherTracks(); ++iTrk) {
	const xAOD::TrackParticle* tautrk = tau->otherTrack(iTrk);
	double dR = seedjet->p4().DeltaR(tautrk->p4());
	if(dR<0.2 && acceptChargedPFO(tautrk,pv) && pfo->charge()!=0 && tautrk==pfo->track(0)) match = true;
      }
      if (!match) continue; 
      pfolist.push_back(pfo);
      if (pfo->charge()) trkvec += MissingETBase::Types::constvec_t(*pfo);
      pfovec += (pfo->charge()==0 ? MissingETBase::Types::constvec_t(pfo->ptEM()*cos(pfo->phiEM()),pfo->ptEM()*sin(pfo->phiEM()),pfo->ptEM()*cosh(pfo->etaEM()),pfo->eEM(),pfo->eEM()) : MissingETBase::Types::constvec_t(*pfo));
    }
  }

  //*********************************************************************************************************
  // Get tau constituents
  void METAssocAlg::extractTopoClusters(const xAOD::TauJet* tau,
					std::vector<const xAOD::IParticle*>& tclist,
					MissingETBase::Types::constvec_t& tcvec)
  {  
    const Jet* seedjet = *tau->jetLink();
    JetConstituentVector constit = seedjet->getConstituents();
    ATH_MSG_VERBOSE("Current tau has " << constit.size() << " constituents.");
    // test for used topoclusters, and retrieve unused ones (ok until/unless we use PFlow taus)
    // only use clusters for computing the overlap removal relative to other objects
    for(const auto& cl : constit) {
      // TEMP: use jet seed axis
      //       taus will provide an accessor
      double dR = seedjet->p4().DeltaR(cl->rawConstituent()->p4());
      if(dR>0.2) continue;
      // skip cluster if dR>0.2
      const CaloCluster* pClus = dynamic_cast<const CaloCluster*>( cl->rawConstituent() );
      tclist.push_back(pClus);
      tcvec += MissingETBase::Types::constvec_t(*pClus);
    } // loop over jet constituents
  }

  bool METAssocAlg::acceptChargedPFO(const xAOD::TrackParticle* trk, const xAOD::Vertex* pv) {
    if(fabs((trk->z0() - pv->z()+trk->vz())*sin(trk->theta())) > 2) return false;
    return true;
  }


  bool METAssocAlg::acceptTrack(const xAOD::TrackParticle* trk, const xAOD::Vertex* pv)
  {
    //    if(!trk || !pv) return false;

  //  if(fabs(trk->pt())<500/*MeV*/ || fabs(trk->eta())>2.5) return false;

  //  // could add some error checking to make sure we successfully read the details
  //  uint8_t nPixHits(0), nSctHits(0);
  //  trk->summaryValue(nPixHits,xAOD::numberOfPixelHits);
  //  if(nPixHits<1) return false;
  //  trk->summaryValue(nSctHits,xAOD::numberOfSCTHits);
  //  if(nSctHits<6) return false;

  //  if(fabs(trk->d0())>1.5) return false;
  //  if(fabs(trk->z0() + trk->vz() - pv->z()) > 1.5) return false;

    if(!(m_trkseltool->accept( *trk, pv ))) return false; 
    else return true;

  }

  bool METAssocAlg::isGoodEoverP(const xAOD::TrackParticle* trk) {

    if( (fabs(trk->eta())<1.5 && trk->pt()>200e3) ||
	(fabs(trk->eta())>=1.5 && trk->pt()>120e3) ) {

      const CaloClusterContainer *clusters = 0;
      if( evtStore()->retrieve(clusters, m_clcoll).isFailure() ) {
	ATH_MSG_WARNING("Unable to retrieve topocluster container " << m_clcoll << " for overlap removal");
	return StatusCode::SUCCESS;
      }

      // Get relative error on qoverp
      float Rerr = Amg::error(trk->definingParametersCovMatrix(),4)/fabs(trk->qOverP());
      ATH_MSG_VERBOSE( "Track momentum error (%): " << Rerr*100 );

      // first compute track and calo isolation variables
      float ptcone20 = 0;
      for(const auto& testtrk : m_goodtracks) {
	if(testtrk==trk) continue;
	if(testtrk->p4().DeltaR(trk->p4()) < 0.2) {
	  ptcone20 += testtrk->pt();
	}
      }
      float isolfrac = ptcone20 / trk->pt();
      ATH_MSG_VERBOSE( "Track isolation fraction: " << isolfrac );

      float etcone10 = 0.;
      for(const auto& clus : *clusters) {
	if(clus->p4().DeltaR(trk->p4()) < 0.1) {
	  etcone10 += clus->pt();
	}
      }
      float EoverP = etcone10/trk->pt();
      ATH_MSG_VERBOSE( "Track E/P: " << EoverP );

      if(isolfrac<0.1) {
	// isolated track cuts
	if(Rerr>0.4) return false;
	else if (EoverP<0.65 && (EoverP>0.1 || Rerr>0.1)) return false;
      } else {
	// non-isolated track cuts
	float trkptsum = ptcone20+trk->pt();
	if(EoverP/trkptsum<0.6 && ptcone20/trkptsum<0.6) return false;
      }
    }
    return true;
  }

  void METAssocAlg::filterTracks(const xAOD::TrackParticleContainer* tracks,
				 const xAOD::Vertex* pv) {
    for(const auto& trk : *tracks) {
      m_goodtracks.clear();
      if(acceptTrack(trk,pv)) m_goodtracks.push_back(trk);
    }
  }

  std::vector<const xAOD::IParticle*> METAssocAlg::selectTracks(const std::vector<const xAOD::IParticle*>& tracksin,
								const xAOD::Vertex* pv)
  {
    std::vector<const xAOD::IParticle*> tracksout;
    tracksout.reserve(tracksin.size());
    for(const auto& trk : tracksin) {
      const TrackParticle* pTrk = dynamic_cast<const TrackParticle*>(trk);
//      if( acceptTrack(pTrk,pv) && isGoodEoverP(pTrk) ) {
      if( acceptTrack(pTrk,pv) ) {
	tracksout.push_back(trk);
	ATH_MSG_VERBOSE("Accept track " << trk << " px, py = " << trk->p4().Px() << ", " << trk->p4().Py());
      }
    }
    return tracksout;
  }

  //**********************************************************************

}


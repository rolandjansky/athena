///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METTrackFilterTool.cxx 
// Implementation file for class METTrackFilterTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METTrackFilterTool.h"

// MET EDM
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"

// Tracking EDM
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

// Calo EDM
#include "xAODCaloEvent/CaloClusterContainer.h"

// Egamma EDM
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/Electron.h"

// Track errors
#include "EventPrimitives/EventPrimitivesHelpers.h"

// ConstDV
#include "AthContainers/ConstDataVector.h"

// DeltaR calculation
#include "FourMomUtils/xAODP4Helpers.h"

namespace met {

  using std::vector;
  //
  using namespace xAOD;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 

  // Constructors
  ////////////////
  METTrackFilterTool::METTrackFilterTool(const std::string& name) : 
    AsgTool(name),
    METRefinerTool(name)
  {
    declareProperty( "DoPVSel",            m_trk_doPVsel = true                 );
    declareProperty( "DoEoverPSel",        m_trk_doEoverPsel = false            );
    declareProperty( "DoVxSep",            m_doVxSep = false                    );
    declareProperty( "DoLepRecovery",      m_doLepRecovery=false                );
    declareProperty( "UseIsolationTools",  m_useIsolationTools=true             );
    declareProperty( "CentralTrackPtThr",  m_cenTrackPtThr = 200e+3             );
    declareProperty( "ForwardTrackPtThr",  m_forTrackPtThr = 120e+3             );
  }

  // Destructor
  ///////////////
  METTrackFilterTool::~METTrackFilterTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METTrackFilterTool::initialize()
  {
    ATH_CHECK( METRefinerTool::initialize() );
    ATH_MSG_INFO ("Initializing " << name() << "...");
    ATH_CHECK(m_trkseltool.retrieve());
    ATH_CHECK(m_trkToVertexTool.retrieve());
    ATH_CHECK(m_trkIsolationTool.retrieve());
    ATH_CHECK(m_caloIsolationTool.retrieve());

    if(m_doVxSep) ATH_MSG_INFO("Building TrackMET for each vertex");

    ATH_CHECK( m_cl_inputkey.initialize());
    ATH_CHECK( m_pv_inputkey.initialize());
    if(m_doLepRecovery){
      ATH_CHECK( m_el_inputkey.initialize());
      ATH_CHECK( m_mu_inputkey.initialize());
    }

    return StatusCode::SUCCESS;
  }

  StatusCode METTrackFilterTool::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");

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
  // bool METTrackFilterTool::isPVTrack(const xAOD::TrackParticle* /*trk*/,
  // 				     const xAOD::Vertex* /*pv*/) const
  // {
  //   if(!trk || !pv) return false;
  //   if(fabs(trk->d0())>m_trk_d0Max) return false;
  //   if(fabs(trk->z0()+trk->vz() - pv->z()) > m_trk_z0Max) return false;
  //   return true;
  // }

  bool METTrackFilterTool::isGoodEoverP(const xAOD::TrackParticle* trk,
					const std::vector<const xAOD::TrackParticle*>& trkList,
					const xAOD::CaloClusterContainer* clusters) const {

    if( (fabs(trk->eta())<1.5 && trk->pt()>m_cenTrackPtThr) ||
    	(fabs(trk->eta())>=1.5 && trk->pt()>m_forTrackPtThr) ) {

      // Get relative error on qoverp
      float Rerr = Amg::error(trk->definingParametersCovMatrix(),4)/fabs(trk->qOverP());
      ATH_MSG_VERBOSE( "Track momentum error (%): " << Rerr*100 );

      // first compute track and calo isolation variables
      float ptcone20 = 0., isolfrac = 0., etcone10 = 0., EoverP = 0.;
      if(!m_useIsolationTools) {
        ATH_MSG_VERBOSE( "Using OLD track isolation setup");
        // ptcone
        for(const auto& obj : trkList) {
          if(obj->type() == xAOD::Type::TrackParticle) {
            const TrackParticle* testtrk = static_cast<const TrackParticle*>(obj);
            if(testtrk==trk) continue;
            if (xAOD::P4Helpers::isInDeltaR(*testtrk,*trk,0.2,m_useRapidity)) {
	          ptcone20 += testtrk->pt();
            } 
          }
          else {ATH_MSG_WARNING("METTrackFilterTool::isGoodEoveP given an object of type " << obj->type());}
        }
        isolfrac = ptcone20 / trk->pt();
        // etcone
        for(const auto& clus : *clusters) {
          if (xAOD::P4Helpers::isInDeltaR(*clus,*trk,0.1,m_useRapidity)) {
            etcone10 += clus->pt();
          }
        }
        EoverP = etcone10/trk->pt();
      } 
      else {
        ATH_MSG_VERBOSE( "Using NEW track isolation setup");
        // ptcone
        TrackIsolation trkIsoResult;
        std::vector<Iso::IsolationType> trkIsoCones; 
        trkIsoCones.push_back(xAOD::Iso::IsolationType::ptcone20);
        xAOD::TrackCorrection trkIsoCorr;
        trkIsoCorr.trackbitset.set(xAOD::Iso::IsolationTrackCorrection::coreTrackPtr); 
        m_trkIsolationTool->trackIsolation(trkIsoResult,
                                           *trk,
                                           trkIsoCones,
                                           trkIsoCorr);
        ptcone20 = trkIsoResult.ptcones.size() > 0 ? trkIsoResult.ptcones[0] : 0;
        isolfrac = ptcone20/trk->pt();
        // etcone
        CaloIsolation caloIsoResult_coreCone;
        std::vector<Iso::IsolationType> caloIsoCones_coreCone; 
        caloIsoCones_coreCone.push_back(xAOD::Iso::IsolationType::etcone20); 
        xAOD::CaloCorrection caloIsoCorr_coreCone;
        caloIsoCorr_coreCone.calobitset.set(xAOD::Iso::IsolationCaloCorrection::coreCone); 
        m_caloIsolationTool->caloTopoClusterIsolation(caloIsoResult_coreCone,
                                                      *trk,
                                                      caloIsoCones_coreCone,
                                                      caloIsoCorr_coreCone);
        etcone10 =  caloIsoResult_coreCone.etcones.size() > 0 ? 
                    caloIsoResult_coreCone.coreCorrections[xAOD::Iso::IsolationCaloCorrection::coreCone][xAOD::Iso::IsolationCorrectionParameter::coreEnergy] : 0.;
        EoverP   =  etcone10/trk->pt(); 
        /////////////////////////////////////////////////////////////////////////
      }
      ATH_MSG_VERBOSE( "Track isolation fraction: " << isolfrac );
      ATH_MSG_VERBOSE( "Track E/P = " << EoverP );

      if(isolfrac<0.1) {
		// isolated track cuts
		if(Rerr>0.4) return false;
		else if (EoverP<0.65 && ((EoverP>0.1 && Rerr>0.05) || Rerr>0.1)) return false;
          } else {
		// non-isolated track cuts
		float trkptsum = ptcone20+trk->pt();
		if(etcone10/trkptsum<0.6 && trk->pt()/trkptsum>0.6) return false;
      }
    }

    return true;
  }

  StatusCode METTrackFilterTool::executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) const
  {

    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    std::vector<const xAOD::Electron*> selElectrons;
    std::vector<const xAOD::Muon*> selMuons;

    if(m_doLepRecovery)
      {
        SG::ReadHandle<xAOD::ElectronContainer> elCont(m_el_inputkey);
        if (!elCont.isValid()) {
          ATH_MSG_WARNING("Unable to retrieve electron container " << m_el_inputkey.key());
          return StatusCode::SUCCESS;
        } else { 
	  selectElectrons(*elCont, selElectrons); 
	} 
        SG::ReadHandle<xAOD::MuonContainer> muCont(m_mu_inputkey);
        if (!muCont.isValid()) {
          ATH_MSG_WARNING("Unable to retrieve muon container " << m_mu_inputkey.key());
          return StatusCode::SUCCESS;
        } else { 
	  selectMuons(*muCont, selMuons); 
	} 

      }     

    MissingETComponentMap::iterator iter = MissingETComposition::find(metMap,metTerm);
    if(iter==metMap->end()) {
      ATH_MSG_WARNING("Could not find current METComponent in MET Map!");
      return StatusCode::FAILURE;
    }
    MissingETComponent* newComp = *iter;
    newComp->setStatusWord(MissingETBase::Status::Tags::correctedTerm(MissingETBase::Status::Nominal,
								      MissingETBase::Status::PileupTrack));

    // Extract the component corresponding to the Track SoftTerms
    MissingETBase::Types::bitmask_t src_ST_idTrk = MissingETBase::Source::SoftEvent | MissingETBase::Source::idTrack();
    MissingETBase::Types::bitmask_t src_ST_refTrk = MissingETBase::Source::softEvent() | MissingETBase::Source::track();
    metTerm->setSource(src_ST_refTrk);

    MissingETComponentMap::const_iterator citer = MissingETComposition::find(metMap,src_ST_idTrk);
    if(citer==metMap->end()) {
      ATH_MSG_WARNING("Could not find Soft ID Track component in MET Map!");
      return StatusCode::FAILURE;
    }
    vector<const TrackParticle*> softTracks;
    softTracks.reserve((*citer)->size());
    for(const auto& obj : (*citer)->objects()) {
      const TrackParticle* trk = dynamic_cast<const TrackParticle*>(obj);
      if(trk) {softTracks.push_back(trk);}
      else {ATH_MSG_WARNING("Track filter given an object of type " << obj->type());}
    }

    const Vertex* pv=0;
    SG::ReadHandle<xAOD::VertexContainer> vxCont(m_pv_inputkey);
    if (!vxCont.isValid()) {
      ATH_MSG_WARNING("Unable to retrieve input primary vertex container");
    }
    vector<const Vertex*> vertices;

    if(m_trk_doPVsel) {
      if(vxCont->size()>0) {
	vertices.reserve(vxCont->size());
	for(const auto& vx : *vxCont) {
	  if(vx->vertexType()==VxType::PriVtx) {pv = vx;}
	  vertices.push_back(vx);
	}
      } else{
	ATH_MSG_WARNING("Event has no primary vertices");
	return StatusCode::FAILURE;
      }
      if(pv) { ATH_MSG_DEBUG("Main primary vertex has z = " << pv->z()); }
      else {
	ATH_MSG_DEBUG("Did not find a primary vertex in the container. Reject all tracks.");
	return StatusCode::SUCCESS;
      }
    }

    if(m_doVxSep) {
      xAOD::TrackVertexAssociationMap trktovxmap=m_trkToVertexTool->getUniqueMatchMap(softTracks, vertices);

      // initialize metContainer and metTerm
      MissingETContainer* metCont = static_cast<MissingETContainer*>( metTerm->container() );

      bool firstVx(true);
      std::string basename = metTerm->name()+"_vx";
      for(const auto& vx : *vxCont){
	if(vx->vertexType()==VxType::PriVtx || vx->vertexType()==VxType::PileUp) {
	  MissingET *met_vx = metTerm;
	  if(!firstVx) {
	    met_vx = new MissingET(0. ,0. ,0. );
	    metCont->push_back(met_vx);
	    met_vx->setSource(metTerm->source());
	    MissingETComposition::add(metMap, met_vx);
	  }
	  met_vx->setName(basename+std::to_string(vx->index()));
	  ATH_MSG_VERBOSE("Building " << met_vx->name());

	  ATH_MSG_VERBOSE("Number of tracks associated to vertex " << vx->index() << ": "<< (trktovxmap[vx]).size());
	  
	  ATH_CHECK( buildTrackMET(metMap,met_vx,vx,selElectrons,selMuons,trktovxmap[vx]) );
	  firstVx = false;
	}
      }
    } else {
      ATH_CHECK( buildTrackMET(metMap,metTerm,pv,selElectrons,selMuons,softTracks) );
    }

    return StatusCode::SUCCESS;
  }

  StatusCode METTrackFilterTool::buildTrackMET(xAOD::MissingETComponentMap* const metMap,
					       xAOD::MissingET* const metTerm,
					       const xAOD::Vertex* const pv,
					       const std::vector<const xAOD::Electron*>& selElectrons,
					       const std::vector<const xAOD::Muon*>& selMuons,
					       const std::vector<const xAOD::TrackParticle*>& softTracks) const {

    vector<const IParticle*> dummyList;
    MissingETBase::Types::weight_t unitWeight(1.,1.,1.);

    const MissingETComponent* metComp = MissingETComposition::getComponent(metMap,metTerm);
    if(!metComp) {
      ATH_MSG_WARNING("Failed to find MissingETComponent for MET term " << metTerm->name());
      return StatusCode::FAILURE;
    }
    // Loop over the tracks and select only good ones
    for( const auto& trk : softTracks ) {
      // Could/should use common implementation of addToMET here -- derive builder and refiner from a common base tool?
      bool passFilters = true;
      SG::ReadHandle<xAOD::CaloClusterContainer> tcCont(m_cl_inputkey);
      if (!tcCont.isValid()) {
	  ATH_MSG_WARNING("Unable to retrieve topocluster container " << m_cl_inputkey << " for overlap removal");
      }
      if(m_trk_doEoverPsel && !isGoodEoverP(trk,softTracks,tcCont.cptr())) passFilters = false;
      if(m_trk_doPVsel) {
	if(!(m_trkseltool->accept( *trk, pv ))) passFilters=false;
      } else {
	if(!(m_trkseltool->accept( trk ))) passFilters=false;
      }

      bool isMuon=false;
      bool isElectron=false;
      size_t el_index=-1;

      if(m_doLepRecovery) {
        isMuon=isMuTrack(*trk,selMuons);
        isElectron=isElTrack(*trk,selElectrons, el_index);
      }

      bool isLepton=(isMuon||isElectron);

      if(passFilters || (m_doLepRecovery && isLepton)) {
        if(!passFilters && isElectron && m_doLepRecovery) {
	  //electron track fails, replace with electron pt
          const Electron* el = selElectrons[el_index];

          metTerm->add(el->pt()*cos(trk->phi()),
              el->pt()*sin(trk->phi()),
              el->pt());
          MissingETComposition::insert(metMap,metTerm,el,dummyList,unitWeight);
        } else {
	  metTerm->add(trk->pt()*cos(trk->phi()),trk->pt()*sin(trk->phi()),trk->pt());
	  MissingETComposition::insert(metMap,metTerm,trk,dummyList,unitWeight);
	  ATH_MSG_VERBOSE("METTerm " << metTerm->name() <<" sumpt: " << metTerm->sumet());
        }
      }
    }
    return StatusCode::SUCCESS;
  }

  bool METTrackFilterTool::isElTrack(const xAOD::TrackParticle &trk, const std::vector<const xAOD::Electron*>& electrons, size_t &el_index) const
  {
    for(unsigned int eli=0; eli<electrons.size(); ++eli) {
      const xAOD::Electron *el=electrons.at(eli);
      if(&trk==xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(el->trackParticle())) {
	el_index=eli;
	return true;
      }
    }
    return false;
  }

  bool METTrackFilterTool::isMuTrack(const xAOD::TrackParticle &trk, const std::vector<const xAOD::Muon*>& muons) const
  {
    for(unsigned mui=0;mui<muons.size();mui++) {
      //        if(((muon_list.at(mui))->trackParticle(xAOD::Muon::InnerDetectorTrackParticle))->pt()==trk->pt())
      if(((muons.at(mui))->trackParticle(xAOD::Muon::InnerDetectorTrackParticle))==&trk) {
	return true;
      }
    }
    return false;
  }

  void METTrackFilterTool::selectElectrons(const xAOD::ElectronContainer &elCont, std::vector<const xAOD::Electron*>& electrons) const
  {
    for(unsigned int eli=0; eli< elCont.size(); eli++) {
      const xAOD::Electron *el=elCont.at(eli);
      if( (el)->author()&0x1  //electron author
	  && (el)->pt()>10000   // electron pt
	  && fabs(el->eta())<2.47  // electron eta
          ) {
	if(!((fabs((el)->eta())>1.37) && (fabs((el)->eta())<1.52) )) {
	  // crack region
	  electrons.push_back(el);
	}
      }
    }
  }
  

  void METTrackFilterTool::selectMuons(const xAOD::MuonContainer &muCont, std::vector<const xAOD::Muon*>& muons) const
  {
    for(unsigned int mui=0; mui<muCont.size();mui++) {
      const xAOD::Muon *mu=muCont.at(mui);
      if( (mu->muonType()==xAOD::Muon::Combined)
	  && (mu->pt()>6000.)
	  && fabs(mu->eta())<2.5
	  ) {
	muons.push_back(mu);
      }
    }
  }


  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

}


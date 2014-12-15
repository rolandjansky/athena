///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METAssociator.cxx 
// Implementation for class METAssociator
//
// This is the base class for tools that construct MET terms
// from other object collections.
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METAssociator.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAssociationMap.h"

// Tracking EDM
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"

// Track errors
#include "EventPrimitives/EventPrimitivesHelpers.h"

namespace met {

  using namespace xAOD;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 

  // Constructors
  ////////////////
  METAssociator::METAssociator(const std::string& name) :
    AsgTool(name),
    m_signalstate(-1)
  {
    declareProperty( "InputCollection",   m_input_data_key                    );
    declareProperty( "PrimVxColl",        m_pvcoll    = "PrimaryVertices"     );
    declareProperty( "TrkColl",           m_trkcoll   = "InDetTrackParticles" );
    declareProperty( "ClusColl",          m_clcoll    = "CaloCalTopoClusters" );
    declareProperty( "PFlow",             m_pflow     = false                 );
    declareProperty( "PFOTool",           m_pfotool                           );

    declareProperty( "TrackSelectorTool", m_trkseltool                        );
  }

  // Destructor
  ///////////////
  METAssociator::~METAssociator()
  {} 

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METAssociator::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    ATH_CHECK( m_trkseltool.retrieve() );

    return StatusCode::SUCCESS;
  }

  StatusCode METAssociator::execute(xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap)
  {
    ATH_MSG_DEBUG ("In execute: " << name() << "...");
    if(!metCont) {
      ATH_MSG_WARNING("Invalid pointer to MissingETContainer supplied! Abort.");
      return StatusCode::FAILURE;
    }

    if(!metMap) {
      ATH_MSG_WARNING("Invalid pointer to MissingETAssociationMap supplied! Abort.");
      return StatusCode::FAILURE;
    }

    // will set the signal state for the first event with jets
    if(m_signalstate<0) {
      if(metMap->size()>0) { //allow for misc association
	const MissingETAssociation* assoc = metMap->front();
	if(!assoc->isMisc()) {
	  m_signalstate = assoc->refJet()->getConstituentsSignalState();
	  ATH_MSG_INFO("Configured METAssociator with signal state " << m_signalstate);
	}
      }
    }

    return this->executeTool(metCont, metMap);
  }

  StatusCode METAssociator::retrieveConstituents(const xAOD::CaloClusterContainer*& tcCont,const xAOD::Vertex*& pv,const xAOD::TrackParticleContainer*& trkCont,const xAOD::PFOContainer*& pfoCont)
  {
    ATH_MSG_DEBUG ("In execute: " << name() << "...");
    tcCont = 0;
    if( evtStore()->retrieve(tcCont, m_clcoll).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve topocluster container " << m_clcoll << " for overlap removal");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Successfully retrieved topocluster collection");

    const VertexContainer *vxCont = 0;
    pv = 0;
    if( evtStore()->retrieve(vxCont, m_pvcoll).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve primary vertex container " << m_pvcoll);
      return StatusCode::FAILURE;
    } else if(vxCont->empty()) {
      ATH_MSG_WARNING("Event has no primary vertices!");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Successfully retrieved primary vertex container");
    for(const auto& vx : *vxCont) {
      if(vx->vertexType()==VxType::PriVtx)
	{pv = vx; break;}
    }
    if(!pv) {
      ATH_MSG_WARNING("Failed to find primary vertex!");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_VERBOSE("Primary vertex has z = " << pv->z());
    }

    trkCont=0;
    ATH_CHECK( evtStore()->retrieve(trkCont, m_trkcoll) );

    // filterTracks(trkCont,pv);

    if(m_pflow) {
      pfoCont = 0;
      if( evtStore()->contains<xAOD::PFOContainer>("EtmissParticleFlowObjects") ) {
	ATH_CHECK(evtStore()->retrieve(pfoCont,"EtmissParticleFlowObjects"));
      } else {
	pfoCont = m_pfotool->retrievePFO(CP::EM, CP::all);
	ATH_CHECK( evtStore()->record(pfoCont,"EtmissParticleFlowObjects"));
      }
      if(!pfoCont) {
	ATH_MSG_WARNING("Unable to retrieve input pfo container");
	return StatusCode::FAILURE;
      }
    }
    return StatusCode::SUCCESS;
  }

  StatusCode METAssociator::finalize()
  {
    ATH_MSG_INFO ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 

  StatusCode METAssociator::fillAssocMap(xAOD::MissingETAssociationMap* metMap,
					 const xAOD::IParticleContainer* hardObjs)
  //					 std::vector<const xAOD::IParticle*>& mutracks)
  {
    const CaloClusterContainer* tcCont;
    const Vertex* pv;
    const TrackParticleContainer* trkCont;
    const PFOContainer* pfoCont;

    if (retrieveConstituents(tcCont,pv,trkCont,pfoCont).isFailure()) {
      ATH_MSG_WARNING("Unable to retrieve constituent containers");
      return StatusCode::FAILURE;
    }

    std::vector<const IParticle*> constlist;
    constlist.reserve(20);
    std::vector<const IParticle*> hardObjs_tmp;
    for(const auto& obj : *hardObjs) {
      hardObjs_tmp.push_back(obj);
    }
    std::sort(hardObjs_tmp.begin(),hardObjs_tmp.end(),greaterPt);

    for(const auto& obj : hardObjs_tmp) {
      if(obj->pt()<5e3 && obj->type()!=xAOD::Type::Muon) continue;
      constlist.clear();
      MissingETBase::Types::constvec_t tcvec,trkvec;
      ATH_MSG_VERBOSE( "Object type, pt, eta, phi = " << obj->type() << ", " << obj->pt() << ", " << obj->eta() << "," << obj->phi() );
      if (m_pflow) {
	ATH_CHECK( this->extractPFO(obj,constlist,tcvec,trkvec,pfoCont,pv) );
      } else {
        ATH_CHECK( this->extractTopoClusters(obj,constlist,tcvec,tcCont) );
        ATH_CHECK( this->extractTracks(obj,constlist,trkvec,tcCont,pv) );
      }
      bool inserted(false);
      inserted = MissingETComposition::insert(metMap,obj,constlist,tcvec,trkvec);
      if(inserted) {
	const MissingETAssociation* assoc = MissingETComposition::getAssociation(metMap,obj);
	ATH_MSG_VERBOSE( obj->type() << " is associated to jet " << assoc->refJet()->index() << " with pt " << assoc->refJet()->pt() );
      } else {
	ATH_MSG_VERBOSE( "Add " << obj->type() << " as miscellaneous object" );
	if(!obj->type()==xAOD::Type::Tau)
	  inserted = MissingETComposition::insertMisc(metMap,obj,constlist,tcvec,trkvec);
      }
    }  
    return StatusCode::SUCCESS;
  }
  
  void METAssociator::filterTracks(const xAOD::TrackParticleContainer* tracks,
				   const xAOD::Vertex* pv) {
    for(const auto& trk : *tracks) {
      m_goodtracks.clear();
      if(acceptTrack(trk,pv)) m_goodtracks.push_back(trk);
    }
  }
  
  // Accept Track
  ////////////////
  bool METAssociator::acceptTrack(const xAOD::TrackParticle* trk, const xAOD::Vertex* vx) const
  {
    //if(fabs(trk->pt())<500/*MeV*/ || fabs(trk->eta())>2.5) return false;
    // could add some error checking to make sure we successfully read the details
    //uint8_t nPixHits(0), nSctHits(0);
    //trk->summaryValue(nPixHits,xAOD::numberOfPixelHits);
    //if(nPixHits<1) return false;
    //trk->summaryValue(nSctHits,xAOD::numberOfSCTHits);
    //if(nSctHits<6) return false;
    //if(fabs(trk->d0())>1.5) return false;
    //if(fabs(trk->z0() + trk->vz() - vx->z()) > 1.5) return false;
    //return true;

    const Root::TAccept& accept = m_trkseltool->accept( *trk, vx );
    // uint8_t nBLHits(0), expectBLHit(false);
    // if(trk->summaryValue(nBLHits,xAOD::numberOfBLayerHits)) {
    //   ATH_MSG_VERBOSE("Track has " << (int) nBLHits << " b-layer hits");
    // }
    // if(trk->summaryValue(expectBLHit,xAOD::expectBLayerHit)) {
    //   ATH_MSG_VERBOSE("Track expected b-layer hit: " << (bool) expectBLHit);
    // }
    // ATH_MSG_VERBOSE("From auxdata: expect hit ? " << (bool) trk->auxdata<uint8_t>("expectBLayerHit")
    // 		    << " Nhits = " << (int) trk->auxdata<uint8_t>("numberOfBLayerHits"));
    
    // if(!accept && fabs(trk->z0() + trk->vz() - vx->z())*sin(trk->theta()) < 1.5) {
    //   for(size_t icut=0; icut<accept.getNCuts(); ++icut) {
    // 	ATH_MSG_VERBOSE("Cut " << accept.getCutName(icut) << ": result = " << accept.getCutResult(icut));
    //   }
    // }
    return accept;
  }


  bool METAssociator::acceptChargedPFO(const xAOD::TrackParticle* trk, const xAOD::Vertex* pv) const
  {
    if(fabs((trk->z0() - pv->z()+trk->vz())*sin(trk->theta())) > 2) return false;
    return true;
  }


  bool METAssociator::isGoodEoverP(const xAOD::TrackParticle* trk,const xAOD::CaloClusterContainer*& tcCont) const
  {

    if( (fabs(trk->eta())<1.5 && trk->pt()>200e3) ||
	(fabs(trk->eta())>=1.5 && trk->pt()>120e3) ) {

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
      for(const auto& clus : *tcCont) {
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

}

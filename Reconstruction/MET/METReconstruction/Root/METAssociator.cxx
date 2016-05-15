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
#include "AthContainers/ConstDataVector.h"

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
    m_trkseltool(""),
    m_trkIsolationTool(""),
    m_caloIsolationTool("")
  {
    ATH_MSG_INFO("METAssoc constructor");
    declareProperty( "InputCollection",    m_input_data_key                      );
    declareProperty( "PrimVxColl",         m_pvcoll      = "PrimaryVertices"     );
    declareProperty( "TrkColl",            m_trkcoll     = "InDetTrackParticles" );
    declareProperty( "ClusColl",           m_clcoll      = "CaloCalTopoClusters" );
    declareProperty( "UseTracks",          m_useTracks   = true                  );
    declareProperty( "PFlow",              m_pflow       = false                 );
    declareProperty( "UseRapidity",        m_useRapidity = false                 );
    declareProperty( "PFOTool",            m_pfotool                             );
    declareProperty( "UseIsolationTools",  m_useIsolationTools=false             );
    declareProperty( "TrackSelectorTool",  m_trkseltool                          );
    declareProperty( "TrackIsolationTool", m_trkIsolationTool                    );
    declareProperty( "CaloIsolationTool",  m_caloIsolationTool                   );
    declareProperty( "GoodTracksColl",     m_goodtracks_coll = "METRecoGoodTrackParticles" );
    declareProperty( "IgnoreJetConst",     m_skipconst = false                   );
    declareProperty( "ForwardColl",        m_forcoll   = ""            );
    declareProperty( "ForwardDef",         m_foreta    = 2.5                     );
  }

  // Destructor
  ///////////////
  METAssociator::~METAssociator()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METAssociator::initialize()
  {
    ATH_MSG_DEBUG ("Initializing " << name() << "...");

    ATH_CHECK( m_trkseltool.retrieve() );
    ATH_CHECK(m_trkIsolationTool.retrieve());
    ATH_CHECK(m_caloIsolationTool.retrieve());

    ATH_MSG_INFO("Should I use tracks? " << m_useTracks);

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
    if(m_pflow &&
       !m_useTracks
       ){
      ATH_MSG_WARNING("Attempting to build PFlow without a track collection.");
      return StatusCode::FAILURE;
    }

    return this->executeTool(metCont, metMap);
  }

  StatusCode METAssociator::retrieveConstituents(const xAOD::IParticleContainer*& tcCont,const xAOD::Vertex*& pv,const xAOD::TrackParticleContainer*& trkCont,const xAOD::PFOContainer*& pfoCont) const
  {
    ATH_MSG_DEBUG ("In execute: " << name() << "...");
    tcCont = 0;
    if (!m_skipconst || m_forcoll.empty()) {
      if( evtStore()->retrieve(tcCont, m_clcoll).isFailure() ) {
        ATH_MSG_WARNING("Unable to retrieve topocluster container " << m_clcoll << " for overlap removal");
        return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("Successfully retrieved topocluster collection");
    } else {
      TString hybridname = "Etmiss";
      hybridname += m_clcoll;
      hybridname += m_foreta;
      hybridname += m_forcoll;
      if( evtStore()->contains<xAOD::IParticleContainer>(hybridname.Data()) ) {
        ATH_CHECK(evtStore()->retrieve(tcCont,hybridname.Data()));
      } else {
        xAOD::IParticleContainer *hybridCont = new xAOD::IParticleContainer(SG::VIEW_ELEMENTS);

        const xAOD::IParticleContainer *centCont = 0;
        const xAOD::IParticleContainer *forCont = 0;
        if( evtStore()->retrieve(centCont, m_clcoll).isFailure() ) {
          ATH_MSG_WARNING("Unable to retrieve central container " << m_clcoll << " for overlap removal");
          return StatusCode::FAILURE;
        }
        if( evtStore()->retrieve(forCont, m_forcoll).isFailure() ) {
          ATH_MSG_WARNING("Unable to retrieve forward container " << m_forcoll << " for overlap removal");
          return StatusCode::FAILURE;
        }
        for(const auto& clus : *centCont) if (fabs(clus->eta())<m_foreta) hybridCont->push_back(clus);
        for(const auto& clus : *forCont) if (fabs(clus->eta())>=m_foreta) hybridCont->push_back(clus);
        ATH_CHECK( evtStore()->record(hybridCont,hybridname.Data()));
        tcCont = hybridCont;
      }
    }

    const VertexContainer *vxCont = 0;
    pv = 0;

    if( !m_useTracks){
      //if you want to skip tracks, set the track collection empty manually
      ATH_MSG_DEBUG("Skipping tracks");
    }else{
      if( evtStore()->retrieve(vxCont, m_pvcoll).isFailure() ) {
	ATH_MSG_WARNING("Unable to retrieve primary vertex container " << m_pvcoll);
	//this is actually really bad.  If it's empty that's okay
	return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("Successfully retrieved primary vertex container");

      for(const auto& vx : *vxCont) {
	if(vx->vertexType()==VxType::PriVtx)
	  {pv = vx; break;}
      }
      if(!pv) {
	ATH_MSG_WARNING("Failed to find primary vertex!");
      } else {
	ATH_MSG_VERBOSE("Primary vertex has z = " << pv->z());
      }

      trkCont=0;
      ATH_CHECK( evtStore()->retrieve(trkCont, m_trkcoll) );
      if(m_useIsolationTools) ATH_CHECK( filterTracks(trkCont,pv) );

      if(m_pflow) {
	pfoCont = 0;
	if( evtStore()->contains<xAOD::PFOContainer>("EtmissParticleFlowObjects") ) {
	  ATH_CHECK(evtStore()->retrieve(pfoCont,"EtmissParticleFlowObjects"));
	} else {
	  pfoCont = m_pfotool->retrievePFO(CP::EM, CP::all);
	  ATH_CHECK( evtStore()->record( const_cast<xAOD::PFOContainer*>(pfoCont),"EtmissParticleFlowObjects"));
	}
	if(!pfoCont) {
	  ATH_MSG_WARNING("Unable to retrieve input pfo container");
	  return StatusCode::FAILURE;
	}//pfoCont check
      }//pflow
    }//retrieve track/pfo containers

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
					 const xAOD::IParticleContainer* hardObjs) const
  //					 std::vector<const xAOD::IParticle*>& mutracks) const
  {
    const IParticleContainer* tcCont;
    const Vertex* pv;
    const TrackParticleContainer* trkCont;
    const PFOContainer* pfoCont;

    if (retrieveConstituents(tcCont,pv,trkCont,pfoCont).isFailure()) {
      ATH_MSG_DEBUG("Unable to retrieve constituent containers");
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
      ATH_MSG_VERBOSE( "Object type, pt, eta, phi = " << obj->type() << ", " << obj->pt() << ", " << obj->eta() << "," << obj->phi() );
      if (m_pflow) {
	if(!m_useTracks){
	  ATH_MSG_DEBUG("Attempting to build PFlow without a track collection.");
	  return StatusCode::FAILURE;
	}else{
	  std::map<const IParticle*,MissingETBase::Types::constvec_t> momentumOverride;
	  ATH_CHECK( this->extractPFO(obj,constlist,pfoCont,momentumOverride,pv) );
	  MissingETComposition::insert(metMap,obj,constlist,momentumOverride);
	}
      } else {
        ATH_CHECK( this->extractTopoClusters(obj,constlist,tcCont) );
	if(m_useTracks) ATH_CHECK( this->extractTracks(obj,constlist,tcCont,pv) );
        MissingETComposition::insert(metMap,obj,constlist);
      }
    }
    return StatusCode::SUCCESS;
  }

  StatusCode METAssociator::filterTracks(const xAOD::TrackParticleContainer* tracks,
				   const xAOD::Vertex* pv) const {
    ConstDataVector<TrackParticleContainer>* goodtracks(0);
    if( !evtStore()->contains<TrackParticleContainer>(m_goodtracks_coll) ) {
      goodtracks = new ConstDataVector<TrackParticleContainer>(SG::VIEW_ELEMENTS);
      ATH_CHECK( evtStore()->record(goodtracks, m_goodtracks_coll) );
      for(const auto& trk : *tracks) {
	if(acceptTrack(trk,pv)) goodtracks->push_back(trk);
      }
    }
    return StatusCode::SUCCESS;
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

    //todo check m_trkseltool exists
    //if (!m_trkseltool){
    //  ATH_MSG_WARNING("Tried to accept tracks with no TrackSelectionTool");
    //  return false;
    //}

    if (!vx) return false;//in events with no pv, we will just reject all tracks, and therefore build only the calo MET
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
    if(!pv) return false; /*reject pfo for events with no pv*/
    if(fabs((trk->z0() - pv->z()+trk->vz())*sin(trk->theta())) > 2) return false;
    return true;
  }


  bool METAssociator::isGoodEoverP(const xAOD::TrackParticle* trk,const xAOD::IParticleContainer*& tcCont) const
  {

    if( (fabs(trk->eta())<1.5 && trk->pt()>200e3) ||
	(fabs(trk->eta())>=1.5 && trk->pt()>120e3) ) {

      // Get relative error on qoverp
      float Rerr = Amg::error(trk->definingParametersCovMatrix(),4)/fabs(trk->qOverP());
      ATH_MSG_VERBOSE( "Track momentum error (%): " << Rerr*100 );

      // first compute track and calo isolation variables
      float ptcone20 = 0., isolfrac = 0., etcone10 = 0., EoverP = 0.;
      if(!m_useIsolationTools) {
        ATH_MSG_VERBOSE( "Using OLD track isolation setup");
        // ptcone
	const TrackParticleContainer* goodtracks(0);
	ATH_CHECK( evtStore()->retrieve(goodtracks, m_goodtracks_coll) );
        for(const auto& testtrk : *goodtracks) {
          if(testtrk==trk) continue;
          if (xAOD::P4Helpers::isInDeltaR(*testtrk,*trk,0.2,m_useRapidity)) {
	        ptcone20 += testtrk->pt();
          } 
        }
        isolfrac = ptcone20 / trk->pt();
        // etcone
        for(const auto& clus : *tcCont) {
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
	    else if (EoverP<0.65 && (EoverP>0.1 || Rerr>0.1)) return false;
      } else {
	    // non-isolated track cuts
	    float trkptsum = ptcone20+trk->pt();
	    if(etcone10/trkptsum<0.6 && trk->pt()/trkptsum>0.6) return false;
      }
    }
    return true;
  }

}

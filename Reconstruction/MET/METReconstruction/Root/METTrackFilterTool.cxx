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

// Track errors
#include "EventPrimitives/EventPrimitivesHelpers.h"

namespace met {

  using std::vector;
  //
  using xAOD::IParticle;
  //
  using xAOD::TrackParticle;
  using xAOD::TrackParticleContainer;
  using xAOD::VertexContainer;
  //
  using xAOD::CaloCluster;
  using xAOD::CaloClusterContainer;
  //
  using xAOD::MissingET;
  using xAOD::MissingETComposition;
  using xAOD::MissingETComponent;
  using xAOD::MissingETComponentMap;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 

  // Constructors
  ////////////////
  METTrackFilterTool::METTrackFilterTool(const std::string& name) : 
    AsgTool(name),
    METRefinerTool(name)
  {
    declareProperty( "DoPVSel",         m_trk_doPVsel = true                 );
    declareProperty( "TrackD0Max",      m_trk_d0Max = 1.5                    );
    declareProperty( "TrackZ0Max",      m_trk_z0Max = 1.5                    );
    declareProperty( "InputPVKey",      m_pv_inputkey = "PrimaryVertices"    );
    declareProperty( "DoEoverPSel",     m_trk_doEoverPsel = true             );
    declareProperty( "InputClusterKey", m_cl_inputkey = "CaloCalTopoCluster" );
  }

  // Destructor
  ///////////////
  METTrackFilterTool::~METTrackFilterTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METTrackFilterTool::initialize()
  {
    ATH_MSG_INFO ("Initializing " << name() << "...");

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
  bool METTrackFilterTool::isPVTrack(const xAOD::TrackParticle* trk,
				     const xAOD::Vertex* pv) const
  {

    if(trk->d0()>m_trk_d0Max) return false;
    if(fabs(trk->z0()+trk->vz() - pv->z()) > m_trk_z0Max) return false;

    return true;
  }

  bool METTrackFilterTool::isGoodEoverP(const xAOD::TrackParticle* trk,
					const std::vector<const xAOD::IParticle*>& trkList,
					const xAOD::CaloClusterContainer* clusters) const {

    if( (fabs(trk->eta())<1.5 && trk->pt()>200e3) ||
    	(fabs(trk->eta())>=1.5 && trk->pt()>120e3) ) {

      // Get relative error on qoverp
      float Rerr = Amg::error(trk->definingParametersCovMatrix(),4)/fabs(trk->qOverP());
      ATH_MSG_VERBOSE( "Track momentum error (%): " << Rerr*100 );

      // first compute track and calo isolation variables
      float ptcone20 = 0;
      for(std::vector<const IParticle*>::const_iterator iTrk=trkList.begin();
	  iTrk!=trkList.end(); ++iTrk) {
	const TrackParticle* testtrk = dynamic_cast<const TrackParticle*>(*iTrk);
	if(testtrk==trk) continue;
	if(testtrk->p4().DeltaR(trk->p4()) < 0.2) {
	  ptcone20 += testtrk->pt();
	}
      }
      float isolfrac = ptcone20 / trk->pt();
      ATH_MSG_VERBOSE( "Track isolation fraction: " << isolfrac );

      float etcone10 = 0.;
      for(CaloClusterContainer::const_iterator iClus=clusters->begin();
	  iClus!=clusters->end(); ++iClus) {
	if((*iClus)->p4().DeltaR(trk->p4()) < 0.1) {
	  etcone10 += (*iClus)->pt();
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

  StatusCode METTrackFilterTool::executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap) {

    ATH_MSG_DEBUG ("In execute: " << name() << "...");

    if(m_trk_doPVsel) {
      m_pv_cont = 0;
      if( evtStore()->retrieve( m_pv_cont, m_pv_inputkey).isFailure() ) {
        ATH_MSG_WARNING("Unable to retrieve input primary vertex container");
        return StatusCode::SUCCESS;
      }
      if(m_pv_cont->size()>0) {
	ATH_MSG_DEBUG("Main primary vertex has z = " << (*m_pv_cont)[0]->z());
      } else{
	ATH_MSG_WARNING("Event has no primary vertices");
	return StatusCode::SUCCESS;
      }
    }

    const CaloClusterContainer* cl_cont = 0;
    if(m_trk_doEoverPsel) {
      if( evtStore()->retrieve( cl_cont, m_cl_inputkey).isFailure() ) {
        ATH_MSG_WARNING("Unable to retrieve input calocluster container");
        return StatusCode::SUCCESS;
      }
    }

    MissingETComponentMap::iterator iter = MissingETComposition::find(metMap,metTerm);
    if(iter==metMap->end()) {
      ATH_MSG_WARNING("Could not find current METComponent in MET Map!");
      return StatusCode::SUCCESS;
    }
    MissingETComponent* newComp = *iter;
    newComp->setStatusWord(MissingETBase::Status::Tags::correctedTerm(MissingETBase::Status::Nominal,
								      MissingETBase::Status::PileupTrack));

    // Extract the component corresponding to the Track SoftTerms
    MissingETBase::Types::bitmask_t src_ST_idTrk = MissingETBase::Source::SoftEvent | MissingETBase::Source::idTrack();
    MissingETBase::Types::bitmask_t src_ST_refTrk = MissingETBase::Source::softEvent() | MissingETBase::Source::track();
    MissingETComponentMap::const_iterator citer = MissingETComposition::find(metMap,src_ST_idTrk);
    if(citer==metMap->end()) {
      ATH_MSG_WARNING("Could not find Soft ID Track component in MET Map!");
      return StatusCode::SUCCESS;
    }
    vector<const IParticle*> softTrackList = (*citer)->objects();

    metTerm->setSource(src_ST_refTrk);

    vector<const IParticle*> dummyList;

    // Loop over the tracks and select only good ones
    for( vector<const IParticle*>::const_iterator iPar=softTrackList.begin();
	 iPar!=softTrackList.end(); ++iPar ) {
      MissingETBase::Types::weight_t trackWeight = (*citer)->weight(*iPar);
      const TrackParticle* trk = dynamic_cast<const TrackParticle*>(*iPar);
      ATH_MSG_VERBOSE("Filter track with pt " << trk->pt());
      // Could/should use common implementation of addToMET here -- derive builder and refiner from a common base tool?
      bool passFilters = true;
      if(m_trk_doPVsel && !isPVTrack(trk,(*m_pv_cont)[0])) passFilters = false;
      if(m_trk_doEoverPsel && !isGoodEoverP(trk,softTrackList,cl_cont)) passFilters = false;
      if(passFilters) {
	ATH_MSG_VERBOSE("Add to MET.");
	metTerm->add(trk->pt()*cos(trk->phi())*trackWeight.wpx(),
		     trk->pt()*sin(trk->phi())*trackWeight.wpy(),
		     trk->pt()*trackWeight.wet());
	MissingETComposition::insert(metMap,metTerm,*iPar,dummyList,trackWeight);
      }
    }
    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

}


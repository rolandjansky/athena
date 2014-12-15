///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METSoftAssociator.cxx 
// Implementation file for class METSoftAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METSoftAssociator.h"

namespace met {

  using namespace xAOD;

  // Constructors
  ////////////////
  METSoftAssociator::METSoftAssociator(const std::string& name) : 
    AsgTool(name),
    METAssociator(name)
  {
  }

  // Destructor
  ///////////////
  METSoftAssociator::~METSoftAssociator()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METSoftAssociator::initialize()
  {
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");

    return StatusCode::SUCCESS;
  }

  StatusCode METSoftAssociator::finalize()
  {
    ATH_MSG_VERBOSE ("Finalizing " << name() << "...");
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

  // executeTool
  ////////////////
  StatusCode METSoftAssociator::executeTool(xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap) 
  {

    ATH_MSG_VERBOSE ("In execute: " << name() << "...");
    const xAOD::CaloClusterContainer* tcCont;
    const xAOD::Vertex* pv;
    const xAOD::TrackParticleContainer* trkCont;
    const xAOD::PFOContainer* pfoCont;
    if (retrieveConstituents(tcCont,pv,trkCont,pfoCont).isFailure()) {
      ATH_MSG_WARNING("Unable to retrieve constituent containers");
      return StatusCode::FAILURE;
    }

    // Create a MissingETContainer with its aux store
    MissingET* metCoreCl = new MissingET(0.,0.,0.,"SoftClusCore",MissingETBase::Source::softEvent() | MissingETBase::Source::cluster());
    metCont->push_back(metCoreCl);
    MissingET* metCoreTrk = new MissingET(0.,0.,0.,"PVSoftTrkCore",MissingETBase::Source::softEvent() | MissingETBase::Source::track());
    metCont->push_back(metCoreTrk);
    if (m_pflow) {
      const IParticleContainer* uniquePFOs = metMap->getUniqueSignals(pfoCont,MissingETBase::UsageHandler::Policy::ParticleFlow);
      for(const auto& sig : *uniquePFOs) {
	const PFO *pfo = dynamic_cast<const PFO*>(sig);
	if (pfo->charge()!=0) {
	  if (acceptChargedPFO(pfo->track(0),pv)) {
	    *metCoreTrk += sig;
	    *metCoreCl += sig;
	  }
	} else {
	  TLorentzVector corrected = pfo->GetVertexCorrectedEMFourVec(*pv);
	  if (pfo->eEM()>0) metCoreCl->add(corrected.Px(),corrected.Py(),corrected.Pt());
	}
      }
    } else {
      const IParticleContainer* uniqueClusters = metMap->getUniqueSignals(tcCont);
      for(const auto& cl : *uniqueClusters) {
	if (cl->e()>0) *metCoreCl += cl;
      }
      const IParticleContainer* uniqueTracks = metMap->getUniqueSignals(trkCont);
      for(const auto& trk : *uniqueTracks) {
	// if(acceptTrack(dynamic_cast<const TrackParticle*>(trk),pv) && isGoodEoverP(dynamic_cast<const TrackParticle*>(trk),tcCont)) {
	ATH_MSG_VERBOSE("Test core track with pt " << trk->pt());
	if(acceptTrack(dynamic_cast<const TrackParticle*>(trk),pv)) {
	  ATH_MSG_VERBOSE("Add core track with pt " << trk->pt());
	  *metCoreTrk += trk;
	}
      }
    }
    return StatusCode::SUCCESS;
  }

}

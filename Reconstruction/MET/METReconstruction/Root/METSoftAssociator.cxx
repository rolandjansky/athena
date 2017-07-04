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
#include "xAODCaloEvent/CaloClusterChangeSignalState.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace met {

  using namespace xAOD;
  static const SG::AuxElement::Decorator<std::vector<ElementLink<IParticleContainer> > > dec_softConst("softConstituents");

  // Constructors
  ////////////////
  METSoftAssociator::METSoftAssociator(const std::string& name) :
    AsgTool(name),
    METAssociator(name)
  {
    declareProperty("DecorateSoftConst", m_decorateSoftTermConst=false);
    declareProperty("LCModClusterKey",   m_lcmodclus_key = "LCOriginTopoClusters");
    declareProperty("EMModClusterKey",   m_emmodclus_key = "EMOriginTopoClusters");
  }

  // Destructor
  ///////////////
  METSoftAssociator::~METSoftAssociator()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METSoftAssociator::initialize()
  {
    ATH_CHECK( METAssociator::initialize() );
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
  StatusCode METSoftAssociator::executeTool(xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap) const
  {

    // Add MET terms to the container
    // Always do this in order that the terms exist even if the method fails
    MissingET* metCoreCl = new MissingET(0.,0.,0.,"SoftClusCore",MissingETBase::Source::softEvent() | MissingETBase::Source::clusterLC());
    metCont->push_back(metCoreCl);
    MissingET* metCoreTrk = new MissingET(0.,0.,0.,"PVSoftTrkCore",MissingETBase::Source::softEvent() | MissingETBase::Source::track());
    metCont->push_back(metCoreTrk);

    ATH_MSG_VERBOSE ("In execute: " << name() << "...");
    met::METAssociator::ConstitHolder constits;
    if (retrieveConstituents(constits).isFailure()) {
      ATH_MSG_WARNING("Unable to retrieve constituent containers");
      return StatusCode::FAILURE;
    }

    if (m_pflow) {
      const IParticleContainer* uniquePFOs = metMap->getUniqueSignals(constits.pfoCont,MissingETBase::UsageHandler::Policy::ParticleFlow);
      if(m_decorateSoftTermConst) {
        dec_softConst(*metCoreTrk) = std::vector<ElementLink<IParticleContainer> >();
        dec_softConst(*metCoreTrk).reserve(uniquePFOs->size());
        dec_softConst(*metCoreCl) = std::vector<ElementLink<IParticleContainer> >();
        dec_softConst(*metCoreCl).reserve(uniquePFOs->size());
      }
      for(const auto& sig : *uniquePFOs) {
	const PFO *pfo = static_cast<const PFO*>(sig);
	if (fabs(pfo->charge())>FLT_MIN) { // Charged PFOs
	  if (pfo->e()>FLT_MIN && ( !m_cleanChargedPFO || isGoodEoverP(pfo->track(0)) ) ) {
	    // For the TST, we add the track pt, as this need not be
	    // corrected for nearby energy in the calo
	    *metCoreTrk += pfo->track(0);
	    // For CST we add the PFO pt, which is weighted down
	    // to account for energy in the calo that may not have
	    // been subtracted
	    *metCoreCl  += sig;
	    if(m_decorateSoftTermConst) {
	      dec_softConst(*metCoreTrk).push_back(ElementLink<IParticleContainer>(*static_cast<const IParticleContainer*>(sig->container()),sig->index()));
	      dec_softConst(*metCoreCl).push_back(ElementLink<IParticleContainer>(*static_cast<const IParticleContainer*>(sig->container()),sig->index()));
	    }
	  }
	} else { // Neutral PFOs
	  if (pfo->e()>FLT_MIN) {
	    // This is a non-issue; just add the four-vector
 	    *metCoreCl += sig;
 	    if(m_decorateSoftTermConst) dec_softConst(*metCoreCl).push_back(ElementLink<IParticleContainer>(*static_cast<const IParticleContainer*>(sig->container()),sig->index()));
	  }
	}
      }
      delete uniquePFOs;
    } else {
      MissingET* metCoreEMCl = new MissingET(0.,0.,0.,"SoftClusEMCore",MissingETBase::Source::softEvent() | MissingETBase::Source::clusterEM());
      metCont->push_back(metCoreEMCl);
      const IParticleContainer* uniqueClusters = metMap->getUniqueSignals(constits.tcCont,MissingETBase::UsageHandler::AllCalo);
      const IParticleContainer* uniqueTracks = constits.trkCont == NULL ? new const IParticleContainer() : metMap->getUniqueSignals(constits.trkCont);
      if(m_decorateSoftTermConst) {
        dec_softConst(*metCoreTrk) = std::vector<ElementLink<IParticleContainer> >();
        dec_softConst(*metCoreTrk).reserve(uniqueTracks->size());
        dec_softConst(*metCoreCl) = std::vector<ElementLink<IParticleContainer> >();
        dec_softConst(*metCoreCl).reserve(uniqueClusters->size());
      }
      const CaloClusterContainer *lctc(0), *emtc(0);
      if(m_useModifiedClus) {
	ATH_CHECK( evtStore()->retrieve(lctc,m_lcmodclus_key) );
	ATH_CHECK( evtStore()->retrieve(emtc,m_emmodclus_key) );
      }
      for(const auto& cl : *uniqueClusters) {
	if (cl->e()>FLT_MIN) {
	  if(m_useModifiedClus) {
	    if(lctc && emtc) {
	      size_t cl_idx(cl->index());
	      // clusters at LC scale
	      *metCoreCl += (*lctc)[cl_idx];
	      if(m_decorateSoftTermConst) dec_softConst(*metCoreCl).push_back(ElementLink<IParticleContainer>(*static_cast<const IParticleContainer*>(lctc),cl->index()));
	      // clusters at EM scale
	      *metCoreEMCl += (*emtc)[cl_idx];
	    } else {
	      ATH_MSG_WARNING("Invalid LC/EM modified cluster collections -- cannot add cluster to soft term!");
	    }
	  } else {
	    // clusters at LC scale
	    if (cl->type()==xAOD::Type::CaloCluster) {
	      CaloClusterChangeSignalState statehelperLC(static_cast<const CaloCluster*>(cl),xAOD::CaloCluster::CALIBRATED);
	      *metCoreCl += cl;
	    } else *metCoreCl += cl;
	    if(m_decorateSoftTermConst) dec_softConst(*metCoreCl).push_back(ElementLink<IParticleContainer>(*static_cast<const IParticleContainer*>(cl->container()),cl->index()));
	    // clusters at EM scale
	    if (cl->type()==xAOD::Type::CaloCluster) {
	      CaloClusterChangeSignalState statehelperEM(static_cast<const CaloCluster*>(cl),xAOD::CaloCluster::UNCALIBRATED);
	      *metCoreEMCl += cl;
	    } else *metCoreEMCl += cl;
	  }
	}
      }

      if(constits.pv) {
	for(const auto& trk : *uniqueTracks) {
	  ATH_MSG_VERBOSE("Test core track with pt " << trk->pt());
	  if(acceptTrack(static_cast<const TrackParticle*>(trk),constits.pv) && isGoodEoverP(static_cast<const TrackParticle*>(trk))) {
	    ATH_MSG_VERBOSE("Add core track with pt " << trk->pt());
	    *metCoreTrk += trk;
	    if(m_decorateSoftTermConst) dec_softConst(*metCoreTrk).push_back(ElementLink<IParticleContainer>(*static_cast<const IParticleContainer*>(trk->container()),trk->index()));

	  }
	}
      }
      delete uniqueClusters;
      delete uniqueTracks;
    }
    return StatusCode::SUCCESS;
  }
}

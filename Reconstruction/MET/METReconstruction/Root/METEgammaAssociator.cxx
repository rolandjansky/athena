///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METEgammaAssociator.cxx 
// Implementation file for class METEgammaAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METEgammaAssociator.h"

// Egamma EDM
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

// Calo helpers
#include "xAODCaloEvent/CaloClusterChangeSignalState.h"

// Tracking EDM
#include "xAODTracking/Vertex.h"

namespace met {

  using xAOD::IParticle;
  //
  using xAOD::Egamma;
  //
  using xAOD::CaloCluster;
  //
  using xAOD::VertexContainer;

  // Constructors
  ////////////////
  METEgammaAssociator::METEgammaAssociator(const std::string& name) : 
    AsgTool(name),
    METAssociator(name)
  {
    declareProperty( "TCMatchDeltaR",     m_tcMatch_dR        = 0.1 );
    declareProperty( "TCMatchMaxRat",     m_tcMatch_maxRat    = 1.5 );
    declareProperty( "TCMatchTolerance",  m_tcMatch_tolerance = 0.2 );
    declareProperty( "TCMatchMethod",     m_tcMatch_method    = 0   );
  }

  // Destructor
  ///////////////
  METEgammaAssociator::~METEgammaAssociator()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METEgammaAssociator::initialize()
  {
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  StatusCode METEgammaAssociator::finalize()
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

  //**********************************************************************
  // Get Egamma constituents
  StatusCode METEgammaAssociator::extractTopoClusters(const xAOD::IParticle* obj,
					std::vector<const xAOD::IParticle*>& tclist,
					MissingETBase::Types::constvec_t& tcvec,
				        const xAOD::CaloClusterContainer* tcCont)
  {
    const Egamma *eg = static_cast<const xAOD::Egamma*>(obj);
    // safe to assume a single SW cluster?
    // will do so for now...
    const CaloCluster* swclus = eg->caloCluster();
    double eg_cl_e = swclus->e();

    // the matching strategy depends on how the cluster container is sorted
    // easier if it's sorted in descending pt order
    // we'll worry about optimisation later
    std::vector<const xAOD::CaloCluster*> nearbyTC;
    CaloClusterChangeSignalStateList stateHelperList;
    nearbyTC.reserve(10);
    for(const auto& cl : *tcCont) {
      // this can probably be done more elegantly
      double dR = swclus->p4().DeltaR(cl->p4());
      if(dR<m_tcMatch_dR && cl->e()>0) {
	// could consider also requirements on the EM fraction or depth
	nearbyTC.push_back(cl);
	stateHelperList.add(cl, CaloCluster::State(m_signalstate));
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
      if(tcl_e>m_tcMatch_maxRat*eg_cl_e) {
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

    return StatusCode::SUCCESS;
  }

}

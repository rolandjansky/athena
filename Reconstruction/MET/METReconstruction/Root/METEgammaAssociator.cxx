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
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaFwd.h"

// Tracking EDM
#include "xAODTracking/Vertex.h"

namespace met {

  using namespace xAOD;

  // Constructors
  ////////////////
  METEgammaAssociator::METEgammaAssociator(const std::string& name) : 
    AsgTool(name),
    METAssociator(name)
  {
    // 0 is delta-R geometrical matching
    // 1 is using TopoClusterLink decoration on clusters
    declareProperty( "TCMatchMethod",     m_tcMatch_method = DeltaR );

    declareProperty( "TCMatchMaxRat",     m_tcMatch_maxRat = 1.5    );
    declareProperty( "TCMatchDeltaR",     m_tcMatch_dR     = 0.1    );
  }

  // Destructor
  ///////////////
  METEgammaAssociator::~METEgammaAssociator()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METEgammaAssociator::initialize()
  {
    ATH_CHECK( METAssociator::initialize() );
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");
    switch(m_tcMatch_method) {
    case DeltaR: ATH_MSG_INFO("Egamma-topocluster matching uses DeltaR check."); break;
    case ClusterLink: ATH_MSG_INFO("Egamma-topocluster matching uses topocluster links."); break;
    default:
      ATH_MSG_WARNING( "Invalid topocluster match method configured!" );
      return StatusCode::FAILURE;
    } 
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
				        const xAOD::IParticleContainer* tcCont) const
  {
    const Egamma *eg = static_cast<const Egamma*>(obj);
    // safe to assume a single SW cluster?
    // will do so for now...
    const CaloCluster* swclus = eg->caloCluster();

    // the matching strategy depends on how the cluster container is sorted
    // easier if it's sorted in descending pt order
    // we'll worry about optimisation later
    std::vector<const IParticle*> inputTC;
    inputTC.reserve(10);
    
    if(m_tcMatch_method==DeltaR) {
      for(const auto& cl : *tcCont) {
	// this can probably be done more elegantly
	double dR = P4Helpers::deltaR(*swclus,*cl,m_useRapidity);
	if(dR<m_tcMatch_dR && cl->e()>0) {
	  // could consider also requirements on the EM fraction or depth
	  inputTC.push_back(cl);
	} // match TC in a cone around SW cluster
      }
      ATH_MSG_VERBOSE("Found " << inputTC.size() << " nearby topoclusters");
      std::sort(inputTC.begin(),inputTC.end(),greaterPt);
    } else if(m_tcMatch_method==ClusterLink) {
      static const SG::AuxElement::ConstAccessor<std::vector<ElementLink<CaloClusterContainer> > > tcLinkAcc("constituentClusterLinks");
      // Fill a vector of vectors
      for(const auto& el : tcLinkAcc(*swclus)) {
	inputTC.push_back(*el);
      }
      ATH_MSG_VERBOSE("Found " << inputTC.size() << " linked topoclusters");
    } else {
      ATH_MSG_WARNING( "Invalid topocluster match method configured!" );
      return StatusCode::FAILURE;
    } 

    ATH_CHECK( selectEgammaClusters(swclus, inputTC, tclist) );

    return StatusCode::SUCCESS;
  }

  StatusCode METEgammaAssociator::selectEgammaClusters(const xAOD::CaloCluster* swclus,
						       const std::vector<const IParticle*>& inputTC,
						       std::vector<const xAOD::IParticle*>& tclist) const
  {    
    double eg_cl_e = swclus->e();

    bool doSum = true;
    double sumE_tc = 0.;
    const IParticle* bestbadmatch = 0;
    for(const auto& cl : inputTC) {
      double tcl_e = cl->e();
      // skip cluster if it's above our bad match threshold
      // retain pointer of the closest matching cluster in case no better is found
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
      } // if we will retain the topocluster
    } // loop over nearby clusters
    if(sumE_tc<1e-9 && bestbadmatch) {
      ATH_MSG_VERBOSE("No better matches found -- add bad match topocluster with pt "
    		      << bestbadmatch->pt() << ", e " << bestbadmatch->e() << ".");
      tclist.push_back(bestbadmatch);
    }
    return StatusCode::SUCCESS;
  }

}

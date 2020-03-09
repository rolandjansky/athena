///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// xAODClusterCompressor.cxx 
// Implementation file for class xAODClusterCompressor
// Author: Walter Lampl 

/////////////////////////////////////////////////////////////////// 

// CaloUtils includes
#include "CaloUtils/xAODClusterCompressor.h"
#include <bitset>

// Constructors
////////////////
xAODClusterCompressor::xAODClusterCompressor(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_allMoments({{xAOD::CaloCluster::FIRST_PHI ,xAOD::CaloCluster::FIRST_ETA ,xAOD::CaloCluster::SECOND_R ,xAOD::CaloCluster::SECOND_LAMBDA ,xAOD::CaloCluster::DELTA_PHI ,xAOD::CaloCluster::DELTA_THETA ,xAOD::CaloCluster::DELTA_ALPHA ,xAOD::CaloCluster::CENTER_X ,xAOD::CaloCluster::CENTER_Y ,xAOD::CaloCluster::CENTER_Z ,xAOD::CaloCluster::CENTER_MAG ,xAOD::CaloCluster::CENTER_LAMBDA ,xAOD::CaloCluster::LATERAL ,xAOD::CaloCluster::LONGITUDINAL ,xAOD::CaloCluster::ENG_FRAC_EM ,xAOD::CaloCluster::ENG_FRAC_MAX ,xAOD::CaloCluster::ENG_FRAC_CORE ,xAOD::CaloCluster::FIRST_ENG_DENS ,xAOD::CaloCluster::SECOND_ENG_DENS ,xAOD::CaloCluster::ISOLATION ,xAOD::CaloCluster::ENG_BAD_CELLS ,xAOD::CaloCluster::N_BAD_CELLS ,xAOD::CaloCluster::N_BAD_CELLS_CORR ,xAOD::CaloCluster::BAD_CELLS_CORR_E ,xAOD::CaloCluster::BADLARQ_FRAC ,xAOD::CaloCluster::ENG_POS ,xAOD::CaloCluster::SIGNIFICANCE ,xAOD::CaloCluster::CELL_SIGNIFICANCE ,xAOD::CaloCluster::CELL_SIG_SAMPLING ,xAOD::CaloCluster::AVG_LAR_Q ,xAOD::CaloCluster::AVG_TILE_Q ,xAOD::CaloCluster::EM_PROBABILITY ,xAOD::CaloCluster::HAD_WEIGHT ,xAOD::CaloCluster::OOC_WEIGHT ,xAOD::CaloCluster::DM_WEIGHT ,xAOD::CaloCluster::TILE_CONFIDENCE_LEVEL ,xAOD::CaloCluster::VERTEX_FRACTION ,xAOD::CaloCluster::NVERTEX_FRACTION ,xAOD::CaloCluster::ENG_CALIB_TOT ,xAOD::CaloCluster::ENG_CALIB_OUT_L ,xAOD::CaloCluster::ENG_CALIB_OUT_M ,xAOD::CaloCluster::ENG_CALIB_OUT_T ,xAOD::CaloCluster::ENG_CALIB_DEAD_L ,xAOD::CaloCluster::ENG_CALIB_DEAD_M ,xAOD::CaloCluster::ENG_CALIB_DEAD_T ,xAOD::CaloCluster::ENG_CALIB_EMB0 ,xAOD::CaloCluster::ENG_CALIB_EME0 ,xAOD::CaloCluster::ENG_CALIB_TILEG3 ,xAOD::CaloCluster::ENG_CALIB_DEAD_TOT ,xAOD::CaloCluster::ENG_CALIB_DEAD_EMB0 ,xAOD::CaloCluster::ENG_CALIB_DEAD_TILE0 ,xAOD::CaloCluster::ENG_CALIB_DEAD_TILEG3 ,xAOD::CaloCluster::ENG_CALIB_DEAD_EME0 ,xAOD::CaloCluster::ENG_CALIB_DEAD_HEC0 ,xAOD::CaloCluster::ENG_CALIB_DEAD_FCAL ,xAOD::CaloCluster::ENG_CALIB_DEAD_LEAKAGE ,xAOD::CaloCluster::ENG_CALIB_DEAD_UNCLASS ,xAOD::CaloCluster::ENG_CALIB_FRAC_EM ,xAOD::CaloCluster::ENG_CALIB_FRAC_HAD ,xAOD::CaloCluster::ENG_CALIB_FRAC_REST}}),
  //This listd DOES NOT contain the egamma-related moments: ETACALOFRAME, PHICALOFRAME, ETA1CALOFRAME, PHI1CALOFRAME, ETA2CALOFRAME, PHI2CALOFRAME
  //Would be nicer to be jobO congfigurable (ideally per container) -> ENUMS? String-to-enum matching?
  m_compressor(7)

{
  declareProperty( "Enable", m_isEnabled=true);
}


StatusCode xAODClusterCompressor::initialize() {
  if (m_isEnabled) 
    ATH_MSG_INFO("Compression of ClusterMoments to 16 bits enabled");
  else 
    ATH_MSG_INFO("Compression of ClusterMoments disabled");
  return StatusCode::SUCCESS;
}


void xAODClusterCompressor::compress(xAOD::CaloClusterContainer* clustercontainer) const {

  if (!m_isEnabled) return;
  if (clustercontainer->empty()) return;

  if (!clustercontainer->hasNonConstStore()) {
    ATH_MSG_DEBUG("ClusterContainer has no non-const aux store. No compression.");
    return;
  }

  constexpr size_t nMoments=std::tuple_size<momentList_t>::value;
    
  std::bitset<nMoments> incompressibleMoments; //Idices of moments that are const or inexistant


  for (xAOD::CaloCluster* cluster : *clustercontainer) { //Outer loop over clusters
    for (size_t iMom=0; iMom<nMoments; ++iMom) { //Inner loop over moments
      if (!incompressibleMoments.test(iMom)) {//Moment not (yet) known to be incompressible
	double momValue;
	const xAOD::CaloCluster::MomentType mom= m_allMoments[iMom];
	if (cluster->retrieveMoment(mom,momValue)) {
	  cluster->insertMoment(mom,m_compressor.reduceFloatPrecision(momValue));
	}
	else {//moment doesn't exist
	   incompressibleMoments.set(iMom);
	  ATH_MSG_VERBOSE("Moment " << mom << " doesn not exists. Not attemps to compress it in subsequent clusters");
	}
      }//end if Moment not yet known to be incompressible
    }//end loop over moments
  }//end loop over clusters

  return;
}


StatusCode xAODClusterCompressor::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IxAODClusterCompressor::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IxAODClusterCompressor*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return ::AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

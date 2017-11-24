/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ClusterDumper.cxx 592171 2014-04-09 14:47:05Z wlampl $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

// Local include(s):
#include "ClusterDumper.h"

ClusterDumper::ClusterDumper( const std::string& name,
				ISvcLocator* svcLoc )
  : AthAlgorithm( name, svcLoc ),
    m_out(&std::cout)
{
  declareProperty( "ContainerName",m_containerName);
  declareProperty( "FileName",m_fileName);
  
}

StatusCode ClusterDumper::initialize() {
  ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

  if (m_fileName.size()!=0) {
    m_fileOut.open(m_fileName);
    if (m_fileOut.is_open()) {
      m_out=&m_fileOut;
      ATH_MSG_INFO("Writing to file " << m_fileName);
    }
    else {
      msg(MSG::ERROR) << "Failed to open file " << m_fileName << endreq;
      return StatusCode::FAILURE;
    }
  }
  else
    ATH_MSG_INFO("Writing to stdout");
  return StatusCode::SUCCESS;
}


StatusCode ClusterDumper::finalize() {
  if (m_fileOut.is_open()) {
    m_fileOut.close();
  }
   return StatusCode::SUCCESS;
}
 
StatusCode ClusterDumper::execute() {

  const xAOD::CaloClusterContainer* clustercontainer = 0;
  CHECK( evtStore()->retrieve(clustercontainer,m_containerName));
  ATH_MSG_DEBUG( "Retrieved clusters with key: " << m_containerName );

  const CaloClusterCellLinkContainer* cclptr=0;
  if (evtStore()->contains<CaloClusterCellLinkContainer>(m_containerName+"_links")) {
    CHECK(evtStore()->retrieve(cclptr,m_containerName+"_links"));
    ATH_MSG_INFO("Found corresponding cell-link container with size " << cclptr->size());
  }
  else
    ATH_MSG_INFO("Did not find corresponding cell-link container");

  
  for (const auto& itr: *clustercontainer) {
    const xAOD::CaloCluster& cluster=*itr;
    (*m_out) << "Kinematics :" << std::endl;
    (*m_out) << "E=" << cluster.e() << ", eta=" << cluster.eta() << ", phi=" << cluster.phi() << ", m=" << cluster.m() << ", pt=" << cluster.pt() << std::endl;
    (*m_out) << "Eta0=" << cluster.eta0() << ", Phi0=" << cluster.phi0() << std::endl;

    (*m_out) << "TLorentzVector :" << std::endl;
    const xAOD::CaloCluster::FourMom_t& p4=cluster.p4();
    (*m_out) << " p4.E=" << p4.E() << ", x=" << p4.X() << ", y=" << p4.Y() << ", z=" << p4.Z() << ", m=" << p4.M() << ", pt=" << p4.Pt() << std::endl;

    (*m_out) << "Sampling variables :" << std::endl;
    for (unsigned iSamp=0;iSamp<CaloSampling::Unknown;++iSamp) {
	xAOD::CaloCluster_v1::CaloSample s=(CaloSampling::CaloSample)iSamp;
	if (cluster.hasSampling(s)) {
	  (*m_out) << "  Sampling #" << s << ": E=" <<  cluster.eSample(s) << ", eta=" << cluster.etaSample(s) << ", phi=" << cluster.phiSample(s) << std::endl;
	}
      }
      
    

    //(*m_out) << "Auxiliary variables: " << std::endl;
    // const SG::auxid_set_t& auxIds=cluster.container()->getAuxIDs(); //->getDynamicAuxIDs();
    // const size_t idx= cluster.index();
    // for (auto ai: auxIds) {
    //   const std::string& auxName=SG::AuxTypeRegistry::instance().getName(ai);
    //   const std::type_info* ti=SG::AuxTypeRegistry::instance().getType (ai);
    //   if ((*ti)==typeid(float)) {
    // 	const float v=clustercontainer->getData<float>(ai,idx);
    // 	(*m_out) << "  Index=" <<idx << ", Auxid=" << ai << ", Name=" << auxName << " value=" << v << std::endl;
    //   }
    //   else
    // 	(*m_out) << "  Index=" <<idx << ", Auxid=" << ai << ", Name=" << auxName << ", unknown type" << ti->name() << std::endl;
    // }

    //    std::array< xAOD::CaloCluster::MomentType,60> allMoments= {xAOD::CaloCluster::FIRST_PHI ,xAOD::CaloCluster::FIRST_ETA ,xAOD::CaloCluster::SECOND_R ,xAOD::CaloCluster::SECOND_LAMBDA ,xAOD::CaloCluster::DELTA_PHI ,xAOD::CaloCluster::DELTA_THETA ,xAOD::CaloCluster::DELTA_ALPHA ,xAOD::CaloCluster::CENTER_X ,xAOD::CaloCluster::CENTER_Y ,xAOD::CaloCluster::CENTER_Z ,xAOD::CaloCluster::CENTER_MAG ,xAOD::CaloCluster::CENTER_LAMBDA ,xAOD::CaloCluster::LATERAL ,xAOD::CaloCluster::LONGITUDINAL ,xAOD::CaloCluster::ENG_FRAC_EM ,xAOD::CaloCluster::ENG_FRAC_MAX ,xAOD::CaloCluster::ENG_FRAC_CORE ,xAOD::CaloCluster::FIRST_ENG_DENS ,xAOD::CaloCluster::SECOND_ENG_DENS ,xAOD::CaloCluster::ISOLATION ,xAOD::CaloCluster::ENG_BAD_CELLS ,xAOD::CaloCluster::N_BAD_CELLS ,xAOD::CaloCluster::N_BAD_CELLS_CORR ,xAOD::CaloCluster::BAD_CELLS_CORR_E ,xAOD::CaloCluster::BADLARQ_FRAC ,xAOD::CaloCluster::ENG_POS ,xAOD::CaloCluster::SIGNIFICANCE ,xAOD::CaloCluster::CELL_SIGNIFICANCE ,xAOD::CaloCluster::CELL_SIG_SAMPLING ,xAOD::CaloCluster::AVG_LAR_Q ,xAOD::CaloCluster::AVG_TILE_Q ,xAOD::CaloCluster::EM_PROBABILITY ,xAOD::CaloCluster::HAD_WEIGHT ,xAOD::CaloCluster::OOC_WEIGHT ,xAOD::CaloCluster::DM_WEIGHT ,xAOD::CaloCluster::TILE_CONFIDENCE_LEVEL ,xAOD::CaloCluster::VERTEX_FRACTION ,xAOD::CaloCluster::NVERTEX_FRACTION ,xAOD::CaloCluster::ENG_CALIB_TOT ,xAOD::CaloCluster::ENG_CALIB_OUT_L ,xAOD::CaloCluster::ENG_CALIB_OUT_M ,xAOD::CaloCluster::ENG_CALIB_OUT_T ,xAOD::CaloCluster::ENG_CALIB_DEAD_L ,xAOD::CaloCluster::ENG_CALIB_DEAD_M ,xAOD::CaloCluster::ENG_CALIB_DEAD_T ,xAOD::CaloCluster::ENG_CALIB_EMB0 ,xAOD::CaloCluster::ENG_CALIB_EME0 ,xAOD::CaloCluster::ENG_CALIB_TILEG3 ,xAOD::CaloCluster::ENG_CALIB_DEAD_TOT ,xAOD::CaloCluster::ENG_CALIB_DEAD_EMB0 ,xAOD::CaloCluster::ENG_CALIB_DEAD_TILE0 ,xAOD::CaloCluster::ENG_CALIB_DEAD_TILEG3 ,xAOD::CaloCluster::ENG_CALIB_DEAD_EME0 ,xAOD::CaloCluster::ENG_CALIB_DEAD_HEC0 ,xAOD::CaloCluster::ENG_CALIB_DEAD_FCAL ,xAOD::CaloCluster::ENG_CALIB_DEAD_LEAKAGE ,xAOD::CaloCluster::ENG_CALIB_DEAD_UNCLASS ,xAOD::CaloCluster::ENG_CALIB_FRAC_EM ,xAOD::CaloCluster::ENG_CALIB_FRAC_HAD ,xAOD::CaloCluster::ENG_CALIB_FRAC_REST};

    std::array<std::string,61> allMoments={{"FIRST_PHI","FIRST_ETA","SECOND_R","SECOND_LAMBDA","DELTA_PHI","DELTA_THETA","DELTA_ALPHA","CENTER_X","CENTER_Y","CENTER_Z","CENTER_MAG","CENTER_LAMBDA","LATERAL","LONGITUDINAL","ENG_FRAC_EM","ENG_FRAC_MAX","ENG_FRAC_CORE","FIRST_ENG_DENS","SECOND_ENG_DENS","ISOLATION","ENG_BAD_CELLS","N_BAD_CELLS","N_BAD_CELLS_CORR","BAD_CELLS_CORR_E","BADLARQ_FRAC","ENG_POS","SIGNIFICANCE","CELL_SIGNIFICANCE","CELL_SIG_SAMPLING","AVG_LAR_Q","AVG_TILE_Q","EM_PROBABILITY","HAD_WEIGHT","OOC_WEIGHT","DM_WEIGHT","TILE_CONFIDENCE_LEVEL","VERTEX_FRACTION","NVERTEX_FRACTION","ENG_CALIB_TOT","ENG_CALIB_OUT_L","ENG_CALIB_OUT_M","ENG_CALIB_OUT_T","ENG_CALIB_DEAD_L","ENG_CALIB_DEAD_M","ENG_CALIB_DEAD_T","ENG_CALIB_EMB0","ENG_CALIB_EME0","ENG_CALIB_TILEG3","ENG_CALIB_DEAD_TOT","ENG_CALIB_DEAD_EMB0","ENG_CALIB_DEAD_TILE0","ENG_CALIB_DEAD_TILEG3","ENG_CALIB_DEAD_EME0","ENG_CALIB_DEAD_HEC0","ENG_CALIB_DEAD_FCAL","ENG_CALIB_DEAD_LEAKAGE","ENG_CALIB_DEAD_UNCLASS","ENG_CALIB_FRAC_EM","ENG_CALIB_FRAC_HAD","ENG_CALIB_FRAC_REST", "ENERGY_Truth"}};
    (*m_out) << "Cluster Moments" << std::endl;
    for (const auto& momName : allMoments) {
      SG::AuxElement::Accessor< float > a(momName);
      if (a.isAvailable(cluster)) {
	float v=a(cluster);
	(*m_out) << "   " <<momName << ": " << v << std::endl;
      }
    }
    

    (*m_out) << "Bad Channel data: " << std::endl;
    for (const auto& bc : cluster.badChannelList()) {
      (*m_out) << "   eta=" << bc.eta() << ", phi=" << bc.phi() << ", layer=" << bc.layer() << ", word=" << bc.badChannel() << std::endl;
    }

    (*m_out) << "Cell-links:" << std::endl;
    const CaloClusterCellLink* cellLinks=cluster.getCellLinks();
    if (cellLinks) {
//      (*m_out) << "  Total: " << cellLinks->size() << std::endl;
      CaloClusterCellLink::const_iterator lnk_it=cellLinks->begin();
      CaloClusterCellLink::const_iterator lnk_it_e=cellLinks->end();
      for (;lnk_it!=lnk_it_e;++lnk_it) {
	const CaloCell* cell=*lnk_it;
	(*m_out) << "   ID=" << std::hex << cell->ID() << std::dec << ", E=" << cell->e() << ", weight=" << lnk_it.weight() << std::endl;
      }
    }
    else
      (*m_out) << "   No Cell Links found" << std::endl;
    
  }//end loop over clusters
  
  return StatusCode::SUCCESS;
}


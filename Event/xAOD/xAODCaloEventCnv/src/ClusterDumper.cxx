/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ClusterDumper.cxx 585681 2014-02-28 14:23:10Z wlampl $

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

  
  for (auto itr: *clustercontainer) {
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
      
    

    (*m_out) << "Auxiliary variables: " << std::endl;
    const SG::auxid_set_t& auxIds=cluster.container()->getAuxIDs(); //->getDynamicAuxIDs();
    const size_t idx= cluster.index();
    for (auto ai: auxIds) {
      const std::string& auxName=SG::AuxTypeRegistry::instance().getName(ai);
      const float v=clustercontainer->getData<float>(ai,idx);
      (*m_out) << "  Index=" <<idx << ", Auxid=" << ai << ", Name=" << auxName << " value=" << v << std::endl;
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


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIClusterSubtraction.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

//**********************************************************************

HIClusterSubtraction::HIClusterSubtraction(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name,pSvcLocator)
{
  declareProperty("ClusterKey", m_cluster_key);
  declareProperty("EventShapeKey",m_event_shape_key);
  declareProperty("Subtractor",m_subtractor_tool);
}

//**********************************************************************


StatusCode HIClusterSubtraction::execute()
{

  //const jet::cellset_t & badcells = badCellMap.cells() ;

  //retrieve UE

  const xAOD::HIEventShapeContainer* shape=0;
  if(evtStore()->retrieve(shape,m_event_shape_key).isFailure())
  {
    ATH_MSG_ERROR("Could not retrieve input HIEventShape " << m_event_shape_key );
    return StatusCode::FAILURE;
  }

  xAOD::CaloClusterContainer* ccl=0;
  if(evtStore()->retrieve(ccl,m_cluster_key).isFailure())
  {
    ATH_MSG_ERROR("Could not retrieve input HIEventShape " << m_cluster_key );
    return StatusCode::FAILURE;
  }

  for(xAOD::HIEventShapeContainer::const_iterator itr=shape->begin(); itr!=shape->end(); itr++)
  {
    const xAOD::HIEventShape* slice=(*itr);
    ATH_MSG(VERBOSE) << std::setw(12) << slice->Et()
		     << std::setw(12) << slice->area()
		     << std::setw(12) << slice->rho()
		     << std::setw(12) << slice->nCells()
		     << std::setw(12) << slice->etaMin()
		     << std::setw(12) << slice->etaMax()
		     << std::setw(12) << slice->layer()
		     << std::setw(12) << slice->nCells()
		     << endreq;
  }


  for(xAOD::CaloClusterContainer::iterator itr=ccl->begin(); itr!=ccl->end(); itr++) 
  {
    xAOD::CaloCluster* cl=*itr;
    xAOD::IParticle::FourMom_t p4;
    m_subtractor_tool->Subtract(p4,cl,shape);
    ATH_MSG(VERBOSE) << std::setw(12) << "Before:"
		  << std::setw(7) << std::setprecision(3) << cl->altE()*1e-3
		  << std::setw(7) << std::setprecision(3) << cl->altEta()
		  << std::setw(7) << std::setprecision(3) << cl->altPhi()
		  << endreq;


    //cl->setSignalState(xAOD::CaloCluster::ALTCALIBRATED);
    cl->setAltE(p4.E());
    cl->setAltEta(p4.Eta());
    cl->setAltPhi(p4.Phi());
    cl->setAltM(0);
    ATH_MSG(VERBOSE)   << std::setw(12) << "After:"
		       << std::setw(7) << std::setprecision(3) << cl->altE()*1e-3
		       << std::setw(7) << std::setprecision(3) << cl->altEta()
		       << std::setw(7) << std::setprecision(3) << cl->altPhi()
		       << endreq;
    
  }
  return StatusCode::SUCCESS;
}


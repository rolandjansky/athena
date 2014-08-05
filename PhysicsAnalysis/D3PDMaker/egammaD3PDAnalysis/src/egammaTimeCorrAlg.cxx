/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: egammaTimeCorrAlg.cxx 605541 2014-07-09 04:46:56Z ssnyder $
/**
* @file egammaD3PDAnalysis/src/egammaTimeCorrAlg.cxx
* @author Nikiforos K. Nikiforou <nikiforo@cern.ch> adapted from various tools by Scott Snyder
* @date Apr, 2012
* @brief Calculate corrected time for an egamma object and store in UserData.
*/


#include "egammaTimeCorrAlg.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "xAODEgamma/Egamma.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


egammaTimeCorrAlg::egammaTimeCorrAlg (const std::string& name,
                                      ISvcLocator* svcloc): 
  AthAlgorithm (name, svcloc), 
  m_getter (this),
  m_caloClusterTimeTool("",this)
{
  declareProperty ("AuxPrefix", m_auxPrefix,
                   "Prefix to add to aux data items.");
  declareProperty ("AllowMissing", m_allowMissing = false,
                   "If true, don't complain if input objects are missing.");
  declareProperty ("Getter", m_getter,
                   "Getter instance for the input egamma objects.");
  declareProperty("VertexContainerName", m_vertexContainerName="VxPrimaryCandidate");
  declareProperty("CaloClusterTimeTool", m_caloClusterTimeTool, "a private tool for caloTime correction");
}


/**
* @brief Standard Gaudi @c initialize method.
*/
StatusCode egammaTimeCorrAlg::initialize()
{
    msg() << MSG::INFO <<" Starting egammaTimeCorrAlg" << endreq;
    
    CHECK( AthAlgorithm::initialize() );
    CHECK( m_caloClusterTimeTool.retrieve() );
    CHECK( m_getter.retrieve() );
    CHECK( m_getter->configureD3PD<xAOD::Egamma>() );
    return StatusCode::SUCCESS;
}


/**
* @brief Standard Gaudi @c execute method.
*/
StatusCode egammaTimeCorrAlg::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

  float vtx_z=0.0;

  const VxContainer* vertexCont(0);
  sc = evtStore()->retrieve(vertexCont,m_vertexContainerName);
  if (sc.isFailure() || !vertexCont || vertexCont->size()<1) {
    msg() << MSG::ERROR <<" Can't retrieve VertexContainer!" << endreq;
    return sc;
  }
    
  const Trk::VxCandidate* Vtx = *(vertexCont->begin());
  vtx_z = Vtx->recVertex().position().z();

#define DECOR(TYPE,N) xAOD::Egamma::Decorator<TYPE> N (m_auxPrefix + #N)
  DECOR(float,        raw_cl_time);
  DECOR(float,        corr_cl_time);
  DECOR(float,        corr_cl_time_err);
  DECOR(unsigned int, corr_cl_time_flags);
#undef DECOR

  CHECK( m_getter->reset (m_allowMissing) );
  while (const xAOD::Egamma* eg = m_getter->next<xAOD::Egamma>())
  {
    raw_cl_time(*eg) = -9898.9;
    corr_cl_time(*eg) = -9898.9;
    corr_cl_time_err(*eg) = 9898.9;
    corr_cl_time_flags(*eg) = ~0U;
        
    const xAOD::CaloCluster *cluster = eg->caloCluster();
    if(cluster) {
      raw_cl_time(*eg) = cluster->time();
        
      if(cluster->getCellLinks() && !m_caloClusterTimeTool.empty()) {
        m_caloClusterTimeTool->makeClusterTimeCorrection
          (vtx_z, cluster,
           corr_cl_time(*eg),
           corr_cl_time_err(*eg),
           corr_cl_time_flags(*eg));
        
      }
    }
    m_getter->releaseElement (eg);
  }

  return StatusCode::SUCCESS;
}


} // namespace D3PD

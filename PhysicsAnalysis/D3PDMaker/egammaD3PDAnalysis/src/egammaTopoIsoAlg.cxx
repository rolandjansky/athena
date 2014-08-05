/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDAnalysis/src/egammaTopoIsoAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Calculate isolation cones of topo clusters for egamma objects.
 *        Save as aux data.
 */


#include "egammaTopoIsoAlg.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "xAODEgamma/Egamma.h"
#include "egammaInterfaces/IegammaTopoIso.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "FourMomUtils/P4Helpers.h"
#include "AthContainers/ConstDataVector.h"
#include "AthenaKernel/errorcheck.h"
#include "boost/foreach.hpp"
#include "boost/format.hpp"
#include <vector>


namespace D3PD {


egammaTopoIsoAlg::egammaTopoIsoAlg (const std::string& name,
                                    ISvcLocator* svcloc)
  : AthAlgorithm (name, svcloc),
    m_egammaGetter (this),
    m_clusterGetter (this)
{ 
  declareProperty ("AuxPrefix", m_auxPrefix,
                   "Prefix to add to aux data items.");
 declareProperty ("EgammaGetter", m_egammaGetter,
                   "Getter instance for the input egamma objects.");
  declareProperty ("ClusterGetter", m_clusterGetter,
                   "Getter instance for the input topo cluster objects.");
  declareProperty ("TopoIsoTool", m_topoIsoTool,
		   "egamma tool for calculating topo isolation.");
  declareProperty ("AllowMissing", m_allowMissing = false,
                   "If true, don't complain if input objects are missing.");
  declareProperty ("ConeSizes", m_coneSizes,
                   "Cone sizes for which to calculate isolation.");
  declareProperty ("UseEMScale", m_useEMScale = true,
                   "Use TopoClusters at the EM scale.");
  declareProperty ("PositiveEnergyClusters", m_positiveClusters = false,
		   "Use only positive-energy clusters.");

  for (int i=2; i<=6; i++)
    m_coneSizes.push_back (i / 10.);
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode egammaTopoIsoAlg::initialize()
{
  CHECK( AthAlgorithm::initialize() );
  CHECK( m_topoIsoTool.retrieve() );
  CHECK( m_egammaGetter.retrieve() );
  CHECK( m_clusterGetter.retrieve() );
  CHECK( m_egammaGetter->configureD3PD<xAOD::Egamma>() );
  CHECK( m_clusterGetter->configureD3PD<xAOD::CaloCluster>() );
  
  m_names.resize (m_coneSizes.size());
  boost::format fmt ("topo%sEtcone%02d");
  const char* em = m_positiveClusters ? (m_useEMScale ? "Pos" : "PosCal"):(m_useEMScale ? "" : "Cal");
  for (size_t i = 0; i < m_coneSizes.size(); i++) {
    m_names[i] = (fmt % em % static_cast<int>(m_coneSizes[i]*100 + 0.5)).str();
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode egammaTopoIsoAlg::execute()
{
  // Photon and topo cluster lists may be long.
  // So make a local copy of the cluster list.
  ConstDataVector<xAOD::CaloClusterContainer> clusts (SG::VIEW_ELEMENTS);
  CHECK( m_clusterGetter->reset (m_allowMissing) );
  clusts.reserve (m_clusterGetter->sizeHint (m_allowMissing) );
  while (const xAOD::CaloCluster* cl = m_clusterGetter->next<xAOD::CaloCluster>()){
    if(m_useEMScale){
      if(!(m_positiveClusters && cl->p4(xAOD::CaloCluster::UNCALIBRATED).Et()<0))
	clusts.push_back (cl);
    }
    else{
      if(!(m_positiveClusters && cl->et()<0))
	clusts.push_back (cl);
    }
  }

  std::vector<float> cones (m_coneSizes.size());

  CHECK( m_egammaGetter->reset (m_allowMissing) );
  while (const xAOD::Egamma* eg = m_egammaGetter->next<xAOD::Egamma>())
  {
    CHECK( topoClustCones (*eg, *clusts.asDataVector(), cones) );
    m_egammaGetter->releaseElement (eg);
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Calculate isolation cones in topo clusters around @a eg.
 */
StatusCode
egammaTopoIsoAlg::topoClustCones (const xAOD::Egamma& eg,
                                  const xAOD::CaloClusterContainer& clusts,
                                  std::vector<float>& cones) const
{
  size_t ncones = m_coneSizes.size();
  
  CHECK( m_topoIsoTool->execute(&eg, &clusts, m_coneSizes) );
  for (size_t i = 0; i < ncones; i++){
    cones[i] = m_topoIsoTool->topoetcone(i);
    SG::AuxElement::Decorator<float> var (m_auxPrefix + m_names[i]);
    var(eg) = cones[i];
  }    

  return StatusCode::SUCCESS;
}


} // namespace D3PD

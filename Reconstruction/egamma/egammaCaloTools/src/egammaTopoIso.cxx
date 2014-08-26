/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaTopoIso.h"
#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/EgammaEnums.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloClusterCorrection/CaloFillRectangularCluster.h"
#include "FourMomUtils/P4Helpers.h"
#include "AthenaKernel/errorcheck.h"

#include "boost/foreach.hpp"
#include "boost/format.hpp"

#include <cmath> 
#include "CaloUtils/CaloClusterStoreHelper.h"

// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
egammaTopoIso::egammaTopoIso(const std::string& type,
		     const std::string& name,
		     const IInterface* parent)
  : AthAlgTool(type, name, parent)
{

  // declare Interface
  declareInterface<IegammaTopoIso>(this);

  declareProperty ("ConeSizes", m_coneSizes,
                   "Cone sizes for which to calculate isolation.");
  declareProperty ("UseEMScale", m_useEMScale = true,
                   "Use TopoClusters at the EM scale.");
  declareProperty("CaloFillRectangularClusterTool", m_caloFillRectangularTool, "Handle of the CaloFillRectangularClusterTool");

  for (int i=2; i<5; i++){
    m_coneSizes.push_back (i / 10.);
    m_coneSizes2.push_back((i / 10.)* (i / 10.)); 
  }               
}

/**
 * @brief Standard Gaudi @c destructor method.
 */
egammaTopoIso::~egammaTopoIso()
{
}

/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode egammaTopoIso::initialize()
{
  ATH_MSG_DEBUG("Initialize  egammaTopoIso");
  CHECK( m_caloFillRectangularTool.retrieve() );

  m_caloFillRectangularToolPtr=dynamic_cast<CaloFillRectangularCluster*>((&(*m_caloFillRectangularTool)));

  if (!m_caloFillRectangularToolPtr) return StatusCode::FAILURE;
  

  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c finalize method.
 */
StatusCode egammaTopoIso::finalize()
{
  return StatusCode::SUCCESS;
}
/**
 * @brief Standard method: Et cone variables for topo cluster passing the parameters to calculate
 */
StatusCode egammaTopoIso::execute(const xAOD::Egamma* eg, 
				  const xAOD::CaloClusterContainer *topo_container, 
				  const std::vector<int>& isotype)
{


 ATH_MSG_DEBUG("In egammaTopoIso execute");

 if ( eg == NULL ){
     ATH_MSG_DEBUG("No Egamma Object");
     return StatusCode::SUCCESS;
 }

 if ( isotype.size() == 0 ) {
    ATH_MSG_DEBUG("No isolation required");
    return StatusCode::SUCCESS;
 }

 m_coneSizes.clear();
 m_coneSizes2.clear();
 m_etcone.clear();
 for (std::vector<int>::const_iterator it = isotype.begin(); it != isotype.end(); ++it) {
   switch (*it) 
     {
     case xAOD::EgammaParameters::topoetcone20:
       m_coneSizes.push_back(0.2);        
       m_coneSizes2.push_back(0.2*0.2);                
       m_etcone.push_back(0.);
       break;
     case xAOD::EgammaParameters::topoetcone30:
       m_coneSizes.push_back(0.3);        
       m_coneSizes2.push_back(0.3*0.3);                
       m_etcone.push_back(0.);
       break;
     case xAOD::EgammaParameters::topoetcone40:
       m_coneSizes.push_back(0.4);
       m_coneSizes2.push_back(0.4*0.4);                        
       m_etcone.push_back(0.);
       break;
     default:
       ATH_MSG_DEBUG("Topo isolation cannot be calculated for "<< (*it));
     }
 }
  
 if (m_coneSizes.size() != isotype.size()) 
    ATH_MSG_WARNING("Not all requested cone sizes are considered;  requested: " << isotype.size()
                    << " sizes calculated: "<< m_coneSizes.size() );

  // Photon and topo cluster lists may be long.
  // So make a local copy of the cluster list.
 std::vector<const xAOD::CaloCluster*> clusts;
 auto clItr  = topo_container->begin();
 auto clItrE = topo_container->end();

 //We need just the clusters inside the dR of the largest cone used for isolation
 auto max_cone_iter=std::max_element(m_coneSizes2.begin(), m_coneSizes2.end());
 float max_cone= (*max_cone_iter);
 for(; clItr != clItrE; ++clItr){

   float dPhi = P4Helpers::deltaPhi( (*clItr)->phi(),  eg->caloCluster()->phi() );
   float dEta = (*clItr)->eta()-eg->caloCluster()->eta();
   float dr2=dPhi*dPhi+ dEta*dEta;
    
   if (dr2 < max_cone) {
     clusts.push_back (*clItr);
   }
 }

  // Calculate isolation for topo cluster 
  CHECK( topoClustCones (*eg, clusts) );

  return StatusCode::SUCCESS;
}

/**
 * @brief Standard method: Et cone variables for topo cluster passing radii
 */
StatusCode egammaTopoIso::execute(const xAOD::Egamma* eg, 
				  const xAOD::CaloClusterContainer *topo_container, 
				  const std::vector<float>& Rs)
{

  // Photon and topo cluster lists may be long.
  // So make a local copy of the cluster list.
  std::vector<const xAOD::CaloCluster*> clusts;
  auto clItr  = topo_container->begin();
  auto clItrE = topo_container->end();

   //We need just the clusters inside the dR of the largest cone used for isolation
  auto max_cone_iter=std::max_element(Rs.begin(), Rs.end());
  float max_cone= (*max_cone_iter)*(*max_cone_iter) ;
  for(; clItr != clItrE; ++clItr){
    
    float dPhi = P4Helpers::deltaPhi( (*clItr)->phi(),  eg->caloCluster()->phi() );
    float dEta = (*clItr)->eta()-eg->caloCluster()->eta();
    float dr2=dPhi*dPhi+ dEta*dEta;
    
    if (dr2 < max_cone) {
      clusts.push_back (*clItr);
    }
  }
 
 // Calculate isolation for topo cluster 
  CHECK( execute (eg, clusts, Rs) );
  
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard method: Et cone variables for topo cluster passing radii
 */
StatusCode egammaTopoIso::execute(const xAOD::Egamma* eg, 
				  const std::vector<const xAOD::CaloCluster*>& clusts, 
				  const std::vector<float>& Rs)
{


 ATH_MSG_DEBUG("In egammaTopoIso execute");

 if ( eg == NULL ){
     ATH_MSG_DEBUG("No Egamma Object");
     return StatusCode::SUCCESS;
 }

 if ( Rs.size() == 0 ) {
    ATH_MSG_DEBUG("No isolation required");
    return StatusCode::SUCCESS;
 }

 m_coneSizes = Rs;
 m_etcone.clear();
 m_etcone.resize(Rs.size());

 m_coneSizes2.clear();
 for (unsigned int i = 0; i < m_coneSizes.size(); i++) {
   m_coneSizes2.push_back(m_coneSizes[i]*m_coneSizes[i]) ;
 }
 
 // Calculate isolation for topo cluster 
 CHECK( topoClustCones (*eg, clusts) );
 
 return StatusCode::SUCCESS;
}


/**
 * @brief Calculate isolation cones in topo clusters around @a eg.
 */
StatusCode egammaTopoIso::topoClustCones (const xAOD::Egamma& eg,
                                          const std::vector<const xAOD::CaloCluster*>& clusts) 
{

  ATH_MSG_DEBUG("In egammaTopoIso::topoClustCones");

  if (eg.caloCluster() == 0 || eg.caloCluster()->pt() <= 0 || fabs(eg.caloCluster()->eta()) > 7.0) {
    ATH_MSG_DEBUG("Cluster potentially malformed");
    return StatusCode::SUCCESS;
  }


  BOOST_FOREACH (const xAOD::CaloCluster* cl, clusts) {
    float et = (m_useEMScale ? cl->p4(xAOD::CaloCluster::State::UNCALIBRATED).Et() : cl->pt() );
    if(et <= 0 || fabs(cl->eta()) > 7.0) continue;

    float st = 1./cosh(cl->p4(xAOD::CaloCluster::State::UNCALIBRATED).Eta());
    float tilegap3_et = cl->eSample(CaloSampling::TileGap3)*st;
    et -= tilegap3_et;

    float dPhi = P4Helpers::deltaPhi( cl->phi(),  eg.caloCluster()->phi() );
    float dEta = cl->eta()-eg.caloCluster()->eta();
    float dr2=dPhi*dPhi+ dEta*dEta;
    
    for (unsigned int i = 0; i < m_coneSizes.size(); i++) {
      if (dr2 < m_coneSizes2[i]) {
	m_etcone[i] += et;
      }
    }
  }
  
  // now correct the isolation energy for the core cluster energy
  const xAOD::CaloCluster *egc = eg.caloCluster();
  float eraw57=0., eta=0.;
  if(egc && egc->getCellLinks()){
    xAOD::CaloCluster* egcCloneFor57 = CaloClusterStoreHelper::makeCluster(egc->getCellLinks()->getCellContainer(),
									   egc->eta0(),egc->phi0(),
									   egc->clusterSize()
									   );
    m_caloFillRectangularToolPtr->makeCorrection(egcCloneFor57);
    eraw57   = egcCloneFor57->e();
    eta = egc->eta(); //FillRectangularCluster doesn't recalculated the overall custer eta (only per-sampling)
    delete egcCloneFor57;
  }
  float et_core = eraw57/cosh(eta);
  for (unsigned int i = 0; i < m_coneSizes.size(); i++){
     m_etcone[i] -= et_core;
  }

  return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaCaloClusterSelector.h"

#include "xAODCaloEvent/CaloCluster.h"

egammaCaloClusterSelector::egammaCaloClusterSelector(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

egammaCaloClusterSelector::~egammaCaloClusterSelector()
{
}

StatusCode egammaCaloClusterSelector::initialize()
{
  if (!m_egammaCheckEnergyDepositTool.empty()) {
    ATH_CHECK( m_egammaCheckEnergyDepositTool.retrieve() );
  } else {
    m_egammaCheckEnergyDepositTool.disable();
  }

  m_runMiddleShapes = m_RetaCuts.size() != 0;

  ATH_CHECK(m_cellsKey.initialize(m_runMiddleShapes));

  if (m_runMiddleShapes) {
    ATH_CHECK(m_egammaMiddleShape.retrieve());
  } else {
    m_egammaMiddleShape.disable();
  }
  // validate that the configuration makes sense
  const auto numBins = m_EMEtRanges.size();

  if (m_EMFCuts.size() != 0 && m_EMFCuts.size() != numBins) {
    ATH_MSG_FATAL("The size of EMFCuts, now " <<  m_EMFCuts.size() 
		  << ", must be zero or the number of Et bins: " <<  numBins);
    return StatusCode::FAILURE;
  }

  if (m_lateralCuts.size() != 0 && m_lateralCuts.size() != numBins) {
    ATH_MSG_FATAL("The size of LateralCuts, now " <<  m_lateralCuts.size() 
		  << ", must be zero or the number of Et bins: " <<  numBins);
    return StatusCode::FAILURE;
  }

  if (m_RetaCuts.size() != 0 && m_RetaCuts.size() != numBins) {
    ATH_MSG_FATAL("The size of LateralCuts, now " <<  m_RetaCuts.size() 
		  << ", must be zero or the number of Et bins: " <<  numBins);
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode egammaCaloClusterSelector::finalize()
{
  return StatusCode::SUCCESS;
}

// ======================================================================
bool egammaCaloClusterSelector::passSelection(const xAOD::CaloCluster* cluster) const
{
  // switch to using cluster properties, not layer 2 properties
  if ( cluster->et() < m_ClusterEtCut ){
    ATH_MSG_DEBUG("Cluster failed Energy Cut: dont make ROI");
    return false;
  }
   
  /* 
   * If lower than the minimum one requested via the ranges
   * return false.
   * Only then run the  egammaCheckEnergyDepositTool or any other step.
   */ 

  static const  SG::AuxElement::ConstAccessor<float> acc("EMFraction");
  double emFrac(0.);
  if (acc.isAvailable(*cluster)) {
    emFrac = acc(*cluster);
  } else if (!cluster->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,emFrac)){
    throw std::runtime_error("No EM fraction momement stored");
  }
  const double EMEt = cluster->et()*emFrac;
  const double bin = findETBin(EMEt);
  
  if (!m_EMEtRanges.empty() && bin<0){
    ATH_MSG_DEBUG("Cluster EM Energy is lower than the lowest cut in EMEtRanges  dont make ROI");
    return false;
  }  

  if( !m_egammaCheckEnergyDepositTool.empty() && 
      !m_egammaCheckEnergyDepositTool->checkFractioninSamplingCluster( cluster ) ) {
    ATH_MSG_DEBUG("Cluster failed sample check: dont make ROI");
    return false;
  }
  
  /*
   * All other cuts are binned. 
   * Pass if no binning is defined.
   */  
  if (m_EMEtRanges.empty()) {
    // no ET bins defined, so pass
    return true;
  }

  if ( m_EMFCuts.size() != 0 && emFrac < m_EMFCuts[bin] ){
    ATH_MSG_DEBUG("Cluster failed EM Fraction cut: don't make ROI");
    return false;
  }
  
  if (m_lateralCuts.size() != 0) {
    double lateral(0.);
    if (!cluster->retrieveMoment(xAOD::CaloCluster::LATERAL, lateral)){
      throw std::runtime_error("No LATERAL momement stored");
    }
    
    if ( lateral >  m_lateralCuts[bin] ){
      ATH_MSG_DEBUG("Cluster failed LATERAL cut: dont make ROI");
      return false;
    }
  }

  if (m_runMiddleShapes) {
    
    // retrieve the cell containers
    SG::ReadHandle<CaloCellContainer> cellcoll(m_cellsKey);

    IegammaMiddleShape::Info info;
    StatusCode sc = m_egammaMiddleShape->execute(*cluster, *cellcoll, info);
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING("call to Middle shape returns failure for execute");
      return false;
    }
    if (info.e277 != 0. && info.e237/info.e277 <= m_RetaCuts[bin]){
      ATH_MSG_DEBUG("Cluster failed Reta test: dont make ROI");
      return false;
    }
  }
  return true;
}

// return the bin number given the EM Et.
int egammaCaloClusterSelector::findETBin(double EMEt) const
{
  const int numBins = m_EMEtRanges.size();
  int newBin = 0;
  while (newBin < numBins && EMEt > m_EMEtRanges[newBin]) {
    newBin++;
  }
  return newBin - 1;
}
     

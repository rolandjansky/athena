/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "egammaCaloClusterSelector.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloUtils/CaloCellList.h"

egammaCaloClusterSelector::egammaCaloClusterSelector(const std::string& type, 
                                                     const std::string& name, 
                                                     const IInterface* parent) :
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
  //Shower Shape tools
  m_doReta=m_RetaCuts.size()!=0;
  m_doHadLeak=m_HadLeakCuts.size()!=0; 
  ATH_CHECK(m_cellsKey.initialize(m_doReta||m_doHadLeak));
  if (m_doReta) {
    ATH_CHECK(m_egammaMiddleShape.retrieve());
  } else {
    m_egammaMiddleShape.disable();
  }
  
  if (m_doHadLeak) {
    ATH_CHECK(m_HadronicLeakageTool.retrieve());
  } else {
    m_HadronicLeakageTool.disable();
  }

  // validate that the configuration makes sense
  const size_t numBins = m_EMEtRanges.size();

  if (m_EMFCuts.size() != 0 && m_EMFCuts.size() != numBins) {
    ATH_MSG_FATAL("The size of EMFCuts, now " <<  m_EMFCuts.size() 
		  << ", must be zero or the number of Et bins: " <<  numBins);
    return StatusCode::FAILURE;
  }

  if (m_RetaCuts.size() != 0 && m_RetaCuts.size() != numBins) {
    ATH_MSG_FATAL("The size of RetaCuts, now " <<  m_RetaCuts.size() 
		  << ", must be zero or the number of Et bins: " <<  numBins);
    return StatusCode::FAILURE;
  }
  if (m_HadLeakCuts.size() != 0 && m_HadLeakCuts.size() != numBins) {
    ATH_MSG_FATAL("The size of HadLeakCuts, now " <<  m_RetaCuts.size() 
		  << ", must be zero or the number of Et bins: " <<  numBins);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode egammaCaloClusterSelector::finalize()
{
  return StatusCode::SUCCESS;
}

bool egammaCaloClusterSelector::passSelection(const xAOD::CaloCluster* cluster) const
{
  /* Minimum Cluster energy*/
  if ( cluster->et() < m_ClusterEtCut ){
    ATH_MSG_DEBUG("Cluster failed Energy Cut: dont make ROI");
    return false;
  }

  /*Check energy deposit if requested*/
  if( !m_egammaCheckEnergyDepositTool.empty() && 
      !m_egammaCheckEnergyDepositTool->checkFractioninSamplingCluster( cluster ) ) {
    ATH_MSG_DEBUG("Cluster failed sample check: dont make ROI");
    return false;
  }
  /*
   * All e/gamma cuts assume/need binning in Et. 
   * Pass if no such binning is defined.
   */  
  if (m_EMEtRanges.empty()) {
    return true;
  }
  /*
   * We need to have second sampling present.
   * And calculate the EM energy and EM Et. 
   */
  if (!cluster->hasSampling(CaloSampling::EMB2) && !cluster->hasSampling(CaloSampling::EME2)){
      return false;
  }
  const double eta2 = fabs(cluster->etaBE(2));
  if(eta2>10){
    return false;
  }
  //use the egamma definition of EMFrac (includes presampler , helps with eff in the crack)
  static const  SG::AuxElement::ConstAccessor<float> acc("EMFraction");
  const double emFrac = acc.isAvailable(*cluster)? acc(*cluster) : 0.;
  const double EMEnergy= cluster->e()* emFrac;
  const double EMEt = EMEnergy/cosh(eta2);
  const double bin = findETBin(EMEt);
  /* Check for the minimum EM Et required this should be the 0th entry in EMEtRanges*/
  if (bin<0){
    ATH_MSG_DEBUG("Cluster EM Et is lower than the lowest cut in EMEtRanges dont make ROI");
    return false;
  }
 /* EM fraction cut*/
  if ( m_EMFCuts.size() != 0 && emFrac < m_EMFCuts[bin] ){
    ATH_MSG_DEBUG("Cluster failed EM Fraction cut: don't make ROI");
    return false;
  }
  /* Reta and Rhad cuts*/
  if (m_doReta||m_doHadLeak) {
    // retrieve the cell containers
    SG::ReadHandle<CaloCellContainer> cellcoll(m_cellsKey);
    if(m_doReta){
      IegammaMiddleShape::Info info;
      StatusCode sc = m_egammaMiddleShape->execute(*cluster, *cellcoll, info);
      if ( sc.isFailure() ) {
        ATH_MSG_WARNING("call to Middle shape returns failure for execute");
        return false;
      }
      if(info.e277<0 || info.e237<0){
        ATH_MSG_DEBUG("Negative e277 or e237: dont make ROI");
        return false;
      }
      if (info.e277 != 0. && info.e237/info.e277 < m_RetaCuts[bin]){
        ATH_MSG_DEBUG("Cluster failed Reta test: dont make ROI");
        return false;
      }
    }
    if(m_doHadLeak){
      // define a new Calo Cell list corresponding to HAD Calo
      static const std::vector<CaloCell_ID::SUBCALO> theVecCalo={CaloCell_ID::LARHEC,CaloCell_ID::TILE};
      CaloCellList HADccl(cellcoll.ptr(),theVecCalo); 
      // calculate information concerning just the hadronic leakage
      IegammaIso::Info info;
      StatusCode sc =  m_HadronicLeakageTool->execute(*cluster,HADccl,info);
      if ( sc.isFailure() ) {
        ATH_MSG_WARNING("call to Iso returns failure for execute");
        return false;
      }
      const double ethad1 = info.ethad1;
      const double ethad  = info.ethad;
      const double raphad1 =EMEt != 0. ? ethad1/EMEt : 0.;
      const double raphad = EMEt != 0. ? ethad/EMEt : 0.;
      if (eta2 >= 0.8 && eta2 < 1.37){
        if (raphad>m_HadLeakCuts[bin]){
          ATH_MSG_DEBUG("Cluster failed Hadronic Leakage test: dont make ROI");
          return false;
        }
      }
      else if(raphad1 >m_HadLeakCuts[bin]){
        ATH_MSG_DEBUG("Cluster failed Hadronic Leakage test: dont make ROI");
        return false;
      }
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

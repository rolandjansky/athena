/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ClusterEnergyPerLayerDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Bruno Lenzi

#include "DerivationFrameworkCalo/ClusterEnergyPerLayerDecorator.h"
#include "xAODEgamma/EgammaContainer.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloClusterCorrection/CaloFillRectangularCluster.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "CaloUtils/CaloCellDetPos.h"


// Constructor
DerivationFramework::ClusterEnergyPerLayerDecorator::ClusterEnergyPerLayerDecorator(const std::string& t,
										    const std::string& n,
										    const IInterface* p) :
  AthAlgTool(t,n,p),
  m_tool(0)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("SGKey_photons", m_SGKey_photons, "The photon collection that one wants to decorate (empty string if you don't want to decorate photons)");
  declareProperty("SGKey_electrons", m_SGKey_electrons, "The electron collection that one wants to decorate (empty string if you don't want to decorate photons)");
  declareProperty("neta", m_eta_size);
  declareProperty("nphi", m_phi_size);
  declareProperty("CaloFillRectangularTool", m_caloFillRectangularTool, "Handle of the CaloFillRectangularClusterTool");
  declareProperty("SGKey_caloCells", m_CellCollectionName="SGKey_caloCells","Name of the CaloCellContainer");
  declareProperty("layers", m_layers = {0,1,2,3});
  declareProperty("SaveExtraDebugInformation", m_saveExtraDebugInformation=false, "decorate e/gammas with vectors of eta, phi, energy for each cell of the three accordion layers + presampler");
}



// Destructor
DerivationFramework::ClusterEnergyPerLayerDecorator::~ClusterEnergyPerLayerDecorator() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::ClusterEnergyPerLayerDecorator::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  if(m_SGKey_photons == "" and m_SGKey_electrons == ""){
    ATH_MSG_FATAL("No e-gamma collection provided");
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_caloFillRectangularTool.retrieve());
  m_tool = dynamic_cast<CaloFillRectangularCluster* >( &(*m_caloFillRectangularTool ));
  if (m_tool == nullptr)
  {
    ATH_MSG_FATAL("Could not retrieve / cast CaloFillRectangularCluster tool");
    return StatusCode::FAILURE;
  } 

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::ClusterEnergyPerLayerDecorator::finalize()
{

  return StatusCode::SUCCESS;
}

// The decoration itself
StatusCode DerivationFramework::ClusterEnergyPerLayerDecorator::addBranches() const
{
  for (const std::string& SGkey : {m_SGKey_photons, m_SGKey_electrons})
  {
    if (SGkey == "") continue;
    const xAOD::EgammaContainer *container(0);
    ATH_CHECK( evtStore()->retrieve(container, SGkey) );
    for (auto egamma : *container) {
      StatusCode sc = decorateObject(egamma);
      if (sc == StatusCode::FAILURE) return sc;
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::ClusterEnergyPerLayerDecorator::decorateObject(const xAOD::Egamma*& egamma) const{
  
  if (not egamma or not egamma->caloCluster()) return StatusCode::SUCCESS;
  
  const CaloCellContainer* cellCont(0);
  xAOD::CaloCluster* egcClone(0);

  bool isBarrel = xAOD::EgammaHelpers::isBarrel(egamma);
  CaloCell_ID::CaloSample sample = isBarrel ? CaloCell_ID::EMB2 : CaloCell_ID::EME2;

  double etaCalo = 0.;
  if(! (egamma->caloCluster()->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME, etaCalo))) {
    ATH_MSG_ERROR("Failed to retrieve eta from calo");
    return StatusCode::FAILURE;
  }
  double phiCalo = 0.;
  if(! (egamma->caloCluster()->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME, phiCalo))) {
    ATH_MSG_ERROR("Failed to retrieve phi from calo");
    return StatusCode::FAILURE;
  }

  std::unique_ptr<CaloCellDetPos> ccdp = std::make_unique<CaloCellDetPos>();
  double etamaxClus, phimaxClus, etamaxClusCalo, phimaxClusCalo;
  if (isBarrel) {
    etamaxClus = egamma->caloCluster()->etamax(CaloCell_ID::EMB2);
    phimaxClus = egamma->caloCluster()->phimax(CaloCell_ID::EMB2);
  }
  else {
    etamaxClus = egamma->caloCluster()->etamax(CaloCell_ID::EME2);
    phimaxClus = egamma->caloCluster()->phimax(CaloCell_ID::EME2);
  }
  ccdp->getDetPosition(sample, etamaxClus, phimaxClus, etamaxClusCalo, phimaxClusCalo); 

  if (evtStore()->retrieve(cellCont,m_CellCollectionName).isFailure())
    ATH_MSG_WARNING(m_CellCollectionName<< " not found");
  else if (not egamma->author(xAOD::EgammaParameters::AuthorCaloTopo35) )
  {  
    egcClone = CaloClusterStoreHelper::makeCluster( cellCont,
                                                    etamaxClusCalo,
                                                    phimaxClusCalo,
                                                    egamma->caloCluster()->clusterSize());
    m_tool->makeCorrection(egcClone);
  }
  
  if (m_saveExtraDebugInformation) {
    auto first_cell = egcClone->cell_begin();
    auto last_cell = egcClone->cell_end();
    int ncelll1 = 0 ;
    int ncelll2 = 0 ;
    int ncelll3 = 0;
    int npres = 0;
    int ncell = 0;   
    std::vector<float>EClusterLr1Eta, EClusterLr1Phi, EClusterLr1E;
    std::vector<float>EClusterLr2Eta, EClusterLr2Phi, EClusterLr2E;
    std::vector<float>EClusterLr3Eta, EClusterLr3Phi, EClusterLr3E;
    std::vector<float>EClusterPreSEta, EClusterPreSPhi, EClusterPreSE;
    
    for (;first_cell != last_cell; ++first_cell,++ncell) {
      const CaloCell* tcell = *first_cell;
      int sampling = tcell->caloDDE()->getSampling();
      
      if( sampling== CaloCell_ID::EMB1 || sampling== CaloCell_ID::EME1 ){
	EClusterLr1Eta.push_back(tcell->eta());
	EClusterLr1Phi.push_back(tcell->phi());
	EClusterLr1E.push_back(tcell->e());      
	ncelll1++;
      }
      
      if(sampling== CaloCell_ID::EMB2 || sampling== CaloCell_ID::EME2 ){
	EClusterLr2Eta.push_back(tcell->eta());
	EClusterLr2Phi.push_back(tcell->phi());
	EClusterLr2E.push_back(tcell->e());
	ncelll2++;
      }
      
      if(sampling== CaloCell_ID::EMB3 || sampling== CaloCell_ID::EME3 ){
	EClusterLr3Eta.push_back(tcell->eta());
	EClusterLr3Phi.push_back(tcell->phi());
	EClusterLr3E.push_back(tcell->e());
	ncelll3++;
      }
      
      if( sampling== CaloCell_ID::PreSamplerB || sampling== CaloCell_ID::PreSamplerE ){
	EClusterPreSEta.push_back(tcell->eta());
	EClusterPreSPhi.push_back(tcell->phi());
	EClusterPreSE.push_back(tcell->e());
	npres++;
      }
    }

    egamma->auxdecor<std::vector<float>>(Form("%dx%dClusterLr1Eta",m_eta_size,m_phi_size)) = EClusterLr1Eta;
    egamma->auxdecor<std::vector<float>>(Form("%dx%dClusterLr1Phi",m_eta_size,m_phi_size)) = EClusterLr1Phi;
    egamma->auxdecor<std::vector<float>>(Form("%dx%dClusterLr1E",m_eta_size,m_phi_size)) = EClusterLr1E;

    egamma->auxdecor<std::vector<float>>(Form("%dx%dClusterLr2Eta",m_eta_size,m_phi_size)) = EClusterLr2Eta;
    egamma->auxdecor<std::vector<float>>(Form("%dx%dClusterLr2Phi",m_eta_size,m_phi_size)) = EClusterLr2Phi;
    egamma->auxdecor<std::vector<float>>(Form("%dx%dClusterLr2E",m_eta_size,m_phi_size)) = EClusterLr2E;
    
    egamma->auxdecor<std::vector<float>>(Form("%dx%dClusterLr3Eta",m_eta_size,m_phi_size)) = EClusterLr3Eta;
    egamma->auxdecor<std::vector<float>>(Form("%dx%dClusterLr3Phi",m_eta_size,m_phi_size)) = EClusterLr3Phi;
    egamma->auxdecor<std::vector<float>>(Form("%dx%dClusterLr3E",m_eta_size,m_phi_size)) = EClusterLr3E;
    
    egamma->auxdecor<std::vector<float>>(Form("%dx%dClusterPreSEta",m_eta_size,m_phi_size)) = EClusterPreSEta;
    egamma->auxdecor<std::vector<float>>(Form("%dx%dClusterPreSPhi",m_eta_size,m_phi_size)) = EClusterPreSPhi;
    egamma->auxdecor<std::vector<float>>(Form("%dx%dClusterPreSE",m_eta_size,m_phi_size)) = EClusterPreSE;
  
    egamma->auxdecor<int>(Form("%dx%dClusterLr1Size",m_eta_size, m_phi_size)) = ncelll1;
    egamma->auxdecor<int>(Form("%dx%dClusterLr2Size",m_eta_size, m_phi_size)) = ncelll2;
    egamma->auxdecor<int>(Form("%dx%dClusterLr3Size",m_eta_size, m_phi_size)) = ncelll3;
    egamma->auxdecor<int>(Form("%dx%dClusterPreSSize",m_eta_size, m_phi_size)) = npres;
  }

  for (unsigned int layer: m_layers)
    egamma->auxdecor<float>(Form("E%dx%d_Lr%d", m_eta_size, m_phi_size, layer)) =\
      egcClone ? egcClone->energyBE(layer) : 0.;

  delete egcClone;

  return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// CellDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Simone Mazza (simone.mazza@mi.infn.it)

#include "DerivationFrameworkCalo/CellDecorator.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/EgammaContainer.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloClusterCorrection/CaloFillRectangularCluster.h"

#include <vector>
#include <string>

// Constructor
DerivationFramework::CellDecorator::CellDecorator(const std::string& t,
						  const std::string& n,
						  const IInterface* p) :
  AthAlgTool(t,n,p),
  m_SGKey_photons(""),
  m_SGKey_electrons(""),
  m_eta_sizes({5,3,3,7}),
  m_phi_sizes({5,5,7,11}),
  m_name_sizes({"Cells5x5","Cells3x5","Cells3x7","Cells7x11"}),
  m_THcaloFillRectangularTool55(""),
  m_THcaloFillRectangularTool35(""),
  m_THcaloFillRectangularTool37(""),
  m_THcaloFillRectangularTool711(""),
  m_caloFillRectangularTool({0,0,0,0})
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("SGKey_photons", m_SGKey_photons);
  declareProperty("SGKey_electrons", m_SGKey_electrons);
  declareProperty("CaloFillRectangularTool_5x5", m_THcaloFillRectangularTool55 , "Handle of the CaloFillRectangularClusterTools");
  declareProperty("CaloFillRectangularTool_3x5", m_THcaloFillRectangularTool35 , "Handle of the CaloFillRectangularClusterTools");
  declareProperty("CaloFillRectangularTool_3x7", m_THcaloFillRectangularTool37 , "Handle of the CaloFillRectangularClusterTools");
  declareProperty("CaloFillRectangularTool_7x11", m_THcaloFillRectangularTool711 , "Handle of the CaloFillRectangularClusterTools");
  declareProperty("CaloCellName", m_CellCollectionName="AODCellContainer","Name of the CaloCellContainer the links should point to");
}



// Destructor
DerivationFramework::CellDecorator::~CellDecorator() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::CellDecorator::initialize()
{
  // Decide which collections need to be checked for ID TrackParticles
  ATH_MSG_VERBOSE("initialize() ...");

  if(m_SGKey_photons == "" && m_SGKey_electrons == "" ){
    ATH_MSG_FATAL("No e-gamma collection provided for thinning. At least one egamma collection (photon/electrons) must be provided!");
    return StatusCode::FAILURE;
  }

  if (m_SGKey_electrons!="") {
    ATH_MSG_INFO("Using "<< m_SGKey_electrons <<" for electrons");
  }

  if (m_SGKey_photons!="") {
    ATH_MSG_INFO("Using "<< m_SGKey_photons <<" for photons");
  }

  if (!m_THcaloFillRectangularTool55.empty()) {
    CHECK(m_THcaloFillRectangularTool55.retrieve());
    m_caloFillRectangularTool[0] = dynamic_cast<CaloFillRectangularCluster*> (&(*m_THcaloFillRectangularTool55));
    if (!m_caloFillRectangularTool[0]) ATH_MSG_ERROR("Could not retrieve CaloFillRectangularCluster tool 5x5");
  }

  if (!m_THcaloFillRectangularTool35.empty()) {
    CHECK(m_THcaloFillRectangularTool35.retrieve());
    m_caloFillRectangularTool[1] = dynamic_cast<CaloFillRectangularCluster*> (&(*m_THcaloFillRectangularTool35));
    if (!m_caloFillRectangularTool[1]) ATH_MSG_ERROR("Could not retrieve CaloFillRectangularCluster tool 3x5");
  }

  if (!m_THcaloFillRectangularTool37.empty()) {
    CHECK(m_THcaloFillRectangularTool37.retrieve());
    m_caloFillRectangularTool[2] = dynamic_cast<CaloFillRectangularCluster*> (&(*m_THcaloFillRectangularTool37));
    if (!m_caloFillRectangularTool[2]) ATH_MSG_ERROR("Could not retrieve CaloFillRectangularCluster tool 3x7");
  }

  if (!m_THcaloFillRectangularTool711.empty()) {
    CHECK(m_THcaloFillRectangularTool711.retrieve());
    m_caloFillRectangularTool[3] = dynamic_cast<CaloFillRectangularCluster*> (&(*m_THcaloFillRectangularTool711));
    if (!m_caloFillRectangularTool[3]) ATH_MSG_ERROR("Could not retrieve CaloFillRectangularCluster tool 7x11");
  }

  ATH_CHECK(detStore()->retrieve(m_caloIdMgr));
  m_calo_id = m_caloIdMgr->getCaloCell_ID();

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::CellDecorator::finalize()
{

  return StatusCode::SUCCESS;
}

// The decoration itself
StatusCode DerivationFramework::CellDecorator::addBranches() const
{
  // Retrieve photon container
  const xAOD::EgammaContainer* importedPhotons(0);
  if(m_SGKey_photons != ""){
    if (evtStore()->retrieve(importedPhotons,m_SGKey_photons).isFailure()) {
      ATH_MSG_ERROR("No e-gamma collection with name " << m_SGKey_photons << " found in StoreGate!");
      return StatusCode::FAILURE;
    }
    for (auto photon : *importedPhotons) {
      if(photon->author() != 128) {
	decorateObject(photon);
      }
    }
  }
  // Retrieve electron container
  const xAOD::EgammaContainer* importedElectrons(0);
  if(m_SGKey_electrons != ""){
    if (evtStore()->retrieve(importedElectrons,m_SGKey_electrons).isFailure()) {
      ATH_MSG_ERROR("No e-gamma collection with name " << m_SGKey_electrons << " found in StoreGate!");
      return StatusCode::FAILURE;
    }
    for (auto electron : *importedElectrons) {
      decorateObject(electron);
    }
  }
  
  return StatusCode::SUCCESS;
}

void DerivationFramework::CellDecorator::decorateObject(const xAOD::Egamma*& egamma) const{
  for(unsigned int i = 0; i < m_name_sizes.size(); i++){
    float total_energy = 0;

    std::vector<float> total_energies(16,0);
    std::vector<std::string> gain_layer_names { "_Lr0", "_Lr1", "_Lr2", "_Lr3",
	"_Lr0_LwG", "_Lr1_LwG", "_Lr2_LwG", "_Lr3_LwG",
	"_Lr0_LwG", "_Lr1_MdG", "_Lr2_MdG", "_Lr3_MdG",
	"_Lr0_LwG", "_Lr1_HiG", "_Lr2_HiG", "_Lr3_HiG"};

    const CaloCellContainer* cellCont(0);
    if (evtStore()->retrieve(cellCont,m_CellCollectionName).isFailure()) {
      ATH_MSG_WARNING(m_CellCollectionName<< " not found");      
      continue;
    }
    
    xAOD::CaloCluster* egcClone = CaloClusterStoreHelper::makeCluster( cellCont,
                                                                       egamma->caloCluster()->eta0(),
                                                                       egamma->caloCluster()->phi0(),
                                                                       egamma->caloCluster()->clusterSize());
    m_caloFillRectangularTool[i]->makeCorrection(egcClone);

    const CaloClusterCellLink* cellLinks = egcClone->getCellLinks();
    if (cellLinks) for (auto cell : *cellLinks ) {
	float energy = cell->energy();
	int layer = m_calo_id->calo_sample(cell->ID());
	total_energy += energy;

	if (layer==CaloSampling::PreSamplerB || layer==CaloSampling::PreSamplerE) total_energies[0]+=energy;
	if (layer==CaloSampling::EMB1 || layer==CaloSampling::EME1) total_energies[1]+=energy;
	if (layer==CaloSampling::EMB2 || layer==CaloSampling::EME2) total_energies[2]+=energy;
	if (layer==CaloSampling::EMB3 || layer==CaloSampling::EME3) total_energies[3]+=energy;
	if (cell->gain()==0) {
	  if (layer==CaloSampling::PreSamplerB || layer==CaloSampling::PreSamplerE) total_energies[4]+=energy;
	  if (layer==CaloSampling::EMB1 || layer==CaloSampling::EME1) total_energies[5]+=energy;
	  if (layer==CaloSampling::EMB2 || layer==CaloSampling::EME2) total_energies[6]+=energy;
	  if (layer==CaloSampling::EMB3 || layer==CaloSampling::EME3) total_energies[7]+=energy;
	}
	if (cell->gain()==1) {
	  if (layer==CaloSampling::PreSamplerB || layer==CaloSampling::PreSamplerE) total_energies[8]+=energy;
	  if (layer==CaloSampling::EMB1 || layer==CaloSampling::EME1) total_energies[9]+=energy;
	  if (layer==CaloSampling::EMB2 || layer==CaloSampling::EME2) total_energies[10]+=energy;
	  if (layer==CaloSampling::EMB3 || layer==CaloSampling::EME3) total_energies[11]+=energy;
	}
	if (cell->gain()==2) {
	  if (layer==CaloSampling::PreSamplerB || layer==CaloSampling::PreSamplerE) total_energies[12]+=energy;
	  if (layer==CaloSampling::EMB1 || layer==CaloSampling::EME1) total_energies[13]+=energy;
	  if (layer==CaloSampling::EMB2 || layer==CaloSampling::EME2) total_energies[14]+=energy;
	  if (layer==CaloSampling::EMB3 || layer==CaloSampling::EME3) total_energies[15]+=energy;
	}
      }

    //ATH_MSG_INFO(m_name_sizes[i]<<" ("<<m_eta_sizes[i]<<"x"<<m_phi_sizes[i]<<"): "<<total_energy);
    SG::AuxElement::Decorator< float > decoration(m_name_sizes[i]);
    decoration(*egamma) = total_energy;

    for(unsigned int j = 0; j < gain_layer_names.size(); j++){
      //ATH_MSG_INFO(m_name_sizes[i]<<gain_layer_names[j]<<" ("<<m_eta_sizes[i]<<"x"<<m_phi_sizes[i]<<"): "<<total_energies[j]);
      std::string sum_name = m_name_sizes[i]+gain_layer_names[j];
      SG::AuxElement::Decorator< float > decoration(sum_name);
      decoration(*egamma) = total_energies[j];
    }
    delete egcClone;
  }
}

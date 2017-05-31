/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkEGamma/TruthCaloShowerDecorator.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/EgammaTruthxAODHelpers.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "CaloCalibHitRec/CalibHitToCaloCellTool.h"

namespace DerivationFramework {

  TruthCaloShowerDecorator::TruthCaloShowerDecorator(const std::string& t,const std::string& n, const IInterface* p) : 
    AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("TruthParticleContainerName", m_truthParticleContainerName ="egammaTruthParticles", "Name of the truth particle container");
    declareProperty("SingleParticleBarcode", m_singleParticleBarcode=10001, "Barcode of single particle");
    declareProperty("CalibHitToCaloCellTool",m_calibhitToCaloCellTool);
    //declareProperty("TruthClusterContainerNames", m_truthClusterContainerNames, "Name of input containers for truth clusters");
  }  

  StatusCode TruthCaloShowerDecorator::initialize()
  {
    ATH_MSG_DEBUG("Initializing " << name() << "...");        

    if(m_calibhitToCaloCellTool.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_calibhitToCaloCellTool);
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Retrieved tool " << m_calibhitToCaloCellTool);
   
    return StatusCode::SUCCESS;
  }

  StatusCode TruthCaloShowerDecorator::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode TruthCaloShowerDecorator::addBranches() const
  {    
    const xAOD::TruthParticleContainer* truthPartContainer(0);
    CHECK ( evtStore()->retrieve(truthPartContainer, m_truthParticleContainerName) );

    // create truth clusters
    ATH_MSG_DEBUG("Creating truth clusters");
    int singleElectronBarcode = 10001;
    if (!m_calibhitToCaloCellTool->processCalibHitsFromParticle(singleElectronBarcode).isSuccess()) {
      ATH_MSG_FATAL("Tool " << m_calibhitToCaloCellTool << " failed.");
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Retrieving truth clusters");
    const xAOD::CaloClusterContainer * truthClusterContainerEtot=0;
    const xAOD::CaloClusterContainer * truthClusterContainerEvis=0;
    const xAOD::CaloClusterContainer * truthClusterContainerEem=0;

    truthClusterContainerEtot = evtStore()->retrieve<const xAOD::CaloClusterContainer>("TruthLArClustersEtot");
    truthClusterContainerEvis = evtStore()->retrieve<const xAOD::CaloClusterContainer>("TruthLArClustersEvis");
    truthClusterContainerEem = evtStore()->retrieve<const xAOD::CaloClusterContainer>("TruthLArClustersEem");
    if (!truthClusterContainerEtot || !truthClusterContainerEvis || !truthClusterContainerEem) {
      ATH_MSG_ERROR("Failed to retrieve truth cluster container");
      return StatusCode::FAILURE;
    }
    
    SG::AuxElement::Decorator< ElementLink<xAOD::CaloClusterContainer> > linkDecoratorClusterEtot("truthLArClusterEtotLink");
    SG::AuxElement::Decorator< ElementLink<xAOD::CaloClusterContainer> > linkDecoratorClusterEvis("truthLArClusterEvisLink");
    SG::AuxElement::Decorator< ElementLink<xAOD::CaloClusterContainer> > linkDecoratorClusterEem("truthLArClusterEemLink");

    ElementLink<xAOD::CaloClusterContainer> truthClusterEtot(*truthClusterContainerEtot,0);
    ElementLink<xAOD::CaloClusterContainer> truthClusterEvis(*truthClusterContainerEvis,0);
    ElementLink<xAOD::CaloClusterContainer> truthClusterEem(*truthClusterContainerEem,0);
    
    ATH_MSG_DEBUG("Decorating truth parts with truth cluster energy");
    for (const auto truthPart: *truthPartContainer) {
      if (!truthPart) continue;
      if (truthPart->barcode() != m_singleParticleBarcode) continue;
      linkDecoratorClusterEtot(*truthPart) = truthClusterEtot;
      linkDecoratorClusterEvis(*truthPart) = truthClusterEvis;
      linkDecoratorClusterEem(*truthPart) = truthClusterEem;	    
    }
    return StatusCode::SUCCESS;
  }
}

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// CellReweight includes
#include "DerivationFrameworkCalo/EGammaReweight.h"
#include "CaloClusterCorrection/CaloFillRectangularCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "egammaInterfaces/IegammaSwTool.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/Egamma.h"
#include "xAODBase/IParticleHelpers.h"

#define Decorate(var) egamma->auxdecor<float>((m_decorationPrefix+#var).c_str()) = newEgamma->showerShapeValue(xAOD::EgammaParameters::var)

DerivationFramework::EGammaReweight::EGammaReweight( const std::string& t,
                          const std::string& n,
                          const IInterface* p) :
  AthAlgTool(t,n,p)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("NewCellContainerName", m_newCellContainerName = "NewCellContainer", "Name of new cell container" );
  declareProperty("ClusterCorrectionToolName", m_ClusterCorrectionToolName = "egammaSwTool/egammaswtool");
  declareProperty("EMShowerBuilderTool", m_EMShowerBuilderTool, "Handle of the EMShowerBuilderTool");
  declareProperty("SGKey_photons", m_SGKey_photons, "Name of photon container");
  declareProperty("SGKey_electrons", m_SGKey_electrons, "Name of electron container");
  declareProperty("DecorateEGammaObjects", m_doDecoration = false, "Decorate or not the original egamma objects with the reweighted shower shape values");
  declareProperty("DecorationPrefix", m_decorationPrefix = "EG_", "Decoration Prefix");
  declareProperty("NewElectronContainer", m_SGKey_newElectrons , "Name of new electron container");
  declareProperty("NewPhotonContainer", m_SGKey_newPhotons , "Name of new photon container");
  declareProperty("CaloClusterLinkName", m_CaloClusterLinkName, "Name of links to calorimeter clusters");
  declareProperty("SaveReweightedContainer", m_saveRewContainer = true, "If the egamma container built from the reweighted cells is saved");
}

DerivationFramework::EGammaReweight::~EGammaReweight() {}


StatusCode DerivationFramework::EGammaReweight::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  if (!m_doDecoration && ! m_saveRewContainer) {
    ATH_MSG_WARNING("Both DecorateEGammaObjects and SaveReweightedContainer are OFF: are you sure? In that case, there is no need to run this tool");
  }

  if (m_doDecoration && m_saveRewContainer) {
    ATH_MSG_WARNING("Both DecorateEGammaObjects and SaveReweightedContainer are ON: original egammas will be decorated, and also new objects with the new shower shape values will be created. Are you sure? This will result in duplication of information and waste of disk space");
  }

  if(m_EMShowerBuilderTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve EMShowerBuilderTool");
    return StatusCode::FAILURE;
  }

  m_clusterCorrectionTool = ToolHandle<IegammaSwTool>(m_ClusterCorrectionToolName);
  if(m_clusterCorrectionTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve " << m_clusterCorrectionTool);
    return StatusCode::SUCCESS;
  }
  else {
    ATH_MSG_DEBUG("Retrieved tool " << m_clusterCorrectionTool);
  }

  if(m_SGKey_photons == "" and m_SGKey_electrons == ""){
    ATH_MSG_FATAL("No e-gamma collection provided");
    return StatusCode::FAILURE;
  }

  if(m_CaloClusterLinkName == ""){
    ATH_MSG_FATAL("No name of the link to the calo cluster decoration provided");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::EGammaReweight::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::EGammaReweight::addBranches() const
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  // check that new containers don't already exist
  if (m_saveRewContainer) {
    bool newContainersAlreadyExist=true;
    if (m_SGKey_newPhotons!="" && !(evtStore()->contains<xAOD::PhotonContainer>(m_SGKey_newPhotons.c_str()))) newContainersAlreadyExist=false;
    if (m_SGKey_newElectrons!="" && !(evtStore()->contains<xAOD::ElectronContainer>(m_SGKey_newElectrons.c_str()))) newContainersAlreadyExist=false;
    if (newContainersAlreadyExist) {
      ATH_MSG_WARNING("The photon container " << m_SGKey_newPhotons << " and the electron container " << m_SGKey_newElectrons << " are already present in the event, no action will be taken. If this is not intended, modify your code to use a different container name.");
      return StatusCode::SUCCESS;
    }
  }

  // retrieve original and reweighted cell containers
  const CaloCellContainer * newCells = nullptr;
  const CaloCellContainer * oldCells = nullptr;
  ATH_CHECK( evtStore()->retrieve(newCells, m_newCellContainerName) );
  ATH_CHECK( evtStore()->retrieve(oldCells, "AODCellContainer") );
  
  // iterate over original egammas containers
  for (const std::string& SGkey : {m_SGKey_photons, m_SGKey_electrons}){
    if(SGkey == "") continue;

    // retrieve original egamma container and create new egamma container
    const xAOD::EgammaContainer* egammas = nullptr;
    const xAOD::PhotonContainer* a = 0;
    const xAOD::ElectronContainer* b = 0;
    xAOD::ElectronContainer *elcont = new xAOD::ElectronContainer();
    xAOD::ElectronAuxContainer *elcontaux = new xAOD::ElectronAuxContainer();
    xAOD::PhotonContainer *gammacont = new xAOD::PhotonContainer();
    xAOD::PhotonAuxContainer *gammacontaux = new xAOD::PhotonAuxContainer();
    elcont->setStore(elcontaux);
    gammacont->setStore(gammacontaux);
    ATH_CHECK( evtStore()->retrieve(egammas, SGkey));
    xAOD::EgammaContainer* egammasCopy;
    if( (b = dynamic_cast<const xAOD::ElectronContainer*>(egammas)) ){
      for(auto el : *b)
        elcont->push_back(new xAOD::Electron(*el));
      egammasCopy = elcont;
    }
    else if( (a = dynamic_cast<const xAOD::PhotonContainer*>(egammas)) ){
      for(auto gamma : *a)
        gammacont->push_back(new xAOD::Photon(*gamma));
      egammasCopy = gammacont;
    }
    else { ATH_MSG_ERROR("Egamma container " << SGkey << " is not photons or electrons!!??"); return StatusCode::FAILURE; }
    
    // overate over the new egammas and decorate the old ones with the modified shower shapes
    unsigned int iEgamma = 0;
    for(auto newEgamma : *(egammasCopy)) {
      ATH_MSG_DEBUG ("debug for elec "<<(egammas->at(iEgamma))->auxdecor< ElementLink< xAOD::CaloClusterContainer > >(m_CaloClusterLinkName.c_str()));
      const std::vector<ElementLink< xAOD::CaloClusterContainer>>  temp_links{(egammas->at(iEgamma))->auxdecor< ElementLink< xAOD::CaloClusterContainer > >(m_CaloClusterLinkName.c_str())};
      const std::vector<ElementLink< xAOD::CaloClusterContainer>>  temp_links_ori{(egammas->at(iEgamma))->caloClusterLink()};
      newEgamma->setCaloClusterLinks(temp_links);
      
      if(newEgamma->caloCluster()) 
	{
	  CHECK(m_EMShowerBuilderTool->recoExecute(newEgamma,newCells));
	  if (m_doDecoration) {
	    DecorateEGamma(egammas->at(iEgamma),newEgamma);
	    ATH_MSG_DEBUG ("Eratio, before: " << (egammas->at(iEgamma))->showerShapeValue(xAOD::EgammaParameters::Eratio) <<" after: "<< newEgamma->showerShapeValue(xAOD::EgammaParameters::Eratio) ); 
	    ATH_MSG_DEBUG ("weta2, before: " << (egammas->at(iEgamma))->showerShapeValue(xAOD::EgammaParameters::weta2) <<" after: "<< newEgamma->showerShapeValue(xAOD::EgammaParameters::weta2) );
	    ATH_MSG_DEBUG ("Reta, before: " << (egammas->at(iEgamma))->showerShapeValue(xAOD::EgammaParameters::Reta) <<" after: "<< newEgamma->showerShapeValue(xAOD::EgammaParameters::Reta) );
	    ATH_MSG_DEBUG ("Rphi, before: " << (egammas->at(iEgamma))->showerShapeValue(xAOD::EgammaParameters::Rphi) <<" after: "<< newEgamma->showerShapeValue(xAOD::EgammaParameters::Rphi) );
	    ATH_MSG_DEBUG ("e132, before: " << (egammas->at(iEgamma))->showerShapeValue(xAOD::EgammaParameters::e132) << " after: " <<  newEgamma->showerShapeValue(xAOD::EgammaParameters::e132) );
	    ATH_MSG_DEBUG ("f1, before: " << (egammas->at(iEgamma))->showerShapeValue(xAOD::EgammaParameters::f1) << " after: " <<  newEgamma->showerShapeValue(xAOD::EgammaParameters::f1) );
	    ATH_MSG_DEBUG ("widths1, before: " << (egammas->at(iEgamma))->showerShapeValue(xAOD::EgammaParameters::widths1) << " after: " <<  newEgamma->showerShapeValue(xAOD::EgammaParameters::widths1) );
	    ATH_MSG_DEBUG ("wtots1, before: " << (egammas->at(iEgamma))->showerShapeValue(xAOD::EgammaParameters::wtots1) << " after: " <<  newEgamma->showerShapeValue(xAOD::EgammaParameters::wtots1) );
	    ATH_MSG_DEBUG ("weta2, before: " << (egammas->at(iEgamma))->showerShapeValue(xAOD::EgammaParameters::weta2) << " after: " <<  newEgamma->showerShapeValue(xAOD::EgammaParameters::weta2) );
	  }
	}
      newEgamma->setCaloClusterLinks(temp_links_ori);	 // new cluster is not available in DAOD so still need to set back to original cluster 
      
      iEgamma++;
    }

    // record reweighted container
    if (m_saveRewContainer) {
      if(a) {  //Photon Container Exist
	ATH_MSG_DEBUG ("Storing new container " << m_SGKey_newPhotons.c_str() << "...");
	CHECK(evtStore()->record(gammacont, m_SGKey_newPhotons.c_str()));
	CHECK(evtStore()->record(gammacontaux, (m_SGKey_newPhotons+"Aux.").c_str()));
	delete elcont;
	delete elcontaux;
      }
      else if(b) { //Electron Container Exist
	ATH_MSG_DEBUG ("Storing new container " << m_SGKey_newElectrons.c_str() << "...");
	CHECK(evtStore()->record(elcont, m_SGKey_newElectrons.c_str()));
	CHECK(evtStore()->record(elcontaux, (m_SGKey_newElectrons+"Aux.").c_str()));
	delete gammacont;
	delete gammacontaux;
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

void DerivationFramework::EGammaReweight::DecorateEGamma(const xAOD::Egamma *egamma, xAOD::Egamma *newEgamma) const{
  Decorate(e011);      
  Decorate(e033);
  Decorate(e132);
  Decorate(e1152);
  Decorate(ethad1);
  Decorate(ethad);
  Decorate(ehad1);
  Decorate(f1);      
  Decorate(f3);      
  Decorate(f1core);
  Decorate(f3core);
  Decorate(e233);
  Decorate(e235);
  Decorate(e255);
  Decorate(e237);
  Decorate(e277);
  Decorate(e333);
  Decorate(e335);
  Decorate(e337);
  Decorate(e377);
  Decorate(weta1);
  Decorate(weta2);
  Decorate(e2ts1);
  Decorate(e2tsts1);
  Decorate(fracs1);
  Decorate(widths1);
  Decorate(widths2);
  Decorate(poscs1);
  Decorate(poscs2);
  Decorate(asy1);
  Decorate(pos);
  Decorate(pos7);
  Decorate(barys1);
  Decorate(wtots1);
  Decorate(emins1);
  Decorate(emaxs1);
  Decorate(r33over37allcalo);
  Decorate(ecore);
  Decorate(Reta);
  Decorate(Rphi);
  Decorate(Eratio);
  Decorate(Rhad);
  Decorate(Rhad1);
  Decorate(DeltaE);
}

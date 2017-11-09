/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#define Decorate(var) egamma->auxdecor<float>((PreNm+#var).c_str()) = c_egamma->showerShapeValue(xAOD::EgammaParameters::var)
//using namespace xAOD::EgammaParameters;
DerivationFramework::EGammaReweight::EGammaReweight( const std::string& t,
                          const std::string& n,
                          const IInterface* p) :
  AthAlgTool(t,n,p)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("NewCellContainerName", m_name_new = "NewCellContainer", "Name of new cell container" );
  declareProperty("ClusterCorrectionToolName", m_ClusterCorrectionToolName = "egammaSwTool/egammaswtool");
  declareProperty("EMShowerBuilderTool", m_EMShowerBuilderTool, "Handle of the EMShowerBuilderTool");
  declareProperty("SGKey_photons", m_SGKey_photons, "Name of photon container");
  declareProperty("SGKey_electrons", m_SGKey_electrons, "Name of electron container");
  declareProperty("doDecorate", isDecor = true, "whether decorate the new Sw to original elecs");
  declareProperty("DecorPrefix", PreNm = "EG_", "Decoration Prefix");
  declareProperty("NewElectronContainer", name_electron , "Name of new electron container");
  declareProperty("NewPhotonContainer", name_gamma , "Name of new photon container");
  declareProperty("CaloClusterLinkName", m_CaloClusterLinkName, "Name of links to calorimeter clusters");
}

DerivationFramework::EGammaReweight::~EGammaReweight() {}


StatusCode DerivationFramework::EGammaReweight::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
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
  
  if (evtStore()->contains<xAOD::PhotonContainer>(name_gamma.c_str()) &&
      evtStore()->contains<xAOD::ElectronContainer>(name_electron.c_str())) {
    ATH_MSG_WARNING("The photon container " << name_gamma << " and the electron container " << name_electron << " are already present in the event, no action will be taken. If this is not intended, modify your code to use a different container name.");
    return StatusCode::SUCCESS;
  }

  const CaloCellContainer * newCells = nullptr;
  const CaloCellContainer * oldCells = nullptr;
  ATH_CHECK( evtStore()->retrieve(newCells, m_name_new) );
  ATH_CHECK( evtStore()->retrieve(oldCells, "AODCellContainer") );
  
  for (const std::string& SGkey : {m_SGKey_photons, m_SGKey_electrons}){
    if(SGkey == "") continue;
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
    
    int nel = 0;
    for(auto c_egamma : *(egammasCopy)) {
      ATH_MSG_DEBUG ("debug for elec "<<(egammas->at(nel))->auxdecor< ElementLink< xAOD::CaloClusterContainer > >(m_CaloClusterLinkName.c_str()));
      const std::vector<ElementLink< xAOD::CaloClusterContainer>>  temp_links{(egammas->at(nel))->auxdecor< ElementLink< xAOD::CaloClusterContainer > >(m_CaloClusterLinkName.c_str())};
      const std::vector<ElementLink< xAOD::CaloClusterContainer>>  temp_links_ori{(egammas->at(nel))->caloClusterLink()};
      c_egamma->setCaloClusterLinks(temp_links);
      
      if(c_egamma->caloCluster()) 
	{
	  CHECK(m_EMShowerBuilderTool->recoExecute(c_egamma,newCells));
	  if(isDecor) DecorateEGamma(egammas->at(nel),c_egamma);
	  ATH_MSG_DEBUG ("Eratio"<< (egammas->at(nel))->auxdecor<float>("EG_Eratio")  << " bf " << (egammas->at(nel))->showerShapeValue(xAOD::EgammaParameters::Eratio) <<" "<< c_egamma->showerShapeValue(xAOD::EgammaParameters::Eratio) );
	  ATH_MSG_DEBUG ("weta2"<< (egammas->at(nel))->auxdecor<float>("EG_weta2")  << " bf " << (egammas->at(nel))->showerShapeValue(xAOD::EgammaParameters::weta2) <<" "<< c_egamma->showerShapeValue(xAOD::EgammaParameters::weta2) );
	}
      c_egamma->setCaloClusterLinks(temp_links_ori);	 // new cluster is not available in DAOD so still need to set back to original cluster 
      
      nel++;
    }
    if(a) {  //Photon Container Exist
      ATH_MSG_DEBUG ("Storing new container " << name_gamma.c_str() << "...");
      CHECK(evtStore()->record(gammacont, name_gamma.c_str()));
      CHECK(evtStore()->record(gammacontaux, (name_gamma+"Aux.").c_str()));
      delete elcont;
      delete elcontaux;
    }
    else if(b) { //Electron Container Exist
      ATH_MSG_DEBUG ("Storing new container " << name_electron.c_str() << "...");
      CHECK(evtStore()->record(elcont, name_electron.c_str()));
      CHECK(evtStore()->record(elcontaux, (name_electron+"Aux.").c_str()));
      delete gammacont;
      delete gammacontaux;
    }
  }
  
  return StatusCode::SUCCESS;
}

void DerivationFramework::EGammaReweight::DecorateEGamma(const xAOD::Egamma *egamma, xAOD::Egamma *c_egamma) const{
  // GM: isn't there a better way to get the list of shower shapes for an egamma object and copy everything from c_egamma to egamma?
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
  //    Decorate(NumberOfShowerShapes);
}

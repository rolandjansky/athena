/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Gaudi/Athena include(s):
#include "StoreGate/ReadHandle.h"

// EDM include(s):
#include "CaloEvent/CaloCellContainer.h"
#include "egammaInterfaces/IegammaStripsShape.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"

// Local include(s):
#include "egammaRec/egammaAODFixAlg.h"

// Misc
#include <memory>


egammaAODFixAlg::egammaAODFixAlg( const std::string& name,
				  ISvcLocator* svcLoc )
  : AthAlgorithm( name, svcLoc ),
    m_stripsShapeTool("egammaStripsShape/egammastripsshape")
{
  declareProperty("ElectronContainerName",
		  m_electronContainerName="Electrons",
		  "Name of Electron Container to be fixed");

  declareProperty("PhotonContainerName",
		  m_photonContainerName="Photons",
		  "Name of Photon Container to be fixed");

  declareProperty("CellsName", m_cellsKey = "AODCellContainer",
		  "The name of the topocluster collection");

  declareProperty("egammaStripsShapeTool", m_stripsShapeTool, "Tool needed to recalculate strips");


}

StatusCode egammaAODFixAlg::initialize() {
  ATH_CHECK(m_cellsKey.initialize());
  ATH_CHECK(m_stripsShapeTool.retrieve());
  // Return gracefully:
  return StatusCode::SUCCESS;
  
}

StatusCode egammaAODFixAlg::execute() {

  SG::ReadHandle<CaloCellContainer> cellcoll(m_cellsKey);
  
  if(!cellcoll.isValid()) {
    ATH_MSG_ERROR("Failed to retrieve cell container: "<< m_cellsKey.key());
    return StatusCode::FAILURE;
  }
    
  // Code copied from InDetVxLinksToMuons (via MuonAODFixAlg)

  // first do electrons
  const SG::DataProxy* proxyEl =
    evtStore()->proxy (ClassID_traits<xAOD::ElectronContainer>::ID(),
                       m_electronContainerName);
  if (!proxyEl) {
    ATH_MSG_WARNING( "No xAOD::ElectronContainer "
		     << "with key \"" << m_electronContainerName << "\" found" );
    return StatusCode::SUCCESS;
  }

  xAOD::ElectronContainer* electrons=nullptr;
  
  if (proxyEl->isConst()) {
    const xAOD::ElectronContainer* originalElectrons = nullptr;
    ATH_CHECK( evtStore()->retrieve (originalElectrons,
				     m_electronContainerName) );
    
    electrons = new xAOD::ElectronContainer;
    auto store = std::make_unique<xAOD::ElectronAuxContainer>();
    electrons->setStore (store.get());
    for (const xAOD::Electron* oldElectron : *originalElectrons) {
      xAOD::Electron* electron = new xAOD::Electron;
      electrons->push_back (electron);
      *electrons->back() = *oldElectron;
      ATH_CHECK(fixEgamma(electron, cellcoll.ptr()));
    }
    ATH_CHECK( evtStore()->overwrite (electrons,
				      m_electronContainerName,
				      true, false) );
    ATH_CHECK( evtStore()->overwrite (std::move(store),
				      m_electronContainerName + "Aux.",
				      true, false) );
  } else {
    ATH_CHECK( evtStore()->retrieve (electrons,
				     m_electronContainerName) );
    
    for (xAOD::Electron* electron : *electrons) {
      ATH_CHECK(fixEgamma(electron, cellcoll.ptr()));
    }
  }

  // now do photons
  const SG::DataProxy* proxyPh =
    evtStore()->proxy (ClassID_traits<xAOD::PhotonContainer>::ID(),
                       m_photonContainerName);
  if (!proxyPh) {
    ATH_MSG_WARNING( "No xAOD::PhotonContainer "
		     << "with key \"" << m_photonContainerName << "\" found" );
    return StatusCode::SUCCESS;
  }

  xAOD::PhotonContainer* photons=nullptr;
  
  if (proxyPh->isConst()) {
    const xAOD::PhotonContainer* originalPhotons = nullptr;
    ATH_CHECK( evtStore()->retrieve (originalPhotons,
				     m_photonContainerName) );
    
    photons = new xAOD::PhotonContainer;
    auto store = std::make_unique<xAOD::PhotonAuxContainer>();
    photons->setStore (store.get());
    for (const xAOD::Photon* oldPhoton : *originalPhotons) {
      xAOD::Photon* photon = new xAOD::Photon;
      photons->push_back (photon);
      *photons->back() = *oldPhoton;
      ATH_CHECK(fixEgamma(photon, cellcoll.ptr()));
    }
    ATH_CHECK( evtStore()->overwrite (photons,
				      m_photonContainerName,
				      true, false) );
    ATH_CHECK( evtStore()->overwrite (std::move(store),
				      m_photonContainerName + "Aux.",
				      true, false) );
  } else {
    ATH_CHECK( evtStore()->retrieve (photons,
				     m_photonContainerName) );
    
    for (xAOD::Photon* photon : *photons) {
      ATH_CHECK(fixEgamma(photon, cellcoll.ptr()));
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode egammaAODFixAlg::fixEgamma(xAOD::Egamma* eg,  const CaloCellContainer *cell_container) {
  const auto clus = eg->caloCluster();
  ATH_CHECK(m_stripsShapeTool->execute(clus, cell_container));

  // copying from EMShowerBuilder. 
  float value=0;
  // E in 3x2 cells in S1
  float e132 = static_cast<float>(m_stripsShapeTool->e132());
  eg->setShowerShapeValue(e132, xAOD::EgammaParameters::e132);
  // E in 15x2 cells in S1
  float olde1152 = 0;
  bool validOlde1152 = eg->showerShapeValue(olde1152, xAOD::EgammaParameters::e1152);
  float e1152 = static_cast<float>(m_stripsShapeTool->e1152());
  eg->setShowerShapeValue(e1152, xAOD::EgammaParameters::e1152);
  // fraction of E in S1
  value=static_cast<float>(m_stripsShapeTool->f1());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::f1);
  // fraction of E in the core(e132) in S1
  value=static_cast<float>(m_stripsShapeTool->f1core());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::f1core);
  // corr width with 3 strips 
  value=static_cast<float>(m_stripsShapeTool->ws3c());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::weta1);
  // energy in second max
  value=static_cast<float>(m_stripsShapeTool->esec());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::e2ts1);
  // energy strip of second max
  float esec = static_cast<float>(m_stripsShapeTool->esec1());
  eg->setShowerShapeValue(esec, xAOD::EgammaParameters::e2tsts1);
  // fraction of E outside core in S1
  value=static_cast<float>(m_stripsShapeTool->fside());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::fracs1);
  // width with 5 strips
  value=static_cast<float>(m_stripsShapeTool->widths5());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::widths1);
  //eta pos within cell in S1
  value=static_cast<float>(m_stripsShapeTool->poscs1());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::poscs1);
  // asymmetry with 3 strips
  value=static_cast<float>(m_stripsShapeTool->asymmetrys3());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::asy1);
  // diff position +/- 1 cells
  value=static_cast<float>(m_stripsShapeTool->deltaEtaTrackShower());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::pos);
  // diff position +/- 7 cells
  value=static_cast<float>(m_stripsShapeTool->deltaEtaTrackShower7());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::pos7);
  // E of strip with min E 
  float emin = static_cast<float>(m_stripsShapeTool->emins1());
  eg->setShowerShapeValue(emin, xAOD::EgammaParameters::emins1);
  // E of strip with max E
  float emax = static_cast<float>(m_stripsShapeTool->emaxs1());
  eg->setShowerShapeValue(emax, xAOD::EgammaParameters::emaxs1);
  // barycentre in eta in S1
  value=static_cast<float>(m_stripsShapeTool->etas3());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::barys1);
  // total width in strips
  value=static_cast<float>(m_stripsShapeTool->wstot());
  eg->setShowerShapeValue(value, xAOD::EgammaParameters::wtots1);
  //
  // information combining all samplings
  //
  // core energy
  float unfixedecore = 0;
  if (validOlde1152 && eg->showerShapeValue(unfixedecore, xAOD::EgammaParameters::ecore)) {
    float ecore = unfixedecore - olde1152 + e1152;
    eg->setShowerShapeValue(ecore, xAOD::EgammaParameters::ecore);
  } else {
    ATH_MSG_WARNING("Missing parameter ecore or e1152");
  }
  
  //
  // information combining different shower shape
  //
  /// @brief (emaxs1-e2tsts1)/(emaxs1+e2tsts1)

  const auto Eratio = std::abs(emax+esec)>0. ? (emax-esec)/(emax+esec) : 0;
  const auto DeltaE = esec - emin;

  eg->setShowerShapeValue(Eratio, xAOD::EgammaParameters::Eratio);

  eg->setShowerShapeValue(DeltaE, xAOD::EgammaParameters::DeltaE);
  
  return StatusCode::SUCCESS;
}


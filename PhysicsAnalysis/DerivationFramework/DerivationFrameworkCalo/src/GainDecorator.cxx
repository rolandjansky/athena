/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// GainDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Simone Mazza (simone.mazza@mi.infn.it), Bruno Lenzi
// Decorate egamma objects with the energy and number of cells per layer per gain 

#include "DerivationFrameworkCalo/GainDecorator.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/EgammaContainer.h"
// #include "CaloIdentifier/CaloCell_ID.h"
// #include "CaloGeoHelpers/CaloSampling.h"

#include <vector>
#include <string>
#include <regex>

// Constructor
DerivationFramework::GainDecorator::GainDecorator(const std::string& t,
						  const std::string& n,
						  const IInterface* p) :
  AthAlgTool(t,n,p),
  m_SGKey_photons(""),
  m_SGKey_electrons("")
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("SGKey_photons", m_SGKey_photons);
  declareProperty("SGKey_electrons", m_SGKey_electrons);
  declareProperty("decoration_pattern", m_decorationPattern = "{info}_Lr{layer}_{gain}G");
  declareProperty("gain_names", m_gainNames = {
    {CaloGain::LARHIGHGAIN, "Hi"},
    {CaloGain::LARMEDIUMGAIN, "Med"},
    {CaloGain::LARLOWGAIN, "Low"} } );
  declareProperty("layers", m_layers = {0,1,2,3});

  // Define the names for the decorations
  for (const auto& kv : m_gainNames)
    for (const auto layer : m_layers)
    {
      std::string name = m_decorationPattern;
      name.replace(name.find("{layer}"), std::string("{layer}").size(), std::to_string(layer));
      name.replace(name.find("{gain}"), std::string("{gain}").size(), kv.second);
      std::string name_E(name), name_nCells(name);
      name_E.replace(name_E.find("{info}"), std::string("{info}").size(), "E");
      name_nCells.replace(name_nCells.find("{info}"), std::string("{info}").size(), "nCells");
      
      std::pair<int, int> key( kv.first, layer );
      m_names_E[ key ] = name_E;
      m_names_nCells[ key ] = name_nCells;
    }
  
  for (auto kv : m_names_E) ATH_MSG_DEBUG("Decorating (layer, gain): " << kv.first << " " << kv.second );
  for (auto kv : m_names_nCells) ATH_MSG_DEBUG("Decorating (layer, gain): " << kv.first << " " << kv.second );
    
}

// Destructor
DerivationFramework::GainDecorator::~GainDecorator() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::GainDecorator::initialize()
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
  
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::GainDecorator::finalize()
{

  return StatusCode::SUCCESS;
}

// The decoration itself
StatusCode DerivationFramework::GainDecorator::addBranches() const
{
  // Retrieve photon container
  const xAOD::EgammaContainer* importedPhotons(0);
  if(m_SGKey_photons != ""){
    if (evtStore()->retrieve(importedPhotons,m_SGKey_photons).isFailure()) {
      ATH_MSG_ERROR("No e-gamma collection with name " << m_SGKey_photons << " found in StoreGate!");
      return StatusCode::FAILURE;
    }
    for (auto photon : *importedPhotons) {
      decorateObject(photon);
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

void DerivationFramework::GainDecorator::decorateObject(const xAOD::Egamma*& egamma) const{

    // Compute energy and number of cells per gain per layer
    // Set the initial values to 0 (needed?)
    std::map< std::pair<int, int>, float > E;
    std::map< std::pair<int, int>, uint8_t > nCells;
    for (const auto& kv : m_names_E)
    {
      E[kv.first] = 0.;
      nCells[kv.first] = 0;
    }
    
    // Skip the computation for missing cell links (like topo-seeded photons)
    // but decorate anyway
    const CaloClusterCellLink* cellLinks = egamma->caloCluster() ? egamma->caloCluster()->getCellLinks() : 0;
    if (cellLinks) 
    {
      for (const CaloCell *cell : *cellLinks)
      {
        if (!cell) continue;
        std::pair<int, int> key( static_cast<int>(cell->gain()), getLayer(cell) );
        // Increment the corresponding entry (not important if it is not initialised)
        E[key] += cell->energy();
        nCells[key]++;
      }
    }
    
    // Decorate    
    for (const auto& kv : m_names_E) egamma->auxdecor<float>(kv.second) = E[ kv.first ];
    for (const auto& kv : m_names_nCells) egamma->auxdecor<uint8_t>(kv.second) = nCells[ kv.first ];

}

int DerivationFramework::GainDecorator::getLayer(const CaloCell *cell)
{
  int sampling = (cell && cell->caloDDE() ? cell->caloDDE()->getSampling() : -1);
  if (sampling==CaloSampling::PreSamplerB || sampling==CaloSampling::PreSamplerE) return 0;
  if (sampling==CaloSampling::EMB1 || sampling==CaloSampling::EME1) return 1;
  if (sampling==CaloSampling::EMB2 || sampling==CaloSampling::EME2) return 2;
  if (sampling==CaloSampling::EMB3 || sampling==CaloSampling::EME3) return 3;
  return -1;
}

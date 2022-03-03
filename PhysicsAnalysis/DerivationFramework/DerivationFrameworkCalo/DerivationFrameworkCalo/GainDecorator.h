/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GainDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Decorate egamma objects with the energy and number of cells per layer per
// gain

#ifndef DERIVATIONFRAMEWORK_GainDecorator_H
#define DERIVATIONFRAMEWORK_GainDecorator_H

#include <map>
#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "CaloEvent/CaloCell.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKeyArray.h"
#include "xAODEgamma/EgammaContainer.h"

namespace DerivationFramework {

class GainDecorator
  : public AthAlgTool
  , public IAugmentationTool
{
public:
  GainDecorator(const std::string& t,
                const std::string& n,
                const IInterface* p);
  ~GainDecorator();
  StatusCode initialize();
  StatusCode finalize();
  virtual StatusCode addBranches() const;
  static int getLayer(const CaloCell* cell);

  struct calculation
  {
    std::map<std::pair<int, int>, float> E;
    std::map<std::pair<int, int>, uint8_t> nCells;
  };

private:
  SG::ReadHandleKey<xAOD::EgammaContainer>
    m_SGKey_photons{ this, "SGKey_photons", "", "SG key of photon container" };

  SG::ReadHandleKey<xAOD::EgammaContainer> m_SGKey_electrons{
    this,
    "SGKey_electrons",
    "",
    "SG key of electron container"
  };

  std::string m_decorationPattern;
  std::map<int, std::string> m_gainNames;
  std::vector<unsigned int> m_layers;

  // Name of the decorations
  std::map<std::pair<int, int>, std::string> m_names_E;
  std::map<std::pair<int, int>, std::string> m_names_nCells;

  SG::WriteDecorHandleKeyArray<xAOD::EgammaContainer>
    m_SGKey_photons_decorations{
      this,
      "SGKey_photons_decorations_noConf",
      {},
      "SG keys for photon decorations not really configurable"
    };

  SG::WriteDecorHandleKeyArray<xAOD::EgammaContainer>
    m_SGKey_electrons_decorations{
      this,
      "SGKey_electrons_decorations_noConf",
      {},
      "SG keys for electrons decorations not really configurable"
    };

  calculation decorateObject(const xAOD::Egamma*& egamma) const;
};
}

#endif // DERIVATIONFRAMEWORK_GainDecorator_H

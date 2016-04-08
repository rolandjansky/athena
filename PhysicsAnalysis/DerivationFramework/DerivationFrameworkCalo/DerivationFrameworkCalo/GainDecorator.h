/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GainDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Decorate egamma objects with the energy and number of cells per layer per gain 

#ifndef DERIVATIONFRAMEWORK_GainDecorator_H
#define DERIVATIONFRAMEWORK_GainDecorator_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODEgamma/EgammaFwd.h"
#include "CaloEvent/CaloCellContainer.h"

namespace DerivationFramework {

  class GainDecorator : public AthAlgTool, public IAugmentationTool {
    public:
      GainDecorator(const std::string& t, const std::string& n, const IInterface* p);
      ~GainDecorator();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;      
      static int getLayer(const CaloCell *cell);
      

    private:
      std::string m_SGKey_photons;
      std::string m_SGKey_electrons;
      std::string m_decorationPattern;
      std::map<int, std::string> m_gainNames;
      std::vector<unsigned int> m_layers;
      
      // Name of the decorations
      std::map< std::pair<int,int>, std::string> m_names_E;
      std::map< std::pair<int,int>, std::string> m_names_nCells;

      void decorateObject(const xAOD::Egamma*& egamma) const;
  };
}

#endif // DERIVATIONFRAMEWORK_GainDecorator_H

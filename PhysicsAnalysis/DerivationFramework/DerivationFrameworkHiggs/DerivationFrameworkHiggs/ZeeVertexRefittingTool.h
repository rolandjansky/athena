/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGVertexRefittingTool.h, (c) ATLAS Detector software
// author: ioannis.nomidis@cern.ch
///////////////////////////////////////////////////////////////////

/**
 * refit the primary vertex after removing the tracks of electrons from a Z->ee decay
 * to imitate the primary vertex reconstruction in H->yy events with unconverted photons
*/

#ifndef DERIVATIONFRAMEWORK_ZEEVERTEXREFITTINGTOOL_H
#define DERIVATIONFRAMEWORK_ZEEVERTEXREFITTINGTOOL_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODEgamma/ElectronContainer.h"
#include "GaudiKernel/ToolHandle.h"

namespace ExpressionParsing { class ExpressionParser; }
namespace Analysis { class PrimaryVertexRefitter; }

namespace DerivationFramework {

  class ZeeVertexRefittingTool : public AthAlgTool, public IAugmentationTool {
    public: 
      ZeeVertexRefittingTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize() override;
      StatusCode finalize() override;
      StatusCode addBranches() const override;

    private:
      std::string m_expression;
      std::unique_ptr<ExpressionParsing::ExpressionParser> m_parser;
      float m_massCut;
      std::string m_refitpvContainerName;          
      std::string m_elContainerName;
      std::string m_pvContainerName;
      std::vector<unsigned int> m_MCSamples;

      ToolHandle < Analysis::PrimaryVertexRefitter > m_pvrefitter;

      StatusCode makeZeePairs( const xAOD::ElectronContainer *particles, std::vector<std::vector<unsigned int> > &ZeePairs) const;
  }; 
}

#endif // DERIVATIONFRAMEWORK_ZEEVERTEXREFITTINGTOOL_H

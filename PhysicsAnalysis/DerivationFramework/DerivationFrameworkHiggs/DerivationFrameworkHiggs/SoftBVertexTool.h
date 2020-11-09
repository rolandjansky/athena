/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGVertexRefittingTool.h, (c) ATLAS Detector software
// author: Vadim Kostyukhin (vadim.kostyukhin@cern.ch)
///////////////////////////////////////////////////////////////////

/**
 * refit the primary vertex after removing the tracks of electrons from a Z->ee decay
 * to imitate the primary vertex reconstruction in H->yy events with unconverted photons
*/

#ifndef DERIVATIONFRAMEWORK_SOFTBVERTEXTOOL_H
#define DERIVATIONFRAMEWORK_SOFTBVERTEXTOOL_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "VrtSecInclusive/IVrtInclusive.h"

namespace ExpressionParsing { class ExpressionParser; }

namespace DerivationFramework {

  class SoftBVertexTool : public AthAlgTool, public IAugmentationTool {
    public: 
 
      SoftBVertexTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize() override;
      StatusCode finalize() override;
      StatusCode addBranches() const override;

    private:

      std::string m_bvertexContainerName;
      std::string m_trContainerName;
      std::string m_pvContainerName;

      ToolHandle < Rec::IVrtInclusive > m_bvertextool;

  }; 
}

#endif // DERIVATIONFRAMEWORK_SoftBVertexTool_H

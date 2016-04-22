/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// DiLepSkimTools.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Dominik Krauss (krauss@mpp.mpg.de)

#ifndef DERIVATIONFRAMEWORK_DILEPSKIMTOOLS_H
#define DERIVATIONFRAMEWORK_DILEPSKIMTOOLS_H 1

// STL
#include <string>

// Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "LongLivedParticleDPDMaker/DiLepFilters.h"

namespace DerivationFramework
{
  // for each filter a dedicated DiLepSkim instance is needed
  class DiLepSkim : public AthAlgTool, public ISkimmingTool
  {
    public:
      enum class Filters {None, SiEl, SiPhX, SiMu, DiEl, DiPh, DiElPh, DiLoElPh};

      DiLepSkim(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize() override;

      // checks if an event passes the filter defined by m_filter
      bool eventPassesFilter() const override;

    private:
      ToolHandle<DerivationFramework::IDiLepFilters> m_dlf;

      int m_filter_prop;
      Filters m_filter;
  };
}

#endif

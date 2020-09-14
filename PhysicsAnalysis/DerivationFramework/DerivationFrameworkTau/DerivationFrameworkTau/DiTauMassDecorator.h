/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiTauMassDecorator.h
// Author: Nadav  Tamir (nadavtamir@mail.tau.ac.il)
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_DITAUMASSDECORATOR_H
#define DERIVATIONFRAMEWORK_DITAUMASSDECORATOR_H

#include <string>
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

//Tool to decorate boosted ditaus with lead+sublead subjet invariant mass

namespace DerivationFramework {

  class DiTauMassDecorator : public AthAlgTool, public IAugmentationTool {
    public:
      DiTauMassDecorator(const std::string& t, const std::string& n, const IInterface* p);

      virtual StatusCode addBranches() const;

    private:
      std::string m_ditauContainerName;

  };
}

#endif // DERIVATIONFRAMEWORK_DITAUMASSDECORATOR_H

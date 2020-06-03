/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PrescaleTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_PRESCALETOOL_H
#define DERIVATIONFRAMEWORK_PRESCALETOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"

namespace DerivationFramework {

  class PrescaleTool : public AthAlgTool, public ISkimmingTool {
    public: 
      PrescaleTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual bool eventPassesFilter() const;

    private:
      unsigned int m_prescale;	
  }; 
}

#endif // DERIVATIONFRAMEWORK_PRESCALETOOL_H

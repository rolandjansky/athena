/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VertexThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_VERTEXTHINNING_H
#define DERIVATIONFRAMEWORK_VERTEXTHINNING_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"

// Members
#include "GaudiKernel/ServiceHandle.h"

// STL includes
#include <string>

class IThinningSvc;

namespace DerivationFramework {

  class VertexThinning : public AthAlgTool, public IThinningTool {
    public: 
      VertexThinning(const std::string& t, const std::string& n, const IInterface* p);
      ~VertexThinning();
      virtual StatusCode doThinning() const;

    private:
      ServiceHandle<IThinningSvc> m_thinningSvc;
      std::string m_vertexSGKey;
  }; 
}

#endif // DERIVATIONFRAMEWORK_JETTRACKPARTICLETHINNING_H


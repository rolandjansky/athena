/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ElectronsDirectionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_ELECTRONSDIRECTIONTOOL_H
#define DERIVATIONFRAMEWORK_ELECTRONSDIRECTIONTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  class ElectronsDirectionTool : public AthAlgTool, public IAugmentationTool {
    public: 
      ElectronsDirectionTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_sgEta;
      std::string m_sgPhi;
      std::string m_sgEt;
      std::string m_sgPt;
      std::string m_sgnPrecisionHits;
      std::string m_collName;
  }; 
}

#endif // DERIVATIONFRAMEWORK_ELECTRONSDIRECTIONTOOL_H

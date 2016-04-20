/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PhotonsDirectionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_PHOTONSDIRECTIONTOOL_H
#define DERIVATIONFRAMEWORK_PHOTONSDIRECTIONTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  class PhotonsDirectionTool : public AthAlgTool, public IAugmentationTool {
    public: 
      PhotonsDirectionTool(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_sgEta;
      std::string m_sgPhi;
      std::string m_sgEt;
      std::string m_sgE;
      std::string m_collName;
  }; 
}

#endif // DERIVATIONFRAMEWORK_PHOTONSDIRECTIONTOOL_H

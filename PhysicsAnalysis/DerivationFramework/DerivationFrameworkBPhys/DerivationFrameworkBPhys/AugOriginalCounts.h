/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AugOriginalCounts.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
 
#ifndef DERIVATIONFRAMEWORKBPHYS_AUGORIGINALCOUNTS_H
#define DERIVATIONFRAMEWORKBPHYS_AUGORIGINALCOUNTS_H
 
#include <string>
 
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
 
namespace DerivationFramework {
 
  class AugOriginalCounts : public AthAlgTool, public IAugmentationTool {
    public:
      AugOriginalCounts(const std::string& t, const std::string& n, const IInterface* p);
      virtual StatusCode addBranches() const;

    private:
      std::string m_TrackContainername;
      std::string m_PVContainername;

  };
}
 
#endif // DERIVATIONFRAMEWORKBPHYS_AUGORIGINALCOUNTS_H

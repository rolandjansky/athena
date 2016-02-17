/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef METTRIGGERAUGMENTATIONTOOL_H
#define METTRIGGERAUGMENTATIONTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include <TFile.h>
#include <TH2.h>

namespace DerivationFramework {

  class METTriggerAugmentationTool : public AthAlgTool, public IAugmentationTool {
    public: 
      METTriggerAugmentationTool(const std::string& t, const std::string& n, const IInterface* p);
      virtual StatusCode initialize();
      virtual StatusCode finalize();
      virtual StatusCode addBranches() const;
    private:
      std::string m_SGPrefix;
      std::string m_LUTFileName;
      TFile* m_LUTFile;
      TH2* m_LUT;
  };
}

#endif // METTRIGGERAUGMENTATIONTOOL_H

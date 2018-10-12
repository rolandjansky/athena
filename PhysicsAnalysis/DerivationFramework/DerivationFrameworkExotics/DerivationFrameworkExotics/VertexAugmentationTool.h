/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_VERTEXAUGMENTATIONTOOL_H
#define DERIVATIONFRAMEWORK_VERTEXAUGMENTATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"

namespace DerivationFramework {

  class VertexAugmentationTool : public AthAlgTool, public IAugmentationTool {

    public: 
      VertexAugmentationTool( const std::string& t, const std::string& n, const IInterface* p );

      ~VertexAugmentationTool();

      StatusCode  initialize();
      StatusCode  finalize();

      virtual StatusCode addBranches() const;

    private:

      std::string m_derivationPrefix;
      std::string m_vertexContainerName;


      TFile *fout;
      TH1F  *hcl_pt;
      TH1F  *hcl_e;
      TH1F  *hcl_dr;
  }; 

}

#endif

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
#include <vector>

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

      TTree *tree_muon;
      TTree *tree_NOTmuon;

      std::vector<float> *m_mu_pt;
      std::vector<float> *m_mu_eta;
      std::vector<float> *m_mu_phi;

      std::vector<float> *m_cl_pt;
      std::vector<float> *m_cl_eta;
      std::vector<float> *m_cl_phi;
      std::vector<float> *m_cl_e;
      std::vector<float> *m_cl_dr;
      std::vector<int>   *m_cl_nSample;
      std::vector<int>   *m_cl_sizePhi;
      std::vector<int>   *m_cl_sizeEta;
      std::vector<int>   *m_cl_calo;
      std::vector<float> *m_cl_layer0;
      std::vector<float> *m_cl_layer1;
      std::vector<float> *m_cl_layer2;
      std::vector<float> *m_cl_layer3;

      std::vector<float> *o_cl_pt;
      std::vector<float> *o_cl_eta;
      std::vector<float> *o_cl_phi;
      std::vector<float> *o_cl_e;
      std::vector<float> *o_cl_dr;
      std::vector<int>   *o_cl_nSample;
      std::vector<int>   *o_cl_sizePhi;
      std::vector<int>   *o_cl_sizeEta;
      std::vector<int>   *o_cl_calo;
      std::vector<float> *o_cl_layer0;
      std::vector<float> *o_cl_layer1;
      std::vector<float> *o_cl_layer2;
      std::vector<float> *o_cl_layer3;
  }; 

}

#endif

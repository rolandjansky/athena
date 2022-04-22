// this file is -*- C++ -*-
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETVERTEXNNTAGGER_H
#define JETVERTEXNNTAGGER_H

///
/// \class JetVertexNNTagger
///
/// Creates a new JetContainer by doing a shallow copy of an input JetVector
///
/// This tool implements the IJetProvider interface. The JetContainer it returns is build by
/// doing a shallow copy of an input JetVector.
/// The JetVector key is also a Property of the tool.
///

#include <string>
#include <vector>
#include <utility>
#include <memory>

#include "AsgTools/PropertyWrapper.h"
#include "AsgTools/AsgTool.h"
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgDataHandles/ReadDecorHandleKey.h"
#include "AsgDataHandles/WriteDecorHandleKey.h"
#include "JetInterface/IJetDecorator.h"

#include "xAODJet/JetContainer.h"
#include "xAODTracking/VertexContainer.h"

#include "lwtnn/generic/FastGraph.hh"

namespace JetPileupTag {

  // Because there is no SystemOfUnits.h in AnalysisBase
  constexpr float GeV=1e3;

  class JetVertexNNTagger
    : public asg::AsgTool,
      virtual public IJetDecorator
  {
    ASG_TOOL_CLASS(JetVertexNNTagger,IJetDecorator)

    public:
      using asg::AsgTool::AsgTool;

      // Called in parent initialize()
      virtual StatusCode initialize() override;

      // Inherited method to decorate a jet container
      virtual StatusCode decorate(const xAOD::JetContainer& jetCont) const override;
  
    private:

      // Determine pt, eta bin of jet
      std::pair<size_t,size_t> get_kinematic_bin(const xAOD::Jet& jet) const;
      // Retrieve hard scatter vertex for its index
      const xAOD::Vertex* findHSVertex() const;

      // Evaluate JVT from Rpt and JVFcorr
      float evaluateJvt(float rpt, float jvfcorr, size_t ptbin, size_t etabin) const;
      // Retrieve JVT cut for pt/eta bin
      float getJvtCut(size_t ptbin, size_t etabin) const;

      /// Internal members for interpreting jet inputs
      /// and NN configuration
      std::unique_ptr<lwt::generic::FastGraph<double> > m_lwnn {nullptr};
      // Discretised jet kinematic info
      std::vector<float> m_ptbin_edges;
      std::vector<float> m_etabin_edges;
      // Cut value for efficiency/rejection working point vs pt,eta bin
      std::vector<std::vector<float> > m_cut_map;

      // Generically needed for moment tools
      Gaudi::Property<std::string> m_jetContainerName{this,"JetContainer", "", "SG key for the input jet container"};
      Gaudi::Property<bool> m_suppressInputDeps{this, "SuppressInputDependence", false, "Will JVFCorr and SumPtTrk be created in the same algorithm that uses this tool?"};

      // NN configuration
      Gaudi::Property<std::string> m_NNConfigDir{this,"NNConfigDir", "JetPileupTag/NNJvt/2022-03-22", "PathResolver-accessible directory holding config files"};
      Gaudi::Property<std::string> m_NNParamFileName{this,"NNParamFile", "NNJVT.Network.graph.Offline.Nonprompt_All_MaxWeight.json", "Name of json file containing network parameters"};
      Gaudi::Property<std::string> m_NNCutFileName{this,"NNCutFile", "NNJVT.Cuts.FixedEffPt.Offline.Nonprompt_All_MaxW.json", "Name of json file containing network parameters"};
      // Additional steering
      Gaudi::Property<float> m_maxpt_for_cut{this,"MaxPtForCut", 60*GeV, "Jet pt above which no cut is applied"};


      // Access to inputs from StoreGate
      SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainer_key{this, "VertexContainer", "PrimaryVertices", "SG key for input vertex container"};
      SG::ReadDecorHandleKey<xAOD::JetContainer> m_jvfCorrKey{this, "JVFCorrName", "JVFCorr", "SG key for input JVFCorr decoration"};
      SG::ReadDecorHandleKey<xAOD::JetContainer> m_sumPtTrkKey{this, "SumPtTrkName", "SumPtTrkPt500", "SG key for input SumPtTrk decoration"};
      SG::WriteDecorHandleKey<xAOD::JetContainer> m_jvtKey{this, "JVTName", "NNJvt", "SG key for output JVT decoration"};
      SG::WriteDecorHandleKey<xAOD::JetContainer> m_rptKey{this, "RpTName", "NNJvtRpt", "SG key for output RpT decoration"};
      SG::WriteDecorHandleKey<xAOD::JetContainer> m_passJvtKey{this, "passJvtName", "NNJvtPass", "SG key for output pass-JVT decoration"};
  };

}
#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C++
// VoronoiWeightTool.h

#ifndef VORONOIWEIGHTTOOL_VORONOIWEIGHTTOOL_H
#define VORONOIWEIGHTTOOL_VORONOIWEIGHTTOOL_H 1

// \class VoronoiWeightTool
// \author Aviv Cukierman
// \date May 2016
//
// This is a tool to reweight clusters by subtracting pileup based on Voronoi areas
// It creates a new cluster collection with reweighted clusters
// Documentation: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/VoronoiWeightTool
//

#include <string>
#include <vector>
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODBase/IParticleContainer.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterChangeSignalState.h"

#include "JetInterface/IJetExecuteTool.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODCore/ShallowAuxContainer.h"

#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/Selector.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/tools/JetMedianBackgroundEstimator.hh"
#include <fastjet/tools/Subtractor.hh>

class VoronoiWeightTool : public JetConstituentModifierBase{
  ASG_TOOL_CLASS(VoronoiWeightTool, IJetConstituentModifier)
  public:
    bool m_debug = false;
    bool m_doLC = true;
    bool m_doSpread = true;
    int m_nSigma = 0;

   // Constructor with parameters:
   VoronoiWeightTool(const std::string& name);

   // Destructor:
   ~VoronoiWeightTool();
   

   // methods used in the analysis
   StatusCode MakeVoronoiClusters(std::vector< std::pair<fastjet::PseudoJet,std::vector<float> > >&) const;
   void SpreadPt(std::vector< std::pair< fastjet::PseudoJet,std::vector<float> > >& correctedptvec, float spreadr=0.4, float alpha=2) const;

  //
  // For Sorting
  //
  struct pt_sort
  {

    inline bool operator() (const TLorentzVector& lhs, const TLorentzVector& rhs)
    {
      return (lhs.Pt() > rhs.Pt());
    }

    inline bool operator() (const TLorentzVector* lhs, const TLorentzVector* rhs)
    {
      return (lhs->Pt() > rhs->Pt());
    }

    inline bool operator() (const xAOD::IParticle& lhs, const xAOD::IParticle& rhs)
    {
      return (lhs.pt() > rhs.pt());
    }

    inline bool operator() (const xAOD::IParticle* lhs, const xAOD::IParticle* rhs)
    {
      return (lhs->pt() > rhs->pt());
    }
  };


  template<typename T>
    T sort_container_pt(T* inCont) const{
    T sortedCont(SG::VIEW_ELEMENTS);
    for(auto el : *inCont) sortedCont.push_back( el );
    std::sort(sortedCont.begin(), sortedCont.end(), pt_sort());
    return sortedCont;
  }

  template<typename T>
    const T sort_container_pt(const T* inCont) const{
    ConstDataVector<T> sortedCont(SG::VIEW_ELEMENTS);

    for(auto el : *inCont) sortedCont.push_back( el );
    std::sort(sortedCont.begin(), sortedCont.end(), pt_sort());
    return *sortedCont.asDataVector();
  }

private:
  //xAOD::TEvent *m_event; //!
  //xAOD::TStore *m_store;  //!
  mutable std::vector<fastjet::PseudoJet> clusters; //!
  struct PJcomp;

public:
  // this is a standard constructor
  VoronoiWeightTool ();

  StatusCode process(xAOD::IParticleContainer* cont) const;
  StatusCode process(xAOD::CaloClusterContainer* cont) const;

  // this is needed to distribute the algorithm to the workers
  //ClassDef(VoronoiWeightTool, 1);
}; 

#endif //> !VORONOIWEIGHTTOOL_VORONOIWEIGHTTOOL_H

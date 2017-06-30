// this file is -*- C++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VoronoiWeightTool.h

#ifndef VORONOIWEIGHTTOOL_VORONOIWEIGHTTOOL_H
#define VORONOIWEIGHTTOOL_VORONOIWEIGHTTOOL_H 1

/// \class VoronoiWeightTool
/// \author Aviv Cukierman
/// \date May 2016
///
/// This is a tool to reweight clusters by subtracting pileup based on Voronoi areas
/// It creates a new cluster collection with reweighted clusters
/// Documentation: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/VoronoiWeightTool
///

#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODBase/IParticleContainer.h"

#include "fastjet/PseudoJet.hh"

class VoronoiWeightTool : public JetConstituentModifierBase{
  ASG_TOOL_CLASS(VoronoiWeightTool, IJetConstituentModifier)
public:

  // // this is a standard constructor
  // VoronoiWeightTool ();
  // Constructor with parameters:
  VoronoiWeightTool(const std::string& name);
  
  // Destructor:
  ~VoronoiWeightTool();

  // Check that the configuration is sane
  StatusCode initialize();

protected:
  // Implement the correction
  StatusCode process_impl(xAOD::IParticleContainer* cont) const;

   // methods used in the analysis
  StatusCode makeVoronoiParticles(std::vector<fastjet::PseudoJet>& particles, std::vector< std::pair<fastjet::PseudoJet,std::vector<float> > >&) const;
  void spreadPt(std::vector< std::pair< fastjet::PseudoJet,std::vector<float> > >& correctedptvec, float spreadr=0.4, float alpha=2) const;

  bool m_debug = false;
  bool m_doSpread = true;
  int m_nSigma = 0;

  bool m_ignoreChargedPFOs=true;

  // this is needed to distribute the algorithm to the workers
  //ClassDef(VoronoiWeightTool, 1);
}; 

#endif //> !VORONOIWEIGHTTOOL_VORONOIWEIGHTTOOL_H

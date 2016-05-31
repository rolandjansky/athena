/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#ifndef LEVEL1TOPOSELECTIONTOOL_H
#define LEVEL1TOPOSELECTIONTOOL_H

// Framework include
#include "AsgTools/AsgTool.h"

// Local includes
#include "TrigTauEmulation/ILevel1TopoSelectionTool.h"
#include "TrigTauEmulation/Level1SelectionTool.h"

class Level1TopoSelectionTool : public virtual ILevel1TopoSelectionTool, public Level1SelectionTool
{

  ASG_TOOL_CLASS(Level1TopoSelectionTool, ILevel1TopoSelectionTool)


 public:

  Level1TopoSelectionTool(const std::string& name);
  Level1TopoSelectionTool(const Level1TopoSelectionTool& other);
  Level1TopoSelectionTool& operator=(const Level1TopoSelectionTool& other) = delete;

  virtual ~Level1TopoSelectionTool() {};
  virtual StatusCode initialize();

  // Get the decision for a specific Level1Topo
  virtual StatusCode execute(const xAOD::EmTauRoIContainer * c1,
			     const xAOD::EmTauRoIContainer * c2,
			     const std::string & decor,
			     const std::string & sel1="",
			     const std::string & sel2="");

  // Get the decision for a specific Level1Topo
  virtual StatusCode execute(const xAOD::EmTauRoIContainer * c1,
			     const xAOD::JetRoIContainer * c2,
			     const std::string & decor,
			     const std::string & sel1="",
			     const std::string & sel2="");

  // Get the decision for a specific Level1Topo
  virtual StatusCode execute(const xAOD::MuonRoIContainer * c1,
			     const xAOD::EmTauRoIContainer * c2,
			     const std::string & decor,
			     const std::string & sel1="",
			     const std::string & sel2="");

  // Get the decision for a specific Level1Topo
  virtual StatusCode execute(const xAOD::EmTauRoIContainer * c1,
			     const xAOD::EnergySumRoI * met,
			     const std::string & decor,
			     const std::string & sel1="",
			     const std::string & sel2="");

  // Get the decision for a specific Level1Topo
  virtual StatusCode execute(const xAOD::EmTauRoIContainer * c1,
			     const xAOD::EmTauRoIContainer * c2,
			     const xAOD::JetRoIContainer * c3,
			     const std::string & decor,
			     const std::string & sel1="",
			     const std::string & sel2="",
			     const std::string & sel3="");

  // Get the decision for a specific Level1Topo
  virtual StatusCode execute(const xAOD::MuonRoIContainer * c1,
			     const xAOD::EmTauRoIContainer * c2,
			     const xAOD::JetRoIContainer * c3,
			     const std::string & decor,
			     const std::string & sel1="",
			     const std::string & sel2="",
			     const std::string & sel3="");


  virtual bool decision(const std::string & item);

 private:
  
  double m_delta_r;
  double m_delta_phi;
  bool m_use_box;
  double m_box_delta_phi;
  double m_box_delta_eta;

  std::map<std::string, bool> m_topo_decisions;

  bool topological_criteria(const double eta1, const double phi1, const double eta2, const double phi2, const bool use_box=false);


};
#endif

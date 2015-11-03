/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HIEVENTSHAPEJETITERATION_H__
#define __HIEVENTSHAPEJETITERATION_H__

#include "AsgTools/AsgTool.h"
#include <string>
#include "JetInterface/IJetExecuteTool.h"
#include "AsgTools/ToolHandle.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "HIJetRec/IHISubtractorTool.h"
#include "HIJetRec/IHIUEModulatorTool.h"


class HIEventShapeJetIteration : virtual public IJetExecuteTool,
  virtual public asg::AsgTool
{
  ASG_TOOL_CLASS(HIEventShapeJetIteration, IJetExecuteTool);

public:

  HIEventShapeJetIteration(std::string name);
  
  int execute() const;
  
private:
  StatusCode makeClusterList(std::vector<const xAOD::CaloCluster*>& particleList, const xAOD::JetContainer* theJets,
						       std::set<unsigned int>& used_indices, std::set<unsigned int>& used_eta_bins) const;


  ToolHandle<IHISubtractorTool> m_subtractor_tool;
  ToolHandle<IHIUEModulatorTool> m_modulator_tool;

  /// \brief Name of input HIEventShapeContainer
  std::string m_input_event_shape_key;

  /// \brief Name of output HIEventShapeContainer
  std::string m_output_event_shape_key;

  /// \brief List of names of JetCollections, all jets in these collections are seeds
  std::string m_calo_jet_seed_key;
  std::string m_track_jet_seed_key;

  /// \brief Name of jet attribute providing link between jets and clusters
  std::string m_association_key;

  /// \brief All clusters w/in this DR of jet are excluded from shape calc.
  float m_exclude_DR;

  /// \brief If selected, the jet constituents define the associated clusters
  bool m_exclude_constituents;

  mutable bool m_isInit;

  bool m_do_remodulation;
  unsigned int m_modulation_scheme;
  std::string m_modulation_key;
  
};

#endif
  

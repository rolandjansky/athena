/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HIEVENTSHAPEJETITERATION_H__
#define __HIEVENTSHAPEJETITERATION_H__

#include "AsgTools/AsgTool.h"
#include <string>
#include <vector>
#include <set>
#include "JetInterface/IJetExecuteTool.h"
#include "HIJetRec/IHISubtractorTool.h"
#include "AsgTools/ToolHandle.h"
#include "xAODHIEvent/HIEventShapeContainer.h"


class HIEventShapeJetIteration : virtual public IJetExecuteTool,
  virtual public asg::AsgTool
{
  ASG_TOOL_CLASS(HIEventShapeJetIteration, IJetExecuteTool);

public:

  HIEventShapeJetIteration(std::string name);

  int execute() const;

private:
  ToolHandle<IHISubtractorTool> m_subtractor_tool;

  /// \brief Name of input HIEventShapeContainer
  std::string m_input_event_shape_key;

  /// \brief Name of output HIEventShapeContainer
  std::string m_output_event_shape_key;

  /// \brief List of names of JetCollections, all jets in these collections are seeds
  std::string m_seed_key;

  /// \brief Name of jet attribute providing link between jets and clusters
  std::string m_association_key;

  /// \brief All clusters w/in this DR of jet are excluded from shape calc.
  float m_exclude_DR;

  /// \brief If selected, the jet constituents define the associated clusters
  bool m_exclude_constituents;

  mutable bool m_isInit;

};

#endif
  

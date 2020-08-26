/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     TrigHIEventShapeJetIteration.h
PACKAGE:  Trigger/TrigAlgorithms/TrigHIRec

AUTHOR:  martin.spousta@cern.ch
         This is an HLT analogue of Reconstruction/HeavyIonRec/HIJetRec/HIEventShapeJetIteration.h

********************************************************************/

#ifndef TRIGHIREC_TRIGHIEVENTSHAPEJETITERATION_H
#define TRIGHIREC_TRIGHIEVENTSHAPEJETITERATION_H

#include "AsgTools/AsgTool.h"
#include <string>
#include <vector>
#include <set>
#include "JetInterface/IJetExecuteTool.h"
#include "HIJetRec/IHISubtractorTool.h"
#include "HIEventUtils/HIEventShapeMapTool.h"
#include "AsgTools/ToolHandle.h"
#include "xAODHIEvent/HIEventShapeContainer.h"

#include "HIJetRec/IHISubtractorTool.h"
#include "HIJetRec/IHIUEModulatorTool.h"

class TrigHIEventShapeJetIteration : virtual public IJetExecuteTool,
  virtual public asg::AsgTool
{
  ASG_TOOL_CLASS(TrigHIEventShapeJetIteration, IJetExecuteTool);

public:

  TrigHIEventShapeJetIteration(std::string name);

  int execute() const;

private:
  ToolHandle<IHISubtractorTool> m_subtractor_tool;
  ToolHandle<IHIUEModulatorTool> m_modulator_tool;
  ToolHandle<IHIEventShapeMapTool> m_eventShapeMapTool { this, "EventShapeMapTool", "HIEventShapeMapTool", "Handle to Event Shape Map Tool"};

  /// \brief Name of input cluster container
  std::string m_input_clusters_key;

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

  std::string m_modulation_key;
};

#endif

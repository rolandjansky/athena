/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     TrigHIEventShapeJetIteration.h
PACKAGE:  Trigger/TrigAlgorithms/TrigHIRec

AUTHOR:  martin.spousta@cern.ch
         This is an HLT analogue of Reconstruction/HeavyIonRec/HIJetRec/HIEventShapeJetIteration.h

********************************************************************/

#ifndef __TrigHIEventShapeJetIteration_H__
#define __TrigHIEventShapeJetIteration_H__

#include "AsgTools/AsgTool.h"
#include <string>
#include <vector>
#include <set>
#include "JetInterface/IJetExecuteTool.h"
#include "HIJetRec/IHISubtractorTool.h"
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
  
  //Need to be compatible with other jetrecs collections (JetRec_jobOptions.py) to run the trigger in the RAW->ESD step
  bool m_trigger;
  
  //OutputContainer is a string, here used to identify the object in TrigHLTJetRecConfig
  //need for situation when HI and pp jets are in the same menu and added to jtm.trigjetrec 
  std::string m_outcoll;

  mutable bool m_isInit;

  bool m_do_remodulation;
  unsigned int m_modulation_scheme;
  std::string m_modulation_key;
};

#endif
  

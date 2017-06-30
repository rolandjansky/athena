// for editors : this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOSTEDJETSTAGGERS_JSSTAGGERBase_H_
#define BOOSTEDJETSTAGGERS_JSSTAGGERBase_H_


#include "AsgTools/AsgTool.h"
#include "JetAnalysisInterfaces/IJetSelectorTool.h"
#include "JetInterface/IJetSelector.h"

#include "PATCore/TAccept.h"

#include <stdio.h>
#include <stdlib.h>

class JSSTaggerBase :   public asg::AsgTool ,
                        virtual public IJetSelectorTool,
                        virtual public IJetSelector {
  ASG_TOOL_CLASS2(JSSTaggerBase, IJetSelectorTool, IJetSelector )

  public:

  // the object where you store the contents of what the jet has passed
  mutable Root::TAccept m_accept;

  // the location where CVMFS files live
  std::string m_calibarea;

  // whether you will decorate information onto the jet
  bool m_decorate;

  // the kinematic bounds for the jet - these are in MeV (not GeV!)
  float m_jetPtMin;
  float m_jetPtMax;
  float m_jetEtaMax;

  // default constructor - to be used in all derived classes
  JSSTaggerBase(const std::string &name);

  // provide a default implementation for IJetSelector::keep :
  virtual int keep(const xAOD::Jet& jet) const {
    return tag(jet);
  }

  // show the user what cuts are configured
  void showCuts() const;

};

#endif

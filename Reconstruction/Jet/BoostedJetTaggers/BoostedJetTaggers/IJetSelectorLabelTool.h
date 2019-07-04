// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
  
  This is an interface class for JSS taggers, for those who want to decorate
  a truth label on their jets for the JES uncertainties depending on the label,
  but do not actually tag their jets.
*/

#ifndef IJETSELECTORLABELTOOL_H
#define IJETSELECTORLABELTOOL_H

#include "PATInterfaces/CorrectionCode.h"
#include "JetAnalysisInterfaces/IJetSelectorTool.h"
#include "xAODJet/JetContainer.h"

class IJetSelectorLabelTool : public virtual IJetSelectorTool {

  ASG_TOOL_INTERFACE( CP::IJetSelectorLabelTool )

  public:

  virtual ~IJetSelectorLabelTool() {}
  
  virtual Root::TAccept tag(const xAOD::Jet& jet) const = 0;
  
  virtual StatusCode decorateTruthLabel(const xAOD::Jet& jet) const = 0;

};

#endif

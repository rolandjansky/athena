// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// Interfaces for JSS taggers tool (e.g. SmoothedWZTagger, JSSWTopTaggerDNN)
/// 
/// If you call:
///    tag(const xAOD::Jet& jet) 
/// function, both a truth label and tagging result are decorated to the 
/// given jet. If it is not a desired  use case, e.g. a truth label is only 
/// needed for the JES uncertainties depending on the label, the separate 
/// interface:
///    decorateTruthLabel(const xAOD::Jet& jet)
/// can be used just to attach the truth label to the jet, which doesn't 
/// allow tagging it.
/// 
/// @author Takuya Nobe <Takuya.Nobe@cern.ch>
/// 
#ifndef IJETSELECTORLABELTOOL_H
#define IJETSELECTORLABELTOOL_H

#include "PATInterfaces/CorrectionCode.h"
#include "JetAnalysisInterfaces/IJetSelectorTool.h"
#include "xAODJet/JetContainer.h"

class IJetSelectorLabelTool : public virtual IJetSelectorTool {

  ASG_TOOL_INTERFACE( CP::IJetSelectorLabelTool )

  public:

  virtual ~IJetSelectorLabelTool() {}
  
  /**
   * @fn
   * attach a truth label and the tagging result to the given jet
   */
  virtual Root::TAccept tag(const xAOD::Jet& jet) const = 0;
  
  /**
   * @fn
   * attach a truth label only
   */
  virtual StatusCode decorateTruthLabel(const xAOD::Jet& jet) const = 0;

};

#endif

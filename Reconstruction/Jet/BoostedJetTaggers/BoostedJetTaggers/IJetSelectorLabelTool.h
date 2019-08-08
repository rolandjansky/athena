// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IJETSELECTORLABELTOOL_H
#define IJETSELECTORLABELTOOL_H

#include "PATInterfaces/CorrectionCode.h"
#include "JetAnalysisInterfaces/IJetSelectorTool.h"
#include "xAODJet/JetContainer.h"

/// Interfaces for JSS taggers tool (e.g. SmoothedWZTagger, JSSWTopTaggerDNN)
/// 
/// If you call:
///    tag(const xAOD::Jet& jet) 
/// function, a truth label is decorated to the given jet and tagging result
/// is calculated. If it is not a desired  use case, e.g. a truth label is only 
/// needed for the JES uncertainties depending on the label, the separate 
/// interface:
///    decorateTruthLabel(const xAOD::Jet& jet)
/// can be used just to attach the truth label to the jet, which doesn't 
/// allow tagging it.
/// 
/// @author Takuya Nobe <Takuya.Nobe@cern.ch>
/// 
class IJetSelectorLabelTool : public virtual IJetSelectorTool {

  ASG_TOOL_INTERFACE( CP::IJetSelectorLabelTool )

  public:

  virtual ~IJetSelectorLabelTool() {}
  
  /**
   * @fn
   * attach a truth label to the given jet and return the tagging result
   */
  virtual Root::TAccept& tag(const xAOD::Jet& jet) const = 0;
  
  /**
   * @fn
   * attach a truth label only
   */
  virtual StatusCode decorateTruthLabel(const xAOD::Jet& jet) const = 0;

};

#endif

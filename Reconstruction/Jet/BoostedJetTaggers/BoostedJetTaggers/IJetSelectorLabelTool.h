// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
/// function, the tagging result is calculated. The functionality of applying 
/// truth labels has been moved to ParticleJetTools/JetTruthLabelingTool
/// 
/// @author Takuya Nobe <Takuya.Nobe@cern.ch>
/// Updated by Jason Veatch <Jason.Veatch@cern.ch> February 2020
/// 
class IJetSelectorLabelTool : public virtual IJetSelectorTool {

  ASG_TOOL_INTERFACE( CP::IJetSelectorLabelTool )

  public:

  virtual ~IJetSelectorLabelTool() {}
  
  /**
   * @fn
   * return the tagging result
   */
  virtual Root::TAccept& tag(const xAOD::Jet& jet) const = 0;
  
};

#endif

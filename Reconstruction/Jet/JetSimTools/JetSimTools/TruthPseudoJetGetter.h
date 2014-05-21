// emacs this file is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETSIMTOOLS_TRUTHPSEUDOJETGETTER_H
#define JETSIMTOOLS_TRUTHPSEUDOJETGETTER_H

#include "GaudiKernel/ToolHandle.h"

#include "JetRec/PseudoJetGetter.h"

#include "JetSimTools/JetTruthParticleSelectorTool.h"

class TruthPseudoJetGetter : public PseudoJetGetter {
  ASG_TOOL_CLASS(TruthPseudoJetGetter, IPseudoJetGetter)
public:
  TruthPseudoJetGetter(const std::string &name);
  

  virtual StatusCode initialize();

  virtual int appendTo(PseudoJetVector& psjs, const LabelIndex* pli) const;

protected:
  

  ToolHandle<JetTruthParticleSelectorTool> m_selector;
};

#endif

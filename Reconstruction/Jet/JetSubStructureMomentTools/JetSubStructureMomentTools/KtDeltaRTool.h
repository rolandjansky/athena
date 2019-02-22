/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETSUBSTRUCTURE_KTDELTARTOOL_H
#define JETSUBSTRUCTURE_KTDELTARTOOL_H

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

class KtDeltaRTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(KtDeltaRTool, IJetModifier)

  public:
    // Ctor.
    KtDeltaRTool(const std::string& t);

    // Add moment to a jet.
    int modifyJet(xAOD::Jet& injet) const;

    // Display properties.
    void print() const;

  private:
    double m_jetrad;

};

#endif

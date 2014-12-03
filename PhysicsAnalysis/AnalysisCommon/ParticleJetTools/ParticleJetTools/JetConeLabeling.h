/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCONELABELING_H
#define JETCONELABELING_H

#include "JetRec/JetModifierBase.h"

#include "AsgTools/ToolHandle.h"
#include "xAODJet/Jet.h"

#include <string>

namespace Analysis
{
  class IJetTruthMatching;

  class JetConeLabeling : public JetModifierBase
  {
  ASG_TOOL_CLASS(JetConeLabeling, IJetModifier)
  public:
    /** Constructors and destructors */
    JetConeLabeling(const std::string& name); 
    virtual ~JetConeLabeling();

    StatusCode initialize();
    virtual int modifyJet(xAOD::Jet& jet) const;

  private:

    ToolHandle< IJetTruthMatching > m_jetTruthMatchTool;        //!< pointer to the Truth matching tool

  }; // End class

} // End namespace

#endif // JETCONELABELING_H

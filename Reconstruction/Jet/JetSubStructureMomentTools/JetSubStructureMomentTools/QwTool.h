// This file is -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETSUBSTRUCTURE_QWTOOL_H
#define JETSUBSTRUCTURE_QWTOOL_H
////////////////////////////////////////////
///
/// \class QwTool
/// \brief Dual-use tool wrapper to set the Qw substructure variable.
/// \author P.A. Delsart
/// \date May 2015
///
/// See JetSubStructureUtils package for the implementation.
//////////////////////////////////////////////////////////
#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"

class QwTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(QwTool, IJetModifier)

  public:
    // Ctor.
    QwTool(const std::string& t);

    // Add moment to a jet.
    int modifyJet(xAOD::Jet& injet) const;

};

#endif

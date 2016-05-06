/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetVoronoiMomentsTool.h

#ifndef JETMOMENTTOOLS_JETVORONOIMOMENTSTOOL_H
#define JETMOMENTTOOLS_JETVORONOIMOMENTSTOOL_H

/// Jakub Cuth \n
/// May 2015
///
/// Tool to add jet moments based on voronoi diagram.
///
/// Moments to add:
///     VoronoiArea            Area of voronoi cell corresponding to jet in eta-phi plane
///

#include <string>
#include "JetRec/JetModifierBase.h"
#include "xAODJet/Jet.h" 

namespace JetVoronoiDiagramHelpers{
  struct Diagram;
}

class JetVoronoiMomentsTool : public JetModifierBase {
ASG_TOOL_CLASS(JetVoronoiMomentsTool,IJetModifier)

public:

  // Constructor from tool name
  JetVoronoiMomentsTool(const std::string& name);

  // Inherited methods to modify a jet container
  virtual int modify(xAOD::JetContainer& jets) const;
  virtual int modifyJet(xAOD::Jet& jet) const;

private:

  // parameters
  float m_x_min;
  float m_x_max;
  float m_y_min;
  float m_y_max;

  int modifyJet(xAOD::Jet& jet, const JetVoronoiDiagramHelpers::Diagram & voro) const;

};

#endif


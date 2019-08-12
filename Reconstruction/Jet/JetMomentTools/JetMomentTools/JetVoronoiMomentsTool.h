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
#include "AsgTools/AsgTool.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "JetInterface/IJetDecorator.h"

namespace JetVoronoiDiagramHelpers{
  struct Diagram;
}

class JetVoronoiMomentsTool : public asg::AsgTool,
                              virtual public IJetDecorator {
ASG_TOOL_CLASS(JetVoronoiMomentsTool,IJetDecorator)

public:

  // Constructor from tool name
  JetVoronoiMomentsTool(const std::string& name);

  // Inherited from AsgTool via IJetDecorator
  virtual StatusCode initialize();

  // Inherited method to decorate a jet container
  virtual StatusCode decorate(const xAOD::JetContainer& jets) const;

private:

  // parameters
  float m_x_min;
  float m_x_max;
  float m_y_min;
  float m_y_max;

  std::string m_jetContainerName;
  std::string m_decorationName;
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_voronoiAreaKey;

};

#endif


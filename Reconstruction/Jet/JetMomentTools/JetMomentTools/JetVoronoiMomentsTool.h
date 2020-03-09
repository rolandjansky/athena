/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  virtual StatusCode initialize() override;

  // Inherited method to decorate a jet container
  virtual StatusCode decorate(const xAOD::JetContainer& jets) const override;

private:

  // parameters
  Gaudi::Property<float> m_x_min{this, "AreaXmin", -10,
      "Minimum x-value for Voronoi diagram"};
  Gaudi::Property<float> m_x_max{this, "AreaXmax", 10,
      "Maximum x-value for Voronoi diagram"};
  Gaudi::Property<float> m_y_min{this, "AreaYmin", -4,
      "Minimum y-value for Voronoi diagram"};
  Gaudi::Property<float> m_y_max{this, "AreaYmax", 4,
      "Maximum y-value for Voronoi diagram"};
  Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "",
      "SG key for the input jet container"};

  SG::WriteDecorHandleKey<xAOD::JetContainer> m_voronoiAreaKey{this, "VoronoiAreaKey", "VoronoiArea",
      "SG key for Voronoi area decoration (not including jet container name)"};
};

#endif


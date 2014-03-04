/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SURFACETOSONODE_H
#define SURFACETOSONODE_H

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class SurfaceToSoNode                              //
//                                                                     //
//  Author: Andreas Salzburger <Andreas.Salzburger@cern.ch> (primary)  //
//          Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial VP1 version: June 2007                                     //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

class SoNode;

namespace Trk {
  class Surface;
  class PlaneSurface;
  class CylinderSurface;
  class DiscSurface;
  class StraightLineSurface;
  class PerigeeSurface;
}

class SurfaceToSoNode{

public:
  // Public type definitions:

  // Default constructor
  SurfaceToSoNode();

  // Destructor
  virtual ~SurfaceToSoNode();

  // the general one, has to do a dynamic cast
  SoNode*   translateSurface(const Trk::Surface& sf, const bool& simple=false) const;

  // translate a PlaneSurface
  SoNode*    translatePlaneSurface(const Trk::PlaneSurface& psf ) const;
  // translate a CylinderSurface
  SoNode*    translateCylinderSurface(const Trk::CylinderSurface& csf ) const;
  // translate a DiscSurface
  SoNode*    translateDiscSurface(const Trk::DiscSurface& dsf) const;
  // translate a StraightLineSurface
  SoNode*    translateStraightLineSurface(const Trk::StraightLineSurface& slsf, const bool& simple=false) const;
  // translate a PerigeeSurface
  SoNode*    translatePerigeeSurface(const Trk::PerigeeSurface& persf) const;

  static double surfaceThickness;
};

#endif



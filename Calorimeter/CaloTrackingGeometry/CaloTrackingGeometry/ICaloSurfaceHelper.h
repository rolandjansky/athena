/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
// -----------------------------------------
//
//
// ***************************************************************************

#ifndef CALOTRACKINGGEOMETRY_ICALOSURFACEHELPER_H
#define CALOTRACKINGGEOMETRY_ICALOSURFACEHELPER_H

#include "GaudiKernel/IAlgTool.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloDetDescr/CaloSubdetNames.h"

namespace Trk {
  class Surface;
}

static const InterfaceID IID_ICaloSurfaceHelper("ICaloSurfaceHelper", 1, 0);

class ICaloSurfaceHelper : virtual public IAlgTool {

 public:
  /**Virtual destructor*/
  virtual ~ICaloSurfaceHelper(){}
      
  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_ICaloSurfaceHelper; } 

  /** entrance of the sample, no eta dependence -> flat approximation */
  virtual const Trk::Surface& GetFlatEntrySurface (const CaloCell_ID::CaloSample sample, int side=0) = 0;

  /** Layer entry */
  virtual const Trk::Surface& GetEntrySurface (const CaloCell_ID::CaloSample sample, int side=0) = 0;

  /** Layer exit */
  virtual const Trk::Surface& GetExitSurface (const CaloCell_ID::CaloSample sample, int side=0) = 0;

#if 0
  /** MBTS surfaces */
  virtual const Trk::Surface& GetMBTSSurface(int side=0) = 0;
  virtual void LoadMBTSSurfaces(std::pair<const Trk::Surface*,const Trk::Surface*>) = 0;
#endif

  /** Girder exit */
  //virtual const Trk::Surface& GetGirderExitSurface() = 0;
};

 
#endif // CALOTRACKINGGEOMETRY_ICALOSURFACEHELPER_H







/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISegmentation.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASDETDESCRINTERFACES_ISEGMENTATION_H
#define ISF_FATRASDETDESCRINTERFACES_ISEGMENTATION_H

// Math & Geometry library
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include "ReadoutGeometryBase/SiCellId.h"

namespace iFatras {

  /**
     @class ISegmentation
     Interface of the segmentation class.
     
     @author Noemi Calace
     
  */

  class ISegmentation {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
    
  public:

    /// Constructor:
    ISegmentation();

    /// Destructor:
    ~ISegmentation();

    virtual bool cellOfPosition(const Amg::Vector2D &localPos, std::pair<int, int>& XY) const = 0;
    virtual Amg::Vector2D localPositionOfCell(const InDetDD::SiCellId &cellId) const = 0;

    virtual double pitchX() const = 0;
    virtual double pitchY() const = 0;
    virtual double phiPitch() const = 0;
    virtual double phiPitch(const Amg::Vector2D &) const = 0;
    virtual double stripLength(const Amg::Vector2D &) const = 0;
    virtual double sinStereoLocal(const Amg::Vector2D &localPos) const = 0;
    
  };
  
  inline ISegmentation::ISegmentation() {}

  inline ISegmentation::~ISegmentation() {}

}

#endif // ISF_FATRASDETDESCRINTERFACES_ISEGMENTATION_H

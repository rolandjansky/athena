/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SubtractedCylinderSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRYSURFACES_SUBTRACTEDCYLINDERSURFACE_H
#define TRKGEOMETRYSURFACES_SUBTRACTEDCYLINDERSURFACE_H

// Trk
#include "TrkDetDescrUtils/AreaExcluder.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkSurfaces/CylinderSurface.h"
// Geometry & Math
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;
class Identifier;

namespace Trk {

/**
 @class SubtractedCylinderSurface
 Class for a cylinder subtracted/shared surface in the ATLAS detector.
 It owns its surface bounds and subtracted volume.

 @author Sarka.Todorova@cern.ch
 */

class SubtractedCylinderSurface : public CylinderSurface
{
public:
  /** Default Constructor - needed for persistency*/
  SubtractedCylinderSurface();

  /** Copy Constructor*/
  SubtractedCylinderSurface(const SubtractedCylinderSurface& psf);

  /** Copy Constructor with shift*/
  SubtractedCylinderSurface(const SubtractedCylinderSurface& psf, const Amg::Transform3D& transf);

  /** Constructor */
  SubtractedCylinderSurface(const CylinderSurface& cs, AreaExcluder* vol, bool shared);

  /**Destructor*/
  virtual ~SubtractedCylinderSurface();

  /**Assignment operator*/
  SubtractedCylinderSurface& operator=(const SubtractedCylinderSurface& psf);

  /**Equality operator*/
  virtual bool operator==(const Surface& sf) const override final;

  /** This method indicates the subtraction mode */
  bool shared() const;

  /**This method calls the inside() method of the Bounds*/
  virtual bool insideBounds(const Amg::Vector2D& locpos,
                            double tol1 = 0.,
                            double tol2 = 0.) const override final;

  /**This method allows access to the subtracted part*/
  SharedObject<AreaExcluder> subtractedVolume() const;

  /** Return properly formatted class name for screen output */
  virtual std::string name() const override final
  {
    return "Trk::SubtractedCylinderSurface";
  }

protected:
  SharedObject<AreaExcluder> m_subtrVol;
  bool m_shared;
};
} // end of namespace

#include "TrkGeometrySurfaces/SubtractedCylinderSurface.icc"
#endif // TRKGEOMETRYSURFACES_SUBTRACTEDCYLINDERSURFACE_H

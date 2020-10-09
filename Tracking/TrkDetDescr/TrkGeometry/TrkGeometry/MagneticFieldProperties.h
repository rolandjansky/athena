/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MagneticFieldProperties.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_MAGNETICFIELDPROPERTIES_H
#define TRKGEOMETRY_MAGNETICFIELDPROPERTIES_H

// Gaudi
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/MsgStream.h"
// Trk
#include "TrkGeometry/MagneticFieldMode.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// STD
#include <iostream>

namespace Trk {

/**
 @class MagneticFieldProperties
  magnetic field properties to steer the behavior of the extrapolation
 @author Andreas.Salzburger@cern.ch
 @author (Athena MT) Christos Anastopoulos
 */
class MagneticFieldProperties
{

public:
  /**Constructor for magnetic field mode - full field is default */
  MagneticFieldProperties(MagneticFieldMode mode = Trk::FullField);
  /**Constructor for magnetic field mode */
  MagneticFieldProperties(const Amg::Vector3D& field);
  // default copy/move/dtor
  MagneticFieldProperties(const MagneticFieldProperties&) = default;
  MagneticFieldProperties(MagneticFieldProperties&&) = default;
  MagneticFieldProperties& operator=(const MagneticFieldProperties&) = default;
  MagneticFieldProperties& operator=(MagneticFieldProperties&&) = default;
  ~MagneticFieldProperties() = default;

  /**Cast operator*/
  operator MagneticFieldMode() const;

  /**Returns the MagneticFieldMode as specified */
  MagneticFieldMode magneticFieldMode() const;

  /** Get the magnetic field - in case of constant field only - throws exception
   * if mode is not constant */
  const Amg::Vector3D& magneticField() const;

protected:
  MagneticFieldMode m_magneticFieldMode;
  Amg::Vector3D m_magneticField;
};

/**Overload of << operator for both, MsgStream and std::ostream for debug
 * output*/
MsgStream&
operator<<(MsgStream& sl, const MagneticFieldProperties& mprop);

std::ostream&
operator<<(std::ostream& sl, const MagneticFieldProperties& mprop);

} // end of namespace
#include "TrkGeometry/MagneticFieldProperties.icc"
#endif // TRKGEOMETRY_MAGNETICFIELDPROPERTIES_H


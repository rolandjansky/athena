/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MagneticFieldMode.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef TRKGEOMETRY_MAGNETICFIELDMODE_H
#define TRKGEOMETRY_MAGNETICFIELDMODE_H

namespace Trk {

/** @enum MagneticFieldMode
    MagneticFieldMode describing the field setup within a volume
   @author Andreas.Salzburger@cern.ch
  */
enum MagneticFieldMode
{
  NoField = 0,       //!< Field is set to 0., 0., 0.,
  ConstantField = 1, //!< Field is set to be constant
  FastField = 2,     //!< call the fast field access method of the FieldSvc
  FullField = 3 //!< Field is set to be realistic, but within a given Volume
};

} // end of namespace
#endif // TRKGEOMETRY_MAGNETICFIELDMODE_H

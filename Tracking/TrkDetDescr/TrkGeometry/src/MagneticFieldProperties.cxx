/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MagneticFieldProperties.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometry/MagneticFieldProperties.h"

/**Overload of << operator for both, MsgStream and std::ostream for debug
 * output*/
MsgStream&
Trk::operator<<(MsgStream& sl, const Trk::MagneticFieldProperties& mprop)
{
  sl << "Trk::MagneticFieldProperties, configuration: "
     << mprop.magneticFieldMode() << endmsg;
  return sl;
}

std::ostream&
Trk::operator<<(std::ostream& sl, const Trk::MagneticFieldProperties& mprop)
{
  sl << "Trk::MagneticFieldProperties, configuration: "
     << mprop.magneticFieldMode() << std::endl;
  return sl;
}

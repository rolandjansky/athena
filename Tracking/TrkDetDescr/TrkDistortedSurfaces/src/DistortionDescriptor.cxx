/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DistortionDescriptor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

//Trk
#include "TrkDistortedSurfaces/DistortionDescriptor.h"

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& Trk::operator << ( MsgStream& sl, const DistortionDescriptor& dd)
{ return dd.dump(sl); }

std::ostream& Trk::operator << ( std::ostream& sl, const DistortionDescriptor& dd)
{ return dd.dump(sl); }    



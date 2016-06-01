/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SurfaceBounds.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////


//Trk
#include "TrkSurfaces/SurfaceBounds.h"

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& Trk::operator << ( MsgStream& sl, const SurfaceBounds& sb)
{ return sb.dump(sl); }

std::ostream& Trk::operator << ( std::ostream& sl, const SurfaceBounds& sb)
{ return sb.dump(sl); }    

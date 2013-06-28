/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VolumeBounds.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


//Trk
#include "TrkVolumes/VolumeBounds.h"

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& Trk::operator << ( MsgStream& sl, const Trk::VolumeBounds& vb)
{ return vb.dump(sl); }

std::ostream& Trk::operator << ( std::ostream& sl, const Trk::VolumeBounds& vb)
{ return vb.dump(sl); }    

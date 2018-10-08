/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LocalDirection.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkEventPrimitives/LocalDirection.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"


/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& Trk::operator << ( MsgStream& ls, const Trk::LocalDirection& lom)
{ 
    ls << std::setiosflags(std::ios::fixed) << std::setprecision(3);
//    ls << "(angleXZ, angleYZ, magnitude) = ";
    ls << "(angleXZ, angleYZ) = ";
    ls << "(" <<  lom.angleXZ();
    ls << "," <<  lom.angleYZ() << ")";
    ls << std::setprecision(-1);
    return ls;
}

std::ostream& Trk::operator << ( std::ostream& ls, const Trk::LocalDirection& lom)
{ 
    ls << std::setiosflags(std::ios::fixed) << std::setprecision(3);
//    ls << "(angleXZ, angleYZ, magnitude) = ";
    ls << "(angleXZ, angleYZ) = ";
    ls << "(" <<  lom.angleXZ();
    ls << "," <<  lom.angleYZ() << ")";
    ls << std::setprecision(-1);
    return ls;
}




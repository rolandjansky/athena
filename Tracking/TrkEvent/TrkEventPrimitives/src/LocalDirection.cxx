/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LocalDirection.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkEventPrimitives/LocalDirection.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"

Trk::LocalDirection::LocalDirection() :
 m_angleXZ(0.),
 m_angleYZ(0.)
{}

Trk::LocalDirection::LocalDirection(double axz, double ayz) :
 m_angleXZ(axz),
 m_angleYZ(ayz)
{}

Trk::LocalDirection::LocalDirection(const LocalDirection& locdir) :
 m_angleXZ(locdir.m_angleXZ),
 m_angleYZ(locdir.m_angleYZ)
{}


Trk::LocalDirection::~LocalDirection()
{}

Trk::LocalDirection& Trk::LocalDirection::operator=(const Trk::LocalDirection& rhs)
{
    if (this != &rhs){
        m_angleXZ   = rhs.m_angleXZ;
        m_angleYZ   = rhs.m_angleYZ;
    }
    return(*this);
}

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




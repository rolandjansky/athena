/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ObjectAccessor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkDetDescrUtils/ObjectAccessor.h"

Trk::ObjectAccessor::ObjectAccessor(const Trk::ObjectAccessor& oa) :
 m_inverseCase(oa.m_inverseCase)
{}

Trk::ObjectAccessor& Trk::ObjectAccessor::operator=(const Trk::ObjectAccessor& oa)
{
  if (this != &oa){
    m_inverseCase = oa.m_inverseCase;
  }
  return(*this);

}

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& Trk::operator << ( MsgStream& sl, const Trk::ObjectAccessor& oac)
{ 
  return oac.dump(sl);
}

std::ostream& Trk::operator << ( std::ostream& sl, const Trk::ObjectAccessor& oac)
{ 
  return oac.dump(sl);
} 

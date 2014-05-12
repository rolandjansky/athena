/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ThreeObjectsAccessor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkDetDescrUtils/ThreeObjectsAccessor.h"

/** Default constructor*/
Trk::ThreeObjectsAccessor::ThreeObjectsAccessor():
 Trk::ObjectAccessor(),
 m_jumper(0),
 m_break(false)
{
  m_accessors[0] = 0;
  m_accessors[1] = 0;
  m_accessors[2] = 0;
}

/**Constructor of vector*/
Trk::ThreeObjectsAccessor::ThreeObjectsAccessor(int first,
                                                int second,
                                                int third) :
 Trk::ObjectAccessor(),
 m_jumper(0),
 m_break(false)

{
  m_accessors[0] = first;
  m_accessors[1] = second;
  m_accessors[2] = third;
}

/**Copy constructor*/
Trk::ThreeObjectsAccessor::ThreeObjectsAccessor(const Trk::ThreeObjectsAccessor& accessor) :
 Trk::ObjectAccessor(accessor),
 m_jumper(accessor.m_jumper),
 m_break(accessor.m_break)

{
    m_accessors[0] = accessor.m_accessors[0];
    m_accessors[1] = accessor.m_accessors[1];
    m_accessors[2] = accessor.m_accessors[2];
}


Trk::ThreeObjectsAccessor& Trk::ThreeObjectsAccessor::operator=(const Trk::ThreeObjectsAccessor& rhs)
{
   if (this!=&rhs){
      Trk::ObjectAccessor::operator=(rhs);
      m_jumper       = rhs.m_jumper;
      m_accessors[0] = rhs.m_accessors[0];
      m_accessors[1] = rhs.m_accessors[1];
      m_accessors[2] = rhs.m_accessors[2];
      m_break        = rhs.m_break;
    }
   return (*this);
}

MsgStream& Trk::ThreeObjectsAccessor::dump(MsgStream& sl) const
{
    sl << "ThreeObjectAccessor: | ";
     for (int iac=0; iac<3; ++iac) sl << m_accessors[iac] << " | ";
    return sl;
}

std::ostream& Trk::ThreeObjectsAccessor::dump(std::ostream& sl) const
{
    sl << "ThreeObjectAccessor: | ";
     for (int iac=0; iac<3; ++iac) sl << m_accessors[iac] << " | ";
    return sl;
}



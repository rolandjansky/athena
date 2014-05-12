/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FiveObjectsAccessor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkDetDescrUtils/FiveObjectsAccessor.h"

/** Default constructor*/
Trk::FiveObjectsAccessor::FiveObjectsAccessor():
 Trk::ObjectAccessor(),
 m_jumper(0),
 m_break(false)
{
  m_accessors[0] = 0;
  m_accessors[1] = 0;
  m_accessors[2] = 0;
  m_accessors[3] = 0;
  m_accessors[4] = 0;
}

/**Constructor of vector*/
Trk::FiveObjectsAccessor::FiveObjectsAccessor(int first,
                                              int second,
                                              int third,
                                              int fourth,
                                              int fifth) :
  Trk::ObjectAccessor(),
  m_jumper(0),
  m_break(false)
{
  m_accessors[0] = first;
  m_accessors[1] = second;
  m_accessors[2] = third;
  m_accessors[3] = fourth;
  m_accessors[4] = fifth;
}

/**Copy constructor*/
Trk::FiveObjectsAccessor::FiveObjectsAccessor(const Trk::FiveObjectsAccessor& accessor) :
 Trk::ObjectAccessor(accessor),
 m_jumper(accessor.m_jumper),
 m_break(accessor.m_break)
{
  m_accessors[0] = accessor.m_accessors[0];
  m_accessors[1] = accessor.m_accessors[1];
  m_accessors[2] = accessor.m_accessors[2];
  m_accessors[3] = accessor.m_accessors[3];
  m_accessors[4] = accessor.m_accessors[4];
}


Trk::FiveObjectsAccessor& Trk::FiveObjectsAccessor::operator=(const Trk::FiveObjectsAccessor& rhs)
{
     if (this!=&rhs){
         Trk::ObjectAccessor::operator=(rhs);
         m_jumper    = rhs.m_jumper;
         m_accessors[0] = rhs.m_accessors[0];
         m_accessors[1] = rhs.m_accessors[1];
         m_accessors[2] = rhs.m_accessors[2];
         m_accessors[3] = rhs.m_accessors[3];
         m_accessors[4] = rhs.m_accessors[4];
         m_break        = rhs.m_break;
        }
     return (*this);
}

MsgStream& Trk::FiveObjectsAccessor::dump(MsgStream& sl) const
{
    sl << "FiveObjectAccessor: | ";
     for (int iac=0; iac<5; ++iac) sl << m_accessors[iac] << " | ";
    return sl;
}

std::ostream& Trk::FiveObjectsAccessor::dump(std::ostream& sl) const
{
    sl << "FiveObjectAccessor: | ";
     for (int iac=0; iac<5; ++iac) sl << m_accessors[iac] << " | ";
    return sl;
}

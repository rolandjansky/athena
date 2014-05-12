/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EightObjectsAccessor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkDetDescrUtils/EightObjectsAccessor.h"

/** Default constructor*/
Trk::EightObjectsAccessor::EightObjectsAccessor():
 Trk::ObjectAccessor(),
 m_jumper(0)
{
  m_accessors[0] = 0;
  m_accessors[1] = 0;
  m_accessors[2] = 0;
  m_accessors[3] = 0;
  m_accessors[4] = 0;
  m_accessors[5] = 0;
  m_accessors[6] = 0;
  m_accessors[7] = 0;
}

/**Constructor of vector*/
Trk::EightObjectsAccessor::EightObjectsAccessor(int first,
                                            int second,
                                            int third,
                                            int fourth,
                                            int fifth,
                                            int sixth,
                                            int seventh,
                                            int eighth) :
 Trk::ObjectAccessor(),
 m_jumper(0)
{
  m_accessors[0] = first;
  m_accessors[1] = second;
  m_accessors[2] = third;
  m_accessors[3] = fourth;
  m_accessors[4] = fifth;
  m_accessors[5] = sixth;
  m_accessors[6] = seventh;
  m_accessors[7] = eighth;
}

/**Copy constructor*/
Trk::EightObjectsAccessor::EightObjectsAccessor(const Trk::EightObjectsAccessor& accessor) :
 Trk::ObjectAccessor(),
 m_jumper(accessor.m_jumper)
{
  m_accessors[0] = accessor.m_accessors[0];
  m_accessors[1] = accessor.m_accessors[1];
  m_accessors[2] = accessor.m_accessors[2];
  m_accessors[3] = accessor.m_accessors[3];
  m_accessors[4] = accessor.m_accessors[4];
  m_accessors[5] = accessor.m_accessors[5];
  m_accessors[6] = accessor.m_accessors[6];
  m_accessors[7] = accessor.m_accessors[7];
}


Trk::EightObjectsAccessor& Trk::EightObjectsAccessor::operator=(const Trk::EightObjectsAccessor& rhs)
{
     if (this!=&rhs){
         m_jumper       = rhs.m_jumper;
         m_accessors[0] = rhs.m_accessors[0];
         m_accessors[1] = rhs.m_accessors[1];
         m_accessors[2] = rhs.m_accessors[2];
         m_accessors[3] = rhs.m_accessors[3];
         m_accessors[4] = rhs.m_accessors[4];
         m_accessors[5] = rhs.m_accessors[5];
         m_accessors[6] = rhs.m_accessors[6];
         m_accessors[7] = rhs.m_accessors[7];
        }
     return (*this);
}

MsgStream& Trk::EightObjectsAccessor::dump(MsgStream& sl) const
{
    sl << "EightObjectsAccessor: | ";
    for (int iac=0; iac<8; ++iac) sl << m_accessors[iac] << " | ";
    return sl;

}

std::ostream& Trk::EightObjectsAccessor::dump(std::ostream& sl) const
{
    sl << "EightObjectsAccessor: | ";
    for (int iac=0; iac<8; ++iac) sl << m_accessors[iac] << " | ";
    return sl;
}



/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SixObjectsAccessor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkDetDescrUtils/SixObjectsAccessor.h"

/** Default constructor*/
Trk::SixObjectsAccessor::SixObjectsAccessor():
 Trk::ObjectAccessor(),
 m_jumper(0),
 m_break(false)
{
  m_accessors[0] = 0;
  m_accessors[1] = 0;
  m_accessors[2] = 0;
  m_accessors[3] = 0;
  m_accessors[4] = 0;
  m_accessors[5] = 0;
}

/**Constructor of vector*/
Trk::SixObjectsAccessor::SixObjectsAccessor(int first,
                                            int second,
                                            int third,
                                            int fourth,
                                            int fifth,
                                            int sixth) :
 Trk::ObjectAccessor(),
 m_jumper(0),
 m_break(false)
{
  m_accessors[0] = first;
  m_accessors[1] = second;
  m_accessors[2] = third;
  m_accessors[3] = fourth;
  m_accessors[4] = fifth;
  m_accessors[5] = sixth;
}

/**Copy constructor*/
Trk::SixObjectsAccessor::SixObjectsAccessor(const Trk::SixObjectsAccessor& accessor) :
 Trk::ObjectAccessor(accessor),
 m_jumper(accessor.m_jumper),
 m_break(accessor.m_break)
{
  m_accessors[0] = accessor.m_accessors[0];
  m_accessors[1] = accessor.m_accessors[1];
  m_accessors[2] = accessor.m_accessors[2];
  m_accessors[3] = accessor.m_accessors[3];
  m_accessors[4] = accessor.m_accessors[4];
  m_accessors[5] = accessor.m_accessors[5];
}


Trk::SixObjectsAccessor& Trk::SixObjectsAccessor::operator=(const Trk::SixObjectsAccessor& rhs)
{
     if (this!=&rhs){
         Trk::ObjectAccessor::operator=(rhs);
         m_jumper       = rhs.m_jumper;
         m_accessors[0] = rhs.m_accessors[0];
         m_accessors[1] = rhs.m_accessors[1];
         m_accessors[2] = rhs.m_accessors[2];
         m_accessors[3] = rhs.m_accessors[3];
         m_accessors[4] = rhs.m_accessors[4];
         m_accessors[5] = rhs.m_accessors[5];
         m_break        = rhs.m_break;
        }
     return (*this);
}

MsgStream& Trk::SixObjectsAccessor::dump(MsgStream& sl) const
{
    sl << "SixObjectsAccessor: | ";
    for (int iac=0; iac<6; ++iac) sl << m_accessors[iac] << " | ";
    return sl;

}

std::ostream& Trk::SixObjectsAccessor::dump(std::ostream& sl) const
{
    sl << "SixObjectsAccessor: | ";
    for (int iac=0; iac<6; ++iac) sl << m_accessors[iac] << " | ";
    return sl;
}



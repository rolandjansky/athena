/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FourObjectsAccessor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkDetDescrUtils/FourObjectsAccessor.h"

/** Default constructor*/
Trk::FourObjectsAccessor::FourObjectsAccessor():
 Trk::ObjectAccessor(),
 m_jumper(0),
 m_break(false)
{
  m_accessors[0] = 0;
  m_accessors[1] = 0;
  m_accessors[2] = 0;
  m_accessors[3] = 0;
}

/**Constructor of vector*/
Trk::FourObjectsAccessor::FourObjectsAccessor(int first,
                                              int second,
                                              int third,
                                              int fourth,
                                              bool inverse) :
 Trk::ObjectAccessor(inverse),
 m_jumper(0),
 m_break(false)
{
  m_accessors[0] = first;
  m_accessors[1] = second;
  m_accessors[2] = third;
  m_accessors[3] = fourth;
}

/**Copy constructor*/
Trk::FourObjectsAccessor::FourObjectsAccessor(const Trk::FourObjectsAccessor& accessor) :
 Trk::ObjectAccessor(accessor),
 m_jumper(accessor.m_jumper),
 m_break(false)
{}


Trk::FourObjectsAccessor& Trk::FourObjectsAccessor::operator=(const Trk::FourObjectsAccessor& rhs)
{
     if (this!=&rhs){
         Trk::ObjectAccessor::operator=(rhs);
         m_jumper       = rhs.m_jumper;
         m_accessors[0] = rhs.m_accessors[0];
         m_accessors[1] = rhs.m_accessors[1];
         m_accessors[2] = rhs.m_accessors[2];
         m_accessors[3] = rhs.m_accessors[3];
         m_break        = rhs.m_break;
        }
     return (*this);
}

MsgStream& Trk::FourObjectsAccessor::dump(MsgStream& sl) const
{
    sl << "FourObjectAccessor: | ";
     for (int iac=0; iac<4; ++iac) sl << m_accessors[iac] << " | ";
    if (inverseRetrieval()) sl << '\t' << "( inverseCase )";
    return sl;
}

std::ostream& Trk::FourObjectsAccessor::dump(std::ostream& sl) const
{
    sl << "FourObjectAccessor: | ";
     for (int iac=0; iac<4; ++iac) sl << m_accessors[iac] << " | ";
    if (inverseRetrieval()) sl << '\t' << "( inverseCase )";
    return sl;
}

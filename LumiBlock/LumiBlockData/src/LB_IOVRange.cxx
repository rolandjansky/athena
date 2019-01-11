/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//
// File: LumiBlockData/src/LB_IOVRange.cxx
// Author: Marjorie Shapiro <mdshapiro@lbl.gov>
//

#include "LumiBlockData/LB_IOVRange.h"

LB_IOVRange::LB_IOVRange() : IOVRange(0,0), m_NumExpected(0), m_NumSeen(0) {}

LB_IOVRange::LB_IOVRange( const IOVTime& start, const IOVTime& stop ) : IOVRange(start, stop ),
    m_NumExpected(0), m_NumSeen(0)  {}

LB_IOVRange::LB_IOVRange( const IOVRange& r ):  IOVRange(r.start(),r.stop()) ,
    m_NumExpected(0), m_NumSeen(0){}

LB_IOVRange::LB_IOVRange( const IOVRange& r, uint32_t numExpected, uint32_t numSeen):  IOVRange(r.start(),r.stop()),
  m_NumExpected(numExpected), m_NumSeen(numSeen) {}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef STGCRAWDATA_P1_TRK_H
#define STGCRAWDATA_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   sTgcRawData_p1.h
//
//-----------------------------------------------------------------------------

namespace Muon
{
  /** 
  */
    class StgcRawData_p1
    {
    public:
        StgcRawData_p1() : m_id(0) {}
                
        unsigned int        m_id; //!< FIXME! Remove this eventually
    };
}

#endif 

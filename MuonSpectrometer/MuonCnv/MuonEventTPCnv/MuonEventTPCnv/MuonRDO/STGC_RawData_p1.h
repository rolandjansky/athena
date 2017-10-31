/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef STGC_RAWDATA_P1_TRK_H
#define STGC_RAWDATA_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   STGC_RawData_p1.h
//
//-----------------------------------------------------------------------------

namespace Muon
{
  /** 
  */
    class STGC_RawData_p1
    {
    public:
        STGC_RawData_p1() : m_id(0) {}
                
        unsigned int        m_id; //!< FIXME! Remove this eventually
    };
}

#endif 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RRPPREPDATA_P3_TRK_H
#define RRPPREPDATA_P3_TRK_H

//-----------------------------------------------------------------------------
//
// file:   RpcPrepData_p3.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "Identifier/IdentifierHash.h"

namespace Muon
{
  /** 
  We don't write out (from Trk::PrepRawData) 
     * m_indexAndHash (can be recomputed), 
     * m_clusId (can be recomputed - well, it's basically stored in Muon::MdtPRD_Container_p3).
  */
    class RpcPrepData_p3
    {
    public:
        RpcPrepData_p3() : m_locX(0.0), m_errorMat(0.0), m_time(0.0), m_triggerInfo(0), m_ambiguityFlag(0){}
        
        std::vector< signed char > 	m_rdoList; //!< Store offsets
        
        /// @name Data from Trk::PrepRawData
        //@{
        float               m_locX; //!< Equivalent to localPosition (locX) in the base class.
        float               m_errorMat; //!< 1-d ErrorMatrix in the base class.
        //@}
        
        /// @name Data from Muon::RpcPrepData
        //@{
        float      m_time;
        int        m_triggerInfo;
        int        m_ambiguityFlag;
        //@}  
    };
}

#endif 

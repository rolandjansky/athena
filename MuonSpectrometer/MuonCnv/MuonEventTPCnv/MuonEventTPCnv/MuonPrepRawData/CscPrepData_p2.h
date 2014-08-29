/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CSCPREPDATA_P2_TRK_H
#define CSCPREPDATA_P2_TRK_H

//-----------------------------------------------------------------------------
//
// file:   CscPrepData_p2.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "Identifier/IdentifierHash.h"

namespace Muon
{
  /** 
  We don't write out (from Trk::PrepRawData) 
     * m_indexAndHash (can be recomputed), 
     * m_clusId (can be recomputed - well, it's basically stored in Muon::MdtPRD_Container_p2).
  */
    class CscPrepData_p2
    {
    public:
        CscPrepData_p2() : m_locX(0.0), m_errorMat(0.0),  m_charge(0), m_time(0.0), m_status(0) {}
        
        std::vector< signed char > 	m_rdoList; //!< Store offsets
        
        /// @name Data from Trk::PrepRawData
        //@{
        float               m_locX; //!< Equivalent to localPosition (locX) in the base class.
        float               m_errorMat; //!< 1-d ErrorMatrix in the base class.
        //@}
        
        /// @name Data from Muon::CscPrepData
        //@{
        int                 m_charge;
        float               m_time;
        int                 m_status; //!< in here we pack both the transient values m_status and m_timeStatus (introduced in MuonPrepRawData-03-11-00)
        //@}  
    };
}

#endif 

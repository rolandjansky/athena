/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTPREPDATA_P2_TRK_H
#define MDTPREPDATA_P2_TRK_H

//-----------------------------------------------------------------------------
//
// file:   MdtPrepData_p2.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "Identifier/IdentifierHash.h"

namespace Muon
{
    /** Persistent representation of the transient Muon::MdtPrepData class.
    We don't write out (from Trk::PrepRawData) 
       * m_rdoList (we don't cluster drift tubes), 
       * m_indexAndHash (can be recomputed), 
       * m_clusId (can be recomputed - well, it's basically stored in Muon::MdtPRD_Container_p2).*/
    class MdtPrepData_p2
    {
    public:
        MdtPrepData_p2() :  m_driftRadius(0.0), m_errorMat(0.0), m_tdc(0), m_adc(0), m_status(0){}
    /// @name Data from Trk::PrepRawData
    //@{
        float                                   m_driftRadius; //!< Equivalent to localPosition (locX) in the base class.
        float                                   m_errorMat; //!< 1-d ErrorMatrix in the base class.
    //@}

    /// @name Data from Muon::MdtPrepData
    //@{
        int                                     m_tdc;
        int                                     m_adc;
        /**Corresponds to Muon::MdtDriftCircleStatus, which currently has only two defined values. Could maybe pack into one of the other variables?*/
        int                                     m_status; 
    //@}        
    };
}

#endif 

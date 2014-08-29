/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TgcPREPDATA_P3_TRK_H
#define TgcPREPDATA_P3_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TgcPrepData_p3.h
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
    class TgcPrepData_p3
    {
    public:
        TgcPrepData_p3() : m_locX(0.), m_errorMat(0.), m_bcBitMap(0) {}
        
        std::vector< signed char > 	m_rdoList; //!< Store offsets
        
        /// @name Data from Trk::PrepRawData
        //@{
        float               m_locX; //!< Equivalent to localPosition (locX) in the base class.
        float               m_errorMat; //!< 1-d ErrorMatrix in the base class.
        uint16_t            m_bcBitMap; //!< BCID variable, 4:Previous, 2:Current, 1:Next
        //@}
 
    };
}

#endif 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef sTgcPREPDATA_p1_TRK_H
#define sTgcPREPDATA_p1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   sTgcPrepData_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include "Identifier/IdentifierHash.h"

namespace Muon
{
  /** 
  We don't write out (from Trk::PrepRawData) 
     * m_indexAndHash (can be recomputed), 
  */
    class sTgcPrepData_p1
    {
    public:
    sTgcPrepData_p1() : m_locX(0.0), m_errorMat(0.0), m_charge(0), m_time(0) {}
        
        std::vector< signed char > 	m_rdoList; //!< Store offsets
        
        /// @name Data from Trk::PrepRawData
        //@{
        float               m_locX; //!< Equivalent to localPosition (locX) in the base class.
        float               m_errorMat; //!< 1-d ErrorMatrix in the base class.
        //@}
	int                 m_charge;
	short int           m_time;
	uint16_t            m_bcBitMap;

	/// cluster quantities
	std::vector<uint16_t> m_stripNumbers;
	std::vector<short int> m_stripTimes;
	std::vector<int>      m_stripCharges;
        
        /// @name Data from Muon::sTgcPrepData
        //@{
        //@}  
    };
}

#endif 

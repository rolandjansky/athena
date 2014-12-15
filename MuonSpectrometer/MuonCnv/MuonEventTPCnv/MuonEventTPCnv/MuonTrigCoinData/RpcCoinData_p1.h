/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTTPCNV_RPCOINDATA_P1_TRK_H
#define MUONEVENTTPCNV_RPCOINDATA_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   RpcCoinData_p1.h
//
//-----------------------------------------------------------------------------
#include <vector>


namespace Muon
{
    /** Persistent representation of the transient Muon::RpcCoinData class.
    We don't write out (from Trk::PrepRawData) 
       * m_indexAndHash (can be recomputed), 
       * m_clusId (can be recomputed).
   For RpcCoinData_p1, I decided not to use MuonCoinDataCollection etc (as is used by TgcCoinData), as we can get better compression with MuonPRD_Container_p2 */
   class RpcCoinData_p1
   {
  public:
     RpcCoinData_p1() : m_localPos(0.), m_errorMat(0.), m_time(0.), m_ambiguityFlag(0), m_ijk(0),
                        m_threshold(0), m_overlap(0), m_parentCmId(0), m_parentPadId(0),
                        m_parentSectorId(0), m_lowPtCm(false) {}
     // base
     
     /// @name Data from Trk::PrepRawData (minus m_indexAndHash and m_clusId, which are both recomputed)
     //@{
     float                          m_localPos; //!< Equivalent to localPosition (locX) in the base class.
     float                          m_errorMat; //!< 1-d ErrorMatrix in the base class.
     std::vector<short>             m_rdoList; //!< delta of Identifiers of RDOs used to make PRD
     //@}

     /// @name Data from Muon::RpcPrepData 
     //@{     
     float                          m_time;
     int                            m_ambiguityFlag;
     // m_triggerInfo is in RpcCoinData
     //@}
     
     /// @name Data from RpcCoinData
     unsigned short                 m_ijk;
     unsigned short                 m_threshold;
     unsigned short                 m_overlap;
     unsigned short                 m_parentCmId;
     unsigned short                 m_parentPadId;
     unsigned short                 m_parentSectorId;
     bool                           m_lowPtCm;  
     //@}
   };
}

#endif 

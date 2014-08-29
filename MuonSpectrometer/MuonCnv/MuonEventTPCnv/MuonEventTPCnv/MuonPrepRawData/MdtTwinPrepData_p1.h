/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTTWINPREPDATA_P1_TRK_H
#define MDTTWINPREPDATA_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   MdtTwinPrepData_p1.h
//
//-----------------------------------------------------------------------------
//#include "AthenaPoolUtilities/TPObjRef.h"
//#include "Identifier/IdentifierHash.h"

namespace Muon
{
    /** Persistent representation of the transient Muon::MdtPrepData class.
    We don't write out (from Trk::PrepRawData) 
       * m_rdoList (we don't cluster drift tubes), 
       * m_indexAndHash (can be recomputed), 
       * m_clusId (can be recomputed - well, it's basically stored in Muon::MdtPRD_Container_p2).*/
    class MdtTwinPrepData_p1
    {
    public:
      MdtTwinPrepData_p1() :  m_driftRadius(0.0), m_errorDriftRadius(0.0), m_tdc(0), m_adc(0), m_tdcTwin(0), m_adcTwin(0),  
    m_xTwin(0.0), m_errorXTwin(0.0),  m_status(0){}
    /// @name Data from Trk::PrepRawData
    //@{
        float                                   m_driftRadius; //!< Equivalent to localPosition (locX) in the base class.
        float                                   m_errorDriftRadius; //!< [0,0]-component of 2-d ErrorMatrix in the base class.
    //@}

    /// @name Data from Muon::MdtPrepData
    //@{
        int                                     m_tdc;
        int                                     m_adc;
    /// @name Data from Muon::MdtTwinPrepData
    //@{
        int                                     m_tdcTwin;
        int                                     m_adcTwin;
    //@{
        float                                   m_xTwin; //!< Equivalent to localPosition (locY) in the base class. 
                                                         //   (x = local position along tube)
        float                                   m_errorXTwin; //!< [1,1]-component of 2-d ErrorMatrix in the base class.
    //@}
     
    /**Corresponds to Muon::MdtDriftCircleStatus, which currently has only two defined values. Could maybe pack into one of the other variables?*/
        int                                     m_status; 
    //@}        
    };
}

#endif 

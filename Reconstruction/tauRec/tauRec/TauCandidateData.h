/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_CANDIDATE_DATA_H
#define TAUREC_CANDIDATE_DATA_H
//-----------------------------------------------------------------------------
// file:        TauCandidateData.h
// package:     Reconstruction/tauEvent
// authors:     Lukasz Janyst
// date:        2007-02-13
//
//  MODIFICATIONS
// 2008-04-22 nicom: moved setObject()/getObject() to TauCandidateData
//
//-----------------------------------------------------------------------------

#include <string>
#include <map>
#include <boost/any.hpp>

#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauDetails.h"
#include "tauEvent/TauDetailsContainer.h"


#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODJet/Jet.h"


/**
 * @brief   The tau candidate object.
 * 
 *  Holds all containers and information needed for the tau reconstruction process.
 * 
 * @authors    Lukasz Janyst
 */

struct TauCandidateData
{
    //-----------------------------------------------------------------
    //! Associate some object to a key - this is meant to be used by
    //! TrigTauRec to pass container pointers to tauRec tools
    //-----------------------------------------------------------------
    template <typename P>
        void setObject( std :: string key, P ptr );

    //-----------------------------------------------------------------
    //! Check if something has been associated with given key
    //-----------------------------------------------------------------
    bool hasObject( std :: string key ) const;

    //-----------------------------------------------------------------
    //! Get the pointer associated with given key, if types don't
    //! match boost :: bad_any_cast exception is thrown
    //-----------------------------------------------------------------
    template <typename P>
        StatusCode getObject( std :: string key, P &ptr );
  
    xAOD::TauJet *xAODTau;
    xAOD::TauJetContainer* xAODTauContainer;
    xAOD::TauJetAuxContainer *tauAuxContainer;
    //think about changing this to IParticle
    const xAOD::Jet  *seed;
    const xAOD::JetContainer *seedContainer;
    unsigned int                        detailsNum;

    std :: map<std :: string, boost :: any> m_ptrMap;
};

//-------------------------------------------------------------------------
// Set pointer
//-------------------------------------------------------------------------
    template <typename P>
inline void TauCandidateData :: setObject( std :: string key, P ptr )
{
    m_ptrMap[key] = ptr;
}

//-------------------------------------------------------------------------
// Get pointer
//-------------------------------------------------------------------------
    template <typename P>
inline StatusCode TauCandidateData :: getObject( std :: string key, P &ptr )
{
    std :: map< std :: string, boost :: any> :: iterator p_it;
    p_it = m_ptrMap.find( key );
    if( p_it == m_ptrMap.end() )
        return StatusCode :: FAILURE;

    ptr = boost :: any_cast<P>( (*p_it).second );
    return StatusCode :: SUCCESS;
}

//-------------------------------------------------------------------------
// Test if any pointer has been associated with given key
//-------------------------------------------------------------------------
inline bool TauCandidateData :: hasObject( std :: string key ) const
{
    return m_ptrMap.find( key ) != m_ptrMap.end();
}

#endif // TAU_CANDIDATE_DATA

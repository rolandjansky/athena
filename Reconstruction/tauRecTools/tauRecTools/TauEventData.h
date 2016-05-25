/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_EVENT_DATA_H
#define TAUREC_EVENT_DATA_H
//-----------------------------------------------------------------------------
// file:        TauEventData.h was TauCandidateData.h
// package:     Reconstruction/tauEvent
// authors:     Lukasz Janyst
// date:        2007-02-13
//
//  MODIFICATIONS
// 2008-04-22 nicom: moved setObject()/getObject() to TauEventData
//
//-----------------------------------------------------------------------------

#include <string>
#include <map>
#include <boost/any.hpp>

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "AsgTools/StatusCode.h"

/**
 * @brief   The tau candidate object.
 * 
 *  Holds all containers and information needed for the tau reconstruction process.
 * 
 * @authors    Lukasz Janyst
 */

class TauEventData
{
public:
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
  
    xAOD::TauJetContainer* xAODTauContainer=0;
    xAOD::TauJetAuxContainer *tauAuxContainer=0;
    /* //think about changing this to IParticle */
    /* const xAOD::Jet  *seed=0; */
    const xAOD::JetContainer *seedContainer=0;
    unsigned int                        detailsNum=0;

    std :: map<std :: string, boost :: any> m_ptrMap;

    void clear();

    inline void setInTrigger(bool v=true);
    inline bool inTrigger() const;
   
 private:
    //don't clear this infomation
    bool m_inTrigger = false;

};

//-------------------------------------------------------------------------
// Set pointer
//-------------------------------------------------------------------------
    template <typename P>
inline void TauEventData :: setObject( std :: string key, P ptr )
{
    m_ptrMap[key] = ptr;
}

//-------------------------------------------------------------------------
// Get pointer
//-------------------------------------------------------------------------
    template <typename P>
inline StatusCode TauEventData :: getObject( std :: string key, P &ptr )
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
inline bool TauEventData :: hasObject( std :: string key ) const
{
    return m_ptrMap.find( key ) != m_ptrMap.end();
}

//-------------------------------------------------------------------------
// reset all information, nothing is deleted assuming pointers
// owned by storegate
//-------------------------------------------------------------------------
inline void TauEventData :: clear(){
  xAODTauContainer=0;
  tauAuxContainer=0;
  seedContainer=0;
  detailsNum=0;
  m_ptrMap.clear();
}

inline void TauEventData :: setInTrigger(bool v) {
  m_inTrigger=v;
}

inline bool TauEventData :: inTrigger() const{
  return m_inTrigger;
}

#endif // TAU_CANDIDATE_DATA

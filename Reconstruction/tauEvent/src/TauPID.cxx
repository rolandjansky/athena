/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauPID.cxx
///
/// Implementation of tau pid transient class
///
/// Package: Reconstruction/tauEvent
///
/// @author Thomas Burgess <tburgess-at-cern.ch>
///
/// Created 2009-06-03.

//TauEvent includes
#include "tauEvent/TauPID.h"
#include "tauEvent/TauCmp.h"
//C/C++ includes
#include <vector>

namespace Analysis {
    //Comparison predicate for doubles
    inline bool cmp(double a, double b) {
        static const double epsilon=1.0e-6;
        return (std::fabs(a-b)<epsilon);
    }

    TauPID::TauPID (std::vector<std::pair<TauJetParameters::TauID, double> >&& params,
                    const std::bitset<32>& isTauFlags,
                    const std::bitset<32>& vetoFlags)
      : m_params( std::move (params) ),
        m_isTauFlags( isTauFlags ),
        m_vetoFlags( vetoFlags )
    {

    }

    TauPID::TauPID(const TauPID& rhs) :
        m_params( rhs.m_params ),
        m_isTauFlags( rhs.m_isTauFlags ),
        m_vetoFlags( rhs.m_vetoFlags )
    {

    }

    TauPID& TauPID::operator = ( const TauPID &rhs )
    {
        if(this != &rhs)
        {
            m_params=rhs.m_params;
            m_isTauFlags=rhs.m_isTauFlags;
            m_vetoFlags=rhs.m_vetoFlags;
        }
        return *this;
    }

    TauPID& TauPID::operator = ( TauPID &&rhs )
    {
        if(this != &rhs)
        {
          m_params=std::move(rhs.m_params);
          m_isTauFlags=std::move(rhs.m_isTauFlags);
          m_vetoFlags=std::move(rhs.m_vetoFlags);
        }
        return *this;
    }

    bool TauPID::operator==( const TauPID& rhs ) const
    {
        //Loop over both params and make sure every entry is equal
        if ( !TauCmp::vcmp(m_params,rhs.m_params) ) return false;
        // 	std::vector<std::pair<TauJetParameters::TauID,
        // 	    double> >::const_iterator iparam, iparamrhs;
        // 	for (iparam = m_params.begin(), iparamrhs=rhs.m_params.begin();
        // 	     (iparam!=m_params.end()) && (iparamrhs!=rhs.m_params.end());
        // 	     ++iparam,++iparamrhs)
        // 	    if (cmp((*iparam).second,(*iparamrhs).second))
        // 		return false;
        if ( m_isTauFlags != rhs.m_isTauFlags )
            return false;
        if ( m_vetoFlags != rhs.m_vetoFlags )
            return false;
        return true;
    }

    const std::vector<std::pair<TauJetParameters::TauID, double> >& TauPID::params() const
    {
      return m_params;
    }
    const std::bitset<32> TauPID::isTauFlags() const
    {
      return m_isTauFlags;
    }
    const std::bitset<32> TauPID::vetoFlags() const
    {
      return m_vetoFlags;
    }
}

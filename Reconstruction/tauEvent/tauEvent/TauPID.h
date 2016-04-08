/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauCommonDetails.h
///
/// Declaration of tau identifiaction algorithms data storage class
///
/// Package: Reconstruction/tauEvent
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created by  Lukasz Janyst 2007-01-14
///
/// $Id: TauPID.h,v 1.6 2008-11-28 13:31:17 tburgess Exp $

#ifndef TAUEVENT_TAUPID_H
#define TAUEVENT_TAUPID_H

//TauEvent includes
#include "tauEvent/TauJetParameters.h"
//C/C++ includes
#include <vector>
#include <bitset>
#include <utility>

namespace Analysis
{
    /// Class containing discriminants produced by identification algorithms
    class TauPID
    {
        public:
            /// Constructor
            TauPID();

            /// Destructor
            virtual ~TauPID();

            TauPID (std::vector<std::pair<TauJetParameters::TauID, double> >&& params,
                    const std::bitset<32>& isTauFlags,
                    const std::bitset<32>& vetoFlags);


            /// Copy constructor
            TauPID(const TauPID& rhs);

            /// assignment operator
            ///@param rhs TauPID to copy
            TauPID &operator = ( const TauPID &rhs );

            TauPID &operator = ( TauPID &&rhs );

            /// Equality operator
            /// @param rhs right hand side TauPID instance to compare
            /// @return true if equal, false otherwise
            bool operator==( const TauPID& rhs ) const;

            /// Inequality operator
            /// @param rhs TauPID instance to compare to
            /// @return !operator==
            bool operator!=( const TauPID& rhs ) const;
            
            /// Check if class contains requested discriminant
            /// @param discID discriminant id requested
            bool hasDiscriminant( TauJetParameters::TauID discID ) const;

            /// Get value of discriminant
            /// @param discID discriminant id requested
            double discriminant(
                    TauJetParameters::TauID discID) const;

            /// Set value of discriminant
            /// @param discID discriminant id requested
            /// @param disc discriminant value
            void setDiscriminant(
                    TauJetParameters::TauID discID,
                    double disc );

            ///Get Flag for tau acceptance based on predefined arbitrary criteria
            /// @param flag flag to check
            bool isTau(
                    TauJetParameters::IsTauFlag flag ) const;

            ///Set Flag for tau acceptance based on predefined arbitrary criteria
            /// @param flag flag to set
            /// @param value value to set
            void setIsTau(
                    TauJetParameters::IsTauFlag flag,
                    bool value );

            /// Set veto flag
            /// @param flag flag to set
            /// @param value value to set
            void setFlag(
                    TauJetParameters::VetoFlags flag,
                    bool value );

            /// Get veto flag
            /// @param flag flag to check
            bool flag(
                    TauJetParameters::VetoFlags flag ) const;

            const std::vector<std::pair<TauJetParameters::TauID, double> >& params() const;
            const std::bitset<32> isTauFlags() const;
            const std::bitset<32> vetoFlags() const;

        //@}
        private:
            std::vector<std::pair<TauJetParameters::TauID, double> > m_params;
            std::bitset<32> m_isTauFlags;
            std::bitset<32> m_vetoFlags;
    };
}

#include "TauDetails.h"
#include "TauPID.icc"

#endif // TAUEVENT_TAUPID_H

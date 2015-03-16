/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///@file TauCmp.cxx
///
///Implementation of tau comparison predicates
///
///Package: Reconstruction/tauEvent
///
///@author Thomas Burgess
///
/// Created by Thomas Burgess on 2009-06-04.

//Tau event includes
#include "tauEvent/TauCmp.h"
#include "tauEvent/TauJetParameters.h"
//Other includes
#include "DataModel/ElementLink.h"
#include "CaloEvent/CaloCellContainer.h"
//C/C++ includes
#include <limits>
#include <cmath>
#include <utility>

namespace Analysis{
    namespace TauCmp {

        // Specialisations for cmp
        //
        // Any type that uses cmp must be implemented explicitly or code
        // will not link!

        template<> bool cmp<> (const double &a, const double &b) {
            return (std::fabs(a-b)<std::numeric_limits<float>::epsilon());
        }

        template<> bool cmp<ElementLink<CaloCellContainer> > (
                const ElementLink<CaloCellContainer> &a, 
                const ElementLink<CaloCellContainer> &b) {
            return (a==b);
        }

        template<> bool cmp <std::pair<TauJetParameters::TauID, double> > (
                const std::pair<TauJetParameters::TauID, double > &a,
                const std::pair<TauJetParameters::TauID, double > &b) 
        {
            return ((a.first==b.first)&&(a.second==b.second));
        }

    }
}

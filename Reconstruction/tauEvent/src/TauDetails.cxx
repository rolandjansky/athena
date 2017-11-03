/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file tauEvent/src/TauDetails.cxx
///
/// Implementation of tau details base class
///
/// Package: Reconstruction/tauEvent
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created by Lukasz Janyst on 2007-01-15.

#include "tauEvent/TauDetails.h"
#include <ostream>
#include <sstream>
#include <limits.h>

namespace Analysis
{
    const float TauDetails::DEFAULT = -1111.;
    const int TauDetails::DEFAULT_INT = INT_MIN;

    TauDetails :: TauDetails()
    {
    }

    TauDetails :: ~TauDetails()
    {
    }

    std::string TauDetails::toString() const
    {
        std::stringstream s;
        printOn(s);
        return s.str();
    }

    double TauDetails :: parameter(
            TauJetParameters :: Detail ) const
    {
        return 0.;
    }

    int TauDetails :: intParameter(
            TauJetParameters :: Detail ) const
    {
        return 0;
    }

    bool TauDetails :: hasParameter(
            TauJetParameters :: Detail  ) const
    {
        return false;
    }

    void TauDetails :: setParameter(
            TauJetParameters :: Detail , double )
    {
    }

    void TauDetails :: setIntParameter(
            TauJetParameters :: Detail , int  )
    {
    }
}

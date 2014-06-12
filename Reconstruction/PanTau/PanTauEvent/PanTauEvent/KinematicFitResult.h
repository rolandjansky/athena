/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#///////////////////////////////////////////////////////////////////
// KinematicFitResult.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PANTAUEVENT_KINEMATICFITRESULT_H
#define PANTAUEVENT_KINEMATICFITRESULT_H

#include "AthContainers/DataVector.h"

//#include "PanTauEvent/CorrectedEflowMomentum.h"

namespace PanTau {
class CorrectedEflowMomentum;
/** @class KinematicFitResult
    Helper class to store result of kinematic fit
    @author peter.wienemann@cern.ch
    */

class KinematicFitResult {

public:
    /** Default Constructor */
    KinematicFitResult();

    /** Full constructor
     * @param[in] chi square of fit
     * @param[in] number of degrees of freedom of fit
     */
    KinematicFitResult(
        double chi2,
        int ndof
        );

    /** Copy Constructor */
    KinematicFitResult(const KinematicFitResult& rhs);

    /** Assignment operator */
    KinematicFitResult& operator=(const KinematicFitResult& rhs);

    /** Destructor */
    virtual ~KinematicFitResult();

    /** returns chi2 of fit */
    double      chi2() const;

    /** returns number of degrees of freedom */
    int         ndof() const;

protected:
    double       m_chi2;
    int          m_ndof;
};


inline double PanTau::KinematicFitResult::chi2() const
{ 
    return m_chi2;
}

inline int PanTau::KinematicFitResult::ndof() const
{ 
    return m_ndof;
}

}

#endif // PANTAUEVENT_KINEMATICFITRESULT_H

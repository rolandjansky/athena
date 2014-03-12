/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          FitterStatusCode.h  -  description
                             -------------------
    email                : sebastian.fleischmann -at- cern.ch
 ***************************************************************************/


#ifndef TRKFITTERSTATUSCODE_H
#define TRKFITTERSTATUSCODE_H

#include "GaudiKernel/StatusCode.h"
#include <string>
#include <iostream>

class MsgStream;

namespace Trk {



/** @brief Status codes for track fitters
 
    This class can be returned by track fitters to give a more
    detailed feedback of fit failures.
 
    @author Sebastian.Fleischmann -at- cern.ch
*/
class FitterStatusCode
            //class FitterStatusCode : public StatusCode
{
public:
    //! fitter status codes
    enum FitCode {
        Success                 =  0,       //!< fit successfull
        StraightTrackModelUsed  =  1,       //!< fit successfull, but straight track model was used
        BadInput                =  2,       //!< bad input
        UpdateFailure           =  3,       //!< parameter update failed
        FitQualityFailure       =  4,       //!< fit quality production failed
        MissingCovariance       =  5,       //!< covariance missed
        ExtrapolationFailure    =  6,       //!< extrapolation failed
        ExtrapolationFailureDueToSmallMomentum  = 7,    //!< extrapolation failed due to small momentum
        CombineStatesFailure    =  8,       //!< fit states could not be combined
        WeaklyConstraintPars    =  9,       //!< parameters only weakly constrained
        OutlierLogicFailure     = 10,       //!< outlier logic failed
        PerigeeMakingFailure    = 11,       //!< perigee could not be produced
        ForwardFilterFailure    = 12,       //!< forward filter failed
        SmootherFailure         = 13,       //!< smoother failed
        FewFittableMeasurements = 14,       //!< number of fittable measurements on trajectory too small
        SeedFailure             = 15,       //!< wrong seed, not recoverable or recovery off.
        MatrixInversionFailure  = 16,
        NoConvergence           = 17,
        InvalidAngles           = 18,
        NumberOfStatusCodes     = 19        //!< number of different status codes

    };
    //! default constructor
    FitterStatusCode();

    /** @brief constructor

        @param code status code according to the enum */
    FitterStatusCode( FitCode );

    //! copy ctor
    FitterStatusCode( const FitterStatusCode& rhs );

    ~FitterStatusCode();

    bool isFailure() const;

    //! Assignment operator.
    FitterStatusCode& operator=(unsigned long value);
    FitterStatusCode& operator=(const FitterStatusCode& rhs);

    //    /** Comparison operator*/
    //    friend bool operator< ( const FitterStatusCode& a, const FitterStatusCode& b );

    //    /** Comparison operator*/
    //    friend bool operator> ( const FitterStatusCode& a, const FitterStatusCode& b );

    //! Cast operator.
    operator unsigned long() const;

    //! Cast operator.
    operator StatusCode() const;

    //! Get the status code by value.
    unsigned long getCode() const;

    //! Get the status code as descriptive string
    std::string getString() const;

private: // data members
    //! fitter status code
    unsigned long m_code;

};


}

MsgStream& operator<<(MsgStream& out, const Trk::FitterStatusCode& fitStatCode);

std::ostream& operator<<(std::ostream& out, const Trk::FitterStatusCode& fitStatCode);

#endif //TRKFITTERSTATUSCODE_H


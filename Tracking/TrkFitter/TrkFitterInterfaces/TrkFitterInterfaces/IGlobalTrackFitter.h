/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// IGlobalTrackFitter.h
//   Header file for interface of global-least-squares track fitters
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Thijs.Cornelissen at cern.ch, Wolfgang.Liebig at cern.ch
///////////////////////////////////////////////////////////////////
#ifndef TRK_IGLOBALTRACKFITTER_H
#define TRK_IGLOBALTRACKFITTER_H

#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkParameters/TrackParameters.h"
#include "EventPrimitives/EventPrimitives.h"
//
//class HepMatrix; class HepSymMatrix;

namespace Trk {

    /** @class IGlobalTrackFitter

        provides additional abstract interfaces for information 
        provided from global-least-squares track fit algorithms working
        in the ATLAS EDM.
    */
    class IGlobalTrackFitter : virtual public ITrackFitter { 

    friend class ShiftingDerivCalcTool;
    friend class Chi2DerivCalcTool;

  public:

    /** access to the matrix of derivatives used during the latest
        global-chi2 track fit. */
    virtual Amg::MatrixX*		DerivMatrix() const = 0;

    /** access to the global fitter's full covariance matrix. */
    virtual Amg::MatrixX*	FullCovarianceMatrix() const = 0;

    /** returns the number of iterations used by the last fit
        (count starts at 1 for a single-iteration fit) */
    virtual int iterationsOfLastFit() const = 0;

  private:

    /** sets the minimum number of iterations to be used in the 
        track fit. */
    virtual void setMinIterations(int iterations) = 0;


  };
}

#endif

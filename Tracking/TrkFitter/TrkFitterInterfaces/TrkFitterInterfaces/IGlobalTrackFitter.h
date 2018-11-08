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

    struct AlignmentCache{

    /** access to the matrix of derivatives used during the latest
        global-chi2 track fit. */
     Amg::MatrixX*	m_derivMatrix = nullptr;

    /** access to the global fitter's full covariance matrix. */
     Amg::MatrixX*	m_fullCovarianceMatrix = nullptr;

    /** returns the number of iterations used by the last fit
        (count starts at 1 for a single-iteration fit) */
     int m_iterationsOfLastFit  = 1;
    /** sets the minimum number of iterations to be used in the
        track fit. */
     int m_minIterations = 0;

     ~AlignmentCache(){
       delete m_derivMatrix;
       delete m_fullCovarianceMatrix;
     }

    };

    /** RE-FIT A TRACK FOR ALIGNMENT.
        Since it is not our but the fitter model's decision if to
        re-fit on PRD or ROT level, it is made pure virtual. */
    virtual Track* alignmentFit( AlignmentCache&,
                        const Track&,
                        const RunOutlierRemoval  runOutlier=false,
                        const ParticleHypothesis matEffects=Trk::nonInteracting) const = 0;

  };
}

#endif

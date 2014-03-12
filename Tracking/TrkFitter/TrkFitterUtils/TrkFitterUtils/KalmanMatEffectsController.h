/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// KalmanMatEffectsController.h
//   Class grouping variables for saving DNA values
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_KALMANMATEFFECTSCONTROLLER_H
#define TRK_KALMANMATEFFECTSCONTROLLER_H

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkFitterUtils/TrackBreakpointType.h"

namespace Trk{

  /** @brief This class is a wrapper around the input particle hypothesis given
             to ATLAS' Kalman-style track fitter with the purpose to steer special
             material effects (eg DNA) more effectively.

     The KalmanMatEffectsController holds information if the KalmanFitter has DNA
     configured (and is actually a KalmanDNAFitter) and about the particle
     hypothesis and brem detection status of the current track fit.
     It translates this knowledge to decisions,
     - which particle hypothesis should be used for extrapolation.
       The special case is that electron material corrections are made externally,
       needing a standard pion extrapolation.
     - what tuning DNA should use: generic, that is to correct electrons but not bias pion/muon
        or electron, and optimise the sensitivity to brem

  @author W. Liebig <http://consult.cern.ch/xwho>

  */
  class KalmanMatEffectsController
  {
  public:
    //! default constructor - needed for what?
    KalmanMatEffectsController() :
      m_inputHypothesis(Trk::pion),
      m_haveDNA(false),
      m_breakPointType(BreakpointNotSpecified) {}

    //! full constructor
    KalmanMatEffectsController(const ParticleHypothesis    hypo,
                               const bool                  haveDNA=false,
                               const TrackBreakpointType   btype=BreakpointNotSpecified) :
      m_inputHypothesis(hypo),
      m_haveDNA(haveDNA),
      m_breakPointType(btype) {}

    /* copy constructor - let compiler implement
    KalmanMatEffectsController( const KalmanMatEffectsController& dmef);
    // default desctructor - also compiler
    ~KalmanMatEffectsController(); */

    //! particleHypothesis as it was passed to the fitter
    ParticleHypothesis originalParticleHypo()    const;
    //! particleHypothesis as it should be used for extrapolation calls
    ParticleHypothesis particleType()    const;
    //! flag saying if dynamic noise adjustment should be called or not
    bool doDNA()                         const;
    //! flag saying if dynamic noise adjustment should maximise sensitivity to electron bremsstrahlung (aggressive tuning) or minimise effects on non-electron particles (generic tuning)
    bool aggressiveDNA()                 const;
    //! return breakpoint type resulting from DNA-based breakpoint search
    TrackBreakpointType breakpointType() const;
    //! sets breakpoint type after DNA & Separator provide such information
    void updateBreakpoint(TrackBreakpointType) const;

  private:
    const ParticleHypothesis m_inputHypothesis;
    const bool               m_haveDNA;
    mutable TrackBreakpointType m_breakPointType;
  };

  /*
    MsgStream& operator << ( MsgStream& sl, const KalmanMatEffectsController& dme);
    std::ostream& operator << ( std::ostream& sl, const KalmanMatEffectsController& dme);
  */

  inline ParticleHypothesis KalmanMatEffectsController::originalParticleHypo() const
    {
      return m_inputHypothesis;
    }

  inline ParticleHypothesis KalmanMatEffectsController::particleType() const
    {
      if (m_haveDNA && m_inputHypothesis==Trk::electron) return Trk::pion;
      return m_inputHypothesis;
    }

  inline bool KalmanMatEffectsController::doDNA() const
    {
      return ( m_haveDNA
               && (m_breakPointType==Trk::BreakpointNotSpecified
                   || m_breakPointType==Trk::DnaBremPointUseful) );
    }

  inline bool KalmanMatEffectsController::aggressiveDNA() const
    { 
      return (this->doDNA() && m_inputHypothesis==electron); 
    }

  inline TrackBreakpointType KalmanMatEffectsController::breakpointType() const
    { return m_breakPointType; }

  inline void KalmanMatEffectsController::updateBreakpoint(TrackBreakpointType newBPT) const
  { m_breakPointType = newBPT; }

} // end namespace Trk
#endif

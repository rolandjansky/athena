// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_MUON_EF_INFO_DISTANCE_FUNCTOR_H
#define TRIG_MUON_EF_INFO_DISTANCE_FUNCTOR_H

#include "TrigObjectMatching/DistanceFunctor.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"

/**
 * @brief The TrigMuonEFInfoDistanceFuntor is the default distance functor
 *        when  matching to TrigMuonEFInfo objects.  It works by determining
 *        the deltaR between the supplied object the track associated with the
 *        TrigMuonEFInfo object.  Note that you can set what type of track to use
 *        via a static member function:
 *
 *        TrigMatch::TrigMuonEFInfoHelper::setTrackToUse(useCombinedTrack);
 *
 *        This will set the method used by all instances of the metric, so be careful!
 */

namespace TrigMatch {

   /// enum defining which track to use from the MuonEFInfo object
   enum {
      useSpectrometerTrack = 0, ///< Use the track reconstructed in the MS
      useExtrapolatedTrack = 1, ///< Use the track extrapolated to the IP
      useCombinedTrack = 2, ///< Use the combined track
      useMuGirlTrack = 3, ///< Use a "MuGirl" track (for _MG chains)
      useAnyTrack = 4, ///< Use any of these tracks (use with caution)
      useMuGirlTagTrack = 5, ///< Use a "MuGirl tag" track
      useAnyMuGirlTrack = 6, ///< Use either "MuGirl" or "MuGirl tag" tracks
      usePhysicsTracks = 7, ///< If TrigMuGirl, use combined. If TrigMuonEF, use combined if available, else extrapolated standalone.
      useObjectPointer = 100 ///< Compare the object pointers
   };

   /// This class does the major processing and calculations for the metrics
   class TrigMuonEFInfoHelper {

   public:
      /// Set which track to use in the matching
      static void setTrackToUse( int trackToUse );
      /// Investigate which track is used in the matching
      static int trackToUse();

      // given a TrigMuonEFInfo object and another object for which eta and phi
      // are defined, calculate the deltaR
      template<typename V>
      static float calculateInfoDistance( const TrigMuonEFInfo *t, const V *v );
      /// Function for calculating the distance of two TrigMuonEFInfo objects
      static float calculateInfoInfoDistance( const TrigMuonEFInfo *left,
                                              const TrigMuonEFInfo *right );

   private:
      /// store the type of track to use
      static int m_trackToUse;

   }; // class TrigMuonEFInfoHelper

} // namespace TrigMatch

// We make two classes - one for when TrigMuonEFInfo is used on the left,
// and one for when its used on the right

// when TrigMuonEFInfo appears on the left
template<typename U>
class TrigMuonEFInfoDistanceFunctor : public DistanceFunctor<TrigMuonEFInfo, U> {

    public:
        TrigMuonEFInfoDistanceFunctor() { };
        virtual ~TrigMuonEFInfoDistanceFunctor() { };
    private:
        virtual float calculateDistance(const TrigMuonEFInfo *t, const U *u) const {
            return TrigMatch::TrigMuonEFInfoHelper::calculateInfoDistance(t,u);
        }

};

// when TrigMuonEFInfo appears on the right
template<typename U>
class TrigMuonEFInfoDistanceFunctorRight : public DistanceFunctor<U, TrigMuonEFInfo> {

    public:
        TrigMuonEFInfoDistanceFunctorRight() { };
        virtual ~TrigMuonEFInfoDistanceFunctorRight() { };
    private:
        virtual float calculateDistance(const U *u, const TrigMuonEFInfo *t) const {
            return TrigMatch::TrigMuonEFInfoHelper::calculateInfoDistance(t, u);
        }

};

// when TrigMuonEFInfo appears on both sides
class TrigMuonEFInfoDistanceFunctorBoth : public DistanceFunctor<TrigMuonEFInfo, TrigMuonEFInfo> {

    public:
        TrigMuonEFInfoDistanceFunctorBoth() { };
        virtual ~TrigMuonEFInfoDistanceFunctorBoth() { };
    private:
        virtual float calculateDistance(const TrigMuonEFInfo *l, const TrigMuonEFInfo *r) const {
            return TrigMatch::TrigMuonEFInfoHelper::calculateInfoInfoDistance(l, r);
        }

};

#include "TrigObjectMatching/TrigMuonEFInfoDistanceFunctor.icc"

#endif // TRIG_MUON_EF_INFO_DISTANCE_FUNCTOR_H

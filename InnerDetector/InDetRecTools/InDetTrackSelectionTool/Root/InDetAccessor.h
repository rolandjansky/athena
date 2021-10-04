/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _InDetAccessor_H_
#define _InDetAccessor_H_

#include <tuple>
#include <array>
#include "AsgMessaging/AsgMessaging.h"

#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"

#ifndef XAOD_ANALYSIS
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkTrackSummary/TrackSummary.h"
#endif

#define IDTRKSEL_MSG(msg_type,the_msg) if (msgHelper.msgLvl(msg_type)) { msgHelper.msg() << the_msg << endmsg; } do {} while (false)
#define IDTRKSEL_MSG_DEBUG(the_msg) IDTRKSEL_MSG(MSG::DEBUG,the_msg)
#define IDTRKSEL_MSG_WARNING(the_msg) IDTRKSEL_MSG(MSG::WARNING,the_msg)
#define IDTRKSEL_MSG_ERROR(the_msg) IDTRKSEL_MSG(MSG::ERROR,the_msg)

namespace InDetAccessor {

   // from TrkEventPrimitives/ParamDefs.h
   enum ETrackParameters {
      d0 = 0,
      z0 = 1,
      phi0 = 2,
      theta = 3,
      qOverP = 4, //!< perigee
   };

   //cannot use partial spezialisation of function templates
   // so define functsion for TrackParticles, and TrkTracks
   class TrackParticleHelper;
   template <short index>
   double getDefiningParameters(TrackParticleHelper helper, asg::AsgMessaging &msgHelper);

#ifndef XAOD_ANALYSIS
   class TrkTrackHelper;
   template <short index>
   double getDefiningParameters(TrkTrackHelper helper, asg::AsgMessaging &msgHelper);
#endif

   class TrackParticleHelper {
   public:
      TrackParticleHelper(const xAOD::TrackParticle &a_track,const xAOD::Vertex *a_vtx)
         : m_track(&a_track), m_vtx(a_vtx) {}

      const xAOD::TrackParticle &track()     const { return *m_track;}
      const xAOD::Vertex *vertex()           const { return  m_vtx; }

      double pt(asg::AsgMessaging &)            const { return track().pt(); }
      double eta(asg::AsgMessaging &)           const { return track().eta(); }
      double theta(asg::AsgMessaging &msgHelper)const { return getDefiningParameters<InDetAccessor::theta>(*this,msgHelper); }
      double qOverP(asg::AsgMessaging &)        const { return track().qOverP(); }
      double d0(asg::AsgMessaging &msgHelper)   const { return getDefiningParameters<InDetAccessor::d0>(*this,msgHelper);}
      double z0(asg::AsgMessaging &msgHelper)   const { return getDefiningParameters<InDetAccessor::z0>(*this,msgHelper);}

   private:
      const xAOD::TrackParticle *m_track;
      const xAOD::Vertex *m_vtx;
   };

#ifndef XAOD_ANALYSIS
   class TrkTrackHelper {
   public:
      TrkTrackHelper(const Trk::Track &a_track, const Trk::TrackSummary &summary, const Trk::TrackParameters *a_perigee )
         : m_track(&a_track), m_summary(&summary), m_perigee(a_perigee) {}

      const Trk::Track &track()                const { return *m_track;}
      const Trk::TrackSummary &summary()       const { return *m_summary;}
      const Trk::TrackParameters *perigee()    const { return m_perigee; }

      double pt(asg::AsgMessaging &msgHelper)     const {
         return hasPerigeeOrError(msgHelper)  ? perigee()->momentum().perp()   : std::nan("");
      }
      double eta(asg::AsgMessaging &msgHelper)    const {
         return hasPerigeeOrError(msgHelper)  ? perigee()->momentum().eta()     : std::nan("");
      }
      double theta(asg::AsgMessaging &msgHelper)  const { return getDefiningParameters<InDetAccessor::theta>(*this,msgHelper);}
      double qOverP(asg::AsgMessaging &msgHelper) const {
         return hasPerigeeOrError(msgHelper)  ?  1./perigee()->momentum().mag() : std::nan("");
      }
      double d0(asg::AsgMessaging &msgHelper)     const { return getDefiningParameters<InDetAccessor::d0>(*this,msgHelper);}
      double z0(asg::AsgMessaging &msgHelper)     const { return getDefiningParameters<InDetAccessor::z0>(*this,msgHelper);}

      bool hasPerigeeOrError(asg::AsgMessaging &msgHelper) const {
         bool ret = perigee();
         if (!ret) {
            IDTRKSEL_MSG_ERROR( "Received zero pointer to perigee." );
         }
         return ret;
      }
      bool hasSummaryOrError(asg::AsgMessaging &msgHelper) const {
         bool ret = perigee();
         if (!ret) {
            IDTRKSEL_MSG_ERROR( "Received null pointer to track summary." );
         }
         return ret;
      }
   private:

      const Trk::Track *m_track;
      const Trk::TrackSummary *m_summary;
      const Trk::TrackParameters *m_perigee;
   };
#endif


   // summary accessor
   template <typename T_TrkHelper>
   uint8_t getSummary(const T_TrkHelper & helper, asg::AsgMessaging &msgHelper, xAOD::SummaryType sumType);

   template <>
   inline uint8_t getSummary(const TrackParticleHelper & helper, asg::AsgMessaging &msgHelper, xAOD::SummaryType summaryType) {
      uint8_t summaryValue = 0;
      if (!helper.track().summaryValue(summaryValue, summaryType)) {
         IDTRKSEL_MSG_DEBUG( "Failed to get SummaryType " << std::to_string(summaryType) << " from xAOD::TrackParticle summary. A value of zero will be used instead." );
         summaryValue = 0;
      }
      return summaryValue;
   }

#ifndef XAOD_ANALYSIS
   template <>
   inline uint8_t getSummary(const TrkTrackHelper & helper, asg::AsgMessaging &msgHelper, xAOD::SummaryType summaryType) {
      Int_t summaryTypeInt = static_cast<Int_t>(summaryType);
      Int_t checkSummaryValue = helper.summary().get( static_cast<Trk::SummaryType>(summaryTypeInt) );
      if (checkSummaryValue < 0) {
         // Trk::TrackSummary::get() will return -1 if the data cannot be retrieved
         IDTRKSEL_MSG_DEBUG( "Received " << checkSummaryValue << " for SummaryType " << std::to_string(summaryType) << " from Trk::TrackSummary. A value of zero will be used instead." );
         checkSummaryValue = 0;
      }
      return checkSummaryValue;
   }
#endif

   template <std::size_t n_summary_types, typename T_TrkHelper>
   inline uint8_t getSummarySum(const T_TrkHelper helper, asg::AsgMessaging &msgHelper, std::array<xAOD::SummaryType,n_summary_types> sumTypes) {
      uint8_t sum=0;
      for (xAOD::SummaryType sum_type: sumTypes) {
         sum += getSummary(helper, msgHelper, sum_type);
      }
      return sum;
   }


   // fit quality accessor
   template <typename Trk_Helper>
   double getFitChiSquare(const Trk_Helper & helper, asg::AsgMessaging &msgHelper);

   template <>
   inline double getFitChiSquare(const TrackParticleHelper & helper, asg::AsgMessaging &) {
      return helper.track().chiSquared();
   }

#ifndef XAOD_ANALYSIS
   template <>
   inline double getFitChiSquare(const TrkTrackHelper & helper, asg::AsgMessaging &msgHelper) {
      if (!helper.track().fitQuality()) {
         IDTRKSEL_MSG_WARNING( "Zero pointer to fit quality received." );
         return std::nan("");
      }
      return helper.track().fitQuality()->chiSquared();
   }
#endif

   template <typename TrkHelper>
   double getFitNDoF(const TrkHelper & helper, asg::AsgMessaging &msgHelper);

   template <>
   inline double getFitNDoF(const TrackParticleHelper & helper, asg::AsgMessaging &) {
      return helper.track().numberDoF();
   }

#ifndef XAOD_ANALYSIS
   template <>
   inline double getFitNDoF(const TrkTrackHelper & helper, asg::AsgMessaging &msgHelper) {
      if (!helper.track().fitQuality()) {
         IDTRKSEL_MSG_WARNING( "Zero pointer to fit quality received." );
         return std::nan("");
      }
      return helper.track().fitQuality()->doubleNumberDoF();
   }
#endif


   // eProbabilityHT accessor
   template <typename Trk_Helper>
   float getEProbabilityHT(const Trk_Helper & helper, asg::AsgMessaging &msgHelper);

   template <>
   inline float getEProbabilityHT(const TrackParticleHelper & helper, asg::AsgMessaging &msgHelper) {
      float eProbHT =0.f;
      if (!helper.track().summaryValue(eProbHT, xAOD::SummaryType::eProbabilityHT)) {
         IDTRKSEL_MSG_DEBUG( "Failed to get eProbabilityHT from xAOD::TrackParticle summary. A value of zero will be used instead." );
         eProbHT =0.f;
      }
      return eProbHT;
   }

#ifndef XAOD_ANALYSIS
   template <>
   inline float getEProbabilityHT(const TrkTrackHelper & helper, asg::AsgMessaging &msgHelper) {
      float eProbHT;
      if (!helper.hasSummaryOrError(msgHelper)) {
         eProbHT = 0.f;
      }
      else {
         eProbHT = helper.summary().getPID( Trk::eProbabilityType::eProbabilityHT );
         if (eProbHT < 0.f) {
            IDTRKSEL_MSG_DEBUG( "Received " << eProbHT << " for eProbabilityHT. A value of zero will be used instead." );
            eProbHT= 0.f;
         }
      }
      return eProbHT;
   }
#endif

   //defining parameters accessor

   template <short index>
   inline double getDefiningParameters(TrackParticleHelper  helper, asg::AsgMessaging &) {
      static_assert(index<5);
      double param = helper.track().definingParameters()[index];
      if (index==1) {
         if (helper.vertex()) {
            param +=  helper.track().vz() - helper.vertex()->z();
         }
      }
      return param;
   }

#ifndef XAOD_ANALYSIS
   template <short index>
   //cppcheck-suppress passedByValue
   inline double getDefiningParameters(TrkTrackHelper  helper, asg::AsgMessaging &msgHelper) {
      assert(index<5);
      return helper.hasPerigeeOrError(msgHelper) ? helper.perigee()->parameters()[index] : std::nan("");
   }
#endif

   // defining parameters covariance accessor
   template <typename Trk_Helper>
   inline double getDefiningParametersCov(Trk_Helper & helper, [[maybe_unused]] asg::AsgMessaging &msgHelper, unsigned int index_i, unsigned int index_j);

   template <>
   inline double getDefiningParametersCov(TrackParticleHelper & helper, [[maybe_unused]] asg::AsgMessaging &msgHelper, unsigned int index_i, unsigned int index_j) {
      assert( index_i<5 && index_j<5);
      return helper.track().definingParametersCovMatrix()(index_i, index_j);
   }

#ifndef XAOD_ANALYSIS
   template <>
   inline double getDefiningParametersCov(TrkTrackHelper & helper, asg::AsgMessaging &msgHelper, unsigned int index_i, unsigned int index_j) {
      assert( index_i<5 && index_j<5);
      return helper.hasPerigeeOrError(msgHelper) ? (*helper.perigee()->covariance())(index_i, index_j) : std::nan("");
   }
#endif

   template <typename Trk_Helper>
   inline Int_t getNumberOfUsedHitsdEdx(Trk_Helper & helper, asg::AsgMessaging &msgHelper);

   template <>
   inline Int_t getNumberOfUsedHitsdEdx(TrackParticleHelper & helper, [[maybe_unused]] asg::AsgMessaging &msgHelper) {
      return helper.track().numberOfUsedHitsdEdx();
   }

#ifndef XAOD_ANALYSIS
   template <>
   inline Int_t getNumberOfUsedHitsdEdx(TrkTrackHelper & helper, asg::AsgMessaging &msgHelper) {
      int n_used_hits;
      if (!helper.hasSummaryOrError(msgHelper)) {
         n_used_hits=0;
      }
      else {
         n_used_hits = helper.summary().numberOfUsedHitsdEdx();
         if (n_used_hits < 0) {
            // Trk::TrackSummary::numberOfUsedHitsdEdx() will return -1 if the data cannot be retrieved
            IDTRKSEL_MSG_DEBUG( "Received " << n_used_hits << " for numberOfUsedHitsdEdx from Trk::TrackSummary. A value of zero will be used instead." );
            n_used_hits = 0;
         }
      }
      return n_used_hits;
   }
#endif

   template <typename Trk_Helper>
   inline Int_t getNumberOfIBLOverflowsdEdx(Trk_Helper & helper, asg::AsgMessaging &msgHelper);

   template <>
   inline Int_t getNumberOfIBLOverflowsdEdx(TrackParticleHelper & helper, [[maybe_unused]] asg::AsgMessaging &msgHelper) {
      return helper.track().numberOfUsedHitsdEdx();
   }

#ifndef XAOD_ANALYSIS
   template <>
   inline Int_t getNumberOfIBLOverflowsdEdx(TrkTrackHelper & helper, asg::AsgMessaging &msgHelper) {
      int n_overflow_hits;
      if (!helper.hasSummaryOrError(msgHelper)) {
         n_overflow_hits=0;
      }
      else {
         n_overflow_hits = helper.summary().numberOfOverflowHitsdEdx();
         if (n_overflow_hits < 0) {
            // Trk::TrackSummary::numberOfUsedHitsdEdx() will return -1 if the data cannot be retrieved
            IDTRKSEL_MSG_DEBUG( "Received " << n_overflow_hits << " for numberOfOverflowHitsdEdx from Trk::TrackSummary. A value of zero will be used instead." );
            n_overflow_hits = 0;
         }
      }
      return n_overflow_hits;
   }
#endif

#ifndef XAOD_ANALYSIS
   std::tuple<uint8_t,uint8_t> getSiHitsTopBottom( const Trk::Track& track, asg::AsgMessaging &msgHelper);

   template <typename Trk_Helper>
   inline std::tuple<uint8_t,uint8_t> getSiHitsTopBottom(Trk_Helper & helper, asg::AsgMessaging &msgHelper);

   template <>
   inline std::tuple<uint8_t,uint8_t> getSiHitsTopBottom(TrackParticleHelper & helper, asg::AsgMessaging &msgHelper) {
      const Trk::Track* trkTrack = helper.track().track();
      if (!trkTrack) {
         IDTRKSEL_MSG_WARNING( "xAOD::TrackParticle has a null pointer to a Trk::Track." );
         IDTRKSEL_MSG_WARNING( "Unable to access top/bottom Si hits." );
         return std::tuple<uint8_t,uint8_t>{};
      }
      else {
         return getSiHitsTopBottom(*trkTrack,msgHelper);
      }
   }

   template <>
   inline std::tuple<uint8_t,uint8_t> getSiHitsTopBottom(TrkTrackHelper & helper, asg::AsgMessaging &msgHelper) {
      return getSiHitsTopBottom(helper.track(),msgHelper);
   }
#endif
}
#endif

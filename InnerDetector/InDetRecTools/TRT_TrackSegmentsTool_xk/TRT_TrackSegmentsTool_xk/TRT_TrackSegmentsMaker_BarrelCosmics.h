/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////////////////
//
//    TRT_TrackSegmentsMaker_BarrelCosmics.h, (c) ATLAS Detector software
//
//    TRT standalone segment finder for TRT barrel
//    specialized for cosmics and L2 trigger, plan to make it working in a magnetic field
//
///////////////////////////////////////////////////////////////////////////////////////////

#ifndef TRT_TrackSegmentsMaker_BarrelCosmics_H
#define TRT_TrackSegmentsMaker_BarrelCosmics_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetRecToolInterfaces/ITRT_TrackSegmentsMaker.h"
#include "TrkEventUtils/EventDataBase.h"

#include "StoreGate/ReadHandleKey.h"
#include <vector>
#include <string>
#include <memory>
#include <iosfwd>

class MsgStream;
class TRT_ID;

namespace InDet{

  /** @class TRT_TrackSegmentsMaker_BarrelCosmics

      AlgTool that creates TrackSegments out of TRT Driftcircles
      for barrel TRT only, specializing for cosmic runs (to be used for LVL2 trigger)

      @author  Sasa.Fratina@cern.ch
   */

  class TRT_TrackSegmentsMaker_BarrelCosmics : virtual public ITRT_TrackSegmentsMaker, public AthAlgTool {

    public:

      TRT_TrackSegmentsMaker_BarrelCosmics(const std::string &, const std::string &, const IInterface*);
      virtual ~TRT_TrackSegmentsMaker_BarrelCosmics() {};

      ///////////////////////////////////////////////////////////////////////////////
      //  methods inherited from ITRT_TrackSegmentsMaker
      //  in each event, methods are called in this order:
      //  -  newEvent() in the offline, newRegion(...) for L2 (directed search)
      //  -  find() finds all segments
      //  -  next() many times (returns next segment), untill next() returns 0 pointer
      ///////////////////////////////////////////////////////////////////////////////

      virtual StatusCode               initialize() override;
//      StatusCode magneticFieldInit(IOVSVC_CALLBACK_ARGS);
      virtual StatusCode               finalize  () override;

      virtual std::unique_ptr<InDet::ITRT_TrackSegmentsMaker::IEventData> newEvent(const EventContext& ctx) const override;
      virtual std::unique_ptr<InDet::ITRT_TrackSegmentsMaker::IEventData> newRegion(const EventContext& ctx, const std::vector<IdentifierHash>&) const override;
      void endEvent(InDet::ITRT_TrackSegmentsMaker::IEventData &event_data) const override;

      virtual void find(const EventContext& ctx, InDet::ITRT_TrackSegmentsMaker::IEventData &event_data) const override;
      virtual Trk::TrackSegment *next(InDet::ITRT_TrackSegmentsMaker::IEventData &event_data) const override;

      virtual MsgStream&    dump          (MsgStream   & out) const override
      { return out; };
      virtual std::ostream& dump          (std::ostream& out) const override
      { return out; };


    protected:
      class EventData;
      class EventData : public Trk::EventDataBase<EventData,InDet::ITRT_TrackSegmentsMaker::IEventData>
      {
         friend class TRT_TrackSegmentsMaker_BarrelCosmics;
      public:
         EventData(const TRT_DriftCircleContainer *trtcontainer) :m_trtcontainer(trtcontainer) {}
         ~EventData() {}

         void clear() {
            m_listHits.clear();
            m_listHitCenter.clear();
            m_segmentDriftCircles.clear();
            m_segments.clear();
            m_segmentDriftCirclesCount = 0;
         }

      protected:
         const TRT_DriftCircleContainer                            *m_trtcontainer = nullptr;
         unsigned int                                               m_segmentDriftCirclesCount = 0;
         std::vector< const InDet::TRT_DriftCircle * >              m_listHits;
         std::vector< Amg::Vector3D >                               m_listHitCenter;

         std::vector< std::vector<const InDet::TRT_DriftCircle *> > m_segmentDriftCircles; // vector of DriftCircles associated to the segments
         std::vector< Trk::TrackSegment * >                         m_segments;            //!< List of found segments
      };

      ///////////////////////////////////////////////////////////////////
      // Protected data and methods
      ///////////////////////////////////////////////////////////////////



      SG::ReadHandleKey<InDet::TRT_DriftCircleContainer>     m_driftCirclesName{this,"TRT_ClustersContainer","TRT_DriftCircles","RHK to retrieve Drift Circles"} ;  //!< Container with TRT clusters
      std::string                              m_TRTManagerName ; //!< Name of TRT det. manager

      const TRT_ID*                            m_trtid       ;

    private:

      // other methods specific for this algorithm

      int findSeed(double xmin,
                   double xmax,
                   double phimin,
                   double phimax,
                   double *bestParameters,
                   TRT_TrackSegmentsMaker_BarrelCosmics::EventData &event_data) const;

      void findSeedInverseR(double *par, TRT_TrackSegmentsMaker_BarrelCosmics::EventData &event_data) const;

      // follow
      // convert lists of TRT circles (std::vector<const InDet::TRT_DriftCircle *>) to track segments (Trk::TrackSegment * -> fill m_segments):
      void convert(std::vector<const InDet::TRT_DriftCircle *> &hits,
                  double *trackpar,
                  TRT_TrackSegmentsMaker_BarrelCosmics::EventData &event_data) const;

      void findOld(TRT_TrackSegmentsMaker_BarrelCosmics::EventData &event_data) const;  // old function - need to debug an increased cpu consumption


      static void linearRegressionParabolaFit(double *mean, double *a); // input: elsewhere calculated mean, output: result a
      /** sort hits on segment such that they are ordered from larger y to smaller (from top down) */

      static bool sortHits( const InDet::TRT_DriftCircle *, const InDet::TRT_DriftCircle * );

      static void segFit(double *measx, double *measy, int nhits, double *residuals = 0, double *result = 0);

      int m_maxTotalHits; // protect against high occupancy events

      int m_minHitsForSeed; // 10-12 is good
      int m_minHitsForSegment; // 20
      int m_minHitsAboveTOT; // 15
      int m_nBinsInX; // 100
      int m_nBinsInPhi; // 10

      double m_minSeedTOT; // minimal time over threshold for seed search - default ~ 10 ns
      bool m_magneticField; // search for lines (if False) or circles (if True)
      bool m_mergeSegments; // default: False, should not be turned on yet

      bool m_useAthenaFieldService;

  };

} // end namespace InDet

#endif // TRT_TrackSegmentsMaker_BarrelCosmics_H

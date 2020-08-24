/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TRT_TrackSegments.h
//    Header file for class TRT_TrackSegmentsMaker_ECcosmics
///////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
//
// Based on TRT_TrackSegmentsMaker_CTBxk from I.Gavrilenko
///////////////////////////////////////////////////////////////////////////////
// Christian.Schmitt@cern.ch
///////////////////////////////////////////////////////////////////////////////


#ifndef TRT_TrackSegmentsMaker_ECcosmics_H
#define TRT_TrackSegmentsMaker_ECcosmics_H

#include <list>
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "InDetRecToolInterfaces/ITRT_TrackSegmentsMaker.h"

#include "StoreGate/ReadHandleKey.h"
#include "TrkEventUtils/PRDtoTrackMap.h"
#include "TrkEventUtils/EventDataBase.h"

#include "CxxUtils/checker_macros.h"
#include <iosfwd>

class MsgStream;

class TF1;

class TRT_ID;
namespace InDet{
  class TRT_DriftCircle;
  //  class TRT_DriftCircleContainer;
}

namespace InDetDD{
  class TRT_DetectorManager;
}

namespace Trk{
  class TrackSegment;
  class Segment;
  class IRIO_OnTrackCreator;
}

class PRD_MultiTruthCollection;


namespace InDet{

  /** @class TRT_TrackSegmentsMaker_ECcosmics

      AlgTool that creates TrackSegments out of TRT Driftcircles
      in the special case of cosmic runs in SR1

      @author Christian.Schmitt@cern.ch
   */
  class ATLAS_NOT_THREAD_SAFE TRT_TrackSegmentsMaker_ECcosmics : // This class read and write gErrorIgnoreLevel without a global lock for ROOT.
    virtual public ITRT_TrackSegmentsMaker, public AthAlgTool
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard tool methods
      ///////////////////////////////////////////////////////////////////

      /** Constructor with parameters */
      TRT_TrackSegmentsMaker_ECcosmics
	(const std::string&,const std::string&,const IInterface*);
      virtual ~TRT_TrackSegmentsMaker_ECcosmics();
      virtual StatusCode               initialize() override;
      virtual StatusCode               finalize  () override;

      ///////////////////////////////////////////////////////////////////
      // Methods to initialize tool for new event or region
      ///////////////////////////////////////////////////////////////////

      virtual std::unique_ptr<InDet::ITRT_TrackSegmentsMaker::IEventData> newEvent(const EventContext& ctx) const override;
      virtual std::unique_ptr<InDet::ITRT_TrackSegmentsMaker::IEventData> newRegion(const EventContext& ctx, const std::vector<IdentifierHash>&) const override;
      void endEvent(InDet::ITRT_TrackSegmentsMaker::IEventData &event_data) const override;

      ///////////////////////////////////////////////////////////////////
      // Methods of seeds production without vertex constraint
      ///////////////////////////////////////////////////////////////////

      virtual void find(const EventContext& ctx, InDet::ITRT_TrackSegmentsMaker::IEventData &event_data) const override;

      ///////////////////////////////////////////////////////////////////
      // Iterator through seeds pseudo collection produced accordingly
      // methods find    
      ///////////////////////////////////////////////////////////////////
      
      virtual Trk::TrackSegment* next(InDet::ITRT_TrackSegmentsMaker::IEventData &event_data) const override;
  
      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      virtual MsgStream&    dump          (MsgStream   & out) const override;
      virtual std::ostream& dump          (std::ostream& out) const override;

    protected:
      class EventData;
      class EventData : public Trk::EventDataBase<EventData,InDet::ITRT_TrackSegmentsMaker::IEventData>
      {
         friend class InDet::TRT_TrackSegmentsMaker_ECcosmics;
      public:
         EventData()  {
            m_segiterator   = m_segments.begin();
         }

         ~EventData() {
            std::vector<std::vector<const InDet::TRT_DriftCircle*> *>::iterator sit,sitE;
            sit= m_seeds.begin();
            sitE= m_seeds.end();
            for(;sit!=sitE;++sit){
               (*sit)->clear();
               delete(*sit);
            }
         }

      protected:
         std::list<const InDet::TRT_DriftCircle*> m_noiseHits; //!< List containing potentially noise hits
         std::list<const InDet::TRT_DriftCircle*> m_goodHits; //!< List containing potenitally good hits


         std::list<const InDet::TRT_DriftCircle*> m_sectors[2][20][16]; //!< Divide into two endcaps and each endcap into 16 sectors in phi and 20 in z

         std::vector<std::vector<const InDet::TRT_DriftCircle*> *> m_seeds; //!<Vector of seeds
         std::list<Trk::TrackSegment*>            m_segments    ; //!< List of found segments
         std::list<Trk::TrackSegment*>::iterator  m_segiterator ; //!< Iterator over found segments

         double m_a_z[1000];
         double m_a_phi[1000];
         double m_a_phi_err[1000];
         double m_a_tan[1000];
         double m_a_tan_err[1000];
         double m_a_z_err[1000];
         double m_a_steigung[1000];

         // Fitting functions
         TF1 *m_fitf_ztanphi=nullptr; //!< analytic function to fit tan(phi) vs. z
         TF1 *m_fitf_zphi=nullptr; //!< analytic function to fit phi vs. z

         TF1 *m_fitf_ztanphi_approx=nullptr; //!< approx function to fit tan(phi) vs. z
         TF1 *m_fitf_zphi_approx=nullptr; //!< anpprox function to fit phi vs. z

      };

      ///////////////////////////////////////////////////////////////////
      // Protected data and methods
      ///////////////////////////////////////////////////////////////////
      

      std::string                              m_multiTruthCollectionTRTName; //!< Name of TRT TruthCollection
      bool                                     m_phaseMode   ; //!< Switch to destinguish between phase calculation and full reco
      std::string                              m_ntrtmanager ; //!< Name of TRT det. manager 
      const TRT_ID*                            m_trtid       ; 

      SG::ReadHandleKey<InDet::TRT_DriftCircleContainer> m_trtname{this,"TRT_ClustersContainer","TRT_DriftCircles","RHK to retrieve TRT_DriftCircles"}; //!< TRTs   container 
      SG::ReadHandleKey<Trk::PRDtoTrackMap>       m_prdToTrackMap
         {this,"PRDtoTrackMap",""};

      ToolHandle<Trk::IRIO_OnTrackCreator>     m_riomakerD   ; //!< RI0_onTrack creator with drift information
      ToolHandle<Trk::IRIO_OnTrackCreator>     m_riomakerN   ; //!< RI0_onTrack creator without drift information

      bool                                     m_useDriftTime; //!< Shall the drifttime be used or only tube hits?
      bool                                     m_removeSuspicious; //!< Remove hits from segments that are suspicious
      bool                                     m_truthCollectionTRT;
      double                                   m_scaleTube   ; //!< Scalefactor for uncertainty of tube hits   
      double                                   m_scaleFactorDrift ; //!< Scalefactor for uncertainty of drifttime hits   
      double                                   m_scaleTubeNoise; //!< Scalefactor for uncertainty of tube hits flagged as noise
      double                                   m_cutToTLoose ; //!< Loose cut on ToT (preselection)
      double                                   m_cutToTTight ; //!< Hard cut on ToT (preselection)
      double                                   m_cutToTUpper ; //!< Upper cut on ToT (preselection)
      int                                      m_minDCSeed   ; //!< Minimum number of driftcircles to form a seed      
      int                                      m_hitLimit    ; //!< Maximum number of good hits (i.e. after noise cut) in endcap


       //      int m_real_counter; //!< Counter of events with enough hits in the endcap to be able to reconstruct a track
       //      int m_seg_counter; //!< Counter of events with at least one segment

      // mutable std::mutex m_counterMutex ATLAS_THREAD_SAFE;
      // mutable int m_classification ATLAS_THREAD_SAFE [3]; //!< Classification of segments: 0 - real, 1 - fake, 2 - mixture

      // mutable int m_nHits ATLAS_THREAD_SAFE [3][400];     //!< Count the numer of segments per classification and per number of hits

      //      int m_counter = 0;
      static std::mutex s_fitMutex;  // @TODO not sufficient need a global root lock

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////
      
      /** sort hits into good/noise lists
       */
      void retrieveHits(TRT_TrackSegmentsMaker_ECcosmics::EventData &event_data) const;

      /** Find seed in the given sector/zslice/endcap
       */
      bool find_seed(int endcap, int zslice, int sector,TRT_TrackSegmentsMaker_ECcosmics::EventData &event_data) const;

      /** Evaluate how many dc match this seed
       */
      int evaluate_seed(int endcap,int zslice,int sector,double *p, TRT_TrackSegmentsMaker_ECcosmics::EventData &event_data) const;

      /** Create segment out of a seed
       */
      void create_segment(std::vector<const InDet::TRT_DriftCircle*> *seed,
                          TRT_TrackSegmentsMaker_ECcosmics::EventData &event_data) const;

      /** Check if given hit originates from true particle
       */
      bool isTrueHit(const InDet::TRT_DriftCircle* dc) const;


      void setFitFunctions(TRT_TrackSegmentsMaker_ECcosmics::EventData &event_data) const;

      /** Perform the fit and return a function that provides the fitted phi information
       */
      TF1 *perform_fit(int count,TRT_TrackSegmentsMaker_ECcosmics::EventData &event_data) const;

      /** Clasify a segment as noise or real
       */
      double classify_segment(Trk::TrackSegment* seg, int &num) const;

      /** checks if a hit that matches the segment looks suspicious (i.e. isolated)
       */
      bool is_suspicious(const InDet::TRT_DriftCircle* dc, std::vector<const InDet::TRT_DriftCircle*> *seed) const;

      /** provide the proper subtraction of two phi values
       */
      static double phidiff(double a,double b);

    };

} // end of name space


inline double InDet::TRT_TrackSegmentsMaker_ECcosmics::phidiff(double a,double b)
{
  double c=a-b;

  if(fabs(c)>100)
    c/=int(c/(2*M_PI));
  while(c>=M_PI) c-=2*M_PI;
  while(c<-M_PI) c+=2*M_PI;
  return c;
}

#endif // TRT_TrackSegmentsMaker_ECcosmics_H


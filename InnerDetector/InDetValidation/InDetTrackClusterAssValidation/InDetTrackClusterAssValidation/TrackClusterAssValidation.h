/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TrackClusterAssValidation_H
#define TrackClusterAssValidation_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CxxUtils/checker_macros.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include <string>
#include <map>

namespace HepPDT{
  class ParticleDataTable;
}

namespace InDet {

  /////////////////////////////////////////////////////////////////////////////////
  //  Header file for class Barcode
  /////////////////////////////////////////////////////////////////////////////////

  class Barcode
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
    public:
      ///default c'tor
      Barcode()  {};
      ///copy c'tor defaulted
      Barcode(const Barcode&) = default;
      ///c'tor 
      Barcode(int charge,int rapidity);
      ///destructor does nothing
      ~Barcode() {};
      ///assignment defaulted
      Barcode& operator = (const Barcode&) = default;
      ///getters
      int barcode () const {return abs(m_barcharge);}
      int charge  () const {return ((m_barcharge>0) - (m_barcharge<0));} //returns 1, -1 or 0 depending on sign
      int rapidity() const {return m_rapidity;}

    protected:
      int m_barcharge;
      int m_rapidity;
    };

  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline Barcode::Barcode (int bc,int rap)
    {
      m_barcharge = bc ;
      m_rapidity  = rap;
    }

  // Class-algorithm for track cluster association validation
  //
  class TrackClusterAssValidation : public AthReentrantAlgorithm
    {

      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////

    public:

      ///////////////////////////////////////////////////////////////////
      // Standard Algotithm methods
      ///////////////////////////////////////////////////////////////////

      TrackClusterAssValidation(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~TrackClusterAssValidation() {}
      StatusCode initialize();
      StatusCode execute(const EventContext& ctx) const;
      StatusCode finalize();

    protected:

      ///////////////////////////////////////////////////////////////////
      // Protected data
      ///////////////////////////////////////////////////////////////////

      bool                               m_usePIX                 ;
      bool                               m_useSCT                 ;
      bool                               m_useTRT                 ;
      bool                               m_useOutliers            ;
      int                                m_pdg                    ;
      struct TrackCollectionStat_t {
      public:
        int                                m_efficiency   [6]  ;
        int                                m_efficiencyN  [6][5];
        int                                m_efficiencyBTE[6][5][4];
        int                                m_efficiencyPOS[6]  ;
        int                                m_efficiencyNEG[6]  ;
        int                                m_ntracksPOSB       ;
        int                                m_ntracksPOSE       ;
        int                                m_ntracksPOSDBM;
        int                                m_ntracksNEGB       ;
        int                                m_ntracksNEGE       ;
        int                                m_ntracksNEGDBM;
        int                                m_total        [50] ;
        int                                m_fake         [50] ;
        int                                m_events{}                 ;
        int                                m_eventsPOS{}              ;
        int                                m_eventsNEG{}              ;
        int                                m_eventsBTE[4]           ;

        TrackCollectionStat_t()
        : m_efficiency    {},
          m_efficiencyN   {},
          m_efficiencyBTE {},
          m_efficiencyPOS {},
          m_efficiencyNEG {},
          m_ntracksPOSB   {},
          m_ntracksPOSE   {},
          m_ntracksPOSDBM {},
          m_ntracksNEGB   {},
          m_ntracksNEGE   {},
          m_ntracksNEGDBM {},
          m_total         {},
          m_fake          {}
        {}

        TrackCollectionStat_t &operator+=(const TrackCollectionStat_t &a_stat) {
            for (int i=0; i<6; ++i) { m_efficiency[i]+=a_stat.m_efficiency[i];}
            for (int i=0; i<6; ++i) { for (int j=0; j<5; ++j) { m_efficiencyN[i][j]+=a_stat.m_efficiencyN[i][j];}}
            for (int i=0; i<6; ++i) { for (int j=0; j<5; ++j) { for (int k=0; k<4; ++k) { m_efficiencyBTE[i][j][k]+=a_stat.m_efficiencyBTE[i][j][k];} } }
            for (int i=0; i<6; ++i) { m_efficiencyPOS[i]+=a_stat.m_efficiencyPOS[i];}
            for (int i=0; i<6; ++i) { m_efficiencyNEG[i]+=a_stat.m_efficiencyNEG[i];}
            m_ntracksPOSB+=a_stat.m_ntracksPOSB       ;
            m_ntracksPOSE+=a_stat.m_ntracksPOSE       ;
            m_ntracksPOSDBM+=a_stat.m_ntracksPOSDBM;
            m_ntracksNEGB+=a_stat.m_ntracksNEGB       ;
            m_ntracksNEGE+=a_stat.m_ntracksNEGE       ;
            m_ntracksNEGDBM+=a_stat.m_ntracksNEGDBM;
            for (int i=0; i<50; ++i) { m_total[i]+=a_stat.m_total[i];}
            for (int i=0; i<50; ++i) { m_fake[i]+=a_stat.m_fake[i];}

            return *this;
        }

      };

      struct EventStat_t {
      public:
        int                                m_events      ;
        int                                m_eventsPOS   ;
        int                                m_eventsNEG   ;
        int                                m_eventsBTE[4];

        int                                m_particleClusters   [50];
        int                                m_particleSpacePoints[50];
        int                                m_particleClustersBTE   [50][4];
        int                                m_particleSpacePointsBTE[50][4];

        int                                m_nclustersPosBP         ;
        int                                m_nclustersPosBS         ;
        int                                m_nclustersPosEP         ;
        int                                m_nclustersPosES         ;
        int                                m_nclustersPosDBM;
        int                                m_nclustersNegBP         ;
        int                                m_nclustersNegBS         ;
        int                                m_nclustersNegEP         ;
        int                                m_nclustersNegES         ;
        int                                m_nclustersNegDBM;
        //
        int                                m_nclustersPTOT          ;
        int                                m_nclustersPTOTt         ;
        int                                m_nclustersSTOT          ;
        int                                m_nclustersSTOTt         ;

        EventStat_t()
        : m_events                 {},
          m_eventsPOS              {},
          m_eventsNEG              {},
          m_eventsBTE              {},
          m_particleClusters       {},
          m_particleSpacePoints    {},
          m_particleClustersBTE    {},
          m_particleSpacePointsBTE {},
          m_nclustersPosBP         {},
          m_nclustersPosBS         {},
          m_nclustersPosEP         {},
          m_nclustersPosES         {},
          m_nclustersPosDBM        {},
          m_nclustersNegBP         {},
          m_nclustersNegBS         {},
          m_nclustersNegEP         {},
          m_nclustersNegES         {},
          m_nclustersNegDBM        {},
          m_nclustersPTOT          {},
	        m_nclustersPTOTt         {},
	        m_nclustersSTOT          {},
	        m_nclustersSTOTt         {}
        {}

        EventStat_t &operator+=(const EventStat_t &a_stat) {
          m_events += a_stat.m_events;
          m_eventsPOS += a_stat.m_eventsPOS;
          m_eventsNEG += a_stat.m_eventsNEG;
          for (int i=0; i<4; ++i)  { m_eventsBTE[i] += a_stat.m_eventsBTE[i]; }
          for (int i=0; i<50; ++i) {m_particleClusters[i] += a_stat.m_particleClusters[i]; };
          for (int i=0; i<50; ++i) {m_particleSpacePoints[i] += a_stat.m_particleSpacePoints[i]; };
          for (int i=0; i<50; ++i) {for (int j=0; j<4; ++j) { m_particleClustersBTE[i][j] += a_stat.m_particleClustersBTE[i][j]; } }
          for (int i=0; i<50; ++i) {for (int j=0; j<4; ++j) { m_particleSpacePointsBTE[i][j] += a_stat.m_particleSpacePointsBTE[i][j];} }
          m_nclustersPosBP  += m_nclustersPosBP;
          m_nclustersPosBS  += m_nclustersPosBS;
          m_nclustersPosEP  += m_nclustersPosEP;
          m_nclustersPosES  += m_nclustersPosES;
          m_nclustersPosDBM += m_nclustersPosDBM;
          m_nclustersNegBP  += m_nclustersNegBP;
          m_nclustersNegBS  += m_nclustersNegBS;
          m_nclustersNegEP  += m_nclustersNegEP;
          m_nclustersNegES  += m_nclustersNegES;
          m_nclustersNegDBM += m_nclustersNegDBM;
          m_nclustersPTOT   +=a_stat.m_nclustersPTOT;
	        m_nclustersPTOTt  +=a_stat.m_nclustersPTOTt;
	        m_nclustersSTOT   +=a_stat.m_nclustersSTOT;
	        m_nclustersSTOTt  +=a_stat.m_nclustersSTOTt;
          return *this;
        }
      };

      mutable std::mutex                           m_statMutex;
      mutable std::vector<TrackCollectionStat_t>   m_trackCollectionStat ATLAS_THREAD_SAFE; // Guarded by m_statMutex
      mutable EventStat_t                          m_eventStat ATLAS_THREAD_SAFE; // Guarded by m_statMutex

      unsigned int                       m_clcut                  ;
      unsigned int                       m_clcutTRT               ;
      unsigned int                       m_spcut                  ;
      double                             m_ptcut                  ;
      double                             m_ptcutmax               ;
      double                             m_rapcut                 ;
      double                             m_tcut                   ;
      double                             m_rmin                   ;
      double                             m_rmax                   ;
      SG::ReadHandleKeyArray<TrackCollection>        m_tracklocation;
      SG::ReadHandleKey<SpacePointContainer>         m_spacepointsSCTname;
      SG::ReadHandleKey<SpacePointContainer>         m_spacepointsPixelname;
      SG::ReadHandleKey<SpacePointOverlapCollection> m_spacepointsOverlapname;
      SG::ReadHandleKey<SiClusterContainer>          m_clustersSCTname;
      SG::ReadHandleKey<SiClusterContainer>          m_clustersPixelname;
      SG::ReadHandleKey<TRT_DriftCircleContainer>    m_clustersTRTname;
      SG::ReadHandleKey<PRD_MultiTruthCollection>    m_truth_locationPixel;
      SG::ReadHandleKey<PRD_MultiTruthCollection>    m_truth_locationSCT;
      SG::ReadHandleKey<PRD_MultiTruthCollection>    m_truth_locationTRT;
      // For P->T converter of PixelClusters
      SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
      // For P->T converter of SCT_Clusters
      SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

      struct EventData_t {
      public:
        EventData_t()
        : m_nspacepoints(0),
          m_nclusters(0),
          m_nqtracks(0),
          m_nclustersTRT(0),
          m_truthPIX{},
          m_truthSCT{},
          m_truthTRT{}
        {}

        EventData_t(unsigned int n_collections)
        : m_nspacepoints(0),
          m_nclusters(0),
          m_nqtracks(0),
          m_nclustersTRT(0),
          m_truthPIX{},
          m_truthSCT{},
          m_truthTRT{}
        {
          m_particles.resize(n_collections);
          m_difference.resize(n_collections);
          m_tracks.resize(n_collections);
          m_trackCollectionStat.resize(n_collections);
        }

        int                                m_nspacepoints           ;
        int                                m_nclusters              ;
        int                                m_nqtracks               ;
        int                                m_nclustersTRT           ;

        std::vector<std::unique_ptr<SG::VarHandleBase> >  m_clusterHandles;
        std::vector<SG::ReadHandle<TrackCollection> >     m_trackcontainer;
        std::vector<SG::ReadHandle<SpacePointContainer> > m_spacePointContainer;
        std::unique_ptr<SG::ReadHandle<SpacePointOverlapCollection> > m_spacepointsOverlap;
        const PRD_MultiTruthCollection           * m_truthPIX       ;
        const PRD_MultiTruthCollection           * m_truthSCT       ;
        const PRD_MultiTruthCollection           * m_truthTRT       ;
        std::multimap<int,const Trk::PrepRawData*> m_kinecluster    ;
        std::multimap<int,const Trk::PrepRawData*> m_kineclusterTRT ;
        std::multimap<int,const Trk::SpacePoint*>  m_kinespacepoint ;
        std::vector<std::list<Barcode> >           m_particles      ;
        std::vector<std::list<int> >               m_difference     ;
        std::vector<std::multimap<int,int> >       m_tracks         ;
        std::vector<TrackCollectionStat_t>         m_trackCollectionStat;
        EventStat_t                                m_eventStat;
      };

      const HepPDT::ParticleDataTable*        m_particleDataTable ;

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

      void newSpacePointsEvent     (const EventContext& ctx, InDet::TrackClusterAssValidation::EventData_t &event_data) const;
      void newClustersEvent        (const EventContext& ctx, InDet::TrackClusterAssValidation::EventData_t &event_data) const;
      void tracksComparison        (const EventContext& ctx, InDet::TrackClusterAssValidation::EventData_t &event_data) const;
      void efficiencyReconstruction(InDet::TrackClusterAssValidation::EventData_t &event_data) const;
      bool noReconstructedParticles(const InDet::TrackClusterAssValidation::EventData_t &event_data) const;

      int qualityTracksSelection(InDet::TrackClusterAssValidation::EventData_t &event_data) const;
      int kine(const InDet::TrackClusterAssValidation::EventData_t &event_data,const Trk::PrepRawData*,const Trk::PrepRawData*,int*,int) const;
      int kine (const InDet::TrackClusterAssValidation::EventData_t &event_data,const Trk::PrepRawData*,int*,int) const;
      int kine0(const InDet::TrackClusterAssValidation::EventData_t &event_data,const Trk::PrepRawData*,int*,int) const;
      bool isTruth(const InDet::TrackClusterAssValidation::EventData_t&  ,const Trk::PrepRawData*) const;
      bool isTheSameDetElement(const InDet::TrackClusterAssValidation::EventData_t &event_data, int,const Trk::PrepRawData*) const;
      bool isTheSameDetElement(const InDet::TrackClusterAssValidation::EventData_t &event_data,int,const Trk::SpacePoint* ) const;

      PRD_MultiTruthCollection::const_iterator findTruth
      (const InDet::TrackClusterAssValidation::EventData_t &event_data,const Trk::PrepRawData*,PRD_MultiTruthCollection::const_iterator&) const;

      int charge(const InDet::TrackClusterAssValidation::EventData_t &event_data,std::pair<int,const Trk::PrepRawData*>,int&) const;

      MsgStream&    dumptools(MsgStream&    out, MSG::Level level) const;
      MsgStream&    dumpevent(MsgStream&    out, const InDet::TrackClusterAssValidation::EventData_t &event_data) const;

    };

}
#endif // TrackClusterAssValidation_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TrackClusterAssValidation_H
#define TrackClusterAssValidation_H

#include <string>
#include <map>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h" 
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "HepMC/GenParticle.h"
#include "HepPDT/ParticleDataTable.hh"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "StoreGate/ReadHandleKey.h"

#include "TrkTrack/TrackCollection.h"

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

      Barcode()  {};
      Barcode(const Barcode&);
      Barcode(int,int);
      ~Barcode() {};
      Barcode& operator = (const Barcode&);
      int barcode () const {return abs(m_barcharge);}
      int charge  () const {if(m_barcharge<0) return -1; if(m_barcharge>0) return 1; return 0;}
      int rapidity() const {return m_rapidity;}

    protected:

      int m_barcharge;
      int m_rapidity;
    };
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////
  
  inline Barcode::Barcode(const Barcode& BC)
    {
      *this = BC;
    }
  
  inline Barcode& Barcode::operator = (const Barcode& BC) 
    {
      if(&BC!=this) {
	m_barcharge = BC.m_barcharge;
	m_rapidity  = BC.m_rapidity ;
      }
      return(*this);
    }
  inline Barcode::Barcode (int bc,int rap)
    {
      m_barcharge = bc ;
      m_rapidity  = rap;
    }

  // Class-algorithm for track cluster association validation
  //
  class TrackClusterAssValidation : public AthAlgorithm 
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
      StatusCode execute();
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
      int                                m_efficiency   [100][6]  ;
      int                                m_efficiencyN  [100][6][5];
      int                                m_efficiencyBTE[100][6][5][4];
      int                                m_efficiencyPOS[100][6]  ;
      int                                m_efficiencyNEG[100][6]  ;
      int                                m_ntracksPOSB  [100]     ;
      int                                m_ntracksPOSE  [100]     ;
      int                                m_ntracksPOSDBM[100];
      int                                m_ntracksNEGB  [100]     ;
      int                                m_ntracksNEGE  [100]     ;
      int                                m_ntracksNEGDBM[100];
      int                                m_total        [100][50] ;
      int                                m_fake         [100][50] ;
      int                                m_events                 ;
      int                                m_eventsPOS              ;
      int                                m_eventsNEG              ;
      int                                m_eventsBTE[4]           ;
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
	{ }

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
        }

        int                                m_nspacepoints           ;
        int                                m_nclusters              ;
        int                                m_nqtracks               ;
        int                                m_nclustersTRT           ;

        std::vector<std::unique_ptr<SG::VarHandleBase> >  m_clusterHandles;
        std::vector<SG::ReadHandle<TrackCollection> >     m_trackcontainer;
        std::vector<SG::ReadHandle<SpacePointContainer> > m_spacePointContainer;
        std::unique_ptr<SG::ReadHandle<SpacePointOverlapCollection> > m_spacepointsOverlap;
        const PRD_MultiTruthCollection   * m_truthPIX               ;
        const PRD_MultiTruthCollection   * m_truthSCT               ;
        const PRD_MultiTruthCollection   * m_truthTRT               ;
        std::multimap<int,const Trk::PrepRawData*> m_kinecluster    ;
        std::multimap<int,const Trk::PrepRawData*> m_kineclusterTRT ;
        std::multimap<int,const Trk::SpacePoint*>  m_kinespacepoint ;
        std::vector<std::list<Barcode> >           m_particles      ;
        std::vector<std::list<int> >               m_difference     ;
        std::vector<std::multimap<int,int> >       m_tracks         ;
      };

      const HepPDT::ParticleDataTable*        m_particleDataTable ;

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

      void newSpacePointsEvent     (InDet::TrackClusterAssValidation::EventData_t &event_data) const;
      void newClustersEvent        (InDet::TrackClusterAssValidation::EventData_t &event_data) const;
      void tracksComparison        (InDet::TrackClusterAssValidation::EventData_t &event_data);
      void efficiencyReconstruction(InDet::TrackClusterAssValidation::EventData_t &event_data);
      bool noReconstructedParticles(const InDet::TrackClusterAssValidation::EventData_t &event_data) const;

      int QualityTracksSelection(InDet::TrackClusterAssValidation::EventData_t &event_data);
      int kine(const InDet::TrackClusterAssValidation::EventData_t &event_data,const Trk::PrepRawData*,const Trk::PrepRawData*,int*,int) const;
      int kine (const InDet::TrackClusterAssValidation::EventData_t &event_data,const Trk::PrepRawData*,int*,int) const;
      int kine0(const InDet::TrackClusterAssValidation::EventData_t &event_data,const Trk::PrepRawData*,int*,int) const;

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

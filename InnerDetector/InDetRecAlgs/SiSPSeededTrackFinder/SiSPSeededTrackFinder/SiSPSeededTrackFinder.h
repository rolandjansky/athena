/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef SiSPSeededTrackFinder_H
#define SiSPSeededTrackFinder_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "InDetRecToolInterfaces/ISiZvertexMaker.h"
#include "InDetRecToolInterfaces/ISiTrackMaker.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "xAODEventInfo/EventInfo.h"

// For new strategy reconstruction
#include "TrkTrack/TrackCollection.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TrkExInterfaces/IPatternParametersPropagator.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "StoreGate/DataHandle.h"

//class SpacePointContainer;
namespace InDet {

// forward declare of these interfaces does not work in opt build
//   class ISiSpacePointsSeedMaker;
//   class ISiZvertexMaker;
//   class ISiTrackMaker; 


  // Class-algorithm for track finding in Pixels and SCT
  // initiated by space points seeds
  // 
  class SiSPSeededTrackFinder : public AthAlgorithm 
    {
    
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard Algotithm methods
      ///////////////////////////////////////////////////////////////////

      SiSPSeededTrackFinder(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~SiSPSeededTrackFinder() {}
      StatusCode initialize();
      StatusCode execute();
      StatusCode finalize();


    protected:

      ///////////////////////////////////////////////////////////////////
      // Protected data 
      ///////////////////////////////////////////////////////////////////
     
      bool                           m_useZvertexTool     ; 
      bool                           m_useMBTS            ;
      bool                           m_useNewStrategy     ;
      bool                           m_useZBoundaryFinding;
      bool                           m_ITKGeometry        ; // Is it ITK geometry

      enum ECounter {kNSeeds,kNTracks,kNCounter};
      class Counter_t : public std::array<int,kNCounter> 
      {
      public:
        Counter_t & operator +=(const Counter_t &counter) {
          for (unsigned int idx=0; idx <kNCounter; ++idx) { (*this)[idx] += counter[idx];}
          return *this;
        }
      };

      /** Print internal tool parameters and status
       */
      MsgStream&    dump     (MSG::Level lvl, const SiSPSeededTrackFinder::Counter_t *) const;

      Counter_t m_counter;
      Counter_t m_counterTotal;

      int                            m_neventsTotal       ; // Number events 
      int                            m_neventsTotalV      ; // Number events 
      int                            m_problemsTotal      ; // Numbe revents with number seeds > maxNumber
      int                            m_problemsTotalV     ; // Numbe revents with number seeds > maxNumber
      int                            m_maxNumberSeeds     ; // Max. number used seeds
      int                            m_maxPIXsp           ; // Max. number pixels space points
      int                            m_maxSCTsp           ; // Max. number sct    space points
      int                            m_nfreeCut           ; // Min number free clusters

      SG::ReadHandleKey<xAOD::EventInfo>  m_evtKey;
      SG::ReadHandle<SpacePointContainer> m_SpacePointsSCT  ;
      SG::ReadHandle<SpacePointContainer> m_SpacePointsPixel;
      SG::WriteHandle<TrackCollection>    m_outputTracks    ;

      ToolHandle< ISiSpacePointsSeedMaker > m_seedsmaker    ;  // Space poins seed     maker
      ToolHandle< ISiZvertexMaker         > m_zvertexmaker  ;  // Space poins z-vertex maker
      ToolHandle< ISiTrackMaker           > m_trackmaker    ;  // Track                maker     
    
      // For new strategy reconstruction
      //
      int*                           m_nhistogram;
      int                            m_histsize  ;
      int                            m_nvertex   ;
      double                         m_pTcut     ;
      double                         m_imcut     ;
      double                         m_zstep     ;
      double                         m_zcut      ;
      double*                        m_zhistogram;
      double*                        m_phistogram;

      std::string                    m_beamconditions          ;
      std::string                    m_fieldmode               ; 
      IBeamCondSvc*                                 m_beam     ;
      ToolHandle<Trk::IPatternParametersPropagator> m_proptool ;
      Trk::MagneticFieldProperties                  m_fieldprop;

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////
      
      bool isGoodEvent();
      double trackQuality(const Trk::Track*);
      void filterSharedTracks(std::multimap<double,Trk::Track*>&);
      void fillZHistogram(const Trk::Track*,Trk::PerigeeSurface&);
      void findZvertex(std::list<Trk::Vertex>&,double*); 
      StatusCode  oldStrategy();
      StatusCode  newStrategy();
      void magneticFieldInit();

      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out, const SiSPSeededTrackFinder::Counter_t &counter) const;

    };
  //  MsgStream&    operator << (MsgStream&   ,const SiSPSeededTrackFinder&);
}
#endif // SiSPSeededTrackFinder_H

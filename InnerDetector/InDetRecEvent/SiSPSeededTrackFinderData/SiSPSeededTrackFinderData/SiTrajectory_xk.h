/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiTrajectory_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for trajectory in Pixels and SCT
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiTrajector_xk_H
#define SiTrajector_xk_H

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "SiSPSeededTrackFinderData/SiTools_xk.h"
#include "SiSPSeededTrackFinderData/SiTrajectoryElement_xk.h"
#include "SiSPSeededTrackFinderData/SiDetElementBoundaryLink_xk.h"

#include <map>

namespace InDet{

  class SiTrajectory_xk
    {
      friend class  SiCombinatorialTrackFinder_xk;

      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      SiTrajectory_xk();
      SiTrajectory_xk(const SiTrajectory_xk&);
      ~SiTrajectory_xk() = default;
      SiTrajectory_xk& operator  = (const SiTrajectory_xk&);

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      const int&  nholes        ()       const {return m_nholes        ;}
      const int&  dholes        ()       const {return m_dholes        ;}
      const int&  nHolesBefore       ()       const {return m_nHolesBefore       ;}
      const int&  nHolesAfter       ()       const {return m_nHolesAfter       ;}
      const int&  nclusters     ()       const {return m_nclusters     ;}
      const int&  ndf           ()       const {return m_ndf           ;}
      const int&  nclustersNoAdd()       const {return m_nclustersNoAdd;}
      const int&  nElements     ()       const {return m_nElements     ;}
      const int&  naElements    ()       const {return m_nActiveElements    ;}
      const int&  difference    ()       const {return m_difference    ;}
      const int&  elementsMap(int& i) const {return m_elementsMap[i];}

      void setTools(const InDet::SiTools_xk*); 
      void setParameters(); 

      bool initialize
        (bool,bool,
         const PixelClusterContainer*                         ,
         const SCT_ClusterContainer*                          ,
         const Trk::TrackParameters                          &,
         std::list<const InDet::SiCluster*>                  &,
         std::vector<const InDet::SiDetElementBoundaryLink_xk*>&,
         bool                                                &,
         const EventContext&);

      bool trackParametersToClusters
        (const PixelClusterContainer*                             ,
         const SCT_ClusterContainer*                              ,
         const Trk::TrackParameters                              &,
         std::vector<const InDet::SiDetElementBoundaryLink_xk*>  &,
         std::multimap<const Trk::PrepRawData*,const Trk::Track*>&,
         std::list<const InDet::SiCluster*>                      &);
      
      bool globalPositionsToClusters
        (const PixelClusterContainer*                             ,
         const SCT_ClusterContainer*                              ,
         const std::list<Amg::Vector3D>                          &,
         std::vector<const InDet::SiDetElementBoundaryLink_xk*>  &,
         std::multimap<const Trk::PrepRawData*,const Trk::Track*>&,
         std::list<const InDet::SiCluster*>                      &);

      bool backwardExtension(int);
      bool forwardExtension (bool,int);
      bool forwardFilter    ();
      bool backwardSmoother (bool);
      bool isLastPixel      ();
      const Trk::TrackParameters* firstTrackParameters();
      void getClusters(std::list<const InDet::SiCluster*>&);

      DataVector<const Trk::TrackStateOnSurface>* convertToTrackStateOnSurface();
      DataVector<const Trk::TrackStateOnSurface>* convertToTrackStateOnSurface(int);
      DataVector<const Trk::TrackStateOnSurface>* convertToTrackStateOnSurfaceWithNewDirection();
      DataVector<const Trk::TrackStateOnSurface>* convertToNextTrackStateOnSurface();

      DataVector<const Trk::TrackStateOnSurface>* convertToSimpleTrackStateOnSurface();
      DataVector<const Trk::TrackStateOnSurface>* convertToSimpleTrackStateOnSurface(int);
      DataVector<const Trk::TrackStateOnSurface>* convertToSimpleTrackStateOnSurfaceWithNewDirection();

      Trk::FitQuality* convertToFitQuality();

      void sortStep          ();
      bool goodOrder         ();
      bool jumpThroughPerigee();
      double quality         () const;
      double qualityOptimization();
      double pTfirst         ();
      std::ostream& dump(std::ostream& out) const;

    protected:
      
      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      int                               m_firstElement    ; /// index of the first element where we have 
                                                            /// a cluster 
      int                               m_lastElement     ; /// index of the last element where we have 
                                                            /// a cluster
      int                               m_nclusters       ; /// Number of clusters on trajectory
      int                               m_nclustersNoAdd  ; // (NCL)
      int                               m_difference      ; // forward-bacward diff 
      int                               m_nHolesBefore         ; // holes before
      int                               m_nHolesAfter         ; // holes after
      int                               m_nholes          ; // holes
      int                               m_dholes          ; // dholes
      int                               m_nActiveElements      ; /// count active elements 
      int                               m_nElements       ; // index 
      int                               m_elementsMap[300]; // index
      int                               m_ndfcut          ; //
      int                               m_ndf             ; //
      int                               m_ntos            ; //
      int                               m_atos[100]       ; //
      int                               m_itos[100]       ; //
      SiTrajectoryElement_xk            m_elements   [300]; /// Trajectory elements on this trajectory. 
                                                            /// Each one corresponds to one detector element on
                                                            /// the search road 
      const InDet::SiTools_xk*          m_tools           ; //

      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////

      void erase(int);
      bool isNewTrack(std::multimap<const Trk::PrepRawData*,const Trk::Track*>&);
    };
  
  std::ostream& operator << (std::ostream&,const SiTrajectory_xk&);

} // end of name space

#include "SiSPSeededTrackFinderData/SiTrajectory_xk.icc"

#endif // SiTrajectory_xk

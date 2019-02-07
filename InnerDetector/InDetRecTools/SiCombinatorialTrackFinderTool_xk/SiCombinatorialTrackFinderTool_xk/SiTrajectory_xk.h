/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include <map>

#include "InDetPrepRawData/SiClusterContainer.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "SiCombinatorialTrackFinderTool_xk/SiTools_xk.h"
#include "SiCombinatorialTrackFinderTool_xk/SiTrajectoryElement_xk.h"
#include "SiCombinatorialTrackFinderTool_xk/SiDetElementBoundaryLink_xk.h"

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
      ~SiTrajectory_xk();
      SiTrajectory_xk& operator  = (const SiTrajectory_xk&);

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      const int&  nholes        ()       const {return m_nholes        ;}
      const int&  dholes        ()       const {return m_dholes        ;}
      const int&  nholesb       ()       const {return m_nholesb       ;}
      const int&  nholese       ()       const {return m_nholese       ;}
      const int&  nclusters     ()       const {return m_nclusters     ;}
      const int&  ndf           ()       const {return m_ndf           ;}
      const int&  nclustersNoAdd()       const {return m_nclustersNoAdd;}
      const int&  nElements     ()       const {return m_nElements     ;}
      const int&  naElements    ()       const {return m_naElements    ;}
      const int&  difference    ()       const {return m_difference    ;}
      const int&  elementsMap(int& i) const {return m_elementsMap[i];}

      void setTools(const InDet::SiTools_xk*); 
      void setParameters(); 

      bool initialize
	(const InDet::SiClusterContainer*                    ,
	 const InDet::SiClusterContainer*                    ,
	 const Trk::TrackParameters                          &,
	 std::vector<const InDet::SiCluster*>                  &,
	 std::vector<const InDet::SiDetElementBoundaryLink_xk*>&,
	 bool                                                &);
 
      double pTseed
	(const Trk::TrackParameters                          &,
	 std::vector<const InDet::SiCluster*>                &,
	 std::vector<const InDet::SiDetElementBoundaryLink_xk*>&);


      bool trackParametersToClusters
	(const InDet::SiClusterContainer*                        ,
	 const InDet::SiClusterContainer*                        ,
	 const Trk::TrackParameters                              &,
	 std::vector<const InDet::SiDetElementBoundaryLink_xk*>    &,
	 std::multimap<const Trk::PrepRawData*,const Trk::Track*>&,
	 std::vector<const InDet::SiCluster*>                      &);
      
      bool globalPositionsToClusters
 	(const InDet::SiClusterContainer*                        ,
	 const InDet::SiClusterContainer*                        ,
	 const std::list<Amg::Vector3D>                          &,
	 std::vector<const InDet::SiDetElementBoundaryLink_xk*>    &,
	 std::multimap<const Trk::PrepRawData*,const Trk::Track*>&,
	 std::vector<const InDet::SiCluster*>                      &); 

      bool backwardExtension(int);
      bool forwardExtension (bool,int);
      bool forwardExtensionDefault (bool,int);
      bool forwardExtensionTracklet (bool,int);
      bool forwardFilter    ();
      bool backwardSmoother (bool);
      bool isLastPixel      (); 
      const Trk::TrackParameters* firstTrackParameters();
      void getClusters(std::vector<const InDet::SiCluster*>&);

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

      int                               m_firstElement    ;
      int                               m_lastElement     ;
      int                               m_nclusters       ; // (NCL)
      int                               m_nclustersNoAdd  ; // (NCL)
      int                               m_difference      ; // forward-bacward diff 
      int                               m_nholesb         ; // holes before
      int                               m_nholese         ; // holes after
      int                               m_nholes          ; // holes
      int                               m_dholes          ; // dholes
      int                               m_naElements      ; // 
      int                               m_nElements       ; // nindex
      int                               m_elementsMap[300]; // index
      int                               m_ndfcut          ; //
      int                               m_ndf             ; //
      int                               m_ntos            ; //
      int                               m_atos[100]       ; //
      int                               m_itos[100]       ; //
      SiTrajectoryElement_xk            m_elements   [300]; // mC
      const InDet::SiTools_xk*          m_tools           ; //

      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////

      void erase(int);
      bool isNewTrack(std::multimap<const Trk::PrepRawData*,const Trk::Track*>&);
    };
  
  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline SiTrajectory_xk::SiTrajectory_xk()
    {
      m_nElements      = 0 ;
      m_tools          = 0 ;
      m_firstElement   = 0 ;
      m_lastElement    = 0 ;
      m_nclusters      = 0 ;
      m_nclustersNoAdd = 0 ;
      m_difference     = 0 ;
      m_nholesb        = 0 ;
      m_nholese        = 0 ; 
      m_nholes         = 0 ;
      m_dholes         = 0 ;
      m_naElements     = 0 ;
      m_ndfcut         = 0 ;
      m_ndf            = 0 ;
      m_ntos           = 0 ;
    }

  inline SiTrajectory_xk::SiTrajectory_xk(const SiTrajectory_xk& T)
    {
      *this = T;
    }
  
  inline SiTrajectory_xk& SiTrajectory_xk::operator = 
    (const SiTrajectory_xk& T) 
    {
      m_firstElement     = T.m_firstElement   ;
      m_lastElement      = T.m_lastElement    ;
      m_nclusters        = T.m_nclusters      ; 
      m_ndfcut           = T.m_ndfcut         ;
      m_ndf              = T.m_ndf            ;
      m_ntos             = T.m_ntos           ;
      m_nclustersNoAdd   = T.m_nclustersNoAdd ; 
      m_nholesb          = T.m_nholesb        ;
      m_nholese          = T.m_nholese        ;
      m_nholes           = T.m_nholes         ;
      m_dholes           = T.m_dholes         ;
      m_naElements       = T.m_naElements     ;
      m_nElements        = T.m_nElements      ;
      m_tools            = T.m_tools          ;

      for(int i=0; i!=m_nElements; ++i) {
	int            e =  T.m_elementsMap[i];
	m_elementsMap[i] = e                  ;
	m_elements   [e] = T.m_elements[e]    ; 
      }
      for(int i=0; i!=m_ntos; ++i) {
	m_atos[i] = T.m_atos[i];
	m_itos[i] = T.m_itos[i];
      }
      return(*this);
    }

  inline SiTrajectory_xk::~SiTrajectory_xk() {}

  inline bool SiTrajectory_xk::isLastPixel()
    {
      if(m_elements[m_elementsMap[m_lastElement]].ndf()==2) return true;
      return false;
    } 

  inline const Trk::TrackParameters* SiTrajectory_xk::firstTrackParameters()
    {
      return m_elements[m_elementsMap[m_firstElement]].trackParameters(false,1);
    }

  std::ostream& operator << (std::ostream&,const SiTrajectory_xk&); 

} // end of name space

#endif // SiTrajectory_xk



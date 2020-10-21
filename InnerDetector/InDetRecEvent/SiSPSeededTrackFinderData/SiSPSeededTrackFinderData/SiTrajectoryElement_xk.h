/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiTrajectoryElement_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for trajector elements in Pixels and SCT
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiTrajectoryElement_xk_H
#define SiTrajectoryElement_xk_H

#include "InDetPrepRawData/SiClusterCollection.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "SiSPSeededTrackFinderData/SiClusterLink_xk.h"
#include "SiSPSeededTrackFinderData/SiDetElementBoundaryLink_xk.h"
#include "SiSPSeededTrackFinderData/SiTools_xk.h"
#include "TrkPatternParameters/PatternTrackParameters.h"
#include "TrkPatternParameters/NoiseOnSurface.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkEventUtils/PRDtoTrackMap.h"
// MagField cache
#include "MagFieldElements/AtlasFieldCache.h"

#include <any>

namespace InDet{

  class SiTrajectoryElement_xk
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      SiTrajectoryElement_xk();
      SiTrajectoryElement_xk(const SiTrajectoryElement_xk&);
      ~SiTrajectoryElement_xk() = default;
      SiTrajectoryElement_xk& operator  = (const SiTrajectoryElement_xk&);

      const int&  detstatus   () const {return m_detstatus;}
      const int&  inside      () const {return m_inside;    }
      const int&  ndist       () const {return m_ndist ;    }
      const int&  nlinksF     () const {return m_nlinksF;   }
      const int&  nlinksB     () const {return m_nlinksB;   }
      const int&  nholesF     () const {return m_nholesF;   }
      const int&  nholesB     () const {return m_nholesB;   }
      const int&  dholesF     () const {return m_dholesF;   }
      const int&  dholesB     () const {return m_dholesB;   }
      const int&  nclustersF  () const {return m_nclustersF;}
      const int&  nclustersB  () const {return m_nclustersB;}
      const int&  npixelsB    () const {return m_npixelsB;  }
      const bool& stereo      () const {return m_stereo;    }
      const int&  status      () const {return m_status;    }
      const int&  noiseModel  () const {return m_noisemodel;}
      const int&  ndf         () const {return m_ndf;       }
      const int&  ndfF        () const {return m_ndfF;      }
      const int&  ndfB        () const {return m_ndfB;      }
      const int&  ntsos       () const {return m_ntsos;     }
      const double& step      () const {return m_step;      }
      const double& dist      () const {return m_dist;      }
      bool        isBarrel    () const {return m_detelement->isBarrel();}

      const InDetDD::SiDetectorElement* detElement  () const {return m_detelement;  }  
      const InDet::SiCluster*           cluster     () const {return m_cluster   ;  }  
      const InDet::SiCluster*           clusterOld  () const {return m_clusterOld;  }  
      const InDet::SiCluster*           clusterNoAdd() const {return m_clusterNoAdd;} 
      Trk::TrackStateOnSurface*         tsos (int i);

      bool isNextClusterHoleB(bool&,double&);
      bool isNextClusterHoleF(bool&,double&);

      ///////////////////////////////////////////////////////////////////
      /// @name Methods update with cluster information
      ///////////////////////////////////////////////////////////////////
      //@{ 
      bool addCluster
	(Trk::PatternTrackParameters&,Trk::PatternTrackParameters&,double&);
      bool addCluster
	(Trk::PatternTrackParameters&,Trk::PatternTrackParameters&);

      bool combineStates
	(Trk::PatternTrackParameters&,
	 Trk::PatternTrackParameters&,
	 Trk::PatternTrackParameters&);
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Methods noise calculation
      ///////////////////////////////////////////////////////////////////
      //@{
      void noiseProduction
	(int,const Trk::PatternTrackParameters&);
      void noiseInitiate();

      bool addNextClusterB();
      bool addNextClusterF();
      bool addNextClusterB(SiTrajectoryElement_xk&,const InDet::SiCluster*);
      bool addNextClusterF(SiTrajectoryElement_xk&,const InDet::SiCluster*);
      void setCluster(const InDet::SiCluster*);

      void setParametersB(Trk::PatternTrackParameters&); 
      void setParametersF(Trk::PatternTrackParameters&); 
      void setNdist(int);

      int   numberClusters() const;
      const double& xi2F      () const {return m_xi2F      ;}
      const double& xi2B      () const {return m_xi2B      ;}
      const double& xi2totalF () const {return m_xi2totalF ;}
      const double& xi2totalB () const {return m_xi2totalB ;}
      const double& radlength () const {return m_radlength ;}
      const double& energylose() const {return m_energylose;}

      const Trk::PatternTrackParameters& parametersPF() const {return m_parametersPF;} 
      const Trk::PatternTrackParameters& parametersUF() const {return m_parametersUF;} 
      const Trk::PatternTrackParameters& parametersPB() const {return m_parametersPB;} 
      const Trk::PatternTrackParameters& parametersUB() const {return m_parametersUB;} 
      const Trk::PatternTrackParameters& parametersSM() const {return m_parametersSM;} 

      const Trk::Surface*  surface()  const {return m_surface;}
      const InDet::SiClusterLink_xk& linkF (int i) const {return m_linkF[i];}
      const InDet::SiClusterLink_xk& linkB (int i) const {return m_linkB[i];}
      //@}
 
      ///////////////////////////////////////////////////////////////////
      /// @name Main methods
      ///////////////////////////////////////////////////////////////////
      //@{

      /**
       * T = InDet::SiClusterCollection::const_iterator or
       *     InDet::PixelClusterCollection::const_iterator or
       *     InDet::SCT_ClusterCollection::const_iterator
       */
      template <typename T>
      void set(int st,
	       const InDet::SiDetElementBoundaryLink_xk*& dl,
	       const T& sb,
	       const T& se,
	       const InDet::SiCluster* si,
	       const EventContext& ctx);

      void setTools(const InDet::SiTools_xk*); 
      void setParameters(); 
      void bremNoiseModel();
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Update first trajectory element on the trajectory with
      /// track parameters
      ///////////////////////////////////////////////////////////////////
      //@{
      bool firstTrajectorElement(const Trk::TrackParameters&);
      bool firstTrajectorElement();
      //@}

      ///////////////////////////////////////////////////////////////////
      // @name Forward propagation  
      ///////////////////////////////////////////////////////////////////
      //@{
      bool ForwardPropagationWithoutSearch(SiTrajectoryElement_xk&);
      bool ForwardPropagationWithSearch(SiTrajectoryElement_xk&);
      /**
       * T = InDet::SiClusterCollection::const_iterator or
       *     InDet::PixelClusterCollection::const_iterator or
       *     InDet::SCT_ClusterCollection::const_iterator
       */
      template <typename T>
        bool ForwardPropagationForClusterSeach
	(int n,
         const Trk::TrackParameters& Tpa,
	 const InDet::SiDetElementBoundaryLink_xk*& dl,
	 const T& sb,
	 const T& se);
      //@}

      /**
       * T = InDet::SiClusterCollection::const_iterator or
       *     InDet::PixelClusterCollection::const_iterator or
       *     InDet::SCT_ClusterCollection::const_iterator
       */
      template <typename T>
        void CloseClusterSeach
	(Trk::PatternTrackParameters& Tpa,
	 const InDet::SiDetElementBoundaryLink_xk*& dl,
	 const T& sb, 
	 const T& se);
      
      void eraseClusterForwardPropagation();

      ///////////////////////////////////////////////////////////////////
      /// @name For last detector element initiate smoother  
      ///////////////////////////////////////////////////////////////////
      //@{
      bool lastTrajectorElement();
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Backward propagation for smoother  
      ///////////////////////////////////////////////////////////////////
      //@{
      bool BackwardPropagationSmoother(SiTrajectoryElement_xk&,bool);
      bool BackwardPropagationFilter  (SiTrajectoryElement_xk&);
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Search clusters compatible with track  
      ///////////////////////////////////////////////////////////////////
      //@{
      int searchClusters                (Trk::PatternTrackParameters&,SiClusterLink_xk*);
      template <typename T> 
        int searchClustersSub             (Trk::PatternTrackParameters&,SiClusterLink_xk*);

      template <typename T>
        int searchClustersWithoutStereoPIX(Trk::PatternTrackParameters&,SiClusterLink_xk*);
      template <typename T>
        int searchClustersWithoutStereoSCT(Trk::PatternTrackParameters&,SiClusterLink_xk*);
      template <typename T>
        int searchClustersWithStereo      (Trk::PatternTrackParameters&,SiClusterLink_xk*);
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Search only clusters without track assosiation compatible with track  
      ///////////////////////////////////////////////////////////////////
      //@{
      template <typename T>
        int searchClustersWithoutStereoAssPIX(Trk::PatternTrackParameters&,SiClusterLink_xk*, const Trk::PRDtoTrackMap &);
      template <typename T>
        int searchClustersWithoutStereoAssSCT(Trk::PatternTrackParameters&,SiClusterLink_xk*, const Trk::PRDtoTrackMap &);
      template <typename T>
        int searchClustersWithStereoAss      (Trk::PatternTrackParameters&,SiClusterLink_xk*, const Trk::PRDtoTrackMap &);
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Is difference between forward and backward propagation   
      ///////////////////////////////////////////////////////////////////
      //@{
      bool difference() const;
      //@}

      ///////////////////////////////////////////////////////////////////
      // @name TrackStateOnSurface production  
      ///////////////////////////////////////////////////////////////////
      //@{
      Trk::TrackStateOnSurface* trackStateOnSurface(bool,bool,bool,int);
      Trk::TrackStateOnSurface* trackSimpleStateOnSurface(bool,bool,int);
      Trk::TrackStateOnSurface* trackPerigeeStateOnSurface();
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name TrackParameters production  
      ///////////////////////////////////////////////////////////////////
      //@{
      const Trk::TrackParameters* trackParameters(bool,int);
      const Trk::TrackParameters* trackParametersWithNewDirection(bool,int);
      const Trk::TrackParameters* trackParameters(Trk::PatternTrackParameters&,bool);
      //@}

      ///////////////////////////////////////////////////////////////////
      // @name Initiate state
      ///////////////////////////////////////////////////////////////////
      //@{
      bool initiateState(Trk::PatternTrackParameters&,Trk::PatternTrackParameters&);
      //@}

      ///////////////////////////////////////////////////////////////////
      // @name Last detector elements with clusters
      ///////////////////////////////////////////////////////////////////
      //@{
      void lastActive();
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Step of trajectory calculation
      ///////////////////////////////////////////////////////////////////
      //@{
      double step(SiTrajectoryElement_xk&);
      double stepToPerigee();
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Global position of the track parameters
      ///////////////////////////////////////////////////////////////////
      //@{
      Amg::Vector3D globalPosition();
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Quality of the trajectory element
      ///////////////////////////////////////////////////////////////////
      //@{
      double quality(int&) const;
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Propagate parameters with covariance
      ///////////////////////////////////////////////////////////////////
      //@{
      bool propagate
	(Trk::PatternTrackParameters  &,
	 Trk::PatternTrackParameters  &,
	 double                       &);
      //@}

      ////////////////////////////////////////////////////////////////////
      /// @name Propagate parameters without covariance
      ///////////////////////////////////////////////////////////////////
      //@{
      bool propagateParameters
	(Trk::PatternTrackParameters&,
	 Trk::PatternTrackParameters&,
	 double                     &);
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Work methods for propagation
      ///////////////////////////////////////////////////////////////////
      //@{
      void transformPlaneToGlobal
	(bool,Trk::PatternTrackParameters&,double*);
      bool transformGlobalToPlane
	(bool,double*,Trk::PatternTrackParameters&,Trk::PatternTrackParameters&);
      bool rungeKuttaToPlane
	(bool,double*);
      bool straightLineStepToPlane
	(bool,double*);
      //@}

    private:
      
      ///////////////////////////////////////////////////////////////////
      /// Private Data
      ///////////////////////////////////////////////////////////////////
 
      enum IteratorType {
        SiClusterColl = 0,
        PixelClusterColl = 1,
        SCT_ClusterColl = 2,
        Invalid = 3
      };

      bool                                        m_stereo      ;
      bool                                        m_utsos[3]    ;
      bool                                        m_fieldMode   ;
      bool                                        m_useassoTool = false ;
      int                                         m_status      ;  
      int                                         m_detstatus   ; //!< 0 (no clusters) 
      int                                         m_inside      ;
      int                                         m_ndist       ;
      int                                         m_nlinksF     ;
      int                                         m_nlinksB     ;
      int                                         m_nholesF     ;
      int                                         m_nholesB     ;
      int                                         m_dholesF     ;
      int                                         m_dholesB     ;
      int                                         m_nclustersF  ;
      int                                         m_nclustersB  ;
      int                                         m_npixelsB    ;
      int                                         m_noisemodel  ;
      int                                         m_ndf         ;
      int                                         m_ndfF        ;
      int                                         m_ndfB        ;
      int                                         m_ntsos       ;
      int                                         m_maxholes    ;
      int                                         m_maxdholes   ;
      double                                      m_dist        ;
      double                                      m_xi2F        ;
      double                                      m_xi2B        ;
      double                                      m_xi2totalF   ;
      double                                      m_xi2totalB   ;
      double                                      m_radlength   ;
      double                                      m_radlengthN  ;
      double                                      m_energylose  ;
      double                                      m_halflength  ;
      double                                      m_step        ;
      double                                      m_xi2max      ;
      double                                      m_xi2maxNoAdd ;
      double                                      m_xi2maxlink  ;  
      double                                      m_xi2multi    ;
      double                                      m_Tr[13]      ;
      double                                      m_A [ 3]      ;

      const InDetDD::SiDetectorElement*           m_detelement  ;
      const InDet::SiDetElementBoundaryLink_xk*   m_detlink     ;
      const Trk::Surface*                         m_surface     ;
      /**
       * @name Data members using std::any
       * std::any is used to cover 
       * - InDet::PixelClusterCollection::const_iterator
       * - InDet::SCT_ClusterCollection::const_iterator
       * - InDet::SiClusterCollection::const_iterator
       * used in SiTrajectoryElement_xk.cxx.
       */
      //@{
      std::any                                    m_sibegin     ;
      std::any                                    m_siend       ;
      //@}
      const InDet::SiCluster*                     m_cluster     ;
      const InDet::SiCluster*                     m_clusterOld  ;
      const InDet::SiCluster*                     m_clusterNoAdd;
      Trk::PatternTrackParameters                 m_parametersPF; 
      Trk::PatternTrackParameters                 m_parametersUF; 
      Trk::PatternTrackParameters                 m_parametersPB; 
      Trk::PatternTrackParameters                 m_parametersUB;
      Trk::PatternTrackParameters                 m_parametersSM;
      InDet::SiClusterLink_xk                     m_linkF[10]   ; 
      InDet::SiClusterLink_xk                     m_linkB[10]   ; 
      Trk::NoiseOnSurface                         m_noise       ; 
      const InDet::SiTools_xk*                    m_tools       ; 
      MagField::AtlasFieldCache                   m_fieldCache;
      const Trk::IPatternParametersUpdator*       m_updatorTool ;
      const Trk::IPatternParametersPropagator*    m_proptool    ;
      const Trk::IRIO_OnTrackCreator*             m_riotool     ;
      const Trk::PRDtoTrackMap                   *m_prdToTrackMap;
      Trk::TrackStateOnSurface*                   m_tsos[3]     ;
      Amg::MatrixX                                m_covariance  ;

      IteratorType m_itType{Invalid};

      ///////////////////////////////////////////////////////////////////
      /// Private Methods
      ///////////////////////////////////////////////////////////////////
      
      void patternCovariances(const InDet::SiCluster*,double&,double&,double&);
    };
  
} // end of name space

#include "SiSPSeededTrackFinderData/SiTrajectoryElement_xk.icc"

#endif // SiTrajectoryElement_xk

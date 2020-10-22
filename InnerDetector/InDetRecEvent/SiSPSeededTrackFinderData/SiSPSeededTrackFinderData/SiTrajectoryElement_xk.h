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
      const int&  nlinksF     () const {return m_nlinksForward;   }
      const int&  nlinksB     () const {return m_nlinksBackward;   }
      const int&  nholesF     () const {return m_nholesForward;   }
      const int&  nholesB     () const {return m_nholesBackward;   }
      const int&  dholesF     () const {return m_dholesForward;   }
      const int&  dholesB     () const {return m_dholesBackward;   }
      const int&  nclustersF  () const {return m_nclustersForward;}
      const int&  nclustersB  () const {return m_nclustersBackward;}
      const int&  npixelsB    () const {return m_npixelsBackward;  }
      const bool& stereo      () const {return m_stereo;    }
      const int&  status      () const {return m_status;    }
      const int&  noiseModel  () const {return m_noisemodel;}
      const int&  ndf         () const {return m_ndf;       }
      const int&  ndfF        () const {return m_ndfForward;      }
      const int&  ndfB        () const {return m_ndfBackward;      }
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
      const double& xi2F      () const {return m_xi2Forward      ;}
      const double& xi2B      () const {return m_xi2Backward      ;}
      const double& xi2totalF () const {return m_xi2totalForward ;}
      const double& xi2totalB () const {return m_xi2totalBackward ;}
      const double& radlength () const {return m_radlength ;}
      const double& energylose() const {return m_energylose;}
      /// track parameters for forward filter / smoother 
      /// predicted
      const Trk::PatternTrackParameters& parametersPF() const {return m_parametersPredForward;} 
      /// updated
      const Trk::PatternTrackParameters& parametersUF() const {return m_parametersUpdatedForward;} 
      /// predicted
      const Trk::PatternTrackParameters& parametersPB() const {return m_parametersPredBackward;} 
      /// observed
      const Trk::PatternTrackParameters& parametersUB() const {return m_parametersUpdatedBackward;} 
      const Trk::PatternTrackParameters& parametersSM() const {return m_parametersSM;} 

      const Trk::Surface*  surface()  const {return m_surface;}
      const InDet::SiClusterLink_xk& linkF (int i) const {return m_linkForward[i];}
      const InDet::SiClusterLink_xk& linkB (int i) const {return m_linkBackward[i];}
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

      /// check for a difference between forward and back propagation
      bool difference() const;

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
      /// inputPars: input parameters. 
      /// outputPars: output parameters. 
      /// Gives outputPars the x and (for pix) the y of the associated cluster, and the corresponding cov elements, 
      /// and copies all other elements else from inputPars
      ///////////////////////////////////////////////////////////////////
      //@{
      bool initiateState(Trk::PatternTrackParameters& inputPars,Trk::PatternTrackParameters& outputPars);
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
      /// Will propagate the startingParameters from their reference 
      /// to the surface associated with this element and write the result into 
      /// the second argument. The third argument records the step length traversed. 
      bool propagate(Trk::PatternTrackParameters  & startingParameters,
                     Trk::PatternTrackParameters  & outParameters,
                     double                       & step);
      //@}

      ////////////////////////////////////////////////////////////////////
      /// @name Propagate parameters without covariance update.
      /// Will propagate the startingParameters from their reference 
      /// to the surface associated with this element and write the result into 
      /// the second argument. The third argument records the step length traversed. 
      ///////////////////////////////////////////////////////////////////
      //@{
      bool propagateParameters(Trk::PatternTrackParameters& startingParameters,
                               Trk::PatternTrackParameters& outParameters,
                               double                     & step);
      //@}

      ///////////////////////////////////////////////////////////////////
      /// @name Work methods for propagation
      ///////////////////////////////////////////////////////////////////
      //@{

        
      /////////////////////////////////////////////////////////////////////////////////
      /// Tramsform from plane to global
      /// Will take the surface and parameters from localParameters
      /// and populate globalPars with the result. 
      /// globalPars needs to be at least 46 elements long. 
      /// Convention: 
      ///                       /dL0    /dL1    /dPhi   /dThe   /dCM
      ///    X  ->globalPars[0]  dX /   globalPars[ 7]   globalPars[14]   globalPars[21]   globalPars[28]   globalPars[35]  
      ///    Y  ->globalPars[1]  dY /   globalPars[ 8]   globalPars[15]   globalPars[22]   globalPars[29]   globalPars[36]  
      ///    Z  ->globalPars[2]  dZ /   globalPars[ 9]   globalPars[16]   globalPars[23]   globalPars[30]   globalPars[37]   
      ///    Ax ->globalPars[3]  dAx/   globalPars[10]   globalPars[17]   globalPars[24]   globalPars[31]   globalPars[38]  
      ///    Ay ->globalPars[4]  dAy/   globalPars[11]   globalPars[18]   globalPars[25]   globalPars[32]   globalPars[39]  
      ///    Az ->globalPars[5]  dAz/   globalPars[12]   globalPars[19]   globalPars[26]   globalPars[33]   globalPars[40]  
      ///    CM ->globalPars[6]  dCM/   globalPars[13]   globalPars[20]   globalPars[27]   globalPars[34]   globalPars[41] 
      ///    + dA/dS (42-44) and step (45) 
      // /////////////////////////////////////////////////////////////////////////////////
      void transformPlaneToGlobal(bool,
                                  Trk::PatternTrackParameters& localParameters,
                                  double* globalPars);

  
      /////////////////////////////////////////////////////////////////////////////////
      /// Tramsform from global to plane surface
      /// Will take the global parameters in globalPars, the surface from this trajectory
      /// element and populate outputParameters with the result. 
      /// If covariance update ("updateJacobian") is requested, also transport the covariance
      /// from "startingParameters" using the jacobian stored in the globalPars and write this 
      /// into outputParameters. 
      /// Convention for globalPars: 
      ///                       /dL0    /dL1    /dPhi   /dThe   /dCM
      ///    X  ->globalPars[0]  dX /   globalPars[ 7]   globalPars[14]   globalPars[21]   globalPars[28]   globalPars[35]  
      ///    Y  ->globalPars[1]  dY /   globalPars[ 8]   globalPars[15]   globalPars[22]   globalPars[29]   globalPars[36]  
      ///    Z  ->globalPars[2]  dZ /   globalPars[ 9]   globalPars[16]   globalPars[23]   globalPars[30]   globalPars[37]   
      ///    Ax ->globalPars[3]  dAx/   globalPars[10]   globalPars[17]   globalPars[24]   globalPars[31]   globalPars[38]  
      ///    Ay ->globalPars[4]  dAy/   globalPars[11]   globalPars[18]   globalPars[25]   globalPars[32]   globalPars[39]  
      ///    Az ->globalPars[5]  dAz/   globalPars[12]   globalPars[19]   globalPars[26]   globalPars[33]   globalPars[40]  
      ///    CM ->globalPars[6]  dCM/   globalPars[13]   globalPars[20]   globalPars[27]   globalPars[34]   globalPars[41] 
      ///    + dA/dS (42-44) and step (45) 
      // /////////////////////////////////////////////////////////////////////////////////
      bool transformGlobalToPlane(bool updateJacobian,
                                  double* globalPars ,
                                  Trk::PatternTrackParameters& startingParameters,
                                  Trk::PatternTrackParameters& outputParameters);

            
      /////////////////////////////////////////////////////////////////////////////////
      /// Runge Kutta step to plane
      /// Updates the "globalPars" array, which is also used to 
      /// pass the input.  
      /// This array follows the convention outlined above (46 components) 
      /////////////////////////////////////////////////////////////////////////////////
      bool rungeKuttaToPlane(bool updateJacobian, double* globalPars);
      /////////////////////////////////////////////////////////////////////////////////
      /// Straight line step to plane
      /// Updates the "globalPars" array, which is also used to 
      /// pass the input.  
      /// This array follows the convention outlined above (46 components) 
      /////////////////////////////////////////////////////////////////////////////////
      bool straightLineStepToPlane(bool updateJacobian, double* globalPars);
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
      /// status flag. Start as 0. 
      /// set to 1 if set up for forward 
      /// set to 2 if set up for backward 
      /// set to 3 by lastTrajectoryElement or  BackwardPropagationSmoother if m_cluster
      int                                         m_status      ;  
      int                                         m_detstatus   ; //!< 0 (no clusters) 
      int                                         m_inside      ;
      int                                         m_ndist       ;
      int                                         m_nlinksForward     ;
      int                                         m_nlinksBackward     ;
      int                                         m_nholesForward     ;
      int                                         m_nholesBackward     ;
      int                                         m_dholesForward     ;
      int                                         m_dholesBackward     ;
      int                                         m_nclustersForward  ;
      int                                         m_nclustersBackward  ;
      int                                         m_npixelsBackward    ;
      int                                         m_noisemodel  ;
      int                                         m_ndf         ;
      int                                         m_ndfForward        ;
      int                                         m_ndfBackward        ;
      int                                         m_ntsos       ;
      int                                         m_maxholes    ;
      int                                         m_maxdholes   ;
      double                                      m_dist        ;
      double                                      m_xi2Forward        ;
      double                                      m_xi2Backward        ;
      double                                      m_xi2totalForward   ;
      double                                      m_xi2totalBackward   ;
      double                                      m_radlength   ;
      double                                      m_radlengthN  ;
      double                                      m_energylose  ;
      double                                      m_halflength  ;
      double                                      m_step        ;
      double                                      m_xi2max      ;
      double                                      m_xi2maxNoAdd ;
      double                                      m_xi2maxlink  ;  
      double                                      m_xi2multi    ;
      double                                      m_localTransform[13]      ;     /// the transform for this element
      double                                      m_localDir [ 3]      ;

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

      /// Pattern track parameters 

      /// For forward filtering / smoothing 
      /// Predicted state, forward
      Trk::PatternTrackParameters                 m_parametersPredForward; 
      /// Updated state, forward
      Trk::PatternTrackParameters                 m_parametersUpdatedForward; 
      /// For backward filtering / smoothing 
      /// Predicted state, backward
      Trk::PatternTrackParameters                 m_parametersPredBackward; 
      /// Updated state, backward
      Trk::PatternTrackParameters                 m_parametersUpdatedBackward;
      Trk::PatternTrackParameters                 m_parametersSM;
      InDet::SiClusterLink_xk                     m_linkForward[10]   ; 
      InDet::SiClusterLink_xk                     m_linkBackward[10]   ; 
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

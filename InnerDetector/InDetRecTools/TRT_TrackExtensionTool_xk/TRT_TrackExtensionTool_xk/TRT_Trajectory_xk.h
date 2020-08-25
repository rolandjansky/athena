/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class TRT_Trajectory_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for trajectory in Pixels and SCT
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRT_Trajectory_xk_H
#define TRT_Trajectory_xk_H

#include "TrkSegment/TrackSegment.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TRT_TrackExtensionTool_xk/TRT_TrajectoryElement_xk.h"
// MagField cache
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

namespace Trk {
  class Track;
}

namespace InDet{

  class samiStruct{
  public:
    double m_F;
    int m_NA;
  };

  class TRT_Trajectory_xk
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////

    public:

      TRT_Trajectory_xk();
      TRT_Trajectory_xk(const TRT_Trajectory_xk&);
      ~TRT_Trajectory_xk();
      TRT_Trajectory_xk& operator  = (const TRT_Trajectory_xk&);

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////

      const int&  nholes     ()  const {return m_nholes    ;}
      const int&  dholes     ()  const {return m_dholes    ;}
      const int&  nholesb    ()  const {return m_nholesb   ;}
      const int&  nholese    ()  const {return m_nholese   ;}
      const int&  nclusters  ()  const {return m_nclusters ;}
      const int&  ntclusters ()  const {return m_ntclusters;}
      const int&  nElements  ()  const {return m_nElements ;}
      const int&  naElements ()  const {return m_naElements;}

      bool isFirstElementBarrel();
      bool isLastElementBarrel ();

      void set(const TRT_ID                     *,
	       const Trk::IPatternParametersPropagator*,
	       const Trk::IPatternParametersUpdator   *,
	       const Trk::IRIO_OnTrackCreator         *,
	       const Trk::IRIO_OnTrackCreator         *,
	       double,
	       double,
	       double,
         double,
         double);

      void set(Trk::MagneticFieldProperties&, const AtlasFieldCacheCondObj*);

      void initiateForPrecisionSeed
	(std::list< std::pair<Amg::Vector3D,double> >&,
	 std::vector<const InDetDD::TRT_BaseElement*>&,
	 const TRT_DriftCircleContainer*&,
	 const Trk::PatternTrackParameters&);

      void initiateForTRTSeed
	(std::list< std::pair<Amg::Vector3D,double> >&,
	 std::vector<const InDetDD::TRT_BaseElement*>&,
	 const TRT_DriftCircleContainer*&,
	 const Trk::PatternTrackParameters&);

      void convert(std::vector<const Trk::MeasurementBase*>&);
      Trk::TrackSegment* convert();
      Trk::Track       * convert(const Trk::Track&);
      void updateTrackParameters(Trk::PatternTrackParameters&);

      ///////////////////////////////////////////////////////////////////
      // Track finding with and without drift time information
      ///////////////////////////////////////////////////////////////////

      void trackFindingWithDriftTime      (double);
      void trackFindingWithoutDriftTime   (double);
      void trackFindingWithDriftTimeBL    (double);
      void trackFindingWithoutDriftTimeBL (double);
      void buildTrajectoryForPrecisionSeed(bool);
      void buildTrajectoryForTRTSeed      (bool);

      ///////////////////////////////////////////////////////////////////
      // Search start and stop trajectory for endcap TRT
      ///////////////////////////////////////////////////////////////////

      bool searchStartStop();

      ///////////////////////////////////////////////////////////////////
      // Trajectory correction for new polar angle
      ///////////////////////////////////////////////////////////////////

      void radiusCorrection();

      ///////////////////////////////////////////////////////////////////
      // Backward kalman fitter
      ///////////////////////////////////////////////////////////////////

      bool fitter();

      ///////////////////////////////////////////////////////////////////
      // Track parameters estimators
      ///////////////////////////////////////////////////////////////////

      bool trackParametersEstimationForPerigeeWithVertexConstraint   ();
      bool trackParametersEstimationForFirstPointWithVertexConstraint();
      bool trackParametersEstimationForLastPoint                     ();
      bool trackParametersEstimationForFirstPoint                    ();

      std::ostream& dump(std::ostream& out) const;

    protected:

      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      int                                 m_firstRoad      ; //
      int                                 m_lastRoad       ; //
      int                                 m_firstTrajectory; //
      int                                 m_lastTrajectory ; //
      int                                 m_nclusters      ; //
      int                                 m_ntclusters     ; //
      int                                 m_nholesb        ; //
      int                                 m_nholese        ; //
      int                                 m_nholes         ; //
      int                                 m_dholes         ; //
      int                                 m_naElements     ; //
      int                                 m_nElements      ; // nindex
      int                                 m_ndf            ; //
      double                              m_xi2            ; //
      double                              m_roadwidth2     ; // road width**2
      double                              m_zVertexWidth   ; // z-vertex half width
      double                              m_impact         ; // max impact parameters
      double                              m_scale_error    ; // scale factor for hit uncertainty
      Trk::PatternTrackParameters         m_parameters     ; // Track parameters
      TRT_TrajectoryElement_xk            m_elements[400]  ; //
      Trk::MagneticFieldProperties        m_fieldprop      ; //
      const Trk::IPatternParametersPropagator*  m_proptool ; //
      const Trk::IPatternParametersUpdator*  m_updatortool ; //

      ///////////////////////////////////////////////////////////////////
      // Work arrey for stab line search
      ///////////////////////////////////////////////////////////////////

      int        m_MA[5000];
      double     m_U [5000];
      double     m_V [5000];
      samiStruct m_SS[5000];
      double m_A;
      double m_B;

      ///////////////////////////////////////////////////////////////////
      // min pT cut for TRT Segment trajectory
      ///////////////////////////////////////////////////////////////////
     
      double                           m_minTRTSegmentpT; //min pT check for initial segment

      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////

      void stabline(int,double);
      void sort    (samiStruct*,int);
      void erase   (int);
      std::pair<const Trk::PseudoMeasurementOnTrack*,const Trk::PseudoMeasurementOnTrack*>
	pseudoMeasurements(const Trk::Surface*,const Trk::Surface*,int bec);
    };

  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline TRT_Trajectory_xk::TRT_Trajectory_xk()
    {
      m_nElements       = 0;
      m_proptool        = 0;
      m_updatortool     = 0;
      m_scale_error     = 0.;
      m_firstRoad       = 0 ;
      m_lastRoad        = 0 ;
      m_firstTrajectory = 0 ;
      m_lastTrajectory  = 0 ;
      m_nclusters       = 0 ;
      m_ntclusters      = 0 ;
      m_nholesb         = 0 ;
      m_nholese         = 0 ;
      m_nholes          = 0 ;
      m_dholes          = 0 ;
      m_naElements      = 0 ;
      m_ndf             = 0 ;
      m_xi2             = 0.;
      m_roadwidth2      = 0.;
      m_zVertexWidth    = 0.;
      m_impact          = 0.;
      m_A               = 0.;
      m_B               = 0.;
      for(int i=0; i!=5000; ++i) {m_SS[i].m_F = 0.; m_SS[i].m_NA = 0; m_U[i]=0.; m_V[i]=0.;}
    }

  inline TRT_Trajectory_xk::TRT_Trajectory_xk(const TRT_Trajectory_xk& T)
    {
      (*this) = T;
    }

  inline TRT_Trajectory_xk& TRT_Trajectory_xk::operator =
    (const TRT_Trajectory_xk& T)
    {
      m_firstRoad        = T.m_firstRoad      ;
      m_lastRoad         = T.m_lastRoad       ;
      m_firstTrajectory  = T.m_firstTrajectory;
      m_lastTrajectory   = T.m_lastTrajectory ;
      m_nclusters        = T.m_nclusters      ;
      m_ntclusters       = T.m_ntclusters     ;
      m_nholesb          = T.m_nholesb        ;
      m_nholese          = T.m_nholese        ;
      m_nholes           = T.m_nholes         ;
      m_dholes           = T.m_dholes         ;
      m_naElements       = T.m_naElements     ;
      m_nElements        = T.m_nElements      ;
      m_roadwidth2       = T.m_roadwidth2     ;
      m_parameters       = T.m_parameters     ;
      m_fieldprop        = T.m_fieldprop      ;
      m_proptool         = T.m_proptool       ;
      m_updatortool      = T.m_updatortool    ;
      m_ndf              = T.m_ndf            ;
      m_xi2              = T.m_xi2            ;
      m_zVertexWidth     = T.m_zVertexWidth   ;
      m_impact           = T.m_impact         ;
      m_scale_error      = T.m_scale_error    ;
      for(int i=0; i!=400; ++i) m_elements[i]=T.m_elements[i];
      return(*this);
    }

  inline TRT_Trajectory_xk::~TRT_Trajectory_xk() {}

  std::ostream& operator << (std::ostream&,const TRT_Trajectory_xk&);

  inline bool TRT_Trajectory_xk::isFirstElementBarrel()
    {
      return m_elements[m_firstTrajectory].isBarrel();
    }
  inline bool TRT_Trajectory_xk::isLastElementBarrel()
    {
      return m_elements[m_lastTrajectory].isBarrel();
    }

} // end of name space

#endif // TRT_Trajectory_xk

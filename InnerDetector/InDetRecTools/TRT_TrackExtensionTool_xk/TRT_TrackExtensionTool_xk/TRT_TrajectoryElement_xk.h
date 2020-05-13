/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class TRT_TrajectoryElement_xk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for trajector element in TRT
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRT_TrajectoryElement_xk_H
#define TRT_TrajectoryElement_xk_H

#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkPatternParameters/PatternTrackParameters.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkToolInterfaces/IPatternParametersUpdator.h"
#include "TrkExInterfaces/IPatternParametersPropagator.h"
#include "TRT_TrackExtensionTool_xk/TRT_ExtensionDriftCircleLink_xk.h"
// MagField cache
#include "MagFieldElements/AtlasFieldCache.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

class TRT_ID;

namespace InDet{

  class TRT_TrajectoryElement_xk
    {
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////

    public:

      TRT_TrajectoryElement_xk();
      TRT_TrajectoryElement_xk(const TRT_TrajectoryElement_xk&);
      ~TRT_TrajectoryElement_xk();
      TRT_TrajectoryElement_xk& operator  = (const TRT_TrajectoryElement_xk&);

      const bool  &  isCluster()                    const {return m_isCluster ;}
      const bool  &  isBarrel ()                    const {return m_barrel    ;}
      const int   &  status   ()                    const {return m_status    ;}
      const int   &  nlinks   ()                    const {return m_nlinks    ;}
      const int   &  bestlink ()                    const {return m_bestlink  ;}
      const double&  radius   ()                    const {return m_radius    ;}
      const double&  z        ()                    const {return m_z         ;}
      const double&  radiusMin()                    const {return m_radiusMin ;}
      const double&  radiusMax()                    const {return m_radiusMax ;}
      const double&  dpositive()                    const {return m_dpositive ;}
      const double&  dnegative()                    const {return m_dnegative ;}
      const InDetDD::TRT_BaseElement* detElement()  const {return m_detelement;}
      const TRT_ExtensionDriftCircleLink_xk&   link  (int i) const {return m_link[i]    ;}

      ///////////////////////////////////////////////////////////////////
      // Main methods
      ///////////////////////////////////////////////////////////////////
      void set
	(const TRT_ID                           *,
	 const Trk::IPatternParametersPropagator*,
	 const Trk::IPatternParametersUpdator   *,
	 const Trk::IRIO_OnTrackCreator               *,
	 const Trk::IRIO_OnTrackCreator               *,
         double                             );

      void set
      (const Trk::MagneticFieldProperties&, const AtlasFieldCacheCondObj* );

      bool initiateForPrecisionSeed     (bool,const InDetDD::TRT_BaseElement*&,
					 InDet::TRT_DriftCircleCollection::const_iterator&,
					 InDet::TRT_DriftCircleCollection::const_iterator&,
					 std::pair<Amg::Vector3D,double>&, const double*,double);

      bool initiateForTRTSeed           (bool,const InDetDD::TRT_BaseElement*&,
					 InDet::TRT_DriftCircleCollection::const_iterator&,
					 InDet::TRT_DriftCircleCollection::const_iterator&,
					 std::pair<Amg::Vector3D,double>&, const double*,double);

      void initiateLinksForPrecisionSeed(InDet::TRT_DriftCircleCollection::const_iterator&,
					 InDet::TRT_DriftCircleCollection::const_iterator&,
					 std::pair<Amg::Vector3D,double>&,
					 const double*,double);

      void initiateLinksForTRTSeed      (InDet::TRT_DriftCircleCollection::const_iterator&,
					 InDet::TRT_DriftCircleCollection::const_iterator&,
					 std::pair<Amg::Vector3D,double>&,
					 const double*,double);

      bool boundaryTest(double,std::pair<Amg::Vector3D,double>&);

      bool buildForPrecisionSeed(double,double,bool&,bool&);
      bool buildForTRTSeed      (double,double,bool&,bool&);

      double findCloseLink(double,double);
      void  radiusCorrection(const double&);
      const Trk::RIO_OnTrack* rioOnTrack();
      const Trk::RIO_OnTrack* rioOnTrackSimple();

      bool trajectoryGlobalPosition(Amg::Vector3D&,double&);

      bool trackParametersEstimation
	(TRT_TrajectoryElement_xk*,TRT_TrajectoryElement_xk*,Trk::PatternTrackParameters&,double);

      bool trackParametersEstimation
	(TRT_TrajectoryElement_xk*,Trk::PatternTrackParameters&,double);

      void polarAngleEstimation
	(TRT_TrajectoryElement_xk*,Amg::Vector3D&,Amg::Vector3D&,double,double,double*);

      bool addCluster
	(Trk::PatternTrackParameters&,Trk::PatternTrackParameters&,double&);
      bool propagate
	(Trk::PatternTrackParameters&,Trk::PatternTrackParameters&);

    protected:

      ///////////////////////////////////////////////////////////////////
      // Protected Data
      ///////////////////////////////////////////////////////////////////

      bool                                               m_barrel     ;
      bool                                               m_isCluster  ;
      int                                                m_status     ;
      int                                                m_bestlink   ;
      int                                                m_nlinks     ;
      double                                             m_z          ;
      double                                             m_zMin       ;
      double                                             m_zMax       ;
      double                                             m_radius     ;
      double                                             m_radiusMin  ;
      double                                             m_radiusMax  ;
      double                                             m_scale_error;
      double                                             m_dpositive  ;
      double                                             m_dnegative  ;
      TRT_ExtensionDriftCircleLink_xk                    m_link[24]   ;
      const InDetDD::TRT_BaseElement *                   m_detelement ;
      const TRT_ID                   *                   m_trtid      ;
      const Trk::IPatternParametersPropagator*           m_proptool   ;
      const Trk::IPatternParametersUpdator*              m_updatortool;
      const Trk::IRIO_OnTrackCreator       *             m_riomakerD  ;
      const Trk::IRIO_OnTrackCreator       *             m_riomakerN  ;
      Trk::MagneticFieldProperties                       m_fieldprop  ;
      MagField::AtlasFieldCache                          m_fieldCache;

      ///////////////////////////////////////////////////////////////////
      // Comments
      // m_status = -1 no crossed straw
      // m_status =  0 crossed straw without signal (hole)
      // m_status =  1 crossed straw with signal but without drift time
      // m_status =  2 crossed straw with signal and with    drift time
      ///////////////////////////////////////////////////////////////////

      ///////////////////////////////////////////////////////////////////
      // Methods
      ///////////////////////////////////////////////////////////////////

    };

  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline TRT_TrajectoryElement_xk::TRT_TrajectoryElement_xk()
    {
      m_isCluster   = false;
      m_status      =-1    ;
      m_riomakerD   = 0    ;
      m_riomakerN   = 0    ;
      m_proptool    = 0    ;
      m_updatortool = 0    ;
      m_trtid       = 0    ;
      m_scale_error = 2.   ;
      m_barrel      = false;
      m_bestlink    = 0    ;
      m_nlinks      = 0    ;
      m_z           = 0.   ;
      m_zMin        = 0.   ;
      m_zMax        = 0.   ;
      m_radius      = 0.   ;
      m_radiusMin   = 0.   ;
      m_radiusMax   = 0.   ;
      m_dpositive   = 0.   ;
      m_dnegative   = 0.   ;
      m_detelement  = 0    ;
    }

  inline TRT_TrajectoryElement_xk::TRT_TrajectoryElement_xk
    (const TRT_TrajectoryElement_xk& E)
    {
      (*this) = E;
    }

  inline TRT_TrajectoryElement_xk& TRT_TrajectoryElement_xk::operator =
    (const TRT_TrajectoryElement_xk& E)
    {
      m_status      = E.m_status     ;
      m_isCluster   = E.m_isCluster  ;
      m_barrel      = E.m_barrel     ;
      m_bestlink    = E.m_bestlink   ;
      m_nlinks      = E.m_nlinks     ;
      m_detelement  = E.m_detelement ;
      m_trtid       = E.m_trtid      ;
      m_proptool    = E.m_proptool   ;
      m_updatortool = E.m_updatortool;
      m_fieldprop   = E.m_fieldprop  ;
      m_riomakerD   = E.m_riomakerD  ;
      m_riomakerN   = E.m_riomakerN  ;
      m_z           = E.m_z          ;
      m_zMin        = E.m_zMin       ;
      m_zMax        = E.m_zMax       ;
      m_radius      = E.m_radius     ;
      m_radiusMin   = E.m_radiusMin  ;
      m_radiusMax   = E.m_radiusMax  ;
      m_scale_error = E.m_scale_error;
      m_dpositive   = E.m_dpositive  ;
      m_dnegative   = E.m_dnegative  ;
      for(int i=0; i!=m_nlinks; ++i) {m_link[i]=E.m_link[i];}
      return(*this);
    }

  inline TRT_TrajectoryElement_xk::~TRT_TrajectoryElement_xk() {}

  inline void TRT_TrajectoryElement_xk::radiusCorrection(const double& dr)
    {
      if(m_bestlink >= 0) m_link[m_bestlink].radiusCorrection(dr);
    }

} // end of name space

#endif // TRT_TrajectoryElement_xk

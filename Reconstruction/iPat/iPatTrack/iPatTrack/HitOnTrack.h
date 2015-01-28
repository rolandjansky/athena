/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 generic hit on track with status, identifier, coordinates etc
 -------------------------------------------------------------
 ***************************************************************************/

#ifndef IPATTRACK_HITONTRACK_H
# define IPATTRACK_HITONTRACK_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "Identifier/Identifier.h"
#include "TrkParameters/TrackParameters.h"
#include "iPatTrack/HitStatus.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace Trk
{
    class PrepRawData;
    class MaterialEffectsBase;
    class MeasurementBase;
    class Surface;
    class TrackSurfaceIntersection;
    class TrkDetElementBase;
}

class HitOnTrack
{
public:
    HitOnTrack (void);
    HitOnTrack (bool                    	barrel,
		const Trk::TrkDetElementBase*	detectorElement,
		const Amg::Vector3D&		position,
		HitStatus               	status);	// silicon 'hole' constructor

    HitOnTrack (bool                    	barrel,
		double				energyLoss,
		const Amg::Vector3D&		position,
		double				radiationThickness,
		HitStatus      			status);	// inert/scatterer constructor		

    HitOnTrack (const Amg::Vector3D&		position,
		double				sigmaPrecise,
		double				sigmaBroad,
		double				secondSigmaPrecise,
		double				secondSigmaBroad,
		double				sinStereo,
		double,				// unused
		bool            		barrel,
		const Trk::TrkDetElementBase*	detectorElement,
		const Identifier&      		identifier,
		const Trk::PrepRawData*		rio,
		const Trk::MeasurementBase*,	// unused
		HitStatus			status);	// early SpacePoint pattern constructor

    HitOnTrack (bool            		barrel,
		const Identifier&		identifier,
		const Trk::PrepRawData*		rio,
		const Trk::MeasurementBase*	rotBroad,
		const Trk::MeasurementBase*	rotPrecise,
		HitStatus		       	status,
		int				trtThreshold = 0); //  constructor using ROTs

    HitOnTrack (const HitOnTrack&);			// copy constructor
    virtual ~HitOnTrack (void);	       			// destructor

    
    // 2 methods identical to those in RIO_OnTrack
    const Trk::TrkDetElementBase*		detectorElement(void) const;
    const Trk::PrepRawData*			prepRawData(void) const;

    bool					aboveTrThreshold (void) const;
    double					broadSigma (void) const;

    // fill specific for SiTracker clusters
    void				fill (const Amg::Vector3D&		position,
					      double				sigmaBroad,
					      double				sigmaPrecise,
					      double				secondSigmaBroad,
					      double				secondSigmaPrecise,
					      double				sinStereo,
					      double,				//unused
					      bool            			barrel,
					      const Trk::TrkDetElementBase*	detectorElement,
					      const Identifier&      		identifier,
					      HitStatus				status);
    
    const Identifier&				identifier (void) const;
    const Trk::TrackSurfaceIntersection*	intersection (void) const;
    void					intersection (const Trk::TrackSurfaceIntersection*);
    bool					isBarrel (void) const;
    bool					isCluster (void) const;
    bool					isClusterOutlier (void) const;
    bool					isDrift (void) const;
    bool					isEndcap (void) const;
    bool					isMaterial (void) const;
    bool					isOutlier (void) const;
    bool					isPixel (void) const;
    bool					isPositionMeasurement (void) const;
    bool					isShared (void) const;
    const Trk::MaterialEffectsBase*		materialEffects (void) const;
    const Amg::Vector3D&			position (void) const;
    void					print (void) const;
    double					radiationThickness (void) const;
    double					residual (void) const;
    void					residual (double);
    const Trk::PrepRawData*			rio (void) const;
    const Trk::MeasurementBase*			rot (void) const;
    void					rot (const Trk::MeasurementBase*);
    double					secondResidual (void) const;
    void					secondResidual (double);
    double					secondSigma (void) const;
    void					secondSigma (double);
    void					setShared (void);
    double					sigma (void) const;
    void					sigma (double);
    double					sinStereo (void) const;
    HitStatus					status (void) const;
    void					status (HitStatus);
    const Trk::Surface*				surface (void) const;
    const Trk::TrackParameters*			trkParameters (void) const;
    
protected:
    void					setSurface(bool			barrel,
							   const Amg::Vector3D&	position);
    
    bool					m_barrel;
    const Trk::TrkDetElementBase*		m_detectorElement; 
    Identifier					m_identifier;
    const Trk::TrackSurfaceIntersection*	m_intersection;
    const Trk::MaterialEffectsBase*		m_materialEffects;
    Amg::Vector3D				m_position;
    double					m_residual;
    const Trk::PrepRawData*			m_rio;
    const Trk::MeasurementBase*			m_rotBroad;
    const Trk::MeasurementBase*			m_rotPrecise;
    // second coordinate measurement: only defined for pixel clusters
    double					m_secondResidual;
    double					m_secondSigmaBroad;
    double					m_secondSigmaPrecise;
    bool					m_shared;
    double					m_sigmaBroad;
    double					m_sigmaPrecise;
    // by definition: a pure rphi measurement has stereo = 0 
    double					m_sinStereo;
    HitStatus					m_status;
    const Trk::Surface*				m_surface;
    bool					m_surfaceOwner;
    bool					m_trtThreshold;

    private:
    // forbidden assignment operator: no implementation
    HitOnTrack &operator= (const HitOnTrack&);
};

//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline const Trk::TrkDetElementBase*
HitOnTrack::detectorElement() const
{ return m_detectorElement; }

inline const Trk::PrepRawData*
HitOnTrack::prepRawData(void) const
{ return m_rio; }

inline bool
HitOnTrack::aboveTrThreshold (void) const
{ return m_trtThreshold; }

inline const Identifier&
HitOnTrack::identifier (void) const
{ return m_identifier; }

inline const Trk::TrackSurfaceIntersection*
HitOnTrack::intersection (void) const
{ return m_intersection; }

inline bool
HitOnTrack::isBarrel (void) const
{ return m_barrel; }

inline bool
HitOnTrack::isClusterOutlier (void) const
{ return (m_status == strip_reject || m_status == pixel_reject); }

inline bool
HitOnTrack::isEndcap (void) const
{ return !m_barrel; }

inline bool
HitOnTrack::isPixel (void) const
{ return (m_status == pixel_cluster || m_status == broad_pixel || m_status == pixel_reject); }

inline bool
HitOnTrack::isShared (void) const
{ return m_shared; } 

inline const Trk::MaterialEffectsBase*
HitOnTrack::materialEffects (void) const
{ return m_materialEffects; }
    
inline const Amg::Vector3D&
HitOnTrack::position (void) const
{ return m_position; }

inline double
HitOnTrack::residual (void) const
{ return m_residual; }

inline void
HitOnTrack::residual (double value)
{ m_residual = value; }

inline const Trk::PrepRawData*
HitOnTrack::rio (void) const
{ return m_rio; }

inline double
HitOnTrack::secondResidual (void) const
{ return m_secondResidual; }

inline void
HitOnTrack::secondResidual (double value)
{ m_secondResidual = value; }

inline void
HitOnTrack::sigma (double value)
{ m_sigmaPrecise = value; }

inline double
HitOnTrack::sinStereo (void) const
{ return m_sinStereo; }

inline HitStatus
HitOnTrack::status (void) const 
{ return m_status; }

inline const Trk::Surface*
HitOnTrack::surface (void) const
{ return m_surface; }

#endif	// IPATTRACK_HITONTRACK_H

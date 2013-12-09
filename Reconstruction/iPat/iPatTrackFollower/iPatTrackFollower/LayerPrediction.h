/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 transient object formed during track-following through a layer giving
 predicted hit parameters in an intersected detector
 ***************************************************************************/

#ifndef IPATTRACKFOLLOWER_LAYERPREDICTION_H
# define IPATTRACKFOLLOWER_LAYERPREDICTION_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/ToolHandle.h"
#include "InDetPrepRawData/SiCluster.h"
#include "TrkParameters/TrackParameters.h"
#include "iPatGeometry/SiliconDetector.h"
#include "iPatTrack/HitOnTrack.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class ISiClusterProperties;
class LayerAssociation;

class LayerPrediction
{

public:
    // constructor
    LayerPrediction(const SiliconDetector*	detector,
		    DetectorIntersect*		intersect,
		    const Trk::TrackParameters*	parameters);
    // destructor
    ~LayerPrediction(void);
    
    // forbidden copy constructor
    // forbidden assignment operator

    // set a unique association: either to a LayerAssociation
    //                           or to an existing hit (with corresponding cluster)
    void			associate (LayerAssociation* association);
    void			associate (const InDet::SiCluster*, HitOnTrack*);
    // associated silicon cluster
    const InDet::SiCluster*	cluster (void) const;
    // associated silicon detector
    const SiliconDetector&	detector (void) const;
    // detectorIdentifier
    const Identifier&		detectorIdentifier (void) const;
    // associated hit (query and get)
    bool			hasHit (void) const;
    // for measurement
    HitOnTrack*		        hit (const ToolHandle<ISiClusterProperties>& properties) const;
    // for scattering centre
    HitOnTrack*		        hit (double weight) const;
    // for overlap tests
    double			localEta (void) const;
    double			localPhi (void) const;
    // predicted position
    const Amg::Vector3D&	position (void) const;
    // print for debugging
    void			print (void) const;
    // enumerated hit status (get/set)
    HitStatus			status (void) const;
    void			status (HitStatus);
    // radius for ordering
    double			radius (void) const;
    
private:
    LayerAssociation*		m_association;
    const InDet::SiCluster*	m_cluster;
    const SiliconDetector*	m_detector;
    HitOnTrack*			m_hit;
    DetectorIntersect*		m_intersect;
    const Trk::TrackParameters*	m_parameters;
    double			m_radius;
    HitStatus                   m_status;
    
    // copy, assignment: no semantics, no implementation
    LayerPrediction (const LayerPrediction&);

    LayerPrediction &operator= (const LayerPrediction&);
    
};

//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline void
LayerPrediction::associate (LayerAssociation* association)
{
    m_association = association;
    m_status	= strip_cluster;  // approx!!
}

inline void
LayerPrediction::associate (const InDet::SiCluster* cluster, HitOnTrack* hit)
{
    m_cluster	= cluster;
    m_hit	= hit;
    m_status	= hit->status();
}

inline const InDet::SiCluster*
LayerPrediction::cluster (void) const
{ return m_cluster; }
    
inline const SiliconDetector&
LayerPrediction::detector (void) const
{ return *m_detector; }

inline const Identifier&
LayerPrediction::detectorIdentifier (void) const
{ return m_detector->identifier(); }

inline bool
LayerPrediction::hasHit (void) const
{ return (m_hit != 0); }

inline double
LayerPrediction::localEta (void) const
{ return m_intersect->localEta(); }

inline double
LayerPrediction::localPhi (void) const
{ return m_intersect->localPhi(); }

inline const Amg::Vector3D&
LayerPrediction::position (void) const
{ return m_intersect->position(); }

inline HitStatus
LayerPrediction::status (void) const
{ return m_status; }

inline void
LayerPrediction::status (HitStatus status)
{ m_status = status; }

inline double
LayerPrediction::radius (void) const
{ return m_radius; }

#endif // IPATTRACKFOLLOWER_LAYERPREDICTION_H


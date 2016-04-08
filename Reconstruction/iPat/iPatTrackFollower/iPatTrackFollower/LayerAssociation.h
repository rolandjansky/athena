/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 tentative association of a SiCluster to a predicted hit (see class 
 LayerPrediction).
 This is a transient object formed during track-following in a layer
 ***************************************************************************/

//<doc><file>	$Id: LayerAssociation.h,v 1.14 2006-03-22 06:32:14 pop Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef IPATTRACKFOLLOWER_LAYERASSOCIATION_H
# define IPATTRACKFOLLOWER_LAYERASSOCIATION_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "InDetPrepRawData/SiCluster.h"
#include "InDetPrepRawData/SiWidth.h"
#include "iPatTrackFollower/LayerPrediction.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class LayerAssociation
{

public:
    // constructor for associated pixel or stereo strip cluster
    LayerAssociation(const InDet::SiCluster*	cluster,
		     double			eta_offset,
		     bool		       	is_pixel,
		     double			phi_offset,
		     LayerPrediction*		prediction);
    // constructor for associated axial strip cluster
    LayerAssociation(const InDet::SiCluster*	cluster,
		     double			phi_offset,
		     LayerPrediction*		prediction);
    
    // implicit destructor
    // forbidden copy constructor
    // forbidden assignment operator

    // the associated silicon cluster
    const InDet::SiCluster&	cluster (void) const;
    // confirm association
    void			confirm (void);
    // discard association
    void			discard (void);
    // abs distance of closest approach to given hit offset (in units of pixel/strip pitch)
    double			closest_approach (LayerAssociation* reference_hit) const;
    // the associated prediction
    const LayerPrediction*	prediction (void) const;
    // print for debugging
    void			print (void) const;
    // enumerated hit status
    HitStatus			status (void) const;
    // update phi offsets
    void			update (LayerAssociation* reference);
    
private:
    const InDet::SiCluster*	m_cluster;
    double			m_eta_offset;
    double			m_eta_scale;
    bool			m_is_pixel;
    double			m_phi_offset;
    LayerPrediction*		m_prediction;
    HitStatus			m_status;
    double			m_tolerance;
    
    // copy, assignment: no semantics, no implementation
    LayerAssociation (const LayerAssociation&);
    LayerAssociation &operator= (const LayerAssociation&);
    
};

//<<<<<< INLINE CLASS STRUCTURE INITIALIZATION                          >>>>>>

inline
LayerAssociation::LayerAssociation(const InDet::SiCluster*	cluster,
				   double			eta_offset,
				   bool				is_pixel,
				   double			phi_offset,
				   LayerPrediction*      	prediction)
    :	m_cluster		(cluster),
	m_eta_offset		(eta_offset),
	m_eta_scale		(1.0),
	m_is_pixel		(is_pixel),
	m_phi_offset		(phi_offset),
	m_prediction		(prediction),
	m_status		(pending)
{
    // representing alignment uncertainty / scattering within layer ...
    m_tolerance	= 0.100*CLHEP::mm;

    // pixel aspect ratio
    if (is_pixel)
    {
	InDet::SiWidth	siWidth = m_cluster->width();
	double	etaClusterSize	= siWidth.colRow().y();
	double	phiClusterSize  = siWidth.colRow().x();
  	m_eta_scale =  etaClusterSize*m_cluster->width().phiR() /
		       (phiClusterSize*m_cluster->width().z());
    }
    else
    {
	m_eta_scale		= 1.0;
    }
}

inline
LayerAssociation::LayerAssociation(const InDet::SiCluster*	cluster,
				   double		       	phi_offset,
				   LayerPrediction*      	prediction)
    :	m_cluster		(cluster),
	m_eta_offset		(0),
	m_eta_scale		(1.0),
	m_is_pixel		(false),
	m_phi_offset		(phi_offset),
	m_prediction		(prediction),
	m_status		(pending)
{
    // representing alignment uncertainty / scattering within layer ...
    m_tolerance	= 0.100*CLHEP::mm;
}

//<<<<<< INLINE PRIVATE MEMBER FUNCTIONS                                >>>>>>
//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline const InDet::SiCluster&
LayerAssociation::cluster (void) const
{ return *m_cluster; }

inline void
LayerAssociation::confirm (void)
{
    if (m_is_pixel)
    {
	m_status = pixel_cluster;
    }
    else
    {
	m_status = strip_cluster;
    }
    m_prediction->associate(this);
}

inline void
LayerAssociation::discard (void)
{ m_status = outside; }

inline const LayerPrediction*
LayerAssociation::prediction (void) const
{ return m_prediction; }

inline HitStatus
LayerAssociation::status (void) const
{ return m_status; }

inline void
LayerAssociation::update (LayerAssociation* reference)
{ m_phi_offset -= reference->m_phi_offset; }

#endif // IPATTRACKFOLLOWER_LAYERASSOCIATION_H


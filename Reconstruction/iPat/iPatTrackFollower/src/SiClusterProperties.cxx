/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 helper class to assign correct cluster properties, hopefully
 this is a temporary requirement (only needed during the EDM upheavals)
 ----------------------------------------------------------------------
 ***************************************************************************/

#include <cmath>
#include <iomanip>
#include <iostream>
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetPrepRawData/SiCluster.h"
#include "InDetPrepRawData/SiWidth.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/RotatedTrapezoidBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "iPatGeometry/SiliconDetector.h"
#include "iPatTrack/HitOnTrack.h"
#include "iPatTrackFollower/SiClusterProperties.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

SiClusterProperties::SiClusterProperties(const std::string&	type,
					 const std::string&	name,
					 const IInterface*	parent)
    :   AthAlgTool		(type, name, parent),
	m_pixelIdHelper		(nullptr),
	m_pixelBroadROT_Maker	(""),
	m_pixelPreciseROT_Maker	(""),
	m_sctIdHelper		(nullptr),
	m_sctBroadROT_Maker	(""),
	m_sctPreciseROT_Maker	(""),
	m_broadEtaError		(0.),
	m_broadPhiError		(0.),
	m_cluster		(nullptr),
	m_element		(nullptr),
	m_parameters		(nullptr),
	m_preciseEtaError	(0.),
	m_precisePhiError	(0.),
	m_rotBroad		(nullptr),
	m_rotPrecise		(nullptr),
	m_sqrt12		(sqrt(12.)),
	m_status		(pending),
	m_trackCotTheta		(0.),
	m_trackRadius		(0.)	
{
    declareInterface<ISiClusterProperties>(this);
    declareProperty("PixelBroadROT_Maker",	m_pixelBroadROT_Maker);
    declareProperty("PixelPreciseROT_Maker",	m_pixelPreciseROT_Maker);
    declareProperty("SCT_BroadROT_Maker",	m_sctBroadROT_Maker);
    declareProperty("SCT_PreciseROT_Maker",	m_sctPreciseROT_Maker);
}

SiClusterProperties::~SiClusterProperties()
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
SiClusterProperties::initialize()
{
    ATH_MSG_INFO( "SiClusterProperties::initialize() - package version " << PACKAGE_VERSION );

    // get the Tools
    if (! m_pixelBroadROT_Maker.empty())
    {
	if (m_pixelBroadROT_Maker.retrieve().isFailure())
	{
	    ATH_MSG_FATAL( "Failed to retrieve tool " << m_pixelBroadROT_Maker );
	    return StatusCode::FAILURE;
	}
	else
	{
	    ATH_MSG_INFO( "Retrieved tool " << m_pixelBroadROT_Maker );
	}
    }
    if (! m_pixelPreciseROT_Maker.empty())
    {
	if (m_pixelPreciseROT_Maker.retrieve().isFailure())
	{
	    ATH_MSG_FATAL( "Failed to retrieve tool " << m_pixelPreciseROT_Maker );
	    return StatusCode::FAILURE;
	}
	else
	{
	    ATH_MSG_INFO( "Retrieved tool " << m_pixelPreciseROT_Maker );
	}
    }
    if (! m_sctBroadROT_Maker.empty())
    {
	if (m_sctBroadROT_Maker.retrieve().isFailure())
	{
	    ATH_MSG_FATAL( "Failed to retrieve tool " << m_sctBroadROT_Maker );
	    return StatusCode::FAILURE;
	}
	else
	{
	    ATH_MSG_INFO( "Retrieved tool " << m_sctBroadROT_Maker );
	}
    }
    if (! m_sctPreciseROT_Maker.empty())
    {
	if (m_sctPreciseROT_Maker.retrieve().isFailure())
	{
	    ATH_MSG_FATAL( "Failed to retrieve tool " << m_sctPreciseROT_Maker );
	    return StatusCode::FAILURE;
	}
	else
	{
	    ATH_MSG_INFO( "Retrieved tool " << m_sctPreciseROT_Maker );
	}
    }
    
    // get the silicon ID helpers
    StoreGateSvc* detStore = 0;
    if (StatusCode::SUCCESS != service( "DetectorStore", detStore ))
    {
        ATH_MSG_FATAL( "Could not get DetectorStore");
        return StatusCode::FAILURE;
    }

    if (StatusCode::SUCCESS != detStore->retrieve(m_pixelIdHelper, "PixelID"))
    {
        ATH_MSG_FATAL( "Could not get PixelID helper !" );
        return StatusCode::FAILURE;
    }
    
    if (StatusCode::SUCCESS != detStore->retrieve(m_sctIdHelper, "SCT_ID"))
    {
        ATH_MSG_FATAL( "Could not get SCT_ID helper !" );
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode
SiClusterProperties::finalize()
{
    ATH_MSG_INFO( "finalised" );

    return StatusCode::SUCCESS;
}

std::pair<double,double>
SiClusterProperties::broadErrors (const InDet::SiCluster*	cluster,
				  const SiliconDetector*	detector,
				  const Amg::Vector3D&   	trackIntersect)
{
    m_cluster		= cluster;
    m_globalPosition	= cluster->globalPosition();
    setProperties (detector,trackIntersect);
    return std::make_pair(m_broadPhiError,m_broadEtaError);
}

HitOnTrack*
SiClusterProperties::hitOnTrack (const InDet::SiCluster*	cluster,
				 const SiliconDetector*		detector,
				 const Trk::TrackParameters*	parameters)
{
    m_cluster 		= cluster;
    m_globalPosition	= cluster->globalPosition();
    m_parameters	= parameters;
    setProperties (detector,m_parameters->position(),m_parameters->momentum().unit());
    
    if (m_rotBroad)
    {	
	return new HitOnTrack(detector->ring().isBarrel(),
			      cluster->identify(),
			      cluster,
			      m_rotBroad,
			      m_rotPrecise,
			      m_status);
    }

    // deprecated: this shouldn't be reached any more
    ATH_MSG_WARNING(" reached deprecated code ");
    
    // return new HitOnTrack(m_globalPosition,
    // 			  m_broadPhiError,
    // 			  m_precisePhiError,
    // 			  m_broadEtaError,
    // 			  m_preciseEtaError,
    // 			  cluster->detectorElement()->sinStereo(m_globalPosition),
    // 			  0.,
    // 			  detector->ring().isBarrel(),
    // 			  cluster->detectorElement(),
    // 			  cluster->identify(),
    // 			  cluster,
    // 			  0,
    // 			  m_status);
    return 0;
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
SiClusterProperties::barrelPixelErrors (void)
{
    // use broad errors for pixel cluster with unusual size or shape
    m_rotBroad			= dynamic_cast<const InDet::SiClusterOnTrack*>
				  (m_pixelBroadROT_Maker->correct(*m_cluster, *m_parameters));
    int	size			= m_cluster->rdoList().size();
    double phiClusterSize	= m_cluster->width().colRow().x();
    double etaClusterSize	= m_cluster->width().colRow().y();

    // cluster too wide in eta or phi or too few pixels at high eta
    if (etaClusterSize		> 2 + static_cast<int>(m_trackCotTheta)
	|| phiClusterSize	> 3
	|| size			< static_cast<int>(m_trackCotTheta) - 2)
    {
	m_status	= broad_pixel;
	m_rotPrecise	= 0;
    }
    else
    {
	m_status	= pixel_cluster;
	m_rotPrecise	= dynamic_cast<const InDet::SiClusterOnTrack*>
			  (m_pixelPreciseROT_Maker->correct(*m_cluster, *m_parameters));
    }
}

void
SiClusterProperties::barrelPixelProperties (void)
{
    // note extra broadening of broad eta error to ~account for pixel inefficiency
    // (which often causes cluster splitting in eta)
    m_status		= broad_pixel;
    m_broadEtaError	= (m_cluster->width().z()+0.050*CLHEP::mm)/m_sqrt12;
    m_broadPhiError	= m_cluster->width().phiR()/m_sqrt12;
}

void
SiClusterProperties::barrelSCT_Errors (void)
{
    // something from old iPatRec code I'd like to see in the ROT:
    //      since clusters of width>2 on stiff tracks (pT>1GeV) are
    //      usually due to overlapping hits or delta rays. Width=2 clusters
    //      are more precise than single strip/pixels on isolated tracks -
    //      but it is dangerous to exploit this in jets and they are thus
    //      treated like singles.

    // anyway use broad errors for clusters with more than 2 strips
    m_rotBroad		= dynamic_cast<const InDet::SiClusterOnTrack*>
			  (m_sctBroadROT_Maker->correct(*m_cluster, *m_parameters));
    if (m_cluster->rdoList().size() > 2)
    {
	m_status	= broad_strip;
	m_rotPrecise	= 0;
    }
    else
    {
	m_status       	= strip_cluster;
	m_rotPrecise	= dynamic_cast<const InDet::SiClusterOnTrack*>
			  (m_sctPreciseROT_Maker->correct(*m_cluster, *m_parameters));
    }
}

void
SiClusterProperties::barrelSCT_Properties (void)
{
    m_status	       	= broad_strip;
    m_broadPhiError	= m_cluster->width().phiR()/m_sqrt12;
}

void
SiClusterProperties::endcapPixelErrors (void)
{
    // broad errors for clusters with more than 2 pixels in either direction
    m_rotBroad			= dynamic_cast<const InDet::SiClusterOnTrack*>
				  (m_pixelBroadROT_Maker->correct(*m_cluster, *m_parameters));
    
    double phiClusterSize	= m_cluster->width().colRow().x();
    double etaClusterSize	= m_cluster->width().colRow().y();
    if (phiClusterSize > 2 || etaClusterSize > 2)
    {
	m_status       	= broad_pixel;
	m_rotPrecise	= 0;
    }
    else
    {
	m_status	= pixel_cluster;
	m_rotPrecise	= dynamic_cast<const InDet::SiClusterOnTrack*>
			  (m_pixelPreciseROT_Maker->correct(*m_cluster, *m_parameters));
    }
}

void
SiClusterProperties::endcapPixelProperties (void)
{
    m_status	       	= broad_pixel;
    m_broadEtaError	= m_cluster->width().z()/m_sqrt12;
    m_broadPhiError	= m_cluster->width().phiR()/m_sqrt12;
}

void
SiClusterProperties::endcapSCT_Errors (void)
{
    // use broad errors for clusters with more than 2 strips
    m_rotBroad		= dynamic_cast<const InDet::SiClusterOnTrack*>
			  (m_sctBroadROT_Maker->correct(*m_cluster, *m_parameters));
    if (m_cluster->rdoList().size() > 2)
    {
	m_status	= broad_strip;
	m_rotPrecise	= 0;
    }
    else
    {
	m_status       	= strip_cluster;
	m_rotPrecise	= dynamic_cast<const InDet::SiClusterOnTrack*>
			  (m_sctPreciseROT_Maker->correct(*m_cluster, *m_parameters));
    }
}

void
SiClusterProperties::endcapSCT_Properties (void)
{
    // 'natural' error for wedge shape + smearing
    // crude approximation to use in initial track finding stages
    m_status	       	= broad_strip;
    double	error	= m_cluster->width().phiR()/m_sqrt12 *
			  m_trackRadius/m_globalPosition.perp();
    m_broadPhiError	= std::sqrt(error*error + 0.012*0.012);	//CLHEP::mm
} 

void
SiClusterProperties::setProperties (const SiliconDetector*	detector,
				    const Amg::Vector3D& 	trackIntersect)
{
    // only broad errors will be required when track direction is unknown
    m_precisePhiError	= 0.;
    m_preciseEtaError	= 0.;
    const InDetDD::SiDetectorElement& element	= detector->element();
    m_globalPosition	= m_cluster->globalPosition();

    if (element.isBarrel())
    {
	if (element.isPixel())
	{
	    barrelPixelProperties();
	}
	else
	{
	    barrelSCT_Properties();
	}
    }
    else
    {
	if (element.isPixel())
	{
	    endcapPixelProperties();
	}
	else
	{
	    m_trackRadius = trackIntersect.perp();
	    endcapSCT_Properties();
	}
    }
}

void
SiClusterProperties::setProperties (const SiliconDetector*	detector,
				    const Amg::Vector3D& 	trackIntersect,
				    const Amg::Vector3D&	trackDirection)
{
    const InDetDD::SiDetectorElement& element	= detector->element();
    if (element.isBarrel())
    {
	if (element.isPixel())
	{
	    m_trackCotTheta	= std::abs(trackDirection.z()/trackDirection.perp());
	    barrelPixelProperties();
	    barrelPixelErrors();
	}
	else
	{
	    barrelSCT_Properties();
	    barrelSCT_Errors();
	}
    }
    else
    {
	if (element.isPixel())
	{
	    endcapPixelProperties();
	    endcapPixelErrors();
	}
	else
	{
	    m_trackRadius = trackIntersect.perp();
	    endcapSCT_Properties();
	    endcapSCT_Errors();
	}
    }
}   

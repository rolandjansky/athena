/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiliconLayerAssociator.cxx, (c) ATLAS iPatRec
//
// associate clusters to predictions given in a SiliconLayer
//
///////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "iPatGeometry/SiliconRing.h"
#include "iPatGeometry/SiliconTrapezoid.h"
#include "iPatTrackFollower/LayerPrediction.h"
#include "iPatTrackFollower/SiliconLayerAssociator.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

SiliconLayerAssociator::SiliconLayerAssociator(const std::string&	type,
					       const std::string&	name,
					       const IInterface*	parent)
    :   AthAlgTool		(type, name, parent),
	m_pixelConditions	("PixelConditionsSummarySvc",name),
	m_sctConditions		("SCT_ConditionsSummarySvc",name)
{
    declareInterface<ISiliconLayerAssociator>(this);
    declareProperty("PixelConditionsSvc",	m_pixelConditions);
    declareProperty("SCT_ConditionsSvc",	m_sctConditions);
}

SiliconLayerAssociator::~SiliconLayerAssociator()
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
SiliconLayerAssociator::initialize()
{
    ATH_MSG_INFO( "SiliconLayerAssociator::initialize() - package version " << PACKAGE_VERSION );

    if (! m_pixelConditions.empty())
    {
	// Get PixelConditionsSummarySvc
	if (m_pixelConditions.retrieve().isFailure())
	{
	    ATH_MSG_FATAL( "Failed to retrieve service " << m_pixelConditions );
	    return StatusCode::FAILURE;
	}
	else
	{
	    ATH_MSG_INFO( "Retrieved service " << m_pixelConditions );
	}
    }
    
    // sufficient for most module overlap cases (only exceeded when high pixel occupancy)
    m_associations.reserve(8);

    return StatusCode::SUCCESS;
}


StatusCode
SiliconLayerAssociator::finalize()
{
    ATH_MSG_INFO( "SiliconLayerAssociator::finalize() " );
    clear();

    return StatusCode::SUCCESS;
}

void
SiliconLayerAssociator::clear()
{
    // clear state
    for (association_iterator i = m_associations.begin();
	 i != m_associations.end();
	 ++i)
	delete *i;
    m_associations.clear();
    m_referenceAssociation = 0;
}

void
SiliconLayerAssociator::associateClusters (const SiliconClusterMap*			clusterMap,
					   const Amg::Vector3D	     			position,
					   const Amg::Vector3D 	     			transverseDirection,
					   double					rPhiWidth,
					   double					zWidth,
					   std::list<LayerPrediction*>::const_iterator	begin,
					   std::list<LayerPrediction*>::const_iterator	end)
{
    if (begin == end) return;
    m_clusterMap		= clusterMap;
    m_position			= position;
    m_transverseDirection	= transverseDirection;
    m_cosPhi			= transverseDirection.x();
    m_sinPhi			= transverseDirection.y();
    m_rPhiWidth	       		= rPhiWidth;
    m_rZWidth    		= zWidth;

    // clear state before next use
    clear();

    // start search in pixels or axial SCT strips
    bool isStereo	= false;
    associate(isStereo,begin,end);

    // search stereo detectors
    isStereo	= true;
    associate(isStereo,begin,end);

    // FIXME: defer choose to here; make Space offset with smaller error;
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
SiliconLayerAssociator::associate (bool			isStereo,
				   prediction_iterator	begin,
				   prediction_iterator	end)
{
    // for each prediction put closest hit clusters into association_vector
    if (begin == end)		return;
    bool isPixel	= (**begin).detector().ring().isPixel();
    if (isPixel && isStereo)	return;
    bool isBarrel	= (**begin).detector().ring().isBarrel();
    for (prediction_iterator p = begin; p != end; ++p)
    {
	// add all compatible clusters to m_association
	// separate pixel/sct and barrel/endcap loops for speed:
	const SiliconDetector& detector = (**p).detector();
	if (isPixel)
	{
	    if (! m_clusterMap->hasPixelCollection(detector.identifyHash()))
	    {
		if (! m_pixelConditions->isGood(detector.identifyHash()))
		{
		    ATH_MSG_VERBOSE( " dead pixel module " );
		    (**p).status(dead);
		}
		continue;
	    }
	    
	    // very small angles expected -> going to assume cosStereo is 1 ...
	    m_sinStereo		= detector.element().sinStereo();
	    m_rStereoWidth	= m_rPhiWidth + m_rZWidth*std::abs(m_sinStereo);
	    if (isBarrel)
	    {
		makeBarrelPixelAssociations(*p);
	    }
	    else
	    {
		makeEndcapPixelAssociations(*p);
	    }
	}
	else 
	{
	    if (detector.ring().isStereo() != isStereo)		continue;
	    
	    if (! m_clusterMap->hasSCT_Collection(detector.identifyHash()))
	    {
		if (! m_sctConditions->isGood(detector.identifyHash()))
		{
		    ATH_MSG_VERBOSE( " dead SCT module " );
		    (**p).status(dead);
		}
		continue;
	    }
	    
	    m_sinStereo		= detector.element().sinStereo();
	    m_cosStereo		= std::sqrt(1. - m_sinStereo*m_sinStereo);
	    m_rStereoWidth	= m_rPhiWidth*m_cosStereo + m_rZWidth*std::abs(m_sinStereo);
	    if (isBarrel)
	    {
		makeBarrelStereoAssociations(*p);
	    }
	    else
	    {
		makeEndcapStereoAssociations(*p);
	    }
	}
    }
    
    // m_associations now contains all possible solutions.
    // first update for existing association
    if (m_referenceAssociation != 0)
    {
	for (association_iterator i = m_associations.begin();
	     i != m_associations.end();
	     ++i)
	    if ((**i).status() == pending) (**i).update(m_referenceAssociation);
	m_referenceAssociation = 0;
    }
    
    // Choose the closest (including effect of cluster_width) as referenceAssociation,
    // the loop is needed to resolve compatible overlapping solutions
    LayerAssociation*	currentAssociation    	= 0;
    do
    {
	currentAssociation = choose(m_referenceAssociation);
	if (m_referenceAssociation == 0
	    || (currentAssociation != 0
		&& currentAssociation->cluster().width().phiR() <
		m_referenceAssociation->cluster().width().phiR()))
	    m_referenceAssociation = currentAssociation;
    } while (currentAssociation != 0);

    for (prediction_iterator p = begin; p != end; ++p)
    {
	if ((**p).detector().ring().isStereo() == isStereo)
	{
	    // discard 'boundary' (really these are just outside)
	    if ((**p).status() == boundary)
	    {
		(**p).status(outside);
	    }
	    else if ((**p).status() == hole
		     && m_referenceAssociation != 0
		     && (std::abs((**p).localEta())	> 0.9
			 || std::abs((**p).localPhi())	> 0.9))
	    {
		// missing overlaps are probably boundaries
		(**p).status(boundary);
	    }
	}
    }
    
    // correct prediction and width (needs work for stereo!!!)
    if (m_referenceAssociation != 0)
    {
	if (!isStereo)
	{
	    double scale	= m_position.perp() /
				  (m_referenceAssociation->cluster().globalPosition().perp());
	    m_position(0)	= scale*m_referenceAssociation->cluster().globalPosition().x();
	    m_position(1)	= scale*m_referenceAssociation->cluster().globalPosition().y();
	    m_rPhiWidth		= 0.5*m_referenceAssociation->cluster().width().phiR();
	    if (isPixel) m_rZWidth	= 0.5*m_referenceAssociation->cluster().width().z();
	}
    }	    
}

LayerAssociation*
SiliconLayerAssociator::choose (LayerAssociation* referenceAssociation)
{
    double			delta       = 9999.;
    association_iterator 	association = m_associations.end();

    for (association_iterator i = m_associations.begin();
	 i != m_associations.end();
	 ++i)
    {
	if ((**i).status() != pending) continue;
	double approach = (**i).closest_approach(referenceAssociation);
	if (delta > approach)
	{
	    delta	= approach;
	    association	= i;
	}	
    }

    if (association != m_associations.end())
    {
	// confirm hit, discard ambiguities and return chosen association
	(**association).confirm();
	for (association_iterator i = m_associations.begin();
	     i != m_associations.end();
	     ++i)
	{
	    if ((**i).status() == pending
		&& (**i).prediction() == (**association).prediction())  (**i).discard();
	}
	return *association;
    }
    
    // no association found
    return 0;
}

void
SiliconLayerAssociator::makeBarrelAxialAssociations (LayerPrediction* prediction)
{
    // force match to existing association
    if (prediction->hasHit())
    {
	const InDet::SiCluster* cluster	= prediction->cluster();
	double	deltaX		= cluster->globalPosition().x() - prediction->position().x();
	double 	deltaY		= cluster->globalPosition().y() - prediction->position().y();
	double 	deltaRPhi	= deltaX*m_sinPhi - deltaY*m_cosPhi;
	m_referenceAssociation = new LayerAssociation(cluster,deltaRPhi,prediction);
	m_referenceAssociation->confirm();
	m_associations.push_back(m_referenceAssociation);
    }
    
    // otherwise keep closest cluster on each side of prediction
    else
    {
	double	posDelta		=  99999.;
	double	negDelta		= -99999.;
	cluster_iterator end		= m_clusterMap->collectionEnd();
	cluster_iterator negCluster	= end;
	cluster_iterator posCluster	= end;
	for (cluster_iterator cluster = m_clusterMap->collectionBegin();
	     cluster != end;
	     ++cluster)
	{	    
	    double deltaX	= (**cluster).globalPosition().x() - prediction->position().x();
	    double deltaY	= (**cluster).globalPosition().y() - prediction->position().y();
	    double delta	= deltaX*m_sinPhi - deltaY*m_cosPhi;
	    if (delta > 0.)
	    {
		if (delta < posDelta)
		{
		    posDelta  	= delta;
		    posCluster	= cluster;
		}
	    }
	    else
	    {
		if (delta > negDelta)
		{
		    negDelta  	= delta;
		    negCluster	= cluster;
		}
	    }
	}

	// store association (if allowed) for the closest cluster on the negative side
	if (negCluster != end
	    && negDelta > -m_rPhiWidth - 0.5*(**negCluster).width().phiR())
	    m_associations.push_back(new LayerAssociation(*negCluster,
							  negDelta,
							  prediction));

	// same for closest positive
	if (posCluster != end
	    && posDelta <  m_rPhiWidth + 0.5*(**posCluster).width().phiR())
	    m_associations.push_back(new LayerAssociation(*posCluster,
							  posDelta,
							  prediction));
    }
}

void
SiliconLayerAssociator::makeBarrelPixelAssociations (LayerPrediction* prediction)
{
    // force match to existing association
    if (prediction->hasHit())
    {
	const InDet::SiCluster* cluster	= prediction->cluster();
	double	deltaX		= cluster->globalPosition().x() - prediction->position().x();
	double 	deltaY		= cluster->globalPosition().y() - prediction->position().y();
	double 	deltaRPhi	= deltaX*m_sinPhi - deltaY*m_cosPhi;
	double 	deltaZ		= cluster->globalPosition().z() - prediction->position().z();
	m_referenceAssociation	= new LayerAssociation(cluster,
						       deltaZ,
						       true,
						       deltaRPhi,
						       prediction);
	m_referenceAssociation->confirm();
	m_associations.push_back(m_referenceAssociation);
    }

    // otherwise store all allowed associations (i.e. within errors)
    else
    {
	for (cluster_iterator cluster = m_clusterMap->collectionBegin();
	     cluster != m_clusterMap->collectionEnd();
	     ++cluster)
	{
	    double deltaZ = (**cluster).globalPosition().z() - prediction->position().z();
	    if (std::abs(deltaZ) < 0.5*(**cluster).width().z() + m_rZWidth)
	    {
		double deltaX		= (**cluster).globalPosition().x() -
					  prediction->position().x();
		double deltaY		= (**cluster).globalPosition().y() -
					  prediction->position().y();
		double deltaRPhi	= deltaX*m_sinPhi - deltaY*m_cosPhi - deltaZ*m_sinStereo;
		if (std::abs(deltaRPhi) < 0.5*(**cluster).width().phiR() + m_rStereoWidth)
		{
		    m_associations.push_back(new LayerAssociation(*cluster,
								  deltaZ,
								  true,
								  deltaRPhi,
								  prediction));
		}
	    }
	}
    }
}

void
SiliconLayerAssociator::makeBarrelStereoAssociations (LayerPrediction* prediction)
{
    // force match to existing association
    if (prediction->hasHit())
    {
	const InDet::SiCluster* cluster	= prediction->cluster();
	Amg::Vector3D 	dxyz		= cluster->globalPosition() - prediction->position();
	double     		delPhi	= (dxyz.x()*m_sinPhi - dxyz.y()*m_cosPhi)*m_cosStereo - 
					  dxyz.z()*m_sinStereo;
	m_referenceAssociation		= new LayerAssociation(cluster,
							       prediction->localEta(),
							       false,
							       delPhi,
							       prediction);
	m_referenceAssociation->confirm();
	m_associations.push_back(m_referenceAssociation);
    }
    
    // otherwise keep closest cluster on each side of prediction
    else
    {
	double	negDelEta		=  0.;
	double	negDelPhi		= -99999.;
	double	posDelEta		=  0.;
	double	posDelPhi		=  99999.;
	cluster_iterator end		= m_clusterMap->collectionEnd();
	cluster_iterator negCluster	= end;
	cluster_iterator posCluster	= end;
	for (cluster_iterator cluster = m_clusterMap->collectionBegin();
	     cluster != end;
	     ++cluster)
	{
	    Amg::Vector3D	dxyz	= (**cluster).globalPosition() - prediction->position();
	    double     		delPhi	= (dxyz.x()*m_sinPhi - dxyz.y()*m_cosPhi)*m_cosStereo - 
					  dxyz.z()*m_sinStereo;
	    if (delPhi > 0.)
	    {
		if (delPhi < posDelPhi)
		{
		    posDelEta	= prediction->localEta();
		    posDelPhi	= delPhi;
		    posCluster	= cluster;
		}
	    }
	    else
	    {
		if (delPhi > negDelPhi)
		{
		    negDelEta	= prediction->localEta();
		    negDelPhi	= delPhi;
		    negCluster	= cluster;
		}
	    }
	}

	// store association (if allowed) for the closest cluster on the negative side
	if (negCluster != end
	    && negDelPhi > -m_rStereoWidth - 0.5*(**negCluster).width().phiR())
	{
	    m_associations.push_back(new LayerAssociation(*negCluster,
							  negDelEta,
							  false,
							  negDelPhi,
							  prediction));
	    //	    (**m_associations.rbegin()).print();
	    
	}

	// same for closest positive
	if (posCluster	!= end
	    && posDelPhi <  m_rStereoWidth + 0.5*(**posCluster).width().phiR())
	{
	    m_associations.push_back(new LayerAssociation(*posCluster,
							  posDelEta,
							  false,
							  posDelPhi,
							  prediction));
	    //	    (**m_associations.rbegin()).print();
	}
    }
}

void
SiliconLayerAssociator::makeEndcapAxialAssociations (LayerPrediction* prediction)
{
    // force match to existing association
    if (prediction->hasHit())
    {
	const InDet::SiCluster* cluster	= prediction->cluster();
	double	deltaX		= cluster->globalPosition().x() - prediction->position().x();
	double 	deltaY		= cluster->globalPosition().y() - prediction->position().y();
	double 	deltaRPhi	= (deltaX*cluster->globalPosition().y() -
				   deltaY*cluster->globalPosition().x()) /
				  cluster->globalPosition().perp();
	m_referenceAssociation = new LayerAssociation(cluster,deltaRPhi,prediction);
	m_referenceAssociation->confirm();
	m_associations.push_back(m_referenceAssociation);
    }
    
    // otherwise keep closest cluster on each side of prediction
    else
    {
	double	posDelta		=  99999.;
	double	negDelta		= -99999.;
	cluster_iterator end		= m_clusterMap->collectionEnd();
	cluster_iterator negCluster	= end;
	cluster_iterator posCluster	= end;
	for (cluster_iterator cluster = m_clusterMap->collectionBegin();
	     cluster != end;
	     ++cluster)
	{	    
	    double deltaX	= (**cluster).globalPosition().x() - prediction->position().x();
	    double deltaY	= (**cluster).globalPosition().y() - prediction->position().y();
	    double deltaRPhi	= (deltaX*(**cluster).globalPosition().y() -
				   deltaY*(**cluster).globalPosition().x()) /
				  (**cluster).globalPosition().perp();
	    if (deltaRPhi > 0.)
	    {
		if (deltaRPhi < posDelta)
		{
		    posDelta  	= deltaRPhi;
		    posCluster	= cluster;
		}
	    }
	    else
	    {
		if (deltaRPhi > negDelta)
		{
		    negDelta  	= deltaRPhi;
		    negCluster	= cluster;
		}
	    }
	}

	// store association (if allowed) for the closest cluster on the negative side
	if (negCluster != end
	    && negDelta > -m_rPhiWidth - 0.5*(**negCluster).width().phiR())
	    m_associations.push_back(new LayerAssociation(*negCluster,
							  negDelta,
							  prediction));

	// same for closest positive
	if (posCluster != end
	    && posDelta <  m_rPhiWidth + 0.5*(**posCluster).width().phiR())
	    m_associations.push_back(new LayerAssociation(*posCluster,
							  posDelta,
							  prediction));
    }
}

void
SiliconLayerAssociator::makeEndcapPixelAssociations (LayerPrediction* prediction)
{
    // force match to existing association
    if (prediction->hasHit())
    {
	const InDet::SiCluster* cluster	= prediction->cluster();
	double	deltaX		= cluster->globalPosition().x() - prediction->position().x();
	double 	deltaY		= cluster->globalPosition().y() - prediction->position().y();
	double 	deltaRPhi	= deltaX*m_sinPhi - deltaY*m_cosPhi;
 	double	deltaR		= deltaX*m_cosPhi + deltaY*m_sinPhi;
	m_referenceAssociation = new LayerAssociation(cluster,
						      deltaR,
						      true,
						      deltaRPhi,
						      prediction);
	m_referenceAssociation->confirm();
	m_associations.push_back(m_referenceAssociation);
    }
    
    // otherwise store all allowed associations (i.e. within errors)
    else
    {
	for (cluster_iterator cluster = m_clusterMap->collectionBegin();
	     cluster != m_clusterMap->collectionEnd();
	     ++cluster)
	{
	    double deltaX     	= (**cluster).globalPosition().x() - prediction->position().x();
	    double deltaY     	= (**cluster).globalPosition().y() - prediction->position().y();
	    double deltaR     	= deltaX*m_cosPhi + deltaY*m_sinPhi;
	    double deltaRPhi	= deltaX*m_sinPhi - deltaY*m_cosPhi - deltaR*m_sinStereo;
	    if (std::abs(deltaRPhi)	< 0.5*(**cluster).width().phiR() + m_rStereoWidth
 		&& std::abs(deltaR)	< 0.5*(**cluster).width().z() + m_rZWidth)
	    {
		m_associations.push_back(new LayerAssociation(*cluster,
							      deltaR,
							      true,
							      deltaRPhi,
							      prediction));
	    }
	}
    }
}

void
SiliconLayerAssociator::makeEndcapStereoAssociations (LayerPrediction* prediction)
{
    // force match to existing association
    if (prediction->hasHit())
    {
	const InDet::SiCluster* cluster	= prediction->cluster();
	double	deltaX		= cluster->globalPosition().x() - prediction->position().x();
	double 	deltaY		= cluster->globalPosition().y() - prediction->position().y();
	double	deltaR		= deltaX*cluster->globalPosition().x() +
				  deltaY*cluster->globalPosition().y();
	double 	deltaRPhi	= deltaX*cluster->globalPosition().y() -
				  deltaY*cluster->globalPosition().x();
	deltaRPhi		= (deltaRPhi*m_cosStereo - deltaR*m_sinStereo) /
				  cluster->globalPosition().perp();
	m_referenceAssociation = new LayerAssociation(cluster,deltaRPhi,prediction);
	m_referenceAssociation->confirm();
	m_associations.push_back(m_referenceAssociation);
    }
    
    // otherwise keep closest cluster on each side of prediction
    else
    {
	double  negDelEta     	=  0.;
	double  negDelPhi     	= -99999.;
	double  posDelEta     	=  0.;
	double  posDelPhi     	=  99999.;
	cluster_iterator end		= m_clusterMap->collectionEnd();
	cluster_iterator negCluster	= end;
	cluster_iterator posCluster	= end;
	for (cluster_iterator cluster = m_clusterMap->collectionBegin();
	     cluster != end;
	     ++cluster)
	{	    
	    double deltaX	= (**cluster).globalPosition().x() - prediction->position().x();
	    double deltaY	= (**cluster).globalPosition().y() - prediction->position().y();
	    double deltaR	= deltaX*(**cluster).globalPosition().x() +
				  deltaY*(**cluster).globalPosition().y();
	    double deltaRPhi	= deltaX*(**cluster).globalPosition().y() -
				  deltaY*(**cluster).globalPosition().x();
	    deltaRPhi		= (deltaRPhi*m_cosStereo - deltaR*m_sinStereo) /
				  (**cluster).globalPosition().perp();	    
	    if (deltaRPhi > 0.)
	    {
		if (deltaRPhi < posDelPhi)
		{
		    posDelEta	= prediction->localEta();
		    posDelPhi	= deltaRPhi;
		    posCluster	= cluster;
		}
	    }
	    else
	    {
		if (deltaRPhi > negDelPhi)
		{
		    negDelEta	= prediction->localEta();
		    negDelPhi	= deltaRPhi;
		    negCluster	= cluster;
		}
	    }
	}

	// store association (if allowed) for the closest cluster on the negative side
	if (negCluster != end
	    && negDelPhi > -m_rStereoWidth - 0.5*(**negCluster).width().phiR())
	    m_associations.push_back(new LayerAssociation(*negCluster,
							  negDelEta,
							  false,
							  negDelPhi,
							  prediction));

	// same for closest positive
	if (posCluster != end
	    && posDelPhi - 0.5*(**posCluster).width().phiR() <  m_rStereoWidth)
	    m_associations.push_back(new LayerAssociation(*posCluster,
							  posDelEta,
							  false,
							  posDelPhi,
							  prediction));
    }
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 AlgTool creating silicon layer/ring/detector geometry structure.
 Used to retrieve the layer/ring/detector intersected by a track vector.
 --------------------------------------------------------------------------
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include <iostream>
#include "GaudiKernel/SystemOfUnits.h"
#include "Identifier/IdentifierHash.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "iPatGeometry/SiliconDetectorBuilder.h"
#include "iPatGeometry/SiliconRing.h"
#include "iPatGeometryTools/LayerAllocator.h"
#include "iPatInterfaces/IDetectorSelection.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

LayerAllocator::LayerAllocator (const std::string&	type,
				const std::string&	name, 
				const IInterface*      	parent)
    :	AthAlgTool		(type, name, parent),
	m_detectorSelection	("DetectorSelection/DetectorSelection"),
	m_pixelName		("Pixel"),
	m_sctName		("SCT"),
	m_ringVector		(new ring_vector),
	m_shapes		(new shape_vector)
{
    declareInterface<ILayerAllocator>(this);
    declareProperty("DetectorSelection",	m_detectorSelection);
    declareProperty("pixelName",		m_pixelName);
    declareProperty("sctName",			m_sctName);
}

LayerAllocator::~LayerAllocator(void)
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
LayerAllocator::initialize()
{
    // print where you are
    ATH_MSG_INFO( "LayerAllocator::initialize()"
		  << " - package version " << PACKAGE_VERSION );

    
    // get the Tools
    if (m_detectorSelection.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_detectorSelection );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_detectorSelection );
    }

    // Retrieve GeoModel Detector Elements
    if (m_detectorSelection->processPixels())
    {
	if (StatusCode::SUCCESS != detStore()->retrieve(m_manager,m_pixelName))
	{
	    ATH_MSG_FATAL( "Could not find the pixel Manager: "
			   << m_pixelName << " !" );
	    return StatusCode::FAILURE;
	}
	else
	{
	    // create pixel rings
	    m_versionName = m_manager->getVersion().name();
	    makeRings();
	    ATH_MSG_INFO( " made pixel rings. Version name " << m_versionName );
	}
    }
    if (m_detectorSelection->processSCT())
    {
	if (StatusCode::SUCCESS != detStore()->retrieve(m_manager,m_sctName))
	{
	    ATH_MSG_FATAL( "Could not find the SCT Manager: "
			   << m_sctName << " !" );
	    return StatusCode::FAILURE;
	}
	else
	{
	    // create sct rings
	    m_versionName = m_manager->getVersion().name();
	    makeRings();
	    ATH_MSG_INFO( " made sct rings. Version name " << m_versionName );
	}
    }
    
    // finalise the rings then make layers
    finaliseRings();
    makeLayers();

    // put containers in the order required by the access methods
    std::sort (m_pixelBarrel.begin(), m_pixelBarrel.end(), SiliconLayer::OutwardsOrder());
    std::sort (m_pixelEndcap.begin(), m_pixelEndcap.end(), SiliconLayer::OutwardsOrder());
    std::sort (m_sctBarrel.begin(), m_sctBarrel.end(), SiliconLayer::OutwardsOrder());
    std::sort (m_sctEndcap.begin(), m_sctEndcap.end(), SiliconLayer::OutwardsOrder());
    
    return StatusCode::SUCCESS;
}

StatusCode
LayerAllocator::finalize()
{
    // release memory
    // first the phi-wrapped detectors
    for (ring_iterator r = m_ringVector->begin();
     	 r != m_ringVector->end();
     	 ++r)
    {
	for (std::vector<SiliconDetector*>::const_iterator d = (**r).detectors().begin();
	     d != (**r).detectors().end();
	     ++d)
	{
	    if (std::find(m_detectors.begin(), m_detectors.end(), *d) == m_detectors.end())
		delete *d;
	}
    }

    // remaining detectors
    for (detector_vector::iterator d = m_detectors.begin();
	 d != m_detectors.end();
	 ++d)
	delete *d;

    // now the layers
    for (layer_iterator l = m_pixelBarrel.begin();
	 l != m_pixelBarrel.end();
	 ++l)
	delete *l;
    for (layer_iterator l = m_pixelEndcap.begin();
	 l != m_pixelEndcap.end();
	 ++l)
	delete *l;
    for (layer_iterator l = m_supportCylinders.begin();
	 l != m_supportCylinders.end();
	 ++l)
	delete *l;
    for (layer_iterator l = m_sctBarrel.begin();
	 l != m_sctBarrel.end();
	 ++l)
	delete *l;
    for (layer_iterator l = m_sctEndcap.begin();
	 l != m_sctEndcap.end();
	 ++l)
	delete *l;

    // the rings
    for (ring_iterator r = m_ringVector->begin();
	 r != m_ringVector->end();
	 ++r)
	delete *r;
    delete m_ringVector;

    // and the shapes
    for (shape_citerator s = m_shapes->begin();
	 s != m_shapes->end();
	 ++s)
	delete *s;
    delete m_shapes;
    
    return StatusCode::SUCCESS;
}


void
LayerAllocator::refresh (void)
{
    // refresh all active layers (pixel + SCT)
    for (layer_iterator l = m_pixelBarrel.begin();
	 l != m_pixelBarrel.end();
	 ++l)
	(**l).refresh();
    
    for (layer_iterator l = m_pixelEndcap.begin();
	 l != m_pixelEndcap.end();
	 ++l)
	(**l).refresh();
    
    for (layer_iterator l = m_sctBarrel.begin();
	 l != m_sctBarrel.end();
	 ++l)
	(**l).refresh();
    
    for (layer_iterator l = m_sctEndcap.begin();
	 l != m_sctEndcap.end();
	 ++l)
	(**l).refresh();

}

LayerAllocator::layer_vector*
LayerAllocator::siliconLayers (double r0, double z0, double cot_theta) const
{
    layer_vector* layers = new layer_vector;

    // assume forwards/backwards symmetry
    if (cot_theta < 0.)
    {
	z0		= -z0;
	cot_theta	= -cot_theta;
    }

    // allow tolerance for r/z - somewhat larger for off-vertex tracks
    double		tolerance		= 15.*CLHEP::mm;
    if (r0 > 30.*CLHEP::mm)	tolerance	= 30.*CLHEP::mm;
    double 	       	z_startBarrelMin	= z0 - tolerance;
    double 	       	z_startEndcapMax	= z0 + tolerance*cot_theta;
    double 	       	z_startEndcapMin	= z0 - tolerance*cot_theta;

    // first pixel barrel
    layer_citerator	l 			= m_pixelBarrel.begin();
    while (l != m_pixelBarrel.end()
	   && (*l)->zMax() > z_startBarrelMin + (*l)->rMin()*cot_theta)
    {
	layers->push_back(*l);
	++l;
    }

    // then pixel endcap
    l = m_pixelEndcap.begin();
    while (l != m_pixelEndcap.end()
	   && (**l).zMin() < z_startEndcapMax + (**l).rMax()*cot_theta)
    {
	if ((**l).zMax() > z_startEndcapMin + (**l).rMin()*cot_theta)
	    layers->push_back(*l);
	++l;
    }

    // pixel outer supports structure
    SiliconLayer* support = 0;
    for (l = m_supportCylinders.begin(); l != m_supportCylinders.end(); ++l)
    {
	if (! (**l).isPixel()) break;
	double z = fabs(z0 + (*l)->rMin()*cot_theta);
	if (z < (*l)->zMax() && z > (*l)->zMin()) support = *l;
    }
    if (support) layers->push_back(support);

    // sct barrel
    l = m_sctBarrel.begin();
    while (l != m_sctBarrel.end()
	   && (*l)->zMax() > z_startBarrelMin + (*l)->rMin()*cot_theta)
    {
	layers->push_back(*l);
	++l;
    }

    // sct endcap (complications as includes end of barrel services)
    l = m_sctEndcap.begin();
    double zInactive = 0.;
    while (l != m_sctEndcap.end())
    {
	if ((**l).zMin()	< z_startEndcapMax + (**l).rMax()*cot_theta
	    && (**l).zMax()	> z_startEndcapMin + (**l).rMin()*cot_theta)
	{
	    if ((**l).isActive())
	    {
		layers->push_back(*l);
	    }
	    // avoid double counting at material boundary
	    else if ((**l).zMin() > zInactive)
	    {
		layers->push_back(*l);
		zInactive = (**l).zMax();
	    }
	}
	++l;
    }

    // endcap outer support cylinder (just outside active SCT endcap)
    support = *m_supportCylinders.rbegin();
    if (! support->isPixel())
    {
	double z = fabs(z0 + support->rMin()*cot_theta);
	if (z < support->zMax() && z > support->zMin()) layers->push_back(support);
    }

    return layers;
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
LayerAllocator::finaliseRings(void)
{
    // sort rings in ~ outward order
    std::sort (m_ringVector->begin(), m_ringVector->end(), SiliconRing::outwardsOrder());
    
    // finalise the ring definition
    ring_vector* ringsWithParity = new ring_vector;
    for (ring_iterator i = m_ringVector->begin();
	 i != m_ringVector->end();
	 ++i)
    {
	(**i).determinePhiProperties();
	if ((**i).hasPhiParity())
	{
	    ringsWithParity->push_back((*i));
	}
	else
	{
	    ringsWithParity->push_back((**i).copyWithParity(true));
	    ringsWithParity->push_back((**i).copyWithParity(false));
	    delete *i;
	}
    }

    // now can remove rings with indefinite parity
    delete m_ringVector;
    m_ringVector 	= ringsWithParity;
    ringsWithParity	= 0;

    // print the result
//      for (ring_iterator i = m_ringVector->begin();
//      	 i != m_ringVector->end();
//      	 ++i)
//      {
//      	(**i).print();
//      }
    
}

void
LayerAllocator::makeLayers(void)
{
    // create some inactive material layers
    if (m_versionName != "CTB2004")
    {
	// beam pipe material layer (double skinned)
	m_pixelBarrel.push_back(new SiliconLayer("beamPipe",29.4*CLHEP::mm,-3400.0*CLHEP::mm,3400.0*CLHEP::mm));
	m_pixelBarrel.push_back(new SiliconLayer("beamPipe",33.4*CLHEP::mm,-3400.0*CLHEP::mm,3400.0*CLHEP::mm));

	// insert a material layer representing barrel support/services 
	// as first layers of pixel and SCT endcaps
 	m_pixelEndcap.push_back(new SiliconLayer("pixelServiceDisk",400.0*CLHEP::mm,80.0*CLHEP::mm,160.0*CLHEP::mm));
	m_sctEndcap.push_back(new SiliconLayer("sctServiceDisk0",800.0*CLHEP::mm,258.0*CLHEP::mm,305.0*CLHEP::mm));
	m_sctEndcap.push_back(new SiliconLayer("sctServiceDisk1",800.0*CLHEP::mm,305.0*CLHEP::mm,500.0*CLHEP::mm));
	m_sctEndcap.push_back(new SiliconLayer("sctServiceDisk2",800.0*CLHEP::mm,500.0*CLHEP::mm,610.0*CLHEP::mm));
	
	// and cylinders in front of the SCT barrel representing the 
	// insertable pixel support structure
	// and in front of the TRT endcap representing the SCT services
	m_supportCylinders.push_back(new SiliconLayer("pixelBarrelServiceCylinder",
						      200.0*CLHEP::mm, -400.0*CLHEP::mm,  400.0*CLHEP::mm));
	m_supportCylinders.push_back(new SiliconLayer("pixelEndCapServiceCylinder",
						      200.0*CLHEP::mm,  400.0*CLHEP::mm,  468.0*CLHEP::mm));
	m_supportCylinders.push_back(new SiliconLayer("pixelSupportCylinder",
						      200.0*CLHEP::mm,  468.0*CLHEP::mm, 3400.0*CLHEP::mm));
	m_supportCylinders.push_back(new SiliconLayer("outerSupportCylinder",
						      600.0*CLHEP::mm,  800.0*CLHEP::mm, 3400.0*CLHEP::mm));
    }

    // loop over SiliconRing's and combine as appropriate to form layers,
    // distinguishing barrel/end-cap and pixel/SCT layers
    for (ring_iterator i = m_ringVector->begin();
	 i != m_ringVector->end();
	 ++i)
    {
	if ((**i).isPixel())
	{
	    if ((**i).isBarrel())
	    {
		if (m_pixelBarrel.begin() == m_pixelBarrel.end()
		    || !(**m_pixelBarrel.rbegin()).addRing(*i))
		    m_pixelBarrel.push_back(new SiliconLayer(*i));
	    }
	    else
	    {
		if (m_pixelEndcap.begin() == m_pixelEndcap.end()
		    || !(**m_pixelEndcap.rbegin()).addRing(*i))
		    m_pixelEndcap.push_back(new SiliconLayer(*i));
	    }
	}
	else
	{
	    if ((**i).isBarrel())
	    {
		if (m_sctBarrel.begin() == m_sctBarrel.end()
		    || !(**m_sctBarrel.rbegin()).addRing(*i))
		    m_sctBarrel.push_back(new SiliconLayer(*i));
	    }
	    else
	    {
		if (m_sctEndcap.begin() == m_sctEndcap.end()
		    || !(**m_sctEndcap.rbegin()).addRing(*i))
		    m_sctEndcap.push_back(new SiliconLayer(*i));
	    }
	}
    }

    // can now determine the properties of each layer
    for (layer_iterator i = m_pixelBarrel.begin();
	 i != m_pixelBarrel.end();
	 ++i)
    {
	(**i).determineRzProperties();
	(**i).determineMaterial(m_versionName);
    }

    for (layer_iterator i = m_pixelEndcap.begin();
	 i != m_pixelEndcap.end();
	 ++i)
    {
	(**i).determineRzProperties();
	(**i).determineMaterial(m_versionName);
    }
    
    for (layer_iterator i = m_supportCylinders.begin();
	 i != m_supportCylinders.end();
	 ++i)
    {
	(**i).determineRzProperties();
	(**i).determineMaterial(m_versionName);
    }
    
    for (layer_iterator i = m_sctBarrel.begin();
	 i != m_sctBarrel.end();
	 ++i)
    {
	(**i).determineRzProperties();
	(**i).determineMaterial(m_versionName);
    }
    
    for (layer_iterator i = m_sctEndcap.begin();
	 i != m_sctEndcap.end();
	 ++i)
    {
	(**i).determineRzProperties();
	(**i).determineMaterial(m_versionName);
    }
    
}

void
LayerAllocator::makeRings(void)
{
    // iterate over all silicon detectors and group them into rings
    SiliconDetectorBuilder builder;
    for (InDetDD::SiDetectorElementCollection::const_iterator
	     elem = m_manager->getDetectorElementBegin();
	 elem != m_manager->getDetectorElementEnd();
	 ++elem)
    {
	if (!*elem) continue;
	SiliconDetector* detector = builder.detector(**elem);
	m_detectors.push_back(detector);
	DetectorShape* 	shape = builder.shape();
	
	// add to existing SiliconRing in case of overlap
	ring_riterator r = m_ringVector->rbegin();
	while (r != m_ringVector->rend())
	{
	    if ((**r).addDetector (detector,shape)) break;
	    ++r;
	}

	// else create a new SiliconRing
	if (r == m_ringVector->rend()) m_ringVector->push_back(new SiliconRing(detector,shape));
    }

    // take over ownership of the shapes (one shape is common to many detectors)
    for (shape_citerator s = builder.shapes().begin();
	 s != builder.shapes().end();
	 ++s)
	m_shapes->push_back(*s);
}

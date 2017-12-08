/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 AlgTool to find the iPatGeometry Detector corresponding to a given hashId
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "iPatGeometryTools/SiDetectorLocator.h"
#include "iPatInterfaces/ILayerAllocator.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

SiDetectorLocator::SiDetectorLocator (const std::string&	type,
				      const std::string&	name, 
				      const IInterface*		parent)
    :	AthAlgTool		(type, name, parent),
	m_layerAllocator	("LayerAllocator/LayerAllocator")
{
    declareInterface<ISiDetectorLocator>(this);
    declareProperty("LayerAllocator",	m_layerAllocator);
}

SiDetectorLocator::~SiDetectorLocator (void) 
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
SiDetectorLocator::initialize()
{
    ATH_MSG_DEBUG( "SiDetectorLocator::initialize() - package version " << PACKAGE_VERSION );

    // set the Identifier Helpers from detStore
    if (StatusCode::SUCCESS != detStore()->retrieve(m_pixelHelper,"PixelID"))
    {
        ATH_MSG_FATAL( "Could not retrieve PixelID " );
	return StatusCode::FAILURE;
    }
    if (StatusCode::SUCCESS != detStore()->retrieve(m_SCT_Helper,"SCT_ID"))
    {
        ATH_MSG_FATAL( "Could not retrieve SCT_ID " );
	return StatusCode::FAILURE;
    }

    // get the Tools
    if (m_layerAllocator.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_layerAllocator );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_layerAllocator );
    }

    for (std::vector<SiliconDetector*>::const_iterator d = m_layerAllocator->detectors().begin();
	 d != m_layerAllocator->detectors().end();
	 ++d)
    {
	if ((**d).ring().isPixel())
	{
	    m_pixelDetectors.push_back(*d);
	}
	else
	{
	    m_SCT_Detectors.push_back(*d);
	}
    }

    // order detectors by hashId and check the hashId contiguity
    if (m_pixelDetectors.size())
    {
	std::sort (m_pixelDetectors.begin(), m_pixelDetectors.end(), SiliconDetector::HashOrder());
	unsigned maxHash = (**m_pixelDetectors.rbegin()).identifyHash();
	if (maxHash + 1 != m_pixelDetectors.size())
	{
	    ATH_MSG_FATAL( " attempt to initialize SiDetectorLocator with "
			   << "invalid Pixel hashId. Check configuration of Identifier Dictionary. "
			   << endmsg
			   << " max HashId: " << maxHash
			   << " should be equal to (number of detectors-1): "
			   << (m_pixelDetectors.size()-1) );

	    return StatusCode::FAILURE;
	}
    }

    if (m_SCT_Detectors.size())
    {
	std::sort (m_SCT_Detectors.begin(), m_SCT_Detectors.end(), SiliconDetector::HashOrder());
	unsigned maxHash = (**m_SCT_Detectors.rbegin()).identifyHash();
	if (maxHash + 1 != m_SCT_Detectors.size())
	{
	    ATH_MSG_FATAL( " attempt to initialize SiDetectorLocator with "
			   << "invalid SCT hashId. Check configuration of Identifier Dictionary. "
			   << endmsg
			   << " max HashId: " << maxHash
			   << " should be equal to (number of detectors-1): "
			   << (m_SCT_Detectors.size()-1) );
	    return StatusCode::FAILURE;
	}
    }
    
    ATH_MSG_INFO( "SiDetectorLocator initialized with " << m_pixelDetectors.size() 
		  << " pixel and " << m_SCT_Detectors.size() << " SCT detectors" );
    return StatusCode::SUCCESS;
}

StatusCode
SiDetectorLocator::finalize()
{
    return StatusCode::SUCCESS;
}

IdentifierHash
SiDetectorLocator::SCT_Hash (const Identifier& id) const
{
    IdContext context = m_SCT_Helper->wafer_context();
    IdentifierHash hash;
    int status = m_SCT_Helper->get_hash(id,hash,&context);
    if (status) throw std::runtime_error("SiDetectorLocator: Unable to set IdentifierHash");
    return hash;
}

IdentifierHash
SiDetectorLocator::pixelHash (const Identifier& id) const
{
    IdContext context = m_pixelHelper->wafer_context();
    IdentifierHash hash;
    int status = m_pixelHelper->get_hash(id,hash,&context);
    if (status) throw std::runtime_error("SiDetectorLocator: Unable to set IdentifierHash");
    return hash;
}


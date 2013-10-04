/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 AlgTool to find the iPatGeometry Detector corresponding to a given hashId
 ***************************************************************************/

#ifndef IPATGEOMETRYTOOLS_SIDETECTORLOCATOR_H
# define IPATGEOMETRYTOOLS_SIDETECTORLOCATOR_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <vector>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "Identifier/IdentifierHash.h"
#include "iPatGeometry/SiliconDetector.h"
#include "iPatInterfaces/ISiDetectorLocator.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class ILayerAllocator;
class PixelID;
class SCT_ID;

class SiDetectorLocator: public AthAlgTool,
			 virtual public ISiDetectorLocator
{

public:
    SiDetectorLocator		(const std::string& type, 
				 const std::string& name,
				 const IInterface* parent);
    ~SiDetectorLocator		(void); 	// destructor

    StatusCode			initialize();
    StatusCode			finalize();

    const SiliconDetector&				SCT_Detector (IdentifierHash hashId) const;
    IdentifierHash					SCT_Hash (const Identifier& id) const;
    std::vector<SiliconDetector*>::const_iterator	beginPixel (void) const; 
    std::vector<SiliconDetector*>::const_iterator	beginSCT (void) const; 
    std::vector<SiliconDetector*>::const_iterator	endPixel (void) const;
    std::vector<SiliconDetector*>::const_iterator	endSCT (void) const; 
    const SiliconDetector&				pixelDetector (IdentifierHash hashId) const;
    IdentifierHash					pixelHash (const Identifier& id) const;

private:
      
    // helpers, managers, tools
    ToolHandle<ILayerAllocator>		m_layerAllocator;
    const SCT_ID*			m_SCT_Helper;
    const PixelID*			m_pixelHelper;

    // state
    std::vector<SiliconDetector*>	m_SCT_Detectors;
    std::vector<SiliconDetector*>	m_pixelDetectors;
};

//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline const SiliconDetector&
SiDetectorLocator::SCT_Detector (IdentifierHash hashId) const
{ return **(m_SCT_Detectors.begin() + hashId); }

inline std::vector<SiliconDetector*>::const_iterator
SiDetectorLocator::beginPixel (void) const
{ return m_pixelDetectors.begin(); }

inline std::vector<SiliconDetector*>::const_iterator
SiDetectorLocator::beginSCT (void) const
{ return m_SCT_Detectors.begin(); }

inline std::vector<SiliconDetector*>::const_iterator
SiDetectorLocator::endPixel (void) const
{ return m_pixelDetectors.end(); }

inline std::vector<SiliconDetector*>::const_iterator
SiDetectorLocator::endSCT (void) const
{ return m_SCT_Detectors.end(); }

inline const SiliconDetector&
SiDetectorLocator::pixelDetector (IdentifierHash hashId) const
{ return **(m_pixelDetectors.begin() + hashId); }

#endif // IPATGEOMETRYTOOLS_SIDETECTORLOCATOR_H


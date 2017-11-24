/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 AlgTool describing silicon layer/ring/detector geometry structure.
 Used to retrieve the layer/ring/detector intersected by a track vector.
 --------------------------------------------------------------------------
 ***************************************************************************/

#ifndef IPATGEOMETRYTOOLS_LAYERALLOCATOR_H
# define IPATGEOMETRYTOOLS_LAYERALLOCATOR_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "iPatGeometry/SiliconLayer.h"
#include "iPatGeometry/SiliconRing.h"
#include "iPatInterfaces/ILayerAllocator.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class IDetectorSelection;
class StoreGateSvc;
namespace InDetDD
{
    class SiDetectorManager;
}

class LayerAllocator: public AthAlgTool,
		      virtual public ILayerAllocator
{

public:
    LayerAllocator				(const std::string& type, 
						 const std::string& name,
						 const IInterface* parent);
    ~LayerAllocator 				(void); 	// destructor
    
    StatusCode					initialize();
    StatusCode					finalize();
  
    // access to silicon detectors
    const std::vector<SiliconDetector*>&	detectors(void) const;
    
    // access to layers forming pixel/sct barrel/endcap
    const std::vector<SiliconLayer*>&		pixelBarrel(void) const;
    const std::vector<SiliconLayer*>&		pixelEndcap(void) const;
    const std::vector<SiliconLayer*>&		sctBarrel(void) const;
    const std::vector<SiliconLayer*>&		sctEndcap(void) const;
    const std::vector<SiliconLayer*>&		supportCylinders(void) const;
    
    // refresh cached data whenever conditions change
    void					refresh (void);
    
    // layers intersected by a straight line in rz-projection
    std::vector<SiliconLayer*>*			siliconLayers (double r0,
							       double z0,
							       double cotTheta) const;
    
private:
    typedef	std::vector<SiliconDetector*>		detector_vector;
    typedef	std::vector<SiliconLayer*>		layer_vector;
    typedef	layer_vector::iterator			layer_iterator;
    typedef	layer_vector::const_iterator		layer_citerator;
    typedef	std::vector<SiliconRing*>		ring_vector;
    typedef	ring_vector::iterator			ring_iterator;
    typedef	ring_vector::reverse_iterator 		ring_riterator;
    typedef	std::vector<DetectorShape*>		shape_vector;
    typedef	shape_vector::const_iterator		shape_citerator;
    
    void			finaliseRings (void);
    void			makeLayers (void);
    void			makeRings (void);

    // helpers, managers, tools
    ToolHandle<IDetectorSelection>	m_detectorSelection;
    detector_vector			m_detectors;
    const InDetDD::SiDetectorManager*	m_manager;
    
    // configuration
    std::string				m_pixelName;
    std::string				m_sctName;

    // state
    layer_vector			m_pixelBarrel;
    layer_vector			m_pixelEndcap;
    ring_vector*			m_ringVector;
    layer_vector			m_sctBarrel;
    layer_vector			m_sctEndcap;
    shape_vector*			m_shapes;
    layer_vector			m_supportCylinders;
    std::string				m_versionName;

};   

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline const LayerAllocator::detector_vector&
LayerAllocator::detectors() const
{ return m_detectors; }

inline const LayerAllocator::layer_vector&
LayerAllocator::pixelBarrel() const
{ return m_pixelBarrel; }

inline const LayerAllocator::layer_vector&
LayerAllocator::pixelEndcap() const
{ return m_pixelEndcap; }
  
inline const LayerAllocator::layer_vector&
LayerAllocator::supportCylinders() const
{ return m_supportCylinders; }
    
inline const LayerAllocator::layer_vector&
LayerAllocator::sctBarrel() const
{ return m_sctBarrel; }

inline const LayerAllocator::layer_vector&
LayerAllocator::sctEndcap() const
{ return m_sctEndcap; }

#endif // IPATGEOMETRYTOOLS_LAYERALLOCATOR_H

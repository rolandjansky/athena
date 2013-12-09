/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PointManager.h, (c) ATLAS iPatRec
//
// allocate SpacePoint's into a point_vector per 'idealized' layer.
// These are further sub-divided into projective eta-slices.
// FIXME: currently hard wired to be 7 layers and 8 eta-slices
//
///////////////////////////////////////////////////////////////////

#ifndef IPATTRACKFINDER_POINTMANAGER_H
# define IPATTRACKFINDER_POINTMANAGER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "iPatInterfaces/IPointManager.h"
#include "iPatTrackFinder/Point.h"
#include "iPatTrackFinder/PointGroup.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class IFinderConfiguration;
class ILayerNumberAllocator;
class ISiDetectorLocator;

class PointManager: public AthAlgTool,
		    virtual public IPointManager
{
    
public:
    PointManager		(const std::string&	type,
				 const std::string&	name,
				 const IInterface*	parent);
    ~PointManager		(void);

    StatusCode			initialize();
    StatusCode			finalize();

    void			clear (void);
    void			makeEtaSlices (double vertex, double halfWidth);
    void			newSearch (void);
    bool			nextSlice (void);
    std::vector<Point*>*	points (int layer) const;
    double			radius (int layer) const;
    void			setPoints (const std::vector<Trk::SpacePoint*>&	spacePoints,
					   const Amg::Vector3D&			vertexRegionCentre);
    
private:
    // point_vector for each 'idealized' layer = layer_vector
    // layer_vector for each eta_slice = slice_vector
    typedef     PointGroup::point_vector			point_vector;
    typedef     PointGroup::point_citerator	       		point_citerator;
    typedef     std::vector<PointGroup*>		  	layer_vector;
    typedef	layer_vector::iterator				layer_iterator;
    typedef	layer_vector::reverse_iterator			layer_riterator;
    typedef     std::vector<layer_vector*>			slice_vector;
    typedef	slice_vector::iterator				slice_iterator;

    // helpers, managers, tools
    ToolHandle<ISiDetectorLocator>	m_detectorLocator;
    ToolHandle<IFinderConfiguration>	m_finderTolerances;
    ToolHandle<ILayerNumberAllocator>	m_layerNumberAllocator;
    
    PointGroup*	                        m_extendedPoints;
    layer_vector			m_layerContainer;
    int					m_numberOfCombinedLayers;
    int					m_numberOfLayers;
    int					m_numberOfSlices;
    slice_vector			m_sliceContainer;
    slice_iterator			m_sliceIterator;
};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>

inline void
PointManager::newSearch (void)
{ m_sliceIterator = m_sliceContainer.end() - 1; }

#endif

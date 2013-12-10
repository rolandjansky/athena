/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ICombinationMaker.h, (c) ATLAS iPatRec
//
//  make SpacePoint combinatorials
//    (from vertex, inner, intermediate and outer layers)
//
///////////////////////////////////////////////////////////////////

#ifndef IPATINTERFACES_COMBINATIONMAKER_H
# define IPATINTERFACES_COMBINATIONMAKER_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"

/** Interface ID for ICombinationMaker*/  
static const InterfaceID IID_ICombinationMaker("ICombinationMaker", 1, 0);
  
/**@class ICombinationMaker

Abstract base class for CombinationMaker AlgTool
      
@author Alan.Poppleton@cern.ch
*/

namespace Trk	{ class SpacePoint; }
class PerigeeParameters;
class Point;

class ICombinationMaker: virtual public IAlgTool
{

public:
    /**Virtual destructor*/
    virtual ~ICombinationMaker(){}

    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID&	interfaceID() { return IID_ICombinationMaker; }

    /**ICombinationMaker interface: release memory	*/
    virtual void				clear (void) = 0;
    
    /**ICombinationMaker interface:
       each search pattern has an initial processing step	*/
    virtual bool				doublePixelPrimarySearch (void) = 0;
    virtual bool				doublePixelTruncatedSearch (void) = 0;
    virtual bool				secondarySearch (void) = 0;
    virtual bool				singlePixelSearch (void) = 0;
    virtual bool				vertexSearch (double minPt) = 0;

    /**ICombinationMaker interface:
       the following access methods are only valid during a given search pattern: */
    virtual bool				has_vertex_points (void) const = 0;
    virtual Point*			       	inner_point (void) const = 0;
    virtual void				intermediatePhiRange (void) = 0;
    virtual Point*			       	intermediate_point (void) const = 0;
    virtual bool				next_inner_point (void) = 0;
    virtual bool				next_intermediate_point (void) = 0;
    virtual bool				next_outer_point (void) = 0;
    virtual bool				next_vertex_point (void) = 0;
    virtual Point*			       	outer_point (void) const = 0;
    virtual bool				pixelInterchange (void) const = 0;
    virtual void				setProperties (
	const std::vector<Trk::SpacePoint*>&	spacePoints,
	const Amg::Vector3D&			vertexRegionCentre) = 0;
    virtual void				set_tolerances (double minPt) = 0;
    virtual void				set_vertex_points (
	const PerigeeParameters& perigeeParameters) = 0;
    virtual Point*			       	vertex_point (void) const = 0;
    virtual void				vertex_region (double vertex, double half_width) = 0;
};

#endif

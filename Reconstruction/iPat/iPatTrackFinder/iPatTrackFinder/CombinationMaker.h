/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CombinationMaker.h, (c) ATLAS iPatRec
//
//  make SpacePoint combinatorials
//    (from vertex, inner, intermediate and outer layers)
//
///////////////////////////////////////////////////////////////////

#ifndef IPATTRACKFINDER_COMBINATIONMAKER_H
# define IPATTRACKFINDER_COMBINATIONMAKER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <vector>
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "iPatInterfaces/ICombinationMaker.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class IAssignedHits;
class IDetectorSelection;
class IFinderConfiguration;
class ILayerNumberAllocator;
class IPointManager;

class CombinationMaker: public AthAlgTool,
			virtual public ICombinationMaker
{

public:
    CombinationMaker	(const std::string&	type,
			 const std::string&	name,
			 const IInterface*	parent);
  
    ~CombinationMaker	(void);
  
    StatusCode		initialize();
    StatusCode		finalize();

    // release memory
    void				clear (void);
    
    // each search pattern has an initial processing step:
    bool				doublePixelPrimarySearch (void);
    bool				doublePixelTruncatedSearch (void);
    bool				secondarySearch (void);
    bool				singlePixelSearch (void);
    bool				vertexSearch (double minPt);

    // the following access methods are valid during a given search pattern:
    bool				has_vertex_points (void) const;
    Point*			       	inner_point (void) const;
    void				intermediatePhiRange (void);
    Point*			       	intermediate_point (void) const;
    bool				next_inner_point (void);
    bool				next_intermediate_point (void);
    bool				next_outer_point (void);
    bool				next_vertex_point (void);
    Point*			       	outer_point (void) const;
    bool				pixelInterchange (void) const;
    void				setProperties (const std::vector<Trk::SpacePoint*>&	spacePoints,
						       const Amg::Vector3D&			vertexRegionCentre);
    void				set_tolerances (double minPt);
    void				set_vertex_points (const PerigeeParameters& perigeeParameters);
    Point*			       	vertex_point (void) const;
    void				vertex_region (double vertex, double half_width);
 
    typedef	std::vector<Point*>	point_vector;
    
private:
    typedef	point_vector::iterator			point_iterator;
    typedef	std::vector<point_vector>::iterator	vector_iterator;
    typedef	std::vector<int>			search_pattern;
    typedef	std::vector<search_pattern>		search_order;

    void				addSearchPattern (int outerLayer,
							  int innerLayer,
							  int middleLayer,
							  int vertexLayer);
    bool				newSearch (void);
    void				printSearch (void) const;
    StatusCode				setSearchPatterns (void);

    // helpers, managers, tools
    ToolHandle<IAssignedHits>		m_assignedHits;
    ToolHandle<IDetectorSelection>	m_detectorSelection;
    ToolHandle<IFinderConfiguration>	m_finderTolerances;
    ToolHandle<ILayerNumberAllocator>	m_layerNumberAllocator;
    ToolHandle<IPointManager>		m_pointManager;
    
    bool				m_allowPrimaryShare;
    double				m_deltaPhiInner;
    double				m_deltaPhiMiddle;
    search_order::const_iterator	m_doublePixelPrimarySearchBegin;
    search_order::const_iterator	m_doublePixelPrimarySearchEnd;
    search_order::const_iterator	m_doublePixelTruncatedSearchBegin;
    search_order::const_iterator	m_doublePixelTruncatedSearchEnd;
    double				m_maxCurvature;
    double				m_maxImpact;
    bool				m_middleInterchange;
    double				m_middleR;
    double				m_middleTolerance;
    unsigned				m_numberOfSearches;
    bool				m_pixelInterchange;
    double				m_radialSpread;
    search_order::const_iterator	m_search;
    search_order::const_iterator	m_searchEnd;
    search_order			m_searchOrder;
    search_order::const_iterator	m_secondarySearchBegin;
    search_order::const_iterator	m_secondarySearchEnd;
    search_order::const_iterator	m_singlePixelSearchBegin;
    search_order::const_iterator	m_singlePixelSearchEnd;
    point_vector			m_vertexPoints;
    search_order::const_iterator	m_vertexSearchBegin;
    search_order::const_iterator	m_vertexSearchEnd;
    point_iterator			m_p;     // by layer
    point_iterator			m_pEnd; 
    point_iterator			m_o;     // outer
    point_iterator                      m_oEnd;
    point_iterator			m_m;     // intermediate
    point_iterator                      m_mEnd;
    point_iterator			m_mFirst; 
    point_iterator			m_i;     // inner
    point_iterator                      m_iEnd;
    point_iterator			m_iFirst;
    point_iterator			m_v;     // vertex
    point_iterator                      m_vEnd;
    double				m_iPhiMax;
    double				m_mPhiMax;
    double				m_vPhiTolerance;
    // keep some statistics
    int					m_outerCombinations;
    int					m_innerCombinations;
    int					m_intermediateCombinations;
    int					m_vertexCombinations;

};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
 
inline bool
CombinationMaker::has_vertex_points (void) const
{ return (m_search->size() == 4); }
    
inline Point*
CombinationMaker::inner_point (void) const
{ return *m_i; }

inline Point*
CombinationMaker::intermediate_point (void) const
{ return *m_m; }

inline Point*
CombinationMaker::outer_point (void) const
{ return *m_o; }

inline bool
CombinationMaker::pixelInterchange (void) const
{ return m_pixelInterchange; }

inline Point*
CombinationMaker::vertex_point (void) const
{ return *m_v; }

#endif

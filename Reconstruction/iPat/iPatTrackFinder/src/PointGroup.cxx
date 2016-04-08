/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 a collection of Point's with some overall properties.
 Usually grouped according to layer, eta slice etc
 ***************************************************************************/

//<doc><file>	$Id: PointGroup.cxx,v 1.1 2004-11-11 23:59:52 pop Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include <algorithm>
#include "iPatTrackFinder/Point.h"
#include "iPatTrackFinder/PointGroup.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

PointGroup::PointGroup (int size)
    :   m_radius		(0.)
{
    m_points.reserve(size);
//     std::cout << " PointGroup:: capacity " << m_points.capacity() << std::endl;
}

PointGroup::PointGroup (const PointGroup& group1, const PointGroup& group2)
    :   m_radius		(0.)
{
    m_points.reserve(group1.size()+group2.size());
    std::merge (group1.begin(),
		group1.end(),
		group2.begin(),
		group2.end(),
		std::back_inserter(m_points),
		ComparePointByPhi());
//     std::cout << " PointGroup:: capacity " << m_points.capacity() << std::endl;
}

PointGroup::~PointGroup (void)
{
    m_points.clear();
}

//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>

void
PointGroup::clearWithDelete (void)
{
    for (point_iterator p = m_points.begin();
	 p != m_points.end();
	 ++p)
	delete *p;
    m_points.clear();
}

void
PointGroup::prepare (void)
{
    // compute mean radius
    m_radius = 0.;
    for (point_citerator p = m_points.begin();
	 p != m_points.end();
	 ++p)
    {
	m_radius += (**p).r();
    }
    unsigned numPoints	= m_points.size();
    if (numPoints > 0) m_radius /= static_cast<double>(numPoints);
    // std::cout << " meanRadius " << m_radius << std::endl; 
}

void
PointGroup::sort (void)
{
    // sort by ascending phi
    std::sort (m_points.begin(), m_points.end(), ComparePointByPhi());
}

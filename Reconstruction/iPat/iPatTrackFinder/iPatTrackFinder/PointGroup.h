/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 a collection of Point's with some overall properties.
 Usually grouped according to layer, eta slice etc
 ***************************************************************************/

//<doc><file>	$Id: PointGroup.h,v 1.2 2009-03-06 13:50:57 pop Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef IPATTRACKFINDER_POINTGROUP_H
# define IPATTRACKFINDER_POINTGROUP_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <vector>

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class Point;

class PointGroup
{
    
public:
    typedef     std::vector<Point*>			point_vector;
    typedef     point_vector::const_iterator		point_citerator;
    typedef     point_vector::iterator			point_iterator;
    typedef     point_vector::reverse_iterator		point_riterator;

    PointGroup		(int size);			// reserve group size
    PointGroup		(const PointGroup& group1,
			 const PointGroup& group2);	// make compound group 
    ~PointGroup		(void);

    void		addPoint (Point* point);
    point_citerator	begin (void) const;
    void		clear (void);
    void		clearWithDelete (void);
    bool		empty (void) const;
    point_citerator	end (void) const;
    point_vector*	points (void);		// deprecated
    void		prepare (void);
    double		radius (void) const;
    unsigned		size (void) const;
    void		sort (void);
    
private:
    
    class ComparePointByPhi 
    {
    public:
	bool operator() (const Point* x, const Point* y) const
	{ return x->phi() < y->phi(); }
    };

    point_vector	m_points;
    double	       	m_radius;
  
    // void constructor, copy, assignment: no semantics, no implementation
    PointGroup (void);
    PointGroup (const PointGroup &);
    PointGroup& operator= (const PointGroup &);
};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>

inline void
PointGroup::addPoint (Point* point)
{ m_points.push_back(point); }

inline PointGroup::point_citerator
PointGroup::begin  (void) const
{ return m_points.begin(); }

inline void
PointGroup::clear (void)
{
    m_points.clear();
    m_radius = 0.;
}

inline bool
PointGroup::empty (void) const
{ return m_points.empty(); }

inline PointGroup::point_citerator
PointGroup::end  (void) const
{ return m_points.end(); }
  
inline PointGroup::point_vector*
PointGroup::points (void)
{ return &m_points; }
  
inline double
PointGroup::radius (void) const
{ return m_radius; }

inline unsigned
PointGroup::size (void) const
{ return m_points.size(); }

#endif

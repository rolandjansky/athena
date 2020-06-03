/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 assess quality = type and distribution of hits in list<HitOnTrack*>
 -------------------------------------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: HitQuality.h,v 1.13 2003-12-11 16:08:31 pop Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef IPATTRACK_HITQUALITY_H
# define IPATTRACK_HITQUALITY_H

//<<<<<< INCLUDES                                                       >>>>>>

//#include <algorithm>
#include <assert.h>
#include <math.h>
//#include <utility>
#include "iPatTrack/HitList.h"
#include "iPatTrack/HitOnTrack.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>

    typedef	HitList::hit_list		hit_list;
    typedef	HitList::hit_citerator		hit_const_iterator;
    typedef	HitList::hit_iterator		hit_iterator;
    typedef	HitList::hit_riterator		hit_reverse_iterator;

//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class HitQuality
{

public:

    HitQuality (void);
    HitQuality (HitList::hit_citerator hit_list_begin,
		HitList::hit_citerator hit_list_end);
    virtual ~HitQuality (void);
    
    // implicit copy constructor
    // implicit destructor
    // implicit assignment operator

    void			fill (HitList::hit_citerator hit_list_begin,
				      HitList::hit_citerator hit_list_end);
    int				missing_layers (void) const;
    int				number_dead (void) const;
    int		        	number_drift_hits (void) const;
    int		        	number_effective_layers (void) const;
    int		        	number_hits (void) const;
    int		        	number_planar_clusters (void) const;
    int				number_planar_holes (void) const;
    int		        	number_layers (void) const;
    void			print (void) const;
    bool			satisfies_inward_continuity (void) const;
    bool			satisfies_outward_continuity (void) const;
    
private:
    typedef	HitList::hit_citerator			hit_citerator;

    void			set_quality(hit_citerator hit_list_begin,
					    hit_citerator hit_list_end);

    bool			m_continuity_inwards;
    bool			m_continuity_outwards;
    int				m_missing_layers;
    int				m_number_dead;
    int				m_number_drift_hits;
    int				m_number_planar_clusters;
    int				m_number_planar_holes;
    int				m_number_layers;
};

//<<<<<< INLINE CLASS STRUCTURE INITIALIZATION                          >>>>>>

inline
HitQuality::HitQuality (void)
    :  	m_continuity_inwards		(true),
	m_continuity_outwards		(true),
	m_missing_layers		(0),
    	m_number_dead			(0),
    	m_number_drift_hits		(0),
    	m_number_planar_clusters	(0),
    	m_number_planar_holes		(0),
    	m_number_layers			(0)
{}

inline
HitQuality::HitQuality (hit_citerator hit_list_begin,
			hit_citerator hit_list_end)
    :  	m_continuity_inwards		(true),
	m_continuity_outwards		(true),
	m_missing_layers		(0),
    	m_number_dead			(0),
    	m_number_drift_hits		(0),
    	m_number_planar_clusters	(0),
    	m_number_planar_holes		(0),
    	m_number_layers		(0)
    
{
    set_quality(hit_list_begin,hit_list_end);
}

inline
HitQuality::~HitQuality (void)
{}

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline void
HitQuality::fill (hit_citerator hit_list_begin,
		  hit_citerator hit_list_end)
{
    m_continuity_inwards	= true;
    m_continuity_outwards	= true;
    m_missing_layers     	= 0;
    m_number_dead	       	= 0;
    m_number_drift_hits		= 0;
    m_number_planar_clusters	= 0;
    m_number_planar_holes      	= 0;
    m_number_layers       	= 0;
    set_quality(hit_list_begin,hit_list_end);
}

inline int
HitQuality::missing_layers (void) const
{ return m_missing_layers; }

inline int
HitQuality::number_dead (void) const
{ return m_number_dead; }

inline int
HitQuality::number_drift_hits (void) const
{ return m_number_drift_hits; }

inline int
HitQuality::number_effective_layers (void) const
{ return m_number_layers - m_missing_layers; }

inline int
HitQuality::number_hits (void) const
{ return m_number_drift_hits + m_number_planar_clusters; }

inline int
HitQuality::number_planar_clusters (void) const
{ return m_number_planar_clusters; }

inline int
HitQuality::number_planar_holes (void) const
{ return m_number_planar_holes; }

inline int
HitQuality::number_layers (void) const
{ return m_number_layers; }

inline bool
HitQuality::satisfies_inward_continuity (void) const
{ return m_continuity_inwards; }

inline bool
HitQuality::satisfies_outward_continuity (void) const
{ return m_continuity_outwards; }

#endif // IPATTRACK_HITQUALITY_H

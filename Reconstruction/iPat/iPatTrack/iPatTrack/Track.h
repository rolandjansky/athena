/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 base Track data class 
 ---------------------
 ***************************************************************************/

#ifndef IPATTRACK_TRACK_H
# define IPATTRACK_TRACK_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <list>
#include <utility>
#include <vector>
#include "iPatTrack/FitQuality.h"
#include "iPatTrack/HitList.h"
#include "iPatTrack/HitOnTrack.h"
#include "iPatTrack/HitQuality.h"
#include "iPatTrack/TrackStatus.h"
#include "iPatTrack/TruthAssociation.h"
#include "iPatTrackParameters/PerigeeParameters.h"
#include "iPatTrackParameters/ScattererParameters.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>

    typedef	std::vector<ScattererParameters*>		parameter_vector;
    typedef	parameter_vector::const_iterator       		parameter_const_iterator;
    typedef	parameter_vector::const_reverse_iterator       	parameter_reverse_iterator;
    typedef	parameter_vector::iterator       		parameter_iterator;

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace Trk
{
    class Track;
}

class Track
{

public:
   
    Track (void);                                                       // void constructor
    
    // note: in order to keep POOL happy without creating a mem leak
    //       the original parameter_vector gets deleted !!! 
    Track (TrackStatus,		
	   hit_list*,
	   parameter_vector*,
	   PerigeeParameters*,
	   FitQuality*,
	   HitQuality*);						// standard constructor
    Track (hit_list*,
	   parameter_vector*,
	   PerigeeParameters*,
	   FitQuality*,
	   HitQuality*);						// TrackEnvelope constructor
    Track (const Track&);						// copy constructor
    virtual ~Track (void);					       	// destructor

    // forbidden assignment operator (=> use copy constructor)
    
    // has methods verify data objects exist, since methods returning
    // a const reference to the data abort on non-existence

    // class (TrackEnvelope) contains the data modification methods needed during InDet PR.
    // This is now deprecated - new track modify methods will be provided as needed.
    // Some set methods are already provided - just for Moore

    // fit quality: has, get and set methods
    bool			hasFitQuality (void) const;
    const FitQuality&		fitQuality (void) const;
    void			fitQuality (FitQuality*);

    // hit quality: has and get methods
    bool			hasHitQuality (void) const;
    const HitQuality&		hitQuality (void) const;

    // perigee parameters: has, get and set methods
    bool			has_perigee_parameters (void) const;
    const PerigeeParameters&	perigee_parameters (void) const;
    void			perigee_parameters (PerigeeParameters*);

    // set method for scatterer parameters
    void			scattererParameters (parameter_vector*);

    // truth association: has, get and set methods 
    // (set necessary since association performed a posteriori)
    bool			hasTruthAssociation (void) const;
    const TruthAssociation&    	truthAssociation (void) const;
    void			truthAssociation (TruthAssociation*);

    // the following is required for Moore
    hit_list&			hits_for_edit (void);

    // general (const) hit/parameter access
    hit_const_iterator		hit_list_begin (void) const;
    hit_const_iterator		hit_list_end (void) const;
    hit_reverse_iterator	hit_list_rbegin (void) const;
    hit_reverse_iterator	hit_list_rend (void) const;
    parameter_const_iterator	parameters_begin (void) const;
    parameter_const_iterator	parameters_end (void) const;
    parameter_reverse_iterator	parameters_rbegin (void) const;

    // for debug
    void	        	print (void) const;
    void			print_hits (void) const;
    void	        	print_parameters (void) const;

    // get/set the enumerated TrackStatus
    TrackStatus			status (void) const;
    void			status (TrackStatus);

    // link to RTF track
    Trk::Track*         	trkTrack (void) const;
    void                	trkTrack (Trk::Track* track);
  
protected:
    // enumerated track status
    TrackStatus			m_status;
    
    // list of constituent hits :
    hit_list			m_hits;

    // track parameters :
    parameter_vector	       	m_parameter_vector;
    PerigeeParameters* 		m_perigeeParameters;
 
    // track quality :
    FitQuality*			m_fitQuality;
    HitQuality*			m_hitQuality;

    // Trk::Track conversion
    Trk::Track*			m_trkTrack;
    // truth association :
    TruthAssociation*		m_truthAssociation;
    
private:    
    // assignment operator: undefined, no semantics
    Track &operator= (const Track &);
};

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline bool
Track::hasFitQuality (void) const
{
    if (m_fitQuality != 0) return true;
    return false;
}

inline const FitQuality&
Track::fitQuality (void) const
{
    assert(m_fitQuality != 0);
    return *m_fitQuality;
}

inline void
Track::fitQuality (FitQuality* quality)
{
    delete m_fitQuality;
    m_fitQuality = quality;
}

inline bool
Track::hasHitQuality (void) const
{
    if (m_hitQuality != 0) return true;
    return false;
}

inline const HitQuality&
Track::hitQuality (void) const
{
    assert(m_hitQuality != 0);
    return *m_hitQuality;
}

inline bool
Track::has_perigee_parameters (void) const
{
    if (m_perigeeParameters != 0) return true;
    return false;
}

inline const PerigeeParameters&
Track::perigee_parameters (void) const
{
    assert(m_perigeeParameters != 0);
    return *m_perigeeParameters;
}

inline void
Track::perigee_parameters (PerigeeParameters* pointer) 
{
    delete m_perigeeParameters;
    m_perigeeParameters = pointer;
}

inline bool
Track::hasTruthAssociation (void) const
{
    if (m_truthAssociation != 0) return true;
    return false;
}

inline const TruthAssociation&
Track::truthAssociation (void) const
{
    assert(m_truthAssociation != 0);
    return *m_truthAssociation;
}

inline void
Track::truthAssociation (TruthAssociation* pointer) 
{
    delete m_truthAssociation;
    m_truthAssociation = pointer;
}

inline hit_list&
Track::hits_for_edit (void)
{ return m_hits; }

inline hit_const_iterator
Track::hit_list_begin (void) const
{ return m_hits.begin(); }

inline hit_const_iterator
Track::hit_list_end (void) const
{ return m_hits.end(); }

inline hit_reverse_iterator
Track::hit_list_rbegin (void) const
{ return m_hits.rbegin(); }

inline hit_reverse_iterator
Track::hit_list_rend (void) const
{ return m_hits.rend(); }

inline parameter_const_iterator
Track::parameters_begin (void) const
{ return m_parameter_vector.begin(); }

inline parameter_const_iterator
Track::parameters_end (void) const
{ return m_parameter_vector.end(); }

inline parameter_reverse_iterator
Track::parameters_rbegin (void) const
{ return m_parameter_vector.rbegin(); }

inline TrackStatus
Track::status (void) const
{ return m_status; }

inline void
Track::status (TrackStatus status)
{ m_status = status; }

#endif // IPATTRACK_TRACK_H

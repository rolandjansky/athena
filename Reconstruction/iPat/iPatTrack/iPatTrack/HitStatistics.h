/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 detailed statistics of hits associated to track
 (including comparison with truth - kine indices)
 -----------------------------------------------
 ***************************************************************************/

#ifndef IPATTRACK_HITSTATISTICS_H
# define IPATTRACK_HITSTATISTICS_H

//<<<<<< INCLUDES                                                       >>>>>>
//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class HitOnTrack;
class Track;

class HitStatistics
{
public:
    HitStatistics (void);
    HitStatistics (const Track &track);
    ~HitStatistics (void);
    
    int			number_b_layer_clusters (void) const;
    int			number_boundary (void) const;
    int			number_dead (void) const;
    int	        	number_drift_hits (void) const;
    int	        	number_drift_rejects (void) const;
    int	        	number_drift_time_hits (void) const;
    int	        	number_outer_pixel_clusters (void) const;
    int			number_pending (void) const;
    int	        	number_pixel_clusters (void) const;
    int	        	number_planar_clusters (void) const;
    int			number_planar_holes (void) const;
    int	        	number_shared (void) const;
    int	        	number_spoilt (void) const;
    int	        	number_strip_clusters (void) const;
    int	        	number_tr_hits (void) const;
    int	        	number_unique (void) const;
    int	      	  	number_wrong (void) const;
    int			pattern (void) const;
    void		print (void) const;
    int			shared_b_layer_clusters (void) const;
    int			shared_drift_hits (void) const;
    int	        	shared_outer_pixel_clusters (void) const;
    int	        	shared_pixel_clusters (void) const;
    int	        	shared_strip_clusters (void) const;
    int			spoilt_b_layer_clusters (void) const;
    int	        	spoilt_drift_hits (void) const;
    int	        	spoilt_outer_pixel_clusters (void) const;
    int	        	spoilt_pixel_clusters (void) const;
    int	        	spoilt_strip_clusters (void) const;
    int			unique_b_layer_clusters (void) const;
    int			unique_drift_hits (void) const;
    int	        	unique_outer_pixel_clusters (void) const;
    int	        	unique_pixel_clusters (void) const;
    int	        	unique_strip_clusters (void) const;
    int			wrong_b_layer_clusters (void) const;
    int	        	wrong_drift_hits (void) const;
    int	        	wrong_outer_pixel_clusters (void) const;
    int	        	wrong_pixel_clusters (void) const;
    int	        	wrong_strip_clusters (void) const;

private:
    unsigned int	bits (const char*, unsigned int, unsigned int, unsigned int) const;
    bool		in_b_layer (const HitOnTrack& hit) const;
    
    int			m_number_boundary;
    int			m_number_dead;
    int			m_number_drift_rejects;
    int			m_number_drift_wires;
    int			m_number_inactive;
    int			m_number_pending;
    int			m_number_planar_holes;
    int			m_number_tr_hits;
    int			m_pattern;
    int			m_shared_b_layer_clusters;
    int	        	m_shared_drift_hits;
    int	        	m_shared_pixel_clusters;
    int	        	m_shared_strip_clusters;
    int			m_spoilt_b_layer_clusters;	
    int	        	m_spoilt_drift_hits;
    int	        	m_spoilt_pixel_clusters;
    int	        	m_spoilt_strip_clusters;
    int			m_unique_b_layer_clusters;
    int	        	m_unique_drift_hits;
    int	        	m_unique_pixel_clusters;
    int	        	m_unique_strip_clusters;
    int			m_wrong_b_layer_clusters;
    int	        	m_wrong_drift_hits;
    int	        	m_wrong_pixel_clusters;
    int	        	m_wrong_strip_clusters;
};

//<<<<<< INLINE CLASS STRUCTURE INITIALIZATION                          >>>>>>

inline
HitStatistics::HitStatistics (void)
    :	m_number_boundary		(0),
	m_number_dead			(0),
	m_number_drift_rejects		(0),
	m_number_drift_wires		(0),
	m_number_inactive		(0),
	m_number_pending		(0),
	m_number_planar_holes		(0),
	m_number_tr_hits		(0),
	m_pattern			(0),
	m_shared_b_layer_clusters	(0),
	m_shared_drift_hits		(0),
	m_shared_pixel_clusters		(0),
	m_shared_strip_clusters		(0),
	m_spoilt_b_layer_clusters	(0),
	m_spoilt_drift_hits		(0),
	m_spoilt_pixel_clusters		(0),
	m_spoilt_strip_clusters		(0),
	m_unique_b_layer_clusters	(0),
	m_unique_drift_hits		(0),
	m_unique_pixel_clusters		(0),
	m_unique_strip_clusters		(0),
	m_wrong_b_layer_clusters	(0),
	m_wrong_drift_hits		(0),
	m_wrong_pixel_clusters		(0),
	m_wrong_strip_clusters		(0)
{
//     cout << " HitStatistics created " << endl;
}

inline
HitStatistics::~HitStatistics (void)
{
//     cout << " HitStatistics destroyed " << endl;
}

//<<<<<< INLINE PRIVATE MEMBER FUNCTIONS                                        >>>>>>

inline unsigned int
HitStatistics::bits (const char *p, unsigned int maxbit, unsigned int from, unsigned int n) const
//
//    Get `n' bits in character array `p' (maximum bit position `maxbit')
//    starting from position `from'.

//    Several things are assumed:
//    - bits are counted from left (regardless of machine byte or bit order)
//    - selected bits do not straddle byte boundaries
//    - if `from' does not exceed `maxbit', neither does `from'+`n'
//      (in other words, `maxbit' falls only at byte boundaries).
{
    return (from > maxbit ? 0 :
            (((unsigned int) *(p+from/CHAR_BIT) >> (CHAR_BIT-from%CHAR_BIT-n))
             & ~(~0u << n)));
}

//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                        >>>>>>

inline int
HitStatistics::number_b_layer_clusters (void) const
{  return m_unique_b_layer_clusters + m_spoilt_b_layer_clusters + m_wrong_b_layer_clusters;}

inline int
HitStatistics::number_boundary (void) const
{  return m_number_boundary;}

inline int
HitStatistics::number_dead (void) const
{  return m_number_dead;}

inline int
HitStatistics::number_drift_hits (void) const
{  return m_unique_drift_hits + m_spoilt_drift_hits + m_wrong_drift_hits;}

inline int
HitStatistics::number_drift_rejects (void) const
{  return m_number_drift_rejects;}

inline int
HitStatistics::number_drift_time_hits (void) const
{  return m_unique_drift_hits + m_spoilt_drift_hits + m_wrong_drift_hits - m_number_drift_wires;}

inline int
HitStatistics::number_outer_pixel_clusters (void) const
{
    return (m_unique_pixel_clusters + m_spoilt_pixel_clusters + m_wrong_pixel_clusters -
	    m_unique_b_layer_clusters - m_spoilt_b_layer_clusters - m_wrong_b_layer_clusters);
}

inline int
HitStatistics::number_pending (void) const
{  return m_number_pending;}

inline int
HitStatistics::number_pixel_clusters (void) const
{  return m_unique_pixel_clusters + m_spoilt_pixel_clusters + m_wrong_pixel_clusters;}

inline int
HitStatistics::number_planar_clusters (void) const
{
    return (m_unique_pixel_clusters + m_spoilt_pixel_clusters + m_wrong_pixel_clusters +
	    m_unique_strip_clusters + m_spoilt_strip_clusters + m_wrong_strip_clusters);
}

inline int
HitStatistics::number_planar_holes (void) const
{  return m_number_planar_holes;}

inline int
HitStatistics::number_shared (void) const
{  return m_shared_pixel_clusters + m_shared_strip_clusters + m_shared_drift_hits;}

inline int
HitStatistics::number_spoilt (void) const
{  return m_spoilt_pixel_clusters + m_spoilt_strip_clusters + m_spoilt_drift_hits;}

inline int
HitStatistics::number_strip_clusters (void) const
{  return m_unique_strip_clusters + m_spoilt_strip_clusters + m_wrong_strip_clusters;}

inline int
HitStatistics::number_tr_hits (void) const
{  return m_number_tr_hits;}

inline int
HitStatistics::number_unique (void) const
{  return m_unique_pixel_clusters + m_unique_strip_clusters + m_unique_drift_hits;}

inline int
HitStatistics::number_wrong (void) const
{  return m_wrong_pixel_clusters + m_wrong_strip_clusters + m_wrong_drift_hits;}

inline int    
HitStatistics::pattern (void) const
{  return m_pattern;}

inline int
HitStatistics::shared_b_layer_clusters (void) const
{  return m_shared_b_layer_clusters;}

inline int
HitStatistics::shared_drift_hits (void) const
{  return m_shared_drift_hits;}

inline int
HitStatistics::shared_outer_pixel_clusters (void) const
{  return m_shared_pixel_clusters - m_shared_b_layer_clusters;}

inline int
HitStatistics::shared_pixel_clusters (void) const
{  return m_shared_pixel_clusters;}

inline int
HitStatistics::shared_strip_clusters (void) const
{  return m_shared_strip_clusters;}

inline int
HitStatistics::spoilt_b_layer_clusters (void) const
{  return m_spoilt_b_layer_clusters;}

inline int
HitStatistics::spoilt_drift_hits (void) const
{  return m_spoilt_drift_hits;}

inline int
HitStatistics::spoilt_outer_pixel_clusters (void) const
{  return m_spoilt_pixel_clusters - m_spoilt_b_layer_clusters;}

inline int
HitStatistics::spoilt_pixel_clusters (void) const
{  return m_spoilt_pixel_clusters;}

inline int
HitStatistics::spoilt_strip_clusters (void) const
{  return m_spoilt_strip_clusters;}

inline int
HitStatistics::unique_b_layer_clusters (void) const
{  return m_unique_b_layer_clusters;}

inline int
HitStatistics::unique_drift_hits (void) const
{  return m_unique_drift_hits;}

inline int
HitStatistics::unique_outer_pixel_clusters (void) const
{  return m_unique_pixel_clusters - m_unique_b_layer_clusters;}

inline int
HitStatistics::unique_pixel_clusters (void) const
{  return m_unique_pixel_clusters;}

inline int
HitStatistics::unique_strip_clusters (void) const
{  return m_unique_strip_clusters;}

inline int
HitStatistics::wrong_b_layer_clusters (void) const
{  return m_wrong_b_layer_clusters;}

inline int
HitStatistics::wrong_drift_hits (void) const
{  return m_wrong_drift_hits;}

inline int
HitStatistics::wrong_outer_pixel_clusters (void) const
{  return m_wrong_pixel_clusters - m_wrong_b_layer_clusters;}
 
inline int
HitStatistics::wrong_pixel_clusters (void) const
{  return m_wrong_pixel_clusters;}

inline int
HitStatistics::wrong_strip_clusters (void) const
{  return m_wrong_strip_clusters;}
 
#endif // IPATTRACK_HITSTATISTICS_H

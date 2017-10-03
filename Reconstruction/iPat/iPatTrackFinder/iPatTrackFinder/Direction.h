/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 some simple geometrical properties for direction vectors
 represented using transverse direction cosines
 --------------------------------------------------------
 ***************************************************************************/

#ifndef IPATTRACKFINDER_DIRECTION_H
# define IPATTRACKFINDER_DIRECTION_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>


//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class Direction
{
public:
    // constructor (with member data)
    Direction	(double cos_phi, double sin_phi, double cot_theta);
    // constructor for vector from (0,0,0) to GlobalPosition
    Direction	(const Amg::Vector3D& point, double r);
    // constructor for vector joining 2 points
    Direction 	(const Amg::Vector3D& first, const Amg::Vector3D& second);

    // implicit copy constructor
    // implicit destructor
    // implicit assignment operator

    double	cos_phi (void) const;
    double	sin_phi (void) const;
    double	cot_theta (void) const;
    double	cos_sum_phi (const Direction& dir) const;
    double	sin_sum_phi (const Direction& dir) const;
    double	cos_diff_phi (const Direction& dir) const;
    double	sin_diff_phi (const Direction& dir) const;
    void        print (void) const;

private:
    double	m_cos_phi;
    double	m_sin_phi;
    double	m_cot_theta;
};

//<<<<<< INLINE CLASS STRUCTURE INITIALIZATION                          >>>>>>
 
inline
Direction::Direction (double cos_phi, double sin_phi, double cot_theta)
        : m_cos_phi (cos_phi),
          m_sin_phi (sin_phi),
          m_cot_theta (cot_theta)
{ }

inline
Direction::Direction (const Amg::Vector3D& point, double r)
{
    m_cos_phi		= point.x() / r;
    m_sin_phi		= point.y() / r;
    m_cot_theta		= point.z() / r;
}

inline
Direction::Direction (const Amg::Vector3D& first, const Amg::Vector3D& second)
{
    Amg::Vector3D	delta	= second - first;
    double		delta_r	= delta.perp();
    m_cos_phi			= delta.x() / delta_r;
    m_sin_phi			= delta.y() / delta_r;
    m_cot_theta			= delta.z() / delta_r;
}

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline double
Direction::cos_phi (void) const
{ return m_cos_phi; }

inline double
Direction::sin_phi (void) const
{ return m_sin_phi; }

inline double
Direction::cot_theta (void) const
{ return m_cot_theta; }

inline double
Direction::cos_sum_phi (const Direction& dir) const
{ return (m_cos_phi*dir.cos_phi() - m_sin_phi*dir.sin_phi()); }

inline double
Direction::sin_sum_phi (const Direction& dir) const
{ return (m_sin_phi*dir.cos_phi() + m_cos_phi*dir.sin_phi()); }

inline double
Direction::cos_diff_phi (const Direction& dir) const
{ return (m_cos_phi*dir.cos_phi() + m_sin_phi*dir.sin_phi()); }

inline double
Direction::sin_diff_phi (const Direction& dir) const
{ return (m_sin_phi*dir.cos_phi()) - m_cos_phi*dir.sin_phi(); }

#endif	// IPATTRACKFINDER_DIRECTION_H

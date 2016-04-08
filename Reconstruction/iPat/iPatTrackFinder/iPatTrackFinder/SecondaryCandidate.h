/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 SecondaryCandidate forms a track with approximate parameters taken 
 from 3 SpacePoints 
 ***************************************************************************/

#ifndef IPATTRACKFINDER_SECONDARYCANDIDATE_H
# define IPATTRACKFINDER_SECONDARYCANDIDATE_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/ToolHandle.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class IFinderConfiguration;
class PerigeeParameters;
class Point;

class SecondaryCandidate
{

public:
    // constructor
    SecondaryCandidate (const Amg::Vector3D&			perigeeVertex,
			ToolHandle<IFinderConfiguration>&	tolerances,
			std::vector<double>::const_iterator	zBegin,
			std::vector<double>::const_iterator	zEnd);
    
    // forbidden copy constructor
    // implicit destructor
    // forbidden assignment operator

    bool		addInnerPoint (const Point&);
    bool		addIntermediatePoint (const Point&);
    PerigeeParameters*	perigee (void);
    void	        print (void) const;
    void		setOuterPoint (const Point&);

private:
    const Amg::Vector3D&	       	m_perigeeVertex;
    ToolHandle<IFinderConfiguration>	m_tolerances;
    
    // SpacePoints and associated quantities :
    bool				m_inBarrel;
    Amg::Vector3D       		m_outerPosition;
    double				m_outerR;
    Amg::Vector3D			m_innerPosition;

    // parabolic representation :
    double				m_a1;
    double				m_a2;
    double				m_cosAxis;
    double				m_sinAxis;
    double				m_x;
    
    // perigee parameters :
    double				m_cosPhi;
    double				m_cotTheta;
    double				m_inversePt;
    double				m_sinPhi;
    double				m_transverseImpact;
    double				m_z;

    // primary vertices z-coordinates :
    std::vector<double>::const_iterator	m_zBegin;
    std::vector<double>::const_iterator	m_zEnd;
    
    // void constructor, copy, assignment: no semantics, no implementation
    SecondaryCandidate (void);
    SecondaryCandidate (const SecondaryCandidate&);
    SecondaryCandidate &operator= (const SecondaryCandidate&);
};

#endif	// IPATTRACKFINDER_SECONDARYCANDIDATE_H

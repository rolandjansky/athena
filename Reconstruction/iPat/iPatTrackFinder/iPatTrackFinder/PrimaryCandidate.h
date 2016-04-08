/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 PrimaryCandidate forms a track with approximate parameters formed from 
 2 SpacePoints with a vertex region
 ***************************************************************************/

#ifndef IPATTRACKFINDER_PRIMARYCANDIDATE_H
# define IPATTRACKFINDER_PRIMARYCANDIDATE_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/ToolHandle.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class IFinderConfiguration;
class PerigeeParameters;
class Point;

class PrimaryCandidate
{

public:
    // constructor
    PrimaryCandidate (ToolHandle<IFinderConfiguration>&	tolerances,
		      const Amg::Vector3D&		vertexRegionCentre);
    
    // forbidden copy constructor
    // implicit destructor
    // forbidden assignment operator

    bool		addInnerPixel (const Point&);
    bool		addInnerPixelAtNewVtx (const Point&,
					       std::vector<double>::const_iterator zBegin,
					       std::vector<double>::const_iterator zEnd);
    bool		isPixelCompatible (const Point&);
    bool		isPointCompatible (const Point&);
    PerigeeParameters*	perigee (void);
    void	        print (void) const;
    void		setOuterPoint (const Point&);

private:

    // SpacePoints and associated quantities :
    bool                        	m_inBarrel;
    Amg::Vector3D                	m_outerPosition;
    double                      	m_outerR;  
    double                      	m_outerPhi;
    double                      	m_outerZ;
    Amg::Vector3D                	m_innerPosition;
    double				m_innerR;
    double				m_innerPhi;
    double				m_innerZ;
    
    // cached tolerances :
    bool				m_cacheValid;
    double				m_etaTolerance;
    double				m_phiTolerance;
    ToolHandle<IFinderConfiguration>	m_tolerances;

    // ? representation :
    double				m_phi0;
    double				m_phiSlope;
    
    // perigee parameters :
    double				m_cosPhi;
    double				m_cotTheta;
    double				m_inversePt;
    double				m_sinPhi;
    double				m_transverseImpact;
    const Amg::Vector3D&		m_vertex;
    double				m_z;

    // void constructor, copy, assignment: no semantics, no implementation
    PrimaryCandidate (void);
    PrimaryCandidate (const PrimaryCandidate&);
    PrimaryCandidate &operator= (const PrimaryCandidate&);
};

#endif	// IPATTRACKFINDER_PRIMARYCANDIDATE_H

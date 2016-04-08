/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 describes a dead region inside a detector active boundary. This is used for
 the SCT wire bond region, and to describe significant groupings of dead
 read-out channels (strips or pixels).
 ***************************************************************************/

#ifndef IPATGEOMETRY_DEADREGION_H
# define IPATGEOMETRY_DEADREGION_H

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class DeadRegion
{

public:
    // constructor
    DeadRegion (double	xEtaMin,
		double	xEtaMax,
		double	xPhiMin,
		double	xPhiMax);

    // implicit copy constructor
    // implicit destructor
    // implicit assignment operator

    bool			inDeadRegion (double xEta,
					      double xEtaTolerance,
					      double xPhi,
					      double xPhiTolerance) const;
    
private:

    double		       	m_xEtaMax;
    double		       	m_xEtaMin;
    double		       	m_xPhiMax;
    double		       	m_xPhiMin;
};

//<<<<<< INLINE CLASS STRUCTURE INITIALIZATION                          >>>>>>

inline
DeadRegion::DeadRegion(double	xEtaMin,
		       double	xEtaMax,
		       double	xPhiMin,
		       double	xPhiMax)
    :	m_xEtaMax      	(xEtaMax),
	m_xEtaMin      	(xEtaMin),
	m_xPhiMax      	(xPhiMax),
	m_xPhiMin      	(xPhiMin)
{}

//<<<<<< INLINE PRIVATE MEMBER FUNCTIONS                                >>>>>>
//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline bool
DeadRegion::inDeadRegion(double xEta,
			 double xEtaTolerance,
			 double xPhi,
			 double xPhiTolerance) const
{
    if (xPhi + xPhiTolerance	< m_xPhiMin
	|| xPhi - xPhiTolerance	> m_xPhiMax
	|| xEta + xEtaTolerance	< m_xEtaMin
	|| xEta - xEtaTolerance	> m_xEtaMax)
    {
	return false;
    }
    else
    {
	return true;
    }
}

#endif // IPATGEOMETRY_DEADREGION_H


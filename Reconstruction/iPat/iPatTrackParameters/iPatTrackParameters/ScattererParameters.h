/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 ScattererParameters are the value of the TrackParameters at a given 
 scattering plane, along with the local scattering angles and errors
 -------------------------------------------------------------------
 ***************************************************************************/

#ifndef IPATTRACKPARAMETERS_SCATTERER_PARAMETERS_H
# define IPATTRACKPARAMETERS_SCATTERER_PARAMETERS_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <iPatTrackParameters/TrackParameters.h>

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class ScattererParameters:public TrackParameters
{
public:
    ScattererParameters (void);
    ScattererParameters (double	x,
			 double	y,
			 double	z,
			 double	cosPhi,
			 double	sinPhi,
			 double	cotTheta,
			 double	qOverPt,
			 double	deltaCotTheta,
			 double deltaPhi,
			 double sigmaCotTheta,
			 double sigmaPhi);
    ScattererParameters (const ScattererParameters&);	// copy constructor

    // implicit destructor
    // forbidden assignment operator

    double			deltaCotTheta (void) const;
    void			deltaCotTheta (double value);
    double			deltaPhi (void) const;
    void			deltaPhi (double value);
    double			sigmaCotTheta (void) const;
    void			sigmaCotTheta (double value);
    double			sigmaPhi (void) const;
    void			sigmaPhi (double value);
    void			fillScatterer (double	x,
					       double	y,
					       double	z,
					       double	cosPhi,
					       double	sinPhi,
					       double	cotTheta,
					       double	qOverPt,
					       double	deltaCotTheta,
					       double	deltaPhi,
					       double	sigmaCotTheta,
					       double	sigmaPhi);
    void			fillScatterer (const ScattererParameters&);
    void			print (void) const;
    
private:
    double			m_deltaCotTheta;
    double			m_deltaPhi;
    double			m_sigmaCotTheta;
    double			m_sigmaPhi;

    // assignment operator: undefined, no semantics
    ScattererParameters &operator= (const ScattererParameters &);
};

//<<<<<< INLINE CLASS STRUCTURE INITIALIZATION                          >>>>>>

inline
ScattererParameters::ScattererParameters (void)
    :	TrackParameters	(),
	m_deltaCotTheta	(0.),
	m_deltaPhi	(0.),
	m_sigmaCotTheta	(0.),
	m_sigmaPhi	(0.)
{}

inline
ScattererParameters::ScattererParameters (double	x,
					  double	y,
					  double	z,
					  double	cosPhi,
					  double	sinPhi,
					  double	cotTheta,
					  double	qOverPt,
					  double	deltaCotTheta,
					  double	deltaPhi,
					  double	sigmaCotTheta,
					  double	sigmaPhi)
    :	TrackParameters	(Amg::Vector3D(x,y,z),
			 cosPhi,
			 sinPhi,
			 cotTheta,
			 qOverPt),
        m_deltaCotTheta	(deltaCotTheta),
        m_deltaPhi	(deltaPhi),
        m_sigmaCotTheta	(sigmaCotTheta),
        m_sigmaPhi	(sigmaPhi)
{}

inline
ScattererParameters::ScattererParameters (const ScattererParameters& scatter)
    :	TrackParameters	(scatter.m_position,
			 scatter.m_cosPhi,
			 scatter.m_sinPhi,
			 scatter.m_cotTheta,
			 scatter.m_qOverPt),
        m_deltaCotTheta (scatter.m_deltaCotTheta),
        m_deltaPhi	(scatter.m_deltaPhi),
        m_sigmaCotTheta	(scatter.m_sigmaCotTheta),
        m_sigmaPhi	(scatter.m_sigmaPhi)
{}

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline double
ScattererParameters::deltaCotTheta (void) const
{ return m_deltaCotTheta; }

inline void
ScattererParameters::deltaCotTheta (double value)
{ m_deltaCotTheta = value; } 

inline double
ScattererParameters::deltaPhi (void) const
{ return m_deltaPhi; }

inline void
ScattererParameters::deltaPhi (double value)
{ m_deltaPhi = value; } 

inline double
ScattererParameters::sigmaCotTheta (void) const
{ return m_sigmaCotTheta; }

inline void
ScattererParameters::sigmaCotTheta (double value)
{ m_sigmaCotTheta = value; } 

inline double
ScattererParameters::sigmaPhi (void) const
{ return m_sigmaPhi; }

inline void
ScattererParameters::sigmaPhi (double value)
{ m_sigmaPhi = value; } 

inline void
ScattererParameters::fillScatterer(double	x,
				   double	y,
				   double	z,
				   double	cosPhi,
				   double	sinPhi,
				   double	cotTheta,
				   double	qOverPt,
				   double	deltaCotTheta,
				   double	deltaPhi,
				   double	sigmaCotTheta,
				   double	sigmaPhi)
{
    m_position		= Amg::Vector3D(x,y,z);
    m_cosPhi 		= cosPhi;
    m_sinPhi 		= sinPhi;
    m_cotTheta	 	= cotTheta;
    m_qOverPt 		= qOverPt;
    m_deltaCotTheta	= deltaCotTheta;
    m_deltaPhi		= deltaPhi;
    m_sigmaCotTheta	= sigmaCotTheta;
    m_sigmaPhi		= sigmaPhi;
}

inline void
ScattererParameters::fillScatterer(const ScattererParameters& scatter)
{
    m_position		= scatter.position();
    m_cosPhi 		= scatter.cosPhi();
    m_sinPhi 		= scatter.sinPhi();
    m_cotTheta	 	= scatter.cotTheta();
    m_qOverPt 		= scatter.qOverPt();
    m_deltaCotTheta	= scatter.deltaCotTheta();
    m_deltaPhi		= scatter.deltaPhi();
    m_sigmaCotTheta	= scatter.sigmaCotTheta();
    m_sigmaPhi		= scatter.sigmaPhi();
}

#endif // IPATTRACKPARAMETERS_SCATTERER_PARAMETERS_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 track fit quality (chi_squared, probability etc)
 ------------------------------------------------
 ***************************************************************************/

#ifndef IPATTRACK_FITQUALITY_H
# define IPATTRACK_FITQUALITY_H

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class FitQuality
{

public:
    FitQuality (void);
    FitQuality (double, int, double, double); 
    virtual ~FitQuality (void);
    
    // Implicit copy constructor
    // Implicit assignment operator

    double		chi_squared(void) const;
    int			degrees_of_freedom(void) const;
    void		fill(double	chi_squared,
			     int     	degrees_of_freedom,
			     int 	fit_code,
			     double 	fit_probability,
			     double 	worst_hit_chi_squared);
    void		fill(const FitQuality&);
    int			fit_code(void) const;
    void		fit_code(int code);	// set method
    double		fit_probability(void) const;
    double		worst_hit_chi_squared(void) const;
    
private:
    double	m_chi_squared;
    int		m_degrees_of_freedom;
    int		m_fit_code;
    double	m_fit_probability;
    double	m_worst_hit_chi_squared;
    
};

//<<<<<< INLINE CLASS STRUCTURE INITIALIZATION                          >>>>>>

inline
FitQuality::FitQuality(void)
    :	m_chi_squared			(0.),
	m_degrees_of_freedom		(0),
	m_fit_code			(2),
	m_fit_probability		(-1.0),
	m_worst_hit_chi_squared		(0.)
{}
 
inline
FitQuality::FitQuality(double 	chi_squared,
		       int     	degrees_of_freedom,
		       double 	fit_probability,
		       double 	worst_hit_chi_squared)
    :	m_chi_squared			(chi_squared),
	m_degrees_of_freedom		(degrees_of_freedom),
	m_fit_code			(0),
	m_fit_probability		(fit_probability),
	m_worst_hit_chi_squared		(worst_hit_chi_squared)
{}
 
inline
FitQuality::~FitQuality(void)
{}
 
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline double
FitQuality::chi_squared(void) const
{ return m_chi_squared; }

inline int
FitQuality::degrees_of_freedom(void) const
{ return m_degrees_of_freedom; }

inline void
FitQuality::fill(double	chi_squared,
		 int   	degrees_of_freedom,
		 int 	fit_code,
		 double	fit_probability,
		 double	worst_hit_chi_squared)
{
    m_chi_squared		= chi_squared;
    m_degrees_of_freedom	= degrees_of_freedom;
    m_fit_code			= fit_code;
    m_fit_probability	 	= fit_probability;
    m_worst_hit_chi_squared    	= worst_hit_chi_squared;
}

inline void
FitQuality::fill(const FitQuality& p)
{
    m_chi_squared		= p.m_chi_squared;
    m_degrees_of_freedom	= p.m_degrees_of_freedom;
    m_fit_code			= p.m_fit_code;
    m_fit_probability	 	= p.m_fit_probability;
    m_worst_hit_chi_squared    	= p.m_worst_hit_chi_squared;
}

inline int
FitQuality::fit_code(void) const
{ return m_fit_code; }

inline void
FitQuality::fit_code(int code)
{
    m_fit_code = code;
    if (m_fit_code > 0) m_fit_probability = -1.;  // old code uses prob < 0 to signal bad quality
}

inline double
FitQuality::fit_probability(void) const
{ return m_fit_probability; }

inline double
FitQuality::worst_hit_chi_squared(void) const
{ return m_worst_hit_chi_squared; }

#endif // IPATTRACK_FITQUALITY_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 quality variables produced during fit procedure
 ***************************************************************************/

#ifndef TRKIPATFITTERUTILS_FITPROCEDUREQUALITY_H
# define TRKIPATFITTERUTILS_FITPROCEDUREQUALITY_H

//<<<<<< INCLUDES                                                       >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class MsgStream;

namespace Trk
{

class FitProcedureQuality
{
public:
    FitProcedureQuality (int	fitCode,
			 int	numberDoF);
	
    FitProcedureQuality (double	chiSq,
			 double	chiSqWorst,
			 double	fitProbability,
			 int	fitCode,
			 int	iterations,
			 int	numberAlignments,
			 int	numberDoF,
			 int	numberScatterers,
			 int	worstMeasurement);
    
    ~FitProcedureQuality (void);

    // implicit copy constructor
    // implicit assignment operator

    double		chiSq (void) const;
    double		chiSqWorst (void) const;
    double		fitProbability (void) const;
    int			fitCode (void) const;
    int			iterations (void) const;
    int			numberDoF (void) const;
    void		print (MsgStream& log) const;
    int			worstMeasurement (void) const;
  
private:
    double		m_chiSq;
    double		m_chiSqWorst;
    double		m_fitProbability;
    int			m_fitCode;
    int			m_iterations;
    int			m_numberAlignments;
    int			m_numberDoF;
    int			m_numberScatterers;
    int			m_worstMeasurement;
};

//<<<<<< INLINE CLASS STRUCTURE INITIALIZATION                          >>>>>>

inline
FitProcedureQuality::FitProcedureQuality (int	fitCode,
					  int	numberDoF)
    :	m_chiSq			(0.),
	m_chiSqWorst		(0.),
	m_fitProbability	(0.),
	m_fitCode		(fitCode),
	m_iterations		(0),
	m_numberAlignments	(0),
	m_numberDoF		(numberDoF),
	m_numberScatterers	(0),
	m_worstMeasurement	(0)
{}

inline
FitProcedureQuality::FitProcedureQuality (double	chiSq,
					  double	chiSqWorst,
					  double	fitProbability,
					  int		fitCode,
					  int		iterations,
					  int		numberAlignments,
					  int		numberDoF,
					  int		numberScatterers,
					  int		worstMeasurement)
    :	m_chiSq			(chiSq),
	m_chiSqWorst		(chiSqWorst),
	m_fitProbability	(fitProbability),
	m_fitCode		(fitCode),
	m_iterations		(iterations),
	m_numberAlignments	(numberAlignments),
	m_numberDoF		(numberDoF),
	m_numberScatterers	(numberScatterers),
	m_worstMeasurement	(worstMeasurement)
{}

inline
FitProcedureQuality::~FitProcedureQuality ()
{}

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE PRIVATE MEMBER FUNCTIONS                                >>>>>>

inline double
FitProcedureQuality::chiSq (void) const
{ return m_chiSq; }

inline double
FitProcedureQuality::chiSqWorst (void) const
{ return m_chiSqWorst; }

inline double
FitProcedureQuality::fitProbability (void) const
{ return m_fitProbability; }

inline int
FitProcedureQuality::fitCode (void) const
{ return m_fitCode; }

inline int
FitProcedureQuality::iterations (void) const
{ return m_iterations; }

inline int
FitProcedureQuality::numberDoF (void) const
{ return m_numberDoF; }

inline int
FitProcedureQuality::worstMeasurement (void) const
{ return m_worstMeasurement; }

}	// end of namespace
  
#endif // TRKIPATFITTERUTILS_FITPROCEDUREQUALITY_H

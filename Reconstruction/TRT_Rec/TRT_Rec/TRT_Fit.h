/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Header:
//  Author: Jim Loken
//


#ifndef TRTREC_TRTFIT_H
#define TRTREC_TRTFIT_H

class TRT_Fit
{
public:
    TRT_Fit();
    ~TRT_Fit();

    double	aFit (void) const;
    double	bFit (void) const;
    void	init (void);
    double	rPhi (double r) const;
    void	solve (void);
    void	sum (double r, double deltaPhi, double weight);

private:
    double	m_aFit;
    double	m_bFit;
    double	m_c1;
    double	m_c2;
    double	m_c3;
    double	m_c4;
    double	m_c5;
};

inline double
TRT_Fit::aFit (void) const
{ return m_aFit; }

inline double
TRT_Fit::bFit (void) const
{ return m_bFit; }

inline double
TRT_Fit::rPhi (double r) const
{ return r*(m_aFit + r*m_bFit); }

#endif



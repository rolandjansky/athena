/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PanTauEvent/KinematicFitResult.h"
#include "PanTauEvent/CorrectedEflowMomentum.h"

PanTau::KinematicFitResult::KinematicFitResult()
    :
    m_chi2(-1),
    m_ndof(-1)
{
}

PanTau::KinematicFitResult::KinematicFitResult(
	double chi2,
	int ndof
        ) :
    m_chi2(chi2),
    m_ndof(ndof)
{
}

PanTau::KinematicFitResult::KinematicFitResult(
    const PanTau::KinematicFitResult& rhs):
    m_chi2(rhs.m_chi2),
    m_ndof(rhs.m_ndof)
{
}

PanTau::KinematicFitResult::~KinematicFitResult()
{
}

PanTau::KinematicFitResult& PanTau::KinematicFitResult::operator=(const PanTau::KinematicFitResult& rhs)
{
    if ( this != &rhs ) {
        m_chi2 = rhs.m_chi2;
	m_ndof = rhs.m_ndof;
    }
    return *this;
}

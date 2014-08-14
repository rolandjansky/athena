/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Contains straw data in search road used by TRT histogram and fitting algs
***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <iostream>
#include "GaudiKernel/SystemOfUnits.h"
#include "TRT_Rec/TRT_RoadData.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

// drift_time constructor
TRT_RoadData::TRT_RoadData (bool			barrel,
			    double			driftDistance,
			    int				driftSign,	
			    const Identifier&		id,
			    double			projectionFactor,
			    double			rPhiRoad,
			    double			rhoRoad,
			    const Trk::PrepRawData*	rio,
			    const Amg::Vector3D&	strawPosition,
			    StrawStatus			strawStatus,
			    bool			trThreshold)
    :   m_barrel        	(barrel),
	m_driftDistance 	(driftDistance),
	m_id            	(id),
	m_projectionFactor	(projectionFactor),
	m_rPhiRoad		(rPhiRoad),
	m_rhoRoad       	(rhoRoad),
	m_rio			(rio),
	m_strawPosition 	(strawPosition),
	m_strawStatus   	(strawStatus),
	m_trThreshold   	(trThreshold)
{
    // fold in drift sign, correct rPhi for drift distance and cache phiRoad
    if (driftSign < 0) m_driftDistance = -m_driftDistance;
    m_rPhiRoad	+= m_driftDistance/m_projectionFactor;
    m_phiRoad   =  m_rPhiRoad/m_rhoRoad;
    
    // weighting for internal chi2 takes into account:
    //   - background peaking at small drift times
    //   - fit performed on hist bin before +/- drift sign fully resolved
    if (fabs(m_driftDistance) > 1.0*Gaudi::Units::mm)
    {
	m_driftWeight = 3.;
    }
    else if (fabs(m_driftDistance) > 0.5*Gaudi::Units::mm)
    {
        m_driftWeight = 2.;
    }
    else
    {
	m_driftWeight = 1.;
    }
}

// straw_hit constructor
TRT_RoadData::TRT_RoadData (bool			barrel,
			    const Identifier&		id,
			    double			projectionFactor,
			    double			rPhiRoad,
			    double			rhoRoad,
			    const Trk::PrepRawData*	rio,
			    const Amg::Vector3D&	strawPosition,
			    bool			trThreshold)
    :   m_barrel		(barrel),
	m_driftDistance		(0.),
	m_driftWeight		(0.),
	m_id			(id),
	m_projectionFactor	(projectionFactor),
	m_rPhiRoad		(rPhiRoad),
	m_rhoRoad		(rhoRoad),
	m_rio			(rio),
	m_strawPosition		(strawPosition),
	m_strawStatus		(pendingStraw),
	m_trThreshold		(trThreshold)
{
    // cache phiRoad
    m_phiRoad   = m_rPhiRoad/m_rhoRoad;
}




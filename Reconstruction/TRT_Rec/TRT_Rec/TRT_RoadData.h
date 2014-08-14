/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Contains straw data in search road used by TRT histogram and fitting algs
***************************************************************************/

#ifndef TRTREC_TRTROADDATA_H
#define TRTREC_TRTROADDATA_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/Identifier.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace Trk
{
    class PrepRawData;
}

class TRT_RoadData
{

public:

enum StrawStatus
{
    rejectedDoublet,
    pendingStraw,
    pendingWire,
    pendingDrift,
    notConfirmed,
    confirmedStraw,
    confirmedWire,
    confirmedDrift
};
    
    // drift-hit constructor
    TRT_RoadData (bool				barrel,
		  double			driftDistance,
		  int				driftSign,	
		  const Identifier&		id,
		  double			projectionFactor,
		  double			rPhiRoad,
		  double			rhoRoad,
		  const Trk::PrepRawData*	rio,
		  const Amg::Vector3D&		strawPosition,
		  StrawStatus			strawStatus,
		  bool				trThreshold);
    
    // straw-hit constructor
    TRT_RoadData (bool				barrel,
		  const Identifier&		id,
		  double			projectionFactor,
		  double			rPhiRoad,
		  double			rhoRoad,
		  const Trk::PrepRawData*	rio,
		  const Amg::Vector3D&		strawPosition,
		  bool				trThreshold);

    // destructor
    ~TRT_RoadData (void);

    double			driftDistance (void) const;
    double			driftWeight (void) const;
    const Identifier&		identify (void) const;
    bool			inBarrel (void) const;
    double			phiRoad (void) const;
    double			projectionFactor (void) const;
    double			rPhiRoad (void) const;
    double			rhoRoad (void) const;
    const Trk::PrepRawData*	rio (void) const;
    const Amg::Vector3D&	strawPosition (void) const;
    StrawStatus			strawStatus (void) const;
    void			strawStatus (StrawStatus status);
    bool			trThreshold (void) const;

private:
    bool			m_barrel;
    double			m_driftDistance;
    double			m_driftWeight;
    Identifier			m_id;
    double			m_phiRoad;
    double			m_projectionFactor;
    double			m_rPhiRoad;
    double			m_rhoRoad;
    const Trk::PrepRawData*	m_rio;
    Amg::Vector3D		m_strawPosition;
    StrawStatus			m_strawStatus;
    bool			m_trThreshold;

    // void and copy constructors, assignment: no semantics, no implementation
    TRT_RoadData (void);
    TRT_RoadData (const TRT_RoadData&);
    TRT_RoadData& operator= (const TRT_RoadData&);
};

//<<<<<< INLINE CLASS STRUCTURE INITIALIZATION                          >>>>>>
   
inline
TRT_RoadData::~TRT_RoadData(void)
{}

//<<<<<< INLINE PRIVATE MEMBER FUNCTIONS                                >>>>>>
//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline double
TRT_RoadData::driftDistance(void) const
{ return m_driftDistance; }

inline double
TRT_RoadData::driftWeight(void) const
{ return m_driftWeight; }

inline const Identifier&
TRT_RoadData::identify(void) const 
{ return m_id; }

inline bool
TRT_RoadData::inBarrel(void) const
{ return m_barrel; }

inline double
TRT_RoadData::phiRoad(void) const
{ return m_phiRoad; }

inline double
TRT_RoadData::projectionFactor(void) const
{ return m_projectionFactor; }

inline double
TRT_RoadData::rPhiRoad(void) const
{ return m_rPhiRoad; }

inline double
TRT_RoadData::rhoRoad(void) const 
{ return m_rhoRoad; }

inline const Trk::PrepRawData*
TRT_RoadData::rio(void) const
{ return m_rio; }

inline const Amg::Vector3D&
TRT_RoadData::strawPosition(void) const
{ return m_strawPosition; }

inline TRT_RoadData::StrawStatus
TRT_RoadData::strawStatus(void) const
{ return m_strawStatus; }

inline void
TRT_RoadData::strawStatus(StrawStatus status)
{ m_strawStatus = status; }

inline bool
TRT_RoadData::trThreshold(void) const  
{ return m_trThreshold; }
    
#endif










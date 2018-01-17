/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FinderConfiguration.h, (c) ATLAS iPatRec
//
// centralised definition of all cuts and tolerances applied during
// iPatRec track finding.
//
// FIXME: to be extended to generically describe layout model and
// combinatorial search strategy. Maybe needs callback
//
///////////////////////////////////////////////////////////////////

#ifndef IPATTRACKFINDER_FINDERTOLERANCES_H
# define IPATTRACKFINDER_FINDERTOLERANCES_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "iPatInterfaces/IFinderConfiguration.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class IIncidentSvc;
namespace InDetDD	{ class SiDetectorManager; }
namespace MagField	{ class IMagFieldSvc; }

class FinderTolerances: public AthAlgTool,
			virtual public IFinderConfiguration, IIncidentListener
{

public:
    FinderTolerances	(const std::string& type,
			 const std::string& name,
			 const IInterface* parent);
    ~FinderTolerances	(void);

    StatusCode		initialize();
    StatusCode		finalize();
    
    /** handle for incident service */
    void		handle(const Incident& inc) ;

    /** method registered for the callback chain */
    // StatusCode		magneticFieldInitialize(IOVSVC_CALLBACK_ARGS);

    // clients: CombinationMaker, PrimaryCandidate, SecondaryCandidate
    double		halfField (void) const;
    // clients: CombinationMaker, PointManager
    double		maxCurvature (void) const;
    double		maxCurvature (double minPt) const;
    // clients: SecondaryCandidate
    double		maxDeltaRZ (void) const;
    // clients: PrimaryCandidate
    double		maxPrimaryDeltaEtaPix (void) const;
    double		maxPrimaryDeltaEtaSct (void) const;
    double		maxPrimaryDeltaEtaVtx (void) const;
    double		maxPrimaryDeltaPhiPix (void) const;
    double		maxPrimaryDeltaPhiSct (void) const;
    double		maxPrimaryDeltaPhiVtx (void) const;
    // clients: PrimaryCandidate, SecondaryCandidate
    double		maxPhiSlope (void) const;
    // clients: CombinationMaker
    double		maxPrimaryImpact (void) const;
    // clients: CombinationMaker, SecondaryCandidate
    double		maxSecondaryImpact (void) const;
    // clients: PrimaryCandidate, SecondaryCandidate, TrackFinder
    double		maxVertexZ (void) const;
    double		minVertexZ (void) const;
    // clients: CombinationMaker, TrackFinder, TrackManager
    double		minPt (void) const;
    void		minPt (double value);

    int		        printLevel (void) const;
    void		setProperties (double			maxSecondaryImpact,
				       double			minPt,
				       int			printLevel,
				       const VertexRegion&	vertexRegion);
    void		vertexZLimits (void);
    void		vertexZLimits (double z);
    void		vertexZLimits (double zMax, double zMin);

private:

    // services and managers:
    ServiceHandle<IIncidentSvc>	       	m_incidentSvc;   //!< IncidentSvc to catch begin of event
    ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;
    const InDetDD::SiDetectorManager*	m_manager;
    
    double				m_halfField;
    double				m_maxPhiSlope;
    double				m_maxDeltaRZ;
    double				m_maxPrimaryDeltaEtaPix;
    double				m_maxPrimaryDeltaEtaSct;
    double				m_maxPrimaryDeltaEtaVtx;
    double				m_maxPrimaryDeltaPhiPix;
    double				m_maxPrimaryDeltaPhiSct;
    double				m_maxPrimaryDeltaPhiVtx;
    double				m_maxPrimaryImpact;
    double				m_maxSecondaryImpact;
    double				m_maxVertexRegionZ;
    double				m_maxVertexZ;
    double				m_minPt;
    double				m_minVertexRegionZ;
    double				m_minVertexZ;
    std::string                 	m_pixelName;
    int					m_printLevel;
    double				m_vertexZWidth;

};

//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline double
FinderTolerances::halfField (void) const
{ return m_halfField; }

inline double
FinderTolerances::maxCurvature (void) const
{ return std::abs(m_halfField/m_minPt); }

inline double
FinderTolerances::maxCurvature (double minPt) const
{ return std::abs(m_halfField/minPt); }

inline double
FinderTolerances::maxDeltaRZ (void) const
{ return m_maxDeltaRZ; }

inline double
FinderTolerances::maxPhiSlope (void) const
{ return m_maxPhiSlope; }

inline double
FinderTolerances::maxPrimaryDeltaEtaPix (void) const
{ return m_maxPrimaryDeltaEtaPix; }

inline double
FinderTolerances::maxPrimaryDeltaEtaSct (void) const
{ return m_maxPrimaryDeltaEtaSct; }

inline double
FinderTolerances::maxPrimaryDeltaEtaVtx (void) const
{ return m_maxPrimaryDeltaEtaVtx; }

inline double
FinderTolerances::maxPrimaryDeltaPhiPix (void) const
{ return m_maxPrimaryDeltaPhiPix; }

inline double
FinderTolerances::maxPrimaryDeltaPhiSct (void) const
{ return m_maxPrimaryDeltaPhiSct; }

inline double
FinderTolerances::maxPrimaryDeltaPhiVtx (void) const
{ return m_maxPrimaryDeltaPhiVtx; }

inline double
FinderTolerances::maxPrimaryImpact (void) const
{ return m_maxPrimaryImpact; }

inline double
FinderTolerances::maxSecondaryImpact (void) const
{ return m_maxSecondaryImpact; }

inline double
FinderTolerances::maxVertexZ (void) const
{ return m_maxVertexZ; }

inline double
FinderTolerances::minVertexZ (void) const
{ return m_minVertexZ; }
  
inline double
FinderTolerances::minPt (void) const
{ return m_minPt; }

inline int
FinderTolerances::printLevel (void) const
{ return m_printLevel; }

inline void
FinderTolerances::vertexZLimits (void)
{
    m_maxVertexZ	= m_maxVertexRegionZ;
    m_minVertexZ	= m_minVertexRegionZ;
}

inline void
FinderTolerances::vertexZLimits (double z)
{
    m_maxVertexZ	= z + m_vertexZWidth;
    m_minVertexZ	= z - m_vertexZWidth;
}

#endif	// IPATTRACKFINDER_FINDERTOLERANCES_H

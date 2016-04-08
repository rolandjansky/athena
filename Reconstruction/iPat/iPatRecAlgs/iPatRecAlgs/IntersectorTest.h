/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IntersectorTest Algorithm
 
 ATLAS Collaboration
 ***************************************************************************/

#ifndef IPATRECALGS_INTERSECTORTEST_H
# define IPATRECALGS_INTERSECTORTEST_H

//<<<<<< INCLUDES >>>>>>

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "iPatRecEvent/iPatTrackContainer.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class IiPatFitter;
namespace Trk
{
    class IIntersector;
    class Surface;
    class TrackSurfaceIntersection;
}

class IntersectorTest:public AthAlgorithm
{
   
public:

    IntersectorTest(const std::string& name, ISvcLocator* pSvcLocator);
    ~IntersectorTest(void);

    StatusCode	initialize(void);    
    StatusCode	execute(void);
    StatusCode	finalize(void);  
    
private:
    void					compareWithPrecise(
	const Trk::TrackSurfaceIntersection*	intersection,
	double					qOverP);

    // helpers, managers, tools
    ToolHandle<Trk::IIntersector>		m_intersector;
    ToolHandle<Trk::IIntersector>		m_preciseIntersector;
    ToolHandle<IiPatFitter>			m_trackFitter;

    // cylinder and precision limits
    double					m_cotThetaMax;
    double					m_etaScale;
    bool					m_fixedPt;
    double					m_maxOffset;
    double					m_maxRotation;
    double					m_zLimit;
    
    // surfaces
    const Trk::Surface*                 	m_backwardsSurface;
    const Trk::Surface*                 	m_barrelSurface;
    const Trk::Surface*                 	m_forwardsSurface;
    const Trk::Surface*                 	m_perigeeSurface;

    // configuration
    double					m_momentum;	// momentum for test
    std::string					m_region;	// indet,solenoid,calo,muon
    std::string					m_tracksName;

    // data access
    const iPatTrackContainer*		      	m_tracks;
    
    // counters
    int						m_failCount;
    int						m_offsetCount;
    int						m_rotationCount;
    int					       	m_trackCount;
};

#endif // IPATRECALGS_INTERSECTORTEST_H





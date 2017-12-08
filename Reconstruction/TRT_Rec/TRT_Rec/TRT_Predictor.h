/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 AlgTool creating TRT_Predictions (along a Track prolongation into the TRT)
 ***************************************************************************/

#ifndef TRTREC_TRTPREDICTOR_H
# define TRTREC_TRTPREDICTOR_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "iPatInterfaces/ITRT_Predictor.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class IdentifierHash;
class PerigeeParameters;
class TRT_Prediction;
class TrackParameters;
namespace InDetDD
{
    class TRT_DetectorManager;
}
namespace Trk
{
    class CylinderSurface;
    class DiscSurface;
    class IIntersector;
    class TrackSurfaceIntersection;
}

class TRT_Predictor: public AthAlgTool,
                     virtual public ITRT_Predictor
{

public:
    TRT_Predictor			(const std::string&	type, 
					 const std::string&	name,
					 const IInterface*	parent);
    ~TRT_Predictor 			(void); 	// destructor
    
    StatusCode				initialize(void);
    StatusCode				finalize(void);

    double				expectedStraws(void) const;
    int					intersectedLayers(void) const;
    const std::vector<TRT_Prediction*>&	makePredictions(const PerigeeParameters&	perigee, 
							const TrackParameters&		parameters);
private:
    void				crudePredictions(void);
    void				finePredictions(void);
    void				setRegion(const TrackParameters& parameters);

    // helpers, managers, tools
    // SurfaceIntersector*				m_intersector;
    ServiceHandle<IRegSelSvc>			m_regionSelector;
    ToolHandle<Trk::IIntersector>		m_rungeKuttaIntersector;
    const InDetDD::TRT_DetectorManager*		m_manager;

    // configuration and derived surfaces
    double					m_barrelAcceptance;
    double					m_cotThetaCorner;
    double					m_electronRoadFactor;
    double					m_etaRoadSafetyFactor;
    double					m_extrapolationTolerance;
    double					m_layerTolerance;
    std::string					m_managerName;
    Trk::CylinderSurface*			m_midBarrelSurface;
    Trk::DiscSurface*				m_outerBackwardsSurface;
    Trk::DiscSurface*				m_outerForwardsSurface;
    double					m_phiSectors;
    double					m_rMeanBarrel;
    double					m_strawWidth;
    double					m_zEndcapMax;
    
    // state
    double					m_cosPhiStart;
    double					m_deltaCurv;
    double					m_deltaPhi;
    double					m_expectedStraws;
    std::vector<IdentifierHash>*		m_hashIds;
    int						m_intersectedLayers;
    const Trk::TrackSurfaceIntersection*	m_intersection;
    double					m_phiHalfWidth;
    double					m_phiWidth;
    std::vector<TRT_Prediction*>		m_predictions;
    double					m_qOverP;
    double					m_rWidth;
    double					m_signDeltaPhi;
    double					m_sinPhiStart;
    double					m_strawHalfWidth;
    double					m_zWidth;
};   

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline double
TRT_Predictor::expectedStraws (void) const
{ return m_expectedStraws; }

inline int
TRT_Predictor::intersectedLayers (void) const
{ return m_intersectedLayers; }

#endif // TRTREC_TRTPREDICTOR_H

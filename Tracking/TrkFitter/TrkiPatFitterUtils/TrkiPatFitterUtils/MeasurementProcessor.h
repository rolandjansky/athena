/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 process measurements i.e. extrapolate to surface,
                           compute derivatives and residuals
 ***************************************************************************/

#ifndef TRKIPATFITTERUTILS_MEASUREMENTPROCESSOR_H
# define TRKIPATFITTERUTILS_MEASUREMENTPROCESSOR_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <list>
#include <vector>
#include "GaudiKernel/ToolHandle.h"
#include "TrkiPatFitterUtils/ExtrapolationType.h" 
#include "TrkGeometry/MagneticFieldProperties.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace Trk
{
    class FitMeasurement;
    class FitParameters;
    class IIntersector;
    class IPropagator;
    class TrackSurfaceIntersection;

class MeasurementProcessor
{
public:
    MeasurementProcessor (bool					asymmetricCaloEnergy,
			  ToolHandle<IIntersector>&		intersector,
			  std::list<FitMeasurement*>&		measurements,
			  FitParameters*			parameters,
			  ToolHandle<IIntersector>&    		rungeKuttaIntersector,
			  const ToolHandle<IPropagator>&    	stepPropagator,
			  int 					useStepPropagator);    
    ~MeasurementProcessor (void);

    // implicit copy constructor
    // implicit assignment operator

    // compute derivatives and residuals
    bool	calculateDerivatives(void);
    bool	calculateFittedTrajectory(int iteration);
    void	calculateResiduals(void);
    void	fieldIntegralUncertainty(MsgStream& log, Amg::MatrixX& covariance);
    void	propagationDerivatives(void);
    
private:
    void		clusterDerivatives(int derivativeFlag, const FitMeasurement& measurement);
    void		driftDerivatives(int derivativeFlag, const FitMeasurement& measurement);
    bool		extrapolateToMeasurements(ExtrapolationType type);

    std::vector<FitMeasurement*>	m_alignments;
    bool				m_asymmetricCaloEnergy;
    FitMeasurement*			m_caloEnergyMeasurement;
    double				m_cosPhi0;
    double				m_cosTheta0;
    double				m_delta[ExtrapolationTypes];
    double				m_derivQOverP0;
    double				m_derivQOverP1;
    double				m_energyResidual;
    int					m_firstScatteringParameter;
    //bool				m_havePhiPseudo;
    const TrackSurfaceIntersection*	m_intersectStartingValue;
    const ToolHandle<IIntersector>&	m_intersector;
    double				m_largeDeltaD0;
    double				m_largeDeltaPhi0;
    std::list<FitMeasurement*>&		m_measurements;
    //double				m_minDistanceForAngle;
    bool				m_numericDerivatives;
    FitParameters*			m_parameters;
    bool				m_phiInstability;
    double				m_qOverP[ExtrapolationTypes];
    double				m_qOverPbeforeCalo;
    double				m_qOverPafterCalo;
    const ToolHandle<IIntersector>&	m_rungeKuttaIntersector;
    const ToolHandle< IPropagator >&    m_stepPropagator;
    int                                 m_useStepPropagator;
    std::vector<FitMeasurement*>	m_scatterers;
    double				m_sinPhi0;
    double				m_sinTheta0;
    //double				m_toroidTurn;
    const TrackSurfaceIntersection*	m_vertexIntersect;
    double				m_x0;
    double				m_y0;
    double				m_z0;
    //bool				m_zInstability;
    Trk::MagneticFieldProperties        m_stepField; 
};

}	// end of namespace

#endif // TRKIPATFITTERUTILS_MEASUREMENTPROCESSOR_H

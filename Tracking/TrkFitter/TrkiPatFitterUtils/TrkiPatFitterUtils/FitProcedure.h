/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// class FitProcedure
//  Basic iterative least square fit procedure producing a fitted Track
//
//  execute and constructTrack methods allow use from standard ITrackFitter
//  and iPatRec specific Algtools.
//  This class is kept transient rather than a tool as there are many state variables.
//
//  for clarity (i.e. to avoid an overly long class) the inner loop over
//  measurements is performed by MeasurementProcessor
//
//  (c) ATLAS tracking software
//////////////////////////////////////////////////////////////////////////////

#ifndef TRKIPATFITTERUTILS_FITPROCEDURE_H
# define TRKIPATFITTERUTILS_FITPROCEDURE_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <list>
#include "DataModel/DataVector.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class MsgStream;

namespace Trk
{
    class FitMatrices;
    class FitMeasurement;
    class FitParameters;
    class FitProcedureQuality;
    class FitQuality;
    class IIntersector;
    class IPropagator;
    class Track;
    class TrackInfo;
    class TrackStateOnSurface;
    class Volume;

class FitProcedure
{
public:
    FitProcedure (bool					constrainedAlignmentEffects,
		  bool					extendedDebug,
		  bool					eigenMatrixTreatment,
		  bool					lineFit,
		  ToolHandle<IIntersector>&		rungeKuttaIntersector,
		  ToolHandle<IIntersector>&		solenoidalIntersector,
		  ToolHandle<IIntersector>&		straightLineIntersector,
		  const ToolHandle<IPropagator>&	stepPropagator, 
		  const Volume*				indetVolume	= 0,
		  int					maxIterations	= 25,
		  int					useStepPropagator = 0);
    
    ~FitProcedure (void);	// destructor
    // forbidden copy constructor
    // forbidden assignment operator

    // retrieve result
    Track*	constructTrack (const std::vector<FitMeasurement*>&		measurements,
				const FitParameters&				parameters,
				const TrackInfo&				trackInfo,
				const DataVector<const TrackStateOnSurface>*	leadingTSOS = 0);
    
    // perform fit procedure
    const FitProcedureQuality&	execute (bool					asymmetricCaloEnergy,
					 MsgStream&				log,
					 std::vector<FitMeasurement*>&		measurements,
					 FitParameters*&			parameters,
					 const FitQuality*			perigeeQuality = 0,
					 bool					for_iPatTrack = false);

    // for IGlobalTrackFit interface
    Amg::MatrixX*		fullCovariance () const;
    
    // set minimum number of iterations to perform (IGlobalTrackFit friends)
    void			setMinIterations (int minIter);
    
private:
    // copy, assignment: no semantics, no implementation
    FitProcedure (const FitProcedure&);
    FitProcedure &operator= (const FitProcedure&);
    
    void				calculateChiSq(std::vector<FitMeasurement*>&	measurements);
    ToolHandle<IIntersector>&		chooseIntersector(std::vector<FitMeasurement*>&	measurements,
							  const FitParameters&	parameters) const;
    void				reportQuality(const std::vector<FitMeasurement*>& measurements,
						      const FitParameters&	parameters) const;

    double				m_chRatio1;
    double				m_chRatio2;
    double				m_chiSq;
    double				m_chiSqCut;
    double				m_chiSqMin;
    double				m_chiSqOld;
    double				m_chiSqWorst;
    bool				m_convergence;
    bool				m_cutStep;
    int					m_cutTaken;
    bool				m_debug;
    double				m_driftSum;
    double				m_driftSumLast;
    bool				m_extendedDebug;
    double				m_extremeOneOverP;
    FitMatrices*			m_fitMatrices;
    double				m_fitProbability;
    FitProcedureQuality*		m_fitQuality;
    const Trk::Volume*			m_indetVolume;
    int					m_iteration;
    double				m_largeRadius;
    bool				m_lineFit;
    MsgStream*				m_log;
    int 				m_maxIter;
    int					m_minIter;
    double				m_minPt;
    int					m_nCuts;
    bool				m_nearConvergence;
    int 				m_numberDoF;
    int 				m_numberParameters;
    ToolHandle<IIntersector>&		m_rungeKuttaIntersector;
    double				m_scatteringConstant;
    double				m_scatteringLogCoeff;
    ToolHandle<IIntersector>&		m_solenoidalIntersector;
    ToolHandle<IIntersector>&		m_straightLineIntersector;
    const ToolHandle<IPropagator>&	m_stepPropagator;
    int                                 m_useStepPropagator;
    bool				m_verbose;
    int					m_worstMeasurement;
};

}	// end of namespace

#endif // TRKIPATFITTERUTILS_FITPROCEDURE_H

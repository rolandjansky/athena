/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/***************************************************************************
 for any measurement type (cluster, drift circle or material)
   stores the quantities needed during track fitting
   i.e. position, surface, weights, intersection, derivatives, residual etc
 ***************************************************************************/

#ifndef TRKIPATFITTERUTILS_FITMEASUREMENT_H
# define TRKIPATFITTERUTILS_FITMEASUREMENT_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <vector>
#include "EventPrimitives/EventPrimitives.h"
// #include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkiPatFitterUtils/ExtrapolationType.h"
#include "TrkiPatFitterUtils/MeasurementType.h"

#include <memory>

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class HitOnTrack;
class MsgStream;

namespace Trk
{
    class AlignmentEffectsOnTrack;
    class MaterialEffectsBase;
    class MeasurementBase;
    class Surface;
    class TrackStateOnSurface;
    class TrackSurfaceIntersection;

class FitMeasurement
{
public:
    FitMeasurement (int		       			hitIndex,
		    HitOnTrack*	       			hitOnTrack,
		    const MeasurementBase*		measurementBase); // MeasurementBase

    FitMeasurement (const MaterialEffectsBase*		materialEffects,
		    double				particleMass,
		    const Amg::Vector3D&		position,
		    double				qOverP = 0.,
		    bool				calo = false);	// MaterialEffectsBase

    FitMeasurement (double				radiationThickness,
		    double				deltaE,
		    double				particleMass,
		    const Amg::Vector3D&		position,
		    const Amg::Vector3D&		direction,
		    double				qOverP,
		    const Surface*			surface = 0);	// for merged material effects
    
    FitMeasurement (const AlignmentEffectsOnTrack*	alignmentEffects,
		    const Amg::Vector3D&		direction,
		    const Amg::Vector3D&		position);	// (mis-)alignment effects

    FitMeasurement (const TrackSurfaceIntersection&	intersection,
		    double				shift);		// aggregationBreak placeholder

    FitMeasurement (const TrackStateOnSurface&		TSOS);		// other TSOS
    
    FitMeasurement (int		       			hitIndex,
		    HitOnTrack*	       			hitOnTrack,
		    const Amg::Vector3D&      		position,
		    double				sigma,
		    double				sigma2,
		    double				sinStereo,
		    int		       			status,
		    const Surface*	    		surface,
		    MeasurementType			type);		// stripCluster

    FitMeasurement (int	       				hitIndex,
		    HitOnTrack*    			hitOnTrack,
		    const Amg::Vector3D&      		position,
		    double				sigma,
		    double				signedDriftDistance,
		    double				sinStereo,
		    const Surface*			surface);      	// driftCircle

    FitMeasurement (const TrackParameters&		perigee);	// perigee

    FitMeasurement (double				d0,
		    const Amg::Vector3D&      		position,
		    double				sigma);      	// transverseVertex
    
    ~FitMeasurement (void);	       					// destructor
    // forbidden copy constructor
    // forbidden assignment operator

    bool				afterCalo (void) const;
    double				alignmentAngle (void) const;
    void				alignmentAngle (double value);
    const AlignmentEffectsOnTrack*	alignmentEffects (void) const;
    double				alignmentOffset (void) const;
    void				alignmentOffset (double value);
    unsigned	       			alignmentParameter (void) const;
    void		       		alignmentParameter (unsigned value);
    unsigned	       			alignmentParameter2 (void) const;
    void		       		alignmentParameter2 (unsigned value);
    double				derivative (int param) const;
    void				derivative (int param, double value) const;
    void				derivative (double* pointer);
    double				derivative2 (int param) const;
    void				derivative2 (int param, double value) const;
    void				derivative2 (double* pointer);
    int 				derivativeRow (void) const;
    void				derivativeRow (int row);
    double				d0 (void) const;
    double				energyLoss (void) const;
    double				energyLossSigma (void) const;
    unsigned	       			firstParameter (void) const;
    void		       		firstParameter (unsigned value);
    void				flipDriftDirection (void);
    int					hitIndex (void) const;
    HitOnTrack*				hitOnTrack (void) const;
    bool				hasIntersection (ExtrapolationType type) const;
    const TrackSurfaceIntersection&	intersection (ExtrapolationType type) const;
    void				intersection (ExtrapolationType type,
						      const TrackSurfaceIntersection* value);
                                                      //std::unique_ptr<TrackSurfaceIntersection>);
    bool				isAlignment (void) const;
    bool				isCluster (void) const;
    bool				isDrift (void) const;
    bool				isEnergyDeposit (void) const;
    bool				isFlipped (void) const;
    bool				isMaterialDelimiter (void) const;
    bool				isOutlier (void) const;
    bool				isPassive (void) const;
    bool				isPerigee (void) const;
    bool				isPositionMeasurement (void) const;
    bool				isPseudo (void) const;
    bool				isScatterer (void) const;
    bool				isTrapezoidCluster (void) const;
    bool				isVertex (void) const;
    bool				is2Dimensional (void) const;

    // number of fit parameters influencing (upstream to) this measurement
    unsigned	       			lastParameter (void) const;
    void		       		lastParameter (unsigned value, bool afterCalo = false);
    const MaterialEffectsBase*		materialEffects (void) const;
    const MeasurementBase*		measurementBase (void) const;
    double				minEnergyDeposit (void) const;
    const Amg::Vector3D&     		minimizationDirection (void) const;
    void				minimizationDirection (Amg::Vector3D* value);
    const Amg::Vector3D&     		normal (void) const;
    int		       			numberDoF (void) const;
    void		       		numberDoF (int value);
    bool				numericalDerivative (void) const;
    const Amg::VectorX*       		perigee (void) const;
    const Amg::MatrixX*			perigeeWeight (void) const;
    const Amg::Vector3D&      		position (void) const;
    void				print (MsgStream& log) const;
    void				printHeading (MsgStream& log) const;
    double				qOverP (void) const;
    void				qOverP (double value);
    double				radiationThickness (void) const;
    double				residual (void) const;
    void				residual (double value);
    void				residual (std::vector<double>::iterator pointer);
    double				residual2 (void) const;
    void				residual2 (double value);
    double				scattererPhi (void) const;
    void				scattererPhi (double value);
    double				scattererTheta (void) const;
    void				scattererTheta (double value);
    void				scatteringAngle (double angle,
							 double totalRadiationThickness);
    const Amg::Vector3D&     		sensorDirection (void) const;

    // for upper hemisphere cosmics
    void				setEnergyGain (void);
    
    void				setMaterialEffectsOwner (void);
    void				setNumericalDerivative (void);
    void				setOutlier (void);

    // to handle calo energy deposit with asymmetric errors
    void				setSigma (void);
    void				setSigmaMinus (void);
    void				setSigmaPlus (void);
    void				setSigmaSymmetric (void);
    
    double				sigma (void) const;
    double				sigma2 (void) const;
    double				signedDriftDistance (void) const;
    int		       			status (void) const;
    const Surface*    			surface (void) const;
    MeasurementType			type (void) const;
    void				unsetOutlier (void);
    double				weight (void) const;
    double				weight2 (void) const;

private:
    // copy, assignment: no semantics, no implementation
    FitMeasurement (const FitMeasurement&);
    FitMeasurement &operator= (const FitMeasurement&);
    
    bool				m_afterCalo;
    const AlignmentEffectsOnTrack*	m_alignmentEffects;
    unsigned				m_alignmentParameter;
    unsigned				m_alignmentParameter2;
    double				m_betaSquared;
    double*				m_derivative;
    double*				m_derivative2;
    int 				m_derivativeRow;
    double				m_d0;
    double				m_energyLoss;
    unsigned				m_firstParameter;
    bool				m_flippedDriftDistance;
    int					m_hitIndex;
    HitOnTrack*				m_hitOnTrack;
    std::array<std::unique_ptr<const TrackSurfaceIntersection>,ExtrapolationTypes >  m_intersection;
    unsigned				m_lastParameter;
    const MaterialEffectsBase*		m_materialEffects;
    bool				m_materialEffectsOwner;
    const MeasurementBase*		m_measurementBase;
    double				m_minEnergyDeposit;
    Amg::Vector3D*			m_minimizationDirection;
    Amg::Vector3D*			m_normal;
    int					m_numberDoF;
    bool				m_numericalDerivative;
    bool				m_outlier;
    double				m_particleMassSquared;
    const Amg::VectorX*			m_perigee;
    const Amg::MatrixX*			m_perigeeWeight;
    Amg::Vector3D			m_position;
    double				m_qOverP;
    double				m_radiationThickness;
    std::vector<double>::iterator      	m_residual;
    double				m_scaleFactor;
    double				m_scatterPhi;
    double				m_scatterTheta;
    double				m_scatteringAngle;
    double				m_scatteringAngleOffSet;
    double				m_secondResidual;
    Amg::Vector3D*			m_sensorDirection;
    double				m_sigma;
    double				m_sigmaMinus;
    double				m_sigmaPlus;
    double				m_signedDriftDistance;
    int					m_status;
    const Surface*			m_surface;
    MeasurementType			m_type;
    double				m_weight;
    double				m_weight2;
};

//<<<<<< INLINE PUBLIC MEMBER FUNCTIONS                                 >>>>>>

inline bool
FitMeasurement::afterCalo (void) const
{ return m_afterCalo; }

inline double
FitMeasurement::alignmentAngle (void) const
{ return m_scatterPhi; }

inline void
FitMeasurement::alignmentAngle (double value)
{ m_scatterPhi = value; }

inline const AlignmentEffectsOnTrack*
FitMeasurement::alignmentEffects (void) const
{ return m_alignmentEffects; }

inline double
FitMeasurement::alignmentOffset (void) const
{ return m_scatterTheta; }

inline void
FitMeasurement::alignmentOffset (double value)
{ m_scatterTheta = value; }

inline unsigned
FitMeasurement::alignmentParameter (void) const
{ return m_alignmentParameter; }

inline void
FitMeasurement::alignmentParameter (unsigned value)
{ m_alignmentParameter = value; }

inline unsigned
FitMeasurement::alignmentParameter2 (void) const
{ return m_alignmentParameter2; }

inline void
FitMeasurement::alignmentParameter2 (unsigned value)
{ m_alignmentParameter2 = value; }

inline double
FitMeasurement::derivative (int param) const
{ return *(m_derivative+param); }

inline void
FitMeasurement::derivative (int param, double value) const
{ *(m_derivative+param) = value; }

inline void
FitMeasurement::derivative (double* pointer)
{ m_derivative = pointer; }

inline double
FitMeasurement::derivative2 (int param) const
{ return *(m_derivative2+param); }

inline void
FitMeasurement::derivative2 (int param, double value) const
{ *(m_derivative2+param) = value; }

inline void
FitMeasurement::derivative2 (double* pointer)
{ m_derivative2 = pointer; }

inline int
FitMeasurement::derivativeRow (void) const
{ return m_derivativeRow; }

inline void
FitMeasurement::derivativeRow (int row)
{ m_derivativeRow = row; }

inline double
FitMeasurement::d0 (void) const
{ return m_d0; }
 
inline double
FitMeasurement::energyLoss (void) const
{ return m_energyLoss; }
  
inline double
FitMeasurement::energyLossSigma (void) const
{ return m_sigma; }

inline unsigned
FitMeasurement::firstParameter (void) const
{ return m_firstParameter; }

inline void
FitMeasurement::firstParameter (unsigned value)
{ m_firstParameter	= value; }

inline void
FitMeasurement::flipDriftDirection (void)
{
    if (m_type == driftCircle)
    {
	m_flippedDriftDistance	= true;
	m_signedDriftDistance	= -m_signedDriftDistance;
    }
}

inline int
FitMeasurement::hitIndex (void) const
{ return m_hitIndex; }

inline HitOnTrack*
FitMeasurement::hitOnTrack (void) const
{ return m_hitOnTrack; }

inline bool
FitMeasurement::hasIntersection (ExtrapolationType type) const
{ return (bool)m_intersection[type]; }

inline const TrackSurfaceIntersection&
FitMeasurement::intersection (ExtrapolationType type) const
{ return *m_intersection[type]; }

inline bool
FitMeasurement::isAlignment (void) const
{ return (m_type == alignment); }

inline bool
FitMeasurement::isCluster (void) const
{ return (m_type == pixelCluster || m_type == stripCluster || m_type == trapezoidCluster); }

inline bool
FitMeasurement::isDrift (void) const
{ return (m_type == driftCircle); }

inline bool
FitMeasurement::isEnergyDeposit (void) const
{ return (m_type == energyDeposit); }

inline bool
FitMeasurement::isFlipped (void) const
{ return (m_type == driftCircle && m_flippedDriftDistance); }

inline bool
FitMeasurement::isMaterialDelimiter (void) const
{ return (m_type == materialDelimiter); }

inline bool
FitMeasurement::isOutlier (void) const
{ return m_outlier; }

inline bool
FitMeasurement::isPassive (void) const
{ return (m_type > bremPoint); }

inline bool
FitMeasurement::isPerigee (void) const
{ return (m_type == perigeeParameters); }

inline bool
FitMeasurement::isPositionMeasurement (void) const
{ return (m_type < barrelScatterer); }

inline bool
FitMeasurement::isPseudo (void) const
{ return (m_type == pseudoMeasurement); }

inline bool
FitMeasurement::isScatterer (void) const
{ return (m_type == barrelScatterer || m_type == endcapScatterer 
          || m_type == calorimeterScatterer
	  || m_type == barrelInert || m_type == endcapInert); }

inline bool
FitMeasurement::isTrapezoidCluster (void) const
{ return (m_type == trapezoidCluster); }
  
inline bool
FitMeasurement::isVertex (void) const
{ return (m_type == vertex || m_type == transverseVertex); }

inline bool
FitMeasurement::is2Dimensional (void) const
{ return (m_numberDoF == 2); }

inline unsigned
FitMeasurement::lastParameter (void) const
{ return m_lastParameter; }

inline void
FitMeasurement::lastParameter (unsigned value, bool afterCalo)
{
    m_afterCalo		= afterCalo;
    m_lastParameter	= value;
}

inline const MaterialEffectsBase*
FitMeasurement::materialEffects (void) const
{ return m_materialEffects; }

inline const MeasurementBase*
FitMeasurement::measurementBase (void) const
{ return m_measurementBase; }

inline double
FitMeasurement::minEnergyDeposit (void) const
{ return m_minEnergyDeposit; }

inline const Amg::Vector3D&
FitMeasurement::minimizationDirection (void) const
{ return *m_minimizationDirection; }

inline void
FitMeasurement::minimizationDirection (Amg::Vector3D* value)
{
    delete m_minimizationDirection;
    m_minimizationDirection = value;
}
 
inline const Amg::Vector3D&
FitMeasurement::normal (void) const
{ return *m_normal; }

inline int
FitMeasurement::numberDoF (void) const
{
    if (m_outlier) return 0;
    return m_numberDoF;
}

inline void
FitMeasurement::numberDoF (int value)
{ m_numberDoF = value; }

inline bool
FitMeasurement::numericalDerivative (void) const
{ return m_numericalDerivative; }

inline const Amg::VectorX*
FitMeasurement::perigee (void) const
{ return m_perigee; }

inline const Amg::MatrixX*
FitMeasurement::perigeeWeight (void) const
{ return m_perigeeWeight; }

inline const Amg::Vector3D&
FitMeasurement::position (void) const
{ return m_position; }

inline double
FitMeasurement::qOverP (void) const
{ return m_qOverP; }

inline double
FitMeasurement::radiationThickness (void) const
{ return m_radiationThickness; }

inline double
FitMeasurement::residual (void) const
{ return *m_residual; }

inline void
FitMeasurement::residual (double value)
{ *m_residual = value; }

inline void
FitMeasurement::residual (std::vector<double>::iterator pointer)
{ m_residual = pointer; }

inline double
FitMeasurement::residual2 (void) const
{ return *(m_residual+1); }

inline void
FitMeasurement::residual2 (double value)
{ *(m_residual+1) = value; }

inline double
FitMeasurement::scattererPhi (void) const
{ return m_scatterPhi; }

inline void
FitMeasurement::scattererPhi (double value)
{ m_scatterPhi = value; }

inline double
FitMeasurement::scattererTheta (void) const
{ return m_scatterTheta; }

inline void
FitMeasurement::scattererTheta (double value)
{ m_scatterPhi = value; }

inline const Amg::Vector3D&
FitMeasurement::sensorDirection (void) const
{ return *m_sensorDirection; }

inline void
FitMeasurement::setEnergyGain (void)
{ m_energyLoss = -fabs(m_energyLoss); }

inline void
FitMeasurement::setMaterialEffectsOwner (void)
{ m_materialEffectsOwner = true; }
     
inline void
FitMeasurement::setNumericalDerivative (void)
{ m_numericalDerivative = true; }

inline void
FitMeasurement::setOutlier (void)
{
    m_numberDoF	= 0;
    m_outlier	= true;
    m_weight	= 1.0;
    m_weight2	= 1.0;
}

inline void
FitMeasurement::setSigma (void)
{ m_weight = 1./m_sigma; }

inline void
FitMeasurement::setSigmaMinus (void)
{ m_weight = 1./m_sigmaMinus; }

inline void
FitMeasurement::setSigmaPlus (void)
{ m_weight = 1./m_sigmaPlus; }

inline double
FitMeasurement::sigma (void) const
{
    if (! m_weight) return 0;
    return 1./m_weight;
}

inline double
FitMeasurement::sigma2 (void) const
{
    if (! m_weight2) return 0;
    return 1./m_weight2;
}

inline double
FitMeasurement::signedDriftDistance (void) const
{ return m_signedDriftDistance; }

inline int
FitMeasurement::status (void) const 
{ return m_status; }

inline const Surface*
FitMeasurement::surface (void) const
{ return m_surface; }

inline MeasurementType
FitMeasurement::type (void) const 
{ return m_type; }

inline void
FitMeasurement::unsetOutlier (void)
{ m_outlier = false; }

inline double
FitMeasurement::weight (void) const
{ return m_weight; }

inline double
FitMeasurement::weight2 (void) const
{ return m_weight2; }


}	// end of namespace

#endif	// TRKIPATFITTERUTILS_FITMEASUREMENT_H

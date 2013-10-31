/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// DistributedKalmanFilter.h
//   Header file for class DistributedKalmanFilter
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dmitry Emeliyanov, RAL
// D.Emeliyanov@rl.ac.uk
///////////////////////////////////////////////////////////////////
#ifndef __TRK_DISTRIBUTED_KALMAN_FILTER__
#define __TRK_DISTRIBUTED_KALMAN_FILTER__

#include "TrkFitterUtils/FitterTypes.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
//#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "MagFieldInterfaces/IMagFieldSvc.h" 

#include <vector>

class IAlgTool;
class AtlasDetectorID;

namespace MagField {	
  class IMagFieldSvc;
}

namespace Trk {
	
    class Track;                   //!> ATLAS standard track class
    class TrkBaseNode;             //!> Base class for track filtering nodes
    class TrkTrackState;           //!> Simple track state: 5 parameters + covariance
    class TrkPlanarSurface;        //!> Simple class for planar surface description
    class TrackStateOnSurface;
    class IMagneticFieldTool;
    class IRIO_OnTrackCreator;     //!> Interface for creation of ROT
    class IExtrapolator;           //!> Extrapolation Tool
    class MeasuredPerigee;


    class DistributedKalmanFilter : virtual public ITrackFitter, public AthAlgTool { 
    public:
	// standard AlgTool methods
	DistributedKalmanFilter(const std::string&,const std::string&,const IInterface*);
	virtual ~DistributedKalmanFilter();
		
	// standard Athena methods
	StatusCode initialize();
	StatusCode finalize();

	// Filter settings:
	//			RunOutlierRemoval - use logic to remove bad hits
	//			ParticleHypothesis   - allow for multiple scattering and energy loss?

	virtual Track* fit(const Track&,
			   const RunOutlierRemoval  runOutlier=false,
			   const ParticleHypothesis matEffects=Trk::nonInteracting) const;

	// refit a track adding a PrepRawDataSet -- use base class method.
	virtual Track* fit(const Track&,
			   const PrepRawDataSet&,
			   const RunOutlierRemoval,
			   const ParticleHypothesis) const {return NULL;}
	
	// fit a set of PrepRawData objects
	virtual Track* fit(const PrepRawDataSet&,
			   const TrackParameters&   estimatedParametersNearOrigine,
			   const RunOutlierRemoval  runOutlier=false,
			   const ParticleHypothesis matEffects=Trk::nonInteracting) const;
	
	// fit a set of RIO_OnTrack objects
	virtual Track* fit(const RIO_OnTrackSet&,
			   const TrackParameters&   estimatedParametersNearOrigine,
			   const RunOutlierRemoval  runOutlier=false,
			   const ParticleHypothesis matEffects=Trk::nonInteracting) const;
        
        virtual Track* fit(const MeasurementSet&,
                           const TrackParameters& estim,
                           const RunOutlierRemoval runo=false, 
                           const ParticleHypothesis 
			   mat=Trk::nonInteracting) const;

	virtual Track* fit(const Track&,
			   const MeasurementSet&,
			   const RunOutlierRemoval runOutlier=false,
			   const ParticleHypothesis matEffects=Trk::nonInteracting) const;

	virtual Track* fit(const Track&,
			   const Track&,
			   const RunOutlierRemoval,
			   const ParticleHypothesis) const {return NULL;}
                        
private:
	///////////////////////////////////////////////////////////////////
	// Private functions:
	///////////////////////////////////////////////////////////////////

	Trk::TrkTrackState* m_extrapolate(TrkTrackState*, 
				      TrkPlanarSurface*,
				      TrkPlanarSurface*) const;
	bool m_runForwardKalmanFilter(TrkTrackState*) const;
	void m_runSmoother() const;
	int  m_findOutliers(double) const;
	void m_calculateLRsolution() const;
	TrackStateOnSurface* m_createTrackStateOnSurface(TrkBaseNode*) const;
	void m_deleteNodes() const;
	void m_deleteSurfaces() const;
	void m_deleteTrackStates() const;
	void m_report();
	void m_report(char fileName[]);
	void m_getMagneticField(double[3],double*) const;
	void m_matrixInversion5x5(double a[5][5]) const;
	Perigee* m_createMeasuredPerigee(TrkTrackState*) const;

	void m_numericalJacobian(TrkTrackState*, TrkPlanarSurface*,TrkPlanarSurface*,
				 double A[5][5]) const;
	double m_integrate(double Rk[5], 
			   TrkPlanarSurface* pSB,
			   TrkPlanarSurface* pSE, double* Rf) const;

	///////////////////////////////////////////////////////////////////
	// Private data:
	///////////////////////////////////////////////////////////////////

	//    RunOutlierRemoval             m_option_runOutlier;
	//    ParticleHypothesis               m_option_matEffects;

	ToolHandle<IRIO_OnTrackCreator>          m_ROTcreator;
	ToolHandle<IExtrapolator>                m_extrapolator;
	const AtlasDetectorID*      m_idHelper;

	ServiceHandle<MagField::IMagFieldSvc> m_MagFieldSvc;  
	std::vector<TrkBaseNode*>* m_pvpNodes;
	std::vector<TrkPlanarSurface*>* m_pvpSurfaces;
	std::vector<TrkTrackState*>* m_pvpTrackStates;

	// ME temporary fix
	std::vector<double> m_option_sortingRefPoint;

    };	

} // end of namespace

#endif

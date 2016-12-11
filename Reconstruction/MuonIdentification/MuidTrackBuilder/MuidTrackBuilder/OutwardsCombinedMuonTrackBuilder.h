/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// OutwardsCombinedMuonTrackBuilder
//  AlgTool gathering  material effects along a combined muon track, in 
//  particular the TSOS'es representing the calorimeter energy deposit and
//  Coulomb scattering.
//  The resulting track is fitted at the IP using the ITrackFitter interface.
// 
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDTRACKBUILDER_OUTWARDSCOMBINEDMUONTRACKBUILDER_H
#define MUIDTRACKBUILDER_OUTWARDSCOMBINEDMUONTRACKBUILDER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace Muon
{
    class IMuonTrackCleaner;
    class IMuonHoleRecoveryTool;
    class IMuonErrorOptimisationTool;
}
namespace Trk
{
    class ITrackSummaryTool;
    class RecVertex;
    class Surface;
    class TrackStateOnSurface;
    class PseudoMeasurementOnTrack;
    class TrackingVolume;
    class Volume;
    class VertexOnTrack; 
}
namespace Rec
{

    class OutwardsCombinedMuonTrackBuilder: public AthAlgTool,
					    virtual public ICombinedMuonTrackBuilder
    {

    public:
	OutwardsCombinedMuonTrackBuilder (const std::string& type, 
					  const std::string& name,
					  const IInterface* parent);
	~OutwardsCombinedMuonTrackBuilder (void); // destructor
  
	StatusCode		initialize();
	StatusCode		finalize();

	/**ICombinedMuonTrackBuilder interface: build and fit combined ID/Calo/MS track */
	Trk::Track* 		combinedFit	(const Trk::Track&	indetTrack,
						 const Trk::Track&	extrapolatedTrack,
						 const Trk::Track&	spectrometerTrack) const;

	/**ICombinedMuonTrackBuilder interface:
	   build and fit indet track extended to include MS Measurement set.
	   Adds material effects as appropriate plus calo energy-loss treatment */
	Trk::Track* 		indetExtension	(const Trk::Track&		indetTrack,
						 const Trk::MeasurementSet&	spectrometerMeas,
						 const Trk::TrackParameters*	innerParameters,
						 const Trk::TrackParameters*	middleParameters,
						 const Trk::TrackParameters*	outerParameters) const;

	/**ICombinedMuonTrackBuilder interface:
	   propagate to perigee adding calo energy-loss and material to MS track */
	Trk::Track*		standaloneFit	(const Trk::Track&	spectrometerTrack,
						 const Trk::Vertex*	vertex,
                                                 float bs_x, float bs_y, float bs_z ) const;

	/**ICombinedMuonTrackBuilder interface:
	   refit a track removing any indet measurements with optional addition of pseudoMeasurements
	   according to original extrapolation */
	Trk::Track*		standaloneRefit	(const Trk::Track&	combinedTrack,
                                                 float bs_x, float bs_y, float bs_z ) const;

	/**ITrackFitter interface:
	   refit a track */
	Trk::Track*		fit (
	    const Trk::Track&			track,
	    const Trk::RunOutlierRemoval	runOutlier = false,
	    const Trk::ParticleHypothesis	particleHypothesis = Trk::muon) const;

	/**ITrackFitter interface:
	   refit a track adding a PrepRawDataSet */
	Trk::Track*		fit (
	    const Trk::Track&			/*track*/,
	    const Trk::PrepRawDataSet&		/*rawDataSet*/,
	    const Trk::RunOutlierRemoval	/*runOutlier*/,
	    const Trk::ParticleHypothesis	/*particleHypothesis*/) const
	    { return 0; };

	/**ITrackFitter interface:
	   fit a set of PrepRawData objects */
	Trk::Track*		fit (
	    const Trk::PrepRawDataSet&,
	    const Trk::TrackParameters&		/*perigeeStartValue*/,
	    const Trk::RunOutlierRemoval	/*runOutlier*/,
	    const Trk::ParticleHypothesis	/*particleHypothesis*/) const
	    { return 0; };

	/**ITrackFitter interface:
	   refit a track adding a MeasurementSet */
	Trk::Track*		fit (
	    const Trk::Track&			/*track*/,
	    const Trk::MeasurementSet&		/*measurementSet*/,
	    const Trk::RunOutlierRemoval	/*runOutlier*/,
	    const Trk::ParticleHypothesis	/*particleHypothesis*/) const
	    { return 0; };

	/**ITrackFitter interface:
	   fit a set of MeasurementBase objects with starting value for perigeeParameters */
	Trk::Track*		fit (
	    const Trk::MeasurementSet&		/*measurementSet*/,
	    const Trk::TrackParameters&		/*perigeeStartValue*/,
	    const Trk::RunOutlierRemoval	/*runOutlier*/,
	    const Trk::ParticleHypothesis	/*particleHypothesis*/) const;

	/**ITrackFitter interface:
	   combined muon fit */
	Trk::Track*		fit (
	    const Trk::Track&			indetTrack,
	    const Trk::Track&			extrapolatedTrack,
	    const Trk::RunOutlierRemoval	runOutlier = false,
	    const Trk::ParticleHypothesis	particleHypothesis = Trk::muon) const;

    private:
        Trk::Track* addIDMSerrors(Trk::Track* track) const;
        Trk::PseudoMeasurementOnTrack*                  vertexOnTrack(
            		const Trk::TrackParameters*                 parameters,
            		const Trk::RecVertex&                           vertex) const;

	
	// helpers, managers, tools
	ToolHandle<Muon::IMuonTrackCleaner>		m_cleaner;
	ToolHandle<Trk::ITrackFitter>       		m_fitter;	// curved track fitter
	ToolHandle<Trk::ITrackSummaryTool>		m_trackSummary;
        ToolHandle<Muon::IMuonHoleRecoveryTool>         m_muonHoleRecovery;	
        ToolHandle<Muon::IMuonErrorOptimisationTool>    m_muonErrorOptimizer;
        ServiceHandle<Trk::ITrackingVolumesSvc>         m_trackingVolumesSvc;
        const Trk::Volume*                              m_calorimeterVolume;
        const Trk::Volume*                              m_indetVolume;
	// other configuration and tolerances
	bool						m_allowCleanerVeto;
	bool						m_cleanCombined;
	bool						m_recoverCombined;
        double                                          m_IDMS_xySigma;
        double                                          m_IDMS_rzSigma;
        bool                                            m_addIDMSerrors;
        
	
    };
 
}	// end of namespace

#endif	// MUIDTRACKBUILDER_OUTWARDSCOMBINEDMUONTRACKBUILDER_H



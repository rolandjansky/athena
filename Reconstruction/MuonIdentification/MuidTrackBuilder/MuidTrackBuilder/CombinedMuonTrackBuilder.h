/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// CombinedMuonTrackBuilder
//  AlgTool gathering  material effects along a combined muon track, in 
//  particular the TSOS'es representing the calorimeter energy deposit and
//  Coulomb scattering.
//  The resulting track is fitted at the IP using the ITrackFitter interface.
// 
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDTRACKBUILDER_COMBINEDMUONTRACKBUILDER_H
# define MUIDTRACKBUILDER_COMBINEDMUONTRACKBUILDER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace MagField {
    class IMagFieldSvc;
}

class CaloEnergy;
class MessageHelper;
namespace Muon
{
    class IMdtDriftCircleOnTrackCreator;
    class IMuonClusterOnTrackCreator;
    class IMuonErrorOptimisationTool;
    class IMuonHoleRecoveryTool;
    class IMuonTrackCleaner;
    class MuonIdHelperTool;
    class MuonEDMPrinterTool;
}
namespace Trk
{
    class IExtrapolator;
    class IIntersector;
    class IMaterialAllocator;
    class IPropagator;
    class ITrackSummaryTool;
    class PerigeeSurface;
    class PseudoMeasurementOnTrack;
    class RecVertex;
    class Surface;
    class TrackStateOnSurface;
    class TrackingVolume;
    class Volume;
    class ITrkMaterialProviderTool;
}
namespace Rec
{
    class IMuidCaloEnergy;
    class IMuidCaloTrackStateOnSurface;
    class IMuonTrackQuery;
    
    class CombinedMuonTrackBuilder: public AthAlgTool,
				    virtual public ICombinedMuonTrackBuilder
    {

    public:
	CombinedMuonTrackBuilder (const std::string& type, 
				  const std::string& name,
				  const IInterface* parent);
	~CombinedMuonTrackBuilder (void); // destructor
  
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
						 const Trk::MeasurementSet& 	spectrometerMeas,
						 const Trk::TrackParameters*	innerParameters,
						 const Trk::TrackParameters*	middleParameters,
						 const Trk::TrackParameters*	outerParameters) const;

	/**ICombinedMuonTrackBuilder interface:
	   propagate to perigee adding calo energy-loss and material to MS track */
	Trk::Track*		standaloneFit	(const Trk::Track&	spectrometerTrack,
						 const Trk::Vertex*	vertex,
                                                 float bs_x, float bs_y, float bs_z ) const;

	/**ICombinedMuonTrackBuilder interface:
	   refit a track removing any indet measurements with optional addition of pseudoMeasurements */
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
	    { return interfaceNotImplemented(); };

	/**ITrackFitter interface:
	   fit a set of PrepRawData objects */
	Trk::Track*		fit (
	    const Trk::PrepRawDataSet&,
	    const Trk::TrackParameters&		/*perigeeStartValue*/,
	    const Trk::RunOutlierRemoval	/*runOutlier*/,
	    const Trk::ParticleHypothesis	/*particleHypothesis*/) const
	    { return interfaceNotImplemented(); };

	/**ITrackFitter interface:
	   refit a track adding a MeasurementSet */
	Trk::Track*		fit (
	    const Trk::Track&			/*track*/,
	    const Trk::MeasurementSet&		/*measurementSet*/,
	    const Trk::RunOutlierRemoval	/*runOutlier*/,
	    const Trk::ParticleHypothesis	/*particleHypothesis*/) const
	    { return interfaceNotImplemented(); };

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
	// private methods
        bool optimizeErrors(Trk::Track* track) const;
        Trk::Track* addIDMSerrors(Trk::Track* track) const; 
	void						appendSelectedTSOS(
	    DataVector<const Trk::TrackStateOnSurface>&			trackStateOnSurfaces,
	    DataVector<const Trk::TrackStateOnSurface>::const_iterator	begin,
	    DataVector<const Trk::TrackStateOnSurface>::const_iterator	end) const;
	const CaloEnergy*				caloEnergyParameters(
	    const Trk::Track*						combinedTrack,
	    const Trk::Track*						muonTrack) const;
	Trk::Track*					createExtrapolatedTrack(
                                                                                const Trk::Track&					spectrometerTrack,
                                                                                const Trk::TrackParameters&					parameters,
                                                                                Trk::ParticleHypothesis					particleHypothesis,
                                                                                Trk::RunOutlierRemoval					runOutlier,
                                                                                const std::vector<const Trk::TrackStateOnSurface*>&		trackStateOnSurfaces,
                                                                                const Trk::RecVertex*					vertex,
                                                                                const Trk::Perigee* seedParameter = 0 ) const;
	Trk::Track*					createIndetTrack(
	    const Trk::TrackInfo&					info,
	    DataVector<const Trk::TrackStateOnSurface>::const_iterator	begin,
	    DataVector<const Trk::TrackStateOnSurface>::const_iterator	end) const;
	Trk::Track*					createMuonTrack(
	    const Trk::Track&	       				        muonTrack,
	    const Trk::TrackParameters*					parameters,
	    const CaloEnergy*						caloEnergy,
	    DataVector<const Trk::TrackStateOnSurface>::const_iterator	begin,
	    DataVector<const Trk::TrackStateOnSurface>::const_iterator	end,
	    unsigned							size) const;
	const Trk::TrackStateOnSurface*			createPhiPseudoMeasurement(
	    const Trk::Track&						track) const;
	std::vector<const Trk::TrackStateOnSurface*>*	createSpectrometerTSOS(
	    const Trk::Track&						spectrometerTrack) const;
	const Trk::TrackStateOnSurface*			entrancePerigee(
	    const Trk::TrackParameters*					parameters) const;
	const Trk::TrackParameters*			extrapolatedParameters(
	    bool&							badlyDeterminedCurvature,
	    const Trk::Track&						spectrometerTrack) const;
	void						finalTrackBuild(
	    Trk::Track*&						track) const;
	Trk::Track*					interfaceNotImplemented(void) const;
	void						momentumUpdate(
	    const Trk::TrackParameters*&				parameters,
	    double							updatedP,
	    bool							directionUpdate = false,
	    double							deltaPhi = 0.,
	    double							deltaTheta = 0.) const;
	double						normalizedChi2(
	    const Trk::Track&						track) const;
	const Trk::Track*				reallocateMaterial(
	    const Trk::Track&						spectrometerTrack) const;
	void						replaceCaloEnergy(
	    const CaloEnergy*						caloEnergy,
	    Trk::Track*							track) const;
	void						removeSpectrometerMaterial(
	    Trk::Track*&						track) const;
	Trk::Track*					trackCleaner(
	    Trk::Track*							combinedTrack,
	    const Trk::Track*						indetTrack,
	    const Trk::Track&						muonTrack) const;
	Trk::PseudoMeasurementOnTrack*			vertexOnTrack(
	    const Trk::TrackParameters&					parameters,
	    const Trk::RecVertex*					vertex) const;

        void dumpCaloEloss(const Trk::Track* track, std::string txt ) const;
        int countAEOTs(const Trk::Track* track, std::string txt ) const;
        bool checkTrack(std::string txt, Trk::Track* newTrack, Trk::Track* track) const;
	
	// helpers, managers, tools
	ToolHandle<Rec::IMuidCaloEnergy>		m_caloEnergyParam;
	ToolHandle<Rec::IMuidCaloTrackStateOnSurface>	m_caloTSOS;
	ToolHandle<Muon::IMuonTrackCleaner>		m_cleaner;
	ToolHandle<Muon::IMuonClusterOnTrackCreator>	m_cscRotCreator;
	ToolHandle<Trk::IExtrapolator>			m_extrapolator;
	ToolHandle<Trk::ITrackFitter>       		m_fitter;	// curved track fitter
	ToolHandle<Trk::ITrackFitter>       		m_fitterSL;	// straight line fitter
	ToolHandle<Muon::MuonIdHelperTool>		m_idHelperTool;
	ToolHandle<Trk::IIntersector>       		m_intersector;
	Trk::MagneticFieldProperties			m_magFieldProperties;
	ServiceHandle<MagField::IMagFieldSvc>		m_magFieldSvc;
	ToolHandle<Trk::IMaterialAllocator>		m_materialAllocator;
	ToolHandle<Muon::IMdtDriftCircleOnTrackCreator>	m_mdtRotCreator;
	ToolHandle<Muon::IMuonErrorOptimisationTool>	m_muonErrorOptimizer;
	ToolHandle<Muon::IMuonHoleRecoveryTool>		m_muonHoleRecovery;
	ToolHandle<Trk::IPropagator>        		m_propagator;
	ToolHandle<Trk::IPropagator>        		m_propagatorSL;
        ToolHandle<Muon::MuonEDMPrinterTool>      	m_printer;
	mutable ServiceHandle<Trk::ITrackingGeometrySvc>	m_trackingGeometrySvc;	// init with callback
	ServiceHandle<Trk::ITrackingVolumesSvc>		m_trackingVolumesSvc;
	ToolHandle<Rec::IMuonTrackQuery>		m_trackQuery;
	ToolHandle<Trk::ITrackSummaryTool>		m_trackSummary;
	ToolHandle<Trk::ITrkMaterialProviderTool>       m_materialUpdator;

	// other configuration and tolerances
	bool						m_allowCleanerVeto;
	bool						m_cleanCombined;
	bool						m_cleanStandalone;
	mutable bool					m_perigeeAtSpectrometerEntrance;
	bool						m_reallocateMaterial;
	double						m_badFitChi2;
	double						m_largeImpact;
	double						m_largeMomentumChange;
	double						m_largeMomentumError;
	double						m_largePhiError;
	double						m_lineMomentum;
	double						m_lowMomentum;
	unsigned					m_maxWarnings;
	double						m_minEnergy;
	double						m_numberSigmaFSR;
	bool						m_redoRots;
	double						m_vertex2DSigmaRPhi;
	double						m_vertex2DSigmaZ;
   	double						m_vertex3DSigmaRPhi;
	double						m_vertex3DSigmaZ;
	double						m_zECToroid;
        double                                          m_IDMS_xySigma;
        double                                          m_IDMS_rzSigma;
	// dummy (unused - kept for backwards compatibility)
	bool                                            m_indetSlimming; 
	bool						m_inputSlimming;

	// constants
	const Trk::Volume*				m_calorimeterVolume;
	const Trk::Volume*				m_indetVolume;

	// constant initialized the first time it's needed
	mutable const Trk::TrackingVolume*		m_spectrometerEntrance;
	
	// vertex region and phi modularity for pseudo-measurement constraints
	mutable Trk::RecVertex*					m_beamAxis;
	mutable Trk::PerigeeSurface*				m_perigeeSurface;
	double						m_sigmaPhiSector;
	mutable Trk::RecVertex*					m_vertex;

	// counters
	mutable unsigned				m_countAcceptedStandaloneFit;
	mutable unsigned				m_countBeamAxis;
	mutable unsigned				m_countCombinedCleanerVeto;
	mutable unsigned				m_countDegradedStandaloneFit;
	mutable unsigned				m_countExtensionCleanerVeto;
	mutable unsigned				m_countStandaloneCleanerVeto;
	mutable unsigned				m_countVertexRegion;

	// internal communication
	mutable const Trk::TrackParameters*		m_combinedEnergyParameters;
	mutable const Trk::TrackParameters*		m_muonEnergyParameters;
	
	// count warnings 
 	mutable MessageHelper*				m_messageHelper;

	bool                                            m_updateWithCaloTG;
	bool                                            m_useCaloTG;
	bool                                            m_iterateCombinedTrackFit;
	bool                                            m_refineELossCombinedTrackFit;
	bool                                            m_refineELossStandAloneTrackFit;
        bool                                            m_addElossID;
        bool                                            m_addIDMSerrors;
        bool                                            m_useRefitTrackError;
        const AtlasDetectorID*                          m_DetID;
    };
 
}	// end of namespace

#endif	// MUIDTRACKBUILDER_COMBINEDMUONTRACKBUILDER_H



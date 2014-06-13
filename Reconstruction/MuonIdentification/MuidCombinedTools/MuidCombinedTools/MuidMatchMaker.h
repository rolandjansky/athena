/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuidMatchMaker
//  AlgTool to match all tracks from an extrapolated muon-spectrometer
//  collection to any corresponding tracks from an inner detector collection.
//  Perform a full track-fit to the resultant combined tracks.
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDCOMBINEDTOOLS_MUIDMATCHMAKER_H
#define MUIDCOMBINEDTOOLS_MUIDMATCHMAKER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuidInterfaces/IMuidMatchMaker.h"
#include "TrkParameters/TrackParameters.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class CaloEnergy;
class MessageHelper;
class MsgStream;
class MuidTrack;
class PerigeeParameters;
namespace Trk
{
    class IIntersector;
    class IPropagator;
    class ITrackAmbiguityProcessorTool;
    class Surface;
    class Track;
}

namespace Rec
{

class ICombinedMuonTrackBuilder;
class IMuidCaloEnergyParam;
class IMuidCaloTrackStateOnSurface;
class IMuidMuonRecovery;
class IMuonMatchQuality;
class IMuonMomentumBalanceSignificance;
class IMuonScatteringAngleSignificance;    
class IMuonTrackQuery;
class IndetMatchCandidate;
class MuonMatch;
    
class MuidMatchMaker: public AthAlgTool,
		      virtual public IMuidMatchMaker
{

public:
    MuidMatchMaker	(const std::string&	type, 
			 const std::string&	name,
			 const IInterface*	parent);
    ~MuidMatchMaker(void); // destructor
  
    StatusCode		initialize();
    StatusCode		finalize();

    /**IMuidMatchMaker interface:
       back convert perigee (legacy function needed during migration) */
    PerigeeParameters*	convertPerigee (const Trk::Perigee* trkPerigee) const;
    
    /**IMuidMatchMaker interface:
       algorithmic code for matching muon spectrometer to inner detector */    
    MuidTrackContainer*	execute(const TrackCollection*	extrapolatedTracks,
				const TrackCollection*	indetTracks,
				const TrackCollection*	spectrometerTracks) const;
private:
    // private methods
    void			accumulateStatistics (MuidTrackContainer&	muonMatches) const;
    bool			bestMatchChooser (const MuonMatch&	match1,
						  const MuonMatch&	match2) const;
    const Trk::Track*		buildCombinedTrack (const Trk::Track&	indetTrack,
						    const Trk::Track&	extrapolatedTrack,
						    const Trk::Track&	spectrometerTrack) const;
    bool			combinedTrackQualityCheck (const Trk::Track& combinedTrack,
							   const Trk::Track& indetTrack,
							   const Trk::Track& spectrometerTrack) const;
    void			debugInput (const TrackCollection*	indetTracks,
					    const TrackCollection*	extrapolatedTracks,
					    const TrackCollection*	spectrometerTracks) const;
    void			debugOutput (const MuidTrackContainer*	combinedMuons,
					     const TrackCollection*	indetTracks,
					     const TrackCollection*	spectrometerTracks) const;
    void			evaluateMatchProperties (MuonMatch& 	muonMatch) const;
    bool			extrapolatedNeedsRefit (MuonMatch& 	muonMatch) const;
    void			indetCandidates (std::vector<IndetMatchCandidate*>&     candidates,
						 const Trk::Track*      extrapolatedTrack,
						 const TrackCollection* indetTracks,
						 const Trk::Track*      spectrometerTrack) const;
    const Trk::Track*		indetExtension (const Trk::Track&	indetTrack,
						const Trk::Track&	spectrometerTrack) const;
    bool			loosePositionMatch (const PerigeeParameters&	indetPerigee,
						    const MuonMatch&		candidate) const;
    bool			momentumBalance (const Trk::Track*     	indetTrack,
						 const Trk::Perigee*	outgoingIndetPerigee,
						 const Trk::Perigee*	outgoingSpectrometerPerigee,
						 const Trk::Track*	spectrometerTrack) const;
    void			printPerigee (const Trk::Perigee& perigee) const;
    void			resolveAmbiguities (MuidTrackContainer&	muonMatches) const;
    
    // helpers, managers, tools
    ToolHandle<Trk::ITrackAmbiguityProcessorTool>	m_ambiguityProcessor;
    ToolHandle<IMuidCaloEnergyParam>			m_caloEnergyParam;
    ToolHandle<IMuidCaloTrackStateOnSurface>		m_caloTSOS;
    ToolHandle<Trk::IIntersector>			m_intersector;
    ServiceHandle<MagField::IMagFieldSvc>               m_magFieldSvc; 
    ToolHandle<IMuonMatchQuality>			m_matchQuality;
    ToolHandle<IMuonMomentumBalanceSignificance>	m_momentumBalanceTool;
    ToolHandle<IMuidMuonRecovery>			m_muonRecovery; 
    ToolHandle<ICombinedMuonTrackBuilder>		m_outwardsBuilder;
    ToolHandle<Trk::IPropagator>			m_propagator;
    ToolHandle<IMuonScatteringAngleSignificance>	m_scatteringAngleTool;
    ToolHandle<ICombinedMuonTrackBuilder>		m_trackBuilder;
    ToolHandle<IMuonTrackQuery>				m_trackQuery;

    // configuration
    double			m_badFitChi2;
    double			m_badMatchChi2;
    double			m_deltaEtaPreSelection;	// Initial matching: eta pre-selection
    double			m_deltaPhiPreSelection;	// Initial matching: phi pre-selection
    double			m_fieldIntegralCut;	// Favour outer match for low spectrometer Bdl
    double			m_indetMinEnergy;	// Minimum energy for InDet track selection
    double			m_indetMinPt;	        // Minimum pt for InDet track selection
    double			m_indetPullCut;		// Cut on indet-combined momentum pull
    double			m_looseMatchCone;	// Loose match min eta/phi cone size
    double			m_looseMatchScaling;	// Loose match cone scaling with inv pT
    double			m_looseMatchSigma;	// Loose match energy balance limit
    double			m_matchChiSquaredCut;	// Chi2 cut for classification as good match 
    bool			m_matchTrtTracks;	// match TRT tracks (without silicon)
    unsigned			m_maxWarnings;		// configure MessageHelper WARNINGs
    double			m_momentumBalanceCut;	// Cut on momentum balance of combined track
    double			m_spectrometerTolerance;// min tolerance for spectrometerPositionMatch
    
    // constants
    double			m_barrelCotTheta;
    const Trk::Surface*		m_caloBackwardDisc;
    const Trk::Surface*		m_caloCylinder;
    const Trk::Surface*		m_caloForwardDisc;
    double			m_scatteringAngle;	// Coulomb scattering constant

    // counters
    mutable unsigned	       	m_countAmbigID;
    mutable unsigned	       	m_countAmbigMS;
    mutable unsigned	       	m_countBadLoose1;
    mutable unsigned	       	m_countBadLoose2;
    mutable unsigned	       	m_countBestMatch1;
    mutable unsigned	       	m_countBestMatch2;
    mutable unsigned	       	m_countGoodLoose1;
    mutable unsigned	       	m_countGoodLoose2;
    mutable unsigned	       	m_countGoodMatch1;
    mutable unsigned	       	m_countGoodMatch2;
    mutable unsigned	       	m_countInnerMatch1;
    mutable unsigned	       	m_countInnerMatch2;
    mutable unsigned	       	m_countMSMatch1;
    mutable unsigned	       	m_countMSMatch2;
    mutable unsigned	       	m_countOuterMatch1;
    mutable unsigned	       	m_countOuterMatch2;
    mutable unsigned	       	m_countNO;
    mutable unsigned	       	m_countSA;
    mutable unsigned	       	m_countSimpleMatch1;
    mutable unsigned	       	m_countSimpleMatch2;
    mutable unsigned	       	m_countTightMatch1;
    mutable unsigned	       	m_countTightMatch2;
    mutable unsigned	       	m_countUN;
    mutable unsigned		m_countBadBoth;
    mutable unsigned		m_countBadFit;
    mutable unsigned		m_countBadMatch;
    mutable unsigned		m_countBadMatch1;
    mutable unsigned		m_countBadMatch2;
    mutable unsigned		m_countBadMatchInner;
    mutable unsigned		m_countBadMatchOuter;
    mutable unsigned	       	m_countFailChargeConflict;
    mutable unsigned	       	m_countFailEnergyGain;
    mutable unsigned	       	m_countFailEnergyLoss;
    mutable unsigned		m_countFailMatchWindowMS;
    mutable unsigned		m_countFailMatchWindowSA;
    mutable unsigned	       	m_countFailMomentumBalance;
    mutable unsigned	       	m_countFailPositionMatch;
    mutable unsigned	       	m_countFailLooseFit;
    mutable unsigned	       	m_countFailInnerFit;
    mutable unsigned	       	m_countFailOuterFit;
    mutable unsigned	       	m_countFailTightFit;
    mutable unsigned	       	m_countFailTrackFit;
    mutable unsigned	       	m_countOutwardsTracks;
    mutable unsigned	       	m_countCSignificance1;
    mutable unsigned	       	m_countCSignificance2;
    mutable unsigned	       	m_countNSignificance1;
    mutable unsigned	       	m_countNSignificance2;
    mutable unsigned	       	m_countPSignificance1;
    mutable unsigned	       	m_countPSignificance2;
    
    // internal communication
    mutable bool		m_failChargeConflict;
    mutable bool		m_failEnergyGain;
    mutable bool		m_failEnergyLoss;
    mutable bool		m_failMomentumBalance;    
    mutable bool		m_failPositionMatch;

    // count warnings
    mutable MessageHelper*	m_messageHelper;
    
};

}	// end of namespace

#endif // MUIDCOMBINEDTOOLS_MUIDMATCHMAKER_H



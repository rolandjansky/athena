/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTrackQuery.h, (c) ATLAS Combined Muon software
///////////////////////////////////////////////////////////////////

#ifndef MUIDTRACKBUILDER_MUONTRACKQUERY_H
#define MUIDTRACKBUILDER_MUONTRACKQUERY_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuidInterfaces/IMuonTrackQuery.h"

namespace Muon
{
    class IMdtDriftCircleOnTrackCreator;
    class MuonEDMHelperTool;
    class MuonIdHelperTool;
}
namespace Trk
{
    class ITrackingGeometrySvc;
    class ITrackFitter;
}
namespace Rec
{
  
    class MuonTrackQuery : public AthAlgTool,
			   virtual public IMuonTrackQuery
    {
    public:
	MuonTrackQuery (const std::string& type, 
			 const std::string& name,
			 const IInterface* parent);
	~MuonTrackQuery (void); // destructor
  
	StatusCode		initialize();
	StatusCode		finalize();

	/**IMuonTrackQuery interface:
	   caloEnergy from appropriate TSOS */
	const CaloEnergy*	caloEnergy (const Trk::Track& track) const;

	/**IMuonTrackQuery interface:
	   track energy deposit in calorimeters (as fitted or otherwise applied) */
	double			caloEnergyDeposit (const Trk::Track& track) const;

	/**IMuonTrackQuery interface:
	   field integral along track from momentum kick between measurements */
	FieldIntegral		fieldIntegral (const Trk::Track& track) const;


	/**IMuonTrackQuery interface:
	   does track have at least 3 TSOS's representing calorimeter ? */
	bool			isCaloAssociated (const Trk::Track& track) const;

	/**IMuonTrackQuery interface:
	   does track have measurements from indet and spectrometer ? */
	bool			isCombined (const Trk::Track& track) const;

	/**IMuonTrackQuery interface:
	   does track have perigee inside indet ? */
	bool			isExtrapolated (const Trk::Track& track) const;

	/**IMuonTrackQuery interface:
	   does track have fitted curvature ? */
	bool			isLineFit (const Trk::Track& track) const;

	/**IMuonTrackQuery interface:
	   is track (roughly) projective towards IP? */
	bool			isProjective (const Trk::Track& track) const;

	/**IMuonTrackQuery interface:
	   is there a long/short chamber overlap? */
	bool			isSectorOverlap (const Trk::Track& track) const;

	/**IMuonTrackQuery interface:
	   does track have TrackParameters at every TSOS ? */
	bool			isSlimmed (const Trk::Track& track) const;

	/**IMuonTrackQuery interface:
	   significance of momentum balance for combined tracks (biassed residual) */
	double			momentumBalanceSignificance (const Trk::Track& track) const;

	/**IMuonTrackQuery interface:
	   number of PseudoMeasurements on track (counts one for any vertex measurement) */
	unsigned		numberPseudoMeasurements (const Trk::Track& track) const;

	/**IMuonTrackQuery interface:
	   perigee expressed outgoing from IP */
	const Trk::Perigee*	outgoingPerigee (const Trk::Track& track) const;
     
	/**IMuonTrackQuery interface:
	   track expressed outgoing from IP */
	const Trk::Track*	outgoingTrack (const Trk::Track& track) const;
     
	/**IMuonTrackQuery interface:
	   significance of early scattering angle pattern for combined tracks (wider than gaussian) */
	ScatteringAngleSignificance	scatteringAngleSignificance (const Trk::Track& track) const;

	/**IMuonTrackQuery interface:
	   trackParameters at innermost measurement TSOS in MS */
	const Trk::TrackParameters*	spectrometerParameters (const Trk::Track& track) const;
        
	/**IMuonTrackQuery interface:
	   assess the number of additional phi measurements needed for MS (or SA) track fit */
	unsigned			spectrometerPhiQuality (const Trk::Track& track) const;
         
	/**IMuonTrackQuery interface:
	   trackParameters at innermost trigger chamber TSOS in MS */
	const Trk::TrackParameters*	triggerStationParameters (const Trk::Track& track) const;
    
    private:
	const Trk::TrackParameters*	flippedParameters (const Trk::TrackParameters& params) const;

	// tools and services
	ToolHandle<Trk::ITrackFitter>       		m_fitter;
	ToolHandle<Muon::MuonEDMHelperTool>		m_helper; 
	ToolHandle<Muon::MuonIdHelperTool>		m_idHelper;
	ToolHandle<Muon::IMdtDriftCircleOnTrackCreator>	m_mdtRotCreator;
        ServiceHandle<Trk::ITrackingGeometrySvc>        m_trackingGeometrySvc;
	
    };
 
}	// end of namespace

#endif // MUIDTRACKBUILDER_MUONTRACKQUERY_H



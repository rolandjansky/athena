/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKSTATISTICSTOOL_MUONTRACKSTATISTICSTOOL_H
#define MUONTRACKSTATISTICSTOOL_MUONTRACKSTATISTICSTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "TrkToolInterfaces/ITruthToTrack.h"

#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"

#include <string>

class MsgStream;
class StoreGateSvc;

/* namespace Muon { */
/*   class MuonEDMHelperTool; */
/* }     */

/* namespace Trk { */
/*   class ITruthToTrack; */
/* } */

static const InterfaceID IID_MuonTrackStatisticsTool("MuonTrackStatisticsTool",1,0);

class MuonTrackStatisticsTool : public AthAlgTool
{
public:
	
	// AlgTool Constructor
	MuonTrackStatisticsTool (const std::string&, const std::string&, const IInterface*);
	~MuonTrackStatisticsTool(){};
	
	// tool interface
	static const InterfaceID& interfaceID() { return IID_MuonTrackStatisticsTool; }
	
	
	// Gaudi algorithm hooks
	StatusCode initialize();
	StatusCode finalize();
	//statistics to be counted
	struct TrackCounters
    {    
		TrackCounters(std::string trkLoc) :
		trackLocation(trkLoc),nEvents(0), nTracks(0), nHits(0), nEtaHits(0), nEtaTrig(0), nPhiHits(0),
		nScatter(0), nHoles(0),  nPsudo(0), summedchi2(0)
		{}
		
		std::string trackLocation;
		int nEvents;
		int nTracks;
		int nHits;
		int nEtaHits;
		int nEtaTrig;
		int nPhiHits;
		int nScatter;
		int nHoles;
		int nPsudo;
		double summedchi2;
		
    };
	
	struct TruthTrackCounters
    {    
		TruthTrackCounters(std::string trkLoc) :
		trackLocation(trkLoc),nEvents(0), nTracks(0)
		{
			for (unsigned int i=0; i<3 ; i++){
				nPIXELhits[i]=0;
				nSCThits[i]=0;
				nTRThits[i]=0;
				nMDThits[i]=0;
				nRPChits[i]=0;
				nTGChits[i]=0;
				nCSChits[i]=0;
			}
		};
		
		std::string trackLocation;
		int nEvents;
		int nTracks;
		int nPIXELhits[3];
		int nSCThits[3];
		int nTRThits[3];
		int nMDThits[3];
		int nRPChits[3];
		int nTGChits[3];
		int nCSChits[3];
		
    };
	
	//function to access tracks and update counters
	//first version updates all track counters (for all selected algorithms), used by MuonTrackStatisticsAlg
	//second version updates a single counter
	//third version updates a single counter with the specified track collection   
	//(useful for debugging within a reconstruction algorithm)  
	
	StatusCode updateTrackCounters() const;
	StatusCode updateTrackCounters(TrackCounters& counters) const;
	StatusCode updateTrackCounters(TrackCounters& counters, const TrackCollection& tracks) const;
	
	StatusCode updateTruthTrackCounters(TruthTrackCounters& counters) const;
	StatusCode updateTruthTrackCounters(TruthTrackCounters& counters, const DetailedTrackTruthCollection& TruthMap) const;
	
	void addTrackCounters( std::string trkLoc ) const;
	//  void addTruthTrackCounters( std::string trkLoc ) const;
	std::string printTrackCounters() const;
	
private:
	
	// Cache the StoreGateSvc ptr for efficiency
	StoreGateSvc*	p_SGevent;
	
	// access to Id Helpers
	ToolHandle<Muon::MuonEDMHelperTool>   m_helperTool;
	ToolHandle<Trk::ITruthToTrack>        m_truthToTrack;
	
	
    
	
	bool                m_doTruth;
	MsgStream*          m_log;
	bool                m_debug;
	bool                m_verbose;
	
	mutable std::vector<MuonTrackStatisticsTool::TrackCounters* > m_allCounters;
	mutable std::vector<MuonTrackStatisticsTool::TruthTrackCounters* > m_allTruthCounters;
	
	void storeTruthTracks(void);
	
};

#endif // MUONTRACKSTATISTICSTOOL_MUONTRACKSTATISTICSTOOL_H



/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 MuonTrackStatisticsTool
 ***************************************************************************/

#include <math.h>

#include "MuonTrackPerformance/MuonTrackStatisticsTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "TString.h"
#include "HepMC/GenParticle.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/MuonTrackSummary.h"

//#include "GeneratorObjects/McEventCollection.h"
//#include "TrackRecord/TrackRecordCollection.h"
//#include "HepMC/GenParticle.h"

//#include "TrkParameters/Perigee.h"
//#include "TrkParameters/MeasuredPerigee.h"
//#include "TrkValTools/ResidualPull.h"
//#include "TrkEventPrimitives/ResidualPull.h"

//#include "TrkSurfaces/StraightLineSurface.h"
//#include "TrkParameters/TrackParameters.h"
//#include "TrkGeometry/MagneticFieldProperties.h"

// INCLUDE GAUDI HEADER FILES:
#include "GaudiKernel/MsgStream.h"

/////////////////////////////////////////////////////////////////

//  CONSTRUCTOR:
MuonTrackStatisticsTool::MuonTrackStatisticsTool(const std::string& t, const std::string& n, const IInterface* p)
: 
AthAlgTool(t,n,p),
m_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelpertTool"),
m_doTruth     (false)



{
	declareInterface<MuonTrackStatisticsTool>(this);
	declareProperty("doTruth",         m_doTruth);
	
	
	
}

// INITIALIZE METHOD:

StatusCode MuonTrackStatisticsTool::initialize()
{
	// MSGStream object to output messages from your sub tool
	m_log = new MsgStream(msgSvc(), name());
	*m_log<<MSG::INFO<< "!initialize!" << endreq; 
	
	// Locate the StoreGateSvc and initialize our local ptr
	StatusCode sc = service("StoreGateSvc", p_SGevent);
	if (!sc.isSuccess() || 0 == p_SGevent) 
    {
		*m_log << MSG::ERROR << "MuonTrackStatisticsTool::initialize() :  Could not find StoreGateSvc" << endreq;
		return	sc;
    }
	
	sc = m_helperTool.retrieve();
	if (sc.isSuccess()){
		*m_log<<MSG::DEBUG << "Retrieved " << m_helperTool << endreq;
	}else{
		*m_log<<MSG::FATAL<<"Could not get " << m_helperTool <<endreq; 
		return sc;
	}
	
	return StatusCode::SUCCESS;
}



/////////////////////////////////////////////////////////////////
// ATHENA FINALIZE:


StatusCode MuonTrackStatisticsTool::finalize() {
	
	if (m_doTruth)
    {
		//empty for now, will impliment access of truth later
		
    }
	
	
	delete m_log;
	
	
	return StatusCode::SUCCESS;
}


StatusCode MuonTrackStatisticsTool::updateTrackCounters() const{
	
	std::vector<MuonTrackStatisticsTool::TrackCounters*>::iterator counter_it = m_allCounters.begin();
	std::vector<MuonTrackStatisticsTool::TrackCounters*>::iterator counter_itEnd = m_allCounters.end();
	StatusCode sc = StatusCode::SUCCESS;
	for (; counter_it!=counter_itEnd ; counter_it++) 
    {
		
		sc = MuonTrackStatisticsTool::updateTrackCounters(**counter_it);
		if(sc.isFailure()){
			*m_log << MSG::FATAL << "Could not update TrackCounters" << endreq;
			return sc;
		}
    }
	
	if(m_doTruth){
		std::vector<MuonTrackStatisticsTool::TruthTrackCounters*>::iterator counterTruth_it = m_allTruthCounters.begin();
		std::vector<MuonTrackStatisticsTool::TruthTrackCounters*>::iterator counterTruth_itEnd = m_allTruthCounters.end();
		for (; counterTruth_it!=counterTruth_itEnd ; counterTruth_it++) 
		{
			
			sc = MuonTrackStatisticsTool::updateTruthTrackCounters(**counterTruth_it);
			if(sc.isFailure()){
				*m_log << MSG::FATAL << "Could not update TruthTrackCounters" << endreq;
				return sc;
			}
		}
	}
	
	return sc;
	
}

StatusCode MuonTrackStatisticsTool::updateTruthTrackCounters(TruthTrackCounters& counters) const{
	
	
	*m_log<<MSG::INFO<<"!updateTruthTrackCounters on:"<<counters.trackLocation<<endreq;
	StatusCode sc = StatusCode::SUCCESS;
	
	//retrieve tracks
	const DetailedTrackTruthCollection* TruthMap  = NULL;
	
	
	if (p_SGevent->contains<DetailedTrackTruthCollection>( counters.trackLocation)){
		
		*m_log << MSG::INFO << "Acessing DetailedTrackTruthCollection for Truth" <<  counters.trackLocation << endreq;
		
		if (counters.trackLocation != "") {
			if (p_SGevent->retrieve (TruthMap , counters.trackLocation).isFailure()) {
				*m_log << MSG::INFO << "DetailedTrackTruthCollection \"" << counters.trackLocation<<"\" not found."<< endreq;
				return StatusCode::SUCCESS;
			}
		}else {
			*m_log << MSG::INFO << "No DetailedTrackTruthCollection key specified to go with this"<<"TrackCollection!"<< endreq;
			return StatusCode::SUCCESS;
		}
		
		if (TruthMap) {
			*m_log << MSG::INFO << "Retrieved " << TruthMap->size()<< " TrackTruth elements from storegate"<< endreq;
			
			sc = MuonTrackStatisticsTool::updateTruthTrackCounters(counters, *TruthMap);
		}
	}else{
		*m_log << MSG::INFO << " Track Collection "  <<  counters.trackLocation << "not available to be retrieved" << endreq; 
		++counters.nEvents;
		
	}
	
	
	return sc;
}	






StatusCode MuonTrackStatisticsTool::updateTrackCounters(TrackCounters& counters) const{
	
	*m_log<<MSG::INFO<<"!updateTrackCounters on:"<<counters.trackLocation<<endreq;
	StatusCode sc = StatusCode::SUCCESS;
	
	//retrieve tracks
	const TrackCollection* tracks = 0;	
	
	if (p_SGevent->contains<TrackCollection>( counters.trackLocation)){
		sc = p_SGevent->retrieve(tracks,counters.trackLocation);
		if (sc.isFailure() ) {
			*m_log << MSG::FATAL << " Could not retrieve track collection " << counters.trackLocation << endreq;
			
		}else{
			*m_log << MSG::INFO << " Number of retrieved tracks = "  << tracks->size() << endreq;  
			sc = MuonTrackStatisticsTool::updateTrackCounters(counters, *tracks);
			
		}
		
	}else{
		*m_log << MSG::INFO << " Track Collection "  <<  counters.trackLocation << "not available to be retrieved" << endreq; 
		++counters.nEvents;
		
	}
	
	
	return sc;
	
}


StatusCode MuonTrackStatisticsTool::updateTruthTrackCounters(TruthTrackCounters& counters,  const DetailedTrackTruthCollection& TruthMap) const{
	
	*m_log << MSG::INFO << "MuonTrackStatisticsTool calling updateTruthTrackCounters: " << counters.trackLocation<<endreq;
	
	++counters.nEvents;
	
	StatusCode sc = StatusCode::SUCCESS;
	
	
	
	if( TruthMap.empty() ) return sc;
	
	
	//update each set of trackcounters for each track
	
	
	
	
	
	
	
	DetailedTrackTruthCollection::const_iterator it_start=TruthMap.begin();
	DetailedTrackTruthCollection::const_iterator it_end=TruthMap.end();
	DetailedTrackTruthCollection::const_iterator it=it_start;
	int myindex=0;
	
	for(it=it_start;it!=it_end;it++){
		
		counters.nPIXELhits[0]+=(*it).second.statsCommon()[SubDetHitStatistics::Pixel];
		counters.nSCThits[0]+=(*it).second.statsCommon()[SubDetHitStatistics::SCT];
		counters.nTRThits[0]+=(*it).second.statsCommon()[SubDetHitStatistics::TRT];
		counters.nMDThits[0]+=(*it).second.statsCommon()[SubDetHitStatistics::MDT];
		counters.nRPChits[0]+=(*it).second.statsCommon()[SubDetHitStatistics::RPC];
		counters.nTGChits[0]+=(*it).second.statsCommon()[SubDetHitStatistics::TGC];
		counters.nCSChits[0]+=(*it).second.statsCommon()[SubDetHitStatistics::CSC];
		
		counters.nPIXELhits[1]+=(*it).second.statsTrack()[SubDetHitStatistics::Pixel];
		counters.nSCThits[1]+=(*it).second.statsTrack()[SubDetHitStatistics::SCT];
		counters.nTRThits[1]+=(*it).second.statsTrack()[SubDetHitStatistics::TRT];
		counters.nMDThits[1]+=(*it).second.statsTrack()[SubDetHitStatistics::MDT];
		counters.nRPChits[1]+=(*it).second.statsTrack()[SubDetHitStatistics::RPC];
		counters.nTGChits[1]+=(*it).second.statsTrack()[SubDetHitStatistics::TGC];
		counters.nCSChits[1]+=(*it).second.statsTrack()[SubDetHitStatistics::CSC];
		
		counters.nPIXELhits[2]+=(*it).second.statsTruth()[SubDetHitStatistics::Pixel];
		counters.nSCThits[2]+=(*it).second.statsTruth()[SubDetHitStatistics::SCT];
		counters.nTRThits[2]+=(*it).second.statsTruth()[SubDetHitStatistics::TRT];
		counters.nMDThits[2]+=(*it).second.statsTruth()[SubDetHitStatistics::MDT];
		counters.nRPChits[2]+=(*it).second.statsTruth()[SubDetHitStatistics::RPC];
		counters.nTGChits[2]+=(*it).second.statsTruth()[SubDetHitStatistics::TGC];
		counters.nCSChits[2]+=(*it).second.statsTruth()[SubDetHitStatistics::CSC];
		
		
		*m_log << MSG::INFO << myindex << ".) " << "Index: "  <<  (*it).first.index() << "		" << (*it).second << " (Pixel, SCT, TRT, MDT, RPC, TGC, CSC) " << endreq;
		*m_log << MSG::INFO << "	GenParticle info:" <<endreq;
		for( unsigned int i=0; i< (*it).second.trajectory().size(); i++) {
			*m_log << MSG::INFO << "		Particle "<<i;
			if( !(*it).second.trajectory().at(i).cptr() ) {
				*m_log << MSG::INFO << " has a null pointer: "<< (*it).second.trajectory().at(i).cptr() << endreq;
			}else{
				*m_log << MSG::INFO << endreq<< "			- pdg_id: "<< (*it).second.trajectory().at(i).cptr()->pdg_id() << endreq;
				*m_log << MSG::INFO << "			- status: "<< (*it).second.trajectory().at(i).cptr()->status() << endreq;
				counters.nTracks++;
			}
			
		}
		myindex++;
	}		
	
	
	return sc;
	
}


StatusCode MuonTrackStatisticsTool::updateTrackCounters(TrackCounters& counters,  const TrackCollection& tracks) const{
	
	*m_log << MSG::INFO << "MuonTrackStatisticsTool calling updateTrackCounters: " << counters.trackLocation<<endreq;
	
	++counters.nEvents;
	
	StatusCode sc = StatusCode::SUCCESS;
	
	
	
	if( tracks.empty() ) return sc;
	
	
	//update each set of trackcounters for each track
	
	counters.nTracks += tracks.size();
	
	TrackCollection::const_iterator it = tracks.begin();
	TrackCollection::const_iterator it_end = tracks.end();
	for( ;it!=it_end;++it ){
	  const Trk::Track* track = *it;
	  if( !track->trackSummary() || !track->trackSummary()->muonTrackSummary() ) continue;
	  const Trk::MuonTrackSummary& summary = *track->trackSummary()->muonTrackSummary();

	  double chi2dof = ((*it)->fitQuality()->chiSquared())/((*it)->fitQuality()->doubleNumberDoF()) ;
	  counters.nHits += summary.netaHits() + summary.nphiHits();
	  counters.nEtaHits += summary.netaHits();
	  counters.nPhiHits += summary.nphiHits();
	  counters.nEtaTrig += 0;
	  counters.nScatter += summary.nscatterers();
	  counters.nPsudo += summary.npseudoMeasurements();
	  counters.nHoles += summary.nholes();
	  counters.summedchi2 += chi2dof;
	}
	
    
	
	return sc;
	
}



void MuonTrackStatisticsTool::addTrackCounters(std::string trkLoc) const
{
	TString temp_string(trkLoc);
	
	if( temp_string.Contains("Truth") && m_doTruth) {
		*m_log << MSG::INFO << "MuonTrackStatisticsTool calling addTrackCounters for truth: " << trkLoc << endreq;
		TruthTrackCounters * counters;
		counters = new TruthTrackCounters (trkLoc);
		m_allTruthCounters.push_back(counters);
	}else if( !temp_string.Contains("Truth")) {
		
		*m_log << MSG::INFO << "MuonTrackStatisticsTool calling addTrackCounters for reco: " << trkLoc << endreq;
		TrackCounters * counters;
		counters = new TrackCounters (trkLoc);
		m_allCounters.push_back(counters);
		
	}
	return;
}


std::string MuonTrackStatisticsTool::printTrackCounters() const
{
	
	std::ostringstream sout;
	
	
	std::vector<MuonTrackStatisticsTool::TrackCounters*>::iterator counter_it = m_allCounters.begin();
	std::vector<MuonTrackStatisticsTool::TrackCounters*>::iterator counter_itEnd = m_allCounters.end();
	std::vector<MuonTrackStatisticsTool::TruthTrackCounters*>::iterator truthcounter_it_start = m_allTruthCounters.begin();
	std::vector<MuonTrackStatisticsTool::TruthTrackCounters*>::iterator truthcounter_itEnd = m_allTruthCounters.end();
	std::vector<MuonTrackStatisticsTool::TruthTrackCounters*>::iterator truthcounter_it = m_allTruthCounters.begin();
	
	double trksPerEvent;
	double hitsPerTrk  ;
	double etaPerTrk   ;
	double tetaPerTrk  ;
	double phiPerTrk   ;
	double scatPerTrk  ;
	double holePerTrk  ;
	double chi2PerTrk  ;
	
	for (; counter_it!=counter_itEnd ; counter_it++) 
	{
		if ((*counter_it)->nEvents != 0 && (*counter_it)->nTracks != 0){
			trksPerEvent = (double) (*counter_it)->nTracks / (*counter_it)->nEvents;
			hitsPerTrk  = (double) (*counter_it)->nHits / (*counter_it)->nTracks;
			etaPerTrk  = (double) (*counter_it)->nEtaHits / (*counter_it)->nTracks;
			tetaPerTrk  = (double) (*counter_it)->nEtaTrig / (*counter_it)->nTracks;
			phiPerTrk  = (double) (*counter_it)->nPhiHits / (*counter_it)->nTracks;
			scatPerTrk  = (double) (*counter_it)->nScatter / (*counter_it)->nTracks;
			holePerTrk  = (double) (*counter_it)->nHoles / (*counter_it)->nTracks;
			chi2PerTrk = (*counter_it)->summedchi2 / (*counter_it)->nTracks;
			
		} else {
			trksPerEvent=0.;
			hitsPerTrk  =0.;
			etaPerTrk   =0.;
			tetaPerTrk  =0.;
			phiPerTrk   =0.;
			scatPerTrk  =0.;
			holePerTrk  =0.;
			chi2PerTrk  =0.;
		}
		
		int TruthTrackCounter = -1;
		double trksPerTrtrk=-1;
		
		if(m_doTruth){
			
			for (truthcounter_it = truthcounter_it_start; truthcounter_it!=truthcounter_itEnd ; truthcounter_it++) {
				TString TruthCollectionName = (*truthcounter_it)->trackLocation;
				if (TruthCollectionName.Contains((*counter_it)->trackLocation)){
					TruthTrackCounter = (*truthcounter_it)->nTracks;
					*m_log << MSG::INFO << "MuonTrackStatisticsTool - Found matching TruthCollection for: " << (*counter_it)->trackLocation << endreq;
				}
			}
			
			
			if( TruthTrackCounter==0 && (*counter_it)->nTracks == 0){
				trksPerTrtrk = 1;
			}else if( TruthTrackCounter==0 && (*counter_it)->nTracks != 0){
				trksPerTrtrk = -1;
			}else{
				trksPerTrtrk = (double) (*counter_it)->nTracks/(double) TruthTrackCounter;
			}
		}
		
		
		if (trksPerTrtrk < 0 && m_doTruth){ 
			*m_log << MSG::WARNING << "MuonTrackStatisticsTool - Could not find matching TruthCollection for: " << (*counter_it)->trackLocation << endreq;
			sout.precision(4);
			sout << std::endl;
			sout << ">>>> MuonTrackStatisticsAlg Summary: Track Container = " << (*counter_it)->trackLocation  << std::endl;
			sout << "-----------------------------------------------------------------------------------------------------------------------------------" << std::endl;
			sout << "|| Events  || Tracks  || Trk/Evt || Hit/Trk || Eta/Trk ||"
			<< " TrigEta/T || Phi/Trk || Scat/Tk || Hole/Tk || Ch2/dof/T || Trks/TruthT ||" << std::endl;
			
			sout << "|| " << std::setw(7) << (*counter_it)->nEvents << " || "         << std::setw(7) << (*counter_it)->nTracks
			<< " || " << std::setw(7) << trksPerEvent  << " || "    << std::setw(7) << hitsPerTrk  << " || " << std::setw(7) << etaPerTrk 
			<< " || " << std::setw(9) << tetaPerTrk << " || "       << std::setw(7) << phiPerTrk << " || "  << std::setw(7) << scatPerTrk
			<< " || " << std::setw(7) << holePerTrk << " || "       << std::setw(9) << chi2PerTrk << " || " <<  std::setw(11) 
			<< "NOT DEFINED" << " || " << std::endl;
			sout << "-----------------------------------------------------------------------------------------------------------------------------------" << std::endl;
			sout << std::endl << std::endl;
		}else if (trksPerTrtrk < 0 && !m_doTruth){
			sout.precision(4);
			sout << std::endl;
			sout << ">>>> MuonTrackStatisticsAlg Summary: Track Container = " << (*counter_it)->trackLocation  << std::endl;
			sout << "--------------------------------------------------------------------------------------------------------------------" << std::endl;
			sout << "|| Events  || Tracks  || Trk/Evt || Hit/Trk || Eta/Trk ||"
			<< " TrigEta/T || Phi/Trk || Scat/Tk || Hole/Tk || Ch2/dof/T ||" << std::endl;
			
			sout << "|| " << std::setw(7) << (*counter_it)->nEvents << " || "         << std::setw(7) << (*counter_it)->nTracks
			<< " || " << std::setw(7) << trksPerEvent  << " || "    << std::setw(7) << hitsPerTrk  << " || " << std::setw(7) << etaPerTrk 
			<< " || " << std::setw(9) << tetaPerTrk << " || "       << std::setw(7) << phiPerTrk << " || "  << std::setw(7) << scatPerTrk
			<< " || " << std::setw(7) << holePerTrk << " || "       << std::setw(9) << chi2PerTrk << " || " << std::endl;
			sout << "--------------------------------------------------------------------------------------------------------------------" << std::endl;
			sout << std::endl << std::endl;
		}else {		
			sout.precision(4);
			sout << std::endl;
			sout << ">>>> MuonTrackStatisticsAlg Summary: Track Container = " << (*counter_it)->trackLocation  << std::endl;
			sout << "-----------------------------------------------------------------------------------------------------------------------------------" << std::endl;
			sout << "|| Events  || Tracks  || Trk/Evt || Hit/Trk || Eta/Trk ||"
			<< " TrigEta/T || Phi/Trk || Scat/Tk || Hole/Tk || Ch2/dof/T || Trks/TruthT ||" << std::endl;
			
			sout << "|| " << std::setw(7) << (*counter_it)->nEvents << " || "         << std::setw(7) << (*counter_it)->nTracks
			<< " || " << std::setw(7) << trksPerEvent  << " || "    << std::setw(7) << hitsPerTrk  << " || " << std::setw(7) << etaPerTrk 
			<< " || " << std::setw(9) << tetaPerTrk << " || "       << std::setw(7) << phiPerTrk << " || "  << std::setw(7) << scatPerTrk
			<< " || " << std::setw(7) << holePerTrk << " || "       << std::setw(9) << chi2PerTrk << " || " <<  std::setw(11) 
			<< trksPerTrtrk << " || " << std::endl;
			sout << "-----------------------------------------------------------------------------------------------------------------------------------" << std::endl;
			sout << std::endl << std::endl;
		}
	}
	
	
	if(m_doTruth){
		
		for (truthcounter_it = truthcounter_it_start; truthcounter_it!=truthcounter_itEnd ; truthcounter_it++) {			double TruthTrksPerEvent;
			double PIXELhitsPerTrk  ;
			double SCThitsPerTrk  ;
			double TRThitsPerTrk  ;
			double MDThitsPerTrk  ;
			double RPChitsPerTrk  ;
			double TGChitsPerTrk  ;
			double CSChitsPerTrk  ;
			
			
			sout << std::endl;
			sout << ">>>> MuonTrackStatisticsAlg Summary: Track Container = " << (*truthcounter_it)->trackLocation  << std::endl;
			for (unsigned int i = 0; i < 3; i++){
				if ((*truthcounter_it)->nEvents != 0 && (*truthcounter_it)->nTracks != 0){
					TruthTrksPerEvent = (double) (*truthcounter_it)->nTracks / (*truthcounter_it)->nEvents;
					PIXELhitsPerTrk  = (double) (*truthcounter_it)->nPIXELhits[i] / (*truthcounter_it)->nTracks;
					SCThitsPerTrk  = (double) (*truthcounter_it)->nSCThits[i] / (*truthcounter_it)->nTracks;
					TRThitsPerTrk  = (double) (*truthcounter_it)->nTRThits[i] / (*truthcounter_it)->nTracks;
					MDThitsPerTrk  = (double) (*truthcounter_it)->nMDThits[i] / (*truthcounter_it)->nTracks;
					RPChitsPerTrk  = (double) (*truthcounter_it)->nRPChits[i] / (*truthcounter_it)->nTracks;
					TGChitsPerTrk  = (double) (*truthcounter_it)->nTGChits[i] / (*truthcounter_it)->nTracks;
					CSChitsPerTrk  = (double) (*truthcounter_it)->nCSChits[i] / (*truthcounter_it)->nTracks;
					
				} else {
					TruthTrksPerEvent = 0;
					PIXELhitsPerTrk = 0; 
					SCThitsPerTrk = 0;
					TRThitsPerTrk = 0;
					MDThitsPerTrk = 0;
					RPChitsPerTrk = 0;
					TGChitsPerTrk = 0;
					CSChitsPerTrk = 0; 
				}
				
				
				sout.precision(4);
				sout << "----------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
				if (i == 0) sout << "-------------------------------------------------------->>>> SubDetStat is COMMON <<<<--------------------------------------------------------" << std::endl;
				else if (i == 1) sout << "-------------------------------------------------------->>>> SubDetStat is ONTRUTH <<<<-------------------------------------------------------" << std::endl;
				else if (i == 2) sout << "-------------------------------------------------------->>>> SubDetStat is ONTRACK <<<<-------------------------------------------------------" << std::endl;
				sout << "----------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
				sout << "|| Events  || Tracks  || Trk/Evt || PIXELhits/Trk || SCThits/Trk ||"
				<< " TRThits/Trk || MDThits/Trk || RPChits/Trk || TGChits/Trk || CSChits/Trk ||" << std::endl;
				
				sout << "|| " << std::setw(7) << (*truthcounter_it)->nEvents << " || "         << std::setw(7) << (*truthcounter_it)->nTracks
				<< " || " << std::setw(7) << TruthTrksPerEvent  << " || "    << std::setw(13) << PIXELhitsPerTrk  << " || " << std::setw(11) << SCThitsPerTrk 
				<< " || " << std::setw(11) << TRThitsPerTrk << " || "       << std::setw(11) << MDThitsPerTrk << " || "  << std::setw(11) << RPChitsPerTrk
				<< " || " << std::setw(11) << TGChitsPerTrk << " || "       << std::setw(11) << CSChitsPerTrk << " || " << std::endl;
				
			}
			
			sout << "----------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
			sout << std::endl << std::endl;
		}
		
		
    }		
	return sout.str();
}





void
MuonTrackStatisticsTool::storeTruthTracks(void)
{
	
	
	
	
	/*
	 const McEventCollection* mcCollection = 0;
	 std::string key = "TruthEvent";
	 StatusCode sc = p_SGevent->retrieve(mcCollection,key);
	 if (sc.isFailure()) {
	 *m_log << MSG::ERROR << "Could not find the McEventCollection" << endreq;
	 return;
	 }
	 
	 const TrackRecordCollection* recordCollection = 0;
	 std::string recordKey = "MuonEntryLayer";
	 if (!(p_SGevent->retrieve(recordCollection, recordKey))) {
	 *m_log << MSG::WARNING << "Could not find the TrackRecordCollection" << endreq;
	 }
	 
	 m_nkine = 0;
	 
	 const HepMC::GenEvent* event = *mcCollection->begin();   
	 HepMC::GenEvent::particle_const_iterator particle = event->particles_begin();
	 HepMC::GenEvent::particle_const_iterator particle_end = event->particles_end();
	 
	 for ( ; particle != particle_end; ++particle) {	
	 
	 // select final-state muons above min calo penetration energy 
	 // and in MS acceptance
	 if (abs((*particle)->pdg_id()) != 13) continue;
	 if ((*particle)->status() != 1) continue;
	 if ((*particle)->momentum().e() < m_minProductionEnergy) continue;	    
	 if (fabs((*particle)->momentum().pseudoRapidity()) > m_maxEtaMS) continue;
	 //    if ((*particle)->barcode() > 10000) continue;
	 
	 const Trk::TrackParameters* genPerigee = m_truthToTrack->makePerigeeParameters(*particle);
	 if( !genPerigee ) continue;
	 
	 const Trk::TrackParameters* genEntry = m_helperTool->extrapolateToMuonEntryRecord(*genPerigee,Trk::muon);
	 if( !genEntry ){
	 delete genPerigee;
	 continue;
	 }
	 HepPoint3D  gen_position = genPerigee->position();
	 HepVector3D	gen_momentum = genPerigee->momentum();
	 double      gen_charge   = (*particle)->pdg_id() < 0 ? 1. : -1.;
	 
	 HepPoint3D  extr_position = genEntry->position();
	 HepVector3D	extr_momentum = genEntry->momentum();
	 double      extr_charge   = (*particle)->pdg_id() < 0 ? 1. : -1.;
	 
	 ++m_nkine;
	 
	 // store the muon generated parameters
	 m_xvtxg->push_back((*particle)->production_vertex()->point3d().x());
	 m_yvtxg->push_back((*particle)->production_vertex()->point3d().y());
	 m_zvtxg->push_back((*particle)->production_vertex()->point3d().z());
	 m_a0g->push_back(genPerigee->parameters()[Trk::d0]);
	 m_z0g->push_back(genPerigee->parameters()[Trk::z0]);
	 m_phig->push_back((*particle)->momentum().phi());
	 m_thetag->push_back((*particle)->momentum().theta());
	 m_qpig->push_back(gen_charge/(*particle)->momentum().mag());
	 m_etag->push_back((*particle)->momentum().pseudoRapidity());
	 m_barcode->push_back((*particle)->barcode());
	 m_status->push_back((*particle)->status());
	 
	 
	 
	 */
	
}

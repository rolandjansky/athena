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
	
	StatusCode sc = m_helperTool.retrieve();
	if (sc.isSuccess()){
		ATH_MSG_DEBUG( "Retrieved " << m_helperTool);
	}else{
		ATH_MSG_FATAL("Could not get " << m_helperTool);
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
	
	return StatusCode::SUCCESS;
}

StatusCode MuonTrackStatisticsTool::updateTruthTrackCounters(std::string name, const DetailedTrackTruthCollection* truthMap) const{
  std::vector<MuonTrackStatisticsTool::TruthTrackCounters*>::iterator counterTruth_it = m_allTruthCounters.begin();
  std::vector<MuonTrackStatisticsTool::TruthTrackCounters*>::iterator counterTruth_itEnd = m_allTruthCounters.end();
  for (; counterTruth_it!=counterTruth_itEnd ; counterTruth_it++){
    if((*counterTruth_it)->trackLocation.compare(name)==0){
      return updateTruthTrackCounters(**counterTruth_it,*truthMap);
    }
  }
  ATH_MSG_WARNING("Failed to match the collection "<<name<<" to any counter");
  return StatusCode::SUCCESS;
}

StatusCode MuonTrackStatisticsTool::updateTruthTrackCounters(TruthTrackCounters& counters,  const DetailedTrackTruthCollection& TruthMap) const{
	
	ATH_MSG_DEBUG( "MuonTrackStatisticsTool calling updateTruthTrackCounters: " << counters.trackLocation);
	
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
		
		
		ATH_MSG_DEBUG( myindex << ".) " << "Index: "  <<  (*it).first.index() << "		" << (*it).second << " (Pixel, SCT, TRT, MDT, RPC, TGC, CSC) ");
		ATH_MSG_DEBUG( "	GenParticle info:" );
		for( unsigned int i=0; i< (*it).second.trajectory().size(); i++) {
		  ATH_MSG_DEBUG("		Particle "<<i);
			if( !(*it).second.trajectory().at(i).cptr() ) {
			  ATH_MSG_DEBUG( " has a null pointer: "<< (*it).second.trajectory().at(i).cptr());
			}else{
			  ATH_MSG_DEBUG( "			- pdg_id: "<< (*it).second.trajectory().at(i).cptr()->pdg_id());
			  ATH_MSG_DEBUG( "			- status: "<< (*it).second.trajectory().at(i).cptr()->status());
			  counters.nTracks++;
			}
			
		}
		myindex++;
	}		
	
	
	return sc;
	
}


StatusCode MuonTrackStatisticsTool::updateTrackCounters(std::string name, const TrackCollection* tracks) const{
  std::vector<MuonTrackStatisticsTool::TrackCounters*>::iterator counter_it = m_allCounters.begin();
  std::vector<MuonTrackStatisticsTool::TrackCounters*>::iterator counter_itEnd = m_allCounters.end();
  for (; counter_it!=counter_itEnd ; counter_it++){
    if((*counter_it)->trackLocation.compare(name)==0){
      return updateTrackCounters(**counter_it,*tracks);
    }
  }
  ATH_MSG_WARNING("Failed to match the collection "<<name<<" to any counter");
  return StatusCode::SUCCESS;
}

StatusCode MuonTrackStatisticsTool::updateTrackCounters(TrackCounters& counters,  const TrackCollection& tracks) const{
	
	ATH_MSG_DEBUG("MuonTrackStatisticsTool calling updateTrackCounters: " << counters.trackLocation);
	
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
		ATH_MSG_INFO("MuonTrackStatisticsTool calling addTrackCounters for truth: " << trkLoc);
		TruthTrackCounters * counters;
		counters = new TruthTrackCounters (trkLoc);
		m_allTruthCounters.push_back(counters);
	}else if( !temp_string.Contains("Truth")) {
		
		ATH_MSG_INFO( "MuonTrackStatisticsTool calling addTrackCounters for reco: " << trkLoc);
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
					ATH_MSG_INFO( "MuonTrackStatisticsTool - Found matching TruthCollection for: " << (*counter_it)->trackLocation);
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
			ATH_MSG_INFO( "MuonTrackStatisticsTool - Could not find matching TruthCollection for: " << (*counter_it)->trackLocation);
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
	 StatusCode sc = evtStore()->retrieve(mcCollection,key);
	 if (sc.isFailure()) {
	 *m_log << MSG::ERROR << "Could not find the McEventCollection" << endmsg;
	 return;
	 }
	 
	 const TrackRecordCollection* recordCollection = 0;
	 std::string recordKey = "MuonEntryLayer";
	 if (!(evtStore()->retrieve(recordCollection, recordKey))) {
	 *m_log << MSG::WARNING << "Could not find the TrackRecordCollection" << endmsg;
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

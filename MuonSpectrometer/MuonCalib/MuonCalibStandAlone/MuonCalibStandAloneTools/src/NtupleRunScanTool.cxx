/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//c - c++
#include "iostream"
#include "fstream"
#include "sstream"
#include "set"
#include <string.h>

//root
#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"

//MuonCalibEventBase
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MuonCalibEventInfo.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"

// MuonReadoutGeometry //
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"


//geo model
#include "MuonIdHelpers/MdtIdHelper.h"

//MuonCalibIdentifier
#include "MuonCalibIdentifier/MuonFixedId.h"


//this
#include "MuonCalibStandAloneTools/NtupleRunScanTool.h"


namespace MuonCalib{

NtupleRunScanTool::NtupleRunScanTool(const std::string& t, const std::string& n, const IInterface* p): AthAlgTool(t, n, p), m_min_hits(2000), m_max_bad_fits(0.1), m_suppress_nofit(false), m_noisy_tube_factor(1), m_time_min(std::numeric_limits<unsigned int>::max()), m_time_max(0), m_run_min(std::numeric_limits<unsigned int>::max()), m_run_max(0) 
	{
	declareInterface< NtupleCalibrationTool >(this);
	declareProperty("MinHits", m_min_hits);
	declareProperty("MaxBadFits", m_max_bad_fits);
	declareProperty("SuppressNofit", m_suppress_nofit);
	declareProperty("NoisyTubeFactor", m_noisy_tube_factor);
	}


StatusCode NtupleRunScanTool::initialize()
	{
	if(m_max_bad_fits<0 || m_max_bad_fits>1)
		{
		ATH_MSG_FATAL( "MaxBadFits must be between 0 and 1!" );
		return StatusCode::FAILURE;
		}
//get geometry
	//retrieve mdt id helper
		ATH_CHECK( detStore()->retrieve(m_mdtIdHelper, "MDTIDHELPER" ) );
	//retrieve detector manager
		ATH_CHECK( detStore()->retrieve( m_detMgr ) );
	p_outfile = new TFile("RunScan.root", "RECREATE");
	return StatusCode :: SUCCESS;		
	}


StatusCode NtupleRunScanTool::handleEvent(const MuonCalibEvent &event, int /*evnt_nr*/, const std::vector<MuonCalibSegment *> &segments, unsigned int position)
	{
//iov
	const MuonCalibEventInfo & eventinfo(event.eventInfo());
	if(eventinfo.timeStamp()<m_time_min)
		{
		m_time_min=eventinfo.timeStamp();
		}
	if(eventinfo.timeStamp()>m_time_max)
		{
		m_time_max=eventinfo.timeStamp();
		}
	if(eventinfo.runNumber()<m_run_min)
		{
		m_run_min=eventinfo.runNumber();
		}
	if(eventinfo.runNumber()>m_run_max)
		{
		m_run_max=eventinfo.runNumber();
		}
	for(unsigned int i=position; i<segments.size(); i++)
		{
		std::set<NtupleStationId> hit_stations;
		MuonCalibSegment :: MdtHitCit it(segments[i]->mdtHOTBegin());
		for(; it!=segments[i]->mdtHOTEnd(); it++)
			{
			NtupleStationId id((*(segments[i]->mdtHOTBegin()))->identify());
			id.SetMultilayer(0);
			hit_stations.insert(id);
			HitCounter &counter(m_hit_counters[id]);
			if(!counter.IsInitialized())
				{
				if(!id.InitializeGeometry(m_mdtIdHelper, m_detMgr))
					{
					ATH_MSG_FATAL( "Cannot initialize Geometry!" );
					return StatusCode::FAILURE;
					}
				p_outfile->cd();
				counter.Initialize(id);
				}
			}
		for(std::set<NtupleStationId> :: iterator it=hit_stations.begin(); it!=hit_stations.end(); it++)	
			m_hit_counters[*it].ProcessSegment(*segments[i]);
		}
	return StatusCode::SUCCESS;
	}


StatusCode NtupleRunScanTool::analyseSegments(const std::vector<MuonCalibSegment *> & /*segemnts*/)
	{
	std::ofstream header_insert_sql("header_insert.sql");
	header_insert_sql<<"insert into MDT_HEAD (implementation, lowrun, uprun, luminosity, rootfile, lowtime, uptime) values ('xX_IMPLEMENTATION_Xx', "<<m_run_min<<", "<< m_run_max <<", 1, 'xX_ROOT_FILE_Xx', "<< m_time_min <<", "<< m_time_max <<");"<<std::endl;
	p_outfile->cd();
	ATH_MSG_INFO( "Location: "<<gDirectory->GetName() );
	TTree *statistics_tree=new TTree("statistics_tree", "statistics tree");
	MuonFixedId id;
//statistics tree
	Int_t station, eta, phi;
	char station_str[100];
	Int_t n_hits, fit_by;
	char fit_by_str[100];
	Int_t n_dead_tubes, n_dead_mezz, n_dead_ml;
	char software_id[100], hardware_id[100];
	statistics_tree->Branch("station", &station, "station/I");
	statistics_tree->Branch("eta", &eta, "eta/I");
	statistics_tree->Branch("phi", &phi, "phi/I");
	statistics_tree->Branch("n_hits", &n_hits, "n_hits/I");
	statistics_tree->Branch("fit_by", &fit_by, "fit_by/I");
	statistics_tree->Branch("station_str", &station_str, "station_str/C");
	statistics_tree->Branch("fit_by_str", &fit_by_str, "fit_by_str/C");
	statistics_tree->Branch("n_dead_tubes", &n_dead_tubes, "n_dead_tubes/I");
	statistics_tree->Branch("n_dead_mezz", &n_dead_mezz, "n_dead_mezz/I");
	statistics_tree->Branch("n_dead_ml", &n_dead_ml, "n_dead_ml/I");
	statistics_tree->Branch("software_id", &software_id, "software_id/C");
	statistics_tree->Branch("hardware_id", &hardware_id, "hardware_id/C");
	std::ofstream joblist("joblist.txt");
	std::ofstream initial_dq_file("initial_dq.txt");
	joblist<<"xX_STATION_Xx xX_ETA_Xx xX_PHI_Xx xX_SORT_BY_Xx"<<std::endl;
	for(std::map<NtupleStationId, HitCounter> :: iterator it=m_hit_counters.begin(); it!= m_hit_counters.end(); it++)
		{
		p_outfile->cd();
		const std::string &output(it->second.FittingBy(m_min_hits, m_max_bad_fits));
		std::map<HitCounter::TubeId, HitCounter::DQInfo> dqinfo(it->second.InitialDQ(m_noisy_tube_factor));
		for(std::map<HitCounter::TubeId, HitCounter::DQInfo> :: const_iterator jt=dqinfo.begin(); jt!=dqinfo.end(); jt++)
			{
			if(jt->second.GetSeverity()>0)
				{
				initial_dq_file<<it->first.regionId()<<" "<<
				jt->first.ml+1<<" "<<jt->first.ly+1<<" "<<jt->first.tb+1<<" "<<jt->second.GetSeverity()<<" "<<jt->second.GetNoisy()<<" "<<jt->second.GetDead()<<std::endl;
				}
			}
		if(!m_suppress_nofit || output!="NOFIT")
			joblist << id.stationNumberToFixedStationString(it->first.GetStation()) << " " << it->first.GetEta() << " " << it->first.GetPhi() << " " << output<<std::endl;
	//fill tree
		station = it->first.GetStation();
		strncpy(station_str, id.stationNumberToFixedStationString(it->first.GetStation()).c_str(), 99);
		strncpy(fit_by_str, output.c_str(), 99);
		eta = it->first.GetEta();
		phi = it->first.GetPhi();
		fit_by = it->second.FitByInt();
		n_hits = it->second.NHits();
		n_dead_tubes = it->second.NDeadTubes();
		n_dead_mezz = it->second.NDeadMezzanines();
		n_dead_ml =  it->second.NDeadMultilayers();
		std::ostringstream sw_id;
		std::ostringstream hw_id;
		hw_id<<station_str<<abs(eta);
		if(eta>0)
			hw_id<<"A";
		else
			hw_id<<"C";
		int hw_phi=phi*2;
		if(station_str[2]=='L')
			hw_phi-=1;
		if(hw_phi<10)
			hw_id<<0;
		hw_id<<hw_phi;
		sw_id<<station_str<<" #eta"<<eta<<" #phi"<<phi;
		strncpy(software_id, sw_id.str().c_str(), 99);
		strncpy(hardware_id, hw_id.str().c_str(), 99);
		statistics_tree->Fill();
		}
	p_outfile->cd();
	TNtuple *ntup=new TNtuple("run_range", "Range of the runs", "min:max");
	ntup->Fill(m_time_min, m_time_max);
	ntup->Fill(m_run_min, m_run_max);
	p_outfile->Write();
	return StatusCode::SUCCESS;
	}
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// standard C++ //
#include <iostream>
#include <fstream>

//root
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"

// MuonCalibEventBase //
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MdtCalibHitBase.h"

//this
	#include "MuonCalibStandAloneTools/NtupleWireCentricityTool.h"


//MdtCalibT0
#include "MdtCalibT0/T0MTHistos.h"
#include "MdtCalibT0/T0MTSettings.h"


//MdtCalibIOSvc
#include "MdtCalibIOSvc/MdtCalibInputSvc.h"

//MdtcalibData
#include "MdtCalibData/IRtRelation.h"


namespace MuonCalib {

/////////////////
// Constructor //
/////////////////

NtupleWireCentricityTool :: NtupleWireCentricityTool(const std::string& t, const std::string& n, const IInterface* p) : AthAlgTool(t, n, p), m_min_track_slope(0.6), m_region_width(8e8), m_calib_input_svc("MdtCalibInputSvc", n)
	{
	declareInterface< NtupleCalibrationTool >(this) ;
	declareProperty("MinTrackSlope", m_min_track_slope);
	declareProperty("RegionWidth", m_region_width);
        declareProperty("MdtCalibInputSvc", m_calib_input_svc);
	}


////////////////
// initialize //
////////////////

StatusCode NtupleWireCentricityTool :: initialize()
	{
	ATH_MSG_INFO( "Initializing NtupleWireCentricityTool" );

	
//create root file and histograms
	p_root_file = new TFile("WireCentricity.root", "RECREATE");
	p_dt_spec_left = new TH1F("p_dt_spec_left", "", 1701, -200.5 * (25.0 / 32.0), 1500.5 * (25.0 / 32.0));
	p_dt_spec_right = new TH1F("p_dt_spec_right", "", 1701, -200.5 * (25.0 / 32.0), 1500.5 * (25.0 / 32.0));
	p_dt_spec_above = new TH1F("p_dt_spec_above", "", 1701, -200.5 * (25.0 / 32.0), 1500.5 * (25.0 / 32.0));
	p_dt_spec_below = new TH1F("p_dt_spec_below", "", 1701, -200.5 * (25.0 / 32.0), 1500.5 * (25.0 / 32.0));
	m_track_radius = new TH1F("track_radius", "", 200, -16.0, 16.0);
	p_track_slope_cut = new TH1F("track_slope_cut", "", 1000, 0, 3);
	p_track_slope = new TH1F("track_slope", "", 1000, -3, 3);
	m_x_coordinate = new TH1F("x_coordinate", "", 4000, -2000, 2000);
	
	ATH_MSG_INFO( "tool initialized." );
//get calibration input service
	ATH_CHECK( m_calib_input_svc.retrieve() );
	setRegion();
	return StatusCode::SUCCESS;
	}


/////////////////
// handleEvent //
/////////////////

StatusCode NtupleWireCentricityTool :: handleEvent(const MuonCalibEvent &/*event*/, int /*evnt_nr*/, const std::vector<MuonCalibSegment *> &segments, unsigned int position)
	{
	for(unsigned int i=position; i<segments.size(); i++)
		{
	//chaeck for minimal track inclination
	// !!not valid for end-cap chambers!!
		const MuonCalibSegment *seg(segments[i]);
		double track_slope(seg->direction().y() / seg->direction().z());
		if(std::isnan(track_slope)) continue;
		p_track_slope->Fill(track_slope);
		
	//loop on all track hits
		for(MuonCalibSegment::MdtHitCit it = seg->mdtHOTBegin(); it!=seg->mdtHOTEnd(); it++)
			{
			const MdtCalibHitBase *hit=(*it);
			process_hit(hit, track_slope);
			}
		for(MuonCalibSegment::MdtHitCit it = seg->mdtCloseHitsBegin(); it!=seg->mdtCloseHitsEnd(); it++)
			{
			const MdtCalibHitBase *hit=(*it);
			process_hit(hit, track_slope);
			}
		}
	return StatusCode::SUCCESS;
	}


////////////////////
//analyseSegments //
////////////////////

StatusCode NtupleWireCentricityTool :: analyseSegments(const std::vector<MuonCalibSegment *> & /*segemnts*/)
	{
	T0MTHistos histos;
	T0MTSettings settings;
	settings.AddFitfun()=true;
	settings.DrawDebugGraphs()=true;
	std::ofstream summary_file;
//create summary tree
	TTree *summary_tree=new TTree("summary_tree", "");
	summary_tree->Branch("station", &m_station, "station/I");
	summary_tree->Branch("eta", &m_eta, "eta/I");
	summary_tree->Branch("phi", &m_phi, "phi/I");
	summary_tree->Branch("hw_phi", &m_hw_phi, "hw_phi/I");
	summary_tree->Branch("mean_track_slope", &m_mean_track_slope, "mean_track_slope/D");
	summary_tree->Branch("n_hits", &m_n_hits, "n_hits/I");
	summary_tree->Branch("drift_velocity", &m_drift_velocity, "drift_velocity/D");
	Double_t l_above(9e9), l_below(9e9);
	summary_tree->Branch("l_above", &l_above, "l_above/D");
	summary_tree->Branch("l_below", &l_below, "l_below/D");
	
	Double_t mean_x(m_x_coordinate->GetMean());
	Double_t all_mean_slope(p_track_slope->GetMean());
	summary_tree->Branch("mean_x", &mean_x, "mean_x/D");
	summary_tree->Branch("all_mean_slope", &all_mean_slope, "all_mean_slope/D");
	Int_t n_hits_above(static_cast<Int_t>(p_dt_spec_above->GetEntries())), n_hits_below(static_cast<Int_t>(p_dt_spec_below->GetEntries()));
	summary_tree->Branch("n_hits_above", &n_hits_above, "n_hits_above/I");
	summary_tree->Branch("n_hits_below", &n_hits_below, "n_hits_below/I");
	
	Double_t l_left(9e9), l_right(9e9);
	summary_tree->Branch("l_left", &l_left, "l_left/D");
	summary_tree->Branch("l_right", &l_right, "l_right/D");
	
	m_n_hits = static_cast<Int_t>(p_track_slope_cut->GetEntries());
	m_mean_track_slope = p_track_slope_cut->GetMean();
	histos.SetTSpec(0, p_dt_spec_left, &settings);
	if(histos.FitT0())
		{
		if(histos.FitTmax())	
			l_left=histos.GetTMaxFunction()-> GetParameter(T0MTHistos :: TMAX_PAR_NR_TMAX) - histos.GetT0Function()->GetParameter(T0MTHistos :: T0_PAR_NR_T0);	
		}
	histos.SetTSpec(1, p_dt_spec_right, &settings);
	if(histos.FitT0())
		{
		if(histos.FitTmax())	
			l_right=histos.GetTMaxFunction()-> GetParameter(T0MTHistos :: TMAX_PAR_NR_TMAX) - histos.GetT0Function()->GetParameter(T0MTHistos :: T0_PAR_NR_T0);
		}
	histos.SetTSpec(2, p_dt_spec_below, &settings);
	if(histos.FitT0())
		{
		if(histos.FitTmax())	
			l_above=histos.GetTMaxFunction()-> GetParameter(T0MTHistos :: TMAX_PAR_NR_TMAX) - histos.GetT0Function()->GetParameter(T0MTHistos :: T0_PAR_NR_T0);
		}
	histos.SetTSpec(3, p_dt_spec_above, &settings);
	if(histos.FitT0())
		{
		if(histos.FitTmax())	
			l_below=histos.GetTMaxFunction()-> GetParameter(T0MTHistos :: TMAX_PAR_NR_TMAX) - histos.GetT0Function()->GetParameter(T0MTHistos :: T0_PAR_NR_T0);
		}
	ATH_MSG_INFO( "Summary:" );
	ATH_MSG_INFO( "left/right: "<<l_left - l_right <<" ns " << (l_left - l_right)*m_drift_velocity << "mm: slope is "<<p_track_slope->GetMean() );
	ATH_MSG_INFO( "above/below: "<<l_above - l_below <<" ns " << (l_above - l_below)*m_drift_velocity << "mm: slope is "<<m_mean_track_slope );
	summary_tree->Fill();		
	p_root_file->Write();
	
	return StatusCode::SUCCESS;
	}

void NtupleWireCentricityTool :: setRegion()
	{
	const IRtRelation *rtrel=m_calib_input_svc->GetRtRelation();
	if(rtrel==NULL) return;
	m_drift_velocity = rtrel->driftvelocity(rtrel->tUpper() - 50);
	ATH_MSG_INFO( "Drift velocity is "<<m_drift_velocity );
	}

inline void NtupleWireCentricityTool :: process_hit(const MdtCalibHitBase * hit, const double & track_slope)
	{
	double r_track=hit->signedDistanceToTrack();
	m_track_radius->Fill(r_track);
	m_x_coordinate->Fill(hit->localPosition().x());
	if(fabs(hit->localPosition().x())>m_region_width/2) return;
	//fill right left histograms
	if(r_track<0)
		p_dt_spec_left->Fill(static_cast<Axis_t>(hit-> driftTime()));
	else
		p_dt_spec_right->Fill(static_cast<Axis_t>(hit-> driftTime()));
	//check for track slope
	if(fabs(track_slope) < m_min_track_slope) return;
	if((r_track < 0 && track_slope < 0) || (r_track > 0 && track_slope > 0))
		{
		p_dt_spec_below->Fill(static_cast<Axis_t>(hit-> driftTime()));
		}
	else
		{
		p_dt_spec_above->Fill(static_cast<Axis_t>(hit-> driftTime()));	
		}
	p_track_slope_cut->Fill(fabs(track_slope));	
	m_station=hit->identify().stationName();
	m_eta=hit->identify().eta();
	m_phi=hit->identify().phi();
	m_hw_phi=m_phi * 2 - (m_station % 2);	
	}
}

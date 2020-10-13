/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 20.06.2007, AUTHOR: OLIVER KORTNER
// Modified: 26.06.2007 by O. Kortner, new naming convention for directories:
//                                     station-phi-eta
//           26.06.2007 by O. Kortner, new histograms added.
//           19.07.2007 by O. Kortner, no t0 refinement without r-t calibration.
//           18.08.2008 by O. Kortner, curved track fit + time-out option added.
//           03.11.2008 by O. Kortner, allow the user to turn off the t0
//                                     refinement histograms.
//           21.02.2009 by O. Kortner, additional residual histograms
//                                     after t0 refinement.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF THE METHODS DEFINED IN THE CLASS ::
//::           NtupleControlHistogramsTool              ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// standard C++ //
#include <iostream>
#include <fstream>

// CLHEP //
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

// MuonReadoutGeometry //
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
// MuonCalib //
#include "MuonCalibStandAloneTools/NtupleControlHistogramsTool.h"
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MuonCalibRawRpcHit.h"
#include "MuonCalibStl/ToString.h"
#include "MuonCalibStandAloneBase/MdtStationT0Container.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MdtCalibT0/T0MTHistos.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"
#include "MdtCalibIOSvc/MdtCalibInputSvc.h"
//root;
#include "TF1.h"
#include "TTree.h"
#include "TProfile.h"

//::::::::::::::::::::::::
//:: NAMESPACE SETTINGS ::
//::::::::::::::::::::::::

using namespace std;
using namespace MuonCalib;

//*****************************************************************************

//:::::::::::::::::
//:: CONSTRUCTOR ::
//:::::::::::::::::

NtupleControlHistogramsTool::NtupleControlHistogramsTool(const std::string & t, 
	const std::string & n, const IInterface * p) : AthAlgTool(t, n, p),
	m_reg_sel_svc("RegionSelectionSvc", n), m_input_service("MdtCalibInputSvc",n) {

/////////////////
// JOB OPTIONS //
/////////////////
	declareProperty("RegionSelectionSvc", m_reg_sel_svc);
	declareProperty("MdtCalibInputSvc", m_input_service);

	m_refit_segments = false; // by default refit the segments
    declareProperty("segmentRefit", m_refit_segments);

	m_curved_fits = false; // by default straight refits of the segments
    declareProperty("curvedSegmentRefit", m_curved_fits);

	m_refine_t0 = false; // by default t0 refinement plots are made
    declareProperty("t0RefinementPlots", m_refine_t0);

	m_time_out = 2.0; // by default 2 s time-out for pattern finding
    declareProperty("timeOut", m_time_out);

	m_road_width = 1.0; // by default road width = 1 mm
	declareProperty("roadWidth", m_road_width);

	m_MDT_ID_helper = string("MDTIDHELPER");
	declareProperty("MDTIdHelper", m_MDT_ID_helper);

	m_RPC_ID_helper = string("RPCIDHELPER");
	declareProperty("RPCIdHelper", m_RPC_ID_helper);

	m_idToFixedIdToolType = string("MuonCalib::IdToFixedIdTool");
	declareProperty("idToFixedIdToolType", m_idToFixedIdToolType);

	m_idToFixedIdToolName = string("MuonCalib_IdToFixedIdTool");
	declareProperty("idToFixedIdToolName", m_idToFixedIdToolName);

	m_ROOT_file_name = string("NtupleControlHistogramsTool.root");
	declareProperty("ROOTFileName", m_ROOT_file_name);

/////////////////////////////
// RESET PRIVATE VARIABLES //
/////////////////////////////

	m_MDT_raw_nb_hits.clear();
	m_MDT_raw_hit_distribution.clear();
	m_MDT_raw_TDC.clear();
	m_MDT_raw_ADC.clear();
	m_t0_diff.clear();
	m_r_mintrt.clear();
	m_MDT_segment_hits.clear();
	m_MDT_segment_hist_to_raw_hits_ratio.clear();
	m_MDT_segment_CL.clear();
	m_MDT_segment_t.clear();
	m_MDT_segment_r.clear();
	m_MDT_segment_TDC.clear();
	m_MDT_segment_ADC.clear();
	m_MDT_segment_slope.clear();
	m_MDT_residuals.clear();
	m_MDT_residuals_vs_x.clear();
	m_MDT_residuals_ml1.clear();
	m_MDT_residuals_ml2.clear();
	m_MDT_t0_refinement.clear();
	m_MDT_t_refined.clear();
	m_MDT_residuals_t0_refined.clear();
	m_MDT_residuals_t0_refined_gt4hits.clear();
    m_MDT_RPC_raw_tubeStripCorVsEvent.clear();
	
	m_nb_ml.clear();
	m_nb_ly.clear();
	m_nb_tb.clear();
	m_nb_totalEtaStrips.clear();
	m_nb_totalEtaStrips2.clear();
	m_nb_raw_MDT_hits.clear();
	m_nb_events = 0;
	m_MdtIdHelper = 0;
	m_RpcIdHelper = 0;
	m_detMgr = 0;
	m_id_tool = 0;
	
	m_qfitter = new StraightPatRec();
	m_cfitter = new CurvedPatRec();
	m_t0_refinement = new T0Refinement();

	declareInterface< NtupleCalibrationTool >(this);	

}

//*****************************************************************************

//::::::::::::::::
//:: DESTRUCTOR ::
//::::::::::::::::

NtupleControlHistogramsTool::~NtupleControlHistogramsTool(void) {

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD initialize ::
//:::::::::::::::::::::::

StatusCode NtupleControlHistogramsTool::initialize(void) {

///////////////
// VARIABLES //
///////////////

/////////////
// MESSAGE //
/////////////

	ATH_MSG_INFO( "Initializing tool..." );

////////////////////////////////////////////////////
// STORE GATE AND GEOMODEL RELATED INITIALIZATION //
////////////////////////////////////////////////////

// MDT ID helper //
	ATH_CHECK( detStore()->retrieve(m_MdtIdHelper, m_MDT_ID_helper) );

// RPC ID helper //
	ATH_CHECK( detStore()->retrieve(m_RpcIdHelper, m_RPC_ID_helper) );

// muon detector manager //
	ATH_CHECK( detStore()->retrieve(m_detMgr) );

// muon fixed id tool //
	ATH_CHECK( toolSvc()->retrieveTool(m_idToFixedIdToolType,
					m_idToFixedIdToolName, m_id_tool) );

// calibration input service //
 	ATH_CHECK( m_input_service.retrieve() );

////////////////////////
// OPEN THE ROOT FILE //
////////////////////////

	m_tfile = new TFile(m_ROOT_file_name.c_str(), "RECREATE");

	m_t0_diff_global = new TH1F("mt_t0_diff_global", "", 201, -100.5, 100.5);
	m_r_mintrt_global = new TH1F ("r_min_rt_global", "", 101, -0.1, 0.1);
	
	m_r_minrt_vs_r = new TH2F ("m_r_minrt_vs_r", "", 100, 0.0, 15.0, 101, -0.1, 0.1);
/////////////////////////////////
// SET UP STRAIGHT-LINE FITTER //
/////////////////////////////////

	m_qfitter->setRoadWidth(m_road_width);
	m_qfitter->switchOnRefit();
//	m_qfitter->switchOffRefit();
	m_qfitter->setTimeOut(m_time_out);
	m_cfitter->setRoadWidth(m_road_width);
	m_cfitter->setTimeOut(m_time_out);

//get region selection service
	ATH_CHECK( m_reg_sel_svc.retrieve() );

	return StatusCode::SUCCESS;

}

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD finalize ::
//:::::::::::::::::::::

StatusCode NtupleControlHistogramsTool::finalize(void) {

///////////////
// VARIABLES //
///////////////


/////////////
// MESSAGE //
/////////////

	ATH_MSG_INFO( "Finalizing tool..." );
//the summary tree creation crashes if the refit option is not set -- To be fixed!
	if (!m_refit_segments) 
		{
		m_tfile->Write();

		return StatusCode::SUCCESS;		
		}
		
///////////////////////////////
// Postprocess histogramms   //
///////////////////////////////
	m_tfile->cd();
//station id
	Int_t station_eta, station_phi, station_name;
	char station_name_str[10];
	char software_id[100];
	char hardware_id[100];
//t0 tmax fit information	
	Double_t t0[3], T0[3], tmax[3], Tmax[3], chi2_t0[3], chi2_tmax[3];
	Double_t t0_err[3], T0_err[3], tmax_err[3], Tmax_err[3];
	Int_t n_hits[3], ml_nr[3];
	for(int i=0; i<3; i++)
		{
		ml_nr[i]=i;
		}
//residual information
	Double_t res_chi2, res_chi2_t0_improved;
	
	m_chamber_tree = new TTree("chamber_tree", "Summary information listed by chamber");
	m_chamber_tree->Branch("station_eta", &station_eta, "station_eta/I");
	m_chamber_tree->Branch("station_phi", &station_phi, "station_phi/I");
	m_chamber_tree->Branch("station_name", &station_name, "station_name/I");
	m_chamber_tree->Branch("station_name_str", &station_name_str, "station_name_str/C");
	m_chamber_tree->Branch("software_id", &software_id, "software_id/C");
	m_chamber_tree->Branch("hardware_id", &hardware_id, "hardware_id/C");
	m_chamber_tree->Branch("t0", &t0, "t0[3]/D");
	m_chamber_tree->Branch("t0_err", &t0_err, "t0_err[3]/D");
	m_chamber_tree->Branch("T0", &T0, "T0[3]/D");
	m_chamber_tree->Branch("T0_err", &T0_err, "T0_err[3]/D");
	m_chamber_tree->Branch("tmax", &tmax, "tmax[3]/D");
	m_chamber_tree->Branch("tmax_err", &tmax_err, "tmax_err[3]/D");
	m_chamber_tree->Branch("Tmax", &Tmax, "Tmax[3]/D");
	m_chamber_tree->Branch("Tmax_err", &Tmax_err, "Tmax_err[3]/D");
	m_chamber_tree->Branch("chi2_t0", &chi2_t0, "chi2_t0[3]/D");
	m_chamber_tree->Branch("chi2_tmax", &chi2_tmax, "chi2_tmax[3]/D");
	m_chamber_tree->Branch("ml_nr", &ml_nr, "ml_nr[3]/I");
	m_chamber_tree->Branch("n_hits", &n_hits, "n_hits[3]/I");
	m_chamber_tree->Branch("res_chi2", &res_chi2, "res_chi2/D");
	m_chamber_tree->Branch("res_chi2_t0_improved", &res_chi2_t0_improved, "res_chi2_t0_improved/D");
	MuonFixedId id;
//loop on all chambers
	for(std::map<NtupleStationId, T0MTHistos *>::iterator it = m_MDT_segment_tspec_ml[0].begin(); it!=m_MDT_segment_tspec_ml[0].end(); it++)
		{
	//chamber id
		station_eta = it->first.GetEta();
		station_phi = it->first.GetPhi();
		station_name = it->first.GetStation();
		strncpy(station_name_str, id.stationNumberToFixedStationString(it->first.GetStation()).c_str(), 9);
		ostringstream sw_id;
		ostringstream hw_id;
		hw_id<<station_name_str<<abs(station_eta);
		if(station_eta>0)
			hw_id<<"A";
		else
			hw_id<<"C";
		int hw_phi=station_phi*2;
		if(station_name_str[2]=='L')
			hw_phi-=1;
		if(hw_phi<10)
			hw_id<<0;
		hw_id<<hw_phi;
		sw_id<<station_name_str<<" #eta"<<station_eta<<" #phi"<<station_phi;
		strncpy(software_id, sw_id.str().c_str(), 99);
		strncpy(hardware_id, hw_id.str().c_str(), 99);
		
	//specra fit
		for(int i=0; i<3; i++) {
		T0MTHistos *histo=m_MDT_segment_tspec_ml[i][it->first];
		n_hits[i] = static_cast<Int_t>(histo->GetTSpec()->GetEntries());
		if(n_hits[i]<2000)
			{
			t0[i]=9e9;
			t0_err[i]=9e9;
			T0[i]=9e9;
			T0_err[i]=9e9;
			tmax[i]=9e9;
			tmax_err[i]=9e9;
			Tmax[i]=9e9;
			Tmax_err[i]=9e9;
			chi2_t0[i]=9e9;
			chi2_tmax[i]=9e9;
			}
		else {
		if(!histo->FitT0())
			{
			ATH_MSG_WARNING( " T0 fit faild for " << it->first.regionId() );
			t0[i]=9e9;
			t0_err[i]=9e9;
			T0[i]=9e9;
			T0_err[i]=9e9;
			tmax[i]=9e9;
			tmax_err[i]=9e9;
			Tmax[i]=9e9;
			Tmax_err[i]=9e9;
			chi2_t0[i]=9e9;
			chi2_tmax[i]=9e9;
			}
		else 
			{
			const TF1 *t0_fun=histo->GetT0Function();
			t0[i]=t0_fun->GetParameter(T0MTHistos::T0_PAR_NR_T0);
			t0_err[i]=t0_fun->GetParError(T0MTHistos :: T0_PAR_NR_T0);
			T0[i]=t0_fun->GetParameter(T0MTHistos::T0_PAR_NR_T);
			T0_err[i]=t0_fun->GetParError(T0MTHistos :: T0_PAR_NR_T);
			chi2_t0[i]=getChi2(t0_fun, it->second->GetTSpec());
			if(!histo->FitTmax())
				{
				ATH_MSG_WARNING( " TMax fit faild for " << 	it->first.regionId() );
				tmax[i]=9e9;
				tmax_err[i]=9e9;
				Tmax[i]=9e9;
				Tmax_err[i]=9e9;
				chi2_tmax[i]=9e9;
				}
			else
				{
				const TF1 *tmax_fun=histo->GetTMaxFunction();
				tmax[i]=tmax_fun-> GetParameter(T0MTHistos :: TMAX_PAR_NR_TMAX);
				tmax_err[i]=tmax_fun-> GetParError(T0MTHistos :: TMAX_PAR_NR_TMAX);
				Tmax[i]=tmax_fun-> GetParameter(T0MTHistos :: TMAX_PAR_NR_T);
				Tmax_err[i]=tmax_fun-> GetParError(T0MTHistos :: TMAX_PAR_NR_T);
				chi2_tmax[i]=getChi2(tmax_fun, it->second->GetTSpec());	
				}
			}
			}
			}
	//residual analysis
		TProfile *px=m_MDT_residuals[it->first]->ProfileX();
		res_chi2=getResidualChi2(px);
		px=m_MDT_residuals_t0_refined[it->first]->ProfileX();
		res_chi2_t0_improved=getResidualChi2(px);
		m_chamber_tree->Fill();
		}


/////////////////////////
// WRITE THE ROOT FILE //
/////////////////////////

	m_tfile->Write();

	return StatusCode::SUCCESS;

}

//*****************************************************************************

//::::::::::::::::::::::::
//:: METHOD handleEvent ::
//::::::::::::::::::::::::

StatusCode NtupleControlHistogramsTool::handleEvent(
			const MuonCalibEvent & event,
			int evnt_nb,
			const std::vector<MuonCalibSegment *> & segments,
			unsigned int position) {

///////////////
// VARIABLES //
///////////////

	const MuonCalibRawHitCollection *raw_hits(event.rawHitCollection());
//	const MuonCalibSegment *rpcHits(event.rpcHitCollection());
	unsigned int ml, ly, tb; // multilayer, layer, tube
	unsigned int ndof; // number of degrees of freedom of the segment fit
	int strip, strip2; // rpc strip
	double r, d; // drift radius and track distance from the wire
	int mdt_close_layer, mdt_close_multi;
	IMdtPatRecFitter *segment_fitter(0); // pointer to the segment fitter
	if (m_curved_fits) {
		segment_fitter = m_cfitter;
	} else {
		segment_fitter = m_qfitter;
	}

////////////////////
// RESET COUNTERS //
////////////////////

	m_nb_raw_MDT_hits.clear();
	
//////////////////////////////
// FILL RAW DATA HISTOGRAMS //
//////////////////////////////


// loop over the raw hits //
	for (MuonCalibRawHitCollection::MuonCalibRawMdtHitVecCit it=
		     			raw_hits->rawMdtHitCollectionBegin();
		 		it!=raw_hits->rawMdtHitCollectionEnd(); ++it) {

   // get the raw hit and check whether it is in the calibration region //
		MuonCalibRawMdtHit *hit(*it);
		
		if(!m_reg_sel_svc->isInRegion(hit->identify())) continue;


   // create maps if necessary //
		NtupleStationId station_identifier(hit->identify());
		station_identifier.SetMultilayer(0);
		createMaps(hit->identify());

   // calculate the number of raw MDT hits per chamber //
   		m_nb_raw_MDT_hits[station_identifier]++;

   // fill raw histograms as far as possible //
	// raw MDT hit distribution //
		ml = (hit->identify()).mdtMultilayer();
		ly = (hit->identify()).mdtTubeLayerIndex()+1;
		tb = (hit->identify()).mdtTubeIndex();
		
		m_MDT_raw_hit_distribution[station_identifier]->Fill(
			tb+(ly-1)*m_nb_tb[station_identifier]
				+(ml-1)*m_nb_tb[station_identifier]*
					m_nb_ly[station_identifier], 1.0);

	// raw MDT TDC spectrum //
		m_MDT_raw_TDC[station_identifier]->Fill(hit->tdcCount(), 1.0);

	// raw MDT ADC spectra //
		m_MDT_raw_ADC[station_identifier]->Fill(hit->adcCount(), 1.0);
	
	}
 
 // fill histograms with number of raw MDT hits //
	std::map<NtupleStationId, unsigned int>::iterator it =
						m_nb_raw_MDT_hits.begin();
	std::map<NtupleStationId, unsigned int>::iterator end_it =
						m_nb_raw_MDT_hits.end();

	while(it!=end_it) {
		m_MDT_raw_nb_hits[(*it).first]->Fill((*it).second, 1.0);
		++it;
	};

// Loop over the raw rpc hits
	
	for (MuonCalibRawHitCollection::MuonCalibRawRpcHitVecCit rit=
						raw_hits->rawRpcHitCollectionBegin();
						rit!=raw_hits->rawRpcHitCollectionEnd(); ++rit){
	mdt_close_multi=0; //ML next to RPC
	mdt_close_layer=0; //Layer next to RPC
	strip  = -1; // tmp var for the right strip number
	strip2 = -1; // tmp var for the right strip number in r doublet 2 rpc

	
   // get the raw rpc hit and check whether it is in the calibration region //
		MuonCalibRawRpcHit *rhit(*rit);

		if(!m_reg_sel_svc->isInRegion(rhit->identify())) continue;

   // create maps if necessary //
		NtupleStationId station_identifier(rhit->identify());
		station_identifier.SetMultilayer(0);
		createMaps(rhit->identify());
		
	// select hits on eta strips		
		if ( (rhit->identify()).rpcMeasuresPhiIndex() ){
			continue;
		}

	//find and select closest mdt layer to the hit rpc
		//BML / BMS
		if ( (rhit->identify()).stationNameIndex() == 2
				|| (rhit->identify()).stationNameIndex() ==3 ){
		// the BMx chambers have two rpcs, select by the R doublet
			switch ( (rhit->identify()).rpcDoubletRIndex() ){
				case (0):
					mdt_close_multi = 1;
					mdt_close_layer = 1;
				// add offset if there is a second rpc Z doublet
					if (! (rhit->identify()).rpcDoubletZIndex() ){
						strip = (rhit->identify()).rpcStripIndex();
					}
					else{
						strip = (rhit->identify()).rpcStripIndex()
									+  m_nb_etaStrips[0][0];
					}
					break;

				case (1):
					mdt_close_multi = 2;
					mdt_close_layer = 3;
				// add offset if there is a second rpc Z doublet
					if (! (rhit->identify()).rpcDoubletZIndex() ){
						strip2 = (rhit->identify()).rpcStripIndex();
					}
					else{
						strip2 = (rhit->identify()).rpcStripIndex()
									+  m_nb_etaStrips[1][0];
					}
					break;

				default:
					ATH_MSG_INFO( "no second rpc found for chambertype: " << 
							(rhit->identify()).stationNameIndex()	);
					break;
			}
							
		}
		// BOL
		else if ( (rhit->identify()).stationNameIndex() == 4 ){
			mdt_close_multi = 2;
			mdt_close_layer = 3;	

		// add offset if there is a second rpc Z doublet
			if (! (rhit->identify()).rpcDoubletZIndex() ){
					strip = (rhit->identify()).rpcStripIndex();
			}
			else{
					strip = (rhit->identify()).rpcStripIndex()
									+  m_nb_etaStrips[0][0];
			}
		}
		// BOS
		else if ( (rhit->identify()).stationNameIndex() == 5 ){
			mdt_close_multi = 1;
			mdt_close_layer = 1;	

		// add offset if there is a second rpc Z doublet
			if (! (rhit->identify()).rpcDoubletZIndex() ){
					strip = (rhit->identify()).rpcStripIndex();
			}
			else{
					strip = (rhit->identify()).rpcStripIndex()
									+  m_nb_etaStrips[0][0];
			}
			
		}
		else{
			ATH_MSG_WARNING( "MDT close layer could no be set for rpc with stationNameIndex: "
					<< (rhit->identify()).stationNameIndex() );
		}
	
	//now loop over all raw mdt hits in the layer which is next to the rpc
		for (MuonCalibRawHitCollection::MuonCalibRawMdtHitVecCit it=
		     			raw_hits->rawMdtHitCollectionBegin();
		 		it!=raw_hits->rawMdtHitCollectionEnd(); ++it) {
   	// get the raw mdt hit and check whether it is in the same chamber as the rpc hit //
			MuonCalibRawMdtHit *hit(*it);

			if ( (hit->identify()).stationName() != (rhit->identify()).stationName() ){
				continue;
			}
			if ( (hit->identify()).etaIndex() != (rhit->identify()).etaIndex() ){
				continue;
			}
			if ( (hit->identify()).phiIndex() != (rhit->identify()).phiIndex() ){
				continue;
			}
		// select layer next to the rpc
			if ( (hit->identify()).mdtMultilayerIndex()+1 != mdt_close_multi ){
				continue;
			} 
			if ( (hit->identify()).mdtTubeLayerIndex()+1 != mdt_close_layer ){
				continue;
			} 
			if (strip != -1){
				m_RPC_raw_etaStrip[station_identifier]->Fill(strip);
				m_MDT_RPC_raw_tubeStripCor[station_identifier]->Fill(
						(hit->identify()).mdtTubeIndex() + 1,
						strip + 1
						);
				m_MDT_RPC_raw_tubeStripCorVsEvent[station_identifier]->Fill(
						evnt_nb,
						((hit->identify()).mdtTubeIndex()) - strip
						);
	
				if (hit->adcCount() > 80 ){ // to cut elx noise
					m_MDT_RPC_raw_tubeStripCor_adcCut[station_identifier]->Fill(
						(hit->identify()).mdtTubeIndex() + 1,strip + 1);
					m_MDT_RPC_raw_tubeStripCorVsEvent_adcCut[station_identifier]->Fill(
						evnt_nb, ((hit->identify()).mdtTubeIndex()) - strip );
				}
			}
			
		// Fill histos for 2nd rpc r doublet if BMx chamber	
			if (m_nb_totalEtaStrips2[station_identifier] 
													&& strip2 != -1){
				m_RPC_raw_etaStrip2[station_identifier]->Fill(strip2);
				m_MDT_RPC_raw_tubeStripCor2[station_identifier]->Fill(
						(hit->identify()).mdtTubeIndex() + 1,
						strip2 + 1
						);
				m_MDT_RPC_raw_tubeStripCorVsEvent2[station_identifier]->Fill(
						evnt_nb,
						((hit->identify()).mdtTubeIndex()) - strip2
						);
				if (hit->adcCount() > 80 ){ // to cut elx noise
					m_MDT_RPC_raw_tubeStripCor_adcCut2[station_identifier]->Fill(
						(hit->identify()).mdtTubeIndex() + 1,strip2 + 1);
					m_MDT_RPC_raw_tubeStripCorVsEvent_adcCut2[station_identifier]->Fill(
						evnt_nb, ((hit->identify()).mdtTubeIndex()) - strip2 );
				}

			}
		
		}
	
	}
	
	
////////////////////////
// SEGMENT HISTOGRAMS //
////////////////////////

// loop over the segments //
	for (unsigned int k=position; k<segments.size(); k++) {


   // get the first hit to check if it is in the calibration region //
		if(!m_reg_sel_svc->isInRegion(segments[k]->mdtHOT()[0] ->identify())) continue;

	   // create maps if necessary //
		NtupleStationId station_identifier((segments[k]->mdtHOT()[0])->identify());
		station_identifier.SetMultilayer(0);
		createMaps((segments[k]->mdtHOT()[0])->identify());
			
   // perform a refit if requested //
 		if (m_refit_segments) {
			if(!segment_fitter->fit(*(segments[k]))) return StatusCode :: SUCCESS;
		}
		
   // number of hits on the segment //
   		if (m_refit_segments) {
			m_MDT_segment_hits[station_identifier]->Fill(
				segment_fitter->numberOfTrackHits(), 1.0);
		} else {
			m_MDT_segment_hits[station_identifier]->Fill(
					segments[k]->mdtHitsOnTrack(), 1.0);
		}

   // ratio of hits on the segment and the number of raw MDT hits //
		if (m_refit_segments) {
			m_MDT_segment_hist_to_raw_hits_ratio[station_identifier
				]->Fill(segment_fitter->numberOfTrackHits()/
				static_cast<double>(m_nb_raw_MDT_hits[
				station_identifier]), 1.0);
		} else {
			m_MDT_segment_hist_to_raw_hits_ratio[station_identifier
				]->Fill(segments[k]->mdtHitsOnTrack()/
				static_cast<double>(m_nb_raw_MDT_hits[
				station_identifier]), 1.0);
		}

   // CL of the segment fit //
		if (m_refit_segments) {
			if (m_curved_fits) {
				ndof = segment_fitter->numberOfTrackHits()-3;
			} else {
				ndof = segment_fitter->numberOfTrackHits()-2;
			}
		} else {
			ndof = segments[k]->mdtHitsOnTrack()-2;
		}
		if (ndof>0 && !std::isnan(segments[k]->chi2())) {
			Genfun::CumulativeChiSquare one_minus_CL(ndof);
			m_MDT_segment_CL[station_identifier]->Fill(
				1.0-one_minus_CL(fabs(segments[k]->chi2())),
				1.0);
		}

   // drift-time and ADC spectra //
		if (m_refit_segments) {
			for (unsigned int l=0; l<segment_fitter->numberOfTrackHits();
									l++) {
				m_MDT_segment_t[station_identifier]->Fill(
					(segment_fitter->trackHits())[l
						]->driftTime(), 1.0);
				m_MDT_segment_r[station_identifier]->Fill(
					(segment_fitter->trackHits())[l
						]->driftRadius(), 1.0);
				const MuonFixedId &h_id((segment_fitter-> trackHits())[l]-> identify());
				m_MDT_segment_hit_dis[station_identifier]-> Fill((h_id.mdtMultilayer() - 1) * 216 + (h_id.mdtTubeLayer() - 1) * 72 + h_id.mdtTube() -1);
				m_MDT_segment_tspec_ml[0][station_identifier]->
				 FillT((segment_fitter->trackHits())[l
						]->driftTime());
				MuonFixedId id((segment_fitter->trackHits())[l
						]->identify());	
				m_MDT_segment_tspec_ml[id.mdtMultilayer()] [station_identifier]-> FillT((segment_fitter->trackHits())[l]->driftTime()); 
				m_MDT_segment_TDC[station_identifier]->Fill(
					(segment_fitter->trackHits())[l
						]->tdcCount(), 1.0);
				m_MDT_segment_ADC[station_identifier]->Fill(
					(segment_fitter->trackHits())[l
						]->adcCount(), 1.0);
					
			}
		} else {
			for (unsigned int l=0; l<segments[k]->hitsOnTrack();
									l++) {
				const MuonFixedId &h_id((segments[k]->mdtHOT())[l]-> identify());
				m_MDT_segment_hit_dis[station_identifier]-> Fill((h_id.mdtMultilayer() - 1) * 216 + (h_id.mdtTubeLayer() - 1) * 72 + h_id.mdtTube() -1);
				m_MDT_segment_t[station_identifier]->Fill(
					(segments[k]->mdtHOT())[l
						]->driftTime(), 1.0);
				m_MDT_segment_r[station_identifier]->Fill(
					(segments[k]->mdtHOT())[l
						]->driftRadius(), 1.0);
				m_MDT_segment_TDC[station_identifier]->Fill(
					(segments[k]->mdtHOT())[l
						]->tdcCount(), 1.0);
				m_MDT_segment_ADC[station_identifier]->Fill(
					(segments[k]->mdtHOT())[l
						]->adcCount(), 1.0);
			}
		}

   // segment slopes //
		m_MDT_segment_slope[station_identifier]->Fill(
			57.30*atan((segments[k]->direction()).y()/
				(segments[k]->direction()).z()), 1.0);

   // track residuals //
		if (m_refit_segments) {
		  if (ndof>0 && !std::isnan(segments[k]->chi2())) {
			for (unsigned int l=0; l<segment_fitter->numberOfTrackHits();
									l++) {
				r = fabs((segment_fitter->trackHits())[l
						]->driftRadius());
				d = (segment_fitter->trackHits())[l
						]->signedDistanceToTrack();
				double t = (segment_fitter->trackHits())[l] -> driftTime();
				//double sig=(segment_fitter->trackHits())[l] -> sigmaDriftRadius();
/*				if(d==0)
					{
					std::cout<<"XXXXXXX "<<d<<" "<<r<<" "<<t<<" "<<sig<<std::endl;
					}
				else
					{
					std::cout<<"ZZZZZZZ "<<d<<" "<<r<<" "<<t<<" "<<sig<<std::endl;
					}*/
				m_MDT_residuals[station_identifier]->Fill(
						fabs(d), r-fabs(d), 1.0);
				m_MDT_residuals_vs_x[station_identifier]->Fill(
						(segment_fitter->trackHits())[l
						]->localPosition().x(), r-fabs(d), 1.0);
				m_MDT_r_vs_rtrack[station_identifier]->Fill(
						d, r, 1.0);
				m_MDT_t_vs_rtrack[station_identifier]->Fill(
						d, t, 1.0);

				MuonFixedId id((segment_fitter->trackHits())[l
						]->identify());
				if (id.mdtMultilayer()==1) {
					m_MDT_residuals_ml1[station_identifier
						]->Fill(d, r-fabs(d), 1.0);
				} else {
					m_MDT_residuals_ml2[station_identifier
						]->Fill(d, r-fabs(d), 1.0);
				}
			}
//			std::cout<<"==========="<<std::endl;
		  }	
		} else {
			for (unsigned int l=0; l<segments[k]->hitsOnTrack();
									l++) {
				r = fabs((segments[k]->mdtHOT())[l
						]->driftRadius());
				d = (segments[k]->mdtHOT())[l
						]->signedDistanceToTrack();
				double xx = (segments[k]->mdtHOT())[l] -> localPosition().x();
				m_MDT_residuals[station_identifier]->Fill(
						fabs(d), r-fabs(d), 1.0);
				m_MDT_residuals_vs_x[station_identifier]->Fill( xx, r-fabs(d), 1.0);

				MuonFixedId id((segments[k]->mdtHOT())[l
						]->identify());
				if (id.mdtMultilayer()==1) {
					m_MDT_residuals_ml1[station_identifier
						]->Fill(d, r-fabs(d), 1.0);
				} else {
					m_MDT_residuals_ml2[station_identifier
						]->Fill(d, r-fabs(d), 1.0);
				}
			}
		}


//test histograms
		const MdtStationT0Container *t0(m_input_service->GetT0(station_identifier));
		const IRtRelation *rt=m_input_service->GetRtRelation(
							station_identifier);
		if(t0!=NULL)
		for (unsigned int l=0; l<segments[k]->hitsOnTrack();l++) 
			{
			const MdtCalibHitBase *hot((segments[k]->mdtHOT())[l]);
			const MuonFixedId &id(hot->identify());
			m_t0_diff[station_identifier]->Fill(hot->tubeT0() - t0->t0(id.mdtMultilayer(), id.mdtTubeLayer(), id.mdtTube()));
			m_t0_diff_global->Fill(hot->tubeT0() - t0->t0(id.mdtMultilayer(), id.mdtTubeLayer(), id.mdtTube()));
			//std::cout<<id.mdtMultilayer()<<" "<<id.mdtTubeLayer()<<" "<<id.mdtTube()<<std::endl;
			}
		if(rt!=NULL)
		for (unsigned int l=0; l<segments[k]->hitsOnTrack();l++) 
			{
			const MdtCalibHitBase *hot((segments[k]->mdtHOT())[l]);
			m_r_mintrt[station_identifier]-> Fill(fabs(hot->driftRadius()) - fabs(rt->radius(hot->driftTime())));
			m_r_mintrt_global->Fill(fabs(hot->driftRadius()) - fabs(rt->radius(hot->driftTime())));
			m_r_minrt_vs_r ->Fill(fabs(hot->driftRadius()), fabs(hot->driftRadius()) - fabs(rt->radius(hot->driftTime())));
			}

// t0 refinement //
	MuonCalibSegment seg(*segments[k]);
        if (!m_refine_t0 || !m_refit_segments) {
		m_MDT_t0_refinement[station_identifier]->Fill(seg.fittedT0());
		for (unsigned int l=0; l<seg.hitsOnTrack();l++) 
				{
				double t = (seg.mdtHOT())[l]->driftTime() + seg.fittedT0();
				m_MDT_t_refined[station_identifier]->Fill(t);
				}
		}

		if (!m_refit_segments) {
			continue;
		}
		if (rt==0) {
			continue;
		}
        if (!m_refine_t0) {
		continue;
	        }


		double error;
		bool fail;
		m_t0_refinement->SetDeltaT0(12.5);
		double delta(m_t0_refinement->getDeltaT0(&seg, rt, true,
		error, fail));
		if (!fail) {
		m_MDT_t0_refinement[station_identifier]->Fill(
			delta, 1.0);

    // refit the segment //
 		if (m_refit_segments) {
			if(!segment_fitter->fit(*(segments[k]))) return StatusCode :: SUCCESS;
		}

		for (unsigned int l=0; l<seg.hitsOnTrack();l++) {
				r = fabs((seg.mdtHOT())[l
						]->driftRadius());
				d = fabs((seg.mdtHOT())[l
						]->signedDistanceToTrack());
				double t = (seg.mdtHOT())[l
						]->driftTime();
				m_MDT_residuals_t0_refined[station_identifier
						]->Fill(d, r-d, 1.0);
				if (seg.hitsOnTrack()>4) {
                    m_MDT_residuals_t0_refined_gt4hits[station_identifier
						]->Fill(d, r-d, 1.0);
                }
				m_MDT_t_refined[station_identifier]->Fill(t);
		}
		}
	}

	return StatusCode::SUCCESS;

}



//*****************************************************************************

//:::::::::::::::::::::::::
//:: METHOD KeepSegments ::
//:::::::::::::::::::::::::

bool NtupleControlHistogramsTool::KeepSegments(void) const {

	return false;

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD createMaps ::
//:::::::::::::::::::::::

void NtupleControlHistogramsTool::createMaps(const MuonFixedId & id) {

///////////////
// VARIABLES //
///////////////

	NtupleStationId station_identifier(id);
	station_identifier.SetMultilayer(0);
	std::map<NtupleStationId, unsigned int>::iterator end_it =
						m_nb_ml.end();

////////////////////////////////////
// FIND THE IDENTIFIER IN m_nb_ml //
////////////////////////////////////

	if (m_nb_ml.find(station_identifier)!=end_it) {
		return;
	}

///////////////////////////////////////////
// GET THE GEOMETRY OF THE GIVEN CHAMBER //
///////////////////////////////////////////

	Identifier station_id = m_id_tool->fixedIdToId(id);
	const MuonGM::MdtReadoutElement *MdtRoEl =
			m_detMgr->getMdtReadoutElement(
	    		m_MdtIdHelper->channelID(station_id,1,1,1));

	m_nb_ml[station_identifier] = m_MdtIdHelper->numberOfMultilayers(
								station_id);
	m_nb_ly[station_identifier] = MdtRoEl->getNLayers();
	m_nb_tb[station_identifier] = MdtRoEl->getNtubesperlayer();

 // get the rpc geom information
 
 // loop over possible R doublets
	m_nb_totalEtaStrips2[station_identifier]=0;
	for (int r_doublet=0; r_doublet<2; r_doublet++){
	// loop over possible Z doublets
		for (int z_doublet=0; z_doublet<4; z_doublet++){
            bool isValid=false;
            Identifier rpcId = m_RpcIdHelper->channelID(m_MdtIdHelper->stationName(station_id), m_MdtIdHelper->stationEta(station_id), m_MdtIdHelper->stationPhi(station_id), r_doublet+1, z_doublet+1, 1, 1, 1, 1, true, &isValid); // last 6 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip, bool check, bool* isValid
            if (!isValid) continue;
            const MuonGM::RpcReadoutElement* RpcRoEl = m_detMgr->getRpcReadoutElement(rpcId);

		//save nuber of eta strips per doublet, if there is one
			if (RpcRoEl){
				m_nb_etaStrips[r_doublet][z_doublet] = RpcRoEl->NetaStrips();
			}
			else {
				m_nb_etaStrips[r_doublet][z_doublet] = 0;
			}

		//save the total number of eta strips for the two different R doublets 
			if (!r_doublet){
			m_nb_totalEtaStrips[station_identifier] = m_nb_totalEtaStrips[station_identifier]
															 + m_nb_etaStrips[r_doublet][z_doublet];
			}
			else{
			m_nb_totalEtaStrips2[station_identifier] = m_nb_totalEtaStrips2[station_identifier]
															 + m_nb_etaStrips[r_doublet][z_doublet];
			}
		}

	}


///////////////////////////////
// CREATE THE REMAINING MAPS //
///////////////////////////////

// counter //
	m_nb_raw_MDT_hits[station_identifier] = 0;

// histograms //
	ToString tostring;
	string file_dir(id.stationNumberToFixedStationString(id.stationName())
		+"_"+tostring(id.phi())+"_"+tostring(id.eta()));
	m_tfile->mkdir(file_dir.c_str());
	m_tfile->cd(file_dir.c_str());

	m_MDT_raw_nb_hits[station_identifier] =
			new TH1F("MDT_raw_nb_hits",
				"Number of Raw MDT Hits",
				1+m_nb_ml[station_identifier]*(
				m_nb_ly[station_identifier]*
				m_nb_tb[station_identifier]),
				-0.5,
				0.5+m_nb_ml[station_identifier]*(
				m_nb_ly[station_identifier]*
				m_nb_tb[station_identifier]));
		m_MDT_raw_nb_hits[station_identifier
				]->SetXTitle("Number of Hits");
		m_MDT_raw_nb_hits[station_identifier
				]->SetYTitle("Entries / 1");


	m_MDT_raw_hit_distribution[station_identifier] =
			new TH1F("MDT_raw_hit_distribution",
				"Raw MDT Hit Profile",
				m_nb_ml[station_identifier]*(
				m_nb_ly[station_identifier]*
				m_nb_tb[station_identifier]),
				0.5,
				0.5+m_nb_ml[station_identifier]*(
				m_nb_ly[station_identifier]*
				m_nb_tb[station_identifier]));
		m_MDT_raw_hit_distribution[station_identifier
				]->SetXTitle("Channel");
		m_MDT_raw_hit_distribution[station_identifier
				]->SetYTitle("Entries / Channel");

	m_MDT_raw_TDC[station_identifier] =
			new TH1F("MDT_raw_TDC",
				"Raw MDT TDC Counts",
				2*4096+1, -4096.5, 4096.5);
		m_MDT_raw_TDC[station_identifier]->SetXTitle("TDC Count");
		m_MDT_raw_TDC[station_identifier
				]->SetYTitle("Entries / (0.78125 ns)");

	m_MDT_raw_ADC[station_identifier] =
			new TH1F("MDT_raw_ADC",
				"Raw MDT ADC Counts",
				4096+1, -0.5, 4096.5);
		m_MDT_raw_ADC[station_identifier]->SetXTitle("ADC count");
		m_MDT_raw_ADC[station_identifier
				]->SetYTitle("Entries / (0.78125 ns)");
	if (m_nb_totalEtaStrips[station_identifier]){
		m_RPC_raw_etaStrip[station_identifier] =
				new TH1F("RPC_raw_etaStrip",
						"Raw Hits: RPC Eta Strip; RPC Eta Strip; Entries / Strip",
						m_nb_totalEtaStrips[station_identifier],
						0.5,
						0.5 + m_nb_totalEtaStrips[station_identifier]);
		m_MDT_RPC_raw_tubeStripCor[station_identifier] =
				new TH2F("MDT_RPC_raw_tubeStripCor",
					"Raw Hits: RPC Eta Strip vs. Raw MDT Tube; MDT Tube; RPC Eta Strip",
					m_nb_tb[station_identifier],
					0.5,
					0.5+m_nb_tb[station_identifier],
					m_nb_totalEtaStrips[station_identifier],
					0.5,
					0.5+m_nb_totalEtaStrips[station_identifier]);
 		m_MDT_RPC_raw_tubeStripCorVsEvent[station_identifier] =
				new TH2F("MDT_RPC_raw_tubeStripCorVsEvent",
					"Raw Hits: (MDT Tube - RPC Strip) vs. First 100k Events; Event ; (MDT Tube - RPC strip)",
					1000,
					0.5,
					100000,
					2 * m_nb_tb[station_identifier],
					-0.5 - m_nb_tb[station_identifier],
					0.5+m_nb_tb[station_identifier]);
		m_MDT_RPC_raw_tubeStripCorVsEvent_adcCut[station_identifier] =
				new TH2F("MDT_RPC_raw_tubeStripCorVsEvent_adcCut",
					"Raw Hits: (MDT Tube - RPC Strip) vs. first 100k Events (MDT ADC > 80); Event ; (MDT Tube - RPC strip)",
					1000,
					0.5,
					100000,
					2 * m_nb_tb[station_identifier],
					-0.5 - m_nb_tb[station_identifier],
					0.5+m_nb_tb[station_identifier]);
		m_MDT_RPC_raw_tubeStripCor_adcCut[station_identifier] =
				new TH2F("MDT_RPC_raw_tubeStripCor_adcCut",
					"Raw RPC Eta Strip vs. Raw MDT Tube (MDT ADC > 80); MDT Tube; RPC Eta Strip",
					m_nb_tb[station_identifier],
					0.5,
					0.5+m_nb_tb[station_identifier],
					m_nb_totalEtaStrips[station_identifier],
					0.5,
					0.5+m_nb_totalEtaStrips[station_identifier]);

		if (m_nb_totalEtaStrips2[station_identifier]){
			 m_RPC_raw_etaStrip2[station_identifier] =
			   	 new TH1F("RPC_raw_etaStrip2",
			   			 "Raw Hits: RPC Eta Strip (2nd RDoublet); RPC Eta Strip; Entries / Strip",
			   			 m_nb_totalEtaStrips2[station_identifier],
			   			 0.5,
			   			 0.5 + m_nb_totalEtaStrips2[station_identifier]);
			 m_MDT_RPC_raw_tubeStripCor2[station_identifier] =
			   	 new TH2F("MDT_RPC_raw_tubeStripCor2",
			   		 "Raw Hits: RPC Eta Strip vs. Raw MDT Tube (2nd RDoublet); MDT Tube; RPC Eta Strip",
			   		 m_nb_tb[station_identifier],
			   		 0.5,
			   		 0.5+m_nb_tb[station_identifier],
			   		 m_nb_totalEtaStrips2[station_identifier],
			   		 0.5,
			   		 0.5+m_nb_totalEtaStrips2[station_identifier]);
 			 m_MDT_RPC_raw_tubeStripCorVsEvent2[station_identifier] =
			   	 new TH2F("MDT_RPC_raw_tubeStripCorVsEvent2",
			   		 "Raw Hits: (MDT Tube - RPC Strip) vs. First 100k Events (2nd RDoublet); Event ; (MDT Tube - RPC strip)",
			   		 1000,
			   		 0.5,
			   		 100000,
			   		 2 * m_nb_tb[station_identifier],
			   		 -0.5 - m_nb_tb[station_identifier],
			   		 0.5+m_nb_tb[station_identifier]);
			 m_MDT_RPC_raw_tubeStripCorVsEvent_adcCut2[station_identifier] =
			   	 new TH2F("MDT_RPC_raw_tubeStripCorVsEvent_adcCut2",
			   		 "Raw Hits: (MDT Tube - RPC Strip) vs. first 100k Events (MDT ADC > 80) (2nd RDoublet); Event ; (MDT Tube - RPC strip)",
			   		 1000,
			   		 0.5,
			   		 100000,
			   		 2 * m_nb_tb[station_identifier],
			   		 -0.5 - m_nb_tb[station_identifier],
			   		 0.5+m_nb_tb[station_identifier]);
			 m_MDT_RPC_raw_tubeStripCor_adcCut2[station_identifier] =
			   	 new TH2F("MDT_RPC_raw_tubeStripCor_adcCut2",
			   		 "Raw RPC Eta Strip vs. Raw MDT Tube (MDT ADC > 80) (2nd RDoublet); MDT Tube; RPC Eta Strip",
			   		 m_nb_tb[station_identifier],
			   		 0.5,
			   		 0.5+m_nb_tb[station_identifier],
			   		 m_nb_totalEtaStrips2[station_identifier],
			   		 0.5,
			   		 0.5+m_nb_totalEtaStrips2[station_identifier]);
	
		}
	
	}
	m_t0_diff[station_identifier] = new TH1F ("t0_diff", "", 201, -100.5, 100.5);
	m_r_mintrt[station_identifier] = new TH1F ("r_min_rt", "", 101, -0.1, 0.1);
	m_MDT_segment_hits[station_identifier] =
			new TH1F("MDT_segments_hits",
				"Number of MDT Segment Hits",
				31, -0.5, 30.5);
		m_MDT_segment_hits[station_identifier
				]->SetXTitle("Number of Hits");
		m_MDT_segment_hits[station_identifier
				]->SetYTitle("Entries / Hit");

	m_MDT_segment_hist_to_raw_hits_ratio[station_identifier] =
			new TH1F("MDT_segment_hist_to_raw_hits_ratio",
			"Ratio of the Number of Segments and Raw MDT Hits",
				100, -0.1, 2.1);
		m_MDT_segment_hits[station_identifier
				]->SetXTitle("Ratio");

	m_MDT_segment_CL[station_identifier] =
			new TH1F("MDT_segment_CL",
				"Segment Confidence Level Distribution",
				120, -0.1, 1.1);
		m_MDT_segment_CL[station_identifier
				]->SetXTitle("confidence level");
		m_MDT_segment_CL[station_identifier
				]->SetYTitle("dn/dCL (1/0.01)");

	m_MDT_segment_TDC[station_identifier] =
			new TH1F("MDT_segment_TDC",
				"Segment MDT Hit TDC Counts",
				2*4096+1, -4096.5, 4096.5);
		m_MDT_raw_TDC[station_identifier]->SetXTitle("TDC count");
		m_MDT_raw_TDC[station_identifier
				]->SetYTitle("entries/(TDC count)");
		m_MDT_raw_TDC[station_identifier]->SetXTitle("drift time (ns)");
		m_MDT_raw_TDC[station_identifier
				]->SetYTitle("entries/(0.78125 ns)");

	T0MTSettings *m_settings = new T0MTSettings;
	m_settings->NBinsTime()=2*4096+1;
	m_settings->TimeMin()=-4096.5*0.78125;
	m_settings->TimeMax()=4096.5*0.78125;
	m_settings->AddFitfun()=true;
	for(unsigned int i=0; i<3; i++)
		{
		m_MDT_segment_tspec_ml[i][station_identifier]= new T0MTHistos();
		std::ostringstream name;
		name<<"segments_ml"<<i+1;
		m_MDT_segment_tspec_ml[i][station_identifier]->Initialize(i, m_settings, name.str().c_str());
		}		
	m_MDT_segment_t[station_identifier] = new TH1F("MDT_segment_t",
				"Segment MDT Hit TDC Counts",
				2*4096+1, -4096.5*0.78125, 4096.5*0.78125);
	m_MDT_segment_r[station_identifier] = new TH1F("MDT_segment_r",
				"Segment Drift radius",
				200, -15.0, 15.0);
	m_MDT_segment_hit_dis[station_identifier] = new TH1F("MDT_segment_hit_dis",
				"hit distribution",
				432, -0.5, 431.5);
		m_MDT_segment_t[station_identifier]->SetXTitle("Drift Time (ns)");
		m_MDT_segment_t[station_identifier
				]->SetYTitle("Entries / ns");

	m_MDT_segment_ADC[station_identifier] =
			new TH1F("MDT_segment_ADC",
				"Segment MDT Hit ADC Counts",
				4096+1, -0.5, 4096.5);
		m_MDT_raw_ADC[station_identifier]->SetXTitle("ADC count");
		m_MDT_raw_ADC[station_identifier
				]->SetYTitle("entries/(ADC count)");

	m_MDT_segment_slope[station_identifier] =
			new TH1F("MDT_segment_slope",
				"Segment Slopes",
				181, -90.5, 90.5);
		m_MDT_raw_ADC[station_identifier]->SetXTitle("slope (degrees)");
		m_MDT_raw_ADC[station_identifier
				]->SetYTitle("dn/dslope (1/degree)");

	m_MDT_residuals[station_identifier] =
			new TH2F("MDT_residuals",
				"MDT residuals",
				152, -0.1, 15.1, 200, -1.0, 1.0);
		m_MDT_residuals[station_identifier
				]->SetXTitle("r (mm)");
		m_MDT_residuals[station_identifier
				]->SetYTitle("residual (mm)");
	m_MDT_residuals_vs_x[station_identifier] =
			new TH2F("MDT_residuals_vs_x",
				"MDT residuals versus the local x coordinate",
				500, -3100.0, 3100.0, 200, -1.0, 1.0);
		m_MDT_residuals_vs_x[station_identifier
				]->SetXTitle("x (mm)");
		m_MDT_residuals_vs_x[station_identifier
				]->SetYTitle("residual (mm)");
	m_MDT_r_vs_rtrack[station_identifier] =
			new TH2F("MDT_r_vs_rtrack",
				"MDT MDT_r_vs_rtrack",
				4000, -15.1, 15.1, 2000, 0, 15.1);
		m_MDT_residuals[station_identifier
				]->SetXTitle("r (mm)");
		m_MDT_residuals[station_identifier
				]->SetYTitle("residual (mm)");
	m_MDT_t_vs_rtrack[station_identifier] =
			new TH2F("MDT_t_vs_rtrack",
				"MDT MDT_t_vs_rtrack",
				1000, -15.1, 15.1, 1000, -100, 800);
		m_MDT_residuals[station_identifier
				]->SetXTitle("r (mm)");
		m_MDT_residuals[station_identifier
				]->SetYTitle("residual (mm)");

	m_MDT_residuals_ml1[station_identifier] =
			new TH2F("MDT_residuals_ml1",
				"MDT residuals in multilayer 1",
				302, -15.1, 15.1, 200, -1.0, 1.0);
		m_MDT_residuals_ml1[station_identifier
				]->SetXTitle("r (mm)");
		m_MDT_residuals_ml1[station_identifier
				]->SetYTitle("residual (mm)");

	m_MDT_residuals_ml2[station_identifier] =
			new TH2F("MDT_residuals_ml2",
				"MDT residuals in multilayer 2",
				302, -15.1, 15.1, 200, -1.0, 1.0);
		m_MDT_residuals_ml2[station_identifier
				]->SetXTitle("r (mm)");
		m_MDT_residuals_ml2[station_identifier
				]->SetYTitle("residual (mm)");

        m_MDT_t0_refinement[station_identifier] =
			new TH1F("MDT_t0_refinement",
				"#Delta t_{0} from Segment Fit",
				901, -150.5, 150.5);
			m_MDT_t0_refinement[station_identifier
				]->SetXTitle("#Delta t_{0} (ns)");
			m_MDT_t0_refinement[station_identifier
				]->SetYTitle("entries (1/ns)");
		
	if(!m_refine_t0)
			{
			m_MDT_t_refined[station_identifier] = new 
				TH1F("m_MDT_t_unrefined",
				"Segment MDT t without t0 refinement",
				2*4096+1, -4096.5*0.78125, 4096.5*0.78125);

			}
	if (m_refine_t0) {
		m_MDT_t_refined[station_identifier] = new 
				TH1F("m_MDT_t_refined",
				"Segment MDT t t0 refined",
				2*4096+1, -4096.5*0.78125, 4096.5*0.78125);

		m_MDT_residuals_t0_refined[station_identifier] =
			new TH2F("MDT_residuals_t0_refined",
				"MDT residuals after t_{0} refinement",
				152, -0.1, 15.1, 200, -1.0, 1.0);
			m_MDT_residuals[station_identifier
				]->SetXTitle("r (mm)");
			m_MDT_residuals[station_identifier
				]->SetYTitle("residual (mm)");

		m_MDT_residuals_t0_refined_gt4hits[station_identifier] =
			new TH2F("MDT_residuals_t0_refined_gt4hits",
				"MDT residuals after t_{0} refinement for segments with more than 4 hits",
				152, -0.1, 15.1, 200, -1.0, 1.0);
			m_MDT_residuals[station_identifier
				]->SetXTitle("r (mm)");
			m_MDT_residuals[station_identifier
				]->SetYTitle("residual (mm)");
	}

	m_tfile->cd("..");

	return;

}


inline double NtupleControlHistogramsTool :: getChi2(const TF1 *fun, const TH1F *hist) const
	{
	double chi2=0;
	int n_bins_used=0;
	Double_t x_min, x_max;
	fun->GetRange(x_min, x_max);
//	cout<<"Range=["<<x_min<<","<<x_max<<"]"<<endl;
	for(int i=1; i<=hist->GetNbinsX(); i++)
		{
		double x=hist->GetBinCenter(i);
		if(x<x_min || x>x_max) continue;
		double diff=hist->GetBinContent(i) - fun->Eval(x);
		double error=hist->GetBinError(i);
		if(error<1.0) continue;
		chi2+=(diff*diff)/(error*error);
		n_bins_used++;
		}
	if(n_bins_used==0) return 9e9;
	return chi2/n_bins_used;
	}


inline double NtupleControlHistogramsTool :: getResidualChi2(TProfile *px) const
	{
//loop over all bins
	double chi2=0.0;
	int n_bins_used=0;
	for(int	i=1; i<=px->GetNbinsX(); i++)
		{
		if(px->GetBinCenter(i)<1.0 || px->GetBinCenter(i)>14) continue;
		double x = px->GetBinContent(i);
		double error = px->GetBinError(i);
		if(error < 1e-6 ) continue;
		chi2+=x*x/(error*error);
		n_bins_used++;
		}
	if(n_bins_used==0) return 9e9;
	return chi2/n_bins_used;
	}

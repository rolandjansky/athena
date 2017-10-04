/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// DRIFT TUBE SPATIAL RESOLUTION AUTOCALIBRATION ALGORITHM //
// Author: I.Potrap  (MPI)                                 //
//*********************************************************//

//////////////////
// HEADER FILES //
//////////////////

// standard C++ //
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

// MuonCalib classes //
#include "MuonCalibEventBase/MuonCalibPattern.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MdtCalibHitBase.h"
#include "MdtCalibData/RtResolutionFromPoints.h"
#include "MdtCalibData/RtChebyshev.h"
#include "MdtCalibData/RtRelationLookUp.h"

// NtupleSimpleResolutionTool //
#include "MuonCalibStandAloneTools/NtupleSimpleresolutionTool.h"
#include "MdtCalibIOSvc/MdtCalibInputSvc.h"


//root
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TTree.h"
#include "TProfile.h"
// HEP MATRIX //
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"

////////////////////////
// NAMESPACE SETTINGS //
////////////////////////

using namespace std;
namespace MuonCalib {

const unsigned int NtupleSimpleResolutionTool::MAXNUMHITS=50;

/////////////////
// Constructor //
/////////////////

NtupleSimpleResolutionTool :: NtupleSimpleResolutionTool(const std::string& t, const std::string&
n, const IInterface* p) : AthAlgTool(t, n, p), m_final_resolution(NULL),
m_calib_input_svc("MdtCalibInputSvc", n), m_reg_sel_svc("RegionSelectionSvc", n), m_is_initialized(false)
	{
	declareInterface< NtupleCalibrationTool >(this);
	declareProperty("MdtCalibInputSvc", m_calib_input_svc);
	declareProperty("RegionSelectionSvc", m_reg_sel_svc);
	m_curved = true;
	declareProperty("CurvedFit", m_curved);
	m_REJ_LOW = 0.1;
	declareProperty("RejectLow", m_REJ_LOW);
	m_REJ_TOP = 0.3;
	declareProperty("RejectTop", m_REJ_TOP);
	m_DELTA_CONV = 0.05;
	declareProperty("DeltaConv", m_DELTA_CONV);
	m_DIFF_MAX = 0.006;
	declareProperty("DiffMax", m_DIFF_MAX);
	m_MAX_NUM_IT = 20;
	declareProperty("MaximumIterations" ,m_MAX_NUM_IT);
	m_replace_with_flat = false;
	declareProperty("InitialFlatResolution", m_replace_with_flat);
	m_hist_fit_method = false;
	declareProperty("HistFitMethod", m_hist_fit_method);
	m_suppress_outliers = false;
	declareProperty("SuppressOutliers", m_suppress_outliers);
	m_SUPPRESS_LOW = 0.2;
	declareProperty("SuppressionLow", m_SUPPRESS_LOW);
	m_SUPPRESS_TOP = 0.2;
	declareProperty("SuppressionTop", m_SUPPRESS_TOP);
	m_spr_out_name = string("spr_out.root");
	declareProperty("OutputResolutionFunction", m_spr_out_name);
	}


///////////////////////
// METHOD initialize //
///////////////////////

StatusCode NtupleSimpleResolutionTool::initialize() 
	{

	ATH_MSG_INFO( "Initializing Simple resolution Tool" );
	//StatusCode sc;

//get pointer to Calibration input service
	ATH_CHECK( m_calib_input_svc.retrieve() );
//get pointer to region selection service
    ATH_CHECK( m_reg_sel_svc.retrieve() );

/////////////////////
// OTHER VARIABLES //
/////////////////////
        m_MINNUMHITS = 4;
	if(m_curved) m_MINNUMHITS = 5;

	m_sprgr=0;
	m_sprgr_r=0;
	m_number_of_segments = 0;
		
////////////////////////////////////////////////////////
// DECLARATION OF HISTOGRAMS AND NTUPLES              //
////////////////////////////////////////////////////////
// histograms //
	m_residual = new TH2F("m_residual", "RESIDUALS",
			RBIN, 0.0, RRANGE, RESBIN, -0.5*RESRANGE, 0.5*RESRANGE);
	m_residual0 = new TH2F("m_residual0", "RESIDUALS",
			RBIN, 0.0, RRANGE, RESBIN, -0.5*RESRANGE, 0.5*RESRANGE);
			
	m_residual2 = new TH2F("m_residual2", "RESIDUALS2",
			RBIN, 0.0, RRANGE, RESBIN, -0.5*RESRANGE, 0.5*RESRANGE);
		
	m_residual3 = new TH2F("m_residual3", "RESIDUALS3",
			RBIN, 0.0, RRANGE, RESBIN, -0.5*RESRANGE, 0.5*RESRANGE);

	m_sigma_track = new TH2F("m_sigma_track", "SIGMA OF TRACK FIT",
					RBIN, 0.0, RRANGE, SBIN, 0, SRANGE);
	m_sigma_track_lim = new TH2F("m_sigma_track_lim",
	"SIGMA OF TRACK FIT (LIMITED)", RBIN, 0.0, RRANGE, SBIN, 0, SRANGE);
/*
// ROOT file //
//	m_sfile = new TFile("spr_ntp.root", "RECREATE");

	m_ntuple = new TTree("ntuple", "NTUPLE FOR DEBUGGIG");
	m_ntuple->Branch("StationName",&hit_st_name,"hit_st_name/I");
	m_ntuple->Branch("Eta",&hit_eta,"hit_eta/I");	
	m_ntuple->Branch("Phi",&hit_phi,"hit_phi/I");
	m_ntuple->Branch("Layer",&hit_layer,"hit_layer/I");
	m_ntuple->Branch("Direction",&hit_track_direction,"hit_track_direction/D");
	m_ntuple->Branch("NumberOfHits",&hit_number_hits,"hit_number_hits/I");	
	m_ntuple->Branch("DriftTime",&hit_drift_time,"hit_drift_time/D");
	m_ntuple->Branch("SignedDistance",&hit_signed_distance,"hit_signed_distance/D");
	m_ntuple->Branch("DriftRadius",&hit_drift_radius,"hit_drift_radius/D");
	m_ntuple->Branch("SigmaDriftRadius",&hit_sigma_drift_radius,"hit_sigma_drift_radius/D");	
	m_ntuple->Branch("SigmaDistanceToTrack",&hit_sigma_distance_to_track,"hit_sigma_distance_to_track/D");
	m_ntuple->Branch("RadialResidual",&hit_radial_residual,"hit_radial_residual/D");
*/
	m_is_initialized=true;	
	setRegion();
	return StatusCode::SUCCESS;
}

//******************************************************************************

//////////////////////
// METHOD setRegion //
//////////////////////


void NtupleSimpleResolutionTool :: setRegion()
	{
	
	m_rt_relation = m_calib_input_svc->GetRtRelation();
	if(m_rt_relation == NULL)
		{
		ATH_MSG_FATAL( "Cannot find rt-relatino for this region" );
		return;
		}
	}



//*****************************************************************************

///////////////////////
// METHOD destruct //
///////////////////////

void NtupleSimpleResolutionTool::destruct(void) {

	if(!m_is_initialized) return;

	return;
}

//*********************************************************************

////////////////////////////
// METHOD analyseSegments //
////////////////////////////
StatusCode NtupleSimpleResolutionTool::analyseSegments(const std::vector<MuonCalibSegment *> & segments)
{
if(segments.size() == 0)
	{
	ATH_MSG_FATAL( "No Segments collected!" );
	return StatusCode :: FAILURE;
	}

for(unsigned int i=0; i<segments.size(); i++) {
	const MuonCalibSegment *seg=segments[i];
	
	if (seg->mdtHitsOnTrack()<m_MINNUMHITS) continue;
		
	bool seg_rej = false;
	for (unsigned int l=0;l<seg->mdtHitsOnTrack();l++) {
		if (fabs(seg->mdtHOT()[l]->driftRadius())>15.0 ||
			fabs(seg->mdtHOT()[l]->driftRadius())<0.0) {
			seg_rej = true;
			continue;
		}
	}
	if(seg_rej) continue;

	for (unsigned int l=0;l<seg->mdtHitsOnTrack();l++) {
/*
// Ntuple variables (for debugging)
		MuonFixedId my_id(seg->mdtHOT()[l]->identify());
		hit_st_name = my_id.stationName();
		hit_eta = my_id.eta();
		hit_phi = my_id.phi();

		hit_track_direction=atan((seg->direction()[1])/
		(seg->direction()[2]));

		hit_drift_time = seg->mdtHOT()[l]->driftTime();
		hit_drift_radius = fabs(seg->mdtHOT()[l]->driftRadius());
		hit_signed_distance = seg->mdtHOT()[l]->signedDistanceToTrack();
		hit_sigma_drift_radius = seg->mdtHOT()[l]->sigmaDriftRadius();
		hit_sigma_distance_to_track = seg->mdtHOT()[l]->sigmaDistanceToTrack();
		hit_radial_residual = seg->mdtHOT()[l]->radialResidual();						
		m_ntuple->Fill();
*/
		if(abs(seg->mdtHOT()[l]->radialResidual())<0.5*RESRANGE) {
			m_residual2->Fill(fabs(seg->mdtHOT()[l]->driftRadius()),
			seg->mdtHOT()[l]->radialResidual(), 1.0);
		}

// replace initial space resolution with flat 0.3
		if(m_replace_with_flat) {
			seg->mdtHOT()[l]->setDriftRadius(fabs(seg->mdtHOT()[l]->driftRadius()),0.3);
		}

   }
	m_number_of_segments = m_refitted_segment.size();
	m_refitted_segment.push_back(new MuonCalibSegment(*(seg)));
	}
	end_of_data(m_rt_relation);

	return StatusCode :: SUCCESS;
}

//********************************************************

///////////////////////////
// METHOD handle_segment //
///////////////////////////

void NtupleSimpleResolutionTool::handle_segment(MuonCalibSegment &seg, double *par_lim_low, 
double *par_lim_upp) {

	if(seg.mdtHitsOnTrack()<m_MINNUMHITS || seg.mdtHitsOnTrack()>MAXNUMHITS) return;
	bool seg_rej = false;
	for (unsigned int l=0;	l<seg.mdtHitsOnTrack();l++) {
		if (fabs(seg.mdtHOT()[l]->driftRadius())>15.0 ||
			fabs(seg.mdtHOT()[l]->driftRadius())<0.0) {
			seg_rej = true;
			continue;
		}
	}
	if(seg_rej) return;

	double dir = (seg.direction ()[1])/(seg.direction ()[2]);
	for (unsigned int l=0; l<seg.mdtHitsOnTrack();l++) {
		double z_exhit, y_exhit, dtty; // Locale coordinates of excluded hit
		dtty=(seg.position ()[1]+dir*(seg.mdtHOT()[l]->localPosition ()[2])-
		seg.mdtHOT()[l]->localPosition ()[1])/(1.0+pow(dir,2));
		y_exhit=seg.mdtHOT()[l]->localPosition ()[1]+dtty;
		z_exhit=seg.mdtHOT()[l]->localPosition ()[2]-dtty*dir;

		double d_hit[MAXNUMHITS];
		double s2_hit[MAXNUMHITS];
		double delta_hit[MAXNUMHITS];
		bzero(d_hit, MAXNUMHITS*sizeof(double));
		bzero(s2_hit, MAXNUMHITS*sizeof(double));
		bzero(delta_hit, MAXNUMHITS*sizeof(double));

		for (unsigned int i=0;i<seg.mdtHitsOnTrack();i++) {
			if(i!=l) {
				double z_hit, y_hit; // Local coordinates of hits used in refit
				dtty=(seg.position ()[1]+dir*(seg.mdtHOT()[i]->localPosition ()[2])-
				seg.mdtHOT()[i]->localPosition ()[1])/(1.0+pow(dir,2));
				y_hit=seg.mdtHOT()[i]->localPosition ()[1]+dtty;
				z_hit=seg.mdtHOT()[i]->localPosition ()[2]-dtty*dir;

				d_hit[i]=sqrt((z_hit-z_exhit)*(z_hit-z_exhit)+(y_hit-y_exhit)*(y_hit-y_exhit))*
				((z_hit-z_exhit)/abs(z_hit-z_exhit));
				s2_hit[i]=seg.mdtHOT()[i]->sigma2DriftRadius();
			}
			else {
				d_hit[i] = 0.0;
				s2_hit[i] = 100.0; // Exclusion of a hit under consideration from a track fit
			}
			double dtt=seg.mdtHOT()[i]->signedDistanceToTrack();
			double drad=fabs(seg.mdtHOT()[i]->driftRadius());
			delta_hit[i]=(drad-abs(dtt))*dtt/abs(dtt);
			// Exclusion of hits with huge residuals from a track fit
			if(fabs(seg.mdtHOT()[i]->radialResidual())>1.0) s2_hit[i] = 100.0;
		}

		double dr_radius(0), dist_to_track(0), rad_residual(0);
		double trk_sigma(0), dist_diff(0);
		dr_radius = fabs(seg.mdtHOT()[l]->driftRadius());
		dist_to_track = seg.mdtHOT()[l]->signedDistanceToTrack();

// Track refit (hit under consideration is excluded)

		if(trackFit(seg.mdtHitsOnTrack(), d_hit,
		delta_hit, s2_hit, dist_diff, trk_sigma)) continue;

		dist_to_track = dist_to_track + dist_diff;
		rad_residual = dr_radius - abs(dist_to_track);
		double dist = abs(dist_to_track);

		double lim_low(0), lim_upp(0);
		for(int i=0;i<6;i++) {
			lim_low += par_lim_low[i]*pow(dist,i);
			lim_upp += par_lim_upp[i]*pow(dist,i);
		}

		if(trk_sigma<SRANGE&&dist<RRANGE&&dr_radius>0&&dr_radius<RRANGE) {
			m_sigma_track->Fill(dist, trk_sigma, 1.0);
			if(abs(rad_residual)<0.5*RESRANGE) {
				m_residual3->Fill(dr_radius, rad_residual, 1.0);
			}
			if(trk_sigma>lim_low&&trk_sigma<lim_upp){
				m_sigma_track_lim->Fill(dist, trk_sigma, 1.0);
				int nbin = int (RBIN*dist/RRANGE);
				if(rad_residual>m_low[nbin] && rad_residual<m_top[nbin]) {
					m_residual->Fill(dr_radius, rad_residual, 1.0);
					m_residual0->Fill(dist, rad_residual, 1.0);
				}
			}
		}
		
	}

	return;	
}

//*****************************************************************************

////////////////////////
// METHOD end_of_data //
////////////////////////

void NtupleSimpleResolutionTool::end_of_data(const IRtRelation * rt_rel) {

///////////////
// VARIABLES //
///////////////
  // double t_min(0.0), t_max(1000.0); // minimum and maximum drift time
	RtResolutionFromPoints res_from_points; // class to convert look-up
						// table for resolution
						// functions into any existing
						// format
	m_sprfun = new TF1("sprfun",ttt0,0.0,RRANGE,7);
	m_sprfun_r = new TF1("sprfun_r",ttt0,0.0,RRANGE,7);

//	TF1 *sprfun_old = new TF1("sprfun_old",ttt0,0.001,RRANGE,5);
	TF1 *sprfun_old = new TF1("sprfun_old",ttt0,0.001,RRANGE,7);

	double delta_conv = 100;
	double r_conv = 0;
	unsigned int iteration(0);

	for(int it=0; (delta_conv>m_DELTA_CONV)&&(it<m_MAX_NUM_IT); it++) {

	double par_low[6] = {0,0,0,0,0,0};
	double par_upp[6] = {SRANGE,0,0,0,0,0};
	
////////////////////////////////////////////////////
// First cycle by segments (TrkError calculation) //
////////////////////////////////////////////////////
	m_sigma_track->Reset();
	m_residual0->Reset();
	m_residual->Reset();

	for(unsigned int j=0;j<RBIN;j++) {
		m_low[j]=-0.5*RESRANGE;
		m_top[j]= 0.5*RESRANGE;
	}

	for (unsigned int k=0; k<m_refitted_segment.size(); k++) {

		handle_segment(*(m_refitted_segment[k]), par_low, par_upp);

	}

	trkErrorFix(par_low, par_upp);

	if(m_suppress_outliers) {
	for(unsigned int j=0;j<RBIN;j++) {
//		TH1D *proj_y = m_residual0->ProjectionY("_y", j+1,j+1);
		TH1D *proj_y = m_residual->ProjectionY("_y", j+1,j+1);
		Double_t num(0), sum_low(0), sum_top(0);

		for(unsigned int i=0;i<RESBIN;i++) {
			num += proj_y->GetBinContent(i+1);
		}
		for(int i=0;sum_low<m_SUPPRESS_LOW*num;i++) {
			sum_low += proj_y->GetBinContent(i+1);
			m_low[j]= -0.5*RESRANGE + RESRANGE*i/float(RESBIN);
		}
		for(int i=0;sum_top<(1-m_SUPPRESS_TOP)*num;i++) {
			sum_top += proj_y->GetBinContent(i+1);
			m_top[j]= -0.5*RESRANGE + RESRANGE*i/float(RESBIN);
		}
	}
	}

///////////////////////////////////////////////////////
// Second cycle by segments (Resolution calculation) //
///////////////////////////////////////////////////////
	m_sigma_track->Reset();
	m_sigma_track_lim->Reset();
	m_residual->Reset();
	m_residual0->Reset();
	m_residual3->Reset();

	for (unsigned int k=0; k<m_refitted_segment.size(); k++) {
		handle_segment(*(m_refitted_segment[k]), par_low, par_upp);				
	}

	spResCalc(true);
	
		for (unsigned int k=0; k<m_refitted_segment.size(); k++) {
			for(unsigned int l=0;l<m_refitted_segment[k]->mdtHitsOnTrack();l++) {
				double dr_radi = fabs(m_refitted_segment[k]->mdtHOT()[l]->driftRadius());
//				double sp_resol = m_sprfun->Eval(dr_radi);
				double sp_resol = m_sprfun_r->Eval(dr_radi);
				if(sp_resol < 0.050) sp_resol = 0.050;
				m_refitted_segment[k]->mdtHOT()[l]->setDriftRadius(dr_radi, sp_resol);
			}				
		}
	
		if(it>0) {
			delta_conv = 0;
			r_conv = 0;
			Double_t x[RBIN];
			for(unsigned int j=1;j<RBIN-2;j++) {
				x[j]=RRANGE*(j+0.5)/float(RBIN);
//				double diff = abs((m_sprfun->Eval(x[j]))-(sprfun_old->Eval(x[j])));
//				double d_diff = diff/abs(m_sprfun->Eval(x[j]));
				double diff = abs((m_sprfun_r->Eval(x[j]))-(sprfun_old->Eval(x[j])));
				double d_diff = diff/abs(m_sprfun_r->Eval(x[j]));
				if(d_diff>delta_conv && diff> m_DIFF_MAX) {
					delta_conv=d_diff;
					r_conv=x[j];
				}
			}
		}

		m_sprfun_r->Copy(*sprfun_old);
//		m_sprfun->Copy(*sprfun_old);
		iteration=it;
		ATH_MSG_INFO( "Iteration "<<iteration<<"  conv "<<delta_conv<<"  r_conv "<<r_conv << "\n" );

	}

// SpRes from DistToTrack calculation:
	spResCalc(false);

	ofstream outfile("spr.out", ios::app);
	outfile<<m_reg_sel_svc->GetRegionSelection()<<":  NumIt-s ="<<setw(3)<<iteration<<
		"  conv ="<<setw(6)<<setprecision(2)<<delta_conv<<
		"  r_conv ="<<setw(6)<<setprecision(2)<<r_conv<<
		"  NumOfSegments ="<<setw(6)<<m_refitted_segment.size()<<endl;
	outfile.close();

	delete sprfun_old;

	ATH_MSG_INFO( m_reg_sel_svc->GetRegionSelection()<<"  NumOfSegments:  "
	<<m_refitted_segment.size() );
	ATH_MSG_INFO( "NumOfIts "<<iteration<<"  convergence "<<delta_conv<<"  r_conv "<<r_conv );

///////////////////////////////////// writing output
//	m_sfile->Write();

	m_outfile = new TFile(m_spr_out_name.c_str(), "RECREATE");
	m_sprfun_r->Write();
	m_sprgr_r->Write();
	m_sigma_track->Write();
	m_sigma_track_lim->Write();
	m_residual->Write();
	m_residual0->Write();
	m_residual2->Write();

	m_outfile->Close();
	if (rt_rel==NULL)
		{
		return;
		}
	
/////////////////////////////////////////////////////////////////////////////
// CONVERT THE RESOLUTION FUNCTION INTO A RESOLUTION FUNCTION DEPENDING ON //
// TIME.                                                                   //
/////////////////////////////////////////////////////////////////////////////

// get minimum and maximum drift time //
	// const RtChebyshev *rt_Chebyshev(
	//			dynamic_cast<const RtChebyshev *>(rt_rel));
	// const RtRelationLookUp *rt_LookUp(
	//			dynamic_cast<const RtRelationLookUp *>(rt_rel));
   // RtChebyshev //
	// if (rt_Chebyshev!=0) {
	//	t_min = rt_Chebyshev->tLower();
	//	t_max = rt_Chebyshev->tUpper();
	// }

   // RtRelationLookUp, dangerous implementation, but the only way right now //
	// if (rt_LookUp!=0) {
	//	t_min = rt_LookUp->par(0);
	//	t_max = rt_LookUp->par(1)*(rt_LookUp->nPar()-2);
	// }

// perform sigma-t fit //
	vector<SamplePoint> point(31);
	double r_min(rt_rel->radius(rt_rel->tLower()));
	double r_max(rt_rel->radius(rt_rel->tUpper()));
	double bin_width((r_max-r_min)/static_cast<double>(point.size()-1));

	for (unsigned int k=0; k<point.size(); k++) {
//		double sp_res_out = m_sprfun->Eval(r_min+k*bin_width);
		double sp_res_out = m_sprfun_r->Eval(r_min+k*bin_width);
		if(sp_res_out<0.050) sp_res_out = 0.050;
		point[k].set_x2(sp_res_out);
		point[k].set_error(sp_res_out);
		if (k==0) {
			point[k].set_x1(rt_rel->tLower()-0.2);
		} else {
			point[k].set_x1(t_from_r(r_min+k*bin_width, rt_rel));
		}
		if (k==point.size()-1) {
			point[k].set_x1(rt_rel->tUpper()+0.2);
		}
	}

	m_final_resolution = new RtResolutionChebyshev(
				(res_from_points.getRtResolutionChebyshev(
								point, 8)));

	return;
}
//***********************************************************

//////////////////////////////////////
//  trkErrorFix                     //
//  TrackError region selection     //
//////////////////////////////////////

int NtupleSimpleResolutionTool::trkErrorFix(double *par_l, double *par_u) {

	Double_t x[RBIN], x_err[RBIN], y_low[RBIN],y_top[RBIN], y_err[RBIN];
	TF1 *func1 = new TF1("fitf1","pol5");
	TF1 *func2 = new TF1("fitf2","pol5");
	
	for(unsigned int j=0;j<RBIN;j++) {
		TH1D *proj_y = m_sigma_track->ProjectionY("_y", j+1,j+1);
		Double_t num(0), sum_low(0), sum_top(0);
		float thr_low(0),thr_top(0);

		for(unsigned int i=0;i<SBIN;i++) {
			num += proj_y->GetBinContent(i+1);
		}
		for(int i=0;sum_low<m_REJ_LOW*num;i++) {
			sum_low += proj_y->GetBinContent(i+1);
			thr_low=SRANGE*i/float(SBIN);
		}
		for(int i=0;sum_top<m_REJ_TOP*num;i++) {
			sum_top += proj_y->GetBinContent(i+1);
			thr_top=SRANGE*i/float(SBIN);
		}
		
		if(abs(thr_top-thr_low)<0.006) {
			thr_low=0.5*(thr_low+thr_top)-0.003;
			thr_top=0.5*(thr_low+thr_top)+0.003;
		}

		y_err[j]=thr_top-thr_low;

		x[j]=RRANGE*(j+0.5)/float(RBIN);
		x_err[j]=(RRANGE/RBIN)/sqrt(12.0);
		
		y_low[j]=thr_low;
		y_top[j]=thr_top;

	}

	TGraphErrors *gr1 = new TGraphErrors (RBIN,x,y_low,x_err,y_err);
	gr1->Fit(func1,"Q","",0.001*RRANGE,0.97*RRANGE);

	TGraphErrors *gr2 = new TGraphErrors (RBIN,x,y_top,x_err,y_err);
	gr2->Fit(func2,"Q","",0.001*RRANGE,0.97*RRANGE);
	
	func1->GetParameters (par_l);
	func2->GetParameters (par_u);

	delete gr1;
	delete gr2;
	delete func1;
	delete func2;

	return 0;
}

//***********************************************************************

//////////////////////////////////////
//  spResCalc                       //
//  Spatial resolution calculation  //
//////////////////////////////////////

int NtupleSimpleResolutionTool::spResCalc(bool SfromR) {

	Double_t sigma_dr[RBIN], sigma_dr_err[RBIN];
	Double_t x[RBIN], x_err[RBIN];

	for(unsigned int j=0;j<RBIN;j++) {
		sigma_dr[j]=0.0;
		sigma_dr_err[j]=0.0;
		x[j]=RRANGE*(j+0.5)/float(RBIN);
		x_err[j]=(RRANGE/RBIN)/sqrt(12.0);
	}
	for(unsigned int j=0;j<RBIN-1;j++) {
		
		TH1D *dr_y = 0;
		if(SfromR) dr_y = m_residual->ProjectionY("dry_y", j+1,j+1);
		else dr_y = m_residual0->ProjectionY("dry_y", j+1,j+1);
		double rms_f = 0;
		double sigma_tr, sigma_r, s_dr, s_tr;
		Double_t entr = 0;

		if(m_hist_fit_method) {

			for(unsigned int k=0;k<RESBIN;k++) {
//				float bin_err = sqrt(dr_y->GetBinContent(k+1));
//				if(bin_err<1.0) bin_err = 1.0;
//				dr_y->SetBinError(k+1,bin_err);
				dr_y->SetBinError(k+1,1.0);
			}

			dr_y->Fit("gaus","Q","", -1.0, 1.0);

			rms_f = dr_y->GetFunction("gaus")->GetParameter(2);
			if(rms_f<0.0001) rms_f=0.0001;

			sigma_tr = m_sigma_track_lim->ProfileX()->GetBinContent(j+1);
			sigma_r = rms_f*rms_f-sigma_tr*sigma_tr;
			if(sigma_r<0.0001) sigma_r=0.0001;
			sigma_r=sqrt(sigma_r);

			s_dr = dr_y->GetFunction("gaus")->GetParError(2);
			s_dr = sqrt(s_dr*s_dr + 0.01*0.01);
			s_tr = m_sigma_track_lim->ProjectionY("str_y", j+1,j+1)->GetRMS();

		} else {

			for(unsigned int k=0;k<RESBIN;k++) {
				float x_x =(float(k)+0.5-0.5*RESBIN)*(RESRANGE/RESBIN);
				rms_f += x_x*x_x*(dr_y->GetBinContent(k+1));
				entr += dr_y->GetBinContent(k+1);
			}
			if(entr==0||rms_f==0) rms_f=0.0001;
			else rms_f = sqrt(rms_f/entr);
				
			float xx_cut = 5*rms_f;		

			rms_f=0;
			entr = 0;
			for(unsigned int k=0;k<RESBIN;k++) {
				float x_x =(float(k)+0.5-0.5*RESBIN)*(RESRANGE/RESBIN);
				if(abs(x_x)<xx_cut) {
					rms_f += x_x*x_x*(dr_y->GetBinContent(k+1));
					entr += dr_y->GetBinContent(k+1);
				}
			}
		
			if(entr==0||rms_f==0) rms_f=0.0001;
			else rms_f = sqrt(rms_f/entr);
			sigma_tr = m_sigma_track_lim->ProfileX()->GetBinContent(j+1);
			sigma_r = rms_f*rms_f-sigma_tr*sigma_tr;
			if(sigma_r<0.0001) sigma_r=0.0001;
			sigma_r=sqrt(sigma_r);

			s_dr = dr_y->GetRMSError();
			if(s_dr<0.00001) s_dr=0.5;
			s_tr = m_sigma_track_lim->ProjectionY("str_y", j+1,j+1)->GetRMS();

		}

		sigma_dr[j] = sigma_r;
		sigma_dr_err[j] = sqrt(s_dr*s_dr+s_tr*s_tr);

		if(sigma_dr[j]<=0.050) sigma_dr[j]=0.050;
	}
	if(SfromR) {
		if(m_sprgr_r) delete m_sprgr_r;
		m_sprgr_r = new TGraphErrors (RBIN,x,sigma_dr,x_err,sigma_dr_err);
		m_sprgr_r->SetName("sprgr_r");

		m_sprfun_r->SetParameter(0,0.3);
		m_sprfun_r->SetParLimits(0,0.1,2.0);
		float rlim = 1.0/RRANGE;
		m_sprfun_r->SetParameter(1,0.0);
		m_sprfun_r->SetParLimits(1,0.0,2.0*rlim);
		rlim = rlim/RRANGE;
		m_sprfun_r->SetParameter(2,0.0);
		m_sprfun_r->SetParLimits(2,-10.0*rlim,10.0*rlim);
		rlim = rlim/RRANGE;
		m_sprfun_r->SetParameter(3,0.0);
		m_sprfun_r->SetParLimits(3,-10.0*rlim,10.0*rlim);
		m_sprfun_r->SetParameter(4,4.0);
		m_sprfun_r->SetParLimits(4,1.0,10.0);
		m_sprfun_r->SetParameter(5,0.1);
		m_sprfun_r->SetParLimits(5,0.01,2.0);
		m_sprfun_r->SetParameter(6,3.0);
		m_sprfun_r->SetParLimits(6,0.0,6.0);

		m_sprgr_r->Fit(m_sprfun_r,"Q","0",RRANGE*1.0/30.0,RRANGE*29.0/30.0);
	}
	else {
		if(m_sprgr) delete m_sprgr;
		m_sprgr = new TGraphErrors (RBIN,x,sigma_dr,x_err,sigma_dr_err);
		m_sprgr->SetName("sprgr");

		m_sprfun->SetParameter(0,0.3);
		m_sprfun->SetParLimits(0,0.1,2.0);
		float rlim = 1.0/RRANGE;
		m_sprfun->SetParameter(1,0.0);
		m_sprfun->SetParLimits(1,0.0,2.0*rlim);
		rlim = rlim/RRANGE;
		m_sprfun->SetParameter(2,0.0);
		m_sprfun->SetParLimits(2,-10.0*rlim,10.0*rlim);
		rlim = rlim/RRANGE;
		m_sprfun->SetParameter(3,0.0);
		m_sprfun->SetParLimits(3,-10.0*rlim,10.0*rlim);
		m_sprfun->SetParameter(4,4.0);
		m_sprfun->SetParLimits(4,1.0,10.0);
		m_sprfun->SetParameter(5,0.1);
		m_sprfun->SetParLimits(5,0.01,2.0);
		m_sprfun->SetParameter(6,3.0);
		m_sprfun->SetParLimits(6,0.0,6.0);

		m_sprgr->Fit(m_sprfun,"Q","0",RRANGE*1.0/30.0,RRANGE*29.0/30.0);
		}

	return 0;
}



//************************************************************

/////////////////////
// METHOD t_from_r //
/////////////////////

double NtupleSimpleResolutionTool::t_from_r(const double & r,
						const IRtRelation * rt_rel) {

///////////////
// VARIABLES //
///////////////

	double precision(0.010); // spatial precision of the inversion 
	double t_max(0.0); // upper time search limit
	double t_min(1000.0); // lower time search limit

///////////////////////////////
// DETERMINE t_min and t_max //
///////////////////////////////

	const RtChebyshev *rt_Chebyshev(
				dynamic_cast<const RtChebyshev *>(rt_rel));
	const RtRelationLookUp *rt_LookUp(
				dynamic_cast<const RtRelationLookUp *>(rt_rel));
   // RtChebyshev //
	if (rt_Chebyshev!=0) {
		t_min = rt_Chebyshev->tLower();
		t_max = rt_Chebyshev->tUpper();
	}

   // RtRelationLookUp, dangerous implementation, but the only way right now //
	if (rt_LookUp!=0) {
		t_min = rt_LookUp->par(0);
		t_max = rt_LookUp->par(1)*(rt_LookUp->nPar()-2);
	}

/////////////////////////////////////////////
// SEARCH FOR THE CORRESPONDING DRIFT TIME //
/////////////////////////////////////////////

	while (t_max-t_min>0.1 &&
			fabs(rt_rel->radius(0.5*(t_min+t_max))-r)>precision) {

		if (rt_rel->radius(0.5*(t_min+t_max))>r) {
			t_max = 0.5*(t_min+t_max);
		} else {
			t_min = 0.5*(t_min+t_max);
		}

	}

	return 0.5*(t_min+t_max);

}

//************************************************************

//////////////////////////////////////////
// Functions for root histogram fitting //
//////////////////////////////////////////

double NtupleSimpleResolutionTool::ttt(double *x, double *par) {
	double y;

	y=0.025+x[0]*(par[4]*exp(-x[0]/
	(par[0]*(1+par[1]*x[0]+par[2]*x[0]*x[0]+par[3]*x[0]*x[0]*x[0])))+
	par[5]*exp(-x[0]*x[0]/(par[6]*par[6])));
	return y;
}

double NtupleSimpleResolutionTool::ttt0(double *x, double *par) {
	double y;
//	y=exp(-x[0]/(par[4]+x[0]))*par[0]*(1.0+par[1]*x[0]+par[2]*x[0]*x[0]+
//	par[3]*x[0]*x[0]*x[0]);

	y=(par[0]-(par[0]-par[5])/(1.0+exp(-(x[0]-par[6])/par[4])))*
	(1+par[1]*x[0]+par[2]*x[0]*x[0]+par[3]*x[0]*x[0]*x[0]);

	return y;
}

//************************************************************

/////////////////////////////////////////////////////////////////////
// Track Fitting Function. Calculates position of a track (shift)  //
//and track error (sigma_track) at the point under consideration   //
/////////////////////////////////////////////////////////////////////

int NtupleSimpleResolutionTool::trackFit(int num_hit, double *dist_hit, double *y,
	double *sigma2_hit, double &shift, double &sigma_track) {

	int NLC=2;
	if(m_curved) NLC=3;
	CLHEP::HepMatrix Gamma = CLHEP::HepMatrix(NLC,NLC,0);
	CLHEP::HepVector betha = CLHEP::HepVector(NLC,0);
	CLHEP::HepVector alpha = CLHEP::HepVector(NLC,0);
	CLHEP::HepVector delta = CLHEP::HepVector(NLC,0);

	for(int k=0;k<num_hit;k++) {
		if(sigma2_hit[k]<=0) return 1;
		for(int i=0;i<NLC;i++) delta[i] = pow(dist_hit[k],i);
		Gamma += (delta*delta.T())/sigma2_hit[k];
		betha += y[k]*delta/sigma2_hit[k];
	}

	int ierr(0); // Status of Matrix Inversion
	Gamma.invert(ierr);
	if(ierr) return 2;
	alpha = Gamma*betha;

	if(std::isnan(alpha[0]) || std::isnan(Gamma[0][0])) return 2;
	if(Gamma[0][0]<=0) return 2;
	shift = alpha[0];
	sigma_track = sqrt(Gamma[0][0]);

	return 0;
}

}

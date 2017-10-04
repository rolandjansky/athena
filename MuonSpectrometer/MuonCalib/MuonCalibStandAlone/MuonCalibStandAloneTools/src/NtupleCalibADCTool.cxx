/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 14.04.2009, AUTHOR:  Zhen YAN
// Use for ADC parameters studies. 
// 
// 2009.09.02  v510  1) Study double hits ( second peak in TDC spectrum ) 
//                   2) Get the ADC normalize parameter and ADC correction factors
//                   3) validate the Residual_vs_Radius by applying ADC correction           
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF THE METHODS DEFINED IN THE CLASS ::
//::           NtupleCalibADCTool              ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// standard C++ //
#include <iostream>
#include <fstream>

// CLHEP //
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

// MuonGeoModel //
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

// MuonCalib //
#include "MdtCalibUtils/GlobalTimeFitter.h"
#include "MdtCalibFitters/DCSLFitter.h"
// 
#include "MuonCalibStandAloneTools/NtupleCalibADCTool.h"
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MuonCalibRawRpcHit.h"
#include "MuonCalibStl/ToString.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MdtCalibT0/T0MTHistos.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"
//#include "MuonCalibData/MdtTubeFitContainer.h"      // SingleTubeCalib*
//root;
#include "TF1.h"
#include "TTree.h"
#include "TProfile.h"
#include <stdio.h>
//#include "TMath.h"

//::::::::::::::::::::::::
//:: NAMESPACE SETTINGS ::
//::::::::::::::::::::::::

using namespace std;
using namespace MuonCalib;

//*****************************************************************************

//:::::::::::::::::
//:: CONSTRUCTOR ::
//:::::::::::::::::

NtupleCalibADCTool::NtupleCalibADCTool(const std::string & t, 
	const std::string & n, const IInterface * p) : AthAlgTool(t, n, p),
	m_reg_sel_svc("RegionSelectionSvc", n), m_input_service("MdtCalibInputSvc", n) {

/////////////////
// JOB OPTIONS //
/////////////////

	declareProperty("RegionSelectionSvc", m_reg_sel_svc);
	declareProperty("MdtCalibInputSvc", m_input_service);

	m_refit_segments = true ; // by default refit the segments
    declareProperty("segmentRefit", m_refit_segments);

        m_Recalc2ndCoordinate = false ;  // check this flag to dicide if ADC_vs_DistanceToReadout can be use or not
    declareProperty("Recalc2ndCoordinate", m_Recalc2ndCoordinate);

	m_curved_fits = false ; // by default straight refits of the segments
    declareProperty("curvedSegmentRefit", m_curved_fits);

	m_time_out = 2.0; // by default 2 s time-out for pattern finding
    declareProperty("timeOut", m_time_out);

	m_road_width = 3.0; // by default road width = 1 mm
	declareProperty("roadWidth", m_road_width);

	m_MDT_ID_helper = string("MDTIDHELPER");
	declareProperty("MDTIdHelper", m_MDT_ID_helper);

	m_RPC_ID_helper = string("RPCIDHELPER");
	declareProperty("RPCIdHelper", m_RPC_ID_helper);

	m_idToFixedIdToolType = string("MuonCalib::IdToFixedIdTool");
	declareProperty("idToFixedIdToolType", m_idToFixedIdToolType);

	m_idToFixedIdToolName = string("MuonCalib_IdToFixedIdTool");
	declareProperty("idToFixedIdToolName", m_idToFixedIdToolName);

	m_ROOT_file_name = string("NtupleCalibADCTool.root");
	declareProperty("ROOTFileName", m_ROOT_file_name);
	
//	m_group_by = string("TUBE");
//	declareProperty("GroupBy", m_group_by);

/////////////////////////////
// RESET PRIVATE VARIABLES //
/////////////////////////////
      m_MDT_adc_vs_radius.clear();
      m_MDT_adc_vs_pathD.clear();
      m_MDT_adc_vs_residual.clear();
      m_MDT_adc_vs_posX.clear();
      m_MDT_adc_vs_DistanceToSegmentTrack.clear();
      m_MDT_radius_vs_DistanceToSegmentTrack.clear();
      m_MDT_radius_vs_t.clear();
      m_MDT_segment_localangle_YZ.clear();	//local incidence angle XZ, YZ distributions of the segments
      m_MDT_segment_localangle_XZ.clear();
   //   mc_MDT_adc_vs_radius.clear();
     // m_MDT_adc_vs_tdc.clear();
     // m_MDT_adc_vs_t.clear();
     // m_MDT_posX_vs_radius.clear();        	
     // m_MDT_Revadc_vs_residual.clear(); 
     // m_MDT_radius_vs_residual.clear(); 
     // m_MDT_posX_vs_residual.clear(); 
     // m_MDT_pathD_vs_residual.clear(); 
 
   //   m_MDT_adc_vs_segment_localangle_YZ.clear();	//local incidence angle XZ, YZ distributions of the segments
   //   m_MDT_adc_vs_segment_localangle_XZ.clear();
   //   m_MDT_adc_vs_segment_globalangle_YZ.clear(); //global incidence angle XZ, YZ distributions of the segments
   //   m_MDT_adc_vs_segment_globalangle_XZ.clear();         

      // v410
for(int i=0;i<3;++i) 
{
   	m_MDT_residual_vs_radius[i].clear(); 
        m_MDT_segment_hits[i].clear();
        m_MDT_segment_chi2[i].clear();
}
     m_MDT_segment_delta_chi2.clear();
     //   m_MDT_residual_vs_radius_NoRefit.clear(); 
     //   m_MDT_residual_vs_radius_Refit.clear(); 
     //   m_MDT_residual_vs_radius_TimeSlewing.clear(); 
      for(int i=0;i<15;++i) 
{
	m_r_MDT_residual_vs_adc[i].clear(); 
}

/*for(int i=0;i<6;++i) 
                        {
//	a_MDT_adc_vs_posX[i].clear(); 
//	a_MDT_adc_vs_residual[i].clear(); 
	a_MDT_adc_vs_radius_XZ[i].clear(); 
	a_MDT_adc_vs_radius_YZ[i].clear(); 
                            }	
*/
m_MDT_fhit_adc.clear();      
m_MDT_shit_adc.clear();      

      //  Other parameter
        m_nb_events = 0;
	m_MdtIdHelper = 0;
	m_RpcIdHelper = 0;
	m_detMgr = 0;
	m_id_tool = 0;
	
	m_qfitter = new QuasianalyticLineReconstruction();
	m_cfitter = new CurvedPatRec();
	m_t0_refinement = new T0Refinement();

	declareInterface< NtupleCalibrationTool >(this);	

}

//*****************************************************************************

//::::::::::::::::
//:: DESTRUCTOR ::
//::::::::::::::::

NtupleCalibADCTool::~NtupleCalibADCTool(void) {

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD initialize ::
//:::::::::::::::::::::::

StatusCode NtupleCalibADCTool::initialize(void) {

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

	/*
//	m_tsfile = new TFile("TS_cor.txt","READ");
	ifstream cor_db("TS_cor.txt"); 
	if(!cor_db) cout<<" Can't find the timeslewing constants from current dir "<<endl;
        string tempstring; 
        while (getline(cor_db,tempstring))   {
        istringstream sp1line(tempstring);
        for(int i=0;i<16;++i)      {
        cout<<"Radius Interval : "<<i<<" cor "<<cor[i]<<"\t"<<ADC0[i]<<"\t"<<low_bin[i]<<"\t"<<high_bin[i];
        sp1line>>cor[i]>>ADC0[i]>>low_bin[i]>>high_bin[i];
                                   }
	                                     }
	.delete();
cor[15] = {-0.00205705,-0.00354407,-0.00306112,-0.00193288,-0.00109093,-0.000831173,-0.00066971,-0.000611956,-0.000610348,-0.00048119,-0.00045121,-0.000412511,-0.000346831,-0.00027503,0.000124506 };
ADC0[15] = {-49.9848,131.757,168.377,180.345,168.991,174.58,209.455,230.637,214.304,200.673,200.648,199.065,163.635,161.754,89.7234 };
high_bin[15] = {241.69,242.37,260.751,270.535,275.781,276.369,272.933,269.351,261.99,258.231,251.039,242.985,235.595,226.821,219.6 };
low_bin[15] = {48.8971,53.4205,63.7885,74.7095,79.9586,81.4944,81.2939,79.9401,79.2682,77.0589,75.6614,74.56,73.1008,71.8534,64.8691 };

	*/
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

//


	return StatusCode::SUCCESS;

}

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD finalize ::
//:::::::::::::::::::::

StatusCode NtupleCalibADCTool::finalize(void) {

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

StatusCode NtupleCalibADCTool::handleEvent(
			const MuonCalibEvent & event,
			int /*evnt_nb*/,
			const std::vector<MuonCalibSegment *> & segments,
			unsigned int position) {

///////////////
// VARIABLES //
/////////////// -0.00205705  for BML4A05 only
/*        double cor[15] = {0,-0.00354407,-0.00306112,-0.00193288,-0.00109093,-0.000831173,-0.00066971,-0.000611956,-0.000610348,-0.00048119,-0.00045121,-0.000412511,-0.000346831,-0.00027503,0.000124506 };
        double low_bin[15] = {48.8971,53.4205,63.7885,74.7095,79.9586,81.4944,81.2939,79.9401,79.2682,77.0589,75.6614,74.56,73.1008,71.8534,64.8691 };
        double high_bin[15] = {241.69,242.37,260.751,270.535,275.781,276.369,272.933,269.351,261.99,258.231,251.039,242.985,235.595,226.821,219.6 };
        double ADC0[15]={-49.9848,131.757,168.377,180.345,168.991,174.58,209.455,230.637,214.304,200.673,200.648,199.065,163.635,161.754,89.7234 };
*/
double cor[15] = {-0.00427636,-0.00594769,-0.00331729,-0.0026683,-0.00112401,-0.000616045,-0.000536907,-0.000297823,-4.62138e-005,0.000156364,0.000120936,-7.1578e-005,0.000259177,-0.000340251,5.11028e-005 } ;
double ADC0[15] = {73.7515,110.755,103.724,122.37,136.487,195.278,221.442,296.233,1432.72,-166.977,-182.321,577.875,-23.6606,195.662,-187.745 } ;
// ADCmean[15] = {125.011,135.959,149.799,156.222,159.417,158.154,156.026,153.048,149.845,146.659,142.953,138.728,134.282,130.304,120.176 } ;
double high_bin[15] = {197.886,212.367,226.685,232.567,236.377,233.817,230.829,225.595,221.07,216.012,210.1,203.221,195.506,188.455,178.88 } ;
double low_bin[15] = {52.1363,59.5508,72.9139,79.8775,82.4582,82.4905,81.2233,80.5007,78.6199,77.3049,75.8061,74.2353,73.0578,72.1533,61.4726 } ;
	
//	const MuonCalibRawHitCollection *raw_hits(event.rawHitCollection());
//	const MuonCalibSegment *rpcHits(event.rpcHitCollection());
//	unsigned int ml, ly, tb; // multilayer, layer, tube/
//	unsigned int ndof; // number of degrees of freedom of the segment fit
//	int strip, strip2; // rpc strip
//	double r, d; // drift radius and track distance from the wire
//	int mdt_close_layer, mdt_close_multi;
	IMdtPatRecFitter *segment_fitter(0); // pointer to the segment fitter
	IMdtPatRecFitter *segment_fitter2(0); // pointer to the segment fitter
	if (m_curved_fits) {
		segment_fitter = m_cfitter;
		segment_fitter2 = m_cfitter;
	} else {
		segment_fitter = m_qfitter;
		segment_fitter2 = m_qfitter;
	}
 // DCSLFitter * betterFitter = new DCSLFitter();
/*  GlobalTimeFitter * GTFitter = new GlobalTimeFitter(betterFitter) ;
  if ( m_refit_segments ) {
     IRtRelation * defaultRt = GTFitter->getDefaultRtRelation() ;
     GTFitter->setRtRelation(defaultRt) ;
                          } 
*/

////////////////////
// RESET COUNTERS //
////////////////////
//setEvent(event);
// cor[0] = {0} ;-0.00205705

        
	
//////////////////////////////
// FILL RAW DATA HISTOGRAMS //
//////////////////////////////
//
//
//
//Loop RAW data -> hitselection -> udpate segment by ADC correction factor
//Increase hitsOnTracks?
//v500 will
//

///////////////////////////////
// Validate the Segment hits //
///////////////////////////////	

// loop over the raw hits //
	for (unsigned int k=position; k<segments.size(); k++) {

   // get the first hit to check if it is in the calibration region //
	if(!m_reg_sel_svc->isInRegion(segments[k]->mdtHOT()[0] ->identify())) continue;
	NtupleStationId station_identifier((segments[k]->mdtHOT()[0])->identify());
	station_identifier.SetMultilayer(0);

	createMaps((segments[k]->mdtHOT()[0])->identify());
//const MdtStationT0Container *t0(m_input_service->GetT0(station_identifier));
//        const IRtRelation *rt=m_input_service->GetRtRelation(station_identifier);
//	rt->t_from_r(r);

//////##############################################################################################
///////// step 1 check the MDT with

// segments selection 
//
// 1) segment hits in same chamber
// 2) segment hits no double hits 
//  bool NoDoubleHitsSegments = true;
//  int  Event_counter =0;
//  int  Segment_counter =0;
  
  const MuonCalibRawHitCollection *raw_hits(event.rawHitCollection());
 //      const MuonCalibSegment *rpcHits(event.rpcHitCollection());
  int ml, ly, tb; // multilayer, layer, tube

  for (MuonCalibRawHitCollection::MuonCalibRawMdtHitVecCit it=raw_hits->rawMdtHitCollectionBegin();
                                 it!=raw_hits->rawMdtHitCollectionEnd(); ++it) {
 
    // get the raw hit and check whether it is in the calibration region //
                 MuonCalibRawMdtHit *hit(*it);
                 
                 if(!m_reg_sel_svc->isInRegion(hit->identify())) continue;
                 		 ml = (hit->identify()).mdtMultilayer();
		                 ly = (hit->identify()).mdtTubeLayerIndex()+1;
		                 tb = (hit->identify()).mdtTubeIndex();
			//	 tdc0 = (hit->.tdcCount();
				//second loop 
	         //int counter = 0;
	//	 while(it!=raw_hits->rawMdtHitCollectionEnd())  {
		 for (MuonCalibRawHitCollection::MuonCalibRawMdtHitVecCit it2=raw_hits->rawMdtHitCollectionBegin();
                                 it2!=raw_hits->rawMdtHitCollectionEnd(); ++it2) {
                 MuonCalibRawMdtHit *hit2(*it2);
		 if(!m_reg_sel_svc->isInRegion(hit2->identify())) continue;
		 bool same_ml(ml==(hit2->identify()).mdtMultilayer());
		 bool same_ly(ly==(hit2->identify()).mdtTubeLayerIndex());
		 bool same_tb(tb==(hit2->identify()).mdtTubeIndex());
		 bool sameTubeHit=same_ml && same_ly && same_tb;
		 if(!sameTubeHit) continue;
		 if(hit2->tdcCount()==hit->tdcCount()) 	continue;
		 if(!((hit2->adcCount()>50)&&(hit->adcCount()>50))) continue;
		 if(hit2->tdcCount()>hit->tdcCount()) 	{
			 m_MDT_fhit_adc[station_identifier]->Fill(hit->tdcCount(),hit->adcCount(),1.0); 
			 m_MDT_shit_adc[station_identifier]->Fill(hit2->tdcCount(),hit2->adcCount(),1.0); 
			                                   	}
		   else 	{  
			 m_MDT_fhit_adc[station_identifier]->Fill(hit2->tdcCount(),hit2->adcCount(),1.0); 
			 m_MDT_shit_adc[station_identifier]->Fill(hit->tdcCount(),hit->adcCount(),1.0);
			      	} 
		   								}
		// counter++;
	//	 						}
		// if(counter>1) cout<<"Error, some tube was record over two times !!"<<endl;

  										} //end raw Mdthit loop


//////##############################################################################################
//     Selection segment
  
  if (m_refit_segments) {
        Double_t slopeYZ=57.30*atan((segments[k]->direction()).y()/(segments[k]->direction()).z());
	Double_t slopeXZ=57.30*atan((segments[k]->direction()).x()/(segments[k]->direction()).z());
//	Double_t gslopeYZ=57.30*atan((segments[k]->globalDirection()).y()/(segments[k]->globalDirection()).z());
//	Double_t gslopeXZ=57.30*atan((segments[k]->globalDirection()).x()/(segments[k]->globalDirection()).z());
	m_MDT_segment_localangle_YZ[station_identifier]->Fill(slopeYZ,1.0);
	m_MDT_segment_localangle_XZ[station_identifier]->Fill(slopeXZ,1.0);

     	for (unsigned int l=0; l<segment_fitter->numberOfTrackHits();l++) 	{
				Int_t adcCount = (segment_fitter->trackHits())[l]->adcCount();
				if(adcCount<50) continue;
				//Double_t distX=(segment_fitter->trackHits())[l]->distanceToReadout();
				//Int_t tdcCount = (segment_fitter->trackHits())[l]->tdcCount();
                                Double_t r = (segment_fitter->trackHits())[l]->driftRadius();
			        Double_t d = (segment_fitter->trackHits())[l]->signedDistanceToTrack();
			        Double_t t = (segment_fitter->trackHits())[l]->driftTime() ;
				Double_t distX=(segment_fitter->trackHits())[l]->distanceToReadout();
				if((fabs(d)>15.0)||(fabs(r)>15.0)) continue;
				m_MDT_radius_vs_DistanceToSegmentTrack[station_identifier]->Fill(d,r,1.0);
                                m_MDT_radius_vs_t[station_identifier]->Fill(t, r,1.0);
                                m_MDT_adc_vs_radius[station_identifier]->Fill(r, adcCount,1.0);
                                m_MDT_adc_vs_DistanceToSegmentTrack[station_identifier]->Fill(d, adcCount,1.0);	
				Double_t resi = fabs(r)-fabs(d);
				m_MDT_adc_vs_residual[station_identifier]->Fill(resi, adcCount,1.0);
           if((m_Recalc2ndCoordinate)&&(fabs(slopeXZ)<80.0)) 	{
		  		m_MDT_adc_vs_posX[station_identifier]->Fill(distX, adcCount,1.0);
		  		Double_t lenD=2.0*sqrt(225.0-d*d)/cos(fabs(slopeXZ)/57.30); 
				m_MDT_adc_vs_pathD[station_identifier]->Fill(lenD, adcCount,1.0); 

	   				}
										} 
  			}
  	else 	 	{      // aux line for no refit segment
	Double_t slopeYZ=57.30*atan((segments[k]->direction()).y()/(segments[k]->direction()).z());
	Double_t slopeXZ=57.30*atan((segments[k]->direction()).x()/(segments[k]->direction()).z());
	//Double_t gslopeYZ=57.30*atan((segments[k]->globalDirection()).y()/(segments[k]->globalDirection()).z());
//	Double_t gslopeXZ=57.30*atan((segments[k]->globalDirection()).x()/(segments[k]->globalDirection()).z());
	m_MDT_segment_localangle_YZ[station_identifier]->Fill(slopeYZ,1.0);
	m_MDT_segment_localangle_XZ[station_identifier]->Fill(slopeXZ,1.0);
      
			}

//////##############################################################################################

//	if(segment_fitter->fit(*(segments[k])))

// Loop all hits in segments[k] and update the driftRadius by ADC correction  v500
// only for the segments-chi2() < 5.0 segments
// Refit one more time to check if refit did same function
// next  hold a vector for compare the residual and radius
	if(segments[k]->chi2() > 5.0) continue; 
        if(segments[k]->hitsOnTrack() < 5 ) continue;
	m_MDT_segment_hits[0][station_identifier]->Fill(segments[k]->hitsOnTrack(),1.0);	
	m_MDT_segment_chi2[0][station_identifier]->Fill(segments[k]->chi2(),1.0);
	for (unsigned int l=0; l<segment_fitter->numberOfTrackHits();l++) 	{
	                        Double_t r0 = (segment_fitter->trackHits())[l]->driftRadius();
			        Double_t d0 = (segment_fitter->trackHits())[l]->signedDistanceToTrack();
	                	m_MDT_residual_vs_radius[0][station_identifier]->Fill(r0,fabs(r0)-fabs(d0),1.0);
										}
	 
	if( !(segment_fitter->fit(*(segments[k])) ) ) ATH_MSG_WARNING( "Segment fit failed. Just going on." );
	Double_t oldchi2 =  segments[k]->chi2() ;
        m_MDT_segment_hits[1][station_identifier]->Fill(segment_fitter->numberOfTrackHits(), 1.0);
	m_MDT_segment_chi2[1][station_identifier]->Fill(segments[k]->chi2(),1.0);
	for (unsigned int l=0; l<segment_fitter->numberOfTrackHits();l++) 	{
	                        Double_t r1 = (segment_fitter->trackHits())[l]->driftRadius();
			        Double_t d1 = (segment_fitter->trackHits())[l]->signedDistanceToTrack();
	                	m_MDT_residual_vs_radius[1][station_identifier]->Fill(r1,fabs(r1)-fabs(d1),1.0);
				Int_t radius=static_cast<int>(fabs(r1));
			        if(radius>14) radius = 14;	
				Int_t adcCount = (segment_fitter->trackHits())[l]->adcCount();
			        m_r_MDT_residual_vs_adc[radius][station_identifier]->Fill(adcCount,fabs(r1)-fabs(d1),1.0);  // remove the delta ray
										} 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool goodSeg = true;
	for (unsigned int l=0; l<segments[k]->hitsOnTrack();l++) {
		   Double_t oldr=(segments[k]->mdtHOT())[l]->driftRadius();
		   Double_t oldrsigma=(segments[k]->mdtHOT())[l]->sigmaDriftRadius();     
		   Int_t binr=static_cast<int>(oldr);
		   if(binr>14) break;
		   if((((segments[k]->mdtHOT())[l]->adcCount())>low_bin[binr])&&(((segments[k]->mdtHOT())[l]->adcCount())<high_bin[binr])) 	{
	           // if ADCcount inside the range, update the Radius by ADC value
		   Double_t dr=(((segments[k]->mdtHOT())[l]->adcCount())-ADC0[binr])*cor[binr];
                   Double_t newr=oldr-dr;
                   if(newr<0.0) newr = 0.0;
                   (segments[k]->mdtHOT())[l]->setDriftRadius(newr, oldrsigma);
	            																}
		  
		                                                  }

	if(goodSeg && (segment_fitter2->fit(*(segments[k]))))	{
// 2nd Segment Refit, see the residual improvement	
	m_MDT_segment_hits[2][station_identifier]->Fill(segment_fitter2->numberOfTrackHits(), 1.0);
	Double_t newchi2 =  segments[k]->chi2() ;
	m_MDT_segment_chi2[2][station_identifier]->Fill(newchi2,1.0); 
        m_MDT_segment_delta_chi2[station_identifier]->Fill(oldchi2 - newchi2 , 1.0); 	
        for (unsigned int l=0; l<segment_fitter2->numberOfTrackHits();l++) {
	                        Double_t r2 = (segment_fitter2->trackHits())[l]->driftRadius();
			        Double_t d2 = (segment_fitter2->trackHits())[l]->signedDistanceToTrack();
                                m_MDT_residual_vs_radius[2][station_identifier]->Fill(r2,fabs(r2)-fabs(d2),1.0);  
                                                                    }
                          }
	                 
				
				}  //end of segment loop 
	
	return StatusCode::SUCCESS;

}

	   




//*****************************************************************************

// statusCode NtupleCalibADCTool::analyseSegments(const std::vector<MuonCalibSegment *> & /*segments*/) {
// v600 start to analysis plot and output ADC coefficiency factor 
//
//
//
//
//
//
//:::::::::::::::::::::::::
//:: METHOD KeepSegments ::
//:::::::::::::::::::::::::

bool NtupleCalibADCTool::KeepSegments(void) const {

	return false;

}

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD createMaps ::
//:::::::::::::::::::::::

void NtupleCalibADCTool::createMaps(const MuonFixedId & id) {

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
	const MuonGM::MdtReadoutElement *MdtRoEl =m_detMgr->getMdtReadoutElement(m_MdtIdHelper->channelID(station_id,1,1,1));

	m_nb_ml[station_identifier] = m_MdtIdHelper->numberOfMultilayers(station_id);
	m_nb_ly[station_identifier] = MdtRoEl->getNLayers();
	m_nb_tb[station_identifier] = MdtRoEl->getNtubesperlayer();


///////////////////////////////
// CREATE THE REMAINING MAPS //
///////////////////////////////

// counter //

// histograms //
	ToString tostring;
	string file_dir(id.stationNumberToFixedStationString(id.stationName())
		+"_"+tostring(id.phi())+"_"+tostring(id.eta()));
   string tempside=(id.eta()>0)?"A":"C";
   string tempeta=tostring(abs(id.eta()));
   int tempphi=(file_dir.substr(2,1)=="L")?(2*(id.phi())-1):(2*(id.phi()));
   char sphi[4];
   snprintf(sphi,sizeof(sphi),"%02d",tempphi);
   string chambername=file_dir.substr(0,3)+tempeta+tempside+sphi;
  
  // string chambername=file_dir;
	m_tfile->mkdir(file_dir.c_str());
	m_tfile->cd(file_dir.c_str());


     //v2.0 move  const char* category[5]={"CalibSeg","Muonboy","Moore","Mean140","CalibSegCalibADC"}; 
       char radc[100],/*mc_radc[100],*//*m_tadc[100],*//*m_tdcadc[100],*/resiadc[100],rt[100],/*m_posXresi[100],*/pathadc[100],posXadc[100],/*m_Revadcresi[100],*/resir[100],/*m_pathresi[100],*//*m_posXr[100],*/dadc[100],dr[200];
     //  char m_hit_dis[100],m_localXZ[100],m_localYZ[100],m_globalXZ[100],m_globalYZ[100];
       char XZ[100],YZ[100];
	       sprintf(pathadc,"%.7s_ADC_vs_PathLength",chambername.c_str());
	       sprintf(resiadc,"%.7s_ADC_vs_Residual",chambername.c_str());
	       sprintf(radc,"%.7s_ADC_vs_Radius",chambername.c_str());
	       sprintf(dadc,"%.7s_ADC_vs_DistanceToSegmentTrack",chambername.c_str());
	       sprintf(dr,"%.7s_Radius_vs_DistanceToSegmentTrack",chambername.c_str());
	       sprintf(posXadc,"%.7s_ADC_vs_DistanceToReadout",chambername.c_str());
	       sprintf(resir,"%.7s_Residual_vs_Radius",chambername.c_str());
	       sprintf(rt,"%.7s_Radius_vs_DriftTime",chambername.c_str());
               m_MDT_adc_vs_pathD[station_identifier]= new TH2F(pathadc,pathadc,1000,0, 100, 350, 50, 400);
               m_MDT_adc_vs_pathD[station_identifier]->SetXTitle("PathLength(mm)");
               m_MDT_adc_vs_pathD[station_identifier]->SetYTitle("ADC count(ns)");	       
               m_MDT_adc_vs_posX[station_identifier]= new TH2F(posXadc,posXadc,600,0, 6000, 350, 50, 400);
               m_MDT_adc_vs_posX[station_identifier]->SetXTitle("DistanceToReadout(mm)");
               m_MDT_adc_vs_posX[station_identifier]->SetYTitle("ADC count(ns)");	       
               m_MDT_adc_vs_DistanceToSegmentTrack[station_identifier]= new TH2F(dadc,dadc,300,-15,15,350, 50, 400);
               m_MDT_adc_vs_DistanceToSegmentTrack[station_identifier]->SetXTitle("DistanceToSegmentTrack(mm)");
               m_MDT_adc_vs_DistanceToSegmentTrack[station_identifier]->SetYTitle("ADC count(ns)");
               m_MDT_adc_vs_radius[station_identifier]= new TH2F(radc,radc,150,0, 15, 350, 50, 400);
               m_MDT_adc_vs_radius[station_identifier]->SetXTitle("Radius(mm)");
               m_MDT_adc_vs_radius[station_identifier]->SetYTitle("ADC count(ns)");	       
               m_MDT_adc_vs_residual[station_identifier]= new TH2F(resiadc,resiadc,300,-15, 15, 350, 50, 400);
               m_MDT_adc_vs_residual[station_identifier]->SetXTitle("Residual(mm)");
               m_MDT_adc_vs_residual[station_identifier]->SetYTitle("ADC count(ns)");	       
              
               m_MDT_radius_vs_DistanceToSegmentTrack[station_identifier]= new TH2F(dr,dr,300,-15,15,300,-15,15);
               m_MDT_radius_vs_DistanceToSegmentTrack[station_identifier]->SetXTitle("DistanceToSegmentTrack(mm)");
               m_MDT_radius_vs_DistanceToSegmentTrack[station_identifier]->SetYTitle("Radius(mm)");
               m_MDT_radius_vs_t[station_identifier]= new TH2F(rt,rt,1000, -100, 900, 150,0,15);
               m_MDT_radius_vs_t[station_identifier]->SetYTitle("Radius(mm)");
               m_MDT_radius_vs_t[station_identifier]->SetXTitle("DriftTime(ns)");	       
               sprintf(XZ,"%.7s_LocalAngle_XZ",chambername.c_str());
	       sprintf(YZ,"%.7s_LocalAngle_YZ",chambername.c_str());
          	m_MDT_segment_localangle_YZ[station_identifier] =new TH1F(YZ,YZ,180, -90, 90);
		m_MDT_segment_localangle_YZ[station_identifier]->SetXTitle("Incidence Angle at local YZ plane(degrees)");
          	m_MDT_segment_localangle_XZ[station_identifier] =new TH1F(XZ,XZ,180, -90, 90);
		m_MDT_segment_localangle_XZ[station_identifier]->SetXTitle("Incidence Angle at local XZ plane(degrees)"); 
     // 1D plot 
      /*       
               mc_MDT_adc_vs_radius[station_identifier]= new TH2F(mc_radc,mc_radc,1500,0, 15, 350, 50, 400);
               mc_MDT_adc_vs_radius[station_identifier]->SetXTitle("Radius(mm)");
               mc_MDT_adc_vs_radius[station_identifier]->SetYTitle("CalibADC count(ns)");	       
               sprintf(m_localXZ,"%.7s_ADC_vs_LocalAngle_XZ",chambername.c_str());
	       sprintf(m_localYZ,"%.7s_ADC_vs_LocalAngle_YZ",chambername.c_str());
               sprintf(m_globalXZ,"%.7s_ADC_vs_GlobalAngle_XZ",chambername.c_str());
	       sprintf(m_globalYZ,"%.7s_ADC_vs_GlobalAngle_YZ",chambername.c_str());
          	m_MDT_adc_vs_segment_localangle_YZ[station_identifier] =new TH2F(m_localYZ,m_localYZ,1800, -90, 90,350,50,400);
		m_MDT_adc_vs_segment_localangle_YZ[station_identifier]->SetXTitle("Incidence Angle at local YZ plane(degrees)");
		m_MDT_adc_vs_segment_localangle_YZ[station_identifier]->SetYTitle("ADC count(ns)");
          	m_MDT_adc_vs_segment_localangle_XZ[station_identifier] =new TH2F(m_localXZ,m_localXZ,1800, -90, 90,350,50,400);
		m_MDT_adc_vs_segment_localangle_XZ[station_identifier]->SetXTitle("Incidence Angle at local XZ plane(degrees)");
		m_MDT_adc_vs_segment_localangle_XZ[station_identifier]->SetYTitle("ADC count(ns)");
         	m_MDT_adc_vs_segment_globalangle_YZ[station_identifier] =new TH2F(m_globalYZ,m_globalYZ,1800, -90, 90,350,50,400);
		m_MDT_adc_vs_segment_globalangle_YZ[station_identifier]->SetXTitle("Incidence Angle at global YZ plane(degrees)");
		m_MDT_adc_vs_segment_globalangle_YZ[station_identifier]->SetYTitle("ADC count(ns)");
          	m_MDT_adc_vs_segment_globalangle_XZ[station_identifier] =new TH2F(m_globalXZ,m_globalXZ,1800, -90, 90,350,50,400);
		m_MDT_adc_vs_segment_globalangle_XZ[station_identifier]->SetXTitle("Incidence Angle at global XZ plane(degrees)");
		m_MDT_adc_vs_segment_globalangle_YZ[station_identifier]->SetYTitle("ADC count(ns)");
	
               sprintf(XZ,"%.7s_LocalAngle_XZ",chambername.c_str());
	       sprintf(YZ,"%.7s_LocalAngle_YZ",chambername.c_str());
          	m_MDT_segment_localangle_YZ[station_identifier] =new TH1F(YZ,YZ,1800, -90, 90);
		m_MDT_segment_localangle_YZ[station_identifier]->SetXTitle("Incidence Angle at local YZ plane(degrees)");
          	m_MDT_segment_localangle_XZ[station_identifier] =new TH1F(XZ,XZ,1800, -90, 90);
		m_MDT_segment_localangle_XZ[station_identifier]->SetXTitle("Incidence Angle at local XZ plane(degrees)");
             
	       sprintf(m_pathresi,"%.7s_Residual_vs_PathLength",chambername.c_str());
	       sprintf(radc,"%.7s_ADC_vs_Radius",chambername.c_str());
	       sprintf(dadc,"%.7s_ADC_vs_DistanceToSegmentTrack",chambername.c_str());
	       sprintf(posXadc,"%.7s_ADC_vs_DistanceToReadout",chambername.c_str());
	
		m_MDT_Revadc_vs_residual[station_identifier]= new TH2F(m_Revadcresi,m_Revadcresi,200,0, 20, 3000,-15, 15);
               m_MDT_Revadc_vs_residual[station_identifier]->SetYTitle("Residual(mm)");
               m_MDT_Revadc_vs_residual[station_identifier]->SetXTitle("Reverse ADC count(1/ns)");	       
               m_MDT_radius_vs_residual[station_identifier]= new TH2F(resir,resir, 1500,0,15,3000,-15, 15);
               m_MDT_radius_vs_residual[station_identifier]->SetXTitle("Radius(mm)");
               m_MDT_radius_vs_residual[station_identifier]->SetYTitle("Residual(mm)");	       
               m_MDT_pathD_vs_residual[station_identifier]= new TH2F(m_pathresi,m_pathresi,2000,0, 100, 3000, -15, 15);
               m_MDT_pathD_vs_residual[station_identifier]->SetXTitle("PathLength(mm)");
               m_MDT_pathD_vs_residual[station_identifier]->SetYTitle("Residual(mm)");	        
 m_MDT_posX_vs_residual[station_identifier]= new TH2F(m_posXresi,m_posXresi,6000,0, 6000, 3000,-15,15);
               m_MDT_posX_vs_residual[station_identifier]->SetXTitle("DistanceToReadout(mm)");
               m_MDT_posX_vs_residual[station_identifier]->SetYTitle("Residual(mm)");	       
               m_MDT_posX_vs_radius[station_identifier]= new TH2F(m_posXr,m_posXr,6000,0, 6000, 1500,0,15);
               m_MDT_posX_vs_radius[station_identifier]->SetXTitle("DistanceToReadout(mm)");
               m_MDT_posX_vs_radius[station_identifier]->SetYTitle("Radius(mm)");	       
            m_MDT_adc_vs_tdc[station_identifier]= new TH2F(m_tdcadc,m_tdcadc,2500,0, 2500, 350, 50, 400);
               m_MDT_adc_vs_tdc[station_identifier]->SetXTitle("TDC count(ns)");
               m_MDT_adc_vs_tdc[station_identifier]->SetYTitle("ADC count(ns)");	       
               m_MDT_adc_vs_t[station_identifier]= new TH2F(m_tadc,m_tadc,1000,-100, 900, 350, 50, 400);
               m_MDT_adc_vs_t[station_identifier]->SetXTitle("DriftTime(ns)");
               m_MDT_adc_vs_t[station_identifier]->SetYTitle("ADC count(ns)");	       
	   //
	   //  v400 new residual
	   char m_resir1[100];
	   sprintf(m_resir1,"%.7s_Residual_vs_Radius_NoRefit",chambername.c_str());
	   m_MDT_residual_vs_radius_NoRefit[station_identifier]= new TH2F(m_resir1,m_resir1, 1500,0,15,3000,-15, 15);
           m_MDT_residual_vs_radius_NoRefit[station_identifier]->SetXTitle("Radius(mm)");
           m_MDT_residual_vs_radius_NoRefit[station_identifier]->SetYTitle("Residual(mm)");	      
	   char m_resir2[100];
	   sprintf(m_resir2,"%.7s_Residual_vs_Radius_Refit",chambername.c_str());
	   m_MDT_residual_vs_radius_Refit[station_identifier]= new TH2F(m_resir2,m_resir2, 1500,0,15,3000,-15, 15);
           m_MDT_residual_vs_radius_Refit[station_identifier]->SetXTitle("Radius(mm)");
           m_MDT_residual_vs_radius_Refit[station_identifier]->SetYTitle("Residual(mm)");	      
	   char m_resir3[100];
	   sprintf(m_resir3,"%.7s_Residual_vs_Radius_TimeSlewing",chambername.c_str());
	   m_MDT_residual_vs_radius_TimeSlewing[station_identifier]= new TH2F(m_resir3,m_resir3, 1500,0,15,3000,-15, 15);
           m_MDT_residual_vs_radius_TimeSlewing[station_identifier]->SetXTitle("Radius(mm)");
           m_MDT_residual_vs_radius_TimeSlewing[station_identifier]->SetYTitle("Residual(mm)");	
	   */
     for(int i=0;i<15;i++) {
	       char r_adcresi[200];
	       sprintf(r_adcresi,"%.7s_Radius%02d_Residual_vs_ADC",chambername.c_str(),i);
               m_r_MDT_residual_vs_adc[i][station_identifier]= new TH2F(r_adcresi,r_adcresi,350,50, 400,300 ,-15, 15);
               m_r_MDT_residual_vs_adc[i][station_identifier]->SetYTitle("Residual(mm)");
               m_r_MDT_residual_vs_adc[i][station_identifier]->SetXTitle("ADC Count(ns)");	       
                            }
     const char* FitFlag[3] = {"NoRefit","Refit","TimeSlewing"};
     char hits[100],chi2[100],rresi[100];
     for(int i=0;i<3;i++) {	     
	        sprintf(hits,"%.7s_Segment_hits_%s",chambername.c_str(),FitFlag[i]);
        	m_MDT_segment_hits[i][station_identifier] =new TH1F(hits,hits,100, 0, 100);
	        m_MDT_segment_hits[i][station_identifier]->SetXTitle("Hits per segment"); 
	        sprintf(chi2,"%.7s_Segment_chi2_%s",chambername.c_str(),FitFlag[i]);
        	m_MDT_segment_chi2[i][station_identifier] =new TH1F(chi2,chi2,500, 0, 50);
	        m_MDT_segment_chi2[i][station_identifier]->SetXTitle("Segment chi2/ndf"); 
	        sprintf(rresi,"%.7s_Residual_vs_Radius_%s",chambername.c_str(),FitFlag[i]);
        	m_MDT_residual_vs_radius[i][station_identifier] = new TH2F(rresi,rresi, 1500,0,15,3000,-15, 15);
	        m_MDT_residual_vs_radius[i][station_identifier]->SetXTitle("Radius(mm)");
	        m_MDT_residual_vs_radius[i][station_identifier]->SetYTitle("Residual(mm)");
                          }
     char delta_chi2[100];
     sprintf(delta_chi2,"%.7s_Segment_delta_chi2",chambername.c_str());
     m_MDT_segment_delta_chi2[station_identifier]= new TH1F(delta_chi2,delta_chi2,100,-5,5);
     m_MDT_segment_delta_chi2[station_identifier]->SetXTitle("Delta_Segment_chi2()");
     char fhit[100],shit[100];
      sprintf(fhit,"%.7s_fristHit_ADC_vs_TDC",chambername.c_str());
      sprintf(shit,"%.7s_secondHit_ADC_vs_TDC",chambername.c_str());
      m_MDT_fhit_adc[station_identifier]= new TH2F(fhit,shit,3000, 0,3000,500, 0, 500);
      m_MDT_fhit_adc[station_identifier]->SetXTitle("ADC count(ns)");	        
      m_MDT_fhit_adc[station_identifier]->SetYTitle("TDC count(ns)");	        
      m_MDT_shit_adc[station_identifier]= new TH2F(shit,shit,3000,0,3000, 500, 0, 500);
      m_MDT_shit_adc[station_identifier]->SetXTitle("ADC count(ns)");	        
      m_MDT_shit_adc[station_identifier]->SetYTitle("TDC count(ns)");	        

      m_tfile->cd("..");
	return;

}

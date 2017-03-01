/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetGlobalBCMTool.cxx
 * Implementation of inner detector beam conditions monitoring tool
 *
 * @author Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
 * Alex Kastanas <Alex.Kastanas@cern.ch> @n
 * Jennifer Jentzsch <Jennifer.Jentzsch@cern.ch>      
 *
 * $Id: InDetGlobalBCMTool.cxx,v 1.1 2009-01-19 11:06:07 sandaker Exp $
 *
 *********************************************************************************/

//Local
#include "InDetGlobalBCMTool.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
//Framework
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
//Root
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"

//Primary Vertex
#include "TrkEventPrimitives/ParamDefs.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "InDetGlobalPrimaryVertexMonTool.h"
//Pixel and SCT stuff
#include "InDetRawData/PixelRDORawData.h"
//EventInfo
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

using namespace TMath;
//Standard c++
#include <string>
#include <list>
#include <vector>


#define BCM_LVL1A 18

//Some variables
static unsigned int bcid_start 				= 0; //to choose the first bcid displayed
static unsigned int bcids_displayed 			= 3570; //to choose the number of bcids displayed when bcid as x-axis
static unsigned int bcid_max 				= 0;
static signed int ecr_start                             = 0;    //to chose the first ECR displayed
static unsigned int ecrs_displayed                      = 256;  //to chose the number of ecrs displayed when ECR as X-axis
static const unsigned int bc_readout			= 31;   // length of read out per L1A, at the moment 31 BCs re read out
static unsigned int lb_start 				= 0;
static unsigned int lb_max				= 5000;

// the declare property can be used to make variables accessible through python
InDetGlobalBCMTool::InDetGlobalBCMTool(
				       const std::string & type, 
				       const std::string & name,
				       const IInterface* parent)
  : InDetGlobalMotherMonTool(type, name, parent),
    m_detector("ID"),
    pulse_width_all{nullptr,nullptr},
    hits_lvl1a{nullptr,nullptr},
    hits_bcid{nullptr, nullptr},
    hitdistribution{nullptr, nullptr},
    pulse_position_gen{nullptr, nullptr},
    pulse_width_gen{nullptr, nullptr},
    strange_signals{nullptr, nullptr},
    highocc(nullptr),
    deltat_vs_hits{nullptr,nullptr},
    deltat_aligned_vs_hits{nullptr,nullptr},
    deltat_vs_bcid{nullptr,nullptr},
    deltat_aligned_vs_bcid{nullptr,nullptr},
    deltat_vs_lb{nullptr,nullptr},
    deltat_aligned_vs_lb{nullptr,nullptr},
    deltat_vs_ecr{nullptr,nullptr},
    deltat_aligned_vs_ecr{nullptr,nullptr},
    deltat_vs_PrVertex{nullptr,nullptr},
    deltat_vs_pixhits{nullptr,nullptr},
    deltat_vs_pixhitsEC{nullptr,nullptr},
    deltat_vs_pixhitsBR{nullptr,nullptr},
    deltat_vs_scthits{nullptr,nullptr},
    deltat_vs_scthitsBR{nullptr,nullptr},
    sct_vs_pix_col{nullptr,nullptr},
    sct_vs_pix_bkg{nullptr,nullptr}
{
  declareProperty("Detector", m_detector); 
  declareProperty("vxContainerName",m_vxContainerName="VxPrimaryCandidate");
  //   declareProperty("triggerChainName", m_triggerChainName = "NoTriggerSelection");
  declareProperty("histFolder",m_histFolder= "InDetGlobal/PrimaryVertex");
}

//---------------------------------------------------------
StatusCode InDetGlobalBCMTool::bookHistogramsRecurrent()
{
    MonGroup monGr_shift ( this, "InDetGlobal/BCM", run, ATTRIB_UNMANAGED);
    MonGroup monGr_GlobalHistograms ( this, "InDetGlobal/BCM/GlobalHistograms", run, ATTRIB_UNMANAGED);
    MonGroup monGr_SideHistograms ( this, "InDetGlobal/BCM/SideHistograms", run, ATTRIB_UNMANAGED);
    MonGroup monGr_DetectorHistograms ( this, "InDetGlobal/BCM/DetectorHistograms", run, ATTRIB_UNMANAGED);
  
    if (newRunFlag()){

    // Example of plot registration per new run
    registerHist(monGr_shift,m_nExamplePlot = new TH1F("m_nExample","Example plot BCM",5,0,5));

    std::string name,title,station_name,gain,pulse;
    
    /*************************************************************************************************************
     * Register Monitoring Histograms
     *************************************************************************************************************/

    name = "HitsVsLvl1AAll";
    title = "Hits vs LVL1A All";
    registerHist(monGr_GlobalHistograms,hits_lvl1a_all = new TH1F(name.c_str(),title.c_str(),32,0,32));
    hits_lvl1a_all->GetXaxis()->SetTitle("lvl1a [25 ns]");
    hits_lvl1a_all->GetYaxis()->SetTitle("# of hits");

    name = "ChannelVsLvl1a";
    title = "Channel vs LVL1 A";
    registerHist(monGr_GlobalHistograms,ChannelVsLvl1a = new TH2F(name.c_str(), title.c_str(), 64, 0, 64, 16, 0, 16));
    ChannelVsLvl1a->GetXaxis()->SetTitle("LVL1 A [25 ns]");
    ChannelVsLvl1a->GetYaxis()->SetTitle("Channel #");
    
    name = "ChannelVsBCID";
    title = "Channel vs BCID";
    registerHist(monGr_GlobalHistograms,ChannelVsBCID = new TH2F(name.c_str(),title.c_str(), bcids_displayed,bcid_start, bcid_start+bcids_displayed, 16, 0,16));
    ChannelVsBCID->GetXaxis()->SetTitle("BCID [25 ns]");
    ChannelVsBCID->GetYaxis()->SetTitle("Channel #");
    
    name = "ChannelVsECR";
    title = "Channel vs ECR";
    registerHist(monGr_GlobalHistograms,ChannelVsECR = new TH2F(name.c_str(), title.c_str(), 256, 0, 256, 16, 0, 16));
    ChannelVsECR->GetXaxis()->SetTitle("ECR [5 s]");
    ChannelVsECR->GetYaxis()->SetTitle("Channel #");

    name = "NumberOfEvents";
    title = "Number of monitored events";
    registerHist(monGr_GlobalHistograms,NumberOfEvents = new TH1F(name.c_str(), title.c_str(), 3,0,3));
    NumberOfEvents->GetXaxis()->SetTitle("sourceID [0=BCM, 1=LowHorizontal, 2=LowVertical]");
    NumberOfEvents->GetYaxis()->SetTitle("# of events");    

    name = "AbortFraction";
    title = "Abort Fraction";
    registerHist(monGr_GlobalHistograms,AbortFraction = new TH1F(name.c_str(), title.c_str(), 101, 0, 101));
    AbortFraction->GetXaxis()->SetTitle("Abort Fraction %");
    AbortFraction->GetYaxis()->SetTitle("# of Hits");

    name = "AbortFractionROD0";
    title = "Abort Fraction ROD0";
    registerHist(monGr_GlobalHistograms,AbortFractionROD0 = new TH1F(name.c_str(), title.c_str(), 101, 0, 101));
    AbortFractionROD0->GetXaxis()->SetTitle("Abort Fraction %");
    AbortFractionROD0->GetYaxis()->SetTitle("# of Hits");

    name = "AbortFractionROD1";
    title = "Abort Fraction ROD1";
    registerHist(monGr_GlobalHistograms,AbortFractionROD1 = new TH1F(name.c_str(), title.c_str(), 101, 0, 101));
    AbortFractionROD1->GetXaxis()->SetTitle("Abort Fraction %");
    AbortFractionROD1->GetYaxis()->SetTitle("# of Hits");

    name = "AbortFractionVsBCID";
    title = "Abort Fraction Vs BCID";
    registerHist(monGr_GlobalHistograms,AbortFractionVsBCID = new TH1F(name.c_str(), title.c_str(), 3563, 0, 3563));
    AbortFractionVsBCID->GetXaxis()->SetTitle("BCID [25 ns]");
    AbortFractionVsBCID->GetYaxis()->SetTitle("Abort Fraction %");

    name = "AbortFractionROD0VsBCID";
    title = "Abort Fraction ROD0 Vs BCID";
    registerHist(monGr_GlobalHistograms,AbortFractionROD0VsBCID = new TH1F(name.c_str(), title.c_str(), 3563, 0, 3563));
    AbortFractionROD0VsBCID->GetXaxis()->SetTitle("BCID [25 ns]");
    AbortFractionROD0VsBCID->GetYaxis()->SetTitle("Abort Fraction %");

    name = "AbortFractionROD1VsBCID";
    title = "Abort Fraction ROD1 Vs BCID";
    registerHist(monGr_GlobalHistograms,AbortFractionROD1VsBCID = new TH1F(name.c_str(), title.c_str(), 3563, 0, 3563));
    AbortFractionROD1VsBCID->GetXaxis()->SetTitle("BCID [25 ns]");
    AbortFractionROD1VsBCID->GetYaxis()->SetTitle("Abort Fraction %");

    name = "AbortFractionVsECR";
    title = "Abort Fraction Vs ECR";
    registerHist(monGr_GlobalHistograms,AbortFractionVsECR = new TH1F(name.c_str(), title.c_str(), 256, 0, 256));
    AbortFractionVsECR->GetXaxis()->SetTitle("ECR [5 s]");
    AbortFractionVsECR->GetYaxis()->SetTitle("Abort Fraction %");

    name = "AbortFractionVsLB";
    title = "Abort Fraction Vs LB";
    registerHist(monGr_GlobalHistograms,AbortFractionVsLB = new TH2F(name.c_str(), title.c_str(), lb_max, lb_start, lb_max, 100, 0, 1));
    AbortFractionVsLB->GetXaxis()->SetTitle("LB");
    AbortFractionVsLB->GetYaxis()->SetTitle("Abort Fraction %");

    name = "AbortFractionROD0VsECR";
    title = "Abort Fraction ROD0 Vs ECR";
    registerHist(monGr_GlobalHistograms,AbortFractionROD0VsECR = new TH1F(name.c_str(), title.c_str(), 256, 0, 256));
    AbortFractionROD0VsECR->GetXaxis()->SetTitle("ECR [5 s]");
    AbortFractionROD0VsECR->GetYaxis()->SetTitle("Abort Fraction %");

    name = "AbortFractionROD1VsECR";
    title = "Abort Fraction ROD1 Vs ECR";
    registerHist(monGr_GlobalHistograms,AbortFractionROD1VsECR = new TH1F(name.c_str(), title.c_str(), 256, 0, 256));
    AbortFractionROD1VsECR->GetXaxis()->SetTitle("BCID [5 s]");
    AbortFractionROD1VsECR->GetYaxis()->SetTitle("Abort Fraction %");

    name = "timewalkAll";
    title = "timewalk All";
    registerHist(monGr_GlobalHistograms,timewalk_all = new TH2F(name.c_str(),title.c_str(),64, 0, 64, 32, 0, 32));
    timewalk_all->GetXaxis()->SetTitle("pulse pos in time bins [25/64 ns]");
    timewalk_all->GetYaxis()->SetTitle("pulse width in time bins [25/64 ns]");

    name = "NumberHighOccupancyEventsVsLB";
    title = "Number of High Occupancy Events Vs LB";
    registerHist(monGr_GlobalHistograms, highocc = new TH1F(name.c_str(),title.c_str(), lb_max, lb_start, lb_max));
    highocc->GetXaxis()->SetTitle("LB number");
    highocc->GetYaxis()->SetTitle("Number of High Occupancy Events");


    // station_nr=0 for A-side, 1 for C-side
    // gain_value=0 if low , 1 if high
    // pulse_nr=0 for pulse1, 1 for pulse2
    
    for (unsigned int gain_value=0;gain_value<2;gain_value++)
      {
	if (gain_value==0){gain = "Low";} 
	else gain="High";   

	name = "PulsePositionAll";
	title = "Pulse Position All";
	registerHist(monGr_GlobalHistograms,pulse_position_all[gain_value] = new TH1F(name.c_str(),title.c_str(),64, 0, 64));
	pulse_position_all[gain_value]->GetXaxis()->SetTitle("time bins [25/64 ns]");
	pulse_position_all[gain_value]->GetYaxis()->SetTitle("# of hits");

	name = "PulseWidthAll";
	title = "Pulse Width All";
	registerHist(monGr_GlobalHistograms,pulse_width_all[gain_value] = new TH1F(name.c_str(),title.c_str(),32, 0, 32));
	pulse_width_all[gain_value]->GetXaxis()->SetTitle("time bins [25/64 ns]");
	pulse_width_all[gain_value]->GetYaxis()->SetTitle("# of hits");
	
	name = "HitDistribution" + gain + "Gain";
	title = "Hits vs Channel " + gain + " Gain";
	registerHist(monGr_GlobalHistograms,hitdistribution[gain_value] = new TH1F(name.c_str(),title.c_str(),8,0,8));
	hitdistribution[gain_value]->GetXaxis()->SetTitle("detector");
	hitdistribution[gain_value]->GetYaxis()->SetTitle("# of hits");

	name = "HitsVsLvl1A" + gain + "Gain";
	title = "Hits vs LVL1 A  " + gain + " Gain";
	registerHist(monGr_GlobalHistograms,hits_lvl1a[gain_value] = new TH1F(name.c_str(),title.c_str(),32,0,32));
	hits_lvl1a[gain_value]->GetXaxis()->SetTitle("lvl1a [25 ns]");
	hits_lvl1a[gain_value]->GetYaxis()->SetTitle("# of hits");
       
	name = "HitsVsBCID" + gain + "Gain";
	title = "Hits vs BCID  " + gain + " Gain";
	registerHist(monGr_GlobalHistograms,hits_bcid[gain_value] = new TH1F(name.c_str(),title.c_str(), bcids_displayed, bcid_start, bcid_start+bcids_displayed));
	hits_bcid[gain_value]->GetXaxis()->SetTitle("BCID [25 ns]");
	hits_bcid[gain_value]->GetYaxis()->SetTitle("# of hits");\
	
	name = "StrangeSignals" + gain + "Gain";
	title = "Strange Signals  " + gain + " Gain";
	registerHist(monGr_GlobalHistograms,strange_signals[gain_value] = new TH1F(name.c_str(),title.c_str(),16,0,16));
	strange_signals[gain_value]->GetXaxis()->SetTitle("Channel");
	strange_signals[gain_value]->GetYaxis()->SetTitle("# of hits");

	name = "PulsePosition" + gain + "Gain";
	title = "Pulse Position  " + gain + " Gain";
	registerHist(monGr_GlobalHistograms,pulse_position_gen[gain_value] = new TH1F(name.c_str(),title.c_str(),64, 0, 64));
	pulse_position_gen[gain_value]->GetXaxis()->SetTitle("time bins [25/64 ns]");
	pulse_position_gen[gain_value]->GetYaxis()->SetTitle("# of hits");

	name = "PulseWidth" + gain + "Gain";
	title = "Pulse Width  " + gain + " Gain";
	registerHist(monGr_GlobalHistograms,pulse_width_gen[gain_value] = new TH1F(name.c_str(),title.c_str(),32, 0, 32));
	pulse_width_gen[gain_value]->GetXaxis()->SetTitle("time bins [25/64 ns]");
	pulse_width_gen[gain_value]->GetYaxis()->SetTitle("# of hits");

	name = "Deltat" + gain + "Gain";
	title = "#Delta t " + gain + " Gain";
	registerHist(monGr_GlobalHistograms, deltat_vs_hits[gain_value] = new TH1F(name.c_str(),title.c_str(),50,-25,25));
	deltat_vs_hits[gain_value]->GetXaxis()->SetTitle("#Delta t [ns]");
	deltat_vs_hits[gain_value]->GetYaxis()->SetTitle("# of hits");
	
	name = "DeltatAligned" + gain + "Gain";
	title = "#Delta t Aligned " + gain + " Gain";
	registerHist(monGr_GlobalHistograms, deltat_aligned_vs_hits[gain_value] = new TH1F(name.c_str(),title.c_str(),50,-25,25));
	deltat_aligned_vs_hits[gain_value]->GetXaxis()->SetTitle("#Delta t [ns]");
	deltat_aligned_vs_hits[gain_value]->GetYaxis()->SetTitle("# of hits");
	
	name = "DeltatVsBCID" + gain + "Gain";
	title = "#Delta t vs BCID " + gain + " Gain";
	registerHist(monGr_GlobalHistograms, deltat_vs_bcid[gain_value] = new TH2F(name.c_str(),title.c_str(), bcids_displayed,bcid_start, bcid_start+bcids_displayed,50,-25,25));
	deltat_vs_bcid[gain_value]->GetXaxis()->SetTitle("BCID [25 ns]");
	deltat_vs_bcid[gain_value]->GetYaxis()->SetTitle("#Delta t [ns]");

	name = "DeltatAlignedVsBCID" + gain + "Gain";
	title = "#Delta t Aligned vs BCID " + gain + " Gain";
	registerHist(monGr_GlobalHistograms, deltat_aligned_vs_bcid[gain_value] = new TH2F(name.c_str(),title.c_str(), bcids_displayed,bcid_start, bcid_start+bcids_displayed,50,-25,25));
	deltat_aligned_vs_bcid[gain_value]->GetXaxis()->SetTitle("BCID [25 ns]");
	deltat_aligned_vs_bcid[gain_value]->GetYaxis()->SetTitle("#Delta t [ns]");

	name = "DeltatVsLB" + gain + "Gain";
	title = "#Delta t vs LB " + gain + " Gain";
	registerHist(monGr_GlobalHistograms, deltat_vs_lb[gain_value] = new TH2F(name.c_str(),title.c_str(), lb_max, lb_start, lb_max, 50,-25,25));
	deltat_vs_lb[gain_value]->GetXaxis()->SetTitle("LB number");
	deltat_vs_lb[gain_value]->GetYaxis()->SetTitle("#Delta t [ns]");

	name = "DeltatAlignedVsLB" + gain + "Gain";
	title = "#Delta t Aligned vs LB " + gain + " Gain";
	registerHist(monGr_GlobalHistograms, deltat_aligned_vs_lb[gain_value] = new TH2F(name.c_str(),title.c_str(), lb_max, lb_start, lb_max, 50,-25,25));
	deltat_aligned_vs_lb[gain_value]->GetXaxis()->SetTitle("LB number");
	deltat_aligned_vs_lb[gain_value]->GetYaxis()->SetTitle("#Delta t [ns]");

	name = "DeltatVsECR" + gain + "Gain";
	title = "#Delta t vs ECR " + gain + " Gain";
	registerHist(monGr_GlobalHistograms, deltat_vs_ecr[gain_value] = new TH2F(name.c_str(),title.c_str(), ecrs_displayed,ecr_start, ecr_start+ecrs_displayed,50,-25,25));
	deltat_vs_ecr[gain_value]->GetXaxis()->SetTitle("ECR");
	deltat_vs_ecr[gain_value]->GetYaxis()->SetTitle("#Delta t [ns]");

	name = "DeltatAlignedVsECR" + gain + "Gain";
	title = "#Delta t Aligned vs ECR " + gain + " Gain";
	registerHist(monGr_GlobalHistograms, deltat_aligned_vs_ecr[gain_value] = new TH2F(name.c_str(),title.c_str(), ecrs_displayed,ecr_start, ecr_start+ecrs_displayed,50,-25,25));
	deltat_aligned_vs_ecr[gain_value]->GetXaxis()->SetTitle("ECR");
	deltat_aligned_vs_ecr[gain_value]->GetYaxis()->SetTitle("#Delta t [ns]");

	name = "DeltatVsPrVertex" + gain + "Gain";
	title = "#Delta t vs Primary Vertex " + gain + " Gain";
	registerHist(monGr_GlobalHistograms, deltat_vs_PrVertex[gain_value] = new TH2F(name.c_str(),title.c_str(),100,-200,200,50,-25,25));
	deltat_vs_PrVertex[gain_value]->GetXaxis()->SetTitle("Primary Vertex z position [mm] ");
	deltat_vs_PrVertex[gain_value]->GetYaxis()->SetTitle("#Delta t [ns]");

	name = "DeltatVsPixelHits" + gain + "Gain";
	title = "#Delta t vs NumberPixelHits " + gain + " Gain";
	registerHist(monGr_GlobalHistograms, deltat_vs_pixhits[gain_value] = new TH2F(name.c_str(),title.c_str(), 100, 0, 10000, 50,-25,25));
	deltat_vs_pixhits[gain_value]->GetXaxis()->SetTitle("Number Pixel Hits");
	deltat_vs_pixhits[gain_value]->GetYaxis()->SetTitle("#Delta t [ns]");
	  
	name = "DeltatVsPixelHitsBR" + gain + "Gain";
	title = "#Delta t vs NumberPixelHits in Barrel " + gain + " Gain";
	registerHist(monGr_GlobalHistograms, deltat_vs_pixhitsBR[gain_value] = new TH2F(name.c_str(),title.c_str(), 100, 0, 10000, 50,-25,25));
	deltat_vs_pixhitsBR[gain_value]->GetXaxis()->SetTitle("Number Barrel Pixel Hits");
	deltat_vs_pixhitsBR[gain_value]->GetYaxis()->SetTitle("#Delta t [ns]");
	  
	name = "DeltatVsPixelHitsEC" + gain + "Gain";
	title = "#Delta t vs NumberPixelHits in Endcap " + gain + " Gain";
	registerHist(monGr_GlobalHistograms, deltat_vs_pixhitsEC[gain_value] = new TH2F(name.c_str(),title.c_str(), 100, 0, 10000, 50,-25,25));
	deltat_vs_pixhitsEC[gain_value]->GetXaxis()->SetTitle("Number Endcap Pixel Hits");
	deltat_vs_pixhitsEC[gain_value]->GetYaxis()->SetTitle("#Delta t [ns]");
	  
	name = "DeltatVsSctHits" + gain + "Gain";
	title = "#Delta t vs NumberSctHits " + gain + " Gain";
	registerHist(monGr_GlobalHistograms, deltat_vs_scthits[gain_value] = new TH2F(name.c_str(),title.c_str(), 100, 0, 20000, 50,-25,25));
	deltat_vs_scthits[gain_value]->GetXaxis()->SetTitle("Number Sct Hits");
	deltat_vs_scthits[gain_value]->GetYaxis()->SetTitle("#Delta t [ns]");
	  
	name = "DeltatVsSctHitsBR" + gain + "Gain";
	title = "#Delta t vs NumberSctHits in Barrel " + gain + " Gain";
	registerHist(monGr_GlobalHistograms, deltat_vs_scthitsBR[gain_value] = new TH2F(name.c_str(),title.c_str(), 100, 0, 20000, 50,-25,25));
	deltat_vs_scthitsBR[gain_value]->GetXaxis()->SetTitle("Number Barrel Sct Hits");
	deltat_vs_scthitsBR[gain_value]->GetYaxis()->SetTitle("#Delta t [ns]");
	  
	name = "DeltatVsSctHitsEC" + gain + "Gain";
	title = "#Delta t vs NumberSctHits in Endcap " + gain + " Gain";
	registerHist(monGr_GlobalHistograms, deltat_vs_scthitsEC[gain_value] = new TH2F(name.c_str(),title.c_str(), 100, 0, 20000, 50,-25,25));
	deltat_vs_scthitsEC[gain_value]->GetXaxis()->SetTitle("Number Endcap Sct Hits");
	deltat_vs_scthitsEC[gain_value]->GetYaxis()->SetTitle("#Delta t [ns]");

	name = "SctVsPixHitsCol" + gain + "Gain";
	title = "#NumberSctHits vs NumberPixHits w/ Collison delta t " + gain + " Gain";
	registerHist(monGr_GlobalHistograms, sct_vs_pix_col[gain_value] = new TH2F(name.c_str(),title.c_str(), 100, 0, 20000, 100, 0, 10000));
	sct_vs_pix_col[gain_value]->GetXaxis()->SetTitle("Number Pixel Hits");
	sct_vs_pix_col[gain_value]->GetYaxis()->SetTitle("Number Sct Hits");
	  
	name = "SctVsPixHitsBkg" + gain + "Gain";
	title = "#NumberSctHits vs NumberPixHits w/ Background delta t " + gain + " Gain";
	registerHist(monGr_GlobalHistograms, sct_vs_pix_bkg[gain_value] = new TH2F(name.c_str(),title.c_str(), 100, 0, 20000, 100, 0, 10000));
	sct_vs_pix_bkg[gain_value]->GetXaxis()->SetTitle("Number Pixel Hits");
	sct_vs_pix_bkg[gain_value]->GetYaxis()->SetTitle("Number Sct Hits");
	  
	for (unsigned int pulse_nr=0;pulse_nr<2;pulse_nr++) {

	  if (pulse_nr==0){pulse = "1";} 
	  else pulse="2";   
	    
	  name="Pulse"+pulse+"Width"+gain+"Gain";
	  title="Pulse"+ pulse +"Width" + gain  + "Gain";
	  registerHist(monGr_GlobalHistograms, pulse_width[gain_value][pulse_nr] = new TH1F(name.c_str(),title.c_str(),32,0,32));
	  pulse_width[gain_value][pulse_nr]->GetXaxis()->SetTitle("time bins [25/32 ns]");
	  pulse_width[gain_value][pulse_nr]->GetYaxis()->SetTitle("# of hits");

	  name= "Pulse"+pulse+"Position"+ gain+"Gain";
	  title = "Pulse " + pulse + " Position  " + gain + " Gain";
	  registerHist(monGr_GlobalHistograms, pulse_position[gain_value][pulse_nr] = new TH1F(name.c_str(),title.c_str(),64,0,64));
	  pulse_position[gain_value][pulse_nr]->GetXaxis()->SetTitle("time bins [25/64 ns]");
	  pulse_position[gain_value][pulse_nr]->GetYaxis()->SetTitle("# of hits");

	  name = "HitDistributionPulse"+pulse+gain+"Gain";
	  title = "Hitdistribution Pulse " + pulse + " " + gain + " Gain";
	  registerHist(monGr_GlobalHistograms, hitdistribution_pulse[gain_value][pulse_nr] = new TH1F(name.c_str(),title.c_str(),8,0,8));
	  hitdistribution_pulse[gain_value][pulse_nr]->GetXaxis()->SetTitle("detector");
	  hitdistribution_pulse[gain_value][pulse_nr]->GetYaxis()->SetTitle("# of hits");

	}//end of pulse loop
      }//end of gain loop
    
    /**************************
     * Register Side Histograms
     ***************************/  
    
    for(unsigned int station_nr=0;station_nr<2;station_nr++)
      {
	if (station_nr==0) station_name="A-side";
	else station_name="C-side";
	
	for (unsigned int gain_value=0;gain_value<2;gain_value++)
	  {
	    if (gain_value==0){gain="Low";} 
	    else gain="High";   
	        
	    name="HitsVsLvl1A"+station_name+"Gain_gain";
	    title = "" + station_name + " Hits vs LVL1 A  " + gain + " Gain";
	    registerHist(monGr_SideHistograms,stat_lvl1a[station_nr][gain_value] = new TH1F(name.c_str(),title.c_str(),32,0,32));
	    stat_lvl1a[station_nr][gain_value]->GetXaxis()->SetTitle("lvl1a [25 ns]");
	    stat_lvl1a[station_nr][gain_value]->GetYaxis()->SetTitle("# of hits");
	        
	    name="PulsePosition"+station_name+"Gain_"+gain;
	    title = "" + station_name + " Pulse Position  " + gain + " Gain";
	    registerHist(monGr_SideHistograms, stat_pulse_position_gen[station_nr][gain_value] = new TH1F(name.c_str(),title.c_str(),64,0,64));
	    stat_pulse_position_gen[station_nr][gain_value]->GetXaxis()->SetTitle("time bins [25/64 ns]");
	    stat_pulse_position_gen[station_nr][gain_value]->GetYaxis()->SetTitle("# of hits");
	        
	    name="PulseWidth"+station_name+"Gain_"+gain;
	    title = "" + station_name + " Pulse Width  " + gain + " Gain";
	    registerHist(monGr_SideHistograms,stat_pulse_width_gen[station_nr][gain_value] = new TH1F(name.c_str(),title.c_str(),32,0,32));
	    stat_pulse_width_gen[station_nr][gain_value]->GetXaxis()->SetTitle("time bins [25/32 ns]");
	    stat_pulse_width_gen[station_nr][gain_value]->GetYaxis()->SetTitle("# of hits");

	    for (unsigned int pulse_nr=0; pulse_nr<2;pulse_nr++)
	      {
		if (pulse_nr==0){pulse="1";} 
		else pulse="2";   
		
		name="Pulse"+station_name+"Position"+pulse+"Gain_"+gain;
		title=station_name+" Pulse "+pulse+" Position "+gain+" Gain";
		registerHist(monGr_SideHistograms, stat_pulse_position[station_nr][gain_value][pulse_nr] = new TH1F(name.c_str(),title.c_str(),64,0,64));
		stat_pulse_position[station_nr][gain_value][pulse_nr]->GetXaxis()->SetTitle("time bins [25/64 ns]");
		stat_pulse_position[station_nr][gain_value][pulse_nr]->GetYaxis()->SetTitle("# of hits");
		
		name="Pulse"+station_name+"Width"+pulse+"Gain_"+gain;
		title=station_name+" Pulse "+pulse+" Width "+gain+" Gain";
		registerHist(monGr_SideHistograms,stat_pulse_width[station_nr][gain_value][pulse_nr] = new TH1F(name.c_str(),title.c_str(),32,0,32));
		stat_pulse_width[station_nr][gain_value][pulse_nr]->GetXaxis()->SetTitle("time bins [25/32 ns]");
		stat_pulse_width[station_nr][gain_value][pulse_nr]->GetYaxis()->SetTitle("# of hits");
			
	      }//end of pulse_nr loop
	  }//end of gain loop
      }//end of side loop 
    
    /*************************
     * Register Detector Histograms
     *************************/
    std::string detector_name;
    
    for (unsigned int detector=0;detector<8;detector++) {

      if (detector<4) station_name="A-side";
      else station_name="C-side";
      
      switch(detector) {
      case 0:
	detector_name="+X";
	break;
      case 1:
	detector_name="+Y";
	break;
      case 2:     
	detector_name="-X";
	break;
      case 3:
	detector_name="-Y";
	break;
      case 4:
	detector_name="+X";
	break;
      case 5:
	detector_name="+Y";
	break;
      case 6:
	detector_name="-X";
	break;
      case 7:
	detector_name="-Y";
	break;
      }
      
      for (unsigned int gain_value=0;gain_value<2;gain_value++)
	{
	  if (gain_value==0){gain="Low";} 
	  else gain="High";   
	    
	  name="HitsVsLvl1A"+station_name+"Gain_"+detector_name+gain;
	  title= station_name+ " "+ detector_name +" Hits vs LVL1 A "+ gain +" Gain";
	  registerHist(monGr_DetectorHistograms,det_lvl1a[detector][gain_value] = new TH1F(name.c_str(),title.c_str(),32,0,32));
	  det_lvl1a[detector][gain_value]->GetXaxis()->SetTitle("lvl1a [25 ns]");
	  det_lvl1a[detector][gain_value]->GetYaxis()->SetTitle("# of hits");
	    
	  name="PulsePosition"+station_name+"Gain_"+detector_name+gain;
	  title=station_name+" "+detector_name+" Pulse Position "+gain +" Gain";
	  registerHist(monGr_DetectorHistograms, det_pulse_position_gen[detector][gain_value] = new TH1F(name.c_str(),title.c_str(),64,0,64));
	  det_pulse_position_gen[detector][gain_value]->GetXaxis()->SetTitle("time bins [25/64 ns]");
	  det_pulse_position_gen[detector][gain_value]->GetYaxis()->SetTitle("# of hits");
	    
	  name="PulseWidth"+station_name+"Gain_"+detector_name+gain;
	  title=station_name+" "+detector_name+" Pulse Width "+gain+" Gain";
	  registerHist(monGr_DetectorHistograms,det_pulse_width_gen[detector][gain_value] = new TH1F(name.c_str(),title.c_str(),32,0,32));
	  det_pulse_width_gen[detector][gain_value]->GetXaxis()->SetTitle("time bins [25/32 ns]");
	  det_pulse_width_gen[detector][gain_value]->GetYaxis()->SetTitle("# of hits");
	  
	  name="timewalk"+station_name+"_"+detector_name+gain;
	  title=station_name+" "+detector_name+" timewalk "+ gain +" Gain";
	  registerHist(monGr_GlobalHistograms,det_timewalk[detector][gain_value] = new TH2F(name.c_str(),title.c_str(),64, 0, 64, 32, 0, 32));
	  det_timewalk[detector][gain_value]->GetXaxis()->SetTitle("pulse pos in time bins [25/64 ns]");
	  det_timewalk[detector][gain_value]->GetYaxis()->SetTitle("pulse width in time bins [25/64 ns]");    
	  
	  for (unsigned int pulse_nr=0;pulse_nr<2;pulse_nr++) {
	    if (pulse_nr==0){pulse="1";} 
	    else pulse="2";   
	        
	    name="Pulse"+station_name+"Position"+detector_name+"Gain_"+pulse+gain;
	    title=station_name+" "+ detector_name +" Pulse "+pulse+" Position "+ gain +" Gain";
	    registerHist(monGr_DetectorHistograms, det_pulse_position[detector][gain_value][pulse_nr] = new TH1F(name.c_str(),title.c_str(),64,0,64));
	    det_pulse_position[detector][gain_value][pulse_nr]->GetXaxis()->SetTitle("time bins [25/64 ns]");
	    det_pulse_position[detector][gain_value][pulse_nr]->GetYaxis()->SetTitle("# of hits");
	        
	    name="Pulse"+station_name+"Width"+detector_name+"Gain_"+pulse+gain;
	    title=station_name + " " + detector_name + " Pulse " + pulse + "  Width " + gain + " Gain";
	    registerHist(monGr_DetectorHistograms,det_pulse_width[detector][gain_value][pulse_nr] = new TH1F(name.c_str(),title.c_str(),32,0,33));
	    det_pulse_width[detector][gain_value][pulse_nr]->GetXaxis()->SetTitle("time bins [25/32 ns]");
	    det_pulse_width[detector][gain_value][pulse_nr]->GetYaxis()->SetTitle("# of hits");
	  }//end of pulse loop
	}//end of gain loop 
    }//end of detector loop
  } 
  
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------
/***************************************************************************
 * At first a method to fill the global and the pulse 1 and 2 histos
 **************************************************************************/
void FillGlobalHistos(TH2F* h1, TH2F* h2, TH2F* h3, TH1F* h4, TH1F* h5, int pulsewid, int channel, int bcid, int m_ecr, int lvl1a){
  if (pulsewid!=0){
    h1->Fill(lvl1a, channel);
    h2->Fill(bcid, channel);
    h3->Fill(m_ecr, channel);
    h4->Fill(lvl1a);
    h5->Fill(bcid);
  }
}

void FillPulseHistos(TH1F* h1, TH1F* h2, TH1F* h3, TH1F* h4, int pulsepos, int pulsewid, int tmp = 0, TH1F* h5 = NULL){
  if (pulsewid!=0){
    h1->Fill(pulsepos);
    h2->Fill(pulsewid);
    h3->Fill(pulsepos);
    h4->Fill(pulsewid);
    if (h5 != NULL){
      h5->Fill(tmp);
    }
  }
}
/****************************************************************************
 * Some methods one needs for the calculation of Delta t
 ***************************************************************************/
deltat_data::deltat_data():
    bcid(0),
    ecr(0),
    position(0),
    detector(0),
    lvl1a(0)
{
}

bool deltat_data::operator<(const deltat_data &data){
  return (bcid<data.bcid);
}

bool bcid_select(const deltat_data &data){
  return (data.bcid <bcid_max);
}

StatusCode InDetGlobalBCMTool::fillHistograms(){

  //Filling of histograms (loop over collections) :
  
  if      (m_detector == "sct") { m_nExampleInt = 2; }   
  else if (m_detector == "trt") { m_nExampleInt = 3; }
  else if (m_detector == "pixel"){m_nExampleInt = 1; }
  else if (m_detector == "ID" ) { m_nExampleInt = 4; }
  else { m_nExampleInt = 0; }
  
  m_nExamplePlot->Fill(m_nExampleInt);

  //retrieve LB number for plots vs LB
  const EventInfo * evtInfo;
  if ( evtStore()->retrieve(evtInfo).isFailure() ){
    m_current_LB = 0;
    if ( msgLvl(MSG::WARNING) ){
      msg(MSG::WARNING) << "Could not retrieve the event information container" << endmsg;
    }
  } else {
      m_current_LB = evtInfo->event_ID()->lumi_block();
  }

  //lists for calculation of deltat  
  std::vector<std::list<deltat_data> > positions_A(2);
  std::vector<std::list<deltat_data> > positions_C(2);

  //double z to store Primary Vertex z position
  double z = 1001; //that it can not mix with data if primary vertex calculation fails, z_max = 1000
  
  // Primary vertex monitoring
  const VxContainer* vxContainer = 0;
  if (evtStore()->contains<VxContainer>(m_vxContainerName)) {
    if (evtStore()->retrieve(vxContainer,m_vxContainerName).isFailure() ) {
      ATH_MSG_DEBUG ("Could not retrieve primary vertex container with key "+m_vxContainerName);
      return StatusCode::SUCCESS;
    } else {
	  
	int nPriVtx = 0;
	int nPileupVtx = 0;
	
	if (vxContainer->size() == 1) {
	    if ( msgLvl(MSG::DEBUG ) ) {
		msg(MSG::DEBUG) <<  "vxContainer size = 1 ->contains only dummy vertex" << endmsg; 
	    }
	}
	else{
	    for (VxContainer::const_iterator vxIter = vxContainer->begin(); vxIter != vxContainer->end(); ++vxIter)
	    {
		// Count different types of vertices
		if ((*vxIter)->vertexType() == Trk::PriVtx) nPriVtx++;
		if ((*vxIter)->vertexType() == Trk::PileUp) nPileupVtx++;
		
		// Select primary vertex
		if ((*vxIter)->vertexType() != Trk::PriVtx) continue;
		z = (*vxIter)->recVertex().position().z();
	    }
	}
    }
  } else {
      ATH_MSG_DEBUG ("StoreGate doesn't contain primary vertex container with key "+m_vxContainerName);
  }

  
  if ( m_BCM_RDO != 0 && !m_BCM_RDO->empty() ){
    BCM_RDO_Container::const_iterator BCM_RDO_itr     = m_BCM_RDO->begin();
    BCM_RDO_Container::const_iterator BCM_RDO_itr_end = m_BCM_RDO->end();

    bool ROD0 = true; //bools for NumberOfEvents histo
    bool ROD1 = true;

    //int abortcountHGRod0 = 0; //FIXME abortFraction calculator
    //int abortcountLGRod0 = 0;
    //int abortcountHGRod1 = 0;
    //int abortcountLGRod1 = 0;

    int nROD0HitLG[bc_readout] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int nROD0HitHG[bc_readout] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int nROD1HitLG[bc_readout] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int nROD1HitHG[bc_readout] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int nROD0BCID[bc_readout]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int nROD1BCID[bc_readout]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    unsigned int ecr = -1;
    int ROD = -1;

    int BC_counter = -1;
    int channel_counter = -1;

    for (; BCM_RDO_itr != BCM_RDO_itr_end; BCM_RDO_itr++) { // loops over 16 channels (HR counter 0 to 15)
      channel_counter++;
      //ATH_MSG_WARNING ( "channel_counter: " << channel_counter);
      if ( (*BCM_RDO_itr)->size() != 0) {
	BCM_RDO_Collection::const_iterator RDO_element        = (*BCM_RDO_itr)->begin();
	BCM_RDO_Collection::const_iterator RDO_element_last   = (*BCM_RDO_itr)->end();
    	BC_counter = -1;
	
	for (; RDO_element != RDO_element_last; RDO_element++){ // loops over 31 BCs read out per L1A
          BC_counter++;
          //ATH_MSG_WARNING ( "BC_counter: " << BC_counter);
	  if (*RDO_element == NULL)
	  {
	      ATH_MSG_WARNING ("NULL pointer!");
	      continue;
	  }
	  //else ATH_MSG_WARNING ("RDO_element :" << *RDO_element );
	    
	  int bcm_lvl1a           = (*RDO_element)->getLVL1A();
	  int bcm_channel         = (*RDO_element)->getChannel();
	  int bcm_bcid            = (*RDO_element)->getBCID();
	  int bcm_pulse1position  = (*RDO_element)->getPulse1Position(); 
	  int bcm_pulse1width     = (*RDO_element)->getPulse1Width();
	  int bcm_pulse2position  = (*RDO_element)->getPulse2Position();
	  int bcm_pulse2width     = (*RDO_element)->getPulse2Width();




	  /*********************************
	  *Filling Abort Fraction arrays
	  **********************************/
	  
	  if (channel_counter == 0 || channel_counter == 2 || channel_counter == 4 || channel_counter == 6  || channel_counter ==  9 || channel_counter ==  11 || channel_counter ==  13 || channel_counter == 15)  ROD = 0;
	  else ROD = 1;
	  if (bcm_pulse1width>0 || bcm_pulse2width>0){
	    if (ROD == 0 && channel_counter < 8 && nROD0HitLG[BC_counter]<3) nROD0HitLG[BC_counter]++;
	    if (ROD == 1 && channel_counter < 8 && nROD1HitLG[BC_counter]<3) nROD1HitLG[BC_counter]++;
	    if (ROD == 0) nROD0BCID[BC_counter] = bcm_bcid;
	    if (ROD == 0 && channel_counter > 7 && nROD0HitHG[BC_counter]<3) nROD0HitHG[BC_counter]++;
	    if (ROD == 1 && channel_counter > 7 && nROD1HitHG[BC_counter]<3) nROD1HitHG[BC_counter]++;
	    if (ROD == 1) nROD1BCID[BC_counter] = bcm_bcid;
	  }








	
		  
	  /*********************************
	   *Filling NumberOfEvent histograms
	   *********************************/
	  if (ROD0 && (bcm_channel == 0 || bcm_channel == 2 || bcm_channel == 4 || bcm_channel == 6 || bcm_channel == 9 || bcm_channel == 11 || bcm_channel == 13 || bcm_channel == 15)){
	    NumberOfEvents->Fill(1.5);
	    NumberOfEvents->Fill(0.5);
	    ROD0 = false; // FIXME seems to be a bug: first loop is over channel, so alternating RODs, bit bool ROD0 (and ROD1) is being set to false after filling first time! as well filled, if all BC was empty!
	  }
	  if (ROD1 && (bcm_channel == 1 || bcm_channel == 3 || bcm_channel == 5 || bcm_channel == 7 || bcm_channel == 8 || bcm_channel == 10 || bcm_channel == 12 || bcm_channel == 14)){
	    NumberOfEvents->Fill(2.5);
	    NumberOfEvents->Fill(0.5);
	    ROD1 = false;
	  }
	    
	  if ( bcm_pulse1width !=0 || bcm_pulse2width !=0){
	        
	    // strange signals: You see something where there should be nothing, because the pulse widths are zero. // FIXME condition never fulfillable
	    if (( bcm_pulse1width == 0 && bcm_pulse2width == 0) &&  (bcm_pulse1position !=0 || bcm_pulse2position !=0)){
	      if (bcm_channel < 8) strange_signals[0]->Fill(bcm_channel);
	      if (bcm_channel > 7) strange_signals[1]->Fill(bcm_channel);
	    }
	        
	    //Event information to get ECR
	    ecr = 0;
	    const EventInfo * evtInfo;
	    if ( evtStore()->retrieve(evtInfo).isFailure() ){
	      if ( msgLvl(MSG::WARNING) ){
		msg(MSG::WARNING) << "Could not retrieve the event information container" << endmsg;
	      }
	    } else {
		ecr = (evtInfo->trigger_info()->extendedLevel1ID() &  0xff000000) >> 24;   // Returns the extendedLevel1ID of the trigger information object
	    }
	        
	    /******************************************************************************************
	     *Global Histograms
	     ******************************************************************************************/

	    if ( bcm_channel < 8 ){
	      if (bcm_pulse1width != 0) hitdistribution[0]->Fill(bcm_channel);
	      if (bcm_pulse2width != 0) hitdistribution[0]->Fill(bcm_channel);
	      FillGlobalHistos(ChannelVsLvl1a, ChannelVsBCID, ChannelVsECR, hits_lvl1a[0], hits_bcid[0], bcm_pulse1width, bcm_channel, bcm_bcid, ecr, bcm_lvl1a);
	      FillGlobalHistos(ChannelVsLvl1a, ChannelVsBCID, ChannelVsECR, hits_lvl1a[0], hits_bcid[0], bcm_pulse2width, bcm_channel, bcm_bcid, ecr, bcm_lvl1a);
	      FillPulseHistos(pulse_position_gen[0], pulse_width_gen[0], pulse_position[0][0], pulse_width[0][0], bcm_pulse1position, bcm_pulse1width, bcm_channel,  hitdistribution_pulse[0][0]);
	      FillPulseHistos(pulse_position_gen[0], pulse_width_gen[0], pulse_position[0][1], pulse_width[0][1], bcm_pulse2position, bcm_pulse2width, bcm_channel,  hitdistribution_pulse[0][1]);
	    }
	    if (bcm_channel > 7){
	      if (bcm_pulse1width != 0) hitdistribution[1]->Fill(bcm_channel-8);
	      if (bcm_pulse2width != 0) hitdistribution[1]->Fill(bcm_channel-8);
	      FillGlobalHistos(ChannelVsLvl1a, ChannelVsBCID, ChannelVsECR, hits_lvl1a[1], hits_bcid[1], bcm_pulse1width, bcm_channel, bcm_bcid, ecr, bcm_lvl1a);
	      FillGlobalHistos(ChannelVsLvl1a, ChannelVsBCID, ChannelVsECR, hits_lvl1a[1], hits_bcid[1], bcm_pulse2width, bcm_channel, bcm_bcid, ecr, bcm_lvl1a);
	      FillPulseHistos(pulse_position_gen[1], pulse_width_gen[1], pulse_position[1][0], pulse_width[1][0], bcm_pulse1position, bcm_pulse1width, bcm_channel-8, hitdistribution_pulse[1][0]);
	      FillPulseHistos(pulse_position_gen[1], pulse_width_gen[1], pulse_position[1][1], pulse_width[1][1], bcm_pulse2position, bcm_pulse2width, bcm_channel-8, hitdistribution_pulse[1][1]);
	    }
	          
	    /************************************************************************************************
	     *Side Histograms
	     *************************************************************************************************/
	          
	    // A Side Low Gain
	    if (bcm_channel < 4){
	      deltat_data s1a;
	      s1a.bcid = bcm_bcid;
	      s1a.position = bcm_pulse1position;
	      s1a.ecr = ecr;
	      s1a.detector = bcm_channel%4;
              s1a.lvl1a = bcm_lvl1a;
	      positions_A[0].push_back(s1a);
	      if (bcm_pulse2width != 0){
		deltat_data s2a;
		s2a.bcid = bcm_bcid;
		s2a.position = bcm_pulse2position;
		s2a.ecr = ecr;
	        s2a.detector = bcm_channel%4;
		s2a.lvl1a = bcm_lvl1a;
		positions_A[0].push_back(s2a);
	      }
	      FillPulseHistos(stat_pulse_position_gen[0][0], stat_pulse_width_gen[0][0], stat_pulse_position[0][0][0], stat_pulse_width[0][0][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, stat_lvl1a[0][0]);
	      FillPulseHistos(stat_pulse_position_gen[0][0], stat_pulse_width_gen[0][0], stat_pulse_position[0][0][1], stat_pulse_width[0][0][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, stat_lvl1a[0][0]);
	    }

	    // A Side High Gain
	    if (bcm_channel > 7 && bcm_channel < 12){
	      deltat_data s1a;
	      s1a.bcid = bcm_bcid;
	      s1a.position = bcm_pulse1position;
	      s1a.ecr = ecr;
	      s1a.detector = bcm_channel%4;
              s1a.lvl1a = bcm_lvl1a;
	      positions_A[1].push_back(s1a);
	      if (bcm_pulse2width != 0){
		deltat_data s2a;
		s2a.bcid = bcm_bcid;
		s2a.position = bcm_pulse2position;
		s2a.ecr = ecr;
	        s2a.detector = bcm_channel%4;
		s2a.lvl1a = bcm_lvl1a;
		positions_A[1].push_back(s2a);
	      }
	      FillPulseHistos(stat_pulse_position_gen[0][1], stat_pulse_width_gen[0][1], stat_pulse_position[0][1][0], stat_pulse_width[0][1][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, stat_lvl1a[0][1]);
	      FillPulseHistos(stat_pulse_position_gen[0][1], stat_pulse_width_gen[0][1], stat_pulse_position[0][1][1], stat_pulse_width[0][1][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, stat_lvl1a[0][1]);
	    }
	    // C Side Low Gain
	    if (bcm_channel > 3 && bcm_channel < 8){
	      deltat_data s1c;
	      s1c.bcid = bcm_bcid;
	      s1c.position = bcm_pulse1position;
	      s1c.ecr = ecr;
	      s1c.detector = bcm_channel%4;
              s1c.lvl1a = bcm_lvl1a;
	      positions_C[0].push_back(s1c);
	      if (bcm_pulse2width != 0){
		deltat_data s2c;
		s2c.bcid = bcm_bcid;
		s2c.position = bcm_pulse2position;
		s2c.ecr = ecr;
	        s2c.detector = bcm_channel%4;
		s2c.lvl1a = bcm_lvl1a;
		positions_C[0].push_back(s2c);
	      }
	      FillPulseHistos(stat_pulse_position_gen[1][0], stat_pulse_width_gen[1][0], stat_pulse_position[1][0][0], stat_pulse_width[1][0][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, stat_lvl1a[1][0]);
	      FillPulseHistos(stat_pulse_position_gen[1][0], stat_pulse_width_gen[1][0], stat_pulse_position[1][0][1], stat_pulse_width[1][0][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, stat_lvl1a[1][0]);
	    }
	    // C Side High Gain
	    if (bcm_channel > 11){
	      deltat_data s1c;
	      s1c.bcid = bcm_bcid;
	      s1c.position = bcm_pulse1position;
	      s1c.ecr = ecr;
	      s1c.detector = bcm_channel%4;
              s1c.lvl1a = bcm_lvl1a;
	      positions_C[1].push_back(s1c);
	      if (bcm_pulse2width != 0){
		deltat_data s2c;
		s2c.bcid = bcm_bcid;
		s2c.position = bcm_pulse2position;
		s2c.ecr = ecr;
	        s2c.detector = bcm_channel%4;
		s2c.lvl1a = bcm_lvl1a;
		positions_C[1].push_back(s2c);
	      }
	      FillPulseHistos(stat_pulse_position_gen[1][1], stat_pulse_width_gen[1][1], stat_pulse_position[1][1][0], stat_pulse_width[1][1][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, stat_lvl1a[1][1]);
	      FillPulseHistos(stat_pulse_position_gen[1][1], stat_pulse_width_gen[1][1], stat_pulse_position[1][1][1], stat_pulse_width[1][1][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, stat_lvl1a[1][1]);
	    }
	          
	    /****************************************************************************
	     *And filling the detector histograms
	     ****************************************************************************/
	    switch (bcm_channel){
	    case 0:
	      FillPulseHistos(det_pulse_position_gen[0][0], det_pulse_width_gen[0][0], det_pulse_position[0][0][0], det_pulse_width[0][0][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, det_lvl1a[0][0]);
	      FillPulseHistos(det_pulse_position_gen[0][0], det_pulse_width_gen[0][0], det_pulse_position[0][0][1], det_pulse_width[0][0][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, det_lvl1a[0][0]);
	      break;
	    case 1:
	      FillPulseHistos(det_pulse_position_gen[1][0], det_pulse_width_gen[1][0], det_pulse_position[1][0][0], det_pulse_width[1][0][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, det_lvl1a[1][0]);
	      FillPulseHistos(det_pulse_position_gen[1][0], det_pulse_width_gen[1][0], det_pulse_position[1][0][1], det_pulse_width[1][0][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, det_lvl1a[1][0]);
	      break;
	    case 2:
	      FillPulseHistos(det_pulse_position_gen[2][0], det_pulse_width_gen[2][0], det_pulse_position[2][0][0], det_pulse_width[2][0][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, det_lvl1a[2][0]);
	      FillPulseHistos(det_pulse_position_gen[2][0], det_pulse_width_gen[2][0], det_pulse_position[2][0][1], det_pulse_width[2][0][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, det_lvl1a[2][0]);
	      break;
	    case 3:
	      FillPulseHistos(det_pulse_position_gen[3][0], det_pulse_width_gen[3][0], det_pulse_position[3][0][0], det_pulse_width[3][0][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, det_lvl1a[3][0]);
	      FillPulseHistos(det_pulse_position_gen[3][0], det_pulse_width_gen[3][0], det_pulse_position[3][0][1], det_pulse_width[3][0][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, det_lvl1a[3][0]);
	      break;
	    case 4:
	      FillPulseHistos(det_pulse_position_gen[4][0], det_pulse_width_gen[4][0], det_pulse_position[4][0][0], det_pulse_width[4][0][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, det_lvl1a[4][0]);
	      FillPulseHistos(det_pulse_position_gen[4][0], det_pulse_width_gen[4][0], det_pulse_position[4][0][1], det_pulse_width[4][0][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, det_lvl1a[4][0]);
	      break;
	    case 5:
	      FillPulseHistos(det_pulse_position_gen[5][0], det_pulse_width_gen[5][0], det_pulse_position[5][0][0], det_pulse_width[5][0][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, det_lvl1a[5][0]);
	      FillPulseHistos(det_pulse_position_gen[5][0], det_pulse_width_gen[5][0], det_pulse_position[5][0][1], det_pulse_width[5][0][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, det_lvl1a[5][0]);
	      break;
	    case 6:
	      FillPulseHistos(det_pulse_position_gen[6][0], det_pulse_width_gen[6][0], det_pulse_position[6][0][0], det_pulse_width[6][0][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, det_lvl1a[6][0]);
	      FillPulseHistos(det_pulse_position_gen[6][0], det_pulse_width_gen[6][0], det_pulse_position[6][0][1], det_pulse_width[6][0][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, det_lvl1a[6][0]);
	      break;
	    case 7:
	      FillPulseHistos(det_pulse_position_gen[7][0], det_pulse_width_gen[7][0], det_pulse_position[7][0][0], det_pulse_width[7][0][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, det_lvl1a[7][0]);
	      FillPulseHistos(det_pulse_position_gen[7][0], det_pulse_width_gen[7][0], det_pulse_position[7][0][1], det_pulse_width[7][0][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, det_lvl1a[7][0]);
	      break;
	    case 8:
	      FillPulseHistos(det_pulse_position_gen[0][1], det_pulse_width_gen[0][1], det_pulse_position[0][1][0], det_pulse_width[0][1][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, det_lvl1a[0][1]);
	      FillPulseHistos(det_pulse_position_gen[0][1], det_pulse_width_gen[0][1], det_pulse_position[0][1][1], det_pulse_width[0][1][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, det_lvl1a[0][1]);
	      break;
	    case 9:
	      FillPulseHistos(det_pulse_position_gen[1][1], det_pulse_width_gen[1][1], det_pulse_position[1][1][0], det_pulse_width[1][1][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, det_lvl1a[1][1]);
	      FillPulseHistos(det_pulse_position_gen[1][1], det_pulse_width_gen[1][1], det_pulse_position[1][1][1], det_pulse_width[1][1][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, det_lvl1a[1][1]);
	      break;
	    case 10:
	      FillPulseHistos(det_pulse_position_gen[2][1], det_pulse_width_gen[2][1], det_pulse_position[2][1][0], det_pulse_width[2][1][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, det_lvl1a[2][1]);
	      FillPulseHistos(det_pulse_position_gen[2][1], det_pulse_width_gen[2][1], det_pulse_position[2][1][1], det_pulse_width[2][1][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, det_lvl1a[2][1]);
	      break;
	    case 11:
	      FillPulseHistos(det_pulse_position_gen[3][1], det_pulse_width_gen[3][1], det_pulse_position[3][1][0], det_pulse_width[3][1][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, det_lvl1a[3][1]);
	      FillPulseHistos(det_pulse_position_gen[3][1], det_pulse_width_gen[3][1], det_pulse_position[3][1][1], det_pulse_width[3][1][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, det_lvl1a[3][1]);
	      break;
	    case 12:
	      FillPulseHistos(det_pulse_position_gen[4][1], det_pulse_width_gen[4][1], det_pulse_position[4][1][0], det_pulse_width[4][1][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, det_lvl1a[4][1]);
	      FillPulseHistos(det_pulse_position_gen[4][1], det_pulse_width_gen[4][1], det_pulse_position[4][1][1], det_pulse_width[4][1][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, det_lvl1a[4][1]);
	      break;
	    case 13:
	      FillPulseHistos(det_pulse_position_gen[5][1], det_pulse_width_gen[5][1], det_pulse_position[5][1][0], det_pulse_width[5][1][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, det_lvl1a[5][1]);
	      FillPulseHistos(det_pulse_position_gen[5][1], det_pulse_width_gen[5][1], det_pulse_position[5][1][1], det_pulse_width[5][1][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, det_lvl1a[5][1]);
	      break;
	    case 14:
	      FillPulseHistos(det_pulse_position_gen[6][1], det_pulse_width_gen[6][1], det_pulse_position[6][1][0], det_pulse_width[6][1][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, det_lvl1a[6][1]);
	      FillPulseHistos(det_pulse_position_gen[6][1], det_pulse_width_gen[6][1], det_pulse_position[6][1][1], det_pulse_width[6][1][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, det_lvl1a[6][1]);
	      break;
	    case 15:
	      FillPulseHistos(det_pulse_position_gen[7][1], det_pulse_width_gen[7][1], det_pulse_position[7][1][0], det_pulse_width[7][1][0], bcm_pulse1position, bcm_pulse1width, bcm_lvl1a, det_lvl1a[7][1]);
	      FillPulseHistos(det_pulse_position_gen[7][1], det_pulse_width_gen[7][1], det_pulse_position[7][1][1], det_pulse_width[7][1][1], bcm_pulse2position, bcm_pulse2width, bcm_lvl1a, det_lvl1a[7][1]);
	      break;
	    }
	  }
	}// End of for BCM_RDO_element
      }
    } // End of for BCM_RDO_itr 
    

    /********************************************    
     *count pixel hits
     ********************************************/    
    unsigned int n_pix_hits[3] = {0};

    if(m_pixRdoContainer != 0){
        PixelRDO_Container::const_iterator colNextpix = m_pixRdoContainer->begin();
        PixelRDO_Container::const_iterator colNextpix_end = m_pixRdoContainer->end();
        for( ; colNextpix != colNextpix_end; ++colNextpix){
            const InDetRawDataCollection<PixelRDORawData>* PIX_Collection(*colNextpix);
	    if(!PIX_Collection) continue;

            Identifier id = PIX_Collection->identify();
            switch ( m_pixelID->barrel_ec( id )  )
                    {
                    case 0:   // Barrel
                        n_pix_hits[1] += PIX_Collection->size();
                        break;
                    case -2:  // ECA
                        n_pix_hits[0] += PIX_Collection->size();
                        break;
                    case 2:   // ECC
                        n_pix_hits[2] += PIX_Collection->size();
                        break;
                    }
        }
    }


    /********************************************    
     *count sct hits
     ********************************************/    
    unsigned int n_sct_hits[3] = {0};

    if(m_sctRdoContainer != 0){
        SCT_RDO_Container::const_iterator colNextsct = m_sctRdoContainer->begin();
        SCT_RDO_Container::const_iterator colNextsct_end = m_sctRdoContainer->end();
        for( ; colNextsct != colNextsct_end; ++colNextsct){
            const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection(*colNextsct);
	    if(!SCT_Collection) continue;

            Identifier id = SCT_Collection->identify();
            switch ( m_sctID->barrel_ec( id )  )
                    {
                    case 0:   // Barrel
                        n_sct_hits[1] += SCT_Collection->size();
                        break;
                    case -2:  // ECA
                        n_sct_hits[0] += SCT_Collection->size();
                        break;
                    case 2:   // ECC
                        n_sct_hits[2] += SCT_Collection->size();
                        break;
                    }
        }
    }

    /********************************************    
     *Delta t histograms
     ********************************************/
    for(unsigned int gain =0;gain<2;gain++){
     
      positions_A[gain].sort();
      positions_C[gain].sort();

      /*
	Code to calculate the weight for the event. Could be useful in the future
	but taken out now as it is unused at the moment, causing warnings during
	compilation.
	
      float weight = 1.0;
      float ndt = 0;

      for (std::list<deltat_data>::iterator it_a =positions_A[gain].begin();it_a!=positions_A[gain].end();it_a++)
          for (std::list<deltat_data>::iterator it_c =positions_C[gain].begin();it_c!=positions_C[gain].end();it_c++)
              if ( (*it_a).bcid == (*it_c).bcid && (*it_a).lvl1a == BCM_LVL1A && (*it_c).lvl1a == BCM_LVL1A )
                  ndt++;

      if ( ndt )
	  weight = 1 / ndt;
      */
      float deltat=350.0;//so that it can't mix with normal data if calculation of deltat fails
      while (!positions_A[gain].empty()){
	unsigned int bcid=(positions_A[gain].front()).bcid;
	unsigned int detector_a=(positions_A[gain].front()).detector;


	for (std::list<deltat_data>::iterator it_c =positions_C[gain].begin();it_c!=positions_C[gain].end();it_c++){
	  if(bcid<(*it_c).bcid) continue;
	  if (bcid==(*it_c).bcid) { // i.e. (positions_A[gain].front()).bcid == (positions_C[gain].begin()).bcid
	    int deltatbins=(*it_c).position-(positions_A[gain].front()).position;
	    deltat = deltatbins/64.0*25.0;
	    deltat_vs_hits[gain]->Fill(deltat);
	    deltat_vs_bcid[gain]->Fill((*it_c).bcid, deltat);
	    deltat_vs_lb[gain]->Fill(m_current_LB, deltat);
	    deltat_vs_ecr[gain]->Fill((*it_c).ecr, deltat);
	    deltat_vs_PrVertex[gain]->Fill(z, deltat);
	    deltat_vs_pixhits[gain]->Fill(n_pix_hits[0]+n_pix_hits[1]+n_pix_hits[2], deltat);
	    deltat_vs_pixhitsBR[gain]->Fill(n_pix_hits[1], deltat);
	    deltat_vs_pixhitsEC[gain]->Fill(n_pix_hits[0]+n_pix_hits[2], deltat);
	    deltat_vs_scthits[gain]->Fill(n_sct_hits[0]+n_sct_hits[1]+n_sct_hits[2], deltat);
	    deltat_vs_scthitsBR[gain]->Fill(n_sct_hits[1], deltat);
	    deltat_vs_scthitsEC[gain]->Fill(n_sct_hits[0]+n_sct_hits[2], deltat);
	    if (abs(deltat) < 6.25) sct_vs_pix_col[gain]->Fill(n_sct_hits[0]+n_sct_hits[1]+n_sct_hits[2], n_pix_hits[0]+n_pix_hits[1]+n_pix_hits[2]);
	    else sct_vs_pix_bkg[gain]->Fill(n_sct_hits[0]+n_sct_hits[1]+n_sct_hits[2], n_pix_hits[0]+n_pix_hits[1]+n_pix_hits[2]);
	    if (detector_a==(*it_c).detector) {
	      deltat_aligned_vs_ecr[gain]->Fill(ecr,deltat);
	      deltat_aligned_vs_bcid[gain]->Fill(bcid,deltat);
	      deltat_aligned_vs_lb[gain]->Fill(m_current_LB, deltat);
	      deltat_aligned_vs_hits[gain]->Fill(deltat);
	    }
	  }//end of bcid if
	}//end of c loop
	bcid_max=(positions_A[gain].front()).bcid;
	positions_C[gain].remove_if(bcid_select);
	positions_A[gain].pop_front();
      }//end of a loop
    }//end of gain loop


	  /*********************************
	  *Filling Abort Fraction arrays
	  **********************************/

    /*for(int i=0; i < 31; i++){
      
      ATH_MSG_WARNING("nROD0HitLG["<<i<<"]: " <<nROD0HitLG[i] );
      ATH_MSG_WARNING("nROD1HitLG["<<i<<"]: " <<nROD1HitLG[i] );
      ATH_MSG_WARNING("nROD0HitHG["<<i<<"]: " <<nROD0HitHG[i] );
      ATH_MSG_WARNING("nROD1HitHG["<<i<<"]: " <<nROD1HitHG[i] );
    } */

    for(unsigned int bc_counter = 0; bc_counter < bc_readout; bc_counter++){
      double nROD0abortfraction = 100*((nROD0HitLG[bc_counter]*11)+nROD0HitHG[bc_counter])/36.0;
      double nROD1abortfraction = 100*((nROD1HitLG[bc_counter]*11)+nROD1HitHG[bc_counter])/36.0;
      if (nROD0abortfraction != 0 || nROD1abortfraction !=0) {
	double nabortfraction = (nROD0abortfraction+nROD1abortfraction)/2;
	AbortFraction->Fill(nabortfraction);
	AbortFractionVsECR->Fill(ecr, nabortfraction);
	AbortFractionVsLB->Fill(m_current_LB, nabortfraction);
	if (nROD0BCID[bc_counter]==nROD1BCID[bc_counter]) {
	  AbortFractionVsBCID->Fill(nROD0BCID[bc_counter], nabortfraction);
	}
      }
      if (nROD0abortfraction != 0) {
	//ATH_MSG_WARNING("nROD0abortfraction: " << nROD0abortfraction);
	AbortFractionROD0->Fill(nROD0abortfraction);
	AbortFractionROD0VsECR->Fill(ecr,nROD0abortfraction);
	AbortFractionROD0VsBCID->Fill(nROD0BCID[bc_counter],nROD0abortfraction);
      }
      if (nROD1abortfraction != 0) {
	//ATH_MSG_WARNING("nROD1abortfraction: " << nROD1abortfraction);
	AbortFractionROD1->Fill(nROD1abortfraction);
	AbortFractionROD1VsECR->Fill(ecr,nROD1abortfraction);
	AbortFractionROD1VsBCID->Fill(nROD1BCID[bc_counter],nROD1abortfraction);
      }
    }
	  
    
  }// end if bcm_rdo not empty 
  return StatusCode::SUCCESS;
}

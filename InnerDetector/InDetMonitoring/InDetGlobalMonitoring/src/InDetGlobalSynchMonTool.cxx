/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file  InDetGlobalSynchMonTool.cxx
 * Implementation of inner detector global monitoring synch tool
 *
 * @author 
 * Anne-Catherine Le Bihan <Anne-Catherine.Le.Bihan@cern.ch> @n
 * Wouter Husbergen <Wouter.Husbergen@cern.ch> @n 
 * Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
 * Arshak Tonoyan <Arshak.Tonyoan@cern.ch> @n
 * Thomas Burgess <Thomas.Burgess@cern.ch> @n
 * Alex Kastanas <Alex.Kastanas@cern.ch>      
 * 
 * $Id: InDetGlobalSynchMonTool.cxx,v 1.67 2009-04-07 01:25:31 kastanas Exp $
 *
 *****************************************************************************/

//Local
#include "InDetGlobalSynchMonTool.h"
#include "InDetGlobalMotherTrigMonTool.h"

//Framework
#include "InDetRawData/InDetTimeCollection.h"
#include "GaudiKernel/StatusCode.h"

#include "CommissionEvent/ComTime.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetRawData/SCT3_RawData.h"
#include "InDetRawData/PixelRDORawData.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetRawData/Pixel1RawData.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetPrepRawData/SiCluster.h"

#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"

//Root
#include "TProfile2D.h"
//Standard c++
#include <string>
#include <vector>
#include <set>
#include <map>

#define ROB_PIXEL_BARREL 0x11
#define ROB_PIXEL_DISKS  0x12
#define ROB_PIXEL_BLAYER 0x13 

#define ROB_SCT_BARREL_A 0x21
#define ROB_SCT_BARREL_C 0x22
#define ROB_SCT_EC_A     0x23
#define ROB_SCT_EC_C     0x24

#define ROB_TRT_BARREL_A 0x31
#define ROB_TRT_BARREL_C 0x32
#define ROB_TRT_EC_A     0x33
#define ROB_TRT_EC_C     0x34


InDetGlobalSynchMonTool::InDetGlobalSynchMonTool(
    const std::string & type, 
    const std::string & name,
    const IInterface* parent)			 
    : InDetGlobalMotherTrigMonTool(type, name, parent),
      m_tracks(0),
      m_n_sct_robs_JO(90),
      m_n_trt_robs_JO(190),
      m_n_pix_robs_JO(132),
      m_diff_LVL1ID(0),
      m_diff_Overview_LVL1ID(0),
      m_diff_BCID(0),
      m_diff_Pixel_SCT_TRT_BCID(0),
      m_diff_Overview_BCID(0),
      m_diff_SCT_BCID(0),
      m_diff_TRT_BCID(0),
      m_diff_PIX_BCID(0), 
      m_diff_SCT_TRT_BCID(0),
      m_diff_SCT_PIX_BCID(0),
      m_diff_SCT_TRT_BCID_evt(0),
      m_diff_SCT_PIX_BCID_evt(0),
      m_BCID(0),
      m_det_diff_BCID(0),
      m_LVL1ID(0),
      m_det_diff_LVL1ID(0),
      m_SCT_BCID(0),
      m_TRT_BCID(0),
      m_PIX_BCID(0),
      m_Pixel_Hits_BCID(0),
      m_SCT_Hits_BCID(0),
      m_TRT_Hits_BCID(0),
      m_InDet_Hits_BCID(0),
      m_Tracks_BCID(0)
{
    // Get parameter values from jobOptions file 
    declareProperty("sct_robs_JO", m_n_sct_robs_JO);
    declareProperty("trt_robs_JO", m_n_trt_robs_JO);
    declareProperty("pix_robs_JO", m_n_pix_robs_JO); 
}

StatusCode InDetGlobalSynchMonTool::initialize(){
    if ( InDetGlobalMotherTrigMonTool::initialize().isFailure() )
    {
	if ( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << "Could not initialize Monitor tool base!" << endmsg;
	return StatusCode::FAILURE;
    }
    
    m_robMapping.clear();
    
    m_robMapping.insert( std::pair<short,short>(ROB_PIXEL_BARREL, 0) );
    m_robMapping.insert( std::pair<short,short>(ROB_PIXEL_DISKS,  0) );
    m_robMapping.insert( std::pair<short,short>(ROB_PIXEL_BLAYER, 0) );
    m_robMapping.insert( std::pair<short,short>(ROB_SCT_BARREL_A, 1) );
    m_robMapping.insert( std::pair<short,short>(ROB_SCT_BARREL_C, 1) );
    m_robMapping.insert( std::pair<short,short>(ROB_SCT_EC_A,     1) );
    m_robMapping.insert( std::pair<short,short>(ROB_SCT_EC_C,     1) );
    m_robMapping.insert( std::pair<short,short>(ROB_TRT_BARREL_A, 2) );
    m_robMapping.insert( std::pair<short,short>(ROB_TRT_BARREL_C, 2) );
    m_robMapping.insert( std::pair<short,short>(ROB_TRT_EC_A,     2) );
    m_robMapping.insert( std::pair<short,short>(ROB_TRT_EC_C,     2) );

    m_robRegionMapping.clear();
    m_robRegionMapping.insert( std::pair<short,short>(ROB_PIXEL_BARREL, 0) );
    m_robRegionMapping.insert( std::pair<short,short>(ROB_PIXEL_DISKS,  1) );
    m_robRegionMapping.insert( std::pair<short,short>(ROB_PIXEL_BLAYER, 2) );
    m_robRegionMapping.insert( std::pair<short,short>(ROB_SCT_BARREL_A, 3) );
    m_robRegionMapping.insert( std::pair<short,short>(ROB_SCT_BARREL_C, 4) );
    m_robRegionMapping.insert( std::pair<short,short>(ROB_SCT_EC_A,     5) );
    m_robRegionMapping.insert( std::pair<short,short>(ROB_SCT_EC_C,     6) );
    m_robRegionMapping.insert( std::pair<short,short>(ROB_TRT_BARREL_A, 7) );
    m_robRegionMapping.insert( std::pair<short,short>(ROB_TRT_BARREL_C, 8) );
    m_robRegionMapping.insert( std::pair<short,short>(ROB_TRT_EC_A,     9) );
    m_robRegionMapping.insert( std::pair<short,short>(ROB_TRT_EC_C,    10) );

    return StatusCode::SUCCESS;
}


//----------------------------------------------------------------------
StatusCode InDetGlobalSynchMonTool::bookHistograms() 		   
{
    bool status = true;
   
    LWHist::LWHistAxis *axis = 0;	
    m_diff_LVL1ID = TProfile_LW::create( "m_diff_LVL1ID",
					 "Fraction of RODS with synchronized LVL1ID in each ID region",
					 11,0,11);
    axis = m_diff_LVL1ID->GetXaxis();
    axis->SetBinLabel(1,"Pixel Barrel");
    axis->SetBinLabel(2,"Pixel Disks");
    axis->SetBinLabel(3,"Pixel B-Layer");
    axis->SetBinLabel(4,"SCT Barrel A");
    axis->SetBinLabel(5,"SCT Barrel C");
    axis->SetBinLabel(6,"SCT ECA");
    axis->SetBinLabel(7,"SCT ECC");
    axis->SetBinLabel(8,"TRT Barrel A");
    axis->SetBinLabel(9,"TRT Barrel C");
    axis->SetBinLabel(10,"TRT ECA");
    axis->SetBinLabel(11,"TRT ECC"); 
    m_diff_LVL1ID->GetYaxis()->SetTitle("Fraction");
    
    status &= regHist(m_diff_LVL1ID, "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    
    m_diff_Overview_LVL1ID = TH2I_LW::create(
	"m_diff_Overview_LVL1ID"
	,"LVL1ID difference warning between the detectors and the RODs of each detector",
	4,-1,3,4,-1,3);
    axis = m_diff_Overview_LVL1ID->GetXaxis();
    axis->SetBinLabel(1,"BCM");
    axis->SetBinLabel(2,"Pixel");
    axis->SetBinLabel(3,"SCT");
    axis->SetBinLabel(4,"TRT");
    axis = m_diff_Overview_LVL1ID->GetYaxis();
    axis->SetBinLabel(1,"BCM");  
    axis->SetBinLabel(2,"Pixel");
    axis->SetBinLabel(3,"SCT");
    axis->SetBinLabel(4,"TRT");
    status &= regHist( m_diff_Overview_LVL1ID, "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    
    m_diff_BCID = TProfile_LW::create( "m_diff_BCID",
				       "Fraction of RODS with synchronized BCID in each ID region",
				       11,0,11);
    m_diff_BCID->GetYaxis()->SetTitle("Fraction");
    axis = m_diff_BCID->GetXaxis();
    axis->SetBinLabel(1,"Pixel Barrel");
    axis->SetBinLabel(2,"Pixel Disks");
    axis->SetBinLabel(3,"Pixel B-Layer");
    axis->SetBinLabel(4,"SCT Barrel A");
    axis->SetBinLabel(5,"SCT Barrel C");
    axis->SetBinLabel(6,"SCT ECA");
    axis->SetBinLabel(7,"SCT ECC");
    axis->SetBinLabel(8,"TRT Barrel A");
    axis->SetBinLabel(9,"TRT Barrel C");
    axis->SetBinLabel(10,"TRT ECA");
    axis->SetBinLabel(11,"TRT ECC");
    status &= regHist( m_diff_BCID, "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    
    m_diff_Pixel_SCT_TRT_BCID = TH2I_LW::create( "m_diff_Pixel_SCT_TRT_BCID"
						 ,"BCID difference warning status between SCT and Pixel vs between SCT and TRT",
						 2,-1,1,2,-1,1);
    axis = m_diff_Pixel_SCT_TRT_BCID->GetXaxis();
    axis->SetBinLabel(1,"SCT!= TRT");
    axis->SetBinLabel(2,"SCT == TRT");
    axis = m_diff_Pixel_SCT_TRT_BCID->GetYaxis();
    axis->SetBinLabel(1,"SCT != Pix");
    axis->SetBinLabel(2,"SCT == Pix");
    status &= regHist( m_diff_Pixel_SCT_TRT_BCID, "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    
    m_diff_Overview_BCID = TH2I_LW::create( "m_diff_Overview_BCID"
					    ,"BCID difference warning between the detectors and the RODs of each detector",
					    4,-1,3,4,-1,3);
    axis = m_diff_Overview_BCID->GetXaxis();
    axis->SetBinLabel(1,"BCM");
    axis->SetBinLabel(2,"Pixel");
    axis->SetBinLabel(3,"SCT");
    axis->SetBinLabel(4,"TRT");
    axis = m_diff_Overview_BCID->GetYaxis();
    axis->SetBinLabel(1,"BCM");
    axis->SetBinLabel(2,"Pixel");
    axis->SetBinLabel(3,"SCT");
    axis->SetBinLabel(4,"TRT");
    status &= regHist( m_diff_Overview_BCID, "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    
    
    m_diff_SCT_TRT_BCID = TH1I_LW::create( "m_diff_SCT_TRT_BCID"
					   ,"BCID difference between SCT and TRT",
					   20000,-10000,10000);
    status &= regHist( m_diff_SCT_TRT_BCID, "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    
    m_diff_SCT_PIX_BCID = TH1I_LW::create( "m_diff_SCT_PIX_BCID",
					   "BCID difference between SCT and PIX",
					   20000,-10000,10000);
    status &= regHist( m_diff_SCT_PIX_BCID, "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    
    
    m_det_diff_BCID.clear();
    m_det_diff_BCID.push_back( TH2I_LW::create( "m_diff_PIX_BCID",
						"BCID warning status between PIX RODS",
						m_n_pix_robs_JO-1,0,m_n_pix_robs_JO-1,
						2,-1,1) );
    status &= regHist( m_det_diff_BCID[0], "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    
    m_det_diff_BCID.push_back( TH2I_LW::create( "m_diff_SCT_BCID",
						"BCID warning status between SCT RODS",
						m_n_sct_robs_JO-1,0, m_n_sct_robs_JO-1,
						2,-1,1) );
    status &= regHist( m_det_diff_BCID[1], "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    
    m_det_diff_BCID.push_back( TH2I_LW::create( "m_diff_TRT_BCID",
						"BCID warning status between TRT RODS",
						m_n_trt_robs_JO-1,0,m_n_trt_robs_JO-1,
						2,-1,1) );
    status &= regHist( m_det_diff_BCID[2] , "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    
    
    m_det_diff_LVL1ID.clear();
    m_det_diff_LVL1ID.push_back( TH2I_LW::create("m_diff_PIX_LVL1ID",
						 "LVL1ID warning status between PIX RODS",
						 m_n_pix_robs_JO-1,0,m_n_pix_robs_JO-1,
						 2,-1,1) );
    status &= regHist( m_det_diff_LVL1ID[0], "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    
    m_det_diff_LVL1ID.push_back( TH2I_LW::create("m_diff_SCT_LVL1ID",
						 "LVL1ID warning status between SCT RODS",
						 m_n_sct_robs_JO-1,0,m_n_sct_robs_JO-1,
						 2,-1,1) );
    status &= regHist( m_det_diff_LVL1ID[1], "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    
    m_det_diff_LVL1ID.push_back( TH2I_LW::create("m_diff_TRT_LVL1ID",
						 "LVL1ID warning status between TRT RODS",
						 m_n_trt_robs_JO-1,0,m_n_trt_robs_JO-1,
						 2,-1,1) );
    status &= regHist( m_det_diff_LVL1ID[2], "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    
    m_BCID.clear();
    m_BCID.push_back( TH2I_LW::create( "m_PIX_BCID", 
				       "BCID of PIX RODS",
				       m_n_pix_robs_JO,-0.5,m_n_pix_robs_JO-0.5,
				       5000,0,5000 ) );
    status  &= regHist( m_BCID[0], "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    m_BCID.push_back( TH2I_LW::create( "m_SCT_BCID", 
				       "BCID of SCT RODS",
				       m_n_sct_robs_JO,-0.5,m_n_sct_robs_JO-0.5,
				       5000,0,5000 ) );
    status  &= regHist( m_BCID[1], "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    m_BCID.push_back( TH2I_LW::create( "m_TRT_BCID", 
				       "BCID of TRT RODS",
				       m_n_trt_robs_JO,-0.5,m_n_trt_robs_JO-0.5,
				       5000,0,5000 ) );
    status  &= regHist( m_BCID[2], "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    
    
    m_LVL1ID.clear();
    m_LVL1ID.push_back( TH2I_LW::create( "m_PIX_LVL1ID",                                                                                                                                                                                 
					 "LVL1ID of PIX RODS",                                                                                                                                                                           
					 m_n_pix_robs_JO,-0.5,m_n_pix_robs_JO-0.5,                                                                                                                                                       
					 5000,0,5000 ) );
    status  &= regHist( m_LVL1ID[0], "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    m_LVL1ID.push_back( TH2I_LW::create( "m_SCT_LVL1ID",                                                                                                                                                                                 
					 "LVL1ID of SCT RODS",                                                                                                                                                                           
					 m_n_sct_robs_JO,-0.5,m_n_sct_robs_JO-0.5,                                                                                                                                                       
					 5000,0,5000 ) );
    status  &= regHist( m_LVL1ID[1], "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    m_LVL1ID.push_back( TH2I_LW::create( "m_TRT_LVL1ID",                                                                                                                                                                                 
					 "LVL1ID of TRT RODS",                                                                                                                                                                           
					 m_n_trt_robs_JO,-0.5,m_n_trt_robs_JO-0.5,                                                                                                                                                       
					 5000,0,5000 ) );
    status  &= regHist( m_LVL1ID[2], "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();
    
    m_InDet_Hits_BCID = new TProfile( "m_InDet_Hits_BCID","Number of Inner Detector (Pixel + SCT + TRT) hits vs BCID",
				      5000,0,5000);
    m_InDet_Hits_BCID->GetXaxis()->SetTitle("BCID");
    m_InDet_Hits_BCID->GetYaxis()->SetTitle("Total number of inner detector hits");
    status &= regHist( m_InDet_Hits_BCID, "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess(); 
    
    m_Pixel_Hits_BCID  = new TProfile( "m_Pixel_Hits_BCID","Number of Pixel hits vs Pixel BCID",
				       500,0,5000);
    m_Pixel_Hits_BCID->GetXaxis()->SetTitle("BCID");
    m_Pixel_Hits_BCID->GetYaxis()->SetTitle("Number of Pixel hits");
    status &= regHist( m_Pixel_Hits_BCID, "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess();  
    
    m_SCT_Hits_BCID  = new TProfile( "m_SCT_Hits_BCID","Number of SCT hits vs SCT BCID",
				     500,0,5000);
    m_SCT_Hits_BCID->GetXaxis()->SetTitle("BCID");
    m_SCT_Hits_BCID->GetYaxis()->SetTitle("Number of SCT hits");
    status &= regHist( m_SCT_Hits_BCID, "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess(); 
    
    m_TRT_Hits_BCID  = new TProfile( "m_TRT_Hits_BCID","Number of TRT hits vs TRT BCID",
				     500,0,5000);
    m_TRT_Hits_BCID->GetXaxis()->SetTitle("BCID");
    m_TRT_Hits_BCID->GetYaxis()->SetTitle("Number of TRT hits");
    status &= regHist( m_TRT_Hits_BCID, "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess(); 
    
    m_Tracks_BCID  = new TProfile( "m_Tracks_BCID","Number of Tracks vs BCID",
				   5000,0,5000);
    m_Tracks_BCID->GetXaxis()->SetTitle("BCID");
    m_Tracks_BCID->GetYaxis()->SetTitle("Number of tracks");
    status &= regHist( m_Tracks_BCID , "InDetGlobal/Synch", run, ATTRIB_MANAGED ).isSuccess(); 
    
    return ( status ? StatusCode::SUCCESS : StatusCode::FAILURE );
}


//----------------------------------------------------------------------
StatusCode InDetGlobalSynchMonTool::fillHistograms()
{
    int prev_bcm_bcid = -1;
    int prev_bcm_lvl1id = -1;

    if ( m_doTrigger == true && InDetGlobalMotherTrigMonTool::CheckTriggers() != StatusCode::SUCCESS ){
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Triggers not found!" << endmsg;
	m_doTrigger = false;
    }
      
    //m_doTrigger = false;  // REMOVE

    if ( m_BCM_RDO != 0 ) // Protection for when there is no BCM data
    {  
	// loop over RDO container

	BCM_RDO_Container::const_iterator it_bcmraw,it_bcmraw_E(m_BCM_RDO->end());
	for ( it_bcmraw = m_BCM_RDO->begin(); it_bcmraw != it_bcmraw_E; ++it_bcmraw) {
	    if ( (*it_bcmraw)->size() != 0) {
		BCM_RDO_Collection::const_iterator RDO_element_first = (*it_bcmraw)->begin();
		int bcm_bcid   = (*RDO_element_first)->getBCID();
		int bcm_lvl1id = (*RDO_element_first)->getLVL1ID();
		
		if ( bcm_bcid != prev_bcm_bcid && prev_bcm_bcid != -1 ) 
		{
		    m_diff_Overview_BCID->Fill(-1.0,-1.0);
		}
		if ( bcm_lvl1id != prev_bcm_lvl1id && prev_bcm_lvl1id != -1 )
		{
		    m_diff_Overview_LVL1ID->Fill(-1.0,-1.0);
		}
		prev_bcm_bcid = bcm_bcid;
		prev_bcm_lvl1id = bcm_lvl1id;
	    }
	}
    }
    
    //Number of hits in the whole Inner Setector (Pixel+SCT+TRT)
    int n_InDet_hits = 0;

    unsigned int prev_pix_bcid = 0;
    //Fill Pixel BCID histograms    
    if ( m_pixBCIDContainer )
        prev_pix_bcid = fillTimingHistos( m_pixBCIDContainer,
					  m_BCID, 
					  m_det_diff_BCID,
					  m_diff_BCID,
					  m_diff_Overview_BCID,
					  &getBCID ); 
    unsigned int prev_sct_bcid = 0;
    //Fill SCT BCID histograms   
    if ( m_sctBCIDContainer )
	prev_sct_bcid = fillTimingHistos( m_sctBCIDContainer ,
					  m_BCID, 
					  m_det_diff_BCID,
					  m_diff_BCID,
					  m_diff_Overview_BCID,
					  &getBCID );
    unsigned int prev_trt_bcid = 0;
    //Fill TRT BCID histograms   
    if ( m_trtBCIDContainer )
	prev_trt_bcid = fillTimingHistos( m_trtBCIDContainer ,
					  m_BCID, 
					  m_det_diff_BCID,
					  m_diff_BCID,
					  m_diff_Overview_BCID,
					  &getBCID );
    unsigned int prev_pix_lvl1id = 0;
    //Fill Pixel Level 1 ID difference histograms 
    if ( m_pixLVL1IDContainer )
	prev_pix_lvl1id = fillTimingHistos( m_pixLVL1IDContainer,
					    m_LVL1ID,
					    m_det_diff_LVL1ID,
					    m_diff_LVL1ID,
					    m_diff_Overview_LVL1ID,
					    &getLVL1ID );
    
    unsigned int prev_sct_lvl1id = 0;
    //Fill SCT Level 1 ID difference histograms 
    if ( m_sctLVL1IDContainer )
	prev_sct_lvl1id = fillTimingHistos( m_sctLVL1IDContainer,
					    m_LVL1ID,
					    m_det_diff_LVL1ID,
					    m_diff_LVL1ID,
					    m_diff_Overview_LVL1ID,
					    &getLVL1ID );
    unsigned int prev_trt_lvl1id = 0;
    //Fill TRT LVL1ID difference histogram
    if ( m_trtLVL1IDContainer )
	prev_trt_lvl1id = fillTimingHistos( m_trtLVL1IDContainer,
					    m_LVL1ID,
					    m_det_diff_LVL1ID,
					    m_diff_LVL1ID,
					    m_diff_Overview_LVL1ID,
					    &getLVL1ID );
    
    m_diff_Pixel_SCT_TRT_BCID->Fill(((prev_sct_bcid-prev_trt_bcid==0)?0:-1),((prev_sct_bcid-prev_pix_bcid==0)?0:-1));

    
    if (prev_sct_bcid-prev_pix_bcid ) m_diff_Overview_BCID->Fill(0.0,1.0);
    if (prev_trt_bcid-prev_pix_bcid ) m_diff_Overview_BCID->Fill(0.0,2.0);
    if (prev_sct_bcid-prev_trt_bcid ) m_diff_Overview_BCID->Fill(1.0,2.0);
    if (prev_pix_bcid-prev_bcm_bcid && prev_bcm_bcid ) m_diff_Overview_BCID->Fill(-1.0,0.0);
    if (prev_sct_bcid-prev_bcm_bcid && prev_bcm_bcid ) m_diff_Overview_BCID->Fill(-1.0,1.0);
    if (prev_trt_bcid-prev_bcm_bcid && prev_bcm_bcid ) m_diff_Overview_BCID->Fill(-1.0,2.0);
    
    if (prev_sct_lvl1id-prev_pix_lvl1id != 0) m_diff_Overview_LVL1ID->Fill(0.0,1.0);
    if (prev_trt_lvl1id-prev_pix_lvl1id != 0) m_diff_Overview_LVL1ID->Fill(0.0,2.0);
    if (prev_sct_lvl1id-prev_trt_lvl1id != 0) m_diff_Overview_LVL1ID->Fill(1.0,2.0);
    if (prev_pix_lvl1id-prev_bcm_lvl1id != 0 && prev_bcm_lvl1id != 0) m_diff_Overview_LVL1ID->Fill(-1.0,0.0);
    if (prev_sct_lvl1id-prev_bcm_lvl1id != 0 && prev_bcm_lvl1id != 0) m_diff_Overview_LVL1ID->Fill(-1.0,1.0);
    if (prev_trt_lvl1id-prev_bcm_lvl1id != 0 && prev_bcm_lvl1id != 0) m_diff_Overview_LVL1ID->Fill(-1.0,2.0);
    

    m_tracks = m_combined_tracks;

    //Count and fill pix hits
    unsigned int n_Pixel_hits = getPixelHits( m_pixRdoContainer );
    m_Pixel_Hits_BCID->Fill(prev_pix_bcid,n_Pixel_hits,1);
    n_InDet_hits += n_Pixel_hits;

    //Count and fill sct hits
    unsigned int n_SCT_hits = getSctHits( m_sctRdoContainer );
    m_SCT_Hits_BCID->Fill(prev_sct_bcid,n_SCT_hits,1);
    n_InDet_hits += n_SCT_hits;
    // Fill the trigger related histograms
 
    if (m_driftCircleContainer!=0) {
	int n_TRT_hits=0;
	TRT_DriftCircleContainer::const_iterator it_col,it_col_E(m_driftCircleContainer->end());
	for ( it_col = m_driftCircleContainer->begin(); it_col != it_col_E;  ++it_col) {
	  
	    const TRT_DriftCircleCollection *colNext=&(**it_col);
	    if (!colNext) continue;
	    
	    n_TRT_hits = n_TRT_hits + colNext->size();
	}
	m_TRT_Hits_BCID->Fill(prev_trt_bcid,n_TRT_hits,1);
	n_InDet_hits = n_InDet_hits + n_TRT_hits;
    }
    
    m_InDet_Hits_BCID->Fill(prev_trt_bcid,(n_InDet_hits),1);
    int n_Tracks=(m_combined_tracks)?m_combined_tracks->size():0;
    m_Tracks_BCID->Fill(prev_trt_bcid,n_Tracks,1);
    
    return StatusCode::SUCCESS;
}


void InDetGlobalSynchMonTool::fillNTimesFast(TH2I_LW*h,double x,double y, int count)
{
  if (count<1)
    return;
  //Fill with weight:
  h->Fill(x,y,count);
  //Fix number of entries:
  h->SetEntries(h->GetEntries()+count-1);
  //Fix sumW2:
  double sumW,sumW2,sumWX,sumWX2,sumWY,sumWY2,sumWXY;
  h->getSums(sumW,sumW2,sumWX,sumWX2,sumWY,sumWY2,sumWXY);
  h->setSums(sumW,h->GetEntries(),sumWX,sumWX2,sumWY,sumWY2,sumWXY);
}

unsigned int InDetGlobalSynchMonTool::getPixelHits(const PixelRDO_Container * collection)
{
    int n_hits=0;
    if ( collection != 0 )
    {
	PixelRDO_Container::const_iterator it_pixcol,it_pixcol_E(collection->end());
	for ( it_pixcol=collection->begin(); it_pixcol != it_pixcol_E; ++it_pixcol) {
	    const InDetRawDataCollection<PixelRDORawData>* PIX_Collection(*it_pixcol);
	    if (PIX_Collection == 0) continue;
   
	    n_hits = n_hits + PIX_Collection->size();
	}
    }
    return n_hits;
}

unsigned int InDetGlobalSynchMonTool::getSctHits(const SCT_RDO_Container * collection )
{
    unsigned int n_SCT_hits =0 ;
    if ( m_sctRdoContainer != 0){
	SCT_RDO_Container::const_iterator it_sctcol,it_sctcol_E(collection->end());
	for (it_sctcol = collection->begin(); it_sctcol != it_sctcol_E; ++it_sctcol) {
	    const InDetRawDataCollection<SCT_RDORawData> * SCT_Collection(*it_sctcol);
	    if (SCT_Collection == 0) continue;
	    n_SCT_hits = n_SCT_hits + SCT_Collection->size();
	}
    }
    return n_SCT_hits;
}

unsigned int InDetGlobalSynchMonTool::getBCID(unsigned int bcid)
{
    return bcid;
}

unsigned int InDetGlobalSynchMonTool::getLVL1ID(unsigned int lvl1id)
{
    return (lvl1id & 0xffff);
}


unsigned int InDetGlobalSynchMonTool::fillTimingHistos( const InDetTimeCollection * collection,
							std::vector<TH2I_LW *> & timing,
							std::vector<TH2I_LW *> & timing_diff,
							TProfile_LW * timing_region_overview,
							TH2I_LW * timing_over,
							unsigned int (*procFunc)(unsigned int) )
{
    unsigned int nrobs = 0;
    unsigned int prev = 0; 

    InDetTimeCollection::const_iterator it_time,it_time_E(collection->end());
    for ( it_time = collection->begin(); it_time != it_time_E; ++it_time ) {
	
	if (!(*it_time)) continue;
	
	const unsigned int curr = (*procFunc)( (*it_time)->second );
	const int robid = getDetectorId ( (*it_time)->first  ); //remember only first 2 digits of ROBID
	
	std::multimap<short, short>::const_iterator curr_rob = m_robMapping.find(robid);
	
	int hist_index = 0;
	
	if( curr_rob != m_robMapping.end() )
	{
	    hist_index = m_robMapping.find(robid)->second;
	}
	else
	{
	    return 0;
	}
	
	timing.at(hist_index)->Fill(nrobs,curr);
	
	//Skip difference for first ROD - FIXME
	if ( it_time > collection->begin() ) {
	    //Fill 0 if bcid is same as last, -1 otherwise
	    timing_diff.at(hist_index)->Fill(nrobs-1,((prev-curr==0)?0:-1));
	    if (prev-curr != 0)
	    {
		timing_over->Fill(hist_index,hist_index);
	    }
	    else
	    {
	    }
	    
	    std::map<short, short>::const_iterator curr_region = m_robRegionMapping.find(robid);
	    if ( curr_region != m_robRegionMapping.end() ){
		timing_region_overview->Fill(m_robRegionMapping.find(robid)->second, ((prev-curr==0)?1:0));
	    }
	    else
	    {
		return 0;
	    }
	}
	
	prev = curr;
	nrobs++;
    } // End loop over timing collection
    return prev;
}

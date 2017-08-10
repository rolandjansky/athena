/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetGlobalNoiseOccupancyMonTool.cxx
* Implementation of inner detector global noise occupancy monitoring tool
*
* @author
* Anne-Catherine Le Bihan <Anne-Catherine.Le.Bihan@cern.ch> @n
* Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
* Arshak Tonoyan <Arshak.Tonyoan@cern.ch> @n
* Thomas Burgess <Thomas.Burgess@cern.ch> @n
* Alex Kastanas <Alex.Kastanas@cern.ch> @n
* Mary Twaddle <Mary.Ruth.Twaddle@cern.ch>
*
* $Id: InDetGlobalNoiseOccupancyMonTool.cxx,v 1.66 2009-05-14 22:43:43 kastanas Exp $
*
*****************************************************************************/


//Local
#include "InDetGlobalNoiseOccupancyMonTool.h"
//Framework
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetRawData/SCT3_RawData.h"
#include "InDetRawData/SCT_TB03_RawData.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "GaudiKernel/StatusCode.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"

//Root
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TProfile_LW.h"

//Standard c++
#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>

#define SCT_BARREL_SEL 0
#define SCT_POSEC_SEL  2
#define SCT_NEGEC_SEL -2


using namespace std;

namespace{
  //use anonymous namespace to restrict scope to this file, 
  // equivalent to 'static' keyword here
  // is a variable in the specified range?
  // __NOTE__
  // This code is copied from 
  // SCT_Monitoring/src/SCTHitsNoiseMonTool.cxx
  //Timing patterns for SCT (IXX is 1XX etc.)

  enum Pattern{ IXX=0, XIX, XXI };

  static bool timeBinInPattern(const int tbin, const Pattern xxx){
    switch(xxx){
    case IXX:
      return (tbin>3);
      break;
    case XIX:
      return(tbin==2||tbin==3||tbin==6||tbin==7);
      break;
    case XXI:
      return(tbin==1||tbin==3||tbin==5||tbin==7);
      break;
    default:
      cout << "A strange timepattern was found;"
	" this should never happen" << endl;
      return false;
      break;
    }
  }
} //end of anonymous namespace

//----------------------------------------------------------------------
InDetGlobalNoiseOccupancyMonTool::InDetGlobalNoiseOccupancyMonTool(
								   const string & type,
								   const string & name,
								   const IInterface* parent)
  :InDetGlobalMotherMonTool(type, name, parent),
   m_tracks(0),
   m_nStraws(0),
   m_nStraws_B(0),
   m_nStraws_ECA(0),
   m_nStraws_ECC(0),
   m_nStrips(0),
   m_nStrips_B(0),
   m_nStrips_ECA(0),
   m_nStrips_ECC(0),
   m_nPixels(0),
   m_nPixels_B(0),
   m_nPixels_ECA(0),
   m_nPixels_ECC(0),
   m_pixelMax(0.001),
   m_pixel_nBins(100),
   m_sctMax(0.001),
   m_sct_nBins(100),
   m_trtMax(0.03),
   m_trt_nBins(100),  
   m_start(true),
   m_doData(true),
   m_SCT_TRT_NO(0),
   m_SCT_PIX_NO(0),
   m_PIX_TRT_NO(0),
   m_PIX_SCT_TRT_NO(0),
   m_TRT_SCTvTRT_PIX_10evt(0),
   m_TRT_SCTvSCT_PIX_10evt(0),
   m_TRT_PIXvSCT_PIX_10evt(0),
   m_PIX_NO_evt(0),
   m_PIX_NO(0),
   m_PIX_NO_B(0),
   m_PIX_NO_ECA(0),
   m_PIX_NO_ECC(0),
   m_PIX_NO_nseg(0),
   m_PIX_NO_ntrk(0),
   m_SCT_NO_evt(0),
   m_SCT_NO(0),
   m_SCT_NO_B(0),
   m_SCT_NO_ECA(0),
   m_SCT_NO_ECC(0),
   m_SCT_NO_nseg(0),
   m_SCT_NO_ntrk(0),
   m_TRT_NO_evt(0),
   m_TRT_NO_BA_phi(0),
   m_TRT_NO_BC_phi(0),
   m_TRT_NO_ECA_phi(0),
   m_TRT_NO_ECC_phi(0),
   m_TRT_HL_NO_evt(0),
   m_TRT_NO(0),
   m_TRT_HL_NO(0),
   m_TRT_NO_nseg(0),
   m_TRT_NO_ntrk(0)
{
  declareProperty("doData", m_doData);
  declareProperty("pixelMax",m_pixelMax);
  declareProperty("pixelNBins",m_pixel_nBins);
  declareProperty("sctMax",m_sctMax);
  declareProperty("sctNBins",m_sct_nBins);
  declareProperty("trtMax",m_trtMax);
  declareProperty("trtNBins",m_trt_nBins);
}

/* Define Histograms */
//----------------------------------------------------------------------
StatusCode InDetGlobalNoiseOccupancyMonTool::bookHistogramsRecurrent()
{
    MonGroup monGr_exp(this, "InDetGlobal/NO", run);
    MonGroup monGr_bug(this, "InDetGlobal/NO", run);
    MonGroup monGr_shift(this,"InDetGlobal/NO", run);
    bool status = true;
    //--- Registration of histograms per run
    if(newRunFlag()){

	//--- ID histograms---------------------
	status &= registerHist( monGr_shift,  m_SCT_TRT_NO =
				       TH2F_LW::create("m_SCT_TRT_NO","TRT noise occupancy vs SCT noise occupancy",
						       m_sct_nBins,0,m_sctMax, m_trt_nBins,0,m_trtMax));
	status &= registerHist( monGr_shift,  m_SCT_PIX_NO =
				       TH2F_LW::create("m_SCT_PIX_NO","Pixel noise occupancy vs SCT noise occupancy",
						       m_sct_nBins, 0, m_sctMax, m_pixel_nBins, 0, m_pixelMax));
	status &= registerHist( monGr_shift,  m_PIX_TRT_NO = 
				       TH2F_LW::create("m_PIX_TRT_NO","TRT noise occupancy vs Pixel noise occupancy",
						       m_pixel_nBins, 0, m_pixelMax, m_trt_nBins, 0, m_trtMax));

	/*
	 * combined NO histos using correlation coefficient
	 * Added by m
	 */
	  
	status &= registerHist( monGr_shift,  m_TRT_SCTvTRT_PIX_10evt =
				       TH2F_LW::create("m_TRT_SCTvTRT_PIX_10evt",
						       "TRT,SCT, PIX combined "
						       "occupancy",100,-1,1,100,-1,1));
	status &= registerHist( monGr_shift,  m_TRT_SCTvSCT_PIX_10evt =
				       TH2F_LW::create("m_TRT_SCTvSCT_PIX_10evt",
						       "TRT,SCT, PIX combined "
						       "occupancy",100,-1,1,100,-1,1));
	status &= registerHist( monGr_shift,  m_TRT_PIXvSCT_PIX_10evt =
				       TH2F_LW::create("m_TRT_PIXvSCT_PIX_10evt",
						       "TRT,SCT, PIX combined "
						       "occupancy",100,-1,1,100,-1,1));
	  
	//---Pixel debug histograms
	status &= registerHist( monGr_shift,  m_PIX_NO_evt = 
				       TProfile_LW::create("m_PIX_NO_evt","PIX noise "
						    "occupancy vs bin of 10 events",m_checkRate/10,
						    0,m_checkRate));
	status &= registerHist( monGr_exp,  m_PIX_NO =
				     TH1F_LW::create("m_PIX_NO","PIX noise occupancy",
						     m_pixel_nBins,0,m_pixelMax));
	status &= registerHist( monGr_exp,  m_PIX_NO_B =
				     TH1F_LW::create("m_PIX_NO_B","PIX noise occupancy (B)",
						     m_pixel_nBins,0,m_pixelMax));
	status &= registerHist( monGr_exp,  m_PIX_NO_ECA =
				     TH1F_LW::create("m_PIX_NO_ECA","PIX noise occupancy (ECA)",
						     m_pixel_nBins,0,m_pixelMax));
	status &= registerHist( monGr_exp,  m_PIX_NO_ECC =
				     TH1F_LW::create("m_PIX_NO_ECC","PIX noise occupancy (ECC)",
						     m_pixel_nBins,0,m_pixelMax));
	status &= registerHist( monGr_exp,  m_PIX_NO_nseg = 
				     TH2F_LW::create("m_PIX_NO_nseg","PIX noise occupancy "
						     "vs number of PIX segments",500,0,500,
						     150,0,m_pixelMax));
	status &= registerHist( monGr_exp,  m_PIX_NO_ntrk = 
				     TH2F_LW::create("m_PIX_NO_ntrk","PIX noise occupancy "
						     "vs number of combinedtracks",500,0,500,
						     200,0,m_pixelMax));
	//---SCT debug histograms
	status &= registerHist( monGr_shift,  m_SCT_NO_evt = 
				       TProfile_LW::create("m_SCT_NO_evt","SCT noise "
						    "occupancy vs bin of 10 events",m_checkRate/10,
						    0,m_checkRate));
	status &= registerHist( monGr_exp,  m_SCT_NO =
				     TH1F_LW::create("m_SCT_NO","SCT noise occupancy",
						     100,0,m_sctMax));
	status &= registerHist( monGr_exp,  m_SCT_NO_B =
				     TH1F_LW::create("m_SCT_NO_B","SCT noise occupancy (B)",
						     100,0,m_sctMax));
	status &= registerHist( monGr_exp,  m_SCT_NO_ECA =
				     TH1F_LW::create("m_SCT_NO_ECA","SCT noise occupancy (ECA)",
						     100,0,m_sctMax));
	status &= registerHist( monGr_exp,  m_SCT_NO_ECC =
				     TH1F_LW::create("m_SCT_NO_ECC","SCT noise occupancy (ECC)",
						     100,0,m_sctMax));
	status &= registerHist( monGr_exp,  m_SCT_NO_nseg = 
				     TH2F_LW::create("m_SCT_NO_nseg","SCT noise occupancy "
						     "vs number of SCT segments"
						     ,50,0,50,150,0,m_sctMax));
	status &= registerHist( monGr_exp,  m_SCT_NO_ntrk =
				     TH2F_LW::create("m_SCT_NO_ntrk", "SCT noise occupancy "
						     "vs number of combined tracks",
						     100,0,100,200,0,m_sctMax));
	  
	//---TRT debug histograms
	status &= registerHist( monGr_shift,  m_TRT_NO_evt =
				       TProfile_LW::create("m_TRT_NO_evt","TRT noise "
						    "occupancy vs bin of 10 events",
						    m_checkRate/10,0,m_checkRate));
	status &= registerHist( monGr_exp,  m_TRT_NO =
				     TH1F_LW::create("m_TRT_NO","TRT noise occupancy",100,
						     0,m_trtMax));
	status &= registerHist( monGr_exp,  m_TRT_NO_BC_phi=
				     TProfile_LW::create("m_TRT_NO_BC_phi","TRT noise occupancy in phi sectors (Barrel C)",32,
						  -0.5,31.5,0,m_trtMax));
	status &= registerHist( monGr_exp,  m_TRT_NO_BA_phi=
				     TProfile_LW::create("m_TRT_NO_BA_phi","TRT noise occupancy in phi sectors (Barrel A)",32,
						  -0.5,31.5,0,m_trtMax));
	status &= registerHist( monGr_exp,  m_TRT_NO_ECA_phi=
				     TProfile_LW::create("m_TRT_NO_ECA_phi","TRT noise occupancy in phi sectors (ECA)",32,
						  -0.5,31.5,0,m_trtMax));
	status &= registerHist( monGr_exp,  m_TRT_NO_ECC_phi=
				     TProfile_LW::create("m_TRT_NO_ECC_phi","TRT noise occupancy in phi sectors (ECC)",32,
						  -0.5,31.5,0,m_trtMax));
    
	status &= registerHist( monGr_exp,  m_TRT_HL_NO_evt =
				     TProfile_LW::create("m_TRT_HL_NO_evt","TRT HL noise "
						  "occupancy vs event number",
						  m_checkRate,0,m_checkRate));
	status &= registerHist( monGr_exp,  m_TRT_HL_NO =
				     TH1F_LW::create("m_TRT_HL_NO","TRT HL noise occupancy",
						     100,0,m_trtMax));
	status &= registerHist( monGr_exp,  m_TRT_NO_nseg =
				     TH2F_LW::create("m_TRT_NO_nseg","TRT noise occupancy "
						     "vs number of TRT segments",500,0,500,
						     150,0,m_trtMax));
	status &= registerHist( monGr_exp,  m_TRT_NO_ntrk = 
				     TH2F_LW::create("m_TRT_NO_ntrk","TRT noise occupancy "
						     "vs number of combined tracks",					 
						     500,0,500,200,0,m_trtMax));
	  
	// To be moved to conditions data?
	m_nStraws = 0.;
	m_nStrips = 0.;
	m_nPixels = 0.;
	m_start= true; 
	  
	  
    }
    // if isNewRun
    /* FIXME
     * this if statement does nothing -- should it be paired with the 
     * return statement below?
     * compiler will just remove it as it is
     */
    if ( status ) return StatusCode::SUCCESS;
    else return StatusCode::FAILURE;
}


//----------------------------------------------------------------------
StatusCode InDetGlobalNoiseOccupancyMonTool::fillHistograms( )
{
    /* Output streams used for debug...
     * ofstream msg("create_vector.out");
     * msg << "m_evt at start of fillHistograms() " << m_evt << endl;
     */
    if(!evtStore()){
	if(msgLvl(MSG::DEBUG))
	    msg(MSG::DEBUG) << "No pointer to StoreGateSvc" 
			    << endmsg;
	return StatusCode::FAILURE;
    }
  
    // awful hack to be changed with condition databases ....

    /*
     * FOR REF: this hack contains lots of hard coded numbers 
     * which should be allocated dynamically
     */
  
    if(m_start){
	if(m_doData){
	    m_nStraws += 1642.*2.*32; // 2 sides 
	    m_nStraws_B = 1642.*2.*32;
      
	    m_nStrips_B = GetSCTCounts(SCT_BARREL_SEL, false);
	    m_nStrips += m_nStrips_B;
	    // full barrel...  check w/ experts if everything cabled up 
	    // 768 strips/rectangle, 2 rectangles, 12 in z, 4 #cylinders  
      
	    m_nPixels_B = 46080*(286+494+676);//GetPixelCounts(SCT_BARREL_SEL, false);
	    m_nPixels += m_nPixels_B;
	    //46080*(286+494+676); //GetPixelCounts(SCT_BARREL_SEL, false);
	    //47232 pixels per sensors, 3 cylinders
      
      
      
	    m_nStraws += 24.*16.*10.*32;
	    m_nStraws_ECA = 24.*16.*10.*32;

	    // 24 straws 16 layers 10 disks
	
	    m_nStrips_ECA = GetSCTCounts(SCT_NEGEC_SEL, false);
	    m_nStrips += m_nStrips_ECA;
	    // 52 modules/ring type 1 - 9 rings of type 1
	    // 40 modules/ring type 2,3,4  - 13 rings of type 2,3,4
	    // TDR p. 53,54 ... to be checked
	
	    m_nPixels_ECA = 46080*(48+48+48); //GetPixelCounts(SCT_NEGEC_SEL, false);
	    m_nPixels += m_nPixels_ECA;
	    //46080*(48+48+48); //GetPixelCounts(SCT_NEGEC_SEL, false);
	    // 47232 pixel46080s per sensors, 3 disks -->CHECK THIS
      
      
	    m_nStraws += 24.*16.*10.*32;
	    m_nStraws_ECC = 24.*16.*10.*32;

	    // 24 straws 16 layers 10 disks 
      
	    m_nStrips_ECC = GetSCTCounts(SCT_POSEC_SEL, false);
	    m_nStrips += m_nStrips_ECC;
      
	    m_nPixels_ECC = 46080*(48+48+48); //GetPixelCounts(SCT_POSEC_SEL, false);
	    m_nPixels += m_nPixels_ECC;
	    //46080*(48+48+48); //GetPixelCounts(SCT_POSEC_SEL, false);
      
	    m_start = false;
      
	}
    }
    // end of awful hack
    
    int nComb_trk=0;
	  
    if(m_combined_tracks)
	nComb_trk = m_combined_tracks->size();
	

    m_tracks = m_combined_tracks;
    m_dcidOnTrack.clear();

    //--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*
    // Loop over Combined tracks
    //--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*

    float nTRT_RDOs=0.;
    int nTRT_seg = 0;
    int nTRT_trk = 0;
    float nTRT_HL_RDOs=0.;
    float nTRT_HL_trk=0.;
    int nSCT_seg = 0;
    int nSCT_trk = 0;
    int nSCT_trk_B = 0;
    int nSCT_trk_ECA = 0;
    int nSCT_trk_ECC = 0;

    int nPIX_seg = 0;
    int nPIX_trk = 0;
    int nPIX_trk_B = 0;
    int nPIX_trk_ECA = 0;
    int nPIX_trk_ECC = 0;

    int m_TRT_nOnTrack_phi_BA[32] = {0};
    int m_TRT_nOnTrack_phi_BC[32] = {0};
    int m_TRT_nOnTrack_phi_ECA[32] = {0};
    int m_TRT_nOnTrack_phi_ECC[32] = {0};


    if (m_tracks) {
	TrackCollection::const_iterator itrack = m_tracks->begin();
	TrackCollection::const_iterator itrack_end = m_tracks->end();
	for ( ; itrack!=itrack_end ; ++itrack) {
	
	    // Get track
	    const Trk::Track *track=(*itrack);
	    if (track == 0){
		if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING)
		    << "no pointer to track!!!" << endmsg;  break;
	    }
	
	    const DataVector<const Trk::TrackStateOnSurface>*
		trackStates=track->trackStateOnSurfaces();
	    if (trackStates == 0) {
		if ( msgLvl(MSG::ERROR) ) msg(MSG::ERROR)
		    << "for current track is TrackStateOnSurfaces == Null "
		    << endmsg;
	    } else {
		DataVector<const Trk::TrackStateOnSurface>::const_iterator
		    it=trackStates->begin();
		DataVector<const Trk::TrackStateOnSurface>::const_iterator
		    it_end=trackStates->end();
		for ( ; it != it_end ; it++) {
		    const Trk::TrackStateOnSurface* trackState=(*it);
	    
		    if (trackState == 0) {
			if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "TrackStateOnSurface == Null, is the tracking OK?" << endmsg;
			continue;
		    }
		    const InDet::TRT_DriftCircleOnTrack *trtcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(trackState->measurementOnTrack());
		    if (trtcircle!=0) {
			nTRT_trk++;
			m_dcidOnTrack.push_back(trtcircle->identify());
			Identifier rdoId  = trtcircle->identify();
			// Fill the number of RDO per phi sector arrays.
			switch ( m_trtID->barrel_ec(rdoId) )
			{
			case 1:
			    m_TRT_nOnTrack_phi_BA[m_trtID->phi_module(rdoId)]++;
			    break;
			case -1:
			    m_TRT_nOnTrack_phi_BC[m_trtID->phi_module(rdoId)]++;
			    break;
			case -2:
			    m_TRT_nOnTrack_phi_ECA[m_trtID->phi_module(rdoId)]++;
			    break;
			case 2:
			    m_TRT_nOnTrack_phi_ECC[m_trtID->phi_module(rdoId)]++;
			    break;
			}
	    
			if(trtcircle->highLevel()) nTRT_HL_trk++;
		    }
		    const InDet::SiClusterOnTrack *clus =
			dynamic_cast<const InDet::SiClusterOnTrack*>(trackState->measurementOnTrack());
		    if (clus) {
			const InDet::SiCluster *RawDataClus =
			    dynamic_cast<const InDet::SiCluster*>(clus->prepRawData());
			if(!RawDataClus){
			    if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) <<
							    "SiCluster WITHOUT " <<
							    "prepRawData!!!!" <<
							    endmsg;
			    continue;
			}
	      
			if (RawDataClus==0) {
			    if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING)
				<< "SiCluster WITHOUT prepRawData!!!!"
				<< endmsg;
			} else {
			    if (RawDataClus->detectorElement()->isSCT()) {
				nSCT_trk += RawDataClus->rdoList().size();
				switch ( m_sctID->barrel_ec(RawDataClus->identify()) )
				{
				case 0:
				    nSCT_trk_B++;
				    break;
				case -2:
				    nSCT_trk_ECA++;
				    break;
				case 2:
				    nSCT_trk_ECC++;
				    break;
				}
		
			    }else{
				nPIX_trk += RawDataClus->rdoList().size();
				switch ( m_pixelID->barrel_ec(RawDataClus->identify()) )
				{
				case 0:
				    nPIX_trk_B++;
				    break;
				case -2:
				    nPIX_trk_ECA++;
				    break;
				case 2:
				    nPIX_trk_ECC++;
				    break;
				}
		
			    }
			}
		    }
		}
	    }
	}
    }

  
	
    //--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*
    // loop over TRT RDOs collections
    //--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*

    int m_TRT_nRDOs_phi_BA[32] = {0};
    int m_TRT_nRDOs_phi_BC[32] = {0};
    int m_TRT_nRDOs_phi_ECA[32] = {0};
    int m_TRT_nRDOs_phi_ECC[32] = {0};

    if(m_driftCircleContainer!=0){
	TRT_DriftCircleContainer::const_iterator colIt =
	    m_driftCircleContainer->begin();
	TRT_DriftCircleContainer::const_iterator colIt_end =
	    m_driftCircleContainer->end();
	for( ; colIt != colIt_end; ++colIt){
	    const TRT_DriftCircleCollection *colNext=(*colIt);
	
	    if(!colNext)
		continue;
	
	    // loop over RDOs
	    DataVector<TRT_DriftCircle>::const_iterator p_rdo =
		colNext->begin();
	    DataVector<TRT_DriftCircle>::const_iterator p_rdo_end =
		colNext->end();
	    for(; p_rdo!=p_rdo_end; ++p_rdo){
		const TRT_DriftCircle* rdo = (*p_rdo);
		if(!rdo)
		    continue;
	  
		Identifier rdoId = rdo->identify();
	
		// Fill the number of RDO per phi sector arrays.
		switch ( m_trtID->barrel_ec(rdoId) )
		{
		case 1:
		    m_TRT_nRDOs_phi_BA[m_trtID->phi_module(rdoId)]++;
		    break;
		case -1:
		    m_TRT_nRDOs_phi_BC[m_trtID->phi_module(rdoId)]++;
		    break;
		case -2:
		    m_TRT_nRDOs_phi_ECA[m_trtID->phi_module(rdoId)]++;
		    break;
		case 2:
		    m_TRT_nRDOs_phi_ECC[m_trtID->phi_module(rdoId)]++;
		    break;
		}
	
		nTRT_RDOs++;
	
		if(rdo->highLevel())
		    nTRT_HL_RDOs++;
	    }
	}
    }
    
    
    //--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*
    // loop over SCT RDOs collections
    //--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*
    
    int nSCT_RDOs=0;
    int nSCT_RDOs_B=0;
    int nSCT_RDOs_ECA=0;
    int nSCT_RDOs_ECC=0;

    if(m_sctRdoContainer != 0){
	SCT_RDO_Container::const_iterator colNextsct =
	    m_sctRdoContainer->begin();
	SCT_RDO_Container::const_iterator colNextsct_end =
	    m_sctRdoContainer->end();
	for( ; colNextsct != colNextsct_end; ++colNextsct){
	    const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection(*colNextsct);
	
	    if(!SCT_Collection)
		continue;
	
	    if(msgLvl(MSG::DEBUG))
		msg(MSG::DEBUG) << "SCT_Collection found with "
				<< SCT_Collection->size() << " RDOs"
				<< endmsg;
	
	    DataVector<SCT_RDORawData>::const_iterator p_rdo = 
		SCT_Collection->begin();
	    DataVector<SCT_RDORawData>::const_iterator p_rdo_end = 
		SCT_Collection->end();
	    for( ; p_rdo!=p_rdo_end; ++p_rdo){
		const SCT_RDORawData* rdo = (*p_rdo);
	  
		if(!rdo)
		    continue;
	  
		const SCT3_RawData* rdo3 =
		    dynamic_cast<const SCT3_RawData*>(rdo);
	  
		// if valid rdo pointer,get time bin,otherwise 
		// assume it's 3
		int tbin = rdo3 ? rdo3->getTimeBin() : 3;
	  
		if(timeBinInPattern(tbin, XIX)){
		    nSCT_RDOs++;
		    switch ( m_sctID->barrel_ec(rdo->identify()) )
		    {
		    case 0:
			nSCT_RDOs_B++;
			break;
		    case -2:
			nSCT_RDOs_ECA++;
			break;
		    case 2:
			nSCT_RDOs_ECC++;
			break;
		    }
		}
	    }
	}
    }
    
    //--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*
    // loop over PIX RDOs collections
    //--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*
    
    int nPIX_RDOs=0;
    int nPIX_RDOs_B=0;
    int nPIX_RDOs_ECA=0;
    int nPIX_RDOs_ECC=0;
    if ( m_pixRdoContainer != 0 ){
	PixelRDO_Container::const_iterator colNextpix =
	    m_pixRdoContainer->begin();
	PixelRDO_Container::const_iterator colNextpix_end =
	    m_pixRdoContainer->end();
	for(; colNextpix != colNextpix_end; ++colNextpix){
	    const InDetRawDataCollection<PixelRDORawData>*
		PIX_Collection(*colNextpix);
	
	    if(!PIX_Collection)
		continue;
	
	    if(msgLvl(MSG::DEBUG))
		msg(MSG::DEBUG) << "PIX_Collection found with " 
				<< PIX_Collection->size() << " RDOs"
				<< endmsg;
	
	    DataVector<PixelRDORawData>::const_iterator p_rdo =
		PIX_Collection->begin();
	    DataVector<PixelRDORawData>::const_iterator p_rdo_end =
		PIX_Collection->end();
	    for(; p_rdo!=p_rdo_end; ++p_rdo){
		const PixelRDORawData* rdo = (*p_rdo);
	  
		if(!rdo)
		    continue;
	  
		nPIX_RDOs++;
		switch ( m_pixelID->barrel_ec(rdo->identify()) )
		{
		case 0:
		    nPIX_RDOs_B++;
		    break;
		case -2:
		    nPIX_RDOs_ECA++;
		    break;
		case 2:
		    nPIX_RDOs_ECC++;
		    break;
		}
	    }
	}
    }
    
    //--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*
    // Filling histograms
    //--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*
    
    // msg << "m_evt" << m_evt << endl;
    m_evt++;
    
    //pre-calculate RDOs-tracks / unit for TRT, SCT and Pixel
    const float dTRTstraw = (nTRT_RDOs-nTRT_trk)/m_nStraws;
    const float dSCTstrip = (nSCT_RDOs-nSCT_trk)/m_nStrips;
    const float dSCTstrip_B = (nSCT_RDOs_B-nSCT_trk_B)/m_nStrips_B;
    const float dSCTstrip_ECA = (nSCT_RDOs_ECA-nSCT_trk_ECA)/m_nStrips_ECA;
    const float dSCTstrip_ECC = (nSCT_RDOs_ECC-nSCT_trk_ECC)/m_nStrips_ECC;
    const float dPIXpixel = (nPIX_RDOs-nPIX_trk)/m_nPixels;
    const float dPIXpixel_B = (nPIX_RDOs_B-nPIX_trk_B)/m_nPixels_B;
    const float dPIXpixel_ECA = (nPIX_RDOs_ECA-nPIX_trk_ECA)/m_nPixels_ECA;
    const float dPIXpixel_ECC = (nPIX_RDOs_ECC-nPIX_trk_ECC)/m_nPixels_ECC;
    const float dTRTHLstraw = (nTRT_HL_RDOs-nTRT_HL_trk)/m_nStraws;
    
    /*
     * fill the vector required for calculating the correlation coeff
     * on the noise occupancies
     * Only fill if all detectors have proper NO values
     * Ad`OBded by m
     */
    
    /*output strem used for debug*/
    //msg(MSG::WARNING) << m_evt << "dTRTstraw " << dTRTstraw << endl;
    //msg(MSG::WARNING) << m_evt << "dSCTstrip " << dSCTstrip << endl;
    //msg(MSG::WARNING) << m_evt << "dPIXpixel " << dPIXpixel << endl;
    //msg(MSG::WARNING) << "nTRT_RDOs " << nTRT_RDOs << " nTRT_trk " << nTRT_trk << endl;
    //msg(MSG::WARNING) << "nSCT_RDOs " << nSCT_RDOs << " nSCT_trk " << nSCT_trk << endl;
    //msg(MSG::WARNING) << "nPIX_RDOs " << nPIX_RDOs << " nPIX_trk " << nPIX_trk << endl;
    //msg(MSG::WARNING) << "m_checkRate " << m_checkRate << endl;
    
    /*
     * Fill combined histograms
     * This will only call if noise in all detectors currently
     * Added by m
     */
    if( nTRT_RDOs > nTRT_trk &&
	nSCT_RDOs > nSCT_trk &&
	nPIX_RDOs > nPIX_trk ){
	/* create a struct of type noise to contain the NOs */
	struct noise noise_occupancy;
      
	/*set the elements of the struct*/
	noise_occupancy.trt_no = dTRTstraw;
	noise_occupancy.sct_no = dSCTstrip;
	noise_occupancy.pix_no = dPIXpixel;
      
	/*
	 * add the current struct to end of the vector
	 * NO - declared in header
	 */
	NO.push_back(noise_occupancy);
      
	/* strem for debug
	 * msg << "added strct to vector" << endl;
	 * msg << m_evt << " noise_occupancy.trt_no "
	 *	<< noise_occupancy.trt_no;
	 */
    }
    
    
    if(nTRT_RDOs > nTRT_trk  && nSCT_RDOs > nSCT_trk)
	m_SCT_TRT_NO->Fill(dSCTstrip,dTRTstraw);
    else
	m_SCT_TRT_NO->Fill(0.,0.);
    
    if(nPIX_RDOs > nPIX_trk  && nSCT_RDOs > nSCT_trk)
	m_SCT_PIX_NO->Fill(dSCTstrip,dPIXpixel); 
    else 
	m_SCT_PIX_NO->Fill(0.,0.);
    
    if (nPIX_RDOs > nPIX_trk  && nTRT_RDOs > nTRT_trk) 
	m_PIX_TRT_NO->Fill(dPIXpixel,dTRTstraw);
    else
	m_PIX_TRT_NO->Fill(0.,0.); 
    
    //if (nPIX_RDOs > nPIX_trk  && nSCT_RDOs > nSCT_trk && 
    // nTRT_RDOs>nTRT_trk )
    //    m_PIX_SCT_TRT_NO->Fill(dSCTstrip-dPIXpixel, dSCTstrip-dTRTstraw); 
    //else
    //    m_PIX_SCT_TRT_NO->Fill(0.,0.); 
    
    
    if(nTRT_RDOs > nTRT_trk){ 
	m_TRT_NO_evt->Fill(m_evt,dTRTstraw,1);
	m_TRT_NO->Fill(dTRTstraw);

	for (int i = 0;i < 32;i++){
	    m_TRT_NO_BA_phi->Fill(i,((float)m_TRT_nRDOs_phi_BA[i]-(float)m_TRT_nOnTrack_phi_BA[i])/(float)m_nStraws_B*32.0*2);
	    m_TRT_NO_BC_phi->Fill(i,((float)m_TRT_nRDOs_phi_BC[i]-(float)m_TRT_nOnTrack_phi_BC[i])/(float)m_nStraws_B*32.0*2);
	    m_TRT_NO_ECA_phi->Fill(i,((float)m_TRT_nRDOs_phi_ECA[i]-(float)m_TRT_nOnTrack_phi_ECA[i])/(float)m_nStraws_ECA*32.0 );
	    m_TRT_NO_ECC_phi->Fill(i,((float)m_TRT_nRDOs_phi_ECC[i]-(float)m_TRT_nOnTrack_phi_ECC[i])/(float)m_nStraws_ECC*32.0 );
	}

	m_TRT_NO_nseg->Fill(nTRT_seg,dTRTstraw);
	m_TRT_NO_ntrk->Fill(nComb_trk,dTRTstraw);
    }
    else{
	m_TRT_NO_evt->Fill(m_evt,0.,1);
	m_TRT_NO->Fill(0.);
	m_TRT_NO_nseg->Fill(nTRT_seg,0.);
	m_TRT_NO_ntrk->Fill(nComb_trk,0.);
    }
    
    m_TRT_HL_NO_evt->Fill(m_evt,dTRTHLstraw,1);
    m_TRT_HL_NO->Fill(dTRTHLstraw);
    
    if(nSCT_RDOs > nSCT_trk){
	m_SCT_NO_evt->Fill(m_evt,dSCTstrip,1);
	m_SCT_NO->Fill(dSCTstrip);
	m_SCT_NO_B->Fill(dSCTstrip_B);
	m_SCT_NO_ECA->Fill(dSCTstrip_ECA);
	m_SCT_NO_ECC->Fill(dSCTstrip_ECC);
	m_SCT_NO_nseg->Fill(nSCT_seg,dSCTstrip);
	m_SCT_NO_ntrk->Fill(nComb_trk,dSCTstrip);
    }
    else{
	m_SCT_NO_evt->Fill(m_evt,0.,1);
	m_SCT_NO->Fill(0.);
	m_SCT_NO_nseg->Fill(nSCT_seg,0.);
	m_SCT_NO_ntrk->Fill(nComb_trk,0.);
    }
    if(nPIX_RDOs > nPIX_trk){
	m_PIX_NO_evt->Fill(m_evt,dPIXpixel,1);
	m_PIX_NO->Fill(dPIXpixel);
	m_PIX_NO_B->Fill(dPIXpixel_B);
	m_PIX_NO_ECA->Fill(dPIXpixel_ECA);
	m_PIX_NO_ECC->Fill(dPIXpixel_ECC);
	m_PIX_NO_nseg->Fill(nPIX_seg,dPIXpixel);
	m_PIX_NO_ntrk->Fill(nComb_trk,dPIXpixel);
    }
    else{
	m_PIX_NO_evt->Fill(m_evt,0.,1);
	m_PIX_NO->Fill(0.);
	m_PIX_NO_nseg->Fill(nPIX_seg,0.);
	m_PIX_NO_ntrk->Fill(nComb_trk,0.);
    }
    
    LWHist::LWHistAxis  *xaxis_ST = m_SCT_TRT_NO->GetXaxis();
    LWHist::LWHistAxis  *yaxis_ST = m_SCT_TRT_NO->GetYaxis();
    xaxis_ST->SetTitle("SCT");
    yaxis_ST->SetTitle("TRT");
    
    LWHist::LWHistAxis  *xaxis_SP = m_SCT_PIX_NO->GetXaxis();
    LWHist::LWHistAxis  *yaxis_SP = m_SCT_PIX_NO->GetYaxis();
    xaxis_SP->SetTitle("SCT");
    yaxis_SP->SetTitle("PIX");
    
    LWHist::LWHistAxis  *xaxis_PT = m_PIX_TRT_NO->GetXaxis();
    LWHist::LWHistAxis  *yaxis_PT = m_PIX_TRT_NO->GetYaxis();
    xaxis_PT->SetTitle("PIX");
    yaxis_PT->SetTitle("TRT");
    
    LWHist::LWHistAxis  *xaxis_PIX_NO_ntrk = m_PIX_NO_ntrk->GetXaxis();
    xaxis_PIX_NO_ntrk->SetTitle("Number of tracks");
    
    LWHist::LWHistAxis  *xaxis_PIX_NO_nseg = m_PIX_NO_nseg->GetXaxis();
    xaxis_PIX_NO_nseg->SetTitle("Number of PIX segments");
    
    LWHist::LWHistAxis  *xaxis_SCT_NO_ntrk = m_SCT_NO_ntrk->GetXaxis();
    xaxis_SCT_NO_ntrk->SetTitle("Number of tracks");
    
    LWHist::LWHistAxis  *xaxis_SCT_NO_nseg = m_SCT_NO_nseg->GetXaxis();
    xaxis_SCT_NO_nseg->SetTitle("Number of SCT segments");
    
    LWHist::LWHistAxis  *xaxis_TRT_NO_ntrk = m_TRT_NO_ntrk->GetXaxis();
    xaxis_TRT_NO_ntrk->SetTitle("Number of tracks");
    
    LWHist::LWHistAxis  *xaxis_TRT_NO_nseg = m_TRT_NO_nseg->GetXaxis();
    xaxis_TRT_NO_nseg->SetTitle("Number of TRT segments");	

    /*
     * Calculate correlation coefficients and fill histos on
     * every 10th pass  - the vector should be filled by then
     * Added by m
     */
    if(m_evt%10==0){
	correlation_coeffs(NO);

	/* 
	 * clear the NO occupancy vector as .push_back()
	 * will continue to add onto the end even although
	 * we're starting a new calcualtion
	 */
	NO.clear();
    }
    
    //--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*
    // Reseting histograms
    //--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*
    
    /* 
     * Resetting the histograms can lead to blank
     * histograms if m_evt%m_checkRate = int
     * Might have to think of some other way of doing this
     */
    
    if(m_evt%m_checkRate==0){
      
	//Reset pixel histograms
	m_PIX_NO_evt->Reset();
	m_PIX_NO->Reset();
	m_PIX_NO_nseg->Reset();
	m_PIX_NO_ntrk->Reset();
      
	//Reset sct histograms
	m_SCT_NO_evt->Reset();
	m_SCT_NO->Reset();
	m_SCT_NO_nseg->Reset();
	m_SCT_NO_ntrk->Reset();
      
	//Reset trt histograms 
	m_TRT_NO_evt->Reset(); 
	m_TRT_HL_NO_evt->Reset();
	m_TRT_NO->Reset();
	m_TRT_NO_nseg->Reset();
	m_TRT_NO_ntrk->Reset();
      
	//Reset id combined histograms
	m_SCT_TRT_NO->Reset();
	m_SCT_PIX_NO->Reset();
	m_PIX_TRT_NO->Reset();
      
	/* 
	 * Reset combined histos 
	 * Added by m
	 */
	m_TRT_SCTvTRT_PIX_10evt->Reset();
	m_TRT_SCTvSCT_PIX_10evt->Reset();
	m_TRT_PIXvSCT_PIX_10evt->Reset();
      
	/* Set m_evt counter to 0 again */
	m_evt = 0;
    
    }
    return StatusCode::SUCCESS;
}
//-------------------------------------------------------------------------
bool InDetGlobalNoiseOccupancyMonTool::isOnTrack(const Identifier & DCId) const{
return(find(m_dcidOnTrack.begin(), m_dcidOnTrack.end(), DCId ) 
       != m_dcidOnTrack.end());  
    }


//------------------------------------------------------------------------
int InDetGlobalNoiseOccupancyMonTool::histIdx(const vector<int> &aList, 
					      const Identifier &rdoId) const{
    //get triplet identifier
    int trplID = 20*vecContains(aList,m_trtID->phi_module(rdoId));
  
    //get layer or wheel id
    const int layer_or_wheel=m_trtID->layer_or_wheel(rdoId);

    //add layer or wheel
    if(layer_or_wheel<6){
	trplID += layer_or_wheel*2;
    } 
    else 
	if(layer_or_wheel>5){
	    trplID += 6 + layer_or_wheel;
	}
    //add straw layer
    if(m_trtID->straw_layer(rdoId) > 7) trplID +=1;
	
    //return triplet id
    return trplID;
}

/* 
 * method to calculate the correlation coefficient for a group of points 
 * this method takes in the already summed points and calculates the 
 * Pearson Moment correlation coefficient
 * Added by m
 */
float InDetGlobalNoiseOccupancyMonTool::calc_correlation(float *sum_x,
							 float *sum_y, float *sum_xy, float *sum_x_sq, float *sum_y_sq,
							 int *points)
{
    /* define tmp floats */
    float r;
    float denom_1;
    float denom_2;
    float num;
	
    /* calculate the sections of the sum 
     */
    num = (float)*points * *sum_xy - *sum_x * *sum_y;
    denom_1 = (float)*points * *sum_x_sq - *sum_x * *sum_x;
    denom_2 = (float)*points * *sum_y_sq - *sum_y * *sum_y;
	
    r = num / sqrt((denom_1 * denom_2));

    return r;
}


/*
 * Method to calculate the correlation coefficient for every n events
 * Added my m
 */
void InDetGlobalNoiseOccupancyMonTool::correlation_coeffs(vector<struct noise>
							  &no){

    /* get the number of events saved in the vector 
     * this should be 10 currently
     */
    int size = no.size();

    if(size!=10){

	/*if(msgLvl(MSG::WARNING)){
	  msg(MSG::WARNING) << "less than " << size << 
	  " events saved for correlation calculation "
	  "- manually setting numbers to -2"
	  << endmsg;
	  }*/
		
	m_TRT_SCTvTRT_PIX_10evt->Fill(-2,-2);
	m_TRT_SCTvSCT_PIX_10evt->Fill(-2,-2);
	m_TRT_PIXvSCT_PIX_10evt->Fill(-2,-2);
    }
    else{
	/*
	 * n is the 'bin size' we will use for calculating
	 * correlation coefficient = number of points used
	 */
	int n = 10;
		
	/* calculate x = the number of correlation coefficients to be 
	 * calculated 
	 * x should be 1 currently
	 * this extra stage has been left from a previous version as 
	 * a check.
	 */
	int x = (int)(size / n);
	
	/* floats for final calculation */
	float corr_trt_sct;
	float corr_trt_pix;
	float corr_sct_pix;
	
	
	/* loop over the number of correlation coefficients to be 
	 * calculated 
	 */
	for(int i=0; i < x; i++){

	    /*
	     * temp floats to be used for summing the values required
	     * for the calc_correlation method - reset to 0 each time
	     */
	    float sum_trt_sct = 0;
	    float sum_trt_pix = 0;
	    float sum_sct_pix = 0;
		
	    float sum_trt = 0;
	    float sum_sct = 0;
	    float sum_pix = 0;
		
	    float sum_trt_sq = 0;
	    float sum_sct_sq = 0;
	    float sum_pix_sq = 0;
		
	    /* debug...
	     * ofstream msg1("corr_coeff.out");
	     */
		
	    /* loop over the individual events in this 'block'*/
	    for(int j = n*i; j < n*i+n; j++){
			
		/* get the values stored in the struct 
		 * in the vector */
		float trt = no.at(j).trt_no;
		//msg1 << i << " trt_no from storage = 
		//" << trt << endl;
		float sct = no.at(j).sct_no;
		//msg1 << i << " sct_no from storage = 
		//" << sct << endl;
		float pix = no.at(j).pix_no;
		//msg1 << i << " pix_no from storage = " 
		//<< pix << endl;
			
		/* add to the counters*/
		sum_trt += trt;
		sum_sct += sct;
		sum_pix += pix;
			
		sum_trt_sct += trt*sct;
		sum_trt_pix += trt*pix;
		sum_sct_pix += sct*pix;

		sum_trt_sq += trt*trt;
		sum_sct_sq += sct*sct;
		sum_pix_sq += pix*pix;
	    }

	    /* call calc_correlation with the current numbers */
	    corr_trt_sct = calc_correlation(&sum_trt,&sum_sct,
					    &sum_trt_sct, &sum_trt_sq, &sum_sct_sq,
					    &n);
	    corr_trt_pix = calc_correlation(&sum_trt,&sum_pix,
					    &sum_trt_pix, &sum_trt_sq, &sum_pix_sq,
					    &n);
	    corr_sct_pix = calc_correlation(&sum_pix,&sum_sct,	
					    &sum_sct_pix, &sum_pix_sq, &sum_sct_sq,
					    &n);
		
	    /* debug...
	     * msg1 << "corr_trt_sct " << corr_trt_sct << endl;
	     * msg1 << "corr_trt_pix " << corr_trt_pix << endl;
	     * msg1 << "corr_sct_pix " << corr_sct_pix << endl;
	     */
		
	    /* 
	     * fill the histograms 
	     */
	    m_TRT_SCTvTRT_PIX_10evt->Fill(corr_trt_sct,
					  corr_trt_pix);
	    m_TRT_SCTvSCT_PIX_10evt->Fill(corr_trt_sct,
					  corr_sct_pix);
	    m_TRT_PIXvSCT_PIX_10evt->Fill(corr_trt_pix,
					  corr_sct_pix);
	}
    }
}
										

// Preliminar code to count the number of strips using the SCT_ID information
//  John Idarraga <idarraga@cern.ch>, 2008.

int InDetGlobalNoiseOccupancyMonTool::GetSCTCounts(int barrel_ec_sel, bool do_checks_flg){
  
    //////////////////////////////////////////
    // SCT part
    // Barrel part
    //     /// For an individual strip                Barrel             |  NegEc, PosEc
    //     Identifier  strip_id ( int barrel_ec,      0                        -2      2
    //                            int layer_disk,     0 ->    3                0 --> 8
    //                            int phi_module,     0 --> <56                0 --> <52
    //                            int eta_module,     -6 --> -1, 1 --> 6       0 --> 2
    //                            int side,           0,1
    //                            int strip) const;   0 --> 767

    if(barrel_ec_sel != 0 && barrel_ec_sel != -2 && barrel_ec_sel != 2)
    {
	if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "Wrong selection on SCTCounts !" << endmsg;
	return -1;
    }

    int barrel_ec = barrel_ec_sel;
    int nStripsCntr = 0;

    // IDhelper do checks on components.  Like strips.
    m_sctID->set_do_checks(do_checks_flg);

    Identifier rdoId;
    int layer_disk_max = 0;
    if(barrel_ec_sel == 0)
    {
	layer_disk_max = 3;
	rdoId= m_sctID->strip_id(barrel_ec, 0, 0, -6, 0, 0); // start point for barrel
    }
    else if(barrel_ec_sel == -2 || barrel_ec_sel == 2)
    {
	layer_disk_max = 8;
	rdoId = m_sctID->strip_id(barrel_ec, 0, 0, 0, 0, 0); // start point for ec
    }

    for(int layerItr = 0 ; layerItr <= layer_disk_max ; layerItr++) // 3 layer_disk for Barrel
    {
	for(int phiItr = 0 ; phiItr <= m_sctID->phi_module_max(rdoId) ; phiItr++) // phi modules
	{
	    for(int etaItr = m_sctID->eta_module_min(rdoId) ;
		etaItr <= m_sctID->eta_module_max(rdoId) ; etaItr++) // eta modules
	    {
		for(int sideItr = 0 ; sideItr <= 1 ; sideItr++) // always two sides 0,1
		{
		    int strip_max_m = m_sctID->strip_max(rdoId);
		    for(int stripItr = 0 ; stripItr <= strip_max_m ; stripItr++) // strips
		    {
			rdoId = m_sctID->strip_id(barrel_ec, layerItr, phiItr, etaItr, sideItr, stripItr);
			nStripsCntr++;
		    }
		}
	    }
	}
    }
  
    // Start point for the positive part.  Good for barrel and ec.
    rdoId = m_sctID->strip_id(barrel_ec, 0, 0, 1, 0, 0);
    for(int layerItr = 0 ; layerItr <= layer_disk_max ; layerItr++) // 3 layer_disk for Barrel
    {
	for(int phiItr = 0 ; phiItr <= m_sctID->phi_module_max(rdoId) ; phiItr++) // phi modules
	{
	    for(int etaItr = m_sctID->eta_module_min(rdoId) ;
		etaItr <= m_sctID->eta_module_max(rdoId) ; etaItr++)  // From 1 to 6 for barrel and 0 to 2 for ec.
	    {
		for(int sideItr = 0 ; sideItr <= 1 ; sideItr++) // always two sides 0,1
		{
		    int strip_max_m = m_sctID->strip_max(rdoId);
		    for(int stripItr = 0 ; stripItr <= strip_max_m ; stripItr++) // strips
		    {
			rdoId = m_sctID->strip_id(barrel_ec, layerItr, phiItr, etaItr, sideItr, stripItr);
			//m_sctID->print(rdoId);
			nStripsCntr++;
		    }
		}
	    }
	}
    }
  
    if ( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << "        SCT nStripsCntr: " << nStripsCntr << endmsg;
    return nStripsCntr;
}

int InDetGlobalNoiseOccupancyMonTool::GetPixelCounts(int barrel_ec_sel, bool do_checks_flg){
  
    //////////////////////////////////////////
    // SCT part
    // Barrel part
    //     /// For an individual strip                Barrel             |  NegEc, PosEc
    //     Identifier  strip_id ( int barrel_ec,      0                        -2      2
    //                            int layer_disk,     0 ->    3                0 --> 8
    //                            int phi_module,     0 --> <56                0 --> <52
    //                            int eta_module,     -6 --> -1, 1 --> 6       0 --> 2
    //                            int side,           0,1
    //                            int strip) const;   0 --> 767

    if(barrel_ec_sel != 0 && barrel_ec_sel != -2 && barrel_ec_sel != 2)
    {
	if ( msgLvl(MSG::WARNING) ) msg(MSG::WARNING) << "Wrong selection on GetPixelCounts !" << endmsg;
	return -1;
    }

    int barrel_ec = barrel_ec_sel;
    int nPixelCntr = 0;

    // IDhelper do checks on components.  Like strips.
    m_pixelID->set_do_checks(do_checks_flg);

    Identifier rdoId;
    int layer_disk_max = 0;
    if(barrel_ec_sel == 0)
    {
	layer_disk_max = 2;
	rdoId= m_pixelID->pixel_id(barrel_ec, 0, 0, 0, 0, 0); // start point for barrel
    }
    else if(barrel_ec_sel == -2 || barrel_ec_sel == 2)
    {
	layer_disk_max = 2;
	rdoId = m_pixelID->pixel_id(barrel_ec, 0, 0, 0, 0, 0); // start point for ec
    }

    for(int layerItr = 0 ; layerItr <= layer_disk_max ; layerItr++) // 3 layer_disk for Barrel
    {
	int eta_module_min = m_pixelID->eta_module_min( m_pixelID->pixel_id(barrel_ec, layerItr, 0, 0, 0, 0) );
	int phi_module_max = m_pixelID->phi_module_max( m_pixelID->pixel_id(barrel_ec, layerItr, 0, 0, 0, 0) );
	int eta_module_max = m_pixelID->eta_module_max( m_pixelID->pixel_id(barrel_ec, layerItr, 0, 0, 0, 0) );
	for(int phiItr = 0 ; phiItr <= phi_module_max ; phiItr++) // phi modules
	{
	    for(int etaItr = eta_module_min; etaItr <= eta_module_max; etaItr++) // eta modules
	    {
		int pixel_phi_max = m_pixelID->phi_index_max( m_pixelID->wafer_id(barrel_ec, layerItr, phiItr, etaItr) );
		int pixel_eta_max = m_pixelID->eta_index_max( m_pixelID->wafer_id(barrel_ec, layerItr, phiItr, etaItr) );
		for(int pixel_phi = 0 ; pixel_phi <= pixel_phi_max ; pixel_phi++){
		    for(int pixel_eta = 0 ; pixel_eta <= pixel_eta_max ; pixel_eta++){
			nPixelCntr++;
		    }
		}
	    }
	}
    }
  
    if ( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << "        Pixel Cntr: " << nPixelCntr << endmsg;

    return nPixelCntr;
}

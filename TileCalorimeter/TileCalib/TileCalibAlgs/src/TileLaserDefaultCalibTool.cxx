/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalibAlgs/TileLaserDefaultCalibTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"


#include "Identifier/HWIdentifier.h"

#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileLaserObject.h"
#include "TileConditions/TileCablingService.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileConditions/TileDCSSvc.h"

#include "TFile.h"
#include "TTree.h"
#include <cmath>
#include <iostream>

/****************************************************/
/* TileLaserDefaultCalibTool.cxx     June 25th 2008 */
/*                                                  */
/* S.Viret       viret@in2p3.fr                     */
/*                                                  */
/* This code is inspired from the CIS tool          */
/****************************************************/

TileLaserDefaultCalibTool::TileLaserDefaultCalibTool(const std::string& type, const std::string& name,const IInterface* pParent):
  AlgTool(type, name, pParent),
  m_tileBadChanTool("TileBadChanTool"), 
  m_tileDCSSvc("TileDCSSvc",name)

{
  declareInterface<ITileCalibTool>( this );
  declareProperty("toolNtuple", m_toolNtuple="h3000");
  declareProperty("rawChannelContainer", m_rawChannelContainerName="");
  declareProperty("laserObjContainer", m_laserContainerName="");
  declareProperty("pisaMethod2", m_pisaMethod2=true);
  declareProperty("TileDCSSvc",m_tileDCSSvc);
}

TileLaserDefaultCalibTool::~TileLaserDefaultCalibTool()
{}


//
// Tool instantiation :
//
// Used to reset all the local variables, 
// and to load the necessary services
//

 
StatusCode TileLaserDefaultCalibTool::initialize()
{
  
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "initialize()" << endreq;


  // Reset local parameters

  m_toolRunNo    = 0;
  m_ADC_problem  = 0;
  m_las_filter   = 0;
  m_las_requ_amp = 0;
  m_hrate        = 0; 
  m_flow         = 0; 
  m_head_temp    = 0;
  m_las_time     = 0;
  m_PMT1_ADC_prev= 0;
  m_PMT2_ADC_prev= 0;
  
  for(int d=0; d<2; ++d) 
  {
    m_PMT[d]      = 0;
    m_PMT_S[d]    = 0;
    PMT_signal[d] = new RunningStat();
  }

  for(int d=0; d<4; ++d) 
  {
    m_diode[d]       = 0;    
    m_diode_S[d]     = 0;  
    m_diode_Ped[d]   = 0;
    m_diode_Alpha[d] = 0;         
    m_diode_SPed[d]  = 0;          
    m_diode_SAlpha[d]= 0;
    diode_signal[d]  = new RunningStat();
  }

  for(int part=0; part<4; ++part) {
    meantime[part] = new RunningStat();
    m_meantime[part] =0.;
    for(int drawer=0; drawer<64; ++drawer) {
	for(int gain=0; gain<2; ++gain) {
	  
	  //--- V.Giangiobbe : k factor for each couple of odd and even PMTs
	  for (int couple=0; couple<22; ++couple){
	    m_reducedKappa[part][drawer][couple][gain] = new RunningStat();
	  }
	
	for(int channel=0; channel<48; ++channel) 
	  { 
          time[part][drawer][channel][gain]      = new RunningStat();
          signal[part][drawer][channel][gain]    = new RunningStat();
          m_time[part][drawer][channel][gain]    = 0;
          m_time_S[part][drawer][channel][gain]  = 0;
          m_mean[part][drawer][channel][gain]    = 0;
          m_mean_S[part][drawer][channel][gain]  = 0;
          m_kappa[part][drawer][channel][gain]   = 0;
          m_status[part][drawer][channel][gain]  = 0;
					
	  for (int iSlice=0; iSlice<100; ++iSlice){
	    m_mean_slice[part][drawer][channel][iSlice][gain] = 0;
	    m_variance_slice[part][drawer][channel][iSlice][gain] = 0;	  				
	  }
	  
          for(int d=0; d<4; ++d) 
          {
            ratio[d][part][drawer][channel][gain]     = new RunningStat();
            m_ratio[d][part][drawer][channel][gain]   = 0;
            m_ratio_S[d][part][drawer][channel][gain] = 0;
          }	
	  m_entries[part][drawer][channel][gain] = 0;
	  
	  //	  int pmt = abs(m_cabling->channel2hole(part,channel));
	  m_HV[part][drawer][channel] = 0.; //m_tileDCSSvc->getDCSHV(part, drawer, pmt);
	  m_HVSet[part][drawer][channel] = 0.; //m_tileDCSSvc->getDCSHVSET(part, drawer, pmt);
	  //	  std::cout << "Henric " <<  m_HV[part][drawer][channel] << " " << m_HVSet[part][drawer][channel] << std::endl; 
	  } // channel loop 
	} // gain loop 
    } // drawer loop 
  } // partition loop 

	

  // Find necessary services
  if(service("StoreGateSvc", m_evtStore).isFailure()){
    log<<MSG::ERROR<<"Unable to get pointer to eventStore Service"<<endreq;
    return StatusCode::FAILURE;
  }

  if(service("DetectorStore", m_detStore).isFailure()){
    log << MSG::ERROR<<"Unable to get pointer to DetectorStore Service" << endreq;
    return StatusCode::FAILURE;
  }


  IToolSvc* toolSvc;
  if (service("ToolSvc",toolSvc).isFailure()) {
    log << MSG::ERROR <<" Can't get ToolSvc " << endreq;
    return StatusCode::FAILURE;
  }

  // get TileHWID helper
  if(m_detStore->retrieve(m_tileHWID).isFailure()){
    log << MSG::ERROR << "Unable to retrieve TileHWID helper from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }

 
  if(m_tileToolEmscale.retrieve().isFailure()){
    log << MSG::ERROR << "Unable to retrieve " << m_tileToolEmscale << endreq;
    return StatusCode::FAILURE;
  }

  // get TileCabling Service
  m_cabling = TileCablingService::getInstance();

  // get beam info tool
  if(toolSvc->retrieveTool("TileBeamInfoProvider",m_beamInfo).isFailure()) {
    log << MSG::ERROR << "Unable to get tool 'TileBeamInfoProvider'" << endreq;
    return StatusCode::FAILURE;
  }

	// get TileBadChanTool
	//StatusCode sc = m_tileBadChanTool.retrieve();
	//if(sc.isFailure()){
	//	log << MSG::ERROR  << "Unable to retrieve " << m_tileBadChanTool << endreq;
	//	return sc;
 // }

  if(m_tileBadChanTool.retrieve().isFailure()){
    log << MSG::ERROR << "Unable to retrieve " << m_tileBadChanTool << endreq;
    return StatusCode::FAILURE;
  }

  //=== get TileDCSSvc
  if(m_tileDCSSvc.retrieve().isFailure()){
    log << MSG::ERROR << "Unable to retrieve " << m_tileDCSSvc << endreq;
    return StatusCode::FAILURE;
  }



  return StatusCode::SUCCESS;  
}       

StatusCode TileLaserDefaultCalibTool::initNtuple(int runNumber, int runType, TFile * rootFile)
{
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "initialize(" << runNumber << "," << runType << "," << rootFile << ")" << endreq;

  return StatusCode::SUCCESS;  
}


//
// Event by event operations :
//
// Here we get the LASER object and the RawChannel information 
// (we get channel amplitude by Eopt method) and we store it into 
// maps
//
// SV 25/06/08 : STILL TO DO : event selection should be made here
//                             NOT DONE for the moment 


StatusCode TileLaserDefaultCalibTool::execute()
{
  
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "execute()" << endreq;

  log << MSG::DEBUG << "Retrieving the LASER object and RawChannel" << endreq;

  const TileRawChannelContainer * rawCnt = 0;
  const TileLaserObject* laserObj;

  if ((m_evtStore->retrieve(rawCnt, m_rawChannelContainerName)).isFailure() ||
      (m_evtStore->retrieve(laserObj, m_laserContainerName)).isFailure())
  {
    log << MSG::ERROR << "There is a problem opening TileRawChannelContainer or the LASER object" << endreq;
    return StatusCode::FAILURE;
  } 

  // First we got event time (From 1/1/70)

  const uint32_t *cispar = m_beamInfo->cispar();
  
  m_las_time = static_cast<double>(cispar[10])+static_cast<double>(cispar[11])/1000000;

  //
  // Then we could start getting the LASER informations 
  // 

  if (laserObj->getDiodeCurrOrd() == 0 || laserObj->getFiltNumber() == 0)   // Sanity check
  {
    log << MSG::WARNING << "Filter wheel moving: skip event !!" << endreq;
    return StatusCode::SUCCESS; // This is expected for some events
  }

  for(int i=0; i<4; ++i) 
    diode_signal[i]->Push(laserObj->getDiodeADC(i)-laserObj->getDiodePedestal(i));

  for(int i=0; i<2; ++i) 
    PMT_signal[i]->Push(laserObj->getPMADC(i)-laserObj->getPMPedestal(i));

  // Check that the ADC has really sent a new information 

  if (laserObj->getPMADC(0) == m_PMT1_ADC_prev && 
      laserObj->getPMADC(1) == m_PMT2_ADC_prev)
  {
    m_ADC_problem = 1;
    log << MSG::WARNING << "There is perhaps an ADC problem with this event" << endreq;
  }

  m_PMT1_ADC_prev = laserObj->getPMADC(0);
  m_PMT2_ADC_prev = laserObj->getPMADC(1);

  // Next parameters are constants, don't need to update them more than once

  if (m_las_filter == 0) 
  {
    m_las_filter   = laserObj->getFiltNumber();
    m_hrate        = laserObj->getHumidity(); 
    m_flow         = laserObj->getGasFlux(); 
    m_head_temp    = laserObj->getPumpDiodeTemp();
    m_las_requ_amp = laserObj->getDiodeCurrOrd();

    for(int i=0; i<4; ++i) 
    {
      m_diode_Ped[i]     = laserObj->getDiodePedestal(i);
      m_diode_SPed[i]    = laserObj->getDiodeSigmaPedestal(i);
      m_diode_Alpha[i]   = laserObj->getAlpha(i);    
      m_diode_SAlpha[i]  = laserObj->getSigmaAlpha(i);
    }    
  }

  double diode[4];
    
  for(int i=0; i<4; ++i) 
  {
    diode[i] = 0; 
    if (laserObj->getDiodeADC(i) <= 2044) // Diode signal OK
      diode[i]   = static_cast<double>(laserObj->getDiodeADC(i)-m_diode_Ped[i]);
  }

  //
  // Then we could loop over all the channels, and get the correction factor
  //


  // Create iterator over RawChannelContainer 
  TileRawChannelUnit::UNIT RChUnit = rawCnt->get_unit();
  TileRawChannelContainer::const_iterator itColl;
  TileRawChannelContainer::const_iterator itCollEnd = rawCnt->end();
  
  double Q1Q2[22];
  for (int couple=0; couple<22; ++couple) Q1Q2[couple]=1;
  int currentDrawer=0;
  

  RunningStat* avg_time[4];
  for (int ros=0; ros<4; ros++) avg_time[ros] = new RunningStat();

  for(itColl=rawCnt->begin(); itColl != itCollEnd; ++itColl) {
    HWIdentifier drawer_id = m_tileHWID->drawer_id((*itColl)->identify());
    int ros = m_tileHWID->ros(drawer_id)-1;                                // LBA=0 LBC=1 EBA=2 EBC=3
    // go through all TileRawChannels in collection
    for(TileRawChannelCollection::const_iterator it = (*itColl)->begin(); it != (*itColl)->end(); ++it) {
      float ofctime = (*it)->time();
      // if (it == (*itColl)->begin()) std::cout << ofctime << std::endl;
      if (ofctime!=0.0 and abs(ofctime-15.0)<30.)
	avg_time[ros]->Push(ofctime);
      /*else 
      	if (ofctime!=0.0)
	std::cout << ofctime << std::endl;*/
    }
  }
  
  /* for (int ros=0; ros<4; ros++)
    std::cout << avg_time[ros]->Mean() << " " ; 
  std::cout << std::endl;
  */
  // Go through all TileRawChannelCollections
  for(itColl=rawCnt->begin(); itColl != itCollEnd; ++itColl) {
    HWIdentifier drawer_id = m_tileHWID->drawer_id((*itColl)->identify());
    int ros = m_tileHWID->ros(drawer_id)-1;                                // LBA=0 LBC=1 EBA=2 EBC=3
    int drawer = m_tileHWID->drawer(drawer_id);                            // 0 to 63
    unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros+1,drawer);   

      

    // go through all TileRawChannels in collection
    TileRawChannelCollection::const_iterator it = (*itColl)->begin();
    for(; it != (*itColl)->end(); ++it) 
    {
 
      // get adcHash
      HWIdentifier hwid=(*it)->adc_HWID();
      int chan   = m_tileHWID->channel(hwid);  // 0 to 47 channel not PMT
      int gain   = m_tileHWID->adc(hwid);      // low=0 high=1
      float amp = (*it)->amplitude();
      float ofctime = (*it)->time();
      if (ofctime!=0.0) ofctime-=avg_time[ros]->Mean();
      //      if (it == (*itColl)->begin()) std::cout << ofctime << " " << avg_time[ros]->Mean() << std::endl;
      const TileDQstatus *theDQstatus = m_beamInfo->getDQstatus();
      
      //bool isEmpty = theDQstatus->isChEmpty(ros+1,drawer,chan);
      //std::cout<<ros<<"\t"<<drawer<<"\t"<<chan<<"\t"<<isEmpty<<"\t"
      //         <<theDQstatus->isAdcDQgood(ros+1,drawer,chan,gain)<<"\t"
      //         <<m_tileBadChanTool->encodeStatus(  m_tileBadChanTool->getAdcStatus(drawerIdx, chan, gain) )
      //         <<std::endl;
      
      // check if channel is connected
      if( theDQstatus->isChEmpty(ros+1,drawer,chan) ) {
         m_status[ros][drawer][chan][0] = -1; 
         m_status[ros][drawer][chan][1] = -1; 
         continue;
      }
      
      //--- Status of the channel in DB
      //TileBchStatus status = m_tileBadChanTool->getAdcStatus(drawerIdx, chan, gain);
      //m_status[ros][drawer][chan][gain] = status.isBad();
      //std::cout<<"Status = "<<status.isBad()<<std::endl; 

      // 0 = isGood()
      // 1 = isNoisy() : Large HF noise; Correlated noise; Large LF noise;
      // 2 = isAffected() : ?
      // 3 = isBad()   : ADC masked (unspecified); ADC dead; Very large HF noise; No data; Wrong DSP configuration; Severe stuck bit; Severe data corruption; Channel masked (unspecified); No PMT connected; No HV; Wrong HV;
      // 4 = other
      // 5 = bad ADC
      
      if( theDQstatus->isAdcDQgood(ros+1,drawer,chan,gain) ) {
         m_status[ros][drawer][chan][gain] = m_tileBadChanTool->encodeStatus(  m_tileBadChanTool->getAdcStatus(drawerIdx, chan, gain) );
      } else {
         m_status[ros][drawer][chan][gain] = 5;
      } 
      
      /*
      // Don't enable negative or null energies
      if ( amp <= 0.) continue;
      In fact we don't like this line since it biases the averages to follow
      */

      float ampInPicoCoulombs = m_tileToolEmscale->channelCalib(drawerIdx, chan, gain, amp, RChUnit, TileRawChannelUnit::PicoCoulombs);

      time[ros][drawer][chan][gain]->Push(ofctime);
      signal[ros][drawer][chan][gain]->Push(ampInPicoCoulombs);

      for(int i=0; i<4; ++i) 
      { 
      if (diode[i] != 0)
      	ratio[i][ros][drawer][chan][gain]->Push(ampInPicoCoulombs/diode[i]);
      }
		
			//---- V.Giangiobbe. Compute the average <q1.q2> for each couple of even and odd PMTs.

			//-- RAZ
			if (currentDrawer != drawer) {
				for (int couple=0; couple<22; ++couple) Q1Q2[couple]=1;
			}
			
			//-- Store the data in m_reducedKappa[ros][drawer][couple][gain]
			for (int couple=0; couple<22; ++couple){
				if ( chan==int(getCoupleOfChan(ros, couple).first) || chan==int(getCoupleOfChan(ros, couple).second) ) {					
					Q1Q2[couple]*=ampInPicoCoulombs;
					currentDrawer = drawer;			

					if (Q1Q2[couple]!=1 && Q1Q2[couple]!=ampInPicoCoulombs) m_reducedKappa[ros][drawer][couple][gain]->Push( Q1Q2[couple] );
				}								
			}
					
    } // End of the loop over the TileRawChannelCollection
    
  } // End of the loop over the TileRawChannelContainer

  for (int ros=0; ros<4; ros++) {
    meantime[ros]->Push(avg_time[ros]->Mean());
    //    printf("%6.2f %6.2f\n",avg_time[ros]->Mean(), meantime[ros]->Mean());
    delete(avg_time[ros]);
  }

  
  return StatusCode::SUCCESS;  
}       

//
// Calibration coefficient calculation :
//
// Event loop is finished, we could then compute the coefficient
// 
//

StatusCode TileLaserDefaultCalibTool::finalizeCalculations()
{
  
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "finalizeCalculations()" << endreq;


  for(int d=0; d<2; ++d) 
  {
    m_PMT[d]      = PMT_signal[d]->Mean();    
    m_PMT_S[d]    = PMT_signal[d]->StandardDeviation();  
  }

  for(int d=0; d<4; ++d) 
  {
    m_diode[d]       = diode_signal[d]->Mean();    
    m_diode_S[d]     = diode_signal[d]->StandardDeviation();    
  }

  for(int i=0; i<4; ++i) {
    m_meantime[i] = meantime[i]->Mean();
    for(int drawer=0; drawer<64; ++drawer) {
      for(int gain=0; gain<2; ++gain) {


				/*/---- V.Giangiobbe : compute the average kappa correction factor for all even and odd PMTs
				kappa is by definition = Cov(q1,q2)/<q1>.<q2> average on all couples of PMTs q1,q2 recieving light from the same clear fiber (only 2 independents
				kappa for each module)
			  */
				int nCouplesEven=0, nCouplesOdd=0;
				for (int couple=0; couple<22; ++couple){
					int chan0 = getCoupleOfChan(i, couple).first;
					int chan1 = getCoupleOfChan(i, couple).second;
					double q0 = signal[i][drawer][chan0][gain]->Mean();
					double q1 = signal[i][drawer][chan1][gain]->Mean();
					
					if (q0*q1==0) continue;
										
					//-- Average of all couples on the same even fiber
					if (couple%2==0){
						m_kappa[i][drawer][0][gain] += (m_reducedKappa[i][drawer][couple][gain]->Mean()/(q0*q1) - 1);
						++nCouplesEven;
					} else {
						m_kappa[i][drawer][1][gain] += (m_reducedKappa[i][drawer][couple][gain]->Mean()/(q0*q1) - 1);
						++nCouplesOdd;
					}										
				}			
				if (nCouplesEven!=0) m_kappa[i][drawer][0][gain] = m_kappa[i][drawer][0][gain]/nCouplesEven;
				if (nCouplesOdd!=0)  m_kappa[i][drawer][1][gain] = m_kappa[i][drawer][1][gain]/nCouplesOdd;

				for (int couple=0; couple<22; ++couple){
					int chan0 = getCoupleOfChan(i, couple).first;
					int chan1 = getCoupleOfChan(i, couple).second;
					if (couple%2==0){
						m_kappa[i][drawer][chan0][gain] = m_kappa[i][drawer][0][gain];	
						m_kappa[i][drawer][chan1][gain] = m_kappa[i][drawer][0][gain];	
					} else {
						m_kappa[i][drawer][chan0][gain] = m_kappa[i][drawer][1][gain];	
						m_kappa[i][drawer][chan1][gain] = m_kappa[i][drawer][1][gain];						
					}
				}
				m_kappa[i][drawer][32][gain] = m_kappa[i][drawer][0][gain];				
				//-- end of Kappa calculation
				
				for(int k=0; k<48; ++k) 
				{ 
					m_time[i][drawer][k][gain]    = time[i][drawer][k][gain]->Mean();
					m_time_S[i][drawer][k][gain]  = time[i][drawer][k][gain]->StandardDeviation();
					m_mean[i][drawer][k][gain]    = signal[i][drawer][k][gain]->Mean();
					m_mean_S[i][drawer][k][gain]  = signal[i][drawer][k][gain]->StandardDeviation();
	  								
					//-- V.Giangiobbe : save the average charge and variance in slices of m_eventsPerSlice=1000
					if (m_pisaMethod2){
						for (int iSlice=0; iSlice<signal[i][drawer][k][gain]->GetNSlices(); ++iSlice){
							if (iSlice>=100) continue;
							m_mean_slice[i][drawer][k][iSlice][gain]     = signal[i][drawer][k][gain]->Mean(iSlice);
							m_variance_slice[i][drawer][k][iSlice][gain] = signal[i][drawer][k][gain]->Variance(iSlice);	  				
						}
					}
					
					for(int d=0; d<4; ++d) {
					  m_ratio[d][i][drawer][k][gain]   = ratio[d][i][drawer][k][gain]->Mean();
					  m_ratio_S[d][i][drawer][k][gain] = ratio[d][i][drawer][k][gain]->StandardDeviation();
					}
					m_entries[i][drawer][k][gain] = signal[i][drawer][k][gain]->NumDataValues();
				}
      }
    }
  } 


  return StatusCode::SUCCESS;  
}       

//
// Calibration coefficient storage :
//
// This method is called from the LaserCalibAlg
// it fills the final ntuple, which name is decided here.
// The name of the ROOTfile is chosen in the algorithm
// 


StatusCode TileLaserDefaultCalibTool::writeNtuple(int runNumber, int runType, TFile * rootFile)
{
  
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "finalize(" << runNumber << "," << runType << "," << rootFile << ")" << endreq;

  // Create output tree

  m_toolRunNo = runNumber;

  TTree *t = new TTree(m_toolNtuple.c_str(), "TileLaserCalib-Ntuple");
  t->Branch("RunNumber",&m_toolRunNo, "runNo/I");
  t->Branch("ADC_status",&m_ADC_problem, "ADC/I");
  t->Branch("WheelPos",&m_las_filter, "wheelpos/I");
  t->Branch("RequestedAmp",&m_las_requ_amp, "requamp/F");
  t->Branch("Humidity",&m_hrate,"humid/F"); 
  t->Branch("AirFlow",&m_flow,"flow/F"); 
  t->Branch("HeadTemp",&m_head_temp,"htemp/F");
  t->Branch("TimeRun",&m_las_time, "timeofrun/F");
  t->Branch("PMT1_Signal",&m_PMT[0], "PMT_1/F");
  t->Branch("PMT2_Signal",&m_PMT[1], "PMT_2/F");
  t->Branch("PMT_Sigma_Signal",m_PMT_S, "PMT_s[2]/F"); 
  t->Branch("Diode_Signal",m_diode, "diode[4]/F");
  t->Branch("Diode_Sigma_Signal",m_diode_S, "diode_s[4]/F");
  t->Branch("Diode_Ped",m_diode_Ped, "diode_Ped[4]/F");
  t->Branch("Diode_Sigma_Ped",m_diode_SPed, "diode_sPed[4]/F");
  t->Branch("Diode_Alpha",m_diode_Alpha, "diode_Alpha[4]/F");
  t->Branch("Diode_Sigma_Alpha",m_diode_SAlpha, "diode_sAlpha[4]/F");
  t->Branch("MeanTime",m_meantime,"meantime[4]/F");
  t->Branch("Time",*m_time,"time[4][64][48][2]/F");
  t->Branch("Sigma_Time",*m_time_S,"time_s[4][64][48][2]/F");
  t->Branch("Signal",*m_mean,"signal[4][64][48][2]/F");
  t->Branch("Sigma_Signal",*m_mean_S,"signal_s[4][64][48][2]/F");
  t->Branch("Ratio",*m_ratio,"signal_cor[4][4][64][48][2]/F");
  t->Branch("Sigma_Ratio",*m_ratio_S,"signal_cor_s[4][4][64][48][2]/F");
  t->Branch("LaserEntries",*m_entries,"LASER_entries[4][64][48][2]/I");  
  t->Branch("Kappa",*m_kappa,"Kappa[4][64][48][2]/F");  
  t->Branch("Status",*m_status,"Status[4][64][48][2]/S");
  t->Branch("HV",*m_HV,"HV[4][64][48]/F");
  t->Branch("HVSet",*m_HVSet,"HVSet[4][64][48]/F");

  if (m_pisaMethod2){  
    t->Branch("MeanSlice",*m_mean_slice,"MeanSlice[4][64][48][100][2]/F");
    t->Branch("VarianceSlice",*m_variance_slice,"VarianceSlice[4][64][48][100][2]/F");
  }


  // Fill with current values (i.e. tree will have only one entry for this whole run)
  t->Fill();
  t->Write();
  
  return StatusCode::SUCCESS;  
}

StatusCode TileLaserDefaultCalibTool::finalize()
{

  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "finalize()" << endreq;

  StatusCode sc = TileLaserDefaultCalibTool::finalizeCalculations(); // Perform the analysis

  
  for(int part=0; part<4; ++part) 
    {
      int ros = part+1;
      for(int drawer=0; drawer<64; ++drawer) 
	{
	  int module = drawer+1;
	  for(int channel=0; channel<48; ++channel) 
	    { 
	      int pmt = abs(m_cabling->channel2hole(ros,channel));

	      m_HV[part][drawer][channel] = m_tileDCSSvc->getDCSHV(ros, module, pmt);
	      m_HVSet[part][drawer][channel] = m_tileDCSSvc->getDCSHVSET(ros, module, pmt);
	      /*      if (part==2 && drawer==0){
		std::cout << part << " " << channel  << " " << m_cabling->channel2hole(ros,channel)  << " " <<  m_HV[part][drawer][channel] << " " <<  m_HVSet[part][drawer][channel] << std::endl;
		}*/
	    }
	}
    }



  if (sc.isFailure()) {
    log << MSG::ERROR
	<< "Failure in DefaultLaserTool finalization!"
	<< endreq;
    return StatusCode::FAILURE;
  } 

  return StatusCode::SUCCESS;  
}  


//--- get couple of channels that are on the same clear fiber
std::pair<unsigned int, unsigned int> TileLaserDefaultCalibTool::getCoupleOfChan(int ros, int couple){
  std::pair<unsigned int, unsigned int> coupleOfChannels;

  int chan1LB[22]={0, 1, 4, 5, 8,  9,  12, 13, 16, 17, 20, 21, 26, 25, 28, 27, 38, 35, 40, 37, 42, 47};
  int chan2LB[22]={2, 3, 6, 7, 10, 11, 14, 15, 18, 19, 22, 23, 24, 29, 34, 39, 36, 33, 44, 41, 46, 45};

  int chan1EB[22]={0, 1, 4, 5,  8, 9,  12, 13, 16, 17, 22, 23, 30, 35, 41, 40, -1, -1, -1, -1, -1, -1};
  int chan2EB[22]={2, 3, 6, 7, 10, 11, 14, 15, 20, 21, 31, 32, 38, 37, 39, 36, -1, -1, -1, -1, -1, -1};
  
  //--- LB, 
  if (ros<2){
		coupleOfChannels.first  = chan1LB[couple];
		coupleOfChannels.second = chan2LB[couple];
  }
  
	//--- EB
  if (ros>=2){
		coupleOfChannels.first  = chan1EB[couple];
		coupleOfChannels.second = chan2EB[couple];   
  }
  return coupleOfChannels;
    
}

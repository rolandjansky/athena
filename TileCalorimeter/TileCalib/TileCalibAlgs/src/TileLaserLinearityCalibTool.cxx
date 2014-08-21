/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalibAlgs/TileLaserLinearityCalibTool.h"

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

#include "TFile.h"
#include "TTree.h"
#include <cmath>
#include <iostream>

/**********************************************************/
/* TileLaserLinearityCalibTool.cxx     November 19th 2009 */
/*                                                        */
/* S.Viret       viret@in2p3.fr                           */
/*                                                        */
/* This code is inspired from the CIS tool                */
/**********************************************************/

TileLaserLinearityCalibTool::TileLaserLinearityCalibTool(const std::string& type, const std::string& name,const IInterface* pParent):
  AlgTool(type, name, pParent)
{
  declareInterface<ITileCalibTool>( this );
  declareProperty("toolNtuple", m_toolNtuple="h3000");
  declareProperty("rawChannelContainer", m_rawChannelContainerName="");
  declareProperty("laserObjContainer", m_laserContainerName="");
}

TileLaserLinearityCalibTool::~TileLaserLinearityCalibTool()
{}


//
// Tool instantiation :
//
// Used to reset all the local variables, 
// and to load the necessary services
//

 
StatusCode TileLaserLinearityCalibTool::initialize()
{
  
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "initialize()" << endreq;

  // Reset local parameters
  // For parameter definition, see the header file
  //

  m_complete_turn = false;

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
  
  last_evt_filter  = 0; 
  n_same_filt_evts = 0; 

  for(int f=0; f<8; ++f) 
  {
    for(int d=0; d<2; ++d) 
    {
      m_PMT[f][d]      = 0;
      m_PMT_S[f][d]    = 0;
      PMT_signal[f][d] = new RunningStat();
    }
  }

  for(int f=0; f<8; ++f) 
  {
    for(int d=0; d<4; ++d) 
    {
      m_diode[f][d]       = 0;    
      m_diode_S[f][d]     = 0;  
      diode_signal[f][d]  = new RunningStat();
    }
  }

  for(int f=0; f<8; ++f)        // Filter 
  {
    for(int i=0; i<4; ++i)      // Partition
    {
      for(int j=0; j<64; ++j)   // Module
      {
	for(int k=0; k<48; ++k) // Channel
	{ 
	  for(int l=0; l<2; ++l) // Gain
	  { 
	    signal[f][i][j][k][l]    = new RunningStat();
	    m_mean[f][i][j][k][l]    = 0;
	    m_mean_S[f][i][j][k][l]  = 0;
	    
	    ratio[f][i][j][k][l]     = new RunningStat();
	    m_ratio[f][i][j][k][l]   = 0;
	    m_ratio_S[f][i][j][k][l] = 0;
	    ratio2[f][i][j][k][l]    = new RunningStat();
	    m_ratio2[f][i][j][k][l]  = 0;
	    m_ratio2_S[f][i][j][k][l]= 0;
	    m_entries[f][i][j][k][l] = 0;	 
	  }
	}
      }
    }
  } 


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

  return StatusCode::SUCCESS;  
}       

StatusCode TileLaserLinearityCalibTool::initNtuple(int runNumber, int runType, TFile * rootFile)
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


StatusCode TileLaserLinearityCalibTool::execute()
{
  
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "execute()" << endreq;

  //
  // Here we analyze a run with filter wheel moving
  // we just keep the information from the first turn of the wheel
  //

  if (m_complete_turn)
  {
    log << MSG::INFO << "Filter wheel turn completed: stop here" << endreq;
    return StatusCode::SUCCESS;
  }

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
    log << MSG::VERBOSE << "Filter wheel moving: skip event !!" << endreq;
    return StatusCode::SUCCESS; // This is expected for some events 
  }

  //
  // Filter number is different from 0
  // But we have to check few points in order to be sure 
  // that the wheel is stabilized 
  //

  m_las_filter = laserObj->getFiltNumber()-1; // Corrected filter position

  if (last_evt_filter == m_las_filter)  // If the position the same as before??
  {
    n_same_filt_evts +=1;
  }
  else
  {
    n_same_filt_evts = 0;
  }
  
  last_evt_filter = m_las_filter;


  //
  // As long as we don't have a suffecient number of consecutive events 
  // with the same filter (200), we consider that we are not stable and thus
  // we reject the entire event
  //

  if (n_same_filt_evts < 200)   // Sanity check
  {
    log << MSG::WARNING << "Still in a non stable area, wait a bit !!" << endreq;
    return StatusCode::SUCCESS; // This is expected for some events 
  }

  // 
  // Then we get the diodes signals and store them
  //

  double diode[4];
    
  for(int i=0; i<4; ++i) 
  {
    diode[i] = 0; 
    if (laserObj->getDiodeADC(i) <= 2044) // Diode signal OK
      diode[i]   = static_cast<double>(laserObj->getDiodeADC(i)-laserObj->getDiodePedestal(i));
  }

  for(int i=0; i<4; ++i) 
    diode_signal[m_las_filter][i]->Push(diode[i]);

  for(int i=0; i<2; ++i) 
    PMT_signal[m_las_filter][i]->Push(laserObj->getPMADC(i)-laserObj->getPMPedestal(i));


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

  if (m_hrate == 0) 
  {
    first_filter   = laserObj->getFiltNumber()-1; // The first filter position
    m_hrate        = laserObj->getHumidity(); 
    m_flow         = laserObj->getGasFlux(); 
    m_head_temp    = laserObj->getPumpDiodeTemp();
    m_las_requ_amp = laserObj->getDiodeCurrOrd();
  }

  //
  // Are we back to the first wheel position??
  // If so we stop here because otherwise
  // we have systematic shifts with filter wheel position
  //

  if (m_las_filter==first_filter) 
  {
    int previous_filter = (first_filter+7)%8;
    if (PMT_signal[previous_filter][0]->NumDataValues()>400) 
    {
      m_complete_turn=true; // Yes, stop here 
      log << MSG::INFO << "Filter wheel turn completed: stop here" << endreq;
      return StatusCode::SUCCESS;
    }
  }

  //
  // Then we could loop over all the channels, and get the correction factor
  //


  // Create iterator over RawChannelContainer 
  TileRawChannelUnit::UNIT RChUnit = rawCnt->get_unit();
  TileRawChannelContainer::const_iterator itColl = rawCnt->begin();
  TileRawChannelContainer::const_iterator itCollEnd = rawCnt->end();
  
  // Go through all TileRawChannelCollections
  for(; itColl != itCollEnd; ++itColl) 
  {
       
    // go through all TileRawChannels in collection
    TileRawChannelCollection::const_iterator it = (*itColl)->begin();
    for(; it != (*itColl)->end(); ++it) 
    {
 
      // get adcHash
      HWIdentifier hwid=(*it)->adc_HWID();
      int ros    = m_tileHWID->ros(hwid)-1;    // LBA=0  LBC=1  EBA=2  EBC=3 
      int drawer = m_tileHWID->drawer(hwid);   // 0 to 63
      int chan   = m_tileHWID->channel(hwid);  // 0 to 47 channel not PMT
      int gain   = m_tileHWID->adc(hwid);      // low=0 high=1

      // amp has to be in PicoCoulombs, here we are mixing gains 
      float amp = (*it)->amplitude();
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros+1,drawer);



      // check if channel is connected
      if( !chanIsConnected(ros+1,chan) ) continue;
     
      // Don't enable negative or null energies
      if ( amp <= 0.) continue;
  
      float ampInPicoCoulombs = m_tileToolEmscale->channelCalib(drawerIdx, chan, gain, amp, RChUnit, TileRawChannelUnit::PicoCoulombs);

      //
      // Here we do an outlier removal when enough events have been collected
      // (Mean and RMS are computed in the same pass
      //)
      // Useful when the filter wheel is moving again (the signal could become bad again)
      //

      if (signal[m_las_filter][ros][drawer][chan][gain]->NumDataValues()>400) 
      {                                                                 
	double current_mean = signal[m_las_filter][ros][drawer][chan][gain]->Mean();
 	double current_RMS  = signal[m_las_filter][ros][drawer][chan][gain]->StandardDeviation();

	if (fabs(ampInPicoCoulombs-current_mean)>5*current_RMS) // 5 sigmas away, not normal
	{
	  log << MSG::VERBOSE << "Rejecting an outlier. If there are a lot this means filter wheel will soon move again!" << endreq;
	}
	else
	{
	  signal[m_las_filter][ros][drawer][chan][gain]->Push(ampInPicoCoulombs);
	  
	  if (diode[0] != 0)
	  {
	    ratio[m_las_filter][ros][drawer][chan][gain]->Push(ampInPicoCoulombs/diode[0]);
	    ratio2[m_las_filter][ros][drawer][chan][gain]->Push(ampInPicoCoulombs/(laserObj->getPMADC(0)-laserObj->getPMPedestal(0)));
	  }

	}
      }
      else
      {
	if (ros == 2 && drawer == 0 && chan == 1)	
	  log << MSG::VERBOSE << m_las_filter+1 <<" / " << gain <<" / " << ampInPicoCoulombs <<" / " 
	      << signal[m_las_filter][ros][drawer][chan][gain]->Mean() 
	      << " / " << signal[m_las_filter][ros][drawer][chan][gain]->StandardDeviation()
	      << endreq;

	signal[m_las_filter][ros][drawer][chan][gain]->Push(ampInPicoCoulombs);

	if (diode[0] != 0)
	{
	  ratio[m_las_filter][ros][drawer][chan][gain]->Push(ampInPicoCoulombs/diode[0]);
	  ratio2[m_las_filter][ros][drawer][chan][gain]->Push(ampInPicoCoulombs/(laserObj->getPMADC(0)-laserObj->getPMPedestal(0)));
	}
      }		
    } // End of the loop over the TileRawChannelCollection
  } // End of the loop over the TileRawChannelContainer
  
  return StatusCode::SUCCESS;  
}       

//
// Calibration coefficient calculation :
//
// Event loop is finished, we could then compute the coefficient
// 
//

StatusCode TileLaserLinearityCalibTool::finalizeCalculations()
{
  
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "execute()" << endreq;

  for(int f=0; f<8; ++f) 
  {
    for(int d=0; d<2; ++d) 
    {
      m_PMT[f][d]      = PMT_signal[f][d]->Mean();    
      m_PMT_S[f][d]    = PMT_signal[f][d]->StandardDeviation();  
    }
  }

  for(int f=0; f<8; ++f) 
  {
    for(int d=0; d<4; ++d) 
    {
      m_diode[f][d]       = diode_signal[f][d]->Mean();    
      m_diode_S[f][d]     = diode_signal[f][d]->StandardDeviation();    
    }
  }

  for(int f=0; f<8; ++f) 
  {
    for(int i=0; i<4; ++i) 
    {
      for(int j=0; j<64; ++j) 
      {
	for(int k=0; k<48; ++k) 
	{ 
	  for(int l=0; l<2; ++l) 
	  { 
	    m_mean[f][i][j][k][l]    = signal[f][i][j][k][l]->Mean();
	    m_mean_S[f][i][j][k][l]  = signal[f][i][j][k][l]->StandardDeviation();
	    
	    m_ratio[f][i][j][k][l]   = ratio[f][i][j][k][l]->Mean();
	    m_ratio_S[f][i][j][k][l] = ratio[f][i][j][k][l]->StandardDeviation();
	    m_ratio2[f][i][j][k][l]   = ratio2[f][i][j][k][l]->Mean();
	    m_ratio2_S[f][i][j][k][l] = ratio2[f][i][j][k][l]->StandardDeviation();
	    m_entries[f][i][j][k][l] = signal[f][i][j][k][l]->NumDataValues();
	  }
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


StatusCode TileLaserLinearityCalibTool::writeNtuple(int runNumber, int runType, TFile * rootFile)
{
  
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "finalize(" << runNumber << "," << runType << "," << rootFile << ")" << endreq;

  // Create output tree

  m_toolRunNo = runNumber;

  TTree *t = new TTree(m_toolNtuple.c_str(), "TileLaserCalib-Ntuple");
  t->Branch("RunNumber",&m_toolRunNo, "runNo/I");
  t->Branch("ADC_status",&m_ADC_problem, "ADC/I");
  t->Branch("RequestedAmp",&m_las_requ_amp, "requamp/D");
  t->Branch("Humidity",&m_hrate,"humid/D"); 
  t->Branch("AirFlow",&m_flow,"flow/D"); 
  t->Branch("HeadTemp",&m_head_temp,"htemp/D");
  t->Branch("Time",&m_las_time, "timeofrun/D");
  t->Branch("PMT_Signal",m_PMT, "PMT[8][2]/D");
  t->Branch("PMT_Sigma_Signal",m_PMT_S, "PMT_s[8][2]/D"); 
  t->Branch("Diode_Signal",m_diode, "diode[8][4]/D");
  t->Branch("Diode_Sigma_Signal",m_diode_S, "diode_s[8][4]/D");
  t->Branch("Signal",*m_mean,"signal[8][4][64][48][2]/D");
  t->Branch("Sigma_Signal",*m_mean_S,"signal_s[8][4][64][48][2]/D");
  t->Branch("Ratio",*m_ratio,"signal_cor[8][4][64][48][2]/D");
  t->Branch("Sigma_Ratio",*m_ratio_S,"signal_cor_s[8][4][64][48][2]/D");
  t->Branch("Ratio2",*m_ratio2,"signal_cor2[8][4][64][48][2]/D");
  t->Branch("Sigma_Ratio2",*m_ratio2_S,"signal_cor2_s[8][4][64][48][2]/D");
  t->Branch("Entries",*m_entries,"n_LASER_entries[8][4][64][48][2]/I");  

     
  // Fill with current values (i.e. tree will have only one entry for this whole run)
  t->Fill();
  t->Write();
  
  return StatusCode::SUCCESS;  
}

StatusCode TileLaserLinearityCalibTool::finalize()
{

  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "finalize()" << endreq;

  StatusCode sc = TileLaserLinearityCalibTool::finalizeCalculations(); // Perform the analysis

  if (sc.isFailure()) {
    log << MSG::ERROR
	<< "Failure in LinearityLaserTool finalization!"
	<< endreq;
    return StatusCode::FAILURE;
  } 

  return StatusCode::SUCCESS;  
}  


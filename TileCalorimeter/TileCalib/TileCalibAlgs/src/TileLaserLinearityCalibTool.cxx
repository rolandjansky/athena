/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalibAlgs/TileLaserLinearityCalibTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"


#include "Identifier/HWIdentifier.h"
#include "StoreGate/ReadHandle.h"

#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileLaserObject.h"
#include "TileConditions/TileCablingService.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

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
  AthAlgTool(type, name, pParent),
  m_tileHWID (nullptr),
  m_cabling (nullptr),
  m_tileToolEmscale("TileCondToolEmscale"),
  m_toolRunNo(0),
  m_ADC_problem(0),
  m_las_filter(0),
  m_las_requ_amp(0),
  m_hrate(0),
  m_flow(0),
  m_head_temp(0),
  m_las_time(0),
  m_LG_PMT(),
  m_LG_PMT_S(),
  m_LG_diode(),
  m_LG_diode_S(),
  m_HG_PMT(),
  m_HG_PMT_S(),
  m_HG_diode(),
  m_HG_diode_S(),
  m_mean(),
  m_mean_S(),
  m_LG_ratio(),
  m_LG_ratio_S(),
  m_LG_ratio2(),
  m_LG_ratio2_S(),
  m_HG_ratio(),
  m_HG_ratio_S(),
  m_HG_ratio2(),
  m_HG_ratio2_S(),
  m_entries(),
  m_PMT1_ADC_prev(),
  m_PMT2_ADC_prev(),
  m_first_filter(0),
  m_last_evt_filter(0),
  m_n_same_filt_evts(0),
  m_complete_turn(0),
  m_HG_diode_signal(),
  m_HG_PMT_signal(),
  m_LG_diode_signal(),
  m_LG_PMT_signal(),
  m_signal(),
  m_LG_ratio_stat(),
  m_LG_ratio2_stat(),
  m_HG_ratio_stat(),
  m_HG_ratio2_stat()
{
  declareInterface<ITileCalibTool>( this );
  declareProperty("toolNtuple", m_toolNtuple="h3000");
  declareProperty("TileDQstatus", m_dqStatusKey = "TileDQstatus");
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
  
  ATH_MSG_INFO ( "initialize()" );

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
  m_PMT1_ADC_prev[0]= 0;
  m_PMT2_ADC_prev[0]= 0;
  m_PMT1_ADC_prev[1]= 0;
  m_PMT2_ADC_prev[1]= 0;

  m_last_evt_filter  = 0; 
  m_n_same_filt_evts = 0; 

  for(int f=0; f<8; ++f) 
  {
    for(int d=0; d<2; ++d) 
    {
      m_LG_PMT[f][d]      = 0;
      m_LG_PMT_S[f][d]    = 0;
      m_LG_PMT_signal[f][d] = new RunningStat();
      m_HG_PMT[f][d]      = 0;
      m_HG_PMT_S[f][d]    = 0;
      m_HG_PMT_signal[f][d] = new RunningStat();
    }
  }

  for(int f=0; f<8; ++f) 
  {
    for(int d=0; d<4; ++d) 
    {
      m_LG_diode[f][d]       = 0;    
      m_LG_diode_S[f][d]     = 0;  
      m_LG_diode_signal[f][d]  = new RunningStat();

      m_HG_diode[f][d]       = 0;    
      m_HG_diode_S[f][d]     = 0;  
      m_HG_diode_signal[f][d]  = new RunningStat();
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
	    m_signal[f][i][j][k][l]    = new RunningStat();
	    m_mean[f][i][j][k][l]    = 0;
	    m_mean_S[f][i][j][k][l]  = 0;
	    
	    m_HG_ratio_stat[f][i][j][k][l]     = new RunningStat();
	    m_HG_ratio[f][i][j][k][l]   = 0;
	    m_HG_ratio_S[f][i][j][k][l] = 0;
	    m_HG_ratio2_stat[f][i][j][k][l]    = new RunningStat();
	    m_HG_ratio2[f][i][j][k][l]  = 0;
	    m_HG_ratio2_S[f][i][j][k][l]= 0;

	    m_LG_ratio_stat[f][i][j][k][l]     = new RunningStat();
	    m_LG_ratio[f][i][j][k][l]   = 0;
	    m_LG_ratio_S[f][i][j][k][l] = 0;
	    m_LG_ratio2_stat[f][i][j][k][l]    = new RunningStat();
	    m_LG_ratio2[f][i][j][k][l]  = 0;
	    m_LG_ratio2_S[f][i][j][k][l]= 0;

	    m_entries[f][i][j][k][l] = 0;	 
	  }
	}
      }
    }
  } 


  ATH_CHECK( detStore()->retrieve(m_tileHWID) );
  ATH_CHECK( m_tileToolEmscale.retrieve() );

  ATH_CHECK( m_rawChannelContainerKey.initialize() );
  ATH_CHECK( m_laserContainerKey.initialize() );

  m_cabling = TileCablingService::getInstance();

  CHECK( m_dqStatusKey.initialize() );

  return StatusCode::SUCCESS;  
}       

StatusCode TileLaserLinearityCalibTool::initNtuple(int runNumber, int runType, TFile * rootFile)
{
  ATH_MSG_INFO ( "initialize(" << runNumber << "," << runType << "," << rootFile << ")" );
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
  ATH_MSG_INFO ( "execute()" );

  const EventContext& ctx = Gaudi::Hive::currentContext();
  const TileDQstatus* dqStatus = SG::makeHandle (m_dqStatusKey, ctx).get();

  //
  // Here we analyze a run with filter wheel moving
  // we just keep the information from the first turn of the wheel
  //

  if (m_complete_turn)
  {
    ATH_MSG_INFO ( "Filter wheel turn completed: stop here" );
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG ( "Retrieving the LASER object and RawChannel" );

  SG::ReadHandle<TileRawChannelContainer> rawCnt(m_rawChannelContainerKey);
  SG::ReadHandle<TileLaserObject> laserObj(m_laserContainerKey);
  
  ATH_CHECK( rawCnt.isValid() );
  ATH_CHECK( laserObj.isValid() );

  // First we got event time (From 1/1/70)

  const uint32_t *cispar = dqStatus->cispar();
  
  m_las_time = static_cast<double>(cispar[10])+static_cast<double>(cispar[11])/1000000;

  //
  // Then we could start getting the LASER informations 
  // 


  if (laserObj->getDiodeCurrOrd() == 0 || laserObj->getFiltNumber() == 0)   // Sanity check
  {
    ATH_MSG_VERBOSE ( "Filter wheel moving: skip event !!" );
    return StatusCode::SUCCESS; // This is expected for some events 
  }

  //
  // Filter number is different from 0
  // But we have to check few points in order to be sure 
  // that the wheel is stabilized 
  //

  m_las_filter = laserObj->getFiltNumber()-1; // Corrected filter position

  if (m_last_evt_filter == m_las_filter)  // If the position the same as before??
  {
    m_n_same_filt_evts +=1;
  }
  else
  {
    m_n_same_filt_evts = 0;
  }
  
  m_last_evt_filter = m_las_filter;


  //
  // As long as we don't have a suffecient number of consecutive events 
  // with the same filter (200), we consider that we are not stable and thus
  // we reject the entire event
  //

  if (m_n_same_filt_evts < 200)   // Sanity check
  {
    ATH_MSG_WARNING ( "Still in a non stable area, wait a bit !!" );
    return StatusCode::SUCCESS; // This is expected for some events 
  }

  // 
  // Then we get the diodes signals and store them
  //

  double LG_diode[10];
  double HG_diode[10];
    
  for(int i=0; i<10; ++i) 
  {
    LG_diode[i] = static_cast<double>(laserObj->getDiodeADC(i,0)-laserObj->getDiodePedestal(i,0));
    HG_diode[i] = static_cast<double>(laserObj->getDiodeADC(i,1)-laserObj->getDiodePedestal(i,1));
  } // FOR

  for(int i=0; i<10; ++i){ 
    m_LG_diode_signal[m_las_filter][i]->Push(LG_diode[i]);
    m_HG_diode_signal[m_las_filter][i]->Push(HG_diode[i]);
  } // FOR

  for(int i=0; i<2; ++i){
    m_LG_PMT_signal[m_las_filter][i]->Push(laserObj->getPMADC(i,0)-laserObj->getPMPedestal(i,0));
    m_HG_PMT_signal[m_las_filter][i]->Push(laserObj->getPMADC(i,1)-laserObj->getPMPedestal(i,1));
  } // FOR

  // Check that the ADC has really sent a new information 

  if (laserObj->getPMADC(0,0) == m_PMT1_ADC_prev[0] && 
      laserObj->getPMADC(1,0) == m_PMT2_ADC_prev[0])
  {
    m_ADC_problem = 1;
    ATH_MSG_WARNING ( "There is perhaps an ADC problem with this event" );
  }
  if (laserObj->getPMADC(0,1) == m_PMT1_ADC_prev[1] && 
      laserObj->getPMADC(1,1) == m_PMT2_ADC_prev[1])
  {
    m_ADC_problem = 1;
    ATH_MSG_WARNING ( "There is perhaps an ADC problem with this event" );
  }

  m_PMT1_ADC_prev[0] = laserObj->getPMADC(0,0);
  m_PMT2_ADC_prev[0] = laserObj->getPMADC(1,0);
  m_PMT1_ADC_prev[1] = laserObj->getPMADC(0,1);
  m_PMT2_ADC_prev[1] = laserObj->getPMADC(1,1);

  // Next parameters are constants, don't need to update them more than once

  if (m_hrate == 0) 
  {
    m_first_filter   = laserObj->getFiltNumber()-1; // The first filter position
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

  if (m_las_filter==m_first_filter) 
  {
    int previous_filter = (m_first_filter+7)%8;
    if (m_HG_PMT_signal[previous_filter][0]->NumDataValues()>400 || m_LG_PMT_signal[previous_filter][0]->NumDataValues()>400) 
    {
      m_complete_turn=true; // Yes, stop here 
      ATH_MSG_INFO ( "Filter wheel turn completed: stop here" );
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

      if (m_signal[m_las_filter][ros][drawer][chan][gain]->NumDataValues()>400) 
      {                                                                 
	double current_mean = m_signal[m_las_filter][ros][drawer][chan][gain]->Mean();
 	double current_RMS  = m_signal[m_las_filter][ros][drawer][chan][gain]->StandardDeviation();

	if (fabs(ampInPicoCoulombs-current_mean)>5*current_RMS) // 5 sigmas away, not normal
	{
	  ATH_MSG_VERBOSE ( "Rejecting an outlier. If there are a lot this means filter wheel will soon move again!" );
	}
	else
	{
	  m_signal[m_las_filter][ros][drawer][chan][gain]->Push(ampInPicoCoulombs);
	  
	  if (HG_diode[0] != 0)
	  {
	    m_HG_ratio_stat[m_las_filter][ros][drawer][chan][gain]->Push(ampInPicoCoulombs/HG_diode[0]);
	    m_HG_ratio2_stat[m_las_filter][ros][drawer][chan][gain]->Push(ampInPicoCoulombs/(laserObj->getPMADC(0,1)-laserObj->getPMPedestal(0,1)));
	  }

	  if (LG_diode[0] != 0)
	  {
	    m_LG_ratio_stat[m_las_filter][ros][drawer][chan][gain]->Push(ampInPicoCoulombs/LG_diode[0]);
	    m_LG_ratio2_stat[m_las_filter][ros][drawer][chan][gain]->Push(ampInPicoCoulombs/(laserObj->getPMADC(0,0)-laserObj->getPMPedestal(0,0)));
	  }

	}
      }
      else
      {
	if (ros == 2 && drawer == 0 && chan == 1)	
	  ATH_MSG_VERBOSE ( m_las_filter+1 <<" / " << gain <<" / " << ampInPicoCoulombs <<" / " 
                            << m_signal[m_las_filter][ros][drawer][chan][gain]->Mean() 
                            << " / " << m_signal[m_las_filter][ros][drawer][chan][gain]->StandardDeviation() );

	m_signal[m_las_filter][ros][drawer][chan][gain]->Push(ampInPicoCoulombs);

	if (HG_diode[0] != 0)
	{
	  m_HG_ratio_stat[m_las_filter][ros][drawer][chan][gain]->Push(ampInPicoCoulombs/HG_diode[0]);
	  m_HG_ratio2_stat[m_las_filter][ros][drawer][chan][gain]->Push(ampInPicoCoulombs/(laserObj->getPMADC(0,1)-laserObj->getPMPedestal(0,1)));
	}

	if (LG_diode[0] != 0)
	{
	  m_LG_ratio_stat[m_las_filter][ros][drawer][chan][gain]->Push(ampInPicoCoulombs/LG_diode[0]);
	  m_LG_ratio2_stat[m_las_filter][ros][drawer][chan][gain]->Push(ampInPicoCoulombs/(laserObj->getPMADC(0,0)-laserObj->getPMPedestal(0,0)));
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
  ATH_MSG_INFO ( "execute()" );

  for(int f=0; f<8; ++f) 
  {
    for(int d=0; d<2; ++d) 
    {
      m_LG_PMT[f][d]      = m_LG_PMT_signal[f][d]->Mean();    
      m_LG_PMT_S[f][d]    = m_LG_PMT_signal[f][d]->StandardDeviation();  

      m_HG_PMT[f][d]      = m_HG_PMT_signal[f][d]->Mean();    
      m_HG_PMT_S[f][d]    = m_HG_PMT_signal[f][d]->StandardDeviation();  
    }
  }

  for(int f=0; f<8; ++f) 
  {
    for(int d=0; d<4; ++d) 
    {
      m_LG_diode[f][d]       = m_LG_diode_signal[f][d]->Mean();    
      m_LG_diode_S[f][d]     = m_LG_diode_signal[f][d]->StandardDeviation();    

      m_HG_diode[f][d]       = m_HG_diode_signal[f][d]->Mean();    
      m_HG_diode_S[f][d]     = m_HG_diode_signal[f][d]->StandardDeviation();    
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
	    m_mean[f][i][j][k][l]    = m_signal[f][i][j][k][l]->Mean();
	    m_mean_S[f][i][j][k][l]  = m_signal[f][i][j][k][l]->StandardDeviation();
	    
	    m_LG_ratio[f][i][j][k][l]   = m_LG_ratio_stat[f][i][j][k][l]->Mean();
	    m_LG_ratio_S[f][i][j][k][l] = m_LG_ratio_stat[f][i][j][k][l]->StandardDeviation();
	    m_LG_ratio2[f][i][j][k][l]   = m_LG_ratio2_stat[f][i][j][k][l]->Mean();
	    m_LG_ratio2_S[f][i][j][k][l] = m_LG_ratio2_stat[f][i][j][k][l]->StandardDeviation();
	    m_HG_ratio[f][i][j][k][l]   = m_HG_ratio_stat[f][i][j][k][l]->Mean();
	    m_HG_ratio_S[f][i][j][k][l] = m_HG_ratio_stat[f][i][j][k][l]->StandardDeviation();
	    m_HG_ratio2[f][i][j][k][l]   = m_HG_ratio2_stat[f][i][j][k][l]->Mean();
	    m_HG_ratio2_S[f][i][j][k][l] = m_HG_ratio2_stat[f][i][j][k][l]->StandardDeviation();
	    m_entries[f][i][j][k][l] = m_signal[f][i][j][k][l]->NumDataValues();
	  }
	}
      }
    }
  } 

  // remove all RunningStat objects from memory

  for(int f=0; f<8; ++f)
  {
    for(int d=0; d<2; ++d)
    {
      delete m_LG_PMT_signal[f][d];
      delete m_HG_PMT_signal[f][d];
    }
  }

  for(int f=0; f<8; ++f)
  {
    for(int d=0; d<4; ++d)
    {
      delete m_LG_diode_signal[f][d];
      delete m_HG_diode_signal[f][d];
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
	    delete m_signal[f][i][j][k][l];
	    delete m_HG_ratio_stat[f][i][j][k][l];
	    delete m_HG_ratio2_stat[f][i][j][k][l];
	    delete m_LG_ratio_stat[f][i][j][k][l];
	    delete m_LG_ratio2_stat[f][i][j][k][l];
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
  ATH_MSG_INFO ( "finalize(" << runNumber << "," << runType << "," << rootFile << ")" );

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
  t->Branch("LG_PMT_Signal",m_LG_PMT, "LG_PMT1[8][2]/D");
  t->Branch("LG_PMT_Sigma_Signal",m_LG_PMT_S, "LG_PMT1_s[8][2]/D");
  t->Branch("HG_PMT_Signal",m_HG_PMT, "HG_PMT1[8][2]/D");
  t->Branch("HG_PMT_Sigma_Signal",m_HG_PMT_S, "HG_PMT1_s[8][2]/D");
 
  t->Branch("LG_Diode_Signal",m_LG_diode, "LG_diode[8][10]/D");
  t->Branch("LG_Diode_Sigma_Signal",m_LG_diode_S, "LG_diode_s[8][10]/D");
  t->Branch("HG_Diode_Signal",m_HG_diode, "HG_diode[8][10]/D");
  t->Branch("HG_Diode_Sigma_Signal",m_HG_diode_S, "HG_diode_s[8][10]/D");

  t->Branch("Signal",*m_mean,"signal[8][4][64][48][2]/D");
  t->Branch("Sigma_Signal",*m_mean_S,"signal_s[8][4][64][48][2]/D");
  t->Branch("LG_Ratio",*m_LG_ratio,"LG_signal_cor[8][4][64][48][2]/D");
  t->Branch("LG_Sigma_Ratio",*m_LG_ratio_S,"LG_signal_cor_s[8][4][64][48][2]/D");
  t->Branch("LG_Ratio2",*m_LG_ratio2,"LG_signal_cor2[8][4][64][48][2]/D");
  t->Branch("LG_Sigma_Ratio2",*m_LG_ratio2_S,"LG_signal_cor2_s[8][4][64][48][2]/D");
  t->Branch("HG_Ratio",*m_HG_ratio,"HG_signal_cor[8][4][64][48][2]/D");
  t->Branch("HG_Sigma_Ratio",*m_HG_ratio_S,"HG_signal_cor_s[8][4][64][48][2]/D");
  t->Branch("HG_Ratio2",*m_HG_ratio2,"HG_signal_cor2[8][4][64][48][2]/D");
  t->Branch("HG_Sigma_Ratio2",*m_HG_ratio2_S,"HG_signal_cor2_s[8][4][64][48][2]/D");
  t->Branch("Entries",*m_entries,"n_LASER_entries[8][4][64][48][2]/I");  

     
  // Fill with current values (i.e. tree will have only one entry for this whole run)
  t->Fill();
  t->Write();
  
  return StatusCode::SUCCESS;  
}

StatusCode TileLaserLinearityCalibTool::finalize()
{
  ATH_MSG_INFO ( "finalize()" );
  return StatusCode::SUCCESS;  
}  


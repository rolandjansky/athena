/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileCalibAlgs/TileLaserDefaultCalibTool.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileLaserObject.h"
#include "TileConditions/TileCablingService.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileMonitoring/ITileStuckBitsProbsTool.h"


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include "Identifier/HWIdentifier.h"
#include "StoreGate/ReadHandle.h"


#include "TFile.h"
#include "TTree.h"
#include <cmath>
#include <iostream>

/****************************************************/
/* TileLaserDefaultCalibTool.cxx     June 2017      */
/*                                                  */
/* Henric Wilkens, following work from Seb Viret and 
   Vincent Giangiobbe    

   Giulia Di Gregorio, following work from Henric 
                                                    */
/****************************************************/

TileLaserDefaultCalibTool::TileLaserDefaultCalibTool(const std::string& type, const std::string& name,const IInterface* pParent):
  AthAlgTool(type, name, pParent),
  m_pisaMethod2(false),
  m_isLaserCalib(false),
  m_tileHWID(nullptr),
  m_cabling(nullptr),
  m_tileToolEmscale("TileCondToolEmscale"),
  m_tileBadChanTool("TileBadChanTool"),
  m_stuckBitsProbs(""),
  m_toolRunNo(0),
  m_ADC_problem(0),
  m_las_filter(0),
  m_las_requ_amp(0),
  m_hrate(0),
  m_flow(0),
  m_head_temp(0),
  m_las_time(0),
  m_PMT(),
  m_PMT_S(),
  m_diode(),
  m_diode_S(),
  m_diode_Ped(),
  m_diode_Alpha(),
  m_diode_SPed(),
  m_diode_SAlpha(),
  m_rs_diode_signal(),
  m_rs_PMT_signal(),
  m_PMT1_ADC_prev(),
  m_PMT2_ADC_prev(),
  m_LASERII(0),
  m_evtNr(0)
{
  declareInterface<ITileCalibTool>( this );
  declareProperty("toolNtuple", m_toolNtuple="h3000");
  declareProperty("pisaMethod2", m_pisaMethod2=true);
  declareProperty("StuckBitsProbsTool", m_stuckBitsProbs);
  declareProperty("TileDQstatus", m_dqStatusKey = "TileDQstatus");
  
  //creating multi-dim arrays on the heap and initialize all elements to zeros
  m_ratio_LASERII = new float[NDIODES][NGAINS][NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_ratio_S_LASERII = new float[NDIODES][NGAINS][NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_ratio_LASERII_good = new float[NDIODES][NGAINS][NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_ratio_S_LASERII_good = new float[NDIODES][NGAINS][NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();

  m_rs_ratio_LASERII = new RunningStat*[NDIODES][NGAINS][NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_rs_ratio_LASERII_good = new RunningStat*[NDIODES][NGAINS][NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();

  m_PMT_LASERII = new float[NPMTS][NGAINS]();
  m_PMT_S_LASERII = new float[NPMTS][NGAINS]();
  m_rs_PMT_signal_LASERII = new RunningStat*[NPMTS][NGAINS]();

  m_diode_LASERII = new float[NDIODES][NGAINS]();
  m_diode_S_LASERII = new float[NDIODES][NGAINS]();
  m_entries_diode_LASERII = new int[NDIODES][NGAINS]();

  m_rs_diode_signal_LASERII = new RunningStat*[NDIODES][NGAINS]();

  m_diode_Ped_LASERII = new float[NDIODES+1][NGAINS]();
  m_diode_Ped_S_LASERII = new float[NDIODES+1][NGAINS]();
  m_diode_Alpha_LASERII = new float[NDIODES+1][NGAINS]();
  m_diode_Alpha_S_LASERII = new float[NDIODES+1][NGAINS]();
  m_diode_Led_LASERII = new float[NDIODES+1][NGAINS]();
  m_diode_Led_S_LASERII = new float[NDIODES+1][NGAINS]();
  m_PMT_Ped_LASERII = new float[NPMTS][NGAINS]();
  m_PMT_Ped_S_LASERII = new float[NPMTS][NGAINS]();

  m_ratio = new float[NDIODES_LASER1][NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_ratio_S = new float[NDIODES_LASER1][NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_ratio_good = new float[NDIODES_LASER1][NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_ratio_good_S = new float[NDIODES_LASER1][NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_pmt_ratios = new float[NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_pmt_S_ratios = new float[NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_rs_ratio = new RunningStat*[NDIODES_LASER1][NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_rs_ratio_good = new RunningStat*[NDIODES_LASER1][NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_rs_pmt_ratios = new RunningStat*[NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
 
  m_meantime = new float[NPARTITIONS][NGAINS]();
  m_time = new float[NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_time_S = new float[NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();

  m_mean = new float[NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_mean_S = new float[NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_raw_mean = new float[NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_raw_mean_S = new float[NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_entries = new int[NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();   
  m_kappa = new float[NPARTITIONS][NDRAWERS][NFIBERS][NGAINS]();
  m_mean_slice = new float[NPARTITIONS][NDRAWERS][NCHANNELS][NSLICES][NGAINS]();
  m_variance_slice = new float[NPARTITIONS][NDRAWERS][NCHANNELS][NSLICES][NGAINS]();
  m_status = new short[NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_HV = new float[NPARTITIONS][NDRAWERS][NCHANNELS]();
  m_HVSet = new float[NPARTITIONS][NDRAWERS][NCHANNELS]();

  m_rs_meantime = new RunningStat*[NPARTITIONS][NGAINS]();
  m_rs_time = new RunningStat*[NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_rs_signal = new RunningStat*[NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_rs_raw_signal = new RunningStat*[NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS]();
  m_rs_reducedKappa = new RunningStat*[NPARTITIONS][NDRAWERS][NCOUPLES-1][NCOUPLES][NGAINS][NFIBERS]();

} // TileLaserDefaultCalibTool::TileLaserDefaultCalibTool

TileLaserDefaultCalibTool::~TileLaserDefaultCalibTool()
{
  delete[] m_ratio_LASERII;
  delete[] m_ratio_S_LASERII;
  delete[] m_ratio_LASERII_good;
  delete[] m_ratio_S_LASERII_good;
  delete[] m_rs_ratio_LASERII;
  delete[] m_rs_ratio_LASERII_good;
  delete[] m_PMT_LASERII;
  delete[] m_PMT_S_LASERII;
  delete[] m_rs_PMT_signal_LASERII;
  delete[] m_diode_LASERII;
  delete[] m_diode_S_LASERII;
  delete[] m_entries_diode_LASERII;
  delete[] m_rs_diode_signal_LASERII;
  delete[] m_diode_Ped_LASERII;
  delete[] m_diode_Ped_S_LASERII;
  delete[] m_diode_Alpha_LASERII;
  delete[] m_diode_Alpha_S_LASERII;
  delete[] m_diode_Led_LASERII;
  delete[] m_diode_Led_S_LASERII;
  delete[] m_PMT_Ped_LASERII;
  delete[] m_PMT_Ped_S_LASERII;
  delete[] m_ratio;
  delete[] m_ratio_S;
  delete[] m_ratio_good;
  delete[] m_ratio_good_S;
  delete[] m_pmt_ratios;
  delete[] m_pmt_S_ratios;
  delete[] m_rs_ratio;
  delete[] m_rs_ratio_good;
  delete[] m_rs_pmt_ratios;
  delete[] m_meantime;
  delete[] m_time;
  delete[] m_time_S;
  delete[] m_mean;
  delete[] m_mean_S;
  delete[] m_raw_mean;
  delete[] m_raw_mean_S;
  delete[] m_entries;   
  delete[] m_kappa;
  delete[] m_mean_slice;
  delete[] m_variance_slice;
  delete[] m_status;
  delete[] m_HV;
  delete[] m_HVSet;
  delete[] m_rs_meantime;
  delete[] m_rs_time;
  delete[] m_rs_signal;
  delete[] m_rs_raw_signal;
  delete[] m_rs_reducedKappa;
}


StatusCode TileLaserDefaultCalibTool::initialize(){
  // Reset local parameters and load necessary service
  ATH_MSG_DEBUG ( "initialize() TileLaserDefaultCalibTool" );
  m_evtNr        = 0;
  m_toolRunNo    = 0;
  m_ADC_problem  = 0;
  m_las_filter   = 0;
  m_las_requ_amp = 0;
  m_hrate        = 0;
  m_flow         = 0;
  m_head_temp    = 0;
  m_las_time     = 0;
  m_PMT1_ADC_prev = 0;
  m_PMT2_ADC_prev = 0;

  
  // Loop over monitoring pmts laserii
  for ( int diode=0; diode<NDIODES; ++diode ) {
    for ( int gain=0; gain<NGAINS; gain++ ) {
      m_diode_LASERII[diode][gain] = 0;      // diode signal values
      m_diode_S_LASERII[diode][gain] = 0;// Sigma of signal values 
      m_entries_diode_LASERII[diode][gain] = 0;
      m_rs_diode_signal_LASERII[diode][gain] = new RunningStat();
    }
  }

  for ( int diode=0; diode<NDIODES+1; ++diode ) {
    for ( int gain=0; gain<NGAINS; gain++ ) {

      m_diode_Ped_LASERII[diode][gain] = 0;  // Corresponding pedestal values
      m_diode_Ped_S_LASERII[diode][gain] = 0;// Sigma of pedestal values    
      m_diode_Alpha_LASERII[diode][gain] = 0;  // Corresponding Alpha values
      m_diode_Alpha_S_LASERII[diode][gain] = 0;// Sigma of Alpha values    
      m_diode_Led_LASERII[diode][gain] = 0;  // Corresponding LED values
      m_diode_Led_S_LASERII[diode][gain] = 0;// Sigma of LED values    

    }
  }

  for(int pmt=0;pmt<NPMTS;++pmt){
    for ( int gain=0; gain<NGAINS; gain++ ) {
      m_PMT_LASERII[pmt][gain]      = 0;
      m_PMT_S_LASERII[pmt][gain]    = 0;
      m_PMT_Ped_LASERII[pmt][gain] = 0;  // Corresponding pedestal values
      m_PMT_Ped_S_LASERII[pmt][gain] = 0;// Sigma of pedestal values    
      
      m_rs_PMT_signal_LASERII[pmt][gain] = new RunningStat();
    }
    // LASERI
    m_PMT[pmt]      = 0;
    m_PMT_S[pmt]    = 0;
    m_rs_PMT_signal[pmt] = new RunningStat();
  } // FOR
  
  // LASERI
  for(int d=0; d<NDIODES_LASER1; ++d){
    m_diode[d]       = 0;
    m_diode_S[d]     = 0;
    m_diode_Ped[d]   = 0;
    m_diode_Alpha[d] = 0;
    m_diode_SPed[d]  = 0;
    m_diode_SAlpha[d]= 0;
    m_rs_diode_signal[d]  = new RunningStat();
  } // FOR
  
  for ( int part=0; part<NPARTITIONS; ++part ) {
    for ( int gain=0; gain<NGAINS; ++gain ) {
      m_rs_meantime[part][gain] = new RunningStat(); 
      m_meantime[part][gain] = 0.;
    } 
    
    for ( int drawer=0; drawer<NDRAWERS; ++drawer ) {
      for ( int gain=0; gain<NGAINS; ++gain ) {
        for (int fiber=0; fiber<NFIBERS; ++fiber){
	   m_kappa[part][drawer][fiber][gain]   = 0;
	   for (int pmt1=0; pmt1<NCOUPLES-1; ++pmt1){
	     for (int pmt2=pmt1+1; pmt2<NCOUPLES; ++pmt2){
	       m_rs_reducedKappa[part][drawer][pmt1][pmt2][gain][fiber] = new RunningStat();
	     }
	   }	      
	}
	
        for ( int channel=0; channel<NCHANNELS; ++channel ) {
          m_rs_time[part][drawer][channel][gain]       = new RunningStat();
          m_rs_signal[part][drawer][channel][gain]     = new RunningStat();
          m_rs_raw_signal[part][drawer][channel][gain] = new RunningStat();

          m_time[part][drawer][channel][gain]    = 0;
          m_time_S[part][drawer][channel][gain]  = 0;

          m_mean[part][drawer][channel][gain]    = 0;
          m_mean_S[part][drawer][channel][gain]  = 0;
          m_raw_mean[part][drawer][channel][gain]    = 0;
          m_raw_mean_S[part][drawer][channel][gain]  = 0;

          for ( int iSlice=0; iSlice<NSLICES; ++iSlice ) {
            m_mean_slice[part][drawer][channel][iSlice][gain] = 0;
            m_variance_slice[part][drawer][channel][iSlice][gain] = 0;
          } // FOR
          
          // LASERII
          for(int diode=0; diode<NDIODES; ++diode){
	    for (int diode_gain=0; diode_gain<NGAINS; diode_gain++) {
	      m_rs_ratio_LASERII[diode][diode_gain][part][drawer][channel][gain]  = new RunningStat();
	      m_rs_ratio_LASERII_good[diode][diode_gain][part][drawer][channel][gain]  = new RunningStat();
	    }
          } // FOR
	  
	  m_rs_pmt_ratios[part][drawer][channel][gain]  = new RunningStat();
	  m_pmt_ratios[part][drawer][channel][gain]   = 0;
	  m_pmt_S_ratios[part][drawer][channel][gain] = 0;
	    
          // LASERI
          for(int d=0; d<NDIODES_LASER1; ++d){
            m_rs_ratio[d][part][drawer][channel][gain]  = new RunningStat();
            m_ratio[d][part][drawer][channel][gain]   = 0;
            m_ratio_S[d][part][drawer][channel][gain] = 0;
            m_rs_ratio_good[d][part][drawer][channel][gain]  = new RunningStat();
            m_ratio_good[d][part][drawer][channel][gain]   = 0;
            m_ratio_good_S[d][part][drawer][channel][gain] = 0;
          } // FOR
          
          m_entries[part][drawer][channel][gain] = 0;
          
          m_HV[part][drawer][channel] = 0.; 
          m_HVSet[part][drawer][channel] = 0.; 

        } // channel loop
      } // gain loop
    } // drawer loop
  } // partition loop
  
  ATH_CHECK( detStore()->retrieve(m_tileHWID) );
  ATH_CHECK( m_tileToolEmscale.retrieve() );
  ATH_CHECK( m_tileBadChanTool.retrieve() );
  
  ATH_CHECK( m_rawChannelContainerKey.initialize() );
  ATH_CHECK( m_laserContainerKey.initialize() );

  ATH_CHECK( m_tileDCS.retrieve() );

  ATH_CHECK( m_dqStatusKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode TileLaserDefaultCalibTool::initNtuple(int runNumber, int runType, TFile * rootFile){
  ATH_MSG_INFO ( "initialize(" << runNumber << "," << runType << "," << rootFile << ")" );
  return StatusCode::SUCCESS;
} // INITNTUPLE


StatusCode TileLaserDefaultCalibTool::execute(){
  const EventContext& ctx = Gaudi::Hive::currentContext();
  const TileDQstatus* dqStatus = SG::makeHandle (m_dqStatusKey, ctx).get();

  const char* text[NGAINS] = {"LG DIODE ","HG DIODE "}; 
  ++m_evtNr;   // Increment event number
  ATH_MSG_DEBUG ( "Event counter: " << m_evtNr );
  
  // Store laser object and rawchannel information into maps
  ATH_MSG_DEBUG ( "execute() TileLaserDefaultCalibTool" );
  
  SG::ReadHandle<TileRawChannelContainer> rawCnt(m_rawChannelContainerKey);
  SG::ReadHandle<TileLaserObject> laserObj(m_laserContainerKey);
  
  ATH_CHECK( rawCnt.isValid() );
  ATH_CHECK( laserObj.isValid() );
  
  
  m_LASERII = laserObj->isLASERII();

  if(m_LASERII) ATH_MSG_DEBUG ( "LaserII version is " << laserObj->getVersion() << " DAQ Type = " << laserObj->getDaqType() );
  else          ATH_MSG_DEBUG ( "LaserI version is "  << laserObj->getVersion() << " DAQ Type = " << laserObj->getDaqType() );
  
  const uint32_t *cispar = dqStatus->cispar();
  
  m_las_time = static_cast<double>(cispar[10])+static_cast<double>(cispar[11])/1000000;
  
  // Retrieve laser information
  if(laserObj->getDiodeCurrOrd() == 0 || laserObj->getFiltNumber() == 0 || laserObj->getQDCTimeout()){
    ATH_MSG_ERROR ( "No filter number or diode current: wheel moving or QDC timeout" );
    return StatusCode::SUCCESS; // This is expected for some events
  } // IF
  
  float normalization[NDIODES][NGAINS];
  float pmt_values[NPARTITIONS][NDRAWERS][NCHANNELS][NGAINS];
  for (int part=0; part<NPARTITIONS; part++){
    for (int draw=0; draw<NDRAWERS; draw++){
      for (int chan=0; chan<NCHANNELS; chan++){
	for (int gain=0; gain<NGAINS; gain++) {
	  pmt_values[part][draw][chan][gain]=0.;
	}
      }
    }
  }

  static int first = 1; // Do only once
  if ( first ) {
    for ( int part=0; part<NPARTITIONS; ++part ) {
      int ros = part+1;
      for ( int drawer=0; drawer<NDRAWERS; ++drawer ) {
	unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros,drawer);
	for ( int channel=0; channel<NCHANNELS; ++channel ) {
	  if ( dqStatus->isChEmpty(ros,drawer,channel) ) {  // Check whether channel is connected
	    continue;
	  }
	  for ( int gain=0; gain<NGAINS; ++gain ) {
	    short status = m_tileBadChanTool->encodeStatus(m_tileBadChanTool->getAdcStatus(drawerIdx, channel, gain)) ;
	    /* --- Status of the channel in DB
	       
	       0 = isGood()
	       1 = isNoisy() : Large HF noise; Correlated noise; Large LF noise;
	       0x2 = isAffected() : ?
	       0x4 = isBad()   : ADC masked (unspecified); ADC dead; Very large HF noise; No data; 
	                         Wrong DSP configuration; Severe stuck bit; Severe data corruption; 
	                         Channel masked (unspecified); No PMT connected;No HV; Wrong HV;
	       0x8 = Unknown ?!

	       0x10 = bad ADC, masked on the fly
	       
	       Later we will add the transiant quality flags ---- */
	    if (status)
	      m_status[part][drawer][channel][gain]  = 1 << (status-1);	 
	  }
	}
      }
    }
    first = 0; 
  }
  
  if ( m_LASERII ) {  // LASERII
    // We need to have pedestals
    static int have_pedestals = 0; 
    static int have_alpha = 0; 
    static int have_led = 0; 
    static int have_linearity = 0; 
    static int have_laser = 0;

    if ( ! (have_pedestals && have_alpha && have_led ) ) {
      ATH_MSG_DEBUG ( "Calib type " << laserObj->getCalibType() << have_pedestals << have_alpha <<have_led<<have_linearity<<have_laser);
      switch ( laserObj->getCalibType()  ) {

      case TileLaserObject::calibType::Pedestal0: 	
	if ( laserObj->isSet(0, 0, TileLaserObject::calibType::Pedestal0) ) { // Pedestal are set
	  for ( int diode=0; diode<NDIODES; ++diode ) {
	    for ( int gain=0; gain<NGAINS; gain++ ) {
	      m_diode_Ped_LASERII[diode][gain]   = laserObj->getMean(diode, gain, TileLaserObject::calibType::Pedestal0);
	      m_diode_Ped_S_LASERII[diode][gain] = laserObj->getSigma(diode, gain, TileLaserObject::calibType::Pedestal0);	     
	    }	   
	  }

	  for (int pmt=0; pmt<NPMTS; pmt++ ) {
	    for ( int gain=0; gain<NGAINS; gain++ ) {
	      m_PMT_Ped_LASERII[pmt][gain]   = laserObj->getMean(pmt, gain, TileLaserObject::calibType::Pedestal0);
	      m_PMT_Ped_S_LASERII[pmt][gain] = laserObj->getSigma(pmt, gain, TileLaserObject::calibType::Pedestal0);	
	    }
	  }
	  // PHOCAL 
	  for ( int gain=0; gain<NGAINS; gain++ ) {
	    m_diode_Ped_LASERII[NDIODES][gain]   = laserObj->getMean(13, gain, TileLaserObject::calibType::Pedestal0);
	    m_diode_Ped_S_LASERII[NDIODES][gain] = laserObj->getSigma(13, gain, TileLaserObject::calibType::Pedestal0);	     
	  }
	  have_pedestals = 1;
	} 
	break; 
	
      case TileLaserObject::calibType::Pedestal1: 
	break;

      case TileLaserObject::calibType::Alpha: 	
	if ( laserObj->isSet(0, 0, TileLaserObject::calibType::Alpha) ) { // Alpha are set
	  for ( int diode=0; diode<NDIODES; ++diode ) {
	    for ( int gain=0; gain<NGAINS; gain++ ) {
	      m_diode_Alpha_LASERII[diode][gain]   = laserObj->getMean(diode, gain, TileLaserObject::calibType::Alpha);
	      m_diode_Alpha_S_LASERII[diode][gain] = laserObj->getSigma(diode, gain, TileLaserObject::calibType::Alpha);	     
	    }	   
	  }
	  // PHOCAL
	  for ( int gain=0; gain<NGAINS; gain++ ) { 
	    m_diode_Alpha_LASERII[NDIODES][gain]   = laserObj->getMean(13, gain, TileLaserObject::calibType::Alpha);
	    m_diode_Alpha_S_LASERII[NDIODES][gain] = laserObj->getSigma(13, gain, TileLaserObject::calibType::Alpha);	     
	  }
	  have_alpha = 1;
	} 
	break; 
		
      case TileLaserObject::calibType::LED: 	
	if ( laserObj->isSet(0, 0, TileLaserObject::calibType::LED) ) { // LED are set
	  for ( int diode=0; diode<NDIODES; ++diode ) {
	    for ( int gain=0; gain<NGAINS; gain++ ) {
	      m_diode_Led_LASERII[diode][gain]   = laserObj->getMean(diode, gain, TileLaserObject::calibType::LED);
	      m_diode_Led_S_LASERII[diode][gain] = laserObj->getSigma(diode, gain, TileLaserObject::calibType::LED);	     
	    }	  
	    //PHOCAL
	    for ( int gain=0; gain<NGAINS; gain++ ) {
	      m_diode_Led_LASERII[NDIODES][gain]   = laserObj->getMean(13, gain, TileLaserObject::calibType::LED);
	      m_diode_Led_S_LASERII[NDIODES][gain] = laserObj->getSigma(13, gain, TileLaserObject::calibType::LED);	     
	    }
	  }
	  have_led = 1;
	} 
	break; 	

      default:
	ATH_MSG_ERROR ("Got an invalid calibration type from LaserObject" ); 
	return StatusCode::SUCCESS; // We can't do anything yet
	break;
      }
    }
    
    if (! have_pedestals) return StatusCode::SUCCESS; // We can't do anything yet

    // Now we have pedestals, start accumulating the Diode responses
    for ( int diode=0; diode<NDIODES; ++diode ) {
      for ( int gain=0; gain<NGAINS; gain++ ) {	
	ATH_MSG_DEBUG ( text[gain] << diode << " PED= " 
			<< m_diode_Ped_LASERII[diode][gain]  << "+/-" << m_diode_Ped_S_LASERII[diode][gain] 
			<< " ( " << laserObj->isSet(diode, gain, 0) << " ) " 
			);
	
	m_rs_diode_signal_LASERII[diode][gain]->Push( laserObj->getDiodeADC(diode,gain) - 
						      m_diode_Ped_LASERII[diode][gain]);	
	normalization[diode][gain] = ((float)laserObj->getDiodeADC(diode,gain)-m_diode_Ped_LASERII[diode][gain]);
	ATH_MSG_DEBUG ( text[gain]  << diode << " Signal=" << normalization[diode][gain] << " " << m_rs_diode_signal_LASERII[diode][gain]->Mean() << " " << laserObj->getDiodeADC(diode,gain) << " Ped="<< m_diode_Ped_LASERII[diode][gain] );
      }
    }    
    
    // And also the PMT responses
    for (int pmt=0; pmt<NPMTS; pmt++ ) {
      for ( int gain=0; gain<NGAINS; gain++ ) {
	m_rs_PMT_signal_LASERII[pmt][gain]->Push(laserObj->getPMADC(pmt,gain)-m_PMT_Ped_LASERII[pmt][gain]);
      }
    }    
    
  } else {  // laserI

    for(int d=0; d<NDIODES_LASER1; ++d){
      m_rs_diode_signal[d]->Push(laserObj->getDiodeADC(d,0)-laserObj->getDiodePedestal(d,0));
    } 
  
    for(int pmt=0; pmt<NPMTS; pmt++){
      m_rs_PMT_signal[pmt]->Push(laserObj->getPMADC(pmt,0)-laserObj->getPMPedestal(pmt,0));    
    } 
  
    // Check that adc information has been sent
    if(laserObj->getPMADC(0,0) == m_PMT1_ADC_prev &&
       laserObj->getPMADC(1,0) == m_PMT2_ADC_prev ){
      m_ADC_problem = 1;
      ATH_MSG_WARNING ( "There is perhaps an ADC problem with this event" );
    } // IF
    
    m_PMT1_ADC_prev = laserObj->getPMADC(0,0); // LG
    m_PMT2_ADC_prev = laserObj->getPMADC(1,0); // LG
    
    for(int d=0; d<NDIODES_LASER1; ++d){
      m_diode_Ped[d]     = laserObj->getDiodePedestal(d,0);
      m_diode_SPed[d]    = laserObj->getDiodeSigmaPedestal(d,0);
      m_diode_Alpha[d]   = laserObj->getAlpha(d,0);
      m_diode_SAlpha[d]  = laserObj->getSigmaAlpha(d,0);
    }
    
  } 
  
  // Next parameters are constants, don't need to update them more than once
  if(m_las_filter == 0){
    m_las_filter   = laserObj->getFiltNumber();
    m_hrate        = laserObj->getHumidity();
    m_flow         = laserObj->getGasFlux();
    m_head_temp    = laserObj->getPumpDiodeTemp();
    m_las_requ_amp = laserObj->getDiodeCurrOrd();
  } // IF

  double Q1Q2[NCOUPLES-1][NCOUPLES];
  for (int pmt1=0; pmt1<NCOUPLES-1; ++pmt1){
    for (int pmt2=0; pmt2<NCOUPLES; ++pmt2){
      Q1Q2[pmt1][pmt2]=0.;
    }
  }
  //int currentDrawer=0;
  
  RunningStat* avg_time[NPARTITIONS][NGAINS];
  for(int part=0; part<NPARTITIONS; part++){
    for(int gain=0;gain<NGAINS;++gain){
      avg_time[part][gain] = new RunningStat();
    }
  }
  /*  
    Iterator over rawchannelcontainer
  */  
  TileRawChannelUnit::UNIT RChUnit = rawCnt->get_unit();
  TileRawChannelContainer::const_iterator itColl;
  TileRawChannelContainer::const_iterator itCollEnd = rawCnt->end();
  
  for ( itColl=rawCnt->begin(); itColl != itCollEnd; ++itColl ) {  // Loop over tilerawchannelcollections to get avg time per partition
    HWIdentifier drawer_id = m_tileHWID->drawer_id((*itColl)->identify());
    int part = m_tileHWID->ros(drawer_id)-1;     // LBA=0 LBC=1 EBA=2 EBC=3
    // Loop over tilerawchannels in collection
    for ( TileRawChannelCollection::const_iterator it = (*itColl)->begin(); it != (*itColl)->end(); ++it ) {
      HWIdentifier hwid=(*it)->adc_HWID();  
      int gain   = m_tileHWID->adc(hwid);      // low=0 high=1    
      float ofctime = (*it)->time();
      if(ofctime!=0.0 and std::abs(ofctime-15.0)<30.)
        avg_time[part][gain]->Push(ofctime);
    } 
  } // Now we have the average time per partition for this event
  
  for ( itColl=rawCnt->begin(); itColl != itCollEnd; ++itColl ) {   // Loop over tilerawchannelcollections 
    HWIdentifier drawer_id = m_tileHWID->drawer_id((*itColl)->identify());
    int ros = m_tileHWID->ros(drawer_id);     // LBA=0 LBC=1 EBA=2 EBC=3
    int part = ros-1; 
    int drawer = m_tileHWID->drawer(drawer_id); // 0 to 63
    unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros,drawer);
    
    // Loop over tilerawchannels in collection
    for ( TileRawChannelCollection::const_iterator it = (*itColl)->begin(); it != (*itColl)->end(); ++it ) {
      // Get adchash
      HWIdentifier hwid = (*it)->adc_HWID();
      int chan          = m_tileHWID->channel(hwid);  // 0 to 47 channel
      int gain          = m_tileHWID->adc(hwid);      // low=0 high=1
      float amp         = (*it)->amplitude();
      float ofctime     = (*it)->time();
      float ped = (*it)->pedestal();
      bool is_good = true;
      
      if(ofctime!=0.0) ofctime -= avg_time[part][gain]->Mean();
      
      if ( dqStatus->isChEmpty(ros,drawer,chan) ) {  // Check whether channel is connected
        m_status[part][drawer][chan][0] = -1;
        m_status[part][drawer][chan][1] = -1;
        continue; // Nothing to be seen here
      } 
            
      if ( !dqStatus->isAdcDQgood(ros,drawer,chan,gain) ) { // Masked on the fly
        m_status[part][drawer][chan][gain] |= 0x10;
	is_good = false;
      }

      int problem = int(ped + 500.)/10000;
      
      switch (problem) {
      case 1: // Underflow
	m_status[part][drawer][chan][gain] |= 0x100;
	is_good = false;
	break;
      case 2: // Overflow
	m_status[part][drawer][chan][gain] |= 0x200;
	is_good = false;
	break;
      case 3: // Under and Overflow
	m_status[part][drawer][chan][gain] |= 0x400;
	is_good = false;
	break;
      case 4: // Constant signal
	m_status[part][drawer][chan][gain] |= 0x800;
	is_good = false;
	break;
      case 8: // Underflow in all channels
	m_status[part][drawer][chan][gain] |= 0x1000;
	is_good = false;
	break;
      case 9: // Overflow in all channels
	m_status[part][drawer][chan][gain] |= 0x2000;
	is_good = false;
	break;
      }

      float ampInPicoCoulombs = m_tileToolEmscale->channelCalib(drawerIdx, chan, gain, amp, RChUnit, TileRawChannelUnit::PicoCoulombs);
      
      m_rs_time[part][drawer][chan][gain]->Push(ofctime);
      m_rs_signal[part][drawer][chan][gain]->Push(ampInPicoCoulombs);
      m_rs_raw_signal[part][drawer][chan][gain]->Push(amp);
      
      if ( m_LASERII ) {
        for(int diode=0; diode<NDIODES; ++diode){
	  for ( int diode_gain=0; diode_gain<NGAINS; diode_gain++ ) {  // MONITORING DIODES	      
	      if ( normalization[diode][diode_gain]!=0. ){
		if (is_good) m_rs_ratio_LASERII_good[diode][diode_gain][part][drawer][chan][gain]->Push( ampInPicoCoulombs/normalization[diode][diode_gain] );
		m_rs_ratio_LASERII[diode][diode_gain][part][drawer][chan][gain]->Push( ampInPicoCoulombs/normalization[diode][diode_gain] );
	      }
          } // Diode Gains
	} // Diodes
	if (is_good) m_entries[part][drawer][chan][gain]++;
      } else {
        for(int i=0; i<NDIODES_LASER1; ++i){
          if((laserObj->getDiodeADC(i,0)-laserObj->getDiodePedestal(i,0)) != 0) {
	    if (is_good) m_rs_ratio_good[i][part][drawer][chan][gain]->Push(ampInPicoCoulombs/(laserObj->getDiodeADC(i,0)-laserObj->getDiodePedestal(i,0)));
	    m_rs_ratio[i][part][drawer][chan][gain]->Push(ampInPicoCoulombs/(laserObj->getDiodeADC(i,0)-laserObj->getDiodePedestal(i,0)));
	  }
        } // FOR
	if (is_good) m_entries[part][drawer][chan][gain]++;
      } // ELSE
      
      if ((is_good) && (!(m_status[part][drawer][chan][gain]&0x4)) )
	pmt_values[part][drawer][chan][gain]=ampInPicoCoulombs;
      else
	pmt_values[part][drawer][chan][gain]=0.;
	
    } // End of the loop over the TileRawChannelCollection
  } // End of the loop over the TileRawChannelContainer
  
  int chan1;
  int chan2;
  for (int part=0; part<NPARTITIONS; part++){
    for (int drawer=0; drawer<NDRAWERS; ++drawer){
      for(int pmt1=0; pmt1<NCOUPLES-1; ++pmt1){
	  for (int pmt2=0; pmt2<NCOUPLES; ++pmt2){
	    Q1Q2[pmt1][pmt2]=0.;
	  }
	}
      //We compute <q1.q2> for odd PMTs
      for (int gain=0; gain<NGAINS; ++gain){
	for (int pmt1=0; pmt1<NCOUPLES-1; ++pmt1){
	  chan1 = int(getCoupleOfPMT(part, pmt1).second);
	  if (chan1==-1) continue;
	  
	  for (int pmt2=pmt1+1; pmt2<NCOUPLES; ++pmt2){
	    chan2 = int(getCoupleOfPMT(part, pmt2).second);
	    if (chan2==-1) continue;
	
	    Q1Q2[pmt1][pmt2]= pmt_values[part][drawer][chan1][gain]*pmt_values[part][drawer][chan2][gain];
	    if (Q1Q2[pmt1][pmt2]>0.){
	      m_rs_reducedKappa[part][drawer][pmt1][pmt2][gain][1]->Push(Q1Q2[pmt1][pmt2]);
	    }//IF
	  }//pmt2
	}//pmt1
	
        //We compute <q1.q2> for even PMTs
	for (int pmt1=0; pmt1<NCOUPLES-1; ++pmt1){
	  chan1 = int(getCoupleOfPMT(part, pmt1).first);
	  if (chan1==-1) continue;
	  for (int pmt2=pmt1+1; pmt2<NCOUPLES; ++pmt2){
	    chan2=int(getCoupleOfPMT(part, pmt2).first);
	    if (chan2==-1)
	      continue;
	    Q1Q2[pmt1][pmt2]= pmt_values[part][drawer][chan1][gain]*pmt_values[part][drawer][chan2][gain];
	    if (Q1Q2[pmt1][pmt2]>0.){
	      m_rs_reducedKappa[part][drawer][pmt1][pmt2][gain][0]->Push(Q1Q2[pmt1][pmt2]);
	    }//IF
	  }//pmt2
	}//pmt1
	
	
      }//gain
    }//drawer
  }//part
	    

  for(int part=0; part<NPARTITIONS; part++){
    for(int drawer=0; drawer<NDRAWERS; ++drawer){
      for (int chan=0; chan<NCHANNELS; ++chan){
	for(int gain=0;gain<NGAINS;++gain){
	  int chanref = 0;
	  if(part<2){
	    chanref = 24 + chan%2;
	  }
	  else{
	    switch (chan) {
	    case 31:
	    case 39:
	    case 41:
	      chanref = 38;
	      break;
	    case 32:
	    case 36:
	    case 40:
	      chanref = 37;
	      break;
	    default:
	      chanref = 38 - chan%2;
	    }
	  }
	  if(pmt_values[part][drawer][chanref][gain]>0.001){
	    m_rs_pmt_ratios[part][drawer][chan][gain]->Push(pmt_values[part][drawer][chan][gain]/pmt_values[part][drawer][chanref][gain]);
	  }

	}
      }
    }
  }
  
  for (int part=0; part<NPARTITIONS; part++){
    for(int gain=0;gain<NGAINS;++gain){
      m_rs_meantime[part][gain]->Push(avg_time[part][gain]->Mean());
      delete(avg_time[part][gain]);
    } // FOR
  } // FOR

  return StatusCode::SUCCESS;
} // EXECUTE


StatusCode TileLaserDefaultCalibTool::finalizeCalculations(){
  // COMPUTE CALIBRATION COEFFICIENT AT THE END OF THE EVENT LOOP
  ATH_MSG_INFO ( "finalizeCalculations()" );

  // Loop over monitors
  if ( m_LASERII ) { // LASERII  
    for(int pmt=0; pmt<NPMTS; pmt++){
      for ( int gain=0; gain<NGAINS; ++gain ) {
	m_PMT_LASERII[pmt][gain]         = m_rs_PMT_signal_LASERII[pmt][gain]->Mean();
	m_PMT_S_LASERII[pmt][gain]       = m_rs_PMT_signal_LASERII[pmt][gain]->StandardDeviation();
      }
    }
    for(int d=0; d<NDIODES; ++d){
      for ( int gain=0; gain<NGAINS; ++gain ) {	 
	m_diode_LASERII[d][gain]       = m_rs_diode_signal_LASERII[d][gain]->Mean();
	m_diode_S_LASERII[d][gain]     = m_rs_diode_signal_LASERII[d][gain]->StandardDeviation();
       	m_entries_diode_LASERII[d][gain] = m_rs_diode_signal_LASERII[d][gain]->NumDataValues();
      }
    }
  } else {           // LASERI
    for(int pmt=0; pmt<NPMTS; pmt++){
      m_PMT[pmt]         = m_rs_PMT_signal[pmt]->Mean();
      m_PMT_S[pmt]       = m_rs_PMT_signal[pmt]->StandardDeviation();
    } // FOR
     
     for(int d=0; d<NDIODES_LASER1; ++d){
       m_diode[d]       = m_rs_diode_signal[d]->Mean();
       m_diode_S[d]     = m_rs_diode_signal[d]->StandardDeviation();
     } // FOR
   }

  // Loop over barrels, modules and gains
  for ( int partition=0; partition<NPARTITIONS; partition++ ) {
    for ( int gain=0; gain<NGAINS; ++gain ) {
      m_meantime[partition][gain] = m_rs_meantime[partition][gain]->Mean();
    }
    
    for ( int drawer=0; drawer<NDRAWERS; ++drawer ) {
      for ( int gain=0; gain<NGAINS; ++gain ) {
	/*  Compute the average kappa correction factor for all event and odd pmts
	    Kappa is by definition: cov(q1,q2)/<q1>*<q2> average on all couples of 
	    pmts q1, q2 receiving light from the same clear fiber (only 2 independent 
	    kappa for each module)    */
        int nCouplesEven=0, nCouplesOdd=0;

	int chan1;
	int chan2;
	double q1;
	double q2;

	//We evaluate kappa value for odd PMTs
	for (int pmt1=0; pmt1<NCOUPLES-1; ++pmt1){
	  for (int pmt2=pmt1+1; pmt2<NCOUPLES; ++pmt2){
	    chan1 = getCoupleOfPMT(partition, pmt1).second;
	    chan2 = getCoupleOfPMT(partition, pmt2).second;
	    q1 = m_rs_signal[partition][drawer][chan1][gain]->Mean();
	    q2 = m_rs_signal[partition][drawer][chan2][gain]->Mean();

	    if (q1*q2<=0){
	      continue;
	    }
	    if (m_rs_reducedKappa[partition][drawer][pmt1][pmt2][gain][1]->Mean() < q1*q2){
	      continue;
	    }
	    if ((m_rs_reducedKappa[partition][drawer][pmt1][pmt2][gain][1]->Mean()/(q1*q2)-1) > 0.01 ){
	      continue;
	    }
	    m_kappa[partition][drawer][1][gain] += (m_rs_reducedKappa[partition][drawer][pmt1][pmt2][gain][1]->Mean()/(q1*q2)-1);
	    if ( m_kappa[partition][drawer][1][gain]<0.){
	      ATH_MSG_DEBUG ( "Negative kappa value: " <<  m_kappa[partition][drawer][1][gain] );
	    }
	    nCouplesOdd++;
	  }// pmt2
	}// pmt1

	//We evaluate kappa value for even PMTs
	for (int pmt1=0; pmt1<NCOUPLES-1; ++pmt1){
	  for (int pmt2=pmt1+1; pmt2<NCOUPLES; ++pmt2){
	    chan1 = getCoupleOfPMT(partition, pmt2).first;
	    chan2 = getCoupleOfPMT(partition, pmt1).first;
	    q1 = m_rs_signal[partition][drawer][chan1][gain]->Mean();
	    q2 = m_rs_signal[partition][drawer][chan2][gain]->Mean();

	    if (q1*q2<=0){
	      continue;
	    }
	    if (m_rs_reducedKappa[partition][drawer][pmt1][pmt2][gain][0]->Mean()<q1*q2){
	      continue;
	    }
	    if ((m_rs_reducedKappa[partition][drawer][pmt1][pmt2][gain][0]->Mean()/(q1*q2)-1) >0.01){
	      continue;
	    }
	    m_kappa[partition][drawer][0][gain] += (m_rs_reducedKappa[partition][drawer][pmt1][pmt2][gain][0]->Mean()/(q1*q2)-1);
	    nCouplesEven++;
	    if (m_kappa[partition][drawer][0][gain]<0.){
	      ATH_MSG_DEBUG ( "Negative kappa value: " <<  m_kappa[partition][drawer][0][gain] );
	    }// if
	  }// pmt2
	}// pmt1
	
        if ( nCouplesEven!=0 ){
	  m_kappa[partition][drawer][0][gain] = m_kappa[partition][drawer][0][gain]/nCouplesEven;
	  if (m_kappa[partition][drawer][0][gain]>0.01){
	    ATH_MSG_DEBUG ( "Too big kappa value: " << m_kappa[partition][drawer][0][gain] << "  " << nCouplesEven);
	  }
	}
        if ( nCouplesOdd!=0 ){
	  m_kappa[partition][drawer][1][gain] = m_kappa[partition][drawer][1][gain]/nCouplesOdd;
	  if ( m_kappa[partition][drawer][1][gain]>0.01){
	    ATH_MSG_DEBUG ( "Too big kappa value: " <<  m_kappa[partition][drawer][1][gain] << "  " << nCouplesOdd );
	  }
	}
               
        for(int channel=0; channel<NCHANNELS; ++channel){
          m_time[partition][drawer][channel][gain]       = m_rs_time[partition][drawer][channel][gain]->Mean();
          m_time_S[partition][drawer][channel][gain]     = m_rs_time[partition][drawer][channel][gain]->StandardDeviation();
          m_mean[partition][drawer][channel][gain]       = m_rs_signal[partition][drawer][channel][gain]->Mean();
          m_mean_S[partition][drawer][channel][gain]     = m_rs_signal[partition][drawer][channel][gain]->StandardDeviation();
          m_raw_mean[partition][drawer][channel][gain]   = m_rs_raw_signal[partition][drawer][channel][gain]->Mean();
          m_raw_mean_S[partition][drawer][channel][gain] = m_rs_raw_signal[partition][drawer][channel][gain]->StandardDeviation();
          
          //-- V.Giangiobbe : save the average charge and variance in slices of m_eventsPerSlice=1000
          if(m_pisaMethod2){
            int nSlices = std::min(NSLICES,m_rs_signal[partition][drawer][channel][gain]->GetNSlices());
            for(int iSlice=0; iSlice<nSlices; ++iSlice){
              m_mean_slice[partition][drawer][channel][iSlice][gain]     = m_rs_signal[partition][drawer][channel][gain]->Mean(iSlice);
              m_variance_slice[partition][drawer][channel][iSlice][gain] = m_rs_signal[partition][drawer][channel][gain]->Variance(iSlice);
            } // FOR
          } // IF
          
          if (m_LASERII) { // Laser II

            for ( int diode=0; diode<NDIODES; diode++ ) {
	      for ( int diode_gain=0; diode_gain<NGAINS; diode_gain++) {
		m_ratio_LASERII[diode][diode_gain][partition][drawer][channel][gain]   = m_rs_ratio_LASERII[diode][diode_gain][partition][drawer][channel][gain]->Mean();
		m_ratio_S_LASERII[diode][diode_gain][partition][drawer][channel][gain] = m_rs_ratio_LASERII[diode][diode_gain][partition][drawer][channel][gain]->StandardDeviation();
		m_ratio_LASERII_good[diode][diode_gain][partition][drawer][channel][gain]   = m_rs_ratio_LASERII_good[diode][diode_gain][partition][drawer][channel][gain]->Mean();
		m_ratio_S_LASERII_good[diode][diode_gain][partition][drawer][channel][gain] = m_rs_ratio_LASERII_good[diode][diode_gain][partition][drawer][channel][gain]->StandardDeviation();
	      } // FOR
	    }
	    m_pmt_ratios[partition][drawer][channel][gain] = m_rs_pmt_ratios[partition][drawer][channel][gain]->Mean();
	    if (std::abs(m_rs_pmt_ratios[partition][drawer][channel][gain]->Mean())>100000.) {
	      ATH_MSG_DEBUG( "too big value for " << partition << " " << drawer	<< " " << channel << " " << gain << " "
			     << m_rs_pmt_ratios[partition][drawer][channel][gain]->NumDataValues() << "status" << m_status[partition][drawer][channel][gain] );
	    }
	    m_pmt_S_ratios[partition][drawer][channel][gain] = m_rs_pmt_ratios[partition][drawer][channel][gain]->StandardDeviation();	    
	    
	  } else { // Laser I

	    for(int d=0; d<NDIODES_LASER1; ++d){
              m_ratio[d][partition][drawer][channel][gain]   = m_rs_ratio[d][partition][drawer][channel][gain]->Mean();
              m_ratio_S[d][partition][drawer][channel][gain] = m_rs_ratio[d][partition][drawer][channel][gain]->StandardDeviation();
              m_ratio_good[d][partition][drawer][channel][gain]   = m_rs_ratio_good[d][partition][drawer][channel][gain]->Mean();
              m_ratio_good_S[d][partition][drawer][channel][gain] = m_rs_ratio_good[d][partition][drawer][channel][gain]->StandardDeviation();
            } // Laser 1 diodes

          }
	  
        } // Channels
      } // Gain
    } // Drawer
  } // Partition
  

  m_cabling = TileCablingService::getInstance();
  // Store high voltage
  for(int part=0; part<NPARTITIONS; ++part){
    int ros = part+1;
    for(int drawer=0; drawer<NDRAWERS; ++drawer){
      for(int channel=0; channel<NCHANNELS; ++channel){
        m_HV[part][drawer][channel] = m_tileDCS->getChannelHV(ros, drawer, channel);
        m_HVSet[part][drawer][channel] = m_tileDCS->getChannelHVSet(ros, drawer, channel);
      } // channel
    } // drawers
  } // partitions

  // remove all RunningStat objects from memory
  for ( int diode=0; diode<NDIODES; ++diode ) {
    for ( int gain=0; gain<NGAINS; gain++ ) {
      delete m_rs_diode_signal_LASERII[diode][gain];
    }
  }

  for(int pmt=0;pmt<NPMTS;++pmt){
    for ( int gain=0; gain<NGAINS; gain++ ) {
      delete m_rs_PMT_signal_LASERII[pmt][gain];
    }
    delete m_rs_PMT_signal[pmt];
  }

  for(int d=0; d<NDIODES_LASER1; ++d){
    delete m_rs_diode_signal[d];
  }

  for ( int part=0; part<NPARTITIONS; ++part ) {
    for ( int gain=0; gain<NGAINS; ++gain ) {
      delete m_rs_meantime[part][gain];
    }

    for ( int drawer=0; drawer<NDRAWERS; ++drawer ) {
      for ( int gain=0; gain<NGAINS; ++gain ) {
        for (int fiber=0; fiber<NFIBERS; ++fiber){
	   for (int pmt1=0; pmt1<NCOUPLES-1; ++pmt1){
	     for (int pmt2=pmt1+1; pmt2<NCOUPLES; ++pmt2){
	       delete m_rs_reducedKappa[part][drawer][pmt1][pmt2][gain][fiber];
	     }
	   }
	}

        for ( int channel=0; channel<NCHANNELS; ++channel ) {
          delete m_rs_time[part][drawer][channel][gain];
          delete m_rs_signal[part][drawer][channel][gain];
          delete m_rs_raw_signal[part][drawer][channel][gain];

          for(int diode=0; diode<NDIODES; ++diode){
	    for (int diode_gain=0; diode_gain<NGAINS; diode_gain++) {
	      delete m_rs_ratio_LASERII[diode][diode_gain][part][drawer][channel][gain];
	      delete m_rs_ratio_LASERII_good[diode][diode_gain][part][drawer][channel][gain];
	    }
          }

	  delete m_rs_pmt_ratios[part][drawer][channel][gain];

          for(int d=0; d<NDIODES_LASER1; ++d){
            delete m_rs_ratio[d][part][drawer][channel][gain];
	    delete m_rs_ratio_good[d][part][drawer][channel][gain];
          }

        } // channel loop
      } // gain loop
    } // drawer loop
  } // partition loop

  return StatusCode::SUCCESS;
} // FINALIZECALCULATIONS


StatusCode TileLaserDefaultCalibTool::writeNtuple(int runNumber, int runType, TFile * rootFile){
  // CALLED FROM LASERCALIBALG TO STORE CALIBRATION COEFFICIENTS
  // STORES NTUPLE AS ROOT FILE
  ATH_MSG_INFO ( "finalize(" << runNumber << "," << runType << "," << rootFile << ")" );
  
  // CREATE OUTPUT TREE
  m_toolRunNo = runNumber;
  
  TTree *t = new TTree(m_toolNtuple.c_str(), "TileLaserCalib-Ntuple");
  t->Branch("RunNumber",&m_toolRunNo, "runNo/I");
  t->Branch("ADC_status",&m_ADC_problem, "ADC/I");
  t->Branch("WheelPos",&m_las_filter, "wheelpos/I");
  t->Branch("RequestedAmp",&m_las_requ_amp, "requamp/F");
  t->Branch("TimeRun",&m_las_time, "timeofrun/F");
  t->Branch("MeanTime",m_meantime,"meantime[4][2]/F");
  t->Branch("Time",*m_time,"time[4][64][48][2]/F");
  t->Branch("Sigma_Time",*m_time_S,"time_s[4][64][48][2]/F");
  t->Branch("Signal",*m_mean,"signal[4][64][48][2]/F");
  t->Branch("Sigma_Signal",*m_mean_S,"signal_s[4][64][48][2]/F");
  t->Branch("Raw_Signal",*m_raw_mean,"rawsignal[4][64][48][2]/F");
  t->Branch("Raw_Sigma_Signal",*m_raw_mean_S,"rawsignal_s[4][64][48][2]/F");
  t->Branch("LaserEntries",*m_entries,"LASER_entries[4][64][48][2]/I");
  t->Branch("Kappa",*m_kappa,"Kappa[4][64][2][2]/F");
  t->Branch("Status",*m_status,"Status[4][64][48][2]/S");
  t->Branch("HV",*m_HV,"HV[4][64][48]/F");
  t->Branch("HVSet",*m_HVSet,"HVSet[4][64][48]/F");
  
  if(m_LASERII){
               /* Laser II */
    t->Branch("PMT_Signal",*m_PMT_LASERII, "PMT[2][2]/F");
    t->Branch("PMT_Sigma_Signal",*m_PMT_S_LASERII, "PMT_s[2][2]/F");

    t->Branch("PMT_Ped",*m_PMT_Ped_LASERII, "PMT_Ped[2][2]/F");
    t->Branch("PMT_Sigma_Ped",*m_PMT_Ped_S_LASERII, "PMT_sPed[2][2]/F");

    t->Branch("Diode_Signal",*m_diode_LASERII, "diode[10][2]/F");
    t->Branch("Diode_Sigma_Signal",*m_diode_S_LASERII, "diode_s[10][2]/F");
    t->Branch("Diode_Entries", *m_entries_diode_LASERII, "diode_entries[10][2]/I");

    t->Branch("Diode_Ped",*m_diode_Ped_LASERII,"diode_Ped[11][2]/F");
    t->Branch("Diode_Sigma_Ped",*m_diode_Ped_S_LASERII,"diode_sPed[11][2]/F");

    t->Branch("Diode_Alpha",*m_diode_Alpha_LASERII,"diode_Alpha[11][2]/F");
    t->Branch("Diode_Sigma_Alpha",*m_diode_Alpha_S_LASERII,"diode_sAlpha[11][2]/F");

    t->Branch("Diode_Led",*m_diode_Led_LASERII,"diode_Led[11][2]/F");
    t->Branch("Diode_Sigma_Led",*m_diode_Led_S_LASERII,"diode_sLed[11][2]/F");

    t->Branch("Ratio",*m_ratio_LASERII,"signal_cor[10][2][4][64][48][2]/F");
    t->Branch("Sigma_Ratio",*m_ratio_S_LASERII,"signal_cor_s[10][2][4][64][48][2]/F");
    t->Branch("Ratio_good",*m_ratio_LASERII_good,"signal_cor_good[10][2][4][64][48][2]/F");
    t->Branch("Sigma_Ratio_good",*m_ratio_S_LASERII_good,"signal_cor_good_s[10][2][4][64][48][2]/F");
    t->Branch("Pmt_Ratio", *m_pmt_ratios, "pmt_ratio[4][64][48][2]/F");
    t->Branch("Sigma_Pmt_Ratio", *m_pmt_S_ratios, "pmt_ratio_s[4][64][48][2]/F");
 
  } else {
              /* Laser I */
    t->Branch("Humidity",&m_hrate,"humid/F");
    t->Branch("AirFlow",&m_flow,"flow/F");
    t->Branch("HeadTemp",&m_head_temp,"htemp/F");

    t->Branch("PMT1_Signal",&m_PMT[0], "PMT_1/F");
    t->Branch("PMT2_Signal",&m_PMT[1], "PMT_2/F");
    t->Branch("PMT_Sigma_Signal",m_PMT_S, "PMT_s[2]/F");
    t->Branch("Diode_Signal",m_diode, "diode[4]/F");
    t->Branch("Diode_Sigma_Signal",m_diode_S, "diode_s[4]/F");
    t->Branch("Diode_Ped",m_diode_Ped, "diode_Ped[4]/F");
    t->Branch("Diode_Sigma_Ped",m_diode_SPed, "diode_sPed[4]/F");
    t->Branch("Diode_Alpha",m_diode_Alpha, "diode_Alpha[4]/F");
    t->Branch("Diode_Sigma_Alpha",m_diode_SAlpha, "diode_sAlpha[4]/F");
    
    t->Branch("Ratio",*m_ratio,"signal_cor[4][4][64][48][2]/F");
    t->Branch("Sigma_Ratio",*m_ratio_S,"signal_cor_s[4][4][64][48][2]/F");
    t->Branch("Ratio",*m_ratio_good,"signal_cor_good[4][4][64][48][2]/F");
    t->Branch("Sigma_Ratio",*m_ratio_good_S,"signal_cor_good_s[4][4][64][48][2]/F");
  } // ELSE
  
  if(m_pisaMethod2){
    t->Branch("MeanSlice",*m_mean_slice,"MeanSlice[4][64][48][100][2]/F");
    t->Branch("VarianceSlice",*m_variance_slice,"VarianceSlice[4][64][48][100][2]/F");
  } // IF
  
  if (!m_stuckBitsProbs.empty()) {
    if (m_stuckBitsProbs.retrieve().isFailure()) {
      ATH_MSG_WARNING("Impossible to get ITileStuckBitsProbsTool and stuck bits probabilities!");
    } else {
      m_stuckBitsProbs->saveStuckBitsProbabilities(t);
    }
  }

  // Fill values for this run
  t->Fill();
  
  return StatusCode::SUCCESS;
} // Write ntuple


StatusCode TileLaserDefaultCalibTool::finalize(){
  ATH_MSG_INFO ( "finalize()" );
  return StatusCode::SUCCESS;
} // FInalize


std::pair<unsigned int, unsigned int> TileLaserDefaultCalibTool::getCoupleOfPMT(int part, int couple){
  std::pair<unsigned int, unsigned int> coupleOfPMTs;

  int chanLBOdd[NCOUPLES] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 33, 35, 37, 39, 41, 45, 47};
  int chanLBEven[NCOUPLES] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 34, 36, 38, 40, 42, 44, 46};

  int chanEBOdd[NCOUPLES] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 21, 23, 32, 35, 36, 37, 40, -1, -1, -1, -1, -1, -1};
  int chanEBEven[NCOUPLES] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 20, 22, 30, 31, 38, 39, 41, -1, -1, -1, -1, -1, -1};

  if (part<2){   //----LB
    coupleOfPMTs.first = chanLBEven[couple];
    coupleOfPMTs.second = chanLBOdd[couple];
  } else {  //----EB
    coupleOfPMTs.first = chanEBEven[couple];
    coupleOfPMTs.second = chanEBOdd[couple];
  }

  return coupleOfPMTs;
}

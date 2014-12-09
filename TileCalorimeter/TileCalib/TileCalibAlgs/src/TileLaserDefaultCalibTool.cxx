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
/* TileLaserDefaultCalibTool.cxx     July 2014      */
/*                                                  */
/* Marco van Woerden <mvanwoer@cern.ch>             */
/*                                                  */
/****************************************************/

TileLaserDefaultCalibTool::TileLaserDefaultCalibTool(const std::string& type, const std::string& name,const IInterface* pParent):
AlgTool(type, name, pParent),
m_tileBadChanTool("TileBadChanTool"),
m_tileDCSSvc("TileDCSSvc",name){
  declareInterface<ITileCalibTool>( this );
  declareProperty("toolNtuple", m_toolNtuple="h3000");
  declareProperty("rawChannelContainer", m_rawChannelContainerName="");
  declareProperty("laserObjContainer", m_laserContainerName="");
  declareProperty("pisaMethod2", m_pisaMethod2=true);
  declareProperty("TileDCSSvc",m_tileDCSSvc);
} // TileLaserDefaultCalibTool::TileLaserDefaultCalibTool

TileLaserDefaultCalibTool::~TileLaserDefaultCalibTool()
{}


StatusCode TileLaserDefaultCalibTool::initialize(){
  // RESET LOCAL PARAMETERS AND LOAD NECESSARY SERVICE
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "initialize() TileLaserDefaultCalibTool" << endreq;
  m_evtNr        = 0;
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
  
  // LOOP OVER MONITORING PMTS
  for(int chan=0; chan<NCHANNELS; ++chan){
    // LASERII
    m_chan_LAS[chan] = 0;             // Mean value for monitoring diodes, PMTs, phocal, CIS
    m_chan_PED[chan] = 0;             // Mean value for monitoring diodes, PMTs, phocal, CIS
    m_chan_LED[chan] = 0;             // Mean value for monitoring diodes, PMTs, phocal, CIS
    m_chan_APH[chan] = 0;             // Mean value for monitoring diodes, PMTs, phocal, CIS
    m_chan_LIN[chan] = 0;             // Mean value for monitoring diodes, PMTs, phocal, CIS
    m_chan_S_LAS[chan] = 0;           // Corresponding RMS
    m_chan_S_PED[chan] = 0;           // Corresponding RMS
    m_chan_S_LED[chan] = 0;           // Corresponding RMS
    m_chan_S_APH[chan] = 0;           // Corresponding RMS
    m_chan_S_LIN[chan] = 0;           // Corresponding RMS
    m_chan_Ped[chan] = 0;             // Corresponding pedestal values
    m_chan_Led[chan] = 0;             // Corresponding LED values
    m_chan_Lin[chan] = 0;             // Corresponding linearity values
    m_chan_Alpha[chan] = 0;           // Corresponding alpha peaks
    m_chan_SPed[chan] = 0;            // Sigma of pedestal values
    m_chan_SLed[chan] = 0;            // Sigma of LED values
    m_chan_SLin[chan] = 0;            // Sigma of linearity values
    m_chan_SAlpha[chan] = 0;          // Sigma of alpha peaks
    
    //m_HG_PMT[d]      = 0;
    //m_HG_PMT_S[d]    = 0;
    //rs_HG_PMT_signal[d] = new RunningStat();
    rs_chan_Laser[chan] = new RunningStat();
    rs_chan_Pedestal[chan] = new RunningStat();
    rs_chan_LED[chan] = new RunningStat();
    rs_chan_Alpha[chan] = new RunningStat();
    rs_chan_Linearity[chan] = new RunningStat();
    
    //m_LG_PMT[d]      = 0;
    //m_LG_PMT_S[d]    = 0;
    //rs_LG_PMT_signal[d] = new RunningStat();
  } // FOR
  
  for(int d=0;d<2;++d){
    // LASERI
    m_PMT[d]      = 0;
    m_PMT_S[d]    = 0;
    rs_PMT_signal[d] = new RunningStat();
  } // FOR
  
  // LASERI
  for(int d=0; d<4; ++d){
    m_diode[d]       = 0;
    m_diode_S[d]     = 0;
    m_diode_Ped[d]   = 0;
    m_diode_Alpha[d] = 0;
    m_diode_SPed[d]  = 0;
    m_diode_SAlpha[d]= 0;
    rs_diode_signal[d]  = new RunningStat();
  } // FOR
  
  for(int part=0; part<4; ++part){
    rs_meantime[part] = new RunningStat();
    m_meantime[part] =0.;
    for(int drawer=0; drawer<64; ++drawer){
      for(int gain=0; gain<2; ++gain){
        
        // K FACTOR FOR EACH COUPLE OF ODD AND EVENT PMTS
        for(int couple=0; couple<22; ++couple){
          rs_reducedKappa[part][drawer][couple][gain] = new RunningStat();
        } // FOR
        
        for(int channel=0; channel<48; ++channel){
          rs_time[part][drawer][channel][gain]   = new RunningStat();
          rs_signal[part][drawer][channel][gain] = new RunningStat();
          rs_raw_signal[part][drawer][channel][gain] = new RunningStat();
          m_time[part][drawer][channel][gain]    = 0;
          m_time_S[part][drawer][channel][gain]  = 0;
          m_mean[part][drawer][channel][gain]    = 0;
          m_mean_S[part][drawer][channel][gain]  = 0;
          m_raw_mean[part][drawer][channel][gain]    = 0;
          m_raw_mean_S[part][drawer][channel][gain]  = 0;
          m_kappa[part][drawer][channel][gain]   = 0;
          m_status[part][drawer][channel][gain]  = 0;
          
          for(int iSlice=0; iSlice<100; ++iSlice){
            m_mean_slice[part][drawer][channel][iSlice][gain] = 0;
            m_variance_slice[part][drawer][channel][iSlice][gain] = 0;
          } // FOR
          
          // LASERII
          for(int chan=0; chan<NCHANNELS; ++chan){
            rs_ratio_LASERII[chan][part][drawer][channel][gain]  = new RunningStat();
            m_ratio_LASERII[chan][part][drawer][channel][gain]   = 0;
            m_ratio_S_LASERII[chan][part][drawer][channel][gain] = 0;
          } // FOR
          // LASERI
          for(int d=0; d<4; ++d){
            rs_ratio[d][part][drawer][channel][gain]  = new RunningStat();
            m_ratio[d][part][drawer][channel][gain]   = 0;
            m_ratio_S[d][part][drawer][channel][gain] = 0;
          } // FOR
          
          
          m_entries[part][drawer][channel][gain] = 0;
          
          //	  int pmt = abs(m_cabling->channel2hole(part,channel));
          m_HV[part][drawer][channel] = 0.; //m_tileDCSSvc->getDCSHV(part, drawer, pmt);
          m_HVSet[part][drawer][channel] = 0.; //m_tileDCSSvc->getDCSHVSET(part, drawer, pmt);
          //	  std::cout << "Henric " <<  m_HV[part][drawer][channel] << " " << m_HVSet[part][drawer][channel] << std::endl;
        } // channel loop
      } // gain loop
    } // drawer loop
  } // partition loop
  
  // RETRIEVE STORAGE SERVICES
  if(service("StoreGateSvc", m_evtStore).isFailure()){
    log<<MSG::ERROR<<"Unable to get pointer to eventStore Service"<<endreq;
    return StatusCode::FAILURE;
  } // IF
  
  if(service("DetectorStore", m_detStore).isFailure()){
    log << MSG::ERROR<<"Unable to get pointer to DetectorStore Service" << endreq;
    return StatusCode::FAILURE;
  } // IF
  
  IToolSvc* toolSvc;
  if(service("ToolSvc",toolSvc).isFailure()){
    log << MSG::ERROR <<" Can't get ToolSvc " << endreq;
    return StatusCode::FAILURE;
  } // IF
  
  // get TileHWID helper
  if(m_detStore->retrieve(m_tileHWID).isFailure()){
    log << MSG::ERROR << "Unable to retrieve TileHWID helper from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  } // IF
  
  if(m_tileToolEmscale.retrieve().isFailure()){
    log << MSG::ERROR << "Unable to retrieve " << m_tileToolEmscale << endreq;
    return StatusCode::FAILURE;
  } // IF
  
  // get TileCabling Service
  m_cabling = TileCablingService::getInstance();
  
  // get beam info tool
  if(toolSvc->retrieveTool("TileBeamInfoProvider",m_beamInfo).isFailure()){
    log << MSG::ERROR << "Unable to get tool 'TileBeamInfoProvider'" << endreq;
    return StatusCode::FAILURE;
  } // IF
  
  if(m_tileBadChanTool.retrieve().isFailure()){
    log << MSG::ERROR << "Unable to retrieve " << m_tileBadChanTool << endreq;
    return StatusCode::FAILURE;
  } // IF
  
  //=== get TileDCSSvc
  if(m_tileDCSSvc.retrieve().isFailure()){
    log << MSG::ERROR << "Unable to retrieve " << m_tileDCSSvc << endreq;
    return StatusCode::FAILURE;
  } // IF
  return StatusCode::SUCCESS;
}

StatusCode TileLaserDefaultCalibTool::initNtuple(int runNumber, int runType, TFile * rootFile){
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "initialize(" << runNumber << "," << runType << "," << rootFile << ")" << endreq;
  return StatusCode::SUCCESS;
} // INITNTUPLE

StatusCode TileLaserDefaultCalibTool::execute(){
  MsgStream log(msgSvc(),name());
  
  // INCREMENT EVENT NUMBER
  ++m_evtNr;
  log << MSG::DEBUG << "EVENT COUNTER: " << m_evtNr << endreq;
  
  // STORE LASER OBJECT AND RAWCHANNEL INFORMATION INTO MAPS
  log << MSG::DEBUG << "execute() TileLaserDefaultCalibTool" << endreq;
  
  const TileRawChannelContainer * rawCnt = 0;
  const TileLaserObject* laserObj;
  
  if((m_evtStore->retrieve(rawCnt,   m_rawChannelContainerName)).isFailure() ||
     (m_evtStore->retrieve(laserObj, m_laserContainerName))     .isFailure()){
    log << MSG::ERROR << "There is a problem opening TileRawChannelContainer or the LASER object" << endreq;
    return StatusCode::FAILURE;
  } // IF
  
  m_LASERII = laserObj->isLASERII();
  if(laserObj->isLASERII()) log << MSG::DEBUG << "LASERII VERSION IS " << laserObj->getVersion() << " DAQ TYPE = " << laserObj->getDaqType() << endreq;
  else                      log << MSG::DEBUG << "LASERI VERSION IS "  << laserObj->getVersion() << " DAQ TYPE = " << laserObj->getDaqType() << endreq;
  
  const uint32_t *cispar = m_beamInfo->cispar();
  
  // EVENT TIME IN NS SINCE 01/JAN/1970
  m_las_time = static_cast<double>(cispar[10])+static_cast<double>(cispar[11])/1000000;
  
  // RETRIEVE LASER INFORMATION
  if(laserObj->getDiodeCurrOrd() == 0 || laserObj->getFiltNumber() == 0){
    log << MSG::DEBUG << "NO FILTER NUMBER OR DIODE CURRENT: WHEEL MOVING?" << endreq;
    return StatusCode::SUCCESS; // This is expected for some events
  } // IF
  
  // LASERII
  for(int i=0; i<NDIODES; ++i){
    log << MSG::DEBUG << i << "UNKNOWN RUN HG="
    << laserObj->getDiodeADC(i,0)
    << "  &  LG=" << laserObj->getDiodeADC(i,1)
    << " ( DAQ TYPE=" << laserObj->getDaqType()
    << " ) " << endreq;
    
    // FIRST 4 EVENTS ARE SKIPPED TO RETRIEVE LASER PEDESTAL VALUES
    // DEBUGGED FROM DATA
    // LASERCALIB: DAQ TYPE 0x10
    // LASER:      DAQ TYPE 0x18
    // PEDESTAL:   DATA FORMAT MISMATCH
    // PHYSICS:    DATA FORMAT MISMATCH
    // ALPHA:      DAQ TYPE 0x10
    
    if(m_evtNr>4){
      // MONITORING DIODES
      if(laserObj->getDaqType()==0x10){
        rs_chan_Pedestal[i*2+0]->Push(laserObj->getDiodeADC(i,0));
        rs_chan_Pedestal[i*2+1]->Push(laserObj->getDiodeADC(i,1));
      } // IF
      else if(laserObj->getDaqType()==0x11){
        rs_chan_Alpha[i*2+0]->Push(laserObj->getDiodeADC(i,0));
        rs_chan_Alpha[i*2+1]->Push(laserObj->getDiodeADC(i,1));
        
        log << MSG::DEBUG << i << "LASER RUN HG="
        << laserObj->getDiodeADC(i,0)
        << "  &  LG=" << laserObj->getDiodeADC(i,1)
        << " ( DAQ TYPE=" << laserObj->getDaqType()
        << " ) " << endreq;
      } // ELSE IF
      else if(laserObj->getDaqType()==0x12){
        rs_chan_LED[i*2+0]->Push(laserObj->getDiodeADC(i,0));
        rs_chan_LED[i*2+1]->Push(laserObj->getDiodeADC(i,1));
        
        log << MSG::DEBUG << i << "LED RUN HG="
        << laserObj->getDiodeADC(i,0)
        << "  &  LG=" << laserObj->getDiodeADC(i,1)
        << " ( DAQ TYPE=" << laserObj->getDaqType()
        << " ) " << endreq;
      } // ELSE IF
      else if(laserObj->getDaqType()==0x13){
        rs_chan_Linearity[i*2+0]->Push(laserObj->getDiodeADC(i,0));
        rs_chan_Linearity[i*2+1]->Push(laserObj->getDiodeADC(i,1));
        
        log << MSG::DEBUG << i << "LINEARITY RUN HG="
        << laserObj->getDiodeADC(i,0)
        << "  &  LG=" << laserObj->getDiodeADC(i,1)
        << " ( DAQ TYPE=" << laserObj->getDaqType()
        << " ) " << endreq;
      } // ELSE IF
      else if(laserObj->getDaqType()==0x18){
        rs_chan_Laser[i*2+0]->Push(laserObj->getDiodeADC(i,0));
        rs_chan_Laser[i*2+1]->Push(laserObj->getDiodeADC(i,1));
        
        log << MSG::DEBUG << i << "LASER RUN HG="
        << laserObj->getDiodeADC(i,0)
        << "  &  LG=" << laserObj->getDiodeADC(i,1)
        << " ( DAQ TYPE=" << laserObj->getDaqType()
        << " ) " << endreq;
      } // ELSE IF
      
      // MONITORING PMTS
      if(i<2){
        if(laserObj->getDaqType()==0x10){
          rs_chan_Pedestal[i*2+0+NDIODES*2]->Push(laserObj->getPMADC(i,0));
          rs_chan_Pedestal[i*2+1+NDIODES*2]->Push(laserObj->getPMADC(i,1));
        } // IF
        else if(laserObj->getDaqType()==0x11){
          rs_chan_Alpha[i*2+0+NDIODES*2]->Push(laserObj->getPMADC(i,0));
          rs_chan_Alpha[i*2+1+NDIODES*2]->Push(laserObj->getPMADC(i,1));
        } // ELSE IF
        else if(laserObj->getDaqType()==0x12){
          rs_chan_LED[i*2+0+NDIODES*2]->Push(laserObj->getPMADC(i,0));
          rs_chan_LED[i*2+1+NDIODES*2]->Push(laserObj->getPMADC(i,1));
        } // ELSE IF
        else if(laserObj->getDaqType()==0x13){
          rs_chan_Linearity[i*2+0+NDIODES*2]->Push(laserObj->getPMADC(i,0));
          rs_chan_Linearity[i*2+1+NDIODES*2]->Push(laserObj->getPMADC(i,1));
        } // ELSE IF
        else if(laserObj->getDaqType()==0x18){
          rs_chan_Laser[i*2+0+NDIODES*2]->Push(laserObj->getPMADC(i,0));
          rs_chan_Laser[i*2+1+NDIODES*2]->Push(laserObj->getPMADC(i,1));
        } // ELSE IF
      } // IF
    } // IF
  } // FOR
  
  // LASERI
  for(int i=0; i<4; ++i){
    rs_diode_signal[i]->Push(laserObj->getDiodeADC(i,0)-laserObj->getDiodePedestal(i,0));
  } // FOR
  
  for(int i=0; i<2; ++i){
    rs_PMT_signal[i]->Push(laserObj->getPMADC(i,0)-laserObj->getPMPedestal(i,0));    // LASERI
  } // FOR
  
  // CHECK THAT ADC INFORMATION HAS BEEN SENT
  if(laserObj->getPMADC(0,0) == m_PMT1_ADC_prev[0] &&
     laserObj->getPMADC(1,0) == m_PMT2_ADC_prev[0]){
    m_ADC_problem = 1;
    log << MSG::WARNING << "There is perhaps an ADC problem with this event" << endreq;
  } // IF
  if(laserObj->getPMADC(0,1) == m_PMT1_ADC_prev[1] &&
     laserObj->getPMADC(1,1) == m_PMT2_ADC_prev[1] && m_LASERII){
    m_ADC_problem = 1;
    log << MSG::WARNING << "There is perhaps an ADC problem with this event" << endreq;
  } // IF
  
  m_PMT1_ADC_prev[0] = laserObj->getPMADC(0,0); // LG
  m_PMT2_ADC_prev[0] = laserObj->getPMADC(1,0); // LG
  m_PMT1_ADC_prev[1] = laserObj->getPMADC(0,1); // HG
  m_PMT2_ADC_prev[1] = laserObj->getPMADC(1,1); // HG
  
  
  // Next parameters are constants, don't need to update them more than once
  
  if(m_las_filter == 0){
    m_las_filter   = laserObj->getFiltNumber();
    m_hrate        = laserObj->getHumidity();
    m_flow         = laserObj->getGasFlux();
    m_head_temp    = laserObj->getPumpDiodeTemp();
    m_las_requ_amp = laserObj->getDiodeCurrOrd();
  } // IF
  
  // LASERII
  for(int chan=0;chan<NCHANNELS;++chan){
    if(laserObj->isSet(chan/2, chan%2, 0) && laserObj->getMean (chan/2,chan%2,0)>0) m_chan_Ped[chan]    = laserObj->getMean(chan/2,chan%2,0);
    if(laserObj->isSet(chan/2, chan%2, 2) && laserObj->getMean (chan/2,chan%2,2)>0) m_chan_Led[chan]    = laserObj->getMean(chan/2,chan%2,2);
    if(laserObj->isSet(chan/2, chan%2, 3) && laserObj->getMean (chan/2,chan%2,3)>0) m_chan_Alpha[chan]  = laserObj->getMean(chan/2,chan%2,3);
    if(laserObj->isSet(chan/2, chan%2, 1) && laserObj->getMean (chan/2,chan%2,1)>0) m_chan_Lin[chan]    = laserObj->getMean(chan/2,chan%2,1);
    if(laserObj->isSet(chan/2, chan%2, 0) && laserObj->getMean (chan/2,chan%2,0)>0) m_chan_SPed[chan]   = laserObj->getSigma(chan/2,chan%2,0);
    if(laserObj->isSet(chan/2, chan%2, 2) && laserObj->getMean (chan/2,chan%2,2)>0) m_chan_SLed[chan]   = laserObj->getSigma(chan/2,chan%2,2);
    if(laserObj->isSet(chan/2, chan%2, 3) && laserObj->getMean (chan/2,chan%2,3)>0) m_chan_SAlpha[chan] = laserObj->getSigma(chan/2,chan%2,3);
    if(laserObj->isSet(chan/2, chan%2, 1) && laserObj->getMean (chan/2,chan%2,1)>0) m_chan_SLin[chan]   = laserObj->getSigma(chan/2,chan%2,1);
    
    // DEBUG OUTPUT
    if(chan%2==0){
      log << MSG::DEBUG <<"HG CHAN " << chan/2 << " PED= " << m_chan_Ped[chan]   << "+/-" << m_chan_SPed[chan]   << " ( " << laserObj->isSet(chan/2, chan%2, 0) << " ) " << endreq;
      log << MSG::DEBUG <<"HG CHAN " << chan/2 << " PED= " << m_chan_Lin[chan]   << "+/-" << m_chan_SLin[chan]   << " ( " << laserObj->isSet(chan/2, chan%2, 1) << " ) " << endreq;
      log << MSG::DEBUG <<"HG CHAN " << chan/2 << " LED= " << m_chan_Led[chan]   << "+/-" << m_chan_SLed[chan]   << " ( " << laserObj->isSet(chan/2, chan%2, 2) << " ) " << endreq;
      log << MSG::DEBUG <<"HG CHAN " << chan/2 << " ALP= " << m_chan_Alpha[chan] << "+/-" << m_chan_SAlpha[chan] << " ( " << laserObj->isSet(chan/2, chan%2, 3) << " ) " << endreq;
    } // IF
    if(chan%2==1){
      log << MSG::DEBUG <<"LG CHAN " << chan/2 << " PED= " << m_chan_Ped[chan]   << "+/-" << m_chan_SPed[chan]   << " ( " << laserObj->isSet(chan/2, chan%2, 0) << " ) " << endreq;
      log << MSG::DEBUG <<"LG CHAN " << chan/2 << " PED= " << m_chan_Lin[chan]   << "+/-" << m_chan_SLin[chan]   << " ( " << laserObj->isSet(chan/2, chan%2, 1) << " ) " << endreq;
      log << MSG::DEBUG <<"LG CHAN " << chan/2 << " LED= " << m_chan_Led[chan]   << "+/-" << m_chan_SLed[chan]   << " ( " << laserObj->isSet(chan/2, chan%2, 2) << " ) " << endreq;
      log << MSG::DEBUG <<"LG CHAN " << chan/2 << " ALP= " << m_chan_Alpha[chan] << "+/-" << m_chan_SAlpha[chan] << " ( " << laserObj->isSet(chan/2, chan%2, 3) << " ) " << endreq;
    } // IF
  } // FOR
  
  // LASERI
  for(int i=0; i<4; ++i){
    m_diode_Ped[i]     = laserObj->getDiodePedestal(i,0);
    m_diode_SPed[i]    = laserObj->getDiodeSigmaPedestal(i,0);
    m_diode_Alpha[i]   = laserObj->getAlpha(i,0);
    m_diode_SAlpha[i]  = laserObj->getSigmaAlpha(i,0);
  } // FOR
  
  // ITERATOR OVER RAWCHANNELCONTAINER
  TileRawChannelUnit::UNIT RChUnit = rawCnt->get_unit();
  TileRawChannelContainer::const_iterator itColl;
  TileRawChannelContainer::const_iterator itCollEnd = rawCnt->end();
  
  double Q1Q2[22];
  for(int couple=0; couple<22; ++couple) Q1Q2[couple]=1;
  int currentDrawer=0;
  
  RunningStat* avg_time[4];
  for(int ros=0; ros<4; ros++) avg_time[ros] = new RunningStat();
  
  // LOOP OVER TILERAWCHANNELCOLLECTIONS
  for(itColl=rawCnt->begin(); itColl != itCollEnd; ++itColl){
    HWIdentifier drawer_id = m_tileHWID->drawer_id((*itColl)->identify());
    
    int ros = m_tileHWID->ros(drawer_id)-1;     // LBA=0 LBC=1 EBA=2 EBC=3
    
    // LOOP OVER TILERAWCHANNELS IN COLLECTION
    for(TileRawChannelCollection::const_iterator it = (*itColl)->begin(); it != (*itColl)->end(); ++it){
      float ofctime = (*it)->time();
      if(ofctime!=0.0 and abs(ofctime-15.0)<30.)
        avg_time[ros]->Push(ofctime);
    } // FOR
  } // FOR
  
  // LOOP OVER TILERAWCHANNELCOLLECTIONS (AGAIN! REASON: WE NEED MEAN OF TIME DISTRIBUTION)
  for(itColl=rawCnt->begin(); itColl != itCollEnd; ++itColl){
    HWIdentifier drawer_id = m_tileHWID->drawer_id((*itColl)->identify());
    int ros = m_tileHWID->ros(drawer_id)-1;     // LBA=0 LBC=1 EBA=2 EBC=3
    int drawer = m_tileHWID->drawer(drawer_id); // 0 to 63
    unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros+1,drawer);
    
    // LOOP OVER TILERAWCHANNELS IN COLLECTION
    for(TileRawChannelCollection::const_iterator it = (*itColl)->begin(); it != (*itColl)->end(); ++it){
      // GET ADCHASH
      HWIdentifier hwid=(*it)->adc_HWID();
      int chan   = m_tileHWID->channel(hwid);  // 0 to 47 channel not PMT
      int gain   = m_tileHWID->adc(hwid);      // low=0 high=1
      int gainrv = gain==0?1:0;                // low=1 high=0 GAIN INDEX
      //log << MSG::DEBUG << "ROS=" << ros << " DRAWER=" << drawer << " CHAN=" << chan << " GAIN=" << gain << endreq;
      float amp = (*it)->amplitude();
      float ofctime = (*it)->time();
      if(ofctime!=0.0) ofctime-=avg_time[ros]->Mean();
      const TileDQstatus *theDQstatus = m_beamInfo->getDQstatus();
      
      // CHECK WHETHER CHANNEL IS CONNECTED
      if(theDQstatus->isChEmpty(ros+1,drawer,chan)){
        m_status[ros][drawer][chan][0] = -1;
        m_status[ros][drawer][chan][1] = -1;
        continue;
      } // IF
      
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
      
      if(theDQstatus->isAdcDQgood(ros+1,drawer,chan,gain)){
        m_status[ros][drawer][chan][gain] = m_tileBadChanTool->encodeStatus(  m_tileBadChanTool->getAdcStatus(drawerIdx, chan, gain) );
      } else {
        m_status[ros][drawer][chan][gain] = 5;
      }
      
      /*
       // Don't enable negative or null energies
       if(amp <= 0.) continue;
       In fact we don't like this line since it biases the averages to follow
       */
      
      float ampInPicoCoulombs = m_tileToolEmscale->channelCalib(drawerIdx, chan, gain, amp, RChUnit, TileRawChannelUnit::PicoCoulombs);
      //log << MSG::DEBUG << "DRAWER=" << drawerIdx << " CHAN=" << chan << " GAIN=" << gain << " AMP=" << amp << " in UNITS: " << RChUnit << " ===> " <<  ampInPicoCoulombs << " pC" << endreq;
      
      rs_time[ros][drawer][chan][gain]->Push(ofctime);
      rs_signal[ros][drawer][chan][gain]->Push(ampInPicoCoulombs);
      rs_raw_signal[ros][drawer][chan][gain]->Push(amp);
      
      //log << MSG::DEBUG << "PMT EVENT STORED!" << endreq;
      
      // FIRST 4 EVENTS ARE SKIPPED TO RETRIEVE LASER PEDESTALS
      if(m_LASERII && m_evtNr>4){
        for(int i=0; i<NDIODES; ++i){
          if(gainrv==0) log << MSG::DEBUG << "HG CHANNEL " << i << " SIG=" << ampInPicoCoulombs << " " << laserObj->getDiodeADC(i,gainrv) << " " << m_chan_Ped[i*2+gainrv] << endreq;
          if(gainrv==1) log << MSG::DEBUG << "LG CHANNEL " << i << " SIG=" << ampInPicoCoulombs << " " << laserObj->getDiodeADC(i,gainrv) << " " << m_chan_Ped[i*2+gainrv] << endreq;
          
          // MONITORING DIODES
          if(laserObj->getDiodeADC(i,gainrv)-m_chan_Ped[i*2+gainrv]!=0)
            rs_ratio_LASERII[i*2+gainrv][ros][drawer][chan][gain]->Push(ampInPicoCoulombs/
                                                                        (laserObj->getDiodeADC(i,gainrv)-m_chan_Ped[i*2+gainrv])
                                                                        );
          
          // MONITORING PMTS
          if(i<2){
            if(laserObj->getPMADC(i,gainrv)-m_chan_Ped[i*2+gainrv+NDIODES*2]!=0)
              rs_ratio_LASERII[i*2+gainrv+NDIODES*2][ros][drawer][chan][gain]->Push(ampInPicoCoulombs/
                                                                                    (laserObj->getPMADC(i,gainrv)-m_chan_Ped[i*2+gainrv+NDIODES*2])
                                                                                    );
          } // IF
        } // FOR
      } // IF
      else{
        for(int i=0; i<4; ++i){
          if((laserObj->getDiodeADC(i,0)-laserObj->getDiodePedestal(i,0)) != 0)
            rs_ratio[i][ros][drawer][chan][gain]->Push(ampInPicoCoulombs/(laserObj->getDiodeADC(i,0)-laserObj->getDiodePedestal(i,0)));
        } // FOR
      } // ELSE
      //---- V.Giangiobbe. Compute the average <q1.q2> for each couple of even and odd PMTs.
      
      //-- RAZ
      if(currentDrawer != drawer){
        for(int couple=0; couple<22; ++couple) Q1Q2[couple]=1;
      } // IF
      
      //-- Store the data in rs_reducedKappa[ros][drawer][couple][gain]
      for(int couple=0; couple<22; ++couple){
        if(chan==int(getCoupleOfChan(ros, couple).first) || chan==int(getCoupleOfChan(ros, couple).second)){
          Q1Q2[couple]*=ampInPicoCoulombs;
          currentDrawer = drawer;
          
          if(Q1Q2[couple]!=1 && Q1Q2[couple]!=ampInPicoCoulombs) rs_reducedKappa[ros][drawer][couple][gain]->Push( Q1Q2[couple] );
        } // IF
      } // FOR
    } // End of the loop over the TileRawChannelCollection
  } // End of the loop over the TileRawChannelContainer
  
  for(int ros=0; ros<4; ros++){
    rs_meantime[ros]->Push(avg_time[ros]->Mean());
    //    printf("%6.2f %6.2f\n",avg_time[ros]->Mean(), meantime[ros]->Mean());
    delete(avg_time[ros]);
  } // FOR
  
  return StatusCode::SUCCESS;
} // EXECUTE

StatusCode TileLaserDefaultCalibTool::finalizeCalculations(){
  // COMPUTE CALIBRATION COEFFICIENT AT THE END OF THE EVENT LOOP
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "finalizeCalculations()" << endreq;
  
  // LASERII
  for(int chan=0;chan<NCHANNELS;++chan){
    m_chan_LAS[chan]   = rs_chan_Laser[chan]->Mean();
    m_chan_PED[chan]   = rs_chan_Pedestal[chan]->Mean();
    m_chan_LED[chan]   = rs_chan_LED[chan]->Mean();
    m_chan_APH[chan]   = rs_chan_Alpha[chan]->Mean();
    m_chan_LIN[chan]   = rs_chan_Linearity[chan]->Mean();
    m_chan_S_LAS[chan] = rs_chan_Laser[chan]->StandardDeviation();
    m_chan_S_PED[chan] = rs_chan_Pedestal[chan]->StandardDeviation();
    m_chan_S_LED[chan] = rs_chan_LED[chan]->StandardDeviation();
    m_chan_S_APH[chan] = rs_chan_Alpha[chan]->StandardDeviation();
    m_chan_S_LIN[chan] = rs_chan_Linearity[chan]->StandardDeviation();
  } // FOR
  
  // LASERI
  for(int d=0; d<2; ++d){
    m_PMT[d]         = rs_PMT_signal[d]->Mean();
    m_PMT_S[d]       = rs_PMT_signal[d]->StandardDeviation();
  } // FOR
  
  for(int d=0; d<4; ++d){
    m_diode[d]       = rs_diode_signal[d]->Mean();
    m_diode_S[d]     = rs_diode_signal[d]->StandardDeviation();
  } // FOR
  
  // LOOP OVER BARRELS, MODULES AND GAINS
  for(int i=0; i<4; ++i){
    m_meantime[i] = rs_meantime[i]->Mean();
    for(int drawer=0; drawer<64; ++drawer){
      for(int gain=0; gain<2; ++gain){
        // COMPUTE THE AVERAGE KAPPA CORRECTION FACTOR FOR ALL EVENT AND ODD PMTS
        // KAPPA IS BY DEFINITION: COV(Q1,Q2)/<Q1>*<Q2> AVERAGE ON ALL COUPLES OF PMTS Q1, Q2
        // RECEIVING LIGHT FROM THE SAME CLEAR FIBER (ONLY 2 INDEPENDENT KAPPA FOR EACH MODULE)
        int nCouplesEven=0, nCouplesOdd=0;
        for(int couple=0; couple<22; ++couple){
          int chan0 = getCoupleOfChan(i, couple).first;
          int chan1 = getCoupleOfChan(i, couple).second;
          double q0 = rs_signal[i][drawer][chan0][gain]->Mean();
          double q1 = rs_signal[i][drawer][chan1][gain]->Mean();
          
          if(q0*q1==0) continue;
          
          //-- Average of all couples on the same even fiber
          if(couple%2==0){
            m_kappa[i][drawer][0][gain] += (rs_reducedKappa[i][drawer][couple][gain]->Mean()/(q0*q1) - 1);
            ++nCouplesEven;
          } else {
            m_kappa[i][drawer][1][gain] += (rs_reducedKappa[i][drawer][couple][gain]->Mean()/(q0*q1) - 1);
            ++nCouplesOdd;
          } // ELSE
        } // FOR
        if(nCouplesEven!=0) m_kappa[i][drawer][0][gain] = m_kappa[i][drawer][0][gain]/nCouplesEven;
        if(nCouplesOdd!=0)  m_kappa[i][drawer][1][gain] = m_kappa[i][drawer][1][gain]/nCouplesOdd;
        
        for(int couple=0; couple<22; ++couple){
          int chan0 = getCoupleOfChan(i, couple).first;
          int chan1 = getCoupleOfChan(i, couple).second;
          if(couple%2==0){
            m_kappa[i][drawer][chan0][gain] = m_kappa[i][drawer][0][gain];
            m_kappa[i][drawer][chan1][gain] = m_kappa[i][drawer][0][gain];
          } else {
            m_kappa[i][drawer][chan0][gain] = m_kappa[i][drawer][1][gain];
            m_kappa[i][drawer][chan1][gain] = m_kappa[i][drawer][1][gain];
          } // ELSE
        } // FOR
        m_kappa[i][drawer][32][gain] = m_kappa[i][drawer][0][gain];
        // END OF KAPPA CALCULATION
        
        for(int k=0; k<48; ++k){
          m_time[i][drawer][k][gain]       = rs_time[i][drawer][k][gain]->Mean();
          m_time_S[i][drawer][k][gain]     = rs_time[i][drawer][k][gain]->StandardDeviation();
          m_mean[i][drawer][k][gain]       = rs_signal[i][drawer][k][gain]->Mean();
          m_mean_S[i][drawer][k][gain]     = rs_signal[i][drawer][k][gain]->StandardDeviation();
          m_raw_mean[i][drawer][k][gain]   = rs_raw_signal[i][drawer][k][gain]->Mean();
          m_raw_mean_S[i][drawer][k][gain] = rs_raw_signal[i][drawer][k][gain]->StandardDeviation();
          
          //-- V.Giangiobbe : save the average charge and variance in slices of m_eventsPerSlice=1000
          if(m_pisaMethod2){
            for(int iSlice=0; iSlice<rs_signal[i][drawer][k][gain]->GetNSlices(); ++iSlice){
              if(iSlice>=100) continue;
              m_mean_slice[i][drawer][k][iSlice][gain]     = rs_signal[i][drawer][k][gain]->Mean(iSlice);
              m_variance_slice[i][drawer][k][iSlice][gain] = rs_signal[i][drawer][k][gain]->Variance(iSlice);
            } // FOR
          } // IF
          
          if(m_LASERII){
            for(int chan=0; chan<NCHANNELS; ++chan){
              m_ratio_LASERII[chan][i][drawer][k][gain]   = rs_ratio_LASERII[chan][i][drawer][k][gain]->Mean();
              m_ratio_S_LASERII[chan][i][drawer][k][gain] = rs_ratio_LASERII[chan][i][drawer][k][gain]->StandardDeviation();
            } // FOR
          } // IF
          else{
            for(int d=0; d<4; ++d){
              m_ratio[d][i][drawer][k][gain]   = rs_ratio[d][i][drawer][k][gain]->Mean();
              m_ratio_S[d][i][drawer][k][gain] = rs_ratio[d][i][drawer][k][gain]->StandardDeviation();
            } // FOR
          } // ELSE
          m_entries[i][drawer][k][gain] = rs_signal[i][drawer][k][gain]->NumDataValues();
        } // FOR
      } // FOR
    } // FOR
  } // FOR
  
  return StatusCode::SUCCESS;
} // FINALIZECALCULATIONS

StatusCode TileLaserDefaultCalibTool::writeNtuple(int runNumber, int runType, TFile * rootFile){
  // CALLED FROM LASERCALIBALG TO STORE CALIBRATION COEFFICIENTS
  // STORES NTUPLE AS ROOT FILE
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "finalize(" << runNumber << "," << runType << "," << rootFile << ")" << endreq;
  
  // CREATE OUTPUT TREE
  m_toolRunNo = runNumber;
  
  TTree *t = new TTree(m_toolNtuple.c_str(), "TileLaserCalib-Ntuple");
  t->Branch("RunNumber",&m_toolRunNo, "runNo/I");
  t->Branch("ADC_status",&m_ADC_problem, "ADC/I");
  t->Branch("WheelPos",&m_las_filter, "wheelpos/I");
  t->Branch("RequestedAmp",&m_las_requ_amp, "requamp/F");
  if(!m_LASERII){
    t->Branch("Humidity",&m_hrate,"humid/F");
    t->Branch("AirFlow",&m_flow,"flow/F");
    t->Branch("HeadTemp",&m_head_temp,"htemp/F");
  } // IF
  t->Branch("TimeRun",&m_las_time, "timeofrun/F");
  
  if(m_LASERII){
    const char* gainnames[2]  = {"HG","LG"};
    const char* channames[16] = {"Diode0","Diode1","Diode2","Diode3","Diode4","Diode5","Diode6","Diode7",
      "Diode8","Diode9","PMT1","ExtCIS0","IntCIS","DiodePhocal","PMT2","ExtCIS1"};
    for(int chan=0;chan<NCHANNELS;++chan){
      t->Branch(Form("%s_%s_Signal",gainnames[chan%2],channames[chan/2]),&m_chan_LAS[chan],Form("%s_%s_Signal/F",gainnames[chan%2],channames[chan/2]));
//      t->Branch(Form("%s_%s_Signal_Pedestal",gainnames[chan%2],channames[chan/2]),&m_chan_PED[chan],Form("%s_%s_Signal_Pedestal/F",gainnames[chan%2],channames[chan/2]));
//      t->Branch(Form("%s_%s_Signal_LED",gainnames[chan%2],channames[chan/2]),&m_chan_LED[chan],Form("%s_%s_Signal_LED/F",gainnames[chan%2],channames[chan/2]));
//      t->Branch(Form("%s_%s_Signal_Alpha",gainnames[chan%2],channames[chan/2]),&m_chan_APH[chan],Form("%s_%s_Signal_Alpha/F",gainnames[chan%2],channames[chan/2]));
//      t->Branch(Form("%s_%s_Signal_Linearity",gainnames[chan%2],channames[chan/2]),&m_chan_LIN[chan],Form("%s_%s_Signal_Linearity/F",gainnames[chan%2],channames[chan/2]));
      
      t->Branch(Form("%s_%s_Sigma",gainnames[chan%2],channames[chan/2]),&m_chan_S_LAS[chan],Form("%s_%s_Sigma/F",gainnames[chan%2],channames[chan/2]));
//      t->Branch(Form("%s_%s_Sigma_Pedestal",gainnames[chan%2],channames[chan/2]),&m_chan_S_PED[chan],Form("%s_%s_Sigma_Pedestal/F",gainnames[chan%2],channames[chan/2]));
//      t->Branch(Form("%s_%s_Sigma_LED",gainnames[chan%2],channames[chan/2]),&m_chan_S_LED[chan],Form("%s_%s_Sigma_LED/F",gainnames[chan%2],channames[chan/2]));
//      t->Branch(Form("%s_%s_Sigma_Alpha",gainnames[chan%2],channames[chan/2]),&m_chan_S_APH[chan],Form("%s_%s_Sigma_Alpha/F",gainnames[chan%2],channames[chan/2]));
//      t->Branch(Form("%s_%s_Sigma_Linearity",gainnames[chan%2],channames[chan/2]),&m_chan_S_LIN[chan],Form("%s_%s_Sigma_Linearity/F",gainnames[chan%2],channames[chan/2]));
//      
      t->Branch(Form("%s_%s_Ped",gainnames[chan%2],channames[chan/2]),&m_chan_Ped[chan],Form("%s_%s_Ped/F",gainnames[chan%2],channames[chan/2]));
      t->Branch(Form("%s_%s_Led",gainnames[chan%2],channames[chan/2]),&m_chan_Led[chan],Form("%s_%s_Led/F",gainnames[chan%2],channames[chan/2]));
      t->Branch(Form("%s_%s_Alpha",gainnames[chan%2],channames[chan/2]),&m_chan_Alpha[chan],Form("%s_%s_Alpha/F",gainnames[chan%2],channames[chan/2]));
//      t->Branch(Form("%s_%s_Ped1",gainnames[chan%2],channames[chan/2]),&m_chan_Lin[chan],Form("%s_%s_Ped1/F",gainnames[chan%2],channames[chan/2]));
      t->Branch(Form("%s_%s_Ped_Sigma",gainnames[chan%2],channames[chan/2]),&m_chan_SPed[chan],Form("%s_%s_Ped_Sigma/F",gainnames[chan%2],channames[chan/2]));
      t->Branch(Form("%s_%s_Led_Sigma",gainnames[chan%2],channames[chan/2]),&m_chan_SLed[chan],Form("%s_%s_Led_Sigma/F",gainnames[chan%2],channames[chan/2]));
      t->Branch(Form("%s_%s_Alpha_Sigma",gainnames[chan%2],channames[chan/2]),&m_chan_SAlpha[chan],Form("%s_%s_Alpha_Sigma/F",gainnames[chan%2],channames[chan/2]));
//      t->Branch(Form("%s_%s_Ped1_Sigma",gainnames[chan%2],channames[chan/2]),&m_chan_SLin[chan],Form("%s_%s_Ped1_Sigma/F",gainnames[chan%2],channames[chan/2]));
    } // FOR
    
    t->Branch("Ratio",*m_ratio_LASERII,"ratio[32][4][64][48][2]/F");
    t->Branch("Sigma_Ratio",*m_ratio_S_LASERII,"ratio_S[32][4][64][48][2]/F");
  } // IF
  else{
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
  } // ELSE
  
  t->Branch("MeanTime",m_meantime,"meantime[4]/F");
  t->Branch("Time",*m_time,"time[4][64][48][2]/F");
  t->Branch("Sigma_Time",*m_time_S,"time_s[4][64][48][2]/F");
  t->Branch("Signal",*m_mean,"signal[4][64][48][2]/F");
  t->Branch("Sigma_Signal",*m_mean_S,"signal_s[4][64][48][2]/F");
  t->Branch("Raw_Signal",*m_raw_mean,"rawsignal[4][64][48][2]/F");
  t->Branch("Raw_Sigma_Signal",*m_raw_mean_S,"rawsignal_s[4][64][48][2]/F");
  t->Branch("LaserEntries",*m_entries,"LASER_entries[4][64][48][2]/I");
  t->Branch("Kappa",*m_kappa,"Kappa[4][64][48][2]/F");
  t->Branch("Status",*m_status,"Status[4][64][48][2]/S");
  t->Branch("HV",*m_HV,"HV[4][64][48]/F");
  t->Branch("HVSet",*m_HVSet,"HVSet[4][64][48]/F");
  
  if(m_pisaMethod2){
    t->Branch("MeanSlice",*m_mean_slice,"MeanSlice[4][64][48][100][2]/F");
    t->Branch("VarianceSlice",*m_variance_slice,"VarianceSlice[4][64][48][100][2]/F");
  } // IF
  
  // FILL VALUES FOR THIS RUN
  t->Fill();
  t->Write();
  
  return StatusCode::SUCCESS;
} // WRITENTUPLE

StatusCode TileLaserDefaultCalibTool::finalize(){
  // STORE HIGH VOLTAGE (HV) AND FINALIZE CALCULATIONS
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "finalize()" << endreq;
  
  StatusCode sc = TileLaserDefaultCalibTool::finalizeCalculations(); // Perform the analysis
  
  for(int part=0; part<4; ++part){
    int ros = part+1;
    for(int drawer=0; drawer<64; ++drawer){
      int module = drawer+1;
      for(int channel=0; channel<48; ++channel){
        int pmt = abs(m_cabling->channel2hole(ros,channel));
        
        m_HV[part][drawer][channel] = m_tileDCSSvc->getDCSHV(ros, module, pmt);
        m_HVSet[part][drawer][channel] = m_tileDCSSvc->getDCSHVSET(ros, module, pmt);
      } // FOR
    } // FOR
  } // FOR
  
  if(sc.isFailure()){
    log << MSG::ERROR
    << "Failure in DefaultLaserTool finalization!"
    << endreq;
    return StatusCode::FAILURE;
  } // IF
  
  return StatusCode::SUCCESS;
} // FINALIZE

std::pair<unsigned int, unsigned int> TileLaserDefaultCalibTool::getCoupleOfChan(int ros, int couple){
  // GET CHANNEL COUPLES THAT ARE ON THE SAME CLEAR FIBER
  std::pair<unsigned int, unsigned int> coupleOfChannels;
  
  int chan1LB[22]={0, 1, 4, 5, 8,  9,  12, 13, 16, 17, 20, 21, 26, 25, 28, 27, 38, 35, 40, 37, 42, 47};
  int chan2LB[22]={2, 3, 6, 7, 10, 11, 14, 15, 18, 19, 22, 23, 24, 29, 34, 39, 36, 33, 44, 41, 46, 45};
  
  int chan1EB[22]={0, 1, 4, 5,  8, 9,  12, 13, 16, 17, 22, 23, 30, 35, 41, 40, -1, -1, -1, -1, -1, -1};
  int chan2EB[22]={2, 3, 6, 7, 10, 11, 14, 15, 20, 21, 31, 32, 38, 37, 39, 36, -1, -1, -1, -1, -1, -1};
  
  //--- LB,
  if(ros<2){
    coupleOfChannels.first  = chan1LB[couple];
    coupleOfChannels.second = chan2LB[couple];
  } // IF
  
  //--- EB
  if(ros>=2){
    coupleOfChannels.first  = chan1EB[couple];
    coupleOfChannels.second = chan2EB[couple];
  } // IF
  return coupleOfChannels;
} // COUPLEOFCHANNELS

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LArRODMonAlg.cxx
// PACKAGE:  LArMonitoring
//
// ********************************************************************

#include "LArRODMonAlg.h"

#include "Identifier/IdentifierHash.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "TMath.h"
#include "CaloIdentifier/CaloGain.h"
#include "xAODEventInfo/EventInfo.h"
#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannel.h"
#include "LArRawEvent/LArFebHeaderContainer.h"

#include "LArRawEvent/LArFebHeader.h" 

#include "LArCabling/LArOnOffIdMapping.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include "AthenaKernel/errorcheck.h"
#include <cmath>
#include <sys/stat.h>


LArRODMonAlg::LArRODMonAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthMonitorAlgorithm(name, pSvcLocator),
    m_LArOnlineIDHelper(nullptr),
    m_badChannelMask(this),
    m_BC(25000.), // picoseconds time unit
    m_ndump(0),
    m_counter(0),
    m_eventsCounter(0)

{
  /*
  declareProperty("OnlineHistorySize",m_history_size = 20);
  declareProperty("OnlineHistoryGranularity",m_history_granularity = 5);
  */

  /*declareProperty("numberOfLB",m_nb_lb = 2000.);*/

  declareProperty("LArBadChannelMask", m_badChannelMask);
  //declareProperty("IsOnline",m_IsOnline = false);  // exists in the base class


  /*
  m_last_lb = -1;
  m_curr_lb = -1;
  */

}

/*---------------------------------------------------------*/
LArRODMonAlg::~LArRODMonAlg()
{
}

/*---------------------------------------------------------*/
StatusCode 
LArRODMonAlg::initialize() {
  ATH_MSG_VERBOSE( "In LArRODMonAlg::initialize() ");

  ATH_CHECK(detStore()->retrieve(m_LArOnlineIDHelper, "LArOnlineID"));

  ATH_CHECK(m_channelKey_fromDigits.initialize());
  ATH_CHECK(m_channelKey_fromBytestream.initialize());
  ATH_CHECK(m_digitContainerKey.initialize());

  ATH_CHECK(m_keyOFC.initialize());
  ATH_CHECK(m_keyShape.initialize());
  ATH_CHECK(m_keyHVScaleCorr.initialize());
  ATH_CHECK(m_keyPedestal.initialize());

  ATH_CHECK(m_adc2mevKey.initialize());

  ATH_CHECK(m_noiseCDOKey.initialize());
  ATH_CHECK(m_cablingKey.initialize());

  if (m_skipKnownProblematicChannels) { 
    if (!m_badChannelMask.empty()) {
       ATH_CHECK(m_badChannelMask.retrieve());
    } else {
       ATH_MSG_WARNING("Disabling bad channel masking !!!");
       m_badChannelMask.disable();
    }
  }
  
  // Open output files for DspTest
  std::ofstream fai;
  std::ofstream fdig;
  std::ofstream fen;
  std::ofstream fdump;
  if (m_doDspTestDump) {
    fai.open(m_AiFileName,std::ios_base::app);
    fdig.open(m_DigitsFileName,std::ios_base::app);
    fen.open(m_EnergyFileName,std::ios_base::app);
  }

  // Output file
  if (m_doCellsDump) {
    fdump.open(m_DumpCellsFileName,std::ios_base::app);
    fdump<<"febid        channel CellID       slot   FT     barrel_ec posneg partition E_off     E_on     T_off     T_on     Q_off     Q_on     event   "<<std::endl;
    fdump.close();
  }

  ATH_MSG_DEBUG("Setting an offset time of " << m_timeOffset << " BC, i.e. " << m_timeOffset*m_BC << " ps");


  m_histoGroups = Monitored::buildToolMap<int>(m_tools,m_MonGroupName, m_partitions);

  return AthMonitorAlgorithm::initialize();
}

StatusCode LArRODMonAlg::fillHistograms(const EventContext& ctx) const {
  ATH_MSG_VERBOSE( "In LArRODMonAlg::fillHistograms()");

  // Increment event counter
  m_eventsCounter++;

  /*
  // Expected precision for energy calculation, depending on energy (ranges) :
  // Between -213 and 213 MeV (~ 8 GeV)        2**0=1 MeV precision (range 0)
  // Between -216 and 216 MeV (~ 64 GeV)       2**3=8 MeV precision (range 1)
  // Between -219 and 219 MeV (~ 512 GeV)      2**6=64 MeV precision (range 2)
  // Between -222 and 222 MeV (~ 4 TeV)        2**9=512 MeV precision (range 3)
  */

 //monitored variables
  auto weight_e = Monitored::Scalar<float>("weight_e",1.);
  auto weight_q = Monitored::Scalar<float>("weight_q",1.);
  auto weight_t = Monitored::Scalar<float>("weight_t",1.);
  auto numE = Monitored::Scalar<int>("numE",1.);
  auto numQ = Monitored::Scalar<int>("numQ",1.);
  auto numT = Monitored::Scalar<int>("numT",1.);
  auto gain = Monitored::Scalar<int>("gain",-1);
  auto partition = Monitored::Scalar<int>("partition",-1);
  auto partitionI = Monitored::Scalar<int>("partitionI",-1);
  auto lb = Monitored::Scalar<int>("LBN",0);
  auto sweetc = Monitored::Scalar<float>("Sweetc",1.);

  SG::ReadHandle<xAOD::EventInfo> thisEventInfo{GetEventInfo(ctx)};

  SG::ReadCondHandle<ILArPedestal>    pedestalHdl{m_keyPedestal, ctx};
  const ILArPedestal* pedestals=*pedestalHdl;

  bool isEventFlaggedByLArNoisyROAlg = false; // keep default as false
  bool isEventFlaggedByLArNoisyROAlgInTimeW = false; // keep deault as false

  if ( thisEventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,0) ) {
    isEventFlaggedByLArNoisyROAlg = true;
    ATH_MSG_DEBUG( " !!! Noisy event found from LArNoisyROAlg !!!" );
  }

  if ( thisEventInfo->isEventFlagBitSet(xAOD::EventInfo::LAr,3) ) {
    isEventFlaggedByLArNoisyROAlgInTimeW = true;
    ATH_MSG_DEBUG( " !!! Noisy event found by LArNoisyROAlg in Time window of 500ms!!!" );
  }
  
  // Noise bursts cleaning (LArNoisyRO_Std or TimeWindowVeto) added by B.Trocme - 19/7/12
  if (m_removeNoiseBursts && (isEventFlaggedByLArNoisyROAlg || isEventFlaggedByLArNoisyROAlgInTimeW)) return StatusCode::SUCCESS;
  
  // Retrieve stream info
  int nStreams = m_streams.size();
   //  if ((nStreams == 1 && m_streams[0] == "all") || nStreams <= 0) selectstreams = false; 
 
  lb = GetEventInfo(ctx)->lumiBlock(); 
  
  //Fixme: Use LArTrigStreamMatching also here.
  const int streamsize = nStreams + 1;
  std::vector<int> hasStream(streamsize,0);
  //  for (int str = 0; str < nStreams + 1; str++) hasStream[str] = 0;

  bool hasstrlist = false;
  const std::vector< xAOD::EventInfo::StreamTag >& evtStreamTags=thisEventInfo->streamTags();
  for (const auto& evtStreamTag : evtStreamTags) {
    std::vector<std::string>::const_iterator evtStreamTagIt=std::find(m_streams.begin(),m_streams.end(),evtStreamTag.name());
    if (evtStreamTagIt!=m_streams.end()) {
      const unsigned str=evtStreamTagIt-m_streams.begin();
      ATH_MSG_VERBOSE( "Keeping Stream Tag: " << evtStreamTag.type() << "_" << evtStreamTag.name());
      hasStream[str] = 1;
      hasstrlist = true;
    }
  }
  if (! hasstrlist) hasStream[nStreams] = 1; 


  SG::ReadHandle<LArRawChannelContainer> rawColl_fromDigits(m_channelKey_fromDigits, ctx);

  SG::ReadHandle<LArRawChannelContainer> rawColl_fromBytestream(m_channelKey_fromBytestream, ctx);
  
  SG::ReadHandle<LArDigitContainer> pLArDigitContainer(m_digitContainerKey, ctx);

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey, ctx};
  const LArOnOffIdMapping* cabling=*cablingHdl;

  SG::ReadCondHandle<ILArOFC>         ofcHdl{m_keyOFC, ctx};
  const ILArOFC* ofcs=*ofcHdl;

  SG::ReadCondHandle<ILArShape>       shapeHdl{m_keyShape, ctx};
  const ILArShape* shapes=*shapeHdl;

  SG::ReadCondHandle<ILArHVScaleCorr> hvScaleCorrHdl{m_keyHVScaleCorr, ctx};
  const ILArHVScaleCorr* hvScaleCorrs=*hvScaleCorrHdl;

  SG::ReadCondHandle<LArADC2MeV> adc2MeVHdl{m_adc2mevKey, ctx};
  const LArADC2MeV* adc2mev=*adc2MeVHdl;

  std::set<HWIdentifier> ignoreFEBs;

  if (m_doCheckSum || m_doRodStatus) {
     const LArFebHeaderContainer* febCont=NULL;
     StatusCode sc = evtStore()->retrieve(febCont);
     if (sc.isFailure() || !febCont) {
       ATH_MSG_WARNING( "No LArFEB container found in TDS" ); 
     } else {
       LArFebHeaderContainer::const_iterator itFEB = febCont->begin(); 
       LArFebHeaderContainer::const_iterator itFEB_e = febCont->end(); 
       for ( ; itFEB!=itFEB_e;++itFEB) {
         if (((m_doCheckSum && (*itFEB)->ChecksumVerification()==false)) || 
            (m_doRodStatus && (*itFEB)->RodStatus()!=0)) 
           ignoreFEBs.insert((*itFEB)->FEBId());
       }
     }
     ATH_MSG_DEBUG("Found " << ignoreFEBs.size() << " FEBs with checksum errors or statatus errors. Will ignore these FEBs.");
  }

  // Open output files for DspTest
  std::ofstream fai;
  std::ofstream fdig;
  std::ofstream fen;
  std::ofstream fdump;
  if (m_doDspTestDump) {
    fai.open(m_AiFileName); fai.put(' '); fai.close();
    fdig.open(m_DigitsFileName); fdig.put(' '); fdig.close();
    fen.open(m_EnergyFileName); fen.put(' '); fen.close();
  }
  if (m_doCellsDump) {

    fdump.open(m_DumpCellsFileName,std::ios_base::app);
    fdump<<"febid        channel CellID       slot   FT     barrel_ec posneg partition E_off     E_on     T_off     T_on     Q_off     Q_on     event   "<<std::endl;
    fdump.close();
  }


  std::vector<ERRCOUNTER> errcounters(N_PARTITIONS);
  
  std::vector<unsigned> errsPerFEB;
  errsPerFEB.resize(m_LArOnlineIDHelper->febHashMax(),0);

  const bool ignoreFebs=(ignoreFEBs.size()>0);
  std::set<HWIdentifier>::const_iterator ignoreFebsEnd=ignoreFEBs.end();
  
  unsigned count_gain[(int)CaloGain::LARNGAIN];

  //Build an assoiciation of channels in the two LArRawChannelContainers.
  //The LArRawChannelContainers are unordered collections of LArRawChannels. 
  //But we know that they have the same order because they were built from the same source (namely the LArDigits and RawChannels in the Bytestream)
  //and we know that the LArRawChannels built offline are a subset of the LArRawChannelContainers read from Bytestream.
  //Therfore we can search much more efficiently
  LArRawChannelContainer::const_iterator rcDigIt=rawColl_fromDigits->begin();
  LArRawChannelContainer::const_iterator rcDigIt_e=rawColl_fromDigits->end();
  LArRawChannelContainer::const_iterator rcBSIt=rawColl_fromBytestream->begin();
  LArRawChannelContainer::const_iterator rcBSIt_e=rawColl_fromBytestream->end();

  //Loop over indices in LArRawChannelContainer built offline (the small one)
  ATH_MSG_DEBUG( "Entering the LArRawChannel loop." );

  short minSamples;
  short maxSamples;

  for (;rcDigIt!=rcDigIt_e;++rcDigIt) {
    const HWIdentifier idDig=rcDigIt->hardwareID();
    const HWIdentifier febId=m_LArOnlineIDHelper->feb_Id(idDig);
    // Check if this FEB should be ignored
    if (ignoreFebs) { 
      if (ignoreFEBs.find(febId)!=ignoreFebsEnd) continue;
    }
    //Check if this is a bad channel
    if (m_skipKnownProblematicChannels && m_badChannelMask->cellShouldBeMasked(idDig)) continue;

    const CaloGain::CaloGain gain = rcDigIt->gain();
    //Check pedestal if needed
    if (m_skipNullPed) {
      const float ped = pedestals->pedestal(idDig,gain);
      if(ped <= (1.0+LArElecCalib::ERRORCODE)) continue;
    }

    //Now look for corresponding channel in the LArRawChannelContainer read from Bytestream (the big one)
    LArRawChannelContainer::const_iterator currIt=rcBSIt; //Remember current position in container
    for (;rcBSIt!=rcBSIt_e && rcBSIt->hardwareID() != idDig; ++rcBSIt);
    if (rcBSIt==rcBSIt_e) {
      ATH_MSG_WARNING( "LArRawChannelContainer not in the expected order. Change of LArByteStream format?" );
      //Wrap-around
      for (rcBSIt=rawColl_fromBytestream->begin();rcBSIt!=currIt && rcBSIt->hardwareID() != idDig; ++rcBSIt);
      if (rcBSIt==currIt) {
	ATH_MSG_ERROR( "Channel " << m_LArOnlineIDHelper->channel_name(idDig) << " not found." );
	return StatusCode::FAILURE;
      }
    }

    bool doDspTestDump =  m_doDspTestDump;
    bool doCellsDump =  m_doCellsDump;

    const LArDigit* dig=NULL;
    unsigned index=rcDigIt-rawColl_fromDigits->begin();
    const unsigned digContSize=pLArDigitContainer->size();
    for(;index<digContSize && pLArDigitContainer->at(index)->hardwareID()!=idDig;++index);
    if (index==digContSize) {
      ATH_MSG_ERROR( "Can't find LArDigit corresponding to channel " << m_LArOnlineIDHelper->channel_name(idDig) << ". Turn off digit dump" );
      doDspTestDump=false;
      doCellsDump=false;
    }
    else{
      dig=pLArDigitContainer->at(index);
    }
    minSamples = 4095;
    maxSamples = 0;
    if (dig){
      const std::vector<short>& samples=dig->samples();
      for (unsigned int k = 0; k<samples.size(); k++) {
	if (samples.at(k) > maxSamples) maxSamples = samples.at(k);
	if (samples.at(k) < minSamples) minSamples = samples.at(k);
      }
    }

    if ((maxSamples-minSamples) > m_adc_th || m_adc_th <= 0) {
      compareChannels(cabling, ofcs, shapes,
                      hvScaleCorrs, pedestals, adc2mev,
                      idDig,
                      errsPerFEB, errcounters,
                      (*rcDigIt),(*rcBSIt),
                      doDspTestDump, doCellsDump, 
                      fai, fdig, fen, fdump,
                      dig).ignore();
      ++count_gain[gain];
    }
    else {
      if (dig) ATH_MSG_DEBUG( "Samples : "<< maxSamples << " " << minSamples );
    }      

  }//end loop over rawColl_fromDigits
  ATH_MSG_DEBUG( "End of rawChannels loop" );


  
  for (unsigned i=0;i<m_LArOnlineIDHelper->febHashMax();++i) {
    const HWIdentifier febid=m_LArOnlineIDHelper->feb_Id(i);
    const std::string pn=getPartitionName(febid);
    sweetc = errsPerFEB[i];
    fill(m_tools[m_histoGroups.at(pn)],sweetc);
  }

  ERRCOUNTER allEC;  
  unsigned allErr_E=0;
  unsigned allErr_T=0;
  unsigned allErr_Q=0;

  for (unsigned g=0;g<3;++g) {
    for (unsigned p=0;p<N_PARTITIONS;++p) {
      allEC.errors_E[g]+=errcounters[p].errors_E[g];
      allEC.errors_T[g]+=errcounters[p].errors_T[g];
      allEC.errors_Q[g]+=errcounters[p].errors_Q[g];
    }
    allErr_E+=allEC.errors_E[g];
    allErr_T+=allEC.errors_T[g];
    allErr_Q+=allEC.errors_Q[g];
  }



  ATH_MSG_VERBOSE( "*Number of errors in Energy Computation : " );
  ATH_MSG_VERBOSE( "*     Low Gain : " << allEC.errors_E[2] << " / " << count_gain[CaloGain::LARLOWGAIN] );
  ATH_MSG_VERBOSE( "*     Medium Gain : " << allEC.errors_E[1] << " / " << count_gain[CaloGain::LARMEDIUMGAIN] );
  ATH_MSG_VERBOSE( "*     High Gain : " <<  allEC.errors_E[0] << " / " << count_gain[CaloGain::LARHIGHGAIN] );
  ATH_MSG_VERBOSE( "*Number of errors in Time Computation : " );
  ATH_MSG_VERBOSE( "*     Low Gain : " <<  allEC.errors_T[2] << " / " << count_gain[CaloGain::LARLOWGAIN] );
  ATH_MSG_VERBOSE( "*     Medium Gain : " <<  allEC.errors_T[1] <<  " / " << count_gain[CaloGain::LARMEDIUMGAIN] );
  ATH_MSG_VERBOSE( "*     High Gain : " <<  allEC.errors_T[0] << " / " << count_gain[CaloGain::LARHIGHGAIN] );
  ATH_MSG_VERBOSE( "*Number of errors in Quality Computation : " );
  ATH_MSG_VERBOSE( "*     Low Gain : " <<  allEC.errors_Q[2] << " / " << count_gain[CaloGain::LARLOWGAIN] );
  ATH_MSG_VERBOSE( "*     Medium Gain : " <<  allEC.errors_Q[1] << " / " << count_gain[CaloGain::LARMEDIUMGAIN] );
  ATH_MSG_VERBOSE( "*     High Gain : " <<  allEC.errors_Q[0] << " / " << count_gain[CaloGain::LARHIGHGAIN] );
  

  for (unsigned p=0;p<N_PARTITIONS;++p) {
    unsigned allErrsPartE=0;
    unsigned allErrsPartT=0;
    unsigned allErrsPartQ=0;
    partition = p;
    for (unsigned g=0;g<3;++g) {
      
      gain = g;
      weight_e = (float)errcounters[p].errors_E[g];
      weight_q = (float)errcounters[p].errors_Q[g];
      weight_t = (float)errcounters[p].errors_T[g];

      fill(m_MonGroupName, partition, gain, weight_e, weight_q, weight_t);

      allErrsPartE+=errcounters[p].errors_E[g];
      allErrsPartT+=errcounters[p].errors_T[g];
      allErrsPartQ+=errcounters[p].errors_Q[g];
    }
    partitionI = p;
    numE = (float)allErrsPartE;
    numT = (float)allErrsPartT;
    numQ = (float)allErrsPartQ;
    fill(m_MonGroupName, lb, partitionI, numE, numT, numQ); 
  }

  for(int str = 0; str < nStreams + 1; str++) {
    if (hasStream[str] == 1) {
       /*
      m_hEErrors_LB_stream->Fill((float)m_curr_lb,(float)str,(float)allErr_E);
      m_hTErrors_LB_stream->Fill((float)m_curr_lb,(float)str,(float)allErr_T);
      m_hQErrors_LB_stream->Fill((float)m_curr_lb,(float)str,(float)allErr_Q);
      FIXME */
    }
  }

  if (m_doDspTestDump) {
    fai.close();
    fdig.close();
    fen.close();
  }
  if (m_doCellsDump) {
    fdump.close();
  }
  return StatusCode::SUCCESS;
}

void LArRODMonAlg::DumpCellEvent(int count,                            // counter
				  ILArOFC::OFCRef_t ofc,                 // vector containing OFC's
				  ILArOFC::OFCRef_t ofcb,                // vector containing OFCb's
				  ILArShape::ShapeRef_t ofch,            // vector containing OFCh's
				  ILArShape::ShapeRef_t ofcd,            // vector containing OFCd's
				  float Escale,                         // ADC2MeV factor
				  float Ramp0,                         // ramp intercept
				  float pedestal,                       // pedestal
				  const std::vector<short>* digits,     // vector containing samples (digits)
				  int E_off,                            // Energy calculated offline (i.e. fromDigits)
				  int E_on,                             // Energy calculated online (i.e. fromBytestream)
				  std::ofstream &ofcfile,                    // output file containing ofc's
				  std::ofstream &digitsfile,                 // output file containing digits
				  std::ofstream &energyfile,                 // output file containing energies
				  HWIdentifier chid,                    // Channel HW ID
				  int event) const                       // Event #
{
  // Dumping file with coefficients
  const int channel=m_LArOnlineIDHelper->channel(chid);
  const HWIdentifier febid=m_LArOnlineIDHelper->feb_Id(chid);
  ATH_MSG_VERBOSE("Dumping OFC, energies and digits for cell " << channel << " of FEB " << febid);

  float sumai = 0.;
  for (unsigned int a = 0; a<ofc.size(); a++) {
    sumai += ofc.at(a);
  }
  float pedplusoffset;
  if (Escale*sumai != 0) pedplusoffset = pedestal - Ramp0/(Escale*sumai);
  else pedplusoffset = 0;
  const float inv_Escale = 1. / Escale;

  ofcfile<< channel<<"\t"<<  pedestal<<"\t"<< pedplusoffset<<"\t"<<  ofc.at(0)*Escale<<"\t"<<  ofc.at(1)*Escale<<"\t"<<  ofc.at(2)*Escale<<"\t"<<  ofc.at(3)*Escale<<"\t"<<  ofc.at(4)*Escale<<"\t"<< ofcb.at(0)*Escale<<"\t"<<  ofcb.at(1)*Escale<<"\t"<<  ofcb.at(2)*Escale<<"\t"<<  ofcb.at(3)*Escale<<"\t"<<  ofcb.at(4)*Escale<<"\t"<< ofch.at(0)*inv_Escale<<"\t"<<  ofch.at(1)*inv_Escale<<"\t"<<  ofch.at(2)*inv_Escale<<"\t"<<  ofch.at(3)*inv_Escale<<"\t"<<  ofch.at(4)*inv_Escale<<"\t"<< ofcd.at(0)*inv_Escale<<"\t"<<  ofcd.at(1)*inv_Escale<<"\t"<<  ofcd.at(2)*inv_Escale<<"\t"<<  ofcd.at(3)*inv_Escale<<"\t"<<  ofcd.at(4)*inv_Escale << std::endl; 

  // Dumping file with offline energy and online energ
  energyfile << count << "   " << E_off << "   " << E_on ;
  energyfile << "     // FEB " << febid.get_identifier32().get_compact() << " ( channel " << channel << " ), event " << event << std::endl;
 
  // Dumping file with digits
  digitsfile << channel << "   ";
  for (unsigned int k = 0; k<digits->size(); k++) {
    digitsfile << digits->at(k) << " ";
  }
  digitsfile << "     // FEB " << febid.get_identifier32().get_compact() << " ( channel " << channel << " ), event " << event << std::endl;
}

void LArRODMonAlg::DumpCellInfo(HWIdentifier chid,                    // FEB HW ID 
				 //int channel,                           // channel #
				 //HWIdentifier CellID,                   // Channel HW ID
				 //int slot,                              // slot 
				 //int FT,                                // Feedthrough
				 //int barrel_ec,                         
				 //int posneg, 
				 //std::string part,
				 const std::string & partc,
				 int gain,
				 int event, 
				 int E_off,                             // Energy calculated offline (i.e. fromDigits)  
				 int E_on,                              // Energy calculated online (i.e. fromBytestream)
				 float T_off,                             // Time calculated offline (i.e. fromDigits)  
				 float T_on,                              // Time calculated online (i.e. fromBytestream)
				 float Q_off,                             // Quality Factor calculated offline (i.e. fromDigits)  
				 float Q_on,                              // Quality Factor calculated online (i.e. fromBytestream)
				 std::ofstream &dumpfile) const
{

  // Dumping cell information in a text file so that it can be compared to unhappy channels lists for instance ...

  const int barrel_ec=m_LArOnlineIDHelper->barrel_ec(chid);
  const int posneg=m_LArOnlineIDHelper->pos_neg(chid);
  const int slot = m_LArOnlineIDHelper->slot(chid);
  const int FT = m_LArOnlineIDHelper->feedthrough(chid);
  const int channel =  m_LArOnlineIDHelper->channel(chid);
  const HWIdentifier febid= m_LArOnlineIDHelper->feb_Id(chid);
  

  dumpfile<<"0x%8x     "<<febid.get_identifier32().get_compact() <<"%3d     "<<channel <<"0x%8x     "<<chid.get_identifier32().get_compact() <<"%2d     "<<slot <<"%2d     "<<FT <<"%2d     "<<barrel_ec <<"%2d     "<<posneg <<"%7s     "<<partc <<"%3d     "<<gain <<"%5d     "<<E_off <<"%5d     "<<E_on <<"%5f     "<<T_off <<"%5f     "<<T_on <<"%5f     "<<Q_off <<"%5f     "<<Q_on <<"%3d     "<<event<<std::endl;
}

StatusCode LArRODMonAlg::finalize() {
  ATH_MSG_VERBOSE("Finalize LArRODMonAlg");
  closeDumpfiles();
  return StatusCode::SUCCESS;
} 
 
void LArRODMonAlg::closeDumpfiles() {
  // Close output files
}

StatusCode LArRODMonAlg::compareChannels( const LArOnOffIdMapping* cabling,
                                          const ILArOFC* ofcs,
                                          const ILArShape* shapes,
                                          const ILArHVScaleCorr* hvScaleCorrs,
                                          const ILArPedestal* pedestals,
                                          const LArADC2MeV* adc2mev,
                                          const HWIdentifier chid,
                                          std::vector<unsigned> &errsPerFEB,
                                          std::vector<ERRCOUNTER> &errcounters,
                                          const LArRawChannel& rcDig, const LArRawChannel& rcBS, 
                                          bool doDspTestDump, bool doCellsDump,
                                          std::ofstream &ofcfile,               
		                          std::ofstream &digitsfile,            
	         	                  std::ofstream &energyfile,
                                          std::ofstream &dumpfile,
                                          const LArDigit* dig) const {

  ATH_MSG_DEBUG( " I am entering compareChannels method" );

  const int slot_fD = m_LArOnlineIDHelper->slot(chid);
  const  int feedthrough_fD = m_LArOnlineIDHelper->feedthrough(chid);
  const float timeOffline = rcDig.time()/m_unit_offline - m_timeOffset*m_BC;

  const unsigned q_gain = (int)rcDig.gain();


  const float& en_fB=rcBS.energy();
  const float& en_fD=rcDig.energy();
  const float abs_en_fB=fabs(en_fB);

  ATH_MSG_VERBOSE(chid.getString()<<" | "<<timeOffline<<" "<<rcBS.time()<<" | "<<en_fB<<" "<<en_fD);

  if (fabs(timeOffline) > m_peakTime_cut*1000.){
    ATH_MSG_DEBUG( " timeOffline too large " << timeOffline );
    return StatusCode::SUCCESS;
  }
// Set the cuts corresponding to the range
  int energyrange, DECut, DTCut, DQCut;
  if (abs_en_fB < m_range_E_0) {
    DECut = m_precision_E_0;
    energyrange=0;
  }
  else if (abs_en_fB < m_range_E_1) {
    DECut = m_precision_E_1;
    energyrange=1;
  }
  else if (abs_en_fB < m_range_E_2) {
    DECut = m_precision_E_2;
    energyrange=2;
  }
  else if (abs_en_fB < m_range_E_3) {
    DECut = m_precision_E_3;
    energyrange=3;
  }
  else { //>= m_range_E_3
    DECut = m_precision_E_max;
    energyrange=4;
  } // not sure of this value ... seems the max, though ...
  auto erange = Monitored::Scalar<int>("Erange",energyrange);

  const float& t_fB=rcBS.time();
  const float abs_t_fB=std::abs(t_fB);
  if (abs_t_fB < m_range_T_0) DTCut = m_precision_T_0;
  else if (abs_t_fB < m_range_T_1) 
    DTCut = m_precision_T_1;
  else if (abs_t_fB < m_range_T_2) 
    DTCut = m_precision_T_2;
  else if (abs_t_fB < m_range_T_3) 
    DTCut = m_precision_T_3;
  else DTCut = m_precision_T_max; // not sure of this value ... seems the max, though ...

  const float q_fB=rcBS.quality();
  const float abs_q_fB=std::abs(q_fB);
  const float q_fD=rcDig.quality();


  if (abs_q_fB < m_range_Q_0) 
    DQCut = m_precision_Q_0;
  else if (abs_q_fB < m_range_Q_1) 
    DQCut = m_precision_Q_1;
  else if (abs_q_fB < m_range_Q_2) 
    DQCut = m_precision_Q_2;
  else if (abs_q_fB < m_range_Q_3) 
    DQCut = m_precision_Q_3;
  else
    DQCut = m_precision_Q_max; // not sure of this value ... seems the max, though ...


  auto DiffE = Monitored::Scalar<float>("Ediff",en_fD - en_fB);
  auto DiffT = Monitored::Scalar<float>("Tdiff",timeOffline - t_fB);

  //Flags
  const bool keepE = true;
  bool keepT = true;
  bool keepQ = true;

  float qdiff = 65535.0; // max possible quality
  if (q_fD > 0.) qdiff = (q_fD - q_fB)/TMath::Sqrt(q_fD);
  auto DiffQ = Monitored::Scalar<float>("Qdiff", qdiff);


  // If q is not calculated online/offline, don't keep event ?
  if (m_skipNullQT && (q_fB <= 0 || q_fD <= 0)) {
    keepT = false;
    keepQ = false;
  } 

  // if overflow for T, skip the event
  if (fabs(timeOffline) > 327680) { // 2**15 * 10 ps
    keepT = false;
  }

  ATH_MSG_VERBOSE(q_fD<<" "<<q_fB<<" | "<<DECut<<" "<<DQCut<<" "<<DTCut<<" | "<<keepE<<" "<<keepQ<<" "<<keepT);

  const PARTITION p=getPartition(chid);
  const std::string & hg=getPartitionName(chid);
  ERRCOUNTER& ec=errcounters[p];
  
  auto slot = Monitored::Scalar<int>("slot",slot_fD);
  auto ft = Monitored::Scalar<int>("FT",feedthrough_fD);

  if (keepE) {//Energy histograms:

    fill(m_MonGroupName,DiffE,erange);

    auto Eon = Monitored::Scalar<float>("Eon",en_fB);
    auto Eoff = Monitored::Scalar<float>("Eoff",en_fD);

    fill(m_tools[m_histoGroups.at(hg)],DiffE,erange,Eon,Eoff);
    
    if ( ((fabs(DiffE) > DECut) || (keepQ && (fabs(DiffQ) > DQCut)))  && dig) {
    // absolute cut on energy 1.MeV
    //if ( ((fabs(DiffE) > 1.) || (keepQ && (fabs(DiffQ) > DQCut)))  && dig) {
      ++ec.errors_E[q_gain];
      auto weight_e = Monitored::Scalar<float>("weight_e",1.);
      fill(m_tools[m_histoGroups.at(hg)],slot,ft,weight_e);

      //adding dumper for online
      if(environment() == Environment_t::online && m_ndump<m_max_dump) {
         SG::ReadCondHandle<CaloNoise> noiseHdl{m_noiseCDOKey};
         const CaloNoise *noisep = *noiseHdl;
         const float hvscale = hvScaleCorrs->HVScaleCorr(chid);
         const int channel=m_LArOnlineIDHelper->channel(chid);
         const HWIdentifier febid=m_LArOnlineIDHelper->feb_Id(chid);
         ATH_MSG_INFO( "Channel: " << channel << " of FEB " << febid );
         if(fabs(DiffE) > DECut ) { 
         // absolute cut on energy 1.MeV
         //if(fabs(DiffE) > 1.) {
             ATH_MSG_INFO( "DSP Energy Error : " << m_LArOnlineIDHelper->channel_name(chid) );
             ATH_MSG_INFO( "   Eonl = " << en_fB << " , Eoff = " << rcDig.energy()
	     		        << " , Eoff - Eonl = " << rcDig.energy() - en_fB );
             ATH_MSG_INFO( "   Qonl = " << q_fB << " , Qoff = " << q_fD
	     		        << " (Qoff - Qnl)/sqrt(Qoff) = " << (q_fD - q_fB)/TMath::Sqrt(q_fD)  );
         }
         if(keepQ && (fabs(DiffQ) > DQCut)) {
             ATH_MSG_INFO( "DSP Quality Error : " << m_LArOnlineIDHelper->channel_name(chid) );
             ATH_MSG_INFO( "   Qonl = " << q_fB << " , Qoff = " << q_fD
	     		        << " (Qoff - Qnl)/sqrt(Qoff) = " << (q_fD - q_fB)/TMath::Sqrt(q_fD)  );
             ATH_MSG_INFO( "   Eonl = " << en_fB << " , Eoff = " << rcDig.energy()
	     		        << " , Eoff - Eonl = " << rcDig.energy() - en_fB );
         }
         ATH_MSG_INFO( " Tauonl = " << rcBS.time() << " , Tauoff = " << rcDig.time() );
         const std::vector<short>& samples=dig->samples();
         std::ostringstream os; 
         os << "Digits : ";
         for (unsigned int k = 0; k<samples.size(); k++) {os << samples.at(k) << " ";}
         ATH_MSG_INFO( os.str() );
         ILArOFC::OFCRef_t this_OFC_a_test = ofcs->OFC_a(chid,rcDig.gain());
         os.clear(); os.seekp(0);
         os << "OFCa : ";
         for (unsigned int k = 0; k<this_OFC_a_test.size(); ++k) {os << this_OFC_a_test.at(k) << " ";}
         ATH_MSG_INFO( os.str() );
         ILArOFC::OFCRef_t this_OFC_b_test = ofcs->OFC_b(chid,rcDig.gain());
         os.clear(); os.seekp(0);
         os << "OFCb : ";
         for (unsigned int k = 0; k<this_OFC_b_test.size(); ++k) {os << this_OFC_b_test.at(k) << " ";}
         ATH_MSG_INFO( os.str() );
         ILArShape::ShapeRef_t this_Shape_test = shapes->Shape(chid,rcDig.gain());
         os.clear(); os.seekp(0);
         os << "Shape : ";
         for (unsigned int k = 0; k<this_Shape_test.size(); ++k) {os << this_Shape_test.at(k) << " ";}
         ATH_MSG_INFO( os.str() );
         ILArShape::ShapeRef_t this_ShapeDer_test = shapes->ShapeDer(chid,rcDig.gain());
         os.clear(); os.seekp(0);
         os << "ShapeDer : ";
         for (unsigned int k = 0; k<this_ShapeDer_test.size(); ++k) {os << this_ShapeDer_test.at(k) << " ";}
         ATH_MSG_INFO( os.str() );
         const  auto polynom_adc2mev=adc2mev->ADC2MEV(chid,rcDig.gain());
         const float escale = (polynom_adc2mev)[1];
         float ramp0 = (polynom_adc2mev)[0];
         if (q_gain == 0) ramp0 = 0.; // no ramp intercepts in HG
         const float ped = pedestals->pedestal(chid,rcDig.gain());
         ATH_MSG_INFO( "Escale: "<<escale<<" intercept: "<<ramp0<<" pedestal: "<<ped<<" gain: "<<rcDig.gain() );
         const Identifier cellid=cabling->cnvToIdentifier(chid);
         const float noise=noisep->getNoise(cellid,rcDig.gain());
         ATH_MSG_INFO( "Noise for mu=20: "<<noise);
         ATH_MSG_INFO( "HVScaleCorr: "<<hvscale);
         double emon=0.;
	 const unsigned nOFCSamp=std::min(samples.size(),this_OFC_a_test.size());
         for (unsigned k=0; k<nOFCSamp; ++k) emon += (samples.at(k)-ped)*this_OFC_a_test.at(k);
         emon *= escale;
         emon += ramp0;
         ATH_MSG_INFO( "intercept + Escale*Sum[(sample-ped)*OFCa] "<<emon);
       ++m_ndump;
      } // DE cut
    } // dumper
  }// end energy histograms

  if (keepT) { //Time histograms
    auto Ton = Monitored::Scalar<float>("Ton",t_fB);
    auto Toff = Monitored::Scalar<float>("Toff",timeOffline);
    fill(m_tools[m_histoGroups.at(hg)],DiffT,Ton,Toff);

    fill(m_MonGroupName,DiffT);

    if (fabs(DiffT) > DTCut) {
      ++(ec.errors_T[q_gain]);
      auto weight_t = Monitored::Scalar<float>("weight_t",1.);
      fill(m_tools[m_histoGroups.at(hg)],slot,ft,weight_t);
    }
  }   
  
  if (keepQ) { //Quality histograms
    auto Qon = Monitored::Scalar<float>("Qon",q_fB);
    auto Qoff = Monitored::Scalar<float>("Qoff",q_fD);
    fill(m_tools[m_histoGroups.at(hg)],DiffQ,Qon,Qoff);

    fill(m_MonGroupName,DiffQ);

    if (fabs(DiffQ) > DQCut) {
      ++(ec.errors_Q[q_gain]);
      auto weight_q = Monitored::Scalar<float>("weight_q",1.);
      fill(m_tools[m_histoGroups.at(hg)],slot,ft,weight_q);
    }

    const HWIdentifier febId=m_LArOnlineIDHelper->feb_Id(chid);
    const IdentifierHash febHash=m_LArOnlineIDHelper->feb_Hash(febId);
    ++(errsPerFEB[febHash]);
  }

  if (m_printEnergyErrors) {
    if (fabs(rcDig.energy() - en_fB) > DECut) {
      ATH_MSG_DEBUG( "DSP Energy Error : " << m_LArOnlineIDHelper->channel_name(chid) );
      ATH_MSG_DEBUG( "   Eonl = " << en_fB << " , Eoff = " << rcDig.energy()
			<< " , Eoff - Eonl = " << rcDig.energy() - en_fB );
    }
  }
 

  //Detailed text dump if requested:
  if (dig) {
    if ((keepE && fabs(DiffE)>DECut) || (keepT && fabs(DiffT)>DTCut) || (keepQ && DiffQ > DQCut)) {
      if (doDspTestDump) {
	int Ind = 0;
	const float timeBinWidth_test=ofcs->timeBinWidth(chid,q_gain);
	unsigned delayIdx_test=(unsigned)floor(0.5+Ind/timeBinWidth_test);
	ILArOFC::OFCRef_t this_OFC_a_test = ofcs->OFC_a(chid,q_gain,delayIdx_test);
	ILArOFC::OFCRef_t this_OFC_b_test = ofcs->OFC_b(chid,q_gain,delayIdx_test);
	ILArShape::ShapeRef_t this_OFC_h_test = shapes->Shape(chid,q_gain,delayIdx_test);
	ILArShape::ShapeRef_t this_OFC_d_test = shapes->ShapeDer(chid,q_gain,delayIdx_test);	
	const std::vector<short>& samples=dig->samples();
	const auto polynom_adc2mev=adc2mev->ADC2MEV(chid,q_gain);
	const float escale = (polynom_adc2mev)[1];
	float ramp0 = (polynom_adc2mev)[0];
	if (q_gain == 0) ramp0 = 0.; // no ramp intercepts in HG
	const float ped = pedestals->pedestal(chid,q_gain);
	this->DumpCellEvent((int)m_counter,this_OFC_a_test,this_OFC_b_test,this_OFC_h_test,this_OFC_d_test,escale,ramp0,ped,&samples,(int)(rcDig.energy()),(int)(en_fB),ofcfile,digitsfile,energyfile, chid, (int)m_eventsCounter);
      }
      if (doCellsDump) {
	this->DumpCellInfo(chid,hg,(int)q_gain,(int)m_eventsCounter,(int)rcDig.energy(),(int)(en_fB),rcDig.time(),t_fB,q_fD,q_fB,dumpfile);
      }
      //++m_counter;

    }//end if E,t or Q cut passed
  }//end if dig
  ATH_MSG_DEBUG( " I am leaving compareChannels method" );
  return StatusCode::SUCCESS;
}


void LArRODMonAlg::ERRCOUNTER::clear() {
  for (unsigned i=0;i<3;++i) {
    errors_E[i]=0;
    errors_T[i]=0;
    errors_Q[i]=0;
  }
  return;
}

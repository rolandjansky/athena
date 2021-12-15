/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArOFC2Ntuple.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArIdentifier/LArOnlineID.h"
#include "StoreGate/ReadCondHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"

LArOFC2Ntuple::LArOFC2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  LArCond2NtupleBase(name, pSvcLocator)
{
  declareProperty("Nsamples",     m_nSamples = 5);
  declareProperty("NtupleName",   m_ntName   = "OFC");
  declareProperty("NtupleFile",   m_ntFile   = "FILE1");
}

LArOFC2Ntuple::~LArOFC2Ntuple() 
{}

StatusCode LArOFC2Ntuple::initialize() {
  m_ntTitle="Optimal Filtering Coefficients";
  m_ntpath=std::string("/NTUPLES/")+m_ntFile+std::string("/")+m_ntName;

  ATH_CHECK( m_ofcKey.initialize() );

  ATH_CHECK( LArCond2NtupleBase::initialize() );
  return StatusCode::SUCCESS;
}

StatusCode LArOFC2Ntuple::stop() {
  const EventContext& ctx = Gaudi::Hive::currentContext();

  // Ntuple booking: Specific
  NTuple::Item<long> gain, phase, nSamples;
  NTuple::Item<float> timeOffset, phasetime;
  NTuple::Array<float> OFCa, OFCb;
  
  ATH_CHECK( m_nt->addItem("Gain",gain,-1,2) );
  ATH_CHECK( m_nt->addItem("TimeOffset",timeOffset,0,100) );
  ATH_CHECK( m_nt->addItem("Phase",phase,0,49) );
  ATH_CHECK( m_nt->addItem("PhaseTime",phasetime,0,800) );
  ATH_CHECK( m_nt->addItem("nSamples",nSamples,0,100) );
  ATH_CHECK( m_nt->addItem("OFCa",m_nSamples,OFCa) );
  ATH_CHECK( m_nt->addItem("OFCb",m_nSamples,OFCb) );
  
  // retrieve OFC object
  const ILArOFC* larOFC=nullptr;
  
  //Try Det-Store (real data, elec-calib case)
  if (detStore()->contains<ILArOFC>(m_ofcKey.key())) {
    ATH_CHECK(detStore()->retrieve(larOFC,m_ofcKey.key()));
  }
  else {//Via ReadCondHandle from CondStore (MC case)
    SG::ReadCondHandle<ILArOFC> larOFCHdl (m_ofcKey, ctx);  
    larOFC=larOFCHdl.cptr();
  }

  const LArOnOffIdMapping *cabling=0;
  if(m_isSC) {
    ATH_MSG_DEBUG( "LArOFC2Ntuple: using SC cabling" );
    SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingSCKey, ctx};
    cabling=*cablingHdl;
  }else{
    SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey, ctx};
    cabling=*cablingHdl;
  }


  if(!cabling) {
     ATH_MSG_WARNING( "Do not have cabling object LArOnOffIdMapping" );
     return StatusCode::FAILURE;
  }

  unsigned cellCounter=0;  
  for ( unsigned igain=CaloGain::LARHIGHGAIN; 
	igain<CaloGain::LARNGAIN ; ++igain )
  {
    for (HWIdentifier chid : m_onlineId->channel_range()) {
      if ( !cabling->isOnlineConnected(chid)) continue;
      ATH_MSG_VERBOSE( "Dumping OFC for channel 0x" << MSG::hex
                       << chid.get_compact() << MSG::dec );
      for (unsigned iphase=0;iphase<larOFC->nTimeBins(chid,igain);iphase++) {
        ILArOFC::OFCRef_t ofc_a=larOFC->OFC_a(chid,igain,iphase);
        //Check if we have OFC for this channel and gain
        //if (!ofc_a.size()) break;//No more phases
        ILArOFC::OFCRef_t ofc_b=larOFC->OFC_b(chid,igain,iphase);
        fillFromIdentifier(chid);
        gain  = (long)igain ;
        phase = (long)iphase ;
        nSamples=ofc_a.size();
        for (int k=0;k<nSamples;k++ ) {
          OFCa[k] = ofc_a[k] ;
          OFCb[k] = ofc_b[k] ;
        }

        timeOffset = larOFC->timeOffset(chid,igain);
        phasetime  = phase*larOFC->timeBinWidth(chid,igain);


        ATH_CHECK( ntupleSvc()->writeRecord(m_nt) );
        cellCounter++;
      }//loop over phases
     }//loop over channels
  }//loop over gains
  ATH_MSG_INFO( "Total number of cells = " << cellCounter );
  ATH_MSG_INFO( "LArOFC2Ntuple has finished." );
  return StatusCode::SUCCESS;
} // end finalize-method.

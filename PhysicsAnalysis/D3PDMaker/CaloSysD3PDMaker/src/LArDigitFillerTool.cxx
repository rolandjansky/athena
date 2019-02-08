/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloSysD3PDMaker/src/LArDigitFillerTool.cxx
 * @author Hong Ma
 * @date Mar 2010
 * @brief Block filler tool for LArDigit
 * adopted from  CBNTAA_LArDigit
 */



#include "LArDigitFillerTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "LArRawEvent/LArDigit.h"
#include "GaudiKernel/IToolSvc.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArRawEvent/LArFebHeaderContainer.h"
#include "LArRawEvent/LArOFIterResultsContainer.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include <vector>

#include "AthenaKernel/errorcheck.h"
#include <sstream>


namespace D3PD {

  /*
LArDigitFillerTool::SortDigits::SortDigits(const LArOnlineID* onlineHelper)
{
  m_onlineHelper=onlineHelper;
}
  */


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
LArDigitFillerTool::LArDigitFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : BlockFillerTool<LArDigit> (type, name, parent),
	m_savesca(false),
        //m_maxchannels(24000),
        m_savedigit(true),
        m_CaloType("EMHECFCAL"),
        m_storeId(false),
	m_em(false),
        m_hec(false),
        m_fcal(false),
        //m_dump_cut(4096),
        m_dump_gain("HIGHMEDIUMLOW"),
        m_dumpIter(false),
        //m_dumpDisc(false),
        m_onlineHelper(0),
        m_emId(0),
        m_hecId(0),
        m_fcalId(0),
        m_cellIndex(0)
{
  declareProperty("SaveDigit",m_savedigit);
  declareProperty("SaveId",m_storeId);
  declareProperty("SaveSCAAddress",m_savesca);
  declareProperty("DumpIterResults",m_dumpIter);

  //declareProperty("DumpDisconnected",m_dumpDisc);
  //declareProperty("DumpGains",m_dump_gain);
  //declareProperty("DumpCut",m_dump_cut);
  //declareProperty("MaxChannels",m_maxChannels);
  //declareProperty("Calo",m_CaloType);

  for (int i = 0; i < CaloGain::LARNGAIN; i++)
    m_gains[i] = false;

  m_layer = 0;
  m_eta = 0;
  m_phi = 0;
  m_gain = 0;
  m_calo = 0;
  m_barrel_ec = 0;
  m_pos_neg = 0;
  m_FT = 0;
  m_slot = 0;
  m_channel = 0;
  m_samples = 0;
  m_sca = 0;
  m_offId = 0;
  m_Itervalid = 0;
  m_Iterconverged = 0;
  m_IterNIter = 0;
  m_IterAmpl = 0;
  m_IterTau = 0;
  m_IterPeakSample = 0;
  m_IterDelay = 0;
  m_IterQual = 0;
  m_IterIdx = 0;
}


/**
 * @brief Book variables for this block.
 */
StatusCode LArDigitFillerTool::book()
{
  ATH_MSG_DEBUG( "in book()" );

  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  m_emId=caloIdMgr->getEM_ID();
  m_hecId=caloIdMgr->getHEC_ID();
  m_fcalId=caloIdMgr->getFCAL_ID();


  CHECK( m_cablingKey.initialize() );
  StoreGateSvc* detStore = 0;
  CHECK( service("DetectorStore", detStore) );

  CHECK( detStore->retrieve(m_onlineHelper, "LArOnlineID") );


  // CHECK(addVariable ("icell",m_cellIndex));
  if (m_savedigit){
    CHECK(addVariable ("layer", m_layer));
    CHECK(addVariable ("ieta",m_eta));
    CHECK(addVariable ("calo", m_calo));
    CHECK(addVariable ("iphi", m_phi));
    CHECK(addVariable ("barrel_ec",m_barrel_ec));
    CHECK(addVariable ("pos_neg", m_pos_neg));
    CHECK(addVariable ("FT", m_FT));
    CHECK(addVariable ("slot", m_slot));
    CHECK(addVariable ("channel", m_channel));
    CHECK(addVariable ("gain",m_gain));
    CHECK(addVariable ("Samples", m_samples));
  }

  if ( m_savesca == true ){
    CHECK(addVariable ("sca_add", m_sca));
  }
  if ( m_storeId ) {
    CHECK(addVariable ("offlineId", m_offId));
  }

  if (m_dumpIter) {
    CHECK(addVariable("iter_valid" , m_Itervalid));
    CHECK(addVariable("iter_converged" , m_Iterconverged));
    CHECK(addVariable("iter_nIter" , m_IterNIter));
    CHECK(addVariable("iter_ampl" , m_IterAmpl));
    CHECK(addVariable("iter_tau" , m_IterTau));
    CHECK(addVariable("iter_peakSample" , m_IterPeakSample));
    CHECK(addVariable("iter_delay" , m_IterDelay));
    CHECK(addVariable("iter_qual" , m_IterQual));
    CHECK(addVariable("iter_index",m_IterIdx));
  }

  std::transform(m_CaloType.begin(), m_CaloType.end(), m_CaloType.begin(), toupper);
  if(m_CaloType.find("EM") < m_CaloType.size()) m_em = true;
  if(m_CaloType.find("HEC") < m_CaloType.size()) m_hec = true;
  if(m_CaloType.find("FCAL") < m_CaloType.size()) m_fcal = true;

  std::transform(m_dump_gain.begin(), m_dump_gain.end(), m_dump_gain.begin(), toupper);

  if(m_dump_gain.find("HIGH")!=std::string::npos)
    m_gains[CaloGain::LARHIGHGAIN]=true;
  else
    m_gains[CaloGain::LARHIGHGAIN]=false;

  if(m_dump_gain.find("MEDIUM")!=std::string::npos)
    m_gains[CaloGain::LARMEDIUMGAIN]=true;
  else
    m_gains[CaloGain::LARMEDIUMGAIN]=false;

  if(m_dump_gain.find("LOW")!=std::string::npos)
    m_gains[CaloGain::LARLOWGAIN]=true;
  else
    m_gains[CaloGain::LARLOWGAIN]=false;

  return StatusCode::SUCCESS;

}



StatusCode LArDigitFillerTool::fill ( const LArDigit& digit)
{
  ATH_MSG_DEBUG( "in fill()"        );

  //if (m_NEvents!=-1 && ++m_count>m_NEvents ) return StatusCode::SUCCESS;

  const LArFebHeaderContainer *larFebHeaderContainer=NULL;
  if (m_savesca == true){
    StatusCode sc = evtStore()->retrieve(larFebHeaderContainer);
    if (sc.isFailure() || !larFebHeaderContainer) {
      ATH_MSG_DEBUG( "Cannot read LArFebHeaderContainer from StoreGate! SCA address not saved."  );
      larFebHeaderContainer=NULL;
    }
  }

  const LArOFIterResultsContainer *larOFIterCont=NULL;
  if (m_dumpIter) {
    StatusCode sc =evtStore()->retrieve(larOFIterCont);
    if (sc.isFailure()) {
      ATH_MSG_DEBUG( "Can't retrieve LArOFIterResultsContainer from StoreGate."  );
      larOFIterCont=NULL;
    }
    ATH_MSG_DEBUG( "Got LArOFIterResultsContainer from StoreGate."  );
  }


  m_cellIndex=0;
  //size_t digitContIdx=0;
  size_t febHeaderContIdx=0;
  size_t ofcIterContIdx=0;
  size_t ofcIterContIdx_temp=0;

  //There are various restrictions when to write channel to CBNT:

  //Check gain
  const long gain=digit.gain();

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling){
     ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key() );
     return StatusCode::FAILURE;
  }
  /**   Don't do skipping yet.
  if (gain>=CaloGain::LARNGAIN || m_gains[gain]==false) continue;

  //Check detector part
  const HWIdentifier chid=digit.hardwareID();
  if ((m_onlineHelper->isEMBchannel(chid) ||  m_onlineHelper->isEMECchannel(chid)) && !m_em) continue;
  if (m_onlineHelper->isHECchannel(chid) && !m_hec) continue;
  if (m_onlineHelper->isFCALchannel(chid) && !m_fcal) continue;

  //Check if connected
  const bool connected=cabling->isOnlineConnected(chid);
  if (!connected && !m_dumpDisc) continue;


  //Copy samples to vector<long>, verify the number of samples, find max-sample
  const std::vector<short>& vSamples=digit.samples();
  size_t n=vSamples.size();
  if ((long)n!=nsamples) {
    ATH_MSG_WARNING( "Inconsistent number of ADC samples found!"  );
    if (n==0) continue;
  }

  // addSamples.resize(n);
  short maxSample=0;
  for (size_t i=0;i<n;++i) {
    // addSamples[i]=vSamples[i];//Convert short -> long
    if (vSamples[i]>maxSample) maxSample=vSamples[i];
  }

  //Check max-sample: skip filtering now! FIXME
  //  if (maxSample<m_dump_cut) continue;

  //All checks passed, fill ntuple now

  */
  const std::vector<short>& vSamples=digit.samples();
  const HWIdentifier chid=digit.hardwareID();
  size_t n=vSamples.size();
  int nsamples=vSamples.size();
  const bool connected=cabling->isOnlineConnected(chid);
  ATH_MSG_DEBUG( " is connected " <<connected );


  if (m_savedigit){

    (*m_samples).resize(n);
    for (size_t i=0;i<n;++i) {
      (*m_samples)[i] = vSamples[i] ; //Convert short -> long
    }

    //Add samples to ntuple
    // *m_samples = vSamples;

    //Add offline ID
    if (connected) {
      const Identifier id=cabling->cnvToIdentifier(chid);
      if(m_storeId) *m_offId = id.get_identifier32().get_compact() ;
      if (m_emId->is_lar_em(id)) {
	*m_calo = 0;
	*m_eta = m_emId->eta(id);
	*m_phi = m_emId->phi(id);
	*m_layer = m_emId->sampling(id);
      }
      else if (m_hecId->is_lar_hec(id)) {
	*m_calo= 1;
	*m_eta = m_hecId->eta(id);
	*m_phi = m_hecId->phi(id);
	*m_layer = m_hecId->sampling(id);
      }
      else if (m_fcalId->is_lar_fcal(id)) {
	*m_calo = 2;
	*m_eta = m_fcalId->eta(id);
	*m_phi = m_fcalId->phi(id);
	*m_layer = m_fcalId->module(id);
      }
      else {
	ATH_MSG_WARNING( "LArDigit Id "<< MSG::hex << id.get_compact() << MSG::dec
                         << " (FT: " << m_onlineHelper->feedthrough(chid) << " FEBSlot: " << m_onlineHelper->slot(chid) << " Chan: " << m_onlineHelper->channel(chid)
                         << ") appears to be neither EM nor HEC nor FCAL."  );
	*m_calo = -998;
      }
    }
    else {
      if(m_storeId) *m_offId= 0;
      *m_calo= -998;
      *m_eta= -998;
      *m_phi= -998;
      *m_layer= -998;
    } // end else disconnected

    //Add online ID
    *m_barrel_ec = m_onlineHelper->barrel_ec(chid);
    *m_pos_neg   = m_onlineHelper->pos_neg(chid);
    *m_FT =  m_onlineHelper->feedthrough(chid);
    *m_slot= m_onlineHelper->slot(chid);
    *m_channel = m_onlineHelper->channel(chid);
    *m_gain = gain;
    m_cellIndex++;
  }

  ATH_MSG_DEBUG( " m_savesca and FebHead, OFIterCont " << m_savesca <<" " << larFebHeaderContainer << " " <<larOFIterCont  );

  //Add SCA info if needed
  if (m_savesca)
    {
     if (larFebHeaderContainer ){
      const HWIdentifier chanFebId=m_onlineHelper->feb_Id(chid); //FebId of current channel
      const size_t febHead_s=larFebHeaderContainer->size();

      //Search for the right element in the FEB header container
      for(;febHeaderContIdx<febHead_s && chanFebId!=larFebHeaderContainer->at(febHeaderContIdx)->FEBId();
	  ++febHeaderContIdx)
        ;

      if (febHeaderContIdx<febHead_s) {
	const std::vector<unsigned short>& sca=larFebHeaderContainer->at(febHeaderContIdx)->SCA();
	n=sca.size();
	if ((long)n!=nsamples){
	  ATH_MSG_WARNING( "Inconsistent number of SCA entries"  );
	}

	/* addSamples.resize(n);
	for (size_t i=0;i<n;++i)
	  addSamples[i]=sca[i];//Convert short -> long
	*/

	(*m_sca).resize(n);
	for (size_t i=0;i<n;++i) {
	  (*m_sca)[i]=sca[i];
	}

	// *m_sca= sca;

      }
      else {
    	ATH_MSG_ERROR( "LArDigitContainer and LArFebHeaderContainer out of sync!"  );
	ATH_MSG_WARNING( "Can't find FEB  with id = "<< std::hex << chanFebId.get_compact() << std::dec  );
      }
     }
     else
       {
	 (*m_sca).clear();
       }
    }

  // Add OFC Iter results if needed
  ATH_MSG_DEBUG( " m_dumpIter, OFIterCont " << m_dumpIter<< " " << larOFIterCont  );
  if (m_dumpIter) {
    if (larOFIterCont) {

      const size_t iterRes_s=larOFIterCont->size();

      ofcIterContIdx_temp = ofcIterContIdx;
      //Search the right element of the results container
      for(;ofcIterContIdx<iterRes_s && chid!=larOFIterCont->at(ofcIterContIdx).getChannelID();++ofcIterContIdx)
        ;

      // Check No Missing Iteration Result
      if( ofcIterContIdx>=iterRes_s) {// && (ofcIterContIdx - ofcIterContIdx_temp)!=1){

	ATH_MSG_DEBUG( "Could not get iteration results for channel with id " << chid.get_compact() << std::dec
                       <<" in LArOFIterResultsContainer. CellIndex="<< m_cellIndex  );

	ofcIterContIdx = ofcIterContIdx_temp;
	*m_Itervalid=  0 ;
	*m_Iterconverged=  0 ;
	*m_IterNIter=  -999 ;
	*m_IterAmpl=  -999 ;
	*m_IterTau=  -999 ;
	*m_IterPeakSample=  -999 ;
	*m_IterDelay=  -999 ;
	*m_IterQual=  -999 ;
	*m_IterIdx= -999;
      }
      else{
	const LArOFIterResults& iterRes=larOFIterCont->at(ofcIterContIdx);
	//std::cout << chid << "\t" << iterRes.getChannelID() << std::endl;
	if (chid!=iterRes.getChannelID()) {
	  ATH_MSG_ERROR( "Channel IDs don't match"  );
	  return StatusCode::FAILURE;
	}
	*m_Itervalid= iterRes.getValid();
	*m_Iterconverged= iterRes.getConverged();
	*m_IterNIter= iterRes.getNIterPerf();
	*m_IterAmpl= iterRes.getAmplitude();
	*m_IterTau= iterRes.getTau();
	*m_IterPeakSample= iterRes.getPeakSample_final();
	*m_IterDelay= iterRes.getDelay_final();
	*m_IterQual= iterRes.getQuality();
	*m_IterIdx= iterRes.getOFCIndex();
      }

    }
    else
      {
	*m_Itervalid=  0 ;
	*m_Iterconverged=  0 ;
	*m_IterNIter=  -999 ;
	*m_IterAmpl=  -999 ;
	*m_IterTau=  -999 ;
	*m_IterPeakSample=  -999 ;
	*m_IterDelay=  -999 ;
	*m_IterQual=  -999 ;
	*m_IterIdx= -999;
      }
  }

  return StatusCode::SUCCESS ;

}


} // end of D3PD namespace

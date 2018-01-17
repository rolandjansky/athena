/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArStripsCrossTalkCorrector.h"

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "LArRawEvent/LArFebErrorSummary.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArRecConditions/ILArBadChannelMasker.h"
#include "LArBadChannelTool/LArBadChanTool.h"
#include "LArRecConditions/LArBadChannel.h"
#include <math.h>


LArStripsCrossTalkCorrector::LArStripsCrossTalkCorrector(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_larCablingSvc("LArCablingService"),
  m_badChannelTool("LArBadChanTool"),
  m_dontUseForCorr("LArBadChannelMasker/DontUseForXtalkCorr",this),
  m_dontCorrect("LArBadChannelMasker/NoXtalkCorr",this),
  m_onlineHelper(nullptr),
  m_emId(nullptr),
  m_event_counter(0),
  m_MAXeta(208), 
  m_MINeta(1),
  m_MAXphi(64),
  m_nStrips(-1),
  m_fatalFebErrorPattern(0xffff),
  m_noIdx(9999999),
  m_differences(6,0.3) { 
  declareProperty("KeyList",             m_keylist,
		  "List of input keys (normally the 'HIGH','MEDIUM','LOW')"); 
  declareProperty("ADCsaturation",       m_ADCsatur=0);
  declareProperty("UseAccumulatedDigits",m_useAccumulatedDigits=true); 
  declareProperty("AcceptableDifference",m_acceptableDifference=20,
		  "For sanity check: By how much the corrected value may differ from the original one (in %)");
  declareProperty("DontUseForXtalkCorr",m_dontUseForCorr,
		  "LArBadChannelMaskingTool telling what types of bad channel should not be taken into accout for xtalk correction of their neighbor");
  declareProperty("NoXtalkCorr",m_dontCorrect,
		  "LArBadChannelMaskingTool telling what types of bad channel should be ignored and not x-talk corrected");
  declareProperty("PedestalKey",m_pedKey="Pedestal",
		  "Key of the pedestal object (to be subtracted)");
   
}

LArStripsCrossTalkCorrector::~LArStripsCrossTalkCorrector()  {}

StatusCode LArStripsCrossTalkCorrector::initialize() {

  StatusCode sc = detStore()->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    msg (MSG::ERROR) << "Could not get LArOnlineID helper !" << endmsg;
    return StatusCode::FAILURE;
  }
  
  sc = detStore()->retrieve(m_emId, "LArEM_ID");
  if (sc.isFailure()) {
    msg (MSG::ERROR) << "Could not get LArEM_ID helper !" << endmsg;
    return StatusCode::FAILURE;
  }
  
  // Retrieve LArCablingService
  sc = m_larCablingSvc.retrieve();
  if (sc!=StatusCode::SUCCESS) {
    msg (MSG::ERROR) << " Can't get LArCablingSvc " << endmsg;
    return sc;
  }

  sc=m_dontCorrect.retrieve();
  if (sc!=StatusCode::SUCCESS) {
    msg (MSG::ERROR) << " Can't get LArBadChannelMaskingTool " << m_dontCorrect.typeAndName() << endmsg;
    return sc;
  }
  else 
        msg (MSG::DEBUG) << "Successfully retrieved " << m_dontCorrect.typeAndName() << endmsg;
  
  sc=m_dontUseForCorr.retrieve();
  if (sc!=StatusCode::SUCCESS) {
    msg (MSG::ERROR) << " Can't get LArBadChannelMaskingTool " << m_dontUseForCorr.typeAndName() << endmsg;
    return sc;
  }
  else
    msg (MSG::DEBUG) <<  "Successfully retrieved " << m_dontUseForCorr.typeAndName() << endmsg;

    

  sc = detStore()->regHandle(m_larPedestal,m_pedKey);
  if (sc!=StatusCode::SUCCESS) {
    msg (MSG::ERROR) << "Could not register DataHandle for pedestal with key " << m_pedKey  << endmsg;
    return sc;
  }

  m_event_counter=0;
  m_nStrips=-1;
  m_acceptableDifference/=100; //Convert from % to fraction

  return StatusCode::SUCCESS;
}


StatusCode LArStripsCrossTalkCorrector::execute()
{ 
  if ( m_event_counter < 100 || ( m_event_counter < 1000 && m_event_counter%100==0 ) || m_event_counter%1000==0 )
    msg(MSG::INFO) << "Processing event " << m_event_counter << endmsg;
  ++m_event_counter;
  
  if (m_keylist.size()==0) {
    msg(MSG::ERROR) << "Key list is empty! No containers to process!" << endmsg;
    return StatusCode::FAILURE;
  }
  
  if (m_useAccumulatedDigits)
    return executeWithAccumulatedDigits();
  else
    return executeWithStandardDigits();  
}


StatusCode LArStripsCrossTalkCorrector::executeWithAccumulatedDigits() 
{
  MsgStream log(msgSvc(), name());
  StatusCode sc;
  unsigned nSaturation=0;

  const LArFebErrorSummary* febErrSum=NULL;
  if (evtStore()->contains<LArFebErrorSummary>("LArFebErrorSummary")) {
    sc=evtStore()->retrieve(febErrSum);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed to retrieve FebErrorSummary object!" << endmsg;
      return sc;
    }
  }
  else
    if (m_event_counter==1)
      msg(MSG::WARNING) << "No FebErrorSummaryObject found! Feb errors not checked!" << endmsg;

  std::vector<std::string>::const_iterator key_it=m_keylist.begin();
  std::vector<std::string>::const_iterator key_it_e=m_keylist.end();
  
  const LArAccumulatedCalibDigitContainer* larAccumulatedCalibDigitContainer;
  
  // now start to deal with digits   
  
  for (;key_it!=key_it_e;key_it++) { // Loop over all containers that are to be processed (e.g. different gains)
    
    sc = evtStore()->retrieve(larAccumulatedCalibDigitContainer,*key_it);
    if (sc.isFailure()){ 
      msg(MSG::WARNING) << "Cannot read LArAccumulatedCalibDigitContainer from StoreGate! key=" << *key_it << endmsg;
      continue; // Try next container
    }
    

    LArAccumulatedCalibDigit febErrorDummy; //Use the address of this object to mark cells belonging to a errornous FEB
    LArAccumulatedCalibDigit inexistingDummy; //Use the address of this object for "cells" that woudl be outside of cryostat
    
    
    LArAccumulatedCalibDigitContainer::const_iterator it=larAccumulatedCalibDigitContainer->begin();
    LArAccumulatedCalibDigitContainer::const_iterator it_end=larAccumulatedCalibDigitContainer->end();

    HWIdentifier  lastFailedFEB(0);

    if(it == it_end) {
      msg(MSG::DEBUG) << "LArAccumulatedCalibDigitContainer with key = " << *key_it << " is empty " << endmsg;
      //return StatusCode::SUCCESS;
      continue; // Try next container
    } else {
      msg( MSG::DEBUG) << "Processing LArAccumulatedCalibDigitContainer with key = " << *key_it 
			 << ". Size: " << larAccumulatedCalibDigitContainer->size() << endmsg;
    }
    
    //Get barrel/ec for online Identifier of the first cell in the container
    HWIdentifier chid=(*it)->hardwareID();      
    const int have_barrel_ec=m_onlineHelper->barrel_ec(chid);

    if (have_barrel_ec==1) {
      m_MINeta = 0;
      m_MAXeta = 208;  //Endcap
    }
    else {
      m_MINeta = 1;
      m_MAXeta = 448;  //Barrel
    }
    
    if (m_knownMissingFebs.size()!=(unsigned)m_MAXeta) {
      sc=initKnownMissingFebs(have_barrel_ec);
      if (sc.isFailure()) return sc;
    }

    m_stripsLookUp.resize(2*m_MAXphi); //change index to coded index
    for (unsigned iside=0;iside<2;++iside) {
      for (unsigned iphi=0; iphi!=m_MAXphi; ++iphi) {
	m_stripsLookUp[iphi+iside*m_MAXphi].resize(m_MAXeta); 
      }
    }
    
    for (unsigned ieta=0;ieta!=m_MAXeta;++ieta) {  //Loop over Strips cells
      for (unsigned iphi=0;iphi!=m_MAXphi;++iphi) {  //Loop over Strips cells	   
        for (unsigned iside=0;iside<2;++iside) {
	  m_stripsLookUp[iphi+iside*m_MAXphi][ieta]=0;
        }
      }
    }    
    
    msg(MSG::DEBUG) << "Filling Strips lookup table..." << endmsg ;
    int nStrips=0;
    
    for (;it!=it_end;it++) {  //Loop over all cells to fill Strips lookup table
      const LArAccumulatedCalibDigit* dig=*it;
      chid=dig->hardwareID();     
      if (!(m_onlineHelper->isEMBchannel(chid) || m_onlineHelper->isEMECchannel(chid))) continue; //Deal only with EM calos case
      if (!m_larCablingSvc->isOnlineConnected(chid)) continue; //ignore disconnected channels
	  

      const Identifier id=m_larCablingSvc->cnvToIdentifier(chid);
      if (m_emId->sampling(id)!=1) continue; //Deal only with strips
	
      const HWIdentifier febid=m_onlineHelper->feb_Id(chid);
      if (febErrSum) {
	const uint16_t febErrs=febErrSum->feb_error(febid);
	if (febErrs & m_fatalFebErrorPattern) {
	  if (febid!=lastFailedFEB) {
	    lastFailedFEB=febid;
	    msg(MSG::ERROR) << "Event " << m_event_counter << " Feb " <<  m_onlineHelper->channel_name(febid) 
			    << " reports error(s):" << febErrSum->error_to_string(febErrs) << ". Data ignored." << endmsg;
	  }
	  dig=&febErrorDummy;
	} //end if fatal feb error
      }//end if check feb error summary


      if (m_onlineHelper->barrel_ec(chid) != have_barrel_ec) {
	msg(MSG::FATAL) << "Found barrel and endcap cells in same event. This is not supported by the LArStripsCrossTalkCorrector!" << endmsg;
	return StatusCode::FAILURE;
      }
    
      const size_t ieta=getEtaIndex(id);
      if (ieta==m_noIdx) continue; //Not a cell we care about
      const size_t iphi=getPhiIndex(id);
      if (iphi>=2*m_MAXphi || ieta>=m_MAXeta) {
	msg( MSG::FATAL) << "Array index out of range: iphi=" << iphi << " (max " << m_MAXphi << "), ieta=" 
			 << ieta << "(max " << m_MAXphi << ")" << endmsg;
	return StatusCode::FAILURE;
      }
      ++nStrips;


      //check for saturation:
      bool saturated=false;
      if (m_ADCsatur>0) {
	const std::vector<uint32_t>& samples=dig->sampleSum();
	const size_t& nS=samples.size();
	const uint32_t maxValue=(uint32_t)(m_ADCsatur*dig->nTriggers());
	for (size_t i=0;i<nS;++i) {
	  if (samples[i] >= maxValue) {
	    saturated=true;
	    ATH_MSG_DEBUG("Found saturating digit (index = " << i           
			  << ", <ADC> = " << samples[i]/dig->nTriggers()
			  << ", DAC = " << dig->DAC()
			  << ") for channel 0x"  << MSG::hex << chid.get_compact() << MSG::dec
			  << ". Skipping.");
	    break;
	  }//end if>maxvalue
	}//end loop over digits
      }
      if (saturated) {
	++nSaturation;
	continue; //Skip this channel
      }

     
      m_stripsLookUp[iphi][ieta]=dig;

    }//End loop over all cells
    
    if (m_nStrips<0) 
      m_nStrips=nStrips;
    
    if (m_nStrips != nStrips) {
      msg(MSG::WARNING) << "Number of strips changed! Have " <<  nStrips << ", had " << m_nStrips 
			<< " before. Size of map: " << 2*m_MAXphi*(m_MAXeta-m_MINeta) << endmsg;
      m_nStrips=nStrips;
    }
    else
      ATH_MSG_DEBUG("Strips lookup table filled. Have " <<  nStrips << " out of " << 2*m_MAXphi*(1+m_MAXeta-m_MINeta) 
		      << " channels.");


    //Weight for the four neighbors: 
    //only half the signal of the next-to-next neihbor is summed 
    //A more correct approach would be to check if the 'mirrored' neighbor is pulsed as well and compute this weights on-the-fly
    //const float weight[4]={0.5,1,1,0.5};
    neighbour_t neighbours[4]; //Keep pointers for the four neightbors
    neighbours[0].dist=-2;
    neighbours[1].dist=-1;
    neighbours[2].dist=1;
    neighbours[3].dist=2;

    for (unsigned iside=0; iside<2; iside++) {          //Loop over both sides of the detector
      for (int ieta=0; ieta<(int)m_MAXeta; ieta++) {    // Loop over eta range
	for (unsigned iphi=0; iphi<m_MAXphi; iphi++) {  // Loop over phi range
	 
	  const unsigned iphi2=iphi+m_MAXphi*iside;    //Phi index inside lookup table 
	  const LArAccumulatedCalibDigit* currDig=m_stripsLookUp[iphi2][ieta];

	  if (currDig==0 || currDig==&febErrorDummy) continue; //Digit not found or FEB in error: ignore

	  if ( currDig->isPulsed() ) {
	    chid = currDig->hardwareID();
	    CaloGain::CaloGain t_gain = currDig->gain();
	    if (m_dontCorrect->cellShouldBeMasked(chid,t_gain)) {
	      ATH_MSG_DEBUG("Strips 0x"  << MSG::hex << chid.get_compact() << MSG::dec << " (Eta = " << ieta << ", Phi = " 
			    << iphi2 << ") should not be touched accoring to " << m_dontCorrect.typeAndName());
	      continue;
	    }

	    const unsigned NtotSamples = currDig->nsamples();	
	    const unsigned NbTriggers = currDig->nTriggers();	    
            ATH_MSG_VERBOSE("Now processing Strips 0x"  << MSG::hex << chid.get_compact() << MSG::dec
			    << " (Eta = " << ieta << ", Phi = " << iphi2 << ")");

	    //Fill the pointers and pedestal in the 'neighbours' array
	    for (unsigned i=0;i<4;i++) {
	      //Set all zero to start with...
	      neighbours[i].dig=NULL;
	      neighbours[i].ped=0.;
	      const int neigbEtaItx=neighbours[i].dist+(int)ieta;
	      //Check if we are supposed to have this neighbour
	      if (neigbEtaItx<(int)m_MINeta || neigbEtaItx>=(int)m_MAXeta) {
		ATH_MSG_DEBUG("Neighbour " << neighbours[i].dist <<" , ieta=" << neigbEtaItx 
			      << " doesn't exist. (min="<< m_MINeta << " ,max=" << m_MAXeta <<  ")");
		neighbours[i].dig=&inexistingDummy;
		continue;
	      }
	      const LArAccumulatedCalibDigit* neighDig=m_stripsLookUp[iphi2][neigbEtaItx];
	      //Check if neighbour is present
	      if (!neighDig) {
		//neighbor missing. Could be known missing FEB or new problem
		//Reported only if at least WARNING
		if (msgLvl(MSG::WARNING)) {
		  if (m_knownMissingFebs[neigbEtaItx].test(iphi2)) {
		    ATH_MSG_DEBUG("FEB missing for neighbour " << neighbours[i].dist << " to be added to Strip 0x" 
				  << MSG::hex << chid.get_compact() << MSG::dec
				  << " (Eta = " << ieta << ", Phi = " << iphi << ")");
		  }
		  else {
		    msg(MSG::WARNING) << "Cannot find neighbour " << neighbours[i].dist << " to be added to Strip 0x" 
				      << MSG::hex << chid.get_compact() << MSG::dec
				      << " (Eta = " << ieta << ", Phi = " << iphi << ")" << endmsg;
		  }
		}//end if msgLvl(INFO)
 		continue;
 	      }
	      if (neighDig==&febErrorDummy) { //Pointer comparison!!
		ATH_MSG_WARNING("Neighbour " << neighbours[i].dist << " of strip 0x" << MSG::hex << chid.get_compact() << MSG::dec
				<< " (Eta = " << ieta << ", Phi = " << iphi <<"), has a FEB error. Ignored.");
		continue;
	      }

	      //Check if neighbour is pulsed
	      if (neighDig->isPulsed()) {
		msg(MSG::WARNING) << "Neighbour " << neighbours[i].dist << " of strip 0x" << MSG::hex << chid.get_compact() << MSG::dec
				  << " (Eta = " << ieta << ", Phi = " << iphi <<", " << printMaxSample(neighDig) 
				  << ") is pulsed. Not used for correction." << endmsg;
		continue;
	      }
	      //Check if neighbour is on the bad-channel list
	      if (m_dontUseForCorr->cellShouldBeMasked(neighDig->hardwareID(),t_gain)) {
		ATH_MSG_DEBUG("Neighbour " << neighbours[i].dist << " of strip 0x" << MSG::hex << chid.get_compact() << MSG::dec
			      << " (Eta = " << ieta << ", Phi = " << iphi 
			      << ") is flagged by the LArBadChannelMaskingTool. Not used for correction.");
		  continue;
	      }
	      //Get Pedestal
	      const float pedestal = m_larPedestal->pedestal(neighDig->hardwareID(),t_gain);
	      if (pedestal <= (1.0+LArElecCalib::ERRORCODE)) {
		msg(MSG::ERROR) << "No pedestal are available for neighbour " << neighbours[i].dist << " of Strip 0x" 
				<< MSG::hex << chid.get_compact() << MSG::dec
				<< " (Eta = " << ieta << ", Phi = " << iphi << "). Not used for correction!" << endmsg;
		continue;
	      }
	      //All went ok, fill struct
	      neighbours[i].dig=neighDig;
	      neighbours[i].ped=pedestal;

	      //The weight is one, unless the neighbor-of-the-neighbor is pulsed too
	      neighbours[i].weight=1.0;	
	      //Now check if the neighbor-of-the-neighbor is pulsed to set a different weight.
	      const int nnEta=ieta+2*neighbours[i].dist;
	      if (nnEta>=(int)m_MINeta && nnEta<(int)m_MAXeta) {
		const LArAccumulatedCalibDigit* nnDig=m_stripsLookUp[iphi2][nnEta];
		if (nnDig!=0) {
		  //Could bea also FebErrorDummy (which will always pretentd to be not pulsed)
		  if (nnDig->isPulsed()) {
		     ATH_MSG_VERBOSE("Neighbour " << neighbours[i].dist << " of Strip 0x" << MSG::hex << chid.get_compact() << MSG::dec 
				     << " has another pulsed cell in the neighborhood. Setting weight to 0.5");
		    neighbours[i].weight=0.5;
		  }
		}//end if neighbor-of-neighbor not NULL
	      }//end if neighbor-of-neighbor in eta range
	    
	    }//End loop over four neighbours


	    //Now loop over samples to apply xtalk correction
	    std::vector<double> SampleSums(NtotSamples);
	    for (std::size_t SampleIndex=0;SampleIndex<NtotSamples; ++SampleIndex ) {
	      SampleSums[SampleIndex]=(double)currDig->sampleSum()[SampleIndex];
	      ATH_MSG_VERBOSE("SampleSum " << SampleIndex << " (" << SampleSums[SampleIndex] << " ADC counts / " << NbTriggers << " Ntriggers)");
	    }
	    //Loop over the neighbours and apply corrections
	    for (unsigned i=0;i<4;i++) {   
	      if (neighbours[i].dig==&inexistingDummy) { //Pointer comparision!
		ATH_MSG_VERBOSE("Neighbour " << neighbours[i].dist << " of strip 0x" << MSG::hex << chid.get_compact() << MSG::dec
				<< " (Eta = " << ieta << ", Phi = " << iphi << ") does not exist.");
		//no neighbor (end of calorimeter). Do nothing
		continue;
	      }
	      
	      if (neighbours[i].dig) { //"Normal" case
		correctSamples(SampleSums,neighbours[i]);
		ATH_MSG_VERBOSE("Neighbour " << neighbours[i].dist << " of strip 0x" << MSG::hex << chid.get_compact() << MSG::dec
			       << " (Eta = " << ieta << ", Phi = " << iphi << ") is used for correction");
	      } //end if have pointer to neighbor cell
	      else { //Neighbor not usable for some reason, try mirrored neighbor
		const int j = 3-i; //get mirrored neighbor
		msg(MSG::INFO) << "Neighbour " << neighbours[i].dist << " of channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
			       << " cannot be used. Taking mirrored neighbour " << neighbours[j].dist << " instead." << endmsg;
		if (neighbours[j].dig!=0 && neighbours[j].dig!=&inexistingDummy){
		  correctSamples(SampleSums,neighbours[j]);
		  ATH_MSG_VERBOSE("Mirrored neighbour " << neighbours[j].dist << " of strip 0x" << MSG::hex << chid.get_compact() << MSG::dec
				  << " (Eta = " << ieta << ", Phi = " << iphi << ") is used for correction");
		}//end if neighbours[j].dig
		else {
		  msg(MSG::WARNING) << "Mirrored Neighbour " << neighbours[j].dist << " of channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
				    << " cannot be used too. No correction applied" << endmsg;
		}
	      }//end if no neighboring cell
	    }//end loop over the four neighbors
	    
	    
	    std::vector<uint32_t> SampleSumInt(NtotSamples);
	    bool unresonable=false;
	    std::size_t iPeak=std::max_element(currDig->sampleSum().begin(),currDig->sampleSum().end())-currDig->sampleSum().begin();
	    for (std::size_t SampleIndex=0;SampleIndex<NtotSamples; ++SampleIndex ) {
	      const double& thisSampleSum=SampleSums[SampleIndex];
	      const uint32_t& oldVal=currDig->sampleSum()[SampleIndex];
	      if (thisSampleSum<0) {
		unresonable=true;
		msg(MSG::WARNING) << "Channel 0x"  << MSG::hex << chid.get_compact() << MSG::dec
				  << " (Eta = " << ieta << ", Phi = " << iphi << ") Resulting ADC sample " << SampleIndex <<" negative! " 
				  << thisSampleSum << " instead of " << oldVal << " Not corrected." << endmsg;
                break;
	      }

	      if (SampleIndex==iPeak) { //check value of correction at peak
		const float dev=(thisSampleSum-oldVal)/oldVal;
		m_differences.add(currDig,dev);
		if (fabs(dev)>m_acceptableDifference) {
		  unresonable=true;
		  ATH_MSG_WARNING("Channel 0x"  << MSG::hex << chid.get_compact() << MSG::dec
				  << " (Eta = " << ieta << ", Phi = " << iphi
				  << ") DAC=" << currDig->DAC() << ":  Resulting ADC sample " << SampleIndex <<" looks unreasonable: " 
				  << thisSampleSum << " instead of " << oldVal << " (off by " << dev*100 << "%)" 
				  << " (=" << thisSampleSum/NbTriggers << " -> " << oldVal/NbTriggers <<", ped=" 
				  << m_larPedestal->pedestal(chid,t_gain)
				  << " Not Corrected.");
		  break;
		}//end if dev>m_acceptableDifference
	      } // end if at peak sample
	      SampleSumInt[SampleIndex] = (uint32_t)(thisSampleSum);
	    }//End loop over samples
	 
	    if (unresonable) {
	      m_uncorrectedIds.insert(chid);
	      ATH_MSG_DEBUG("Correction for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec << " failed");
	    }
	    else {
	      ATH_MSG_VERBOSE("Correction for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec << " DAC="<<currDig->DAC() << " succeeded " 
			      << currDig->sampleSum()[2] << "->" << SampleSumInt[2]);
              // FIXME: const_cast, modifying object in SG.
	      const_cast<LArAccumulatedCalibDigit*>(currDig)->setSampleSum(SampleSumInt);
	    }

	  }// end if-pulsed 
	}//end loop over phi
      }//end loop over eta
    }//End loop over sides   
  } //End loop over all containers    

  if (nSaturation) 
    msg(MSG::INFO) << "Found " << nSaturation << " saturating digits in this event." << endmsg;
  
  return StatusCode::SUCCESS;
}


StatusCode LArStripsCrossTalkCorrector::executeWithStandardDigits() {
  msg(MSG::FATAL) << "Xtalk correction wiht non-accumulated digits not supported any more." << endmsg;
  return StatusCode::FAILURE;
}

StatusCode LArStripsCrossTalkCorrector::finalize() { 
  if (msgLvl(MSG::WARNING) && m_uncorrectedIds.size()>0 ) {
    const LArBadChanBitPacking packing;
    msg(MSG::WARNING) << "The following "<<  m_uncorrectedIds.size() << " channels are (partly) uncorrected because of unresonable high correction:" << endmsg;
    std::set<HWIdentifier>::const_iterator it=m_uncorrectedIds.begin();
    std::set<HWIdentifier>::const_iterator it_e=m_uncorrectedIds.end();
    for (;it!=it_e;++it) {
      LArBadChannel bc=m_badChannelTool->status(*it);
      msg(MSG::WARNING) << "Not corrected: " << m_onlineHelper->channel_name(*it) << " " << packing.stringStatus(bc) << endmsg;
    }
  }		    

  m_differences.print(msg(),m_onlineHelper);

  return StatusCode::SUCCESS;
}


std::string  LArStripsCrossTalkCorrector::printMaxSample(const LArAccumulatedCalibDigit* thisDig) {

  const std::vector<uint32_t>& ss=thisDig->sampleSum();
  std::vector<uint32_t>::const_iterator imax=std::max_element(ss.begin(),ss.end());
  std::ostringstream output;
  output << "S[" << imax-ss.begin() << "]=" << *imax/thisDig->nTriggers(); 
  return output.str();
}


StatusCode LArStripsCrossTalkCorrector::initKnownMissingFebs(const int bec) {
  unsigned nMissing=0;
  m_knownMissingFebs.clear();
  m_knownMissingFebs.resize(m_MAXeta);
  
  //const std::vector<HWIdentifier> mf=m_badChannelTool->missingFEBs();
  const LArBadChanTool::BadFebVec& mf=m_badChannelTool->fullBadFebsState();
  
  //msg(MSG::DEBUG) << "Got " << mf.size() << " missing FEBs" << endmsg;
  LArBadChanTool::BadFebVec::const_iterator it=mf.begin();
  LArBadChanTool::BadFebVec::const_iterator it_e=mf.end();
  for (;it!=it_e;++it) {
    const HWIdentifier& fid=it->first;
    const LArBadFeb& bf=it->second;
    if (bf.deadAll() || bf.deadReadout() || bf.deactivatedInOKS()) {
      if ((bec==0 && m_onlineHelper->isEMBchannel(fid)) ||
	  (bec==1 && m_onlineHelper->isEMECchannel(fid))) {
	msg(MSG::DEBUG) << "Feb  " << MSG::hex << fid.get_compact() << MSG::dec << " reported as missing" << endmsg;
	const int nChan=m_onlineHelper->channelInSlotMax(fid);
	for (int c=0;c<nChan;++c) {
	  const HWIdentifier chid=m_onlineHelper->channel_Id(fid,c);
	  const Identifier id=m_larCablingSvc->cnvToIdentifier(chid);
	  const size_t ieta=getEtaIndex(id);
	  if (ieta==m_noIdx) continue; //Not a cell we care about
	  const size_t iphi=getPhiIndex(id);
	  if (iphi>=2*m_MAXphi || ieta>=m_MAXeta) {
	    msg(MSG::FATAL) << "Array index out of range: iphi=" << iphi << " (max " << m_MAXphi << "), ieta=" 
			    << ieta << "(max " << m_MAXphi << ")" << endmsg;
	    return StatusCode::FAILURE;
	  }
	  m_knownMissingFebs[ieta].set(iphi);
	  ++nMissing;
	}//end loop over channels of one feb
      }//end if is barrel/endcap & EM
    }//end if is dead
  }//end loop over problematic febs
  msg(MSG::INFO) << "Number of known missing Strip cells: "<< nMissing << endmsg;
  return StatusCode::SUCCESS;
}

















XtalkCorrHisto::XtalkCorrHisto(const unsigned nBins, const float upperLimit) :
  m_nBins(nBins),
  m_binwidth(upperLimit/nBins),
  m_diffs(nBins,0)
{}

void XtalkCorrHisto::add(const LArAccumulatedCalibDigit* dig, const float reldiff) {

  unsigned iBin=(unsigned)(fabs(reldiff)/m_binwidth);
  if (iBin>=m_nBins-1) {
    iBin=m_nBins-1;
    m_maxlist.push_back(maxlistitem(dig->hardwareID(),dig->DAC(),dig->delay(),reldiff));
  }
  ++m_diffs[iBin];
  return;
}

void XtalkCorrHisto::print(MsgStream& out, const LArOnlineID* onlId,  MSG::Level lvl) {

  const unsigned nBins=m_diffs.size();
  if (nBins==0) return;
  out << lvl << "Cross Talk Correction Summary:" << endmsg;
  for (unsigned i=0;i<nBins-1;++i) 
    out << lvl << " " << i*m_binwidth << " - " << (i+1)*m_binwidth << ":" << m_diffs[i] << " channels" << endmsg;

  out << lvl << " " << (nBins-1)*m_binwidth << " - max" << ":" << m_diffs[nBins-1] << " channels" << endmsg;

  if (m_maxlist.size()>0) {
    out << lvl << "Data points with the highes correction:" << endmsg;
    std::vector<maxlistitem>::const_iterator it=m_maxlist.begin();
    std::vector<maxlistitem>::const_iterator it_e=m_maxlist.end();
    for(;it!=it_e;++it) 
      out << lvl << " " << onlId->channel_name(it->chid) << " DAC=" << it->dac << " Delay=" << it->delay << " Deviation:" << it->reldiff << endmsg;
  }
  return;
}




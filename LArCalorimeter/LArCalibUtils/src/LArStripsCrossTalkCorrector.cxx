/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArStripsCrossTalkCorrector.h"

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "LArRawEvent/LArFebErrorSummary.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArRecConditions/LArBadChannel.h"
#include "LArBadChannelTool/LArBadChannelDBTools.h"
#include <math.h>


StatusCode LArStripsCrossTalkCorrector::initialize() {

  StatusCode sc = detStore()->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not get LArOnlineID helper !" );
    return StatusCode::FAILURE;
  }
  
  sc = detStore()->retrieve(m_emId, "LArEM_ID");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not get LArEM_ID helper !" );
    return StatusCode::FAILURE;
  }
  

  ATH_CHECK(m_dontUseForCorrMask.buildBitMask(m_dontUseForCorr,msg()));
  ATH_CHECK(m_dontCorrMask.buildBitMask(m_dontCorr,msg()));

  ATH_CHECK(m_BCKey.initialize());
  ATH_CHECK(m_BFKey.initialize());
  ATH_CHECK(m_cablingKey.initialize());
  ATH_CHECK(m_pedKey.initialize());
  
  m_acceptableDifference.value()/=100; //Convert from % to fraction

  //Initialize strips lookup table:
  for (unsigned bec : {0,1}) {
    m_stripsLookUp[bec].resize(2*m_MAXphi); //change index to coded index
    for (unsigned iside=0;iside<2;++iside) {
      for (unsigned iphi=0; iphi!=m_MAXphi; ++iphi) {
	      m_stripsLookUp[bec][iphi+iside*m_MAXphi].resize(m_MAXeta[bec],nullptr); 
      }   
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode LArStripsCrossTalkCorrector::execute()
{ 
  if ( m_event_counter < 100 || ( m_event_counter < 1000 && m_event_counter%100==0 ) || m_event_counter%1000==0 )
    ATH_MSG_INFO( "Processing event " << m_event_counter );
  ++m_event_counter;
  
  if (m_keylist.size()==0) {
    ATH_MSG_ERROR( "Key list is empty! No containers to process!" );
    return StatusCode::FAILURE;
  }
  

  StatusCode sc;
  unsigned nSaturation=0;

  const LArFebErrorSummary* febErrSum=NULL;
  if (evtStore()->contains<LArFebErrorSummary>("LArFebErrorSummary")) {
    sc=evtStore()->retrieve(febErrSum);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Failed to retrieve FebErrorSummary object!" );
      return sc;
    }
  }
  else
    if (m_event_counter==1)
      ATH_MSG_WARNING( "No FebErrorSummaryObject found! Feb errors not checked!" );

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR( "Do not have cabling object LArOnOffIdMapping");
     return StatusCode::FAILURE;
  }

  SG::ReadCondHandle<LArBadChannelCont> bcHdl{m_BCKey};
  const LArBadChannelCont* bcCont{*bcHdl};

  SG::ReadCondHandle<ILArPedestal> pedHdl{m_pedKey};
  const ILArPedestal* larPedestal=*pedHdl; 


  const LArAccumulatedCalibDigitContainer* larAccumulatedCalibDigitContainer;
  
  // now start to deal with digits   

  for (const std::string& key : m_keylist) { // Loop over all containers that are to be processed (e.g. different gains)
    
    sc = evtStore()->retrieve(larAccumulatedCalibDigitContainer,key);
    if (sc.isFailure()){ 
      ATH_MSG_WARNING( "Cannot read LArAccumulatedCalibDigitContainer from StoreGate! key=" << key );
      continue; // Try next container
    }
    

    LArAccumulatedCalibDigit febErrorDummy; //Use the address of this object to mark cells belonging to a errornous FEB
    LArAccumulatedCalibDigit inexistingDummy; //Use the address of this object for "cells" that would be outside of cryostat
    
    
    HWIdentifier  lastFailedFEB(0);

    if(larAccumulatedCalibDigitContainer->empty()) {
      ATH_MSG_DEBUG( "LArAccumulatedCalibDigitContainer with key = " << key << " is empty " );
      //return StatusCode::SUCCESS;
      continue; // Try next container
    } else {
      ATH_MSG_DEBUG( "Processing LArAccumulatedCalibDigitContainer with key = " << key
			 << ". Size: " << larAccumulatedCalibDigitContainer->size() );
    }
    
    //Fill missing febs (if not done yet)
    if (!m_missingFEBsDone) {
      ATH_CHECK(initKnownMissingFebs());
    }
    
      
    //Reset strips lookup table to nullptr:
    for (int bec : {0,1}) {
      for (unsigned ieta=0;ieta!=m_MAXeta[bec];++ieta) {  //Loop over Strips cells
        for (unsigned iphi=0;iphi!=m_MAXphi;++iphi) {  //Loop over Strips cells	   
          for (unsigned iside=0;iside<2;++iside) {
	          m_stripsLookUp[bec][iphi+iside*m_MAXphi][ieta]=nullptr;
          }
        }
      }    
    } //end loop over barrel & EC
    ATH_MSG_DEBUG( "Filling strips lookup table..." ) ;
    int nStrips=0;
    
    for (const LArAccumulatedCalibDigit* dig : *larAccumulatedCalibDigitContainer) {  //Loop over all cells to fill Strips lookup table
      const HWIdentifier chid=dig->hardwareID();     
      if (!(m_onlineHelper->isEMBchannel(chid) || m_onlineHelper->isEMECchannel(chid))) continue; //Deal only with EM calos case
      if (!cabling->isOnlineConnected(chid)) continue; //ignore disconnected channels
	  

      const Identifier id=cabling->cnvToIdentifier(chid);
      if (m_emId->sampling(id)!=1) continue; //Deal only with strips
	
      const HWIdentifier febid=m_onlineHelper->feb_Id(chid);
      if (febErrSum) {
	      const uint16_t febErrs=febErrSum->feb_error(febid);
	       if (febErrs & m_fatalFebErrorPattern) {
	         if (febid!=lastFailedFEB) {
	          lastFailedFEB=febid;
	           ATH_MSG_ERROR( "Event " << m_event_counter << " Feb " <<  m_onlineHelper->channel_name(febid) 
		          << " reports error(s):" << febErrSum->error_to_string(febErrs) << ". Data ignored." );
	        }   
	        dig=&febErrorDummy;
	      } //end if fatal feb error
      }//end if check feb error summary


      const int bec=m_onlineHelper->barrel_ec(chid);  
      const size_t ieta=getEtaIndex(id);
      if (ieta==m_noIdx) continue; //Not a cell we care about
      const size_t iphi=getPhiIndex(id);
      if (iphi>=2*m_MAXphi || ieta>=m_MAXeta[bec]) {
	      ATH_MSG_FATAL( "Array index out of range: iphi=" <<  iphi << " (max " << m_MAXphi << "), ieta=" 
			                << ieta << "(max " << m_MAXeta[bec] << ")" );
	      return StatusCode::FAILURE;
      }
      ++nStrips;


      //check for saturation:
      bool saturated=false;
      if (m_ADCsatur>0) {
	const std::vector<uint64_t>& samples=dig->sampleSum();
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
     
    m_stripsLookUp[bec][iphi][ieta]=dig;

    }//End loop over all cells
    
    if (m_nStrips<0) 
      m_nStrips=nStrips;
    
    if (m_nStrips != nStrips) {
      ATH_MSG_WARNING( "Number of strips changed! Have " <<  nStrips << ", had " << m_nStrips << " before.");
      m_nStrips=nStrips;
    }
    else
      ATH_MSG_DEBUG("strips lookup table filled. Have " <<  nStrips << " out of " << 2*m_MAXphi*(1+m_MAXeta[0]-m_MINeta[0]) 
		      << " barrel channels plus " << 2*m_MAXphi*(1+m_MAXeta[1]-m_MINeta[1]) << "endcap channels.");


    //Weight for the four neighbors: 
    //only half the signal of the next-to-next neihbor is summed 
    //A more correct approach would be to check if the 'mirrored' neighbor is pulsed as well and compute this weights on-the-fly
    //const float weight[4]={0.5,1,1,0.5};
    neighbour_t neighbours[4]; //Keep pointers for the four neightbors
    neighbours[0].dist=-2;
    neighbours[1].dist=-1;
    neighbours[2].dist=1;
    neighbours[3].dist=2;

    for (unsigned bec : {0,1}) { //Loop over barrel and endcap
      for (unsigned iside=0; iside<2; iside++) {          //Loop over both sides of the detector
        for (int ieta=0; ieta<(int)m_MAXeta[bec]; ieta++) {    // Loop over eta range
	  for (unsigned iphi=0; iphi<m_MAXphi; iphi++) {  // Loop over phi range
	    const unsigned iphi2=iphi+m_MAXphi*iside;    //Phi index inside lookup table 
	    const LArAccumulatedCalibDigit* currDig=m_stripsLookUp[bec][iphi2][ieta];
	    if (currDig==0 || currDig==&febErrorDummy) continue; //Digit not found or FEB in error: ignore
	    
	    if ( currDig->isPulsed() ) {
	      HWIdentifier chid = currDig->hardwareID();
	      CaloGain::CaloGain t_gain = currDig->gain();
	      if (m_dontCorrMask.cellShouldBeMasked(bcCont,chid)) {
		ATH_MSG_DEBUG("Strip " << m_onlineHelper->channel_name(chid) <<" (Eta = " << ieta << ", Phi = " 
			      << iphi2 << ") should not be touched accoring to jobConfig");
		continue;
	      }

	      const unsigned NtotSamples = currDig->nsamples();	
	      const unsigned NbTriggers = currDig->nTriggers();	    
	      ATH_MSG_VERBOSE("Now processing strip "<< m_onlineHelper->channel_name(chid) << " (Eta = " << ieta << ", Phi = " << iphi2 << ")");

	      //Fill the pointers and pedestal in the 'neighbours' array
	      for (unsigned i=0;i<4;i++) {
		//Set all zero to start with...
		neighbours[i].dig=NULL;
		neighbours[i].ped=0.;
		const int neigbEtaItx=neighbours[i].dist+(int)ieta;
		//Check if we are supposed to have this neighbour
		if (neigbEtaItx<(int)m_MINeta[bec] || neigbEtaItx>=(int)m_MAXeta[bec]) {
		  ATH_MSG_DEBUG("Neighbour " << neighbours[i].dist <<" , ieta=" << neigbEtaItx 
				<< " doesn't exist. (min="<< m_MINeta[bec] << " ,max=" << m_MAXeta[bec] <<  ")");
		  neighbours[i].dig=&inexistingDummy;
		  continue;
		}
		const LArAccumulatedCalibDigit* neighDig=m_stripsLookUp[bec][iphi2][neigbEtaItx];
		//Check if neighbour is present
		if (!neighDig) {
		  //neighbor missing. Could be known missing FEB or new problem
		  //Reported only if at least WARNING
		  if (msgLvl(MSG::WARNING)) {
		    if (m_knownMissingFebs[bec][neigbEtaItx].test(iphi2)) {
		      ATH_MSG_DEBUG("FEB missing for neighbour " << neighbours[i].dist << " to be added to strip 0x" 
				    << m_onlineHelper->channel_name(chid) << " (Eta = " << ieta << ", Phi = " << iphi << ")");
		    }
		    else {
		      ATH_MSG_WARNING( "Cannot find neighbour " << neighbours[i].dist << " to be added to strip 0x" 
				       << m_onlineHelper->channel_name(chid) << " (Eta = " << ieta << ", Phi = " << iphi << ")" );
		    } 
		  }//end if msgLvl(WARNING)
		  continue;
		}//end if !neighDIg
		if (neighDig==&febErrorDummy) { //Pointer comparison!!
		  ATH_MSG_WARNING("Neighbour " << neighbours[i].dist << " of strip " << m_onlineHelper->channel_name(chid)
				  << " (Eta = " << ieta << ", Phi = " << iphi <<"), has a FEB error. Ignored.");
		  continue;
		}
		
		//Check if neighbour is pulsed
		if (neighDig->isPulsed()) {
		  ATH_MSG_WARNING( "Neighbour " << neighbours[i].dist << " of strip " << m_onlineHelper->channel_name(chid)
				   << " (Eta = " << ieta << ", Phi = " << iphi <<", " << printMaxSample(neighDig) 
				   << ") is pulsed. Not used for correction." );
		  continue;
		}
		
		//Check if neighbour is on the bad-channel list
		if (m_dontUseForCorrMask.cellShouldBeMasked(bcCont,neighDig->hardwareID())) {
		  ATH_MSG_DEBUG("Neighbour " << neighbours[i].dist << " of strip " << m_onlineHelper->channel_name(chid) 
				<< " (Eta = " << ieta << ", Phi = " << iphi 
				<< ") is flagged by the LArBadChannelMaskingTool. Not used for correction.");
		  continue;
		}

		//Get Pedestal
		const float pedestal = larPedestal->pedestal(neighDig->hardwareID(),t_gain);
		if (pedestal <= (1.0+LArElecCalib::ERRORCODE)) {
		  ATH_MSG_ERROR( "No pedestal are available for neighbour " << neighbours[i].dist << " of strip " 
				 << m_onlineHelper->channel_name(chid) << " (Eta = " << ieta << ", Phi = " << iphi << "). Not used for correction!" );
		  continue;
		}
		//All went ok, fill struct
		neighbours[i].dig=neighDig;
		neighbours[i].ped=pedestal;

		//The weight is one, unless the neighbor-of-the-neighbor is pulsed too
		neighbours[i].weight=1.0;	
		//Now check if the neighbor-of-the-neighbor is pulsed to set a different weight.
		const int nnEta=ieta+2*neighbours[i].dist;
		if (nnEta>=(int)m_MINeta[bec] && nnEta<(int)m_MAXeta[bec]) {
		  const LArAccumulatedCalibDigit* nnDig=m_stripsLookUp[bec][iphi2][nnEta];
		  if (nnDig!=nullptr) {
		    //Could be also FebErrorDummy (which will always pretentd to be not pulsed)
		    if (nnDig->isPulsed()) {
		      ATH_MSG_VERBOSE("Neighbour " << neighbours[i].dist << " of strip " << m_onlineHelper->channel_name(chid) 
				      << " has another pulsed cell in the neighborhood. Setting weight to 0.5");
		      neighbours[i].weight=0.5;
		    }//end if isPulsed
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
		  ATH_MSG_VERBOSE("Neighbour " << neighbours[i].dist << " of strip " << m_onlineHelper->channel_name(chid) 
				  << " (Eta = " << ieta << ", Phi = " << iphi << ") does not exist.");
		  //no neighbor (end of calorimeter). Do nothing
		  continue;
		}
	      
		if (neighbours[i].dig) { //"Normal" case
		  correctSamples(SampleSums,neighbours[i]);
		  ATH_MSG_VERBOSE("Neighbour " << neighbours[i].dist << " of strip " << m_onlineHelper->channel_name(chid) 
				  << " (Eta = " << ieta << ", Phi = " << iphi << ") is used for correction");
		} //end if have pointer to neighbor cell
		else { //Neighbor not usable for some reason, try mirrored neighbor
		  const int j = 3-i; //get mirrored neighbor
		  ATH_MSG_INFO( "Neighbour " << neighbours[i].dist << " of strip " << m_onlineHelper->channel_name(chid) 
				<< " cannot be used. Taking mirrored neighbour " << neighbours[j].dist << " instead." );
		  if (neighbours[j].dig!=0 && neighbours[j].dig!=&inexistingDummy){
		    correctSamples(SampleSums,neighbours[j]);
		    ATH_MSG_VERBOSE("Mirrored neighbour " << neighbours[j].dist << " of strip " << m_onlineHelper->channel_name(chid) 
				    << " (Eta = " << ieta << ", Phi = " << iphi << ") is used for correction");
		  }//end if neighbours[j].dig
		  else {
		    ATH_MSG_WARNING( "Mirrored Neighbour " << neighbours[j].dist << " of strip " << m_onlineHelper->channel_name(chid) 
				     << " cannot be used too. No correction applied" );
		  } 
		}//end if no neighboring cell
	      }//end loop over the four neighbors
	    
	    
	    std::vector<uint64_t> SampleSumInt(NtotSamples);
	    bool unresonable=false;
	    std::size_t iPeak=std::max_element(currDig->sampleSum().begin(),currDig->sampleSum().end())-currDig->sampleSum().begin();
	    for (std::size_t SampleIndex=0;SampleIndex<NtotSamples; ++SampleIndex ) {
	      const double& thisSampleSum=SampleSums[SampleIndex];
	      const uint32_t& oldVal=currDig->sampleSum()[SampleIndex];
	      if (thisSampleSum<0) {
		unresonable=true;
		ATH_MSG_WARNING( "Channel 0x"  << MSG::hex << chid.get_compact() << MSG::dec
				  << " (Eta = " << ieta << ", Phi = " << iphi << ") Resulting ADC sample " << SampleIndex <<" negative! " 
				  << thisSampleSum << " instead of " << oldVal << " Not corrected." );
		break;
	      }

		if (SampleIndex==iPeak) { //check value of correction at peak
		  const float dev=(thisSampleSum-oldVal)/oldVal;
		  m_differences.add(currDig,dev);
		  if (fabs(dev)>m_acceptableDifference) {
		    unresonable=true;
		    ATH_MSG_WARNING("Strip " << m_onlineHelper->channel_name(chid) << " (Eta = " << ieta << ", Phi = " << iphi
				    << ") DAC=" << currDig->DAC() << ":  Resulting ADC sample " << SampleIndex <<" looks unreasonable: " 
				    << thisSampleSum << " instead of " << oldVal << " (off by " << dev*100 << "%)" 
				    << " (=" << thisSampleSum/NbTriggers << " -> " << oldVal/NbTriggers <<", ped=" 
				    << larPedestal->pedestal(chid,t_gain)
				    << " Not Corrected.");
		    break;
		  }//end if dev>m_acceptableDifference
		} // end if at peak sample
	      SampleSumInt[SampleIndex] = (uint64_t)(thisSampleSum);
	    }//End loop over samples

	 
	      if (unresonable) {
		m_uncorrectedIds.insert(chid);
		ATH_MSG_DEBUG("Correction for channel " << m_onlineHelper->channel_name(chid) << " failed");
	      }
	      else {
		ATH_MSG_VERBOSE("Correction for channel " << m_onlineHelper->channel_name(chid) << " DAC="<<currDig->DAC() << " succeeded " 
				<< currDig->sampleSum()[2] << "->" << SampleSumInt[2]);
		// FIXME: const_cast, modifying object in SG.
		const_cast<LArAccumulatedCalibDigit*>(currDig)->setSampleSum(SampleSumInt);
	      }      
	    }// end if-pulsed 
	  }//end loop over phi
	}//end loop over eta
      }//End loop over sides
    }//End loop over barrel-EC   
  } //End loop over all containers    

  if (nSaturation) {
    ATH_MSG_INFO( "Found " << nSaturation << " saturating digits in this event." );
  }
  return StatusCode::SUCCESS;
}

  

StatusCode LArStripsCrossTalkCorrector::finalize() { 
  if (msgLvl(MSG::WARNING) && m_uncorrectedIds.size()>0 ) {
    const LArBadChanBitPacking packing;
    ATH_MSG_WARNING( "The following "<<  m_uncorrectedIds.size() << " channels are (partly) uncorrected because of unresonable high correction:" );
    SG::ReadCondHandle<LArBadChannelCont> bcHdl{m_BCKey};
    const LArBadChannelCont* bcCont{*bcHdl};
    if(bcCont) {
       std::set<HWIdentifier>::const_iterator it=m_uncorrectedIds.begin();
       std::set<HWIdentifier>::const_iterator it_e=m_uncorrectedIds.end();
       for (;it!=it_e;++it) {
         LArBadChannel bc=bcCont->status(*it);
         ATH_MSG_WARNING( "Not corrected: " << m_onlineHelper->channel_name(*it) << " " << packing.stringStatus(bc) );
       }
    }
  }		    

  m_differences.print(msg(),m_onlineHelper);

  return StatusCode::SUCCESS;
}


std::string  LArStripsCrossTalkCorrector::printMaxSample(const LArAccumulatedCalibDigit* thisDig) {

  const std::vector<uint64_t>& ss=thisDig->sampleSum();
  std::vector<uint64_t>::const_iterator imax=std::max_element(ss.begin(),ss.end());
  std::ostringstream output;
  output << "S[" << imax-ss.begin() << "]=" << *imax/thisDig->nTriggers(); 
  return output.str();
}


StatusCode LArStripsCrossTalkCorrector::initKnownMissingFebs() {
unsigned nMissing=0;  

  for (unsigned bec : {0,1}) {    
    m_knownMissingFebs[bec].clear();
    m_knownMissingFebs[bec].resize(m_MAXeta[bec]);
  }
  
  //const std::vector<HWIdentifier> mf=m_badChannelTool->missingFEBs();
  SG::ReadCondHandle<LArBadFebCont> bfHdl{m_BFKey};
  const LArBadFebCont* bfCont{*bfHdl};
  if(!bfCont) {
     ATH_MSG_ERROR( "Do not have missing FEBs " << m_BFKey.key() );
     return StatusCode::FAILURE;
  }
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR( "Do not have cabling object LArOnOffIdMapping");
     return StatusCode::FAILURE;
  }

  LArBadChannelDBTools::BadFebVec mf;
  for (const auto& entry : bfCont->fullCont()) {
      const HWIdentifier hid=HWIdentifier(entry.first);
      mf.emplace_back(LArBadChannelDBTools::BadFebEntry(hid,entry.second));
  } 

  //ATH_MSG_DEBUG( "Got " << mf.size() << " missing FEBs" );
  LArBadChannelDBTools::BadFebVec::const_iterator it=mf.begin();
  LArBadChannelDBTools::BadFebVec::const_iterator it_e=mf.end();
  for (;it!=it_e;++it) {
    const HWIdentifier& fid=it->first;
    const LArBadFeb& bf=it->second;
    if (bf.deadAll() || bf.deadReadout() || bf.deactivatedInOKS()) {
      if (m_onlineHelper->isEMBchannel(fid) || m_onlineHelper->isEMECchannel(fid)) {
	      ATH_MSG_DEBUG( "Feb  " << MSG::hex << fid.get_compact() << MSG::dec << " reported as missing" );
	      const int nChan=m_onlineHelper->channelInSlotMax(fid);
        const unsigned bec=m_onlineHelper->barrel_ec(fid);
	      for (int c=0;c<nChan;++c) {
	        const HWIdentifier chid=m_onlineHelper->channel_Id(fid,c);
	        const Identifier id=cabling->cnvToIdentifier(chid);
	        const size_t ieta=getEtaIndex(id);
	        if (ieta==m_noIdx) continue; //Not a cell we care about
	        const size_t iphi=getPhiIndex(id);
	        if (iphi>=2*m_MAXphi || ieta>=m_MAXeta[bec]) {
	          ATH_MSG_FATAL( "Array index out of range: iphi=" << iphi << " (max " << m_MAXphi << "), ieta=" 
			        << ieta << "(max " << m_MAXeta[bec] << ")" );
	          return StatusCode::FAILURE;
	        }
	        m_knownMissingFebs[bec][ieta].set(iphi);
	        ++nMissing;
	      }//end loop over channels of one feb
      }//end if is barrel/endcap & EM
    }//end if is dead
  }//end loop over problematic febs
  ATH_MSG_INFO( "Number of known missing strip cells: "<< nMissing );
  m_missingFEBsDone=true;
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




/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArTPCnv/LArDigitContainerCnv_p2.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "Identifier/Identifier.h"
#include "LArIdentifier/LArOnlineID_Base.h"
#include "GaudiKernel/GaudiException.h"
#include "CxxUtils/AthUnlikelyMacros.h"

#include <algorithm>
#include <cstdlib>

namespace {

  void writeTwoBits(std::vector<unsigned char>& vec, const unsigned idx, const unsigned char val) {
    const auto qr=std::div(idx,4);
    const unsigned char maskval=(val&0x3) << 2*qr.rem;
    vec[qr.quot]|=maskval;
    return;
  }

  unsigned char readTwoBits(const std::vector<unsigned char>& vec, const unsigned idx) {
    const auto qr=std::div(idx,4);
    const unsigned char& v=vec[qr.quot]; 
    return (v>>2*qr.rem) & 0x3;
  }
}
    
LArDigitContainerCnv_p2::LArDigitContainerCnv_p2(const LArOnlineID_Base* idHelper) :
  m_idHelper(idHelper) {}

void
LArDigitContainerCnv_p2::persToTrans(const LArDigitContainer_p2* pers, 
				     LArDigitContainer* trans, MsgStream &/*log*/) const
{
  if (pers->m_nSamples==0 || pers->m_samples.size()==0) {
    //No data
    return;
  }

  const unsigned nMaxChannels=4*(pers->m_gain.size());
  trans->clear();
  trans->reserve(pers->m_samples.size()/pers->m_nSamples);

  std::vector<short>::const_iterator samplesIt=pers->m_samples.begin();
  for(unsigned idx=0;idx<nMaxChannels;idx++) {
    const unsigned char gain=readTwoBits(pers->m_gain,idx);
    if (gain!=0) {
      const HWIdentifier hwid=m_idHelper->channel_Id(IdentifierHash(idx));
      std::vector<short>::const_iterator samplesIt_e=samplesIt+pers->m_nSamples;
      std::vector<short> samples(samplesIt,samplesIt_e);
      trans->push_back(new LArDigit(hwid,(CaloGain::CaloGain)(gain-1),
				    std::move(samples)));
      samplesIt=samplesIt_e;
    }//end if gain!=0, eg channel present 
  }//end loop over all hashes
}


void
LArDigitContainerCnv_p2::transToPers(const LArDigitContainer* trans, 
				     LArDigitContainer_p2* pers, MsgStream &log) const {

  const unsigned int nChannels=trans->size();
  if (nChannels==0) {
    pers->m_nSamples=0; //Avoid undefined value in output file 
    return;
  }

  //Store the number of ADC samples from first digit
  pers->m_nSamples=(*(trans->begin()))->samples().size();
  const unsigned hashMax=m_idHelper->channelHashMax();
 
  //Resize persistant storage
  pers->m_gain.assign(hashMax/4,0);
  pers->m_samples.assign(pers->m_nSamples*nChannels,0);

  //Copy pointer to digits into a hash-indexed vector, nullptr for non-existing digits 
  std::vector<const LArDigit*> digitsvsHash(hashMax,nullptr); 
  for (const LArDigit* transDigit : *trans) {
    const IdentifierHash h=m_idHelper->channel_Hash(transDigit->hardwareID());
    if (ATH_UNLIKELY(transDigit->samples().size()!=pers->m_nSamples)) {
      log << MSG::ERROR << "Encountered LArDigit Container with varying numbers of ADC samples" << endmsg;
      throw GaudiException("Encountered LArDigit Container with varying numbers of ADC samples",
			   "LArDigitContainerCnv_p2",StatusCode::FAILURE);
    }
    digitsvsHash[h]=transDigit;
  }

  unsigned idx=0;
  std::vector<short>::iterator writeIt=pers->m_samples.begin();
  std::vector<const LArDigit*>::const_iterator it=digitsvsHash.begin();
  std::vector<const LArDigit*>::const_iterator it_e=digitsvsHash.end();
  for (;it!=it_e;++it,++idx) {
    const LArDigit* dig=*it;
    if (dig!=nullptr) {
      //A gain value of 0 means no data for this hash-value
      //Store gain+1
      writeTwoBits(pers->m_gain,idx,1+dig->gain()); 
      //Copy ADC samples
      writeIt=std::copy(dig->samples().begin(),dig->samples().end(),writeIt);
    }
  }
}

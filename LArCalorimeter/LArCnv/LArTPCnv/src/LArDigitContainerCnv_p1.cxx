/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArTPCnv/LArDigitContainerCnv_p1.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "Identifier/Identifier.h"
#include "LArTPCnv/LArDigitContainer_p1.h"



void
LArDigitContainerCnv_p1::persToTrans(const LArDigitContainer_p1* pers, 
				     LArDigitContainer* trans, MsgStream &log) const
{
  const unsigned nDigits=pers->m_channelID.size();
  if (nDigits != pers->m_gain.size() || nDigits != pers->m_nSamples.size()) {
    log << MSG::ERROR << "LArDigitContainer_p1 is inconsistent! " << std::endl;
    return;
  }

  //LArDigit digit;
  trans->clear();
  trans->reserve(nDigits);
  std::vector<unsigned short>::const_iterator sample_it=pers->m_samples.begin();
  std::vector<unsigned short>::const_iterator sample_it_e;

  for (unsigned i=0;i<nDigits;i++) {
    sample_it_e=sample_it+pers->m_nSamples[i];
    std::vector<short> samples (sample_it,sample_it_e);

    trans->push_back (new LArDigit (HWIdentifier(Identifier32(pers->m_channelID[i])),
                                    (CaloGain::CaloGain)pers->m_gain[i],
                                    std::move(samples)));

    sample_it = sample_it_e;
                                    
                                    
    //std:: cout << "Working on channel "<< i << "(" << std::hex 
    //	       << pers->m_channelID[i] <<std::dec << ")" <<std::endl;
  }
}


void
LArDigitContainerCnv_p1::transToPers(const LArDigitContainer* trans, 
				     LArDigitContainer_p1* pers, MsgStream &/*log*/) const
{
  
  unsigned int nDigits=trans->size();
  pers->m_channelID.reserve(nDigits);
  pers->m_gain.reserve(nDigits);
  pers->m_nSamples.reserve(nDigits);
  if (nDigits) 
    pers->m_samples.reserve( 
			    (*(trans->begin()))->nsamples() 
			    * nDigits);

  for (const LArDigit* transDigit : *trans) {
    pers->m_channelID.push_back(transDigit->hardwareID().get_identifier32().get_compact());
    pers->m_gain.push_back((unsigned char)transDigit->gain());
    pers->m_nSamples.push_back(transDigit->samples().size());
    pers->m_samples.insert (pers->m_samples.end(),
                            transDigit->samples().begin(),
                            transDigit->samples().end());
  }
}


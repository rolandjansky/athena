/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTPCnv/TBLArDigitContainerCnv_p1.h"
//#include "LArRawEvent/LArDigitContainer.h"
#include "TBEvent/TBLArDigitContainer.h"
#include "Identifier/Identifier.h"
#include "TBTPCnv/TBLArDigitContainer_p1.h"



void
TBLArDigitContainerCnv_p1::persToTrans(const TBLArDigitContainer_p1* pers, 
                                     TBLArDigitContainer* trans, MsgStream &log) const
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

  for (unsigned i=0;i<nDigits;i++) {
    std::vector<unsigned short>::const_iterator sample_it_e =
      sample_it+pers->m_nSamples[i];
    std::vector<short> samples (sample_it,sample_it_e);
    sample_it = sample_it_e;

    auto transDig = std::make_unique<LArDigit>
      (HWIdentifier(pers->m_channelID[i]),
       static_cast<CaloGain::CaloGain>(pers->m_gain[i]),
       std::move(samples));

    trans->push_back (std::move(transDig));
  }
}


void
TBLArDigitContainerCnv_p1::transToPers(const TBLArDigitContainer* trans, 
                                     TBLArDigitContainer_p1* pers, MsgStream &/*log*/) const
{
  
  unsigned int nDigits=trans->size();
  pers->m_channelID.reserve(nDigits);
  pers->m_gain.reserve(nDigits);
  pers->m_nSamples.reserve(nDigits);
  if (nDigits) 
    pers->m_samples.reserve( 
                            (*(trans->begin()))->nsamples() 
                            * nDigits);

  TBLArDigitContainer::const_iterator it=trans->begin();
  TBLArDigitContainer::const_iterator it_e=trans->end();
  //unsigned i=0;
  for (;it!=it_e;++it) {
    const LArDigit* transDigit=*it;
    pers->m_channelID.push_back(transDigit->hardwareID().get_identifier32().get_compact());
    pers->m_gain.push_back((unsigned char)transDigit->gain());
    pers->m_nSamples.push_back(transDigit->nsamples());
    pers->m_samples.insert (pers->m_samples.end(),
                            transDigit->samples().begin(),
                            transDigit->samples().end());
    //std:: cout << "Working on channel "<< i++ << "(" << std::hex 
    //         << transDigit->m_hardwareID.get_compact() <<std::dec << ")" <<std::endl;
  }
}

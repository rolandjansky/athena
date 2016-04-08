/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTPCnv/TBLArDigitContainerCnv_p1.h"
#define private public
#define protected public
//#include "LArRawEvent/LArDigitContainer.h"
#include "TBEvent/TBLArDigitContainer.h"

#undef private
#undef protected
#include "Identifier/Identifier.h"
#include "TBTPCnv/TBLArDigitContainer_p1.h"



void
TBLArDigitContainerCnv_p1::persToTrans(const TBLArDigitContainer_p1* pers, 
                                     TBLArDigitContainer* trans, MsgStream &log)
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
    LArDigit* transDig=new LArDigit();
    transDig->m_hardwareID=HWIdentifier(pers->m_channelID[i]);
    transDig->m_gain=(CaloGain::CaloGain)pers->m_gain[i];

    sample_it_e=sample_it+pers->m_nSamples[i];
    transDig->m_samples.assign(sample_it,sample_it_e);
    sample_it=sample_it_e;
    trans->push_back(transDig);
    //std:: cout << "Working on channel "<< i << "(" << std::hex 
    //         << pers->m_channelID[i] <<std::dec << ")" <<std::endl;
  }
}


void
TBLArDigitContainerCnv_p1::transToPers(const TBLArDigitContainer* trans, 
                                     TBLArDigitContainer_p1* pers, MsgStream &/*log*/)
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
  for (;it!=it_e;it++) {
    const LArDigit* transDigit=*it;
    pers->m_channelID.push_back(transDigit->m_hardwareID.get_compact());
    pers->m_gain.push_back((unsigned char)transDigit->m_gain);
    pers->m_nSamples.push_back(transDigit->m_samples.size());
    std::vector<short>::const_iterator it_dig=transDigit->m_samples.begin();
    std::vector<short>::const_iterator it_dig_e=transDigit->m_samples.end();
    for(;it_dig!=it_dig_e;it_dig++)
      pers->m_samples.push_back(*it_dig);
    //std:: cout << "Working on channel "<< i++ << "(" << std::hex 
    //         << transDigit->m_hardwareID.get_compact() <<std::dec << ")" <<std::endl;
  }
}




/* --------------------------------------------------------------------------
original file:
	http://alxr.usatlas.bnl.gov/lxr/source/atlas/LArCalorimeter/LArCnv/LArTPCnv/src/LArDigitContainerCnv_p1.cxx
-------------------------------------------------------------------------- * /

#include "LArTPCnv/LArDigitContainerCnv_p1.h"
#define private public
#define protected public
#include "LArRawEvent/LArDigitContainer.h"
#undef private
#undef protected
#include "Identifier/Identifier.h"
#include "LArTPCnv/LArDigitContainer_p1.h"



void
LArDigitContainerCnv_p1::persToTrans(const LArDigitContainer_p1* pers, 
                                     LArDigitContainer* trans, MsgStream &log)
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
        LArDigit* transDig=new LArDigit();
    transDig->m_hardwareID=HWIdentifier(pers->m_channelID[i]);
    transDig->m_gain=(CaloGain::CaloGain)pers->m_gain[i];

    sample_it_e=sample_it+pers->m_nSamples[i];
    transDig->m_samples.assign(sample_it,sample_it_e);
    sample_it=sample_it_e;
    trans->push_back(transDig);
    //std:: cout << "Working on channel "<< i << "(" << std::hex 
    //         << pers->m_channelID[i] <<std::dec << ")" <<std::endl;
  }
}


void
LArDigitContainerCnv_p1::transToPers(const LArDigitContainer* trans, 
                                     LArDigitContainer_p1* pers, MsgStream &/ *log* / )

{
  
  unsigned int nDigits=trans->size();
  pers->m_channelID.reserve(nDigits);
  pers->m_gain.reserve(nDigits);
  pers->m_nSamples.reserve(nDigits);
  if (nDigits) 
    pers->m_samples.reserve( 
                            (*(trans->begin()))->nsamples() 
                            * nDigits);

  LArDigitContainer::const_iterator it=trans->begin();
  LArDigitContainer::const_iterator it_e=trans->end();
  //unsigned i=0;
  for (;it!=it_e;it++) {
    LArDigit* transDigit=*it;
    pers->m_channelID.push_back(transDigit->m_hardwareID.get_compact());
    pers->m_gain.push_back((unsigned char)transDigit->m_gain);
    pers->m_nSamples.push_back(transDigit->m_samples.size());
    std::vector<short>::const_iterator it_dig=transDigit->m_samples.begin();
    std::vector<short>::const_iterator it_dig_e=transDigit->m_samples.end();
    for(;it_dig!=it_dig_e;it_dig++)
      pers->m_samples.push_back(*it_dig);
    //std:: cout << "Working on channel "<< i++ << "(" << std::hex 
    //         << transDigit->m_hardwareID.get_compact() <<std::dec << ")" <<std::endl;
  }
}


*/

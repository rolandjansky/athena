/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArTPCnv/LArSCDigitContainerCnv_p1.h"
#include "LArRawEvent/LArSCDigitContainer.h"
#include "Identifier/Identifier.h"
#include "LArTPCnv/LArSCDigitContainer_p1.h"



void
LArSCDigitContainerCnv_p1::persToTrans(const LArSCDigitContainer_p1* pers, 
				     LArSCDigitContainer* trans, MsgStream &log)
{
  const unsigned nDigits=pers->m_channelID.size();
  if (nDigits > 0 && (nDigits != pers->m_chan.size() || nDigits != pers->m_nSamples.size() || nDigits*pers->m_nSamples[0] != pers->m_bcids.size() || nDigits*pers->m_nSamples[0] != pers->m_samples.size())) {
    log << MSG::ERROR << "LArSCDigitContainer_p1 is inconsistent! " << std::endl;
    return;
  }

  trans->clear();
  trans->reserve(nDigits);
  std::vector<unsigned short>::const_iterator sample_it=pers->m_samples.begin();
  std::vector<unsigned short>::const_iterator sample_it_e;

  std::vector<unsigned short>::const_iterator bcid_it=pers->m_bcids.begin();
  std::vector<unsigned short>::const_iterator bcid_it_e;

  for (unsigned i=0;i<nDigits;i++) {
    sample_it_e=sample_it+pers->m_nSamples[i];
    std::vector<short> samples (sample_it,sample_it_e);

    bcid_it_e=bcid_it+pers->m_nSamples[i];
    std::vector<unsigned short> bcids (bcid_it,bcid_it_e);

    trans->push_back (new LArSCDigit (HWIdentifier(Identifier32(pers->m_channelID[i])),
                                    pers->m_chan[i], pers->m_sourceId[i],
                                    std::move(samples), std::move(bcids) ));

    sample_it = sample_it_e;
    bcid_it = bcid_it_e;                                  
                                    
  }
}


void
LArSCDigitContainerCnv_p1::transToPers(const LArSCDigitContainer* trans, 
				     LArSCDigitContainer_p1* pers, MsgStream &/*log*/)
{
  
  unsigned int nDigits=trans->size();
  pers->m_channelID.reserve(nDigits);
  pers->m_chan.reserve(nDigits);
  pers->m_sourceId.reserve(nDigits);
  pers->m_nSamples.reserve(nDigits);
  if (nDigits) { 
    pers->m_samples.reserve( 
			    (*(trans->begin()))->nsamples() 
			    * nDigits);
    pers->m_bcids.reserve( 
			    (*(trans->begin()))->nsamples() 
			    * nDigits);
  }
  for (const LArSCDigit* transDigit : *trans) {
    pers->m_channelID.push_back(transDigit->hardwareID().get_identifier32().get_compact());
    pers->m_chan.push_back(transDigit->Channel());
    pers->m_sourceId.push_back(transDigit->SourceId());
    pers->m_nSamples.push_back(transDigit->samples().size());
    pers->m_samples.insert (pers->m_samples.end(),
                            transDigit->samples().begin(),
                            transDigit->samples().end());
    pers->m_bcids.insert (pers->m_bcids.end(),
                            transDigit->BCId().begin(),
                            transDigit->BCId().end());
  }
}


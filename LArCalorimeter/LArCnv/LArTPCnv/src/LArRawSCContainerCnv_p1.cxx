/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArTPCnv/LArRawSCContainerCnv_p1.h"
#include "LArRawEvent/LArRawSCContainer.h"
#include "Identifier/Identifier.h"
#include "LArTPCnv/LArRawSCContainer_p1.h"



void
LArRawSCContainerCnv_p1::persToTrans(const LArRawSCContainer_p1* pers, 
				     LArRawSCContainer* trans, MsgStream &log)
{
  const unsigned nDigits=pers->m_channelID.size();
  if (nDigits != pers->m_chan.size() || nDigits != pers->m_nEnergies.size()) {
    log << MSG::ERROR << "LArRawSCContainer_p1 is inconsistent! " << std::endl;
    return;
  }

  trans->clear();
  trans->reserve(nDigits);
  std::vector<int>::const_iterator en_it=pers->m_energies.begin();
  std::vector<int>::const_iterator en_it_e;

  std::vector<unsigned short>::const_iterator bcid_it=pers->m_bcids.begin();
  std::vector<unsigned short>::const_iterator bcid_it_e;

  std::vector<bool>::const_iterator satur_it=pers->m_satur.begin();
  std::vector<bool>::const_iterator satur_it_e;

  for (unsigned i=0;i<nDigits;i++) {
    en_it_e=en_it+pers->m_nEnergies[i];
    std::vector<int> energies (en_it,en_it_e);

    bcid_it_e=bcid_it+pers->m_nEnergies[i];
    std::vector<unsigned short> bcids (bcid_it,bcid_it_e);

    satur_it_e=satur_it+pers->m_nEnergies[i];
    std::vector<bool> satur (satur_it,satur_it_e);

    trans->push_back (new LArRawSC (HWIdentifier(Identifier32(pers->m_channelID[i])),
                                    pers->m_chan[i],
                                    pers->m_sourceId[i],
                                    std::move(energies),
                                    std::move(bcids),
                                    std::move(satur)));

    en_it = en_it_e;
    bcid_it = bcid_it_e;
    satur_it = satur_it_e;
                                    
  }
}


void
LArRawSCContainerCnv_p1::transToPers(const LArRawSCContainer* trans, 
				     LArRawSCContainer_p1* pers, MsgStream &/*log*/)
{
  
  unsigned int nDigits=trans->size();
  pers->m_channelID.reserve(nDigits);
  pers->m_chan.reserve(nDigits);
  pers->m_sourceId.reserve(nDigits);
  pers->m_nEnergies.reserve(nDigits);
  if (nDigits) {
    pers->m_energies.reserve( 
			    (*(trans->begin()))->nsamples() 
			    * nDigits);
    pers->m_bcids.reserve( 
			    (*(trans->begin()))->nsamples() 
			    * nDigits);
    pers->m_satur.reserve( 
			    (*(trans->begin()))->nsamples() 
			    * nDigits);
  }
  for (const LArRawSC* transDigit : *trans) {
    pers->m_channelID.push_back(transDigit->hardwareID().get_identifier32().get_compact());
    pers->m_chan.push_back(transDigit->chan());
    pers->m_sourceId.push_back(transDigit->SourceId());
    pers->m_nEnergies.push_back(transDigit->energies().size());
    pers->m_energies.insert (pers->m_energies.end(),
                            transDigit->energies().begin(),
                            transDigit->energies().end());
    pers->m_bcids.insert (pers->m_bcids.end(),
                            transDigit->bcids().begin(),
                            transDigit->bcids().end());
    pers->m_satur.insert (pers->m_satur.end(),
                            transDigit->satur().begin(),
                            transDigit->satur().end());
  }
}


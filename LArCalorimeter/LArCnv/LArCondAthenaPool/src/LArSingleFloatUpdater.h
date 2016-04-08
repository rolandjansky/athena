/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARSINGLEFLOATUPDATER_H
#define LARSINGLEFLOATUPDATER_H

//class LArMphysOverMcalP1;
#include "LArRawConditions/LArMphysOverMcalP.h"
#include "LArRawConditions/LArDAC2uAP.h"
#include "LArRawConditions/LAruA2MeVP.h"
#include "LArRawConditions/LArfSamplP.h"


class LArDAC2uAP;

class LArSingleFloatUpdater {
 public:
  template<class P>
  void update(const LArConditionsSubset<P>* old,
	      LArConditionsSubset<LArSingleFloatP>* newObj) {

       // Get the number of febs and corrections
    unsigned int nFebs       = old->m_subset.size();
    unsigned int nCorrs      = old->m_correctionVec.size();

    //log << MSG::DEBUG << "LArMphysOverMcalCompleteCnv::createTransient old 1, nFebs, nCorrs " 
    //    << nFebs << " " << nCorrs << endreq; 

    // Copy conditions

    // Resize subset
    newObj->m_subset.resize(nFebs);
    
    // Loop over febs
    for (unsigned int i = 0; i < nFebs; ++i){
        newObj->m_subset[i].first = old->m_subset[i].first;
	unsigned nChannels=old->m_subset[i].second.size();
        newObj->m_subset[i].second.resize(nChannels);
        // Loop over channels in feb
        for (unsigned int j = 0; j < nChannels; ++j)
	  newObj->m_subset[i].second[j].m_data=getPayload(old->m_subset[i].second[j]);
    }// end loop over Febs

    //log << MSG::DEBUG << "LArMphysOverMcalCompleteCnv::createTransient old 2 " << old << endreq; 

    // Copy corrections
    newObj->m_correctionVec.resize(nCorrs);

    // Loop over corrections
    for (unsigned int i = 0; i < nCorrs; ++i){
      newObj->m_correctionVec[i].first = old->m_correctionVec[i].first;
      newObj->m_correctionVec[i].second.m_data = getPayload(old->m_correctionVec[i].second);
    }

    // Copy the rest
    newObj->m_gain          = old->m_gain; 
    newObj->m_channel       = old->m_channel;
    newObj->m_groupingType  = old->m_groupingType;
  }

 private:
  float getPayload(const LArMphysOverMcalP& p) {
    return p.m_MphysOverMcal;
  }

//  float getPayload(const LArMphysOverMcalP1& p) {
//  return p.m_MphysOverMcal;
//}

  float getPayload(const LArDAC2uAP& p) {
    return p.m_DAC2uA;
  }
  float getPayload(const LAruA2MeVP& p) {
    return p.m_uA2MeV;
  }

  float getPayload(const LArfSamplP& p) {
    return p.m_fSampl;
  }

  
};

#endif


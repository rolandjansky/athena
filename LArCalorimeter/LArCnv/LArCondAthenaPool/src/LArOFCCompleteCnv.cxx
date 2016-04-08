/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArOFCCompleteCnv.cxx
 * @brief AthenaPool converter LArOFCComplete
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "LArOFCCompleteCnv.h"
#include "LArCondTPCnv/LArOFCSubsetCnv_p1.h"

static LArOFCSubsetCnv_p1   TPconverter;

LArOFCSubset_p1*
LArOFCCompleteCnv::createPersistent (LArOFCTransType* transObj)
{
    MsgStream log(messageService(), "LArOFCCompleteCnv" ); 
    //log << MSG::DEBUG << "LArOFCComplete write" << endreq;
    LArOFCPersType* persObj = TPconverter.createPersistent( transObj, log );
    //log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}

LArConditionsSubset<LArOFCP1>*
LArOFCCompleteCnv::createTransient () {
    static pool::Guid   p1_guid("0A1DE2E2-90E4-4A24-BC6E-2092EDC9FDF6");
    static pool::Guid   p0_guid("3E5389EF-D163-4099-91D9-D3F0EE06C1CD");

    if( compareClassGuid(p1_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< LArOFCSubset_p1 > col_vect( poolReadObject< LArOFCSubset_p1 >() );
        MsgStream log(messageService(), "LArOFCCompleteCnv" ); 
        //log << MSG::INFO << "Reading LArOFCSubset_p1" << endreq; 
        return TPconverter.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // subset from before TP separation

        MsgStream log(messageService(), "LArOFCCompleteCnv" ); 
        log << MSG::DEBUG << "Reading LArOFCSubset (original)" << endreq; 

        std::auto_ptr< LArConditionsSubset<LArOFCP> > subset ( poolReadObject< LArConditionsSubset<LArOFCP> >() );
        // Here we must convert from LArOFCP to LArOFCP1
        
        log << MSG::DEBUG << "subset ptr " << subset.get() << endreq; 

        return (createTransient(subset.get()));

    } 
    throw std::runtime_error("Unsupported persistent version of LArOFCCompleteCnv");
}

LArConditionsSubset<LArOFCP1>* 
LArOFCCompleteCnv::createTransient(LArConditionsSubset<LArOFCP>* orig)
{

    MsgStream log(messageService(), "LArOFCCompleteCnv" ); 
    log << MSG::DEBUG << "LArOFCCompleteCnv::createTransient orig " << orig << endreq; 

    LArConditionsSubset<LArOFCP1>* result = new LArConditionsSubset<LArOFCP1>();
    
    // Copy LArOFCP subset to LArOFCP1
    LArOFCCopy copier;
    copier.copyOldtoNew(orig, result);
    
    return (result);
}
    
// Copy LArOFCP subset to LArOFCP1
void 
LArOFCCopy::copyOldtoNew(const LArConditionsSubset<LArOFCP>* oldOFC,
			   LArConditionsSubset<LArOFCP1>* newOFC)
{
    // Get the number of febs and corrections
    unsigned int nFebs       = oldOFC->m_subset.size();
    unsigned int nCorrs      = oldOFC->m_correctionVec.size();

    //log << MSG::DEBUG << "LArOFCCompleteCnv::createTransient oldOFC 1, nFebs, nCorrs " 
    //    << nFebs << " " << nCorrs << endreq; 

    // Copy conditions

    // Resize subset
    newOFC->m_subset.resize(nFebs);
    
    // Loop over febs
    for (unsigned int i = 0; i < nFebs; ++i){
        newOFC->m_subset[i].first = oldOFC->m_subset[i].first;
	unsigned nChannels=oldOFC->m_subset[i].second.size();
        newOFC->m_subset[i].second.resize(nChannels);
        // Loop over channels in feb
        for (unsigned int j = 0; j < nChannels; ++j){
          LArOFCP1 tmp (oldOFC->m_subset[i].second[j].m_timeOffset,
                        25./24,
                        oldOFC->m_subset[i].second[j].m_vOFC_a,
                        oldOFC->m_subset[i].second[j].m_vOFC_b);
          newOFC->m_subset[i].second[j] = tmp;
        }
    }

    //log << MSG::DEBUG << "LArOFCCompleteCnv::createTransient oldOFC 2 " << oldOFC << endreq; 

    // Copy corrections
    newOFC->m_correctionVec.resize(nCorrs);

    // Loop over corrections
    for (unsigned int i = 0; i < nCorrs; ++i){
      newOFC->m_correctionVec[i].first = oldOFC->m_correctionVec[i].first;

      const LArOFCP& old = oldOFC->m_correctionVec[i].second;
      LArOFCP1 tmp (old.m_timeOffset,
                    25./24.,
                    old.m_vOFC_a,
                    old.m_vOFC_b);
      newOFC->m_correctionVec[i].second.setFrom (tmp);
    }

    // Copy the rest
    newOFC->m_gain          = oldOFC->m_gain; 
    newOFC->m_channel       = oldOFC->m_channel;
    newOFC->m_groupingType  = oldOFC->m_groupingType;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArRampCompleteCnv.cxx
 * @brief AthenaPool converter LArRampComplete
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "LArRampCompleteCnv.h"
#include "LArCondTPCnv/LArRampSubsetCnv_p1.h"

static LArRampSubsetCnv_p1   TPconverter;

LArRampSubset_p1*
LArRampCompleteCnv::createPersistent (LArRampTransType* transObj)
{
    MsgStream log(messageService(), "LArRampCompleteCnv" ); 
    //log << MSG::DEBUG << "LArRampComplete write" << endreq;
    LArRampPersType* persObj = TPconverter.createPersistent( transObj, log );
    //log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}

LArConditionsSubset<LArRampP1>*
LArRampCompleteCnv::createTransient ()
{
    static pool::Guid   p1_guid("BB629283-BDF5-4FA8-93F4-5D8C2F201364");
    static pool::Guid   p0_guid("4019776D-D528-4401-9CBD-7956C4B00607");
    if( compareClassGuid(p1_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< LArRampSubset_p1 > col_vect( poolReadObject< LArRampSubset_p1 >() );
        MsgStream log(messageService(), "LArRampCompleteCnv" ); 
        //log << MSG::INFO << "Reading LArRampSubset_p1" << endreq; 
        return TPconverter.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // subset from before TP separation

        MsgStream log(messageService(), "LArRampCompleteCnv" ); 
        log << MSG::DEBUG << "Reading LArRampSubset (original)" << endreq; 

        std::auto_ptr< LArConditionsSubset<LArRampP> > subset ( poolReadObject< LArConditionsSubset<LArRampP> >() );
        // Here we must convert from LArRampP to LArRampP1
        
        log << MSG::DEBUG << "subset ptr " << subset.get() << endreq; 

        return (createTransient(subset.get()));

    } 
    throw std::runtime_error("Unsupported persistent version of LArRampCompleteCnv");
}

LArConditionsSubset<LArRampP1>* 
LArRampCompleteCnv::createTransient(LArConditionsSubset<LArRampP>* orig)
{

    MsgStream log(messageService(), "LArRampCompleteCnv" ); 
    log << MSG::DEBUG << "LArRampCompleteCnv::createTransient orig " << orig << endreq; 

    LArConditionsSubset<LArRampP1>* result = new LArConditionsSubset<LArRampP1>();
    
    // Copy LArRampP subset to LArRampP1
    LArRampCopy copier;
    copier.copyOldtoNew(orig, result);

    return (result);
}
    
// Copy LArRampP subset to LArRampP1
void 
LArRampCopy::copyOldtoNew(const LArConditionsSubset<LArRampP>* oldRamp,
			   LArConditionsSubset<LArRampP1>* newRamp)
{
    // Get the number of febs and corrections
    unsigned int nFebs       = oldRamp->m_subset.size();
    unsigned int nCorrs      = oldRamp->m_correctionVec.size();

    //log << MSG::DEBUG << "LArRampCompleteCnv::createTransient oldRamp 1, nFebs, nCorrs " 
    //    << nFebs << " " << nCorrs << endreq; 

    // Copy conditions

    // Resize subset
    newRamp->m_subset.resize(nFebs);
    
    // Loop over febs
    for (unsigned int i = 0; i < nFebs; ++i){
        newRamp->m_subset[i].first = oldRamp->m_subset[i].first;
	unsigned nChannels=oldRamp->m_subset[i].second.size();
        newRamp->m_subset[i].second.resize(nChannels);
        // Loop over channels in feb
        for (unsigned int j = 0; j < nChannels; ++j){
            // reserve space for ramp vec
            unsigned int nRamps = oldRamp->m_subset[i].second[j].m_vRamp.size();
            newRamp->m_subset[i].second[j].m_vRamp.resize(nRamps);
            // Loop over ramps per channel
            for (unsigned int k = 0; k < nRamps; ++k){
                newRamp->m_subset[i].second[j].m_vRamp[k] = 
                    oldRamp->m_subset[i].second[j].m_vRamp[k];
            }
        }
    }

    //log << MSG::DEBUG << "LArRampCompleteCnv::createTransient oldRamp 2 " << oldRamp << endreq; 

    // Copy corrections
    newRamp->m_correctionVec.resize(nCorrs);

    // Loop over corrections
    for (unsigned int i = 0; i < nCorrs; ++i){
        newRamp->m_correctionVec[i].first = oldRamp->m_correctionVec[i].first;
        // reserve space for ramp vec
        unsigned int nRamps = oldRamp->m_correctionVec[i].second.m_vRamp.size();
        newRamp->m_correctionVec[i].second.m_vRamp.resize(nRamps);
        // Loop over ramps per channel
        for (unsigned int k = 0; k < nRamps; ++k){
            newRamp->m_correctionVec[i].second.m_vRamp[k] = 
                oldRamp->m_correctionVec[i].second.m_vRamp[k];
        }
    }

    // Copy the rest
    newRamp->m_gain          = oldRamp->m_gain; 
    newRamp->m_channel       = oldRamp->m_channel;
    newRamp->m_groupingType  = oldRamp->m_groupingType;
}


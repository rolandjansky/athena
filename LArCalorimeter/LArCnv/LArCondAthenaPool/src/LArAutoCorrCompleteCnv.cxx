/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArAutoCorrCompleteCnv.cxx
 * @brief AthenaPool converter LArAutoCorrComplete
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "LArAutoCorrCompleteCnv.h"
#include "LArCondTPCnv/LArAutoCorrSubsetCnv_p1.h"

static LArAutoCorrSubsetCnv_p1   TPconverter;

LArAutoCorrSubset_p1*
LArAutoCorrCompleteCnv::createPersistent (LArAutoCorrTransType* transObj)
{
    MsgStream log(messageService(), "LArAutoCorrCompleteCnv" ); 
    //log << MSG::DEBUG << "LArAutoCorrComplete write" << endreq;
    LArAutoCorrPersType* persObj = TPconverter.createPersistent( transObj, log );
    //log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}

LArConditionsSubset<LArAutoCorrP1>*
LArAutoCorrCompleteCnv::createTransient ()
{
    static pool::Guid   p1_guid("FA16A69D-241E-40F3-B710-77A95937E394");
    static pool::Guid   p0_guid("4E7E36E9-2121-4327-88C5-8A516D6D6D2A");
    if( compareClassGuid(p1_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< LArAutoCorrSubset_p1 > col_vect( poolReadObject< LArAutoCorrSubset_p1 >() );
        MsgStream log(messageService(), "LArAutoCorrCompleteCnv" ); 
        //log << MSG::INFO << "Reading LArAutoCorrSubset_p1" << endreq; 
        return TPconverter.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // subset from before TP separation

        MsgStream log(messageService(), "LArAutoCorrCompleteCnv" ); 
        log << MSG::DEBUG << "Reading LArAutoCorrSubset (original)" << endreq; 

        std::auto_ptr< LArConditionsSubset<LArAutoCorrP> > subset ( poolReadObject< LArConditionsSubset<LArAutoCorrP> >() );
        // Here we must convert from LArAutoCorrP to LArAutoCorrP1
        
        log << MSG::DEBUG << "subset ptr " << subset.get() << endreq; 

        return (createTransient(subset.get()));

    } 
    throw std::runtime_error("Unsupported persistent version of LArAutoCorrCompleteCnv");
}

LArConditionsSubset<LArAutoCorrP1>* 
LArAutoCorrCompleteCnv::createTransient(LArConditionsSubset<LArAutoCorrP>* orig)
{

    MsgStream log(messageService(), "LArAutoCorrCompleteCnv" ); 
    log << MSG::DEBUG << "LArAutoCorrCompleteCnv::createTransient orig " << orig << endreq; 

    LArConditionsSubset<LArAutoCorrP1>* result = new LArConditionsSubset<LArAutoCorrP1>();
    
    // Copy LArAutoCorrP subset to LArAutoCorrP1
    LArAutoCorrCopy copier;
    copier.copyOldtoNew(orig, result);
    
    return (result);
}
    

// Copy LArAutoCorrP subset to LArAutoCorrP1
void 
LArAutoCorrCopy::copyOldtoNew(const LArConditionsSubset<LArAutoCorrP>* oldAutoCorr,
			   LArConditionsSubset<LArAutoCorrP1>* newAutoCorr)
{
    // Get the number of febs and corrections
    unsigned int nFebs       = oldAutoCorr->m_subset.size();
    unsigned int nCorrs      = oldAutoCorr->m_correctionVec.size();

    //log << MSG::DEBUG << "LArAutoCorrCompleteCnv::createTransient oldAutoCorr 1, nFebs, nCorrs " 
    //    << nFebs << " " << nCorrs << endreq; 

    // Copy conditions

    // Resize subset
    newAutoCorr->m_subset.resize(nFebs);
    
    // Loop over febs
    for (unsigned int i = 0; i < nFebs; ++i){
        newAutoCorr->m_subset[i].first = oldAutoCorr->m_subset[i].first;
	unsigned nChannels=oldAutoCorr->m_subset[i].second.size();
        newAutoCorr->m_subset[i].second.resize(nChannels);
        // Loop over channels in feb
        for (unsigned int j = 0; j < nChannels; ++j){
            // reserve space for autocorr vec
            unsigned int nAutoCorrs = oldAutoCorr->m_subset[i].second[j].m_vAutoCorr.size();
            newAutoCorr->m_subset[i].second[j].m_vAutoCorr.resize(nAutoCorrs);
            // Loop over autocorrs per channel
            for (unsigned int k = 0; k < nAutoCorrs; ++k){
                newAutoCorr->m_subset[i].second[j].m_vAutoCorr[k] = 
                    oldAutoCorr->m_subset[i].second[j].m_vAutoCorr[k];
            }
        }
    }

    // log << MSG::DEBUG << "LArAutoCorrCompleteCnv::createTransient oldAutoCorr 2 " << oldAutoCorr << endreq; 

    // Copy corrections
    newAutoCorr->m_correctionVec.resize(nCorrs);

    // Loop over corrections
    for (unsigned int i = 0; i < nCorrs; ++i){
        newAutoCorr->m_correctionVec[i].first = oldAutoCorr->m_correctionVec[i].first;
        // reserve space for autocorr vec
        unsigned int nAutoCorrs = oldAutoCorr->m_correctionVec[i].second.m_vAutoCorr.size();
        newAutoCorr->m_correctionVec[i].second.m_vAutoCorr.resize(nAutoCorrs);
        // Loop over autocorrs per channel
        for (unsigned int k = 0; k < nAutoCorrs; ++k){
            newAutoCorr->m_correctionVec[i].second.m_vAutoCorr[k] = 
                oldAutoCorr->m_correctionVec[i].second.m_vAutoCorr[k];
        }
    }

    // Copy the rest
    newAutoCorr->m_gain          = oldAutoCorr->m_gain; 
    newAutoCorr->m_channel       = oldAutoCorr->m_channel;
    newAutoCorr->m_groupingType  = oldAutoCorr->m_groupingType;
}

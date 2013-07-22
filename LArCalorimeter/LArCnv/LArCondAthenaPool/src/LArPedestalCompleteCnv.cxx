/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArPedestalCompleteCnv.cxx
 * @brief AthenaPool converter LArPedestalComplete
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "LArPedestalCompleteCnv.h"
#include "LArCondTPCnv/LArPedestalSubsetCnv_p1.h"

static LArPedestalSubsetCnv_p1   TPconverter1;
static LArPedestalSubsetCnv_p2   TPconverter2;


LArPedestalSubset_p2*
LArPedestalCompleteCnv::createPersistent (LArPedTransType* transObj)
{
    MsgStream log(messageService(), "LArPedestalCompleteCnv" ); 
    //log << MSG::DEBUG << "LArPedestalComplete write" << endreq;
    return TPconverter2.createPersistent( transObj, log );
}

LArConditionsSubset<LArPedestalP1>*
LArPedestalCompleteCnv::createTransient ()
{
    static pool::Guid p2_guid("8BD3FE69-A3C4-418C-ACB9-E362CE524353");
    static pool::Guid   p1_guid("E365F747-264B-4A0C-B80A-570DBE099881");
    static pool::Guid   p0_guid("E17191DD-4C0A-4B1A-AE49-7D587C6BE3EE");

    if (compareClassGuid(p2_guid)) {
      // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< LArPedestalSubset_p2 > col_vect( poolReadObject< LArPedestalSubset_p2 >() );
      MsgStream log(messageService(), "LArPedestalCompleteCnv" ); 
      //log << MSG::INFO << "Reading LArPedestalSubset_p1" << endreq; 
      return TPconverter2.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p1_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< LArPedestalSubset_p1 > col_vect( poolReadObject< LArPedestalSubset_p1 >() );
        MsgStream log(messageService(), "LArPedestalCompleteCnv" ); 
        //log << MSG::INFO << "Reading LArPedestalSubset_p1" << endreq; 
        return TPconverter1.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // subset from before TP separation

        MsgStream log(messageService(), "LArPedestalCompleteCnv" ); 
        log << MSG::DEBUG << "Reading LArPedestalSubset (original)" << endreq; 

        std::auto_ptr< LArConditionsSubset<LArPedestalP> > subset ( poolReadObject< LArConditionsSubset<LArPedestalP> >() );
        // Here we must convert from LArPedestalP to LArPedestalP1
        
        log << MSG::DEBUG << "subset ptr " << subset.get() << endreq; 

        return (createTransient(subset.get()));

    } 
    throw std::runtime_error("Unsupported persistent version of LArPedestalCompleteCnv");
}

LArConditionsSubset<LArPedestalP1>* 
LArPedestalCompleteCnv::createTransient(LArConditionsSubset<LArPedestalP>* orig)
{

    MsgStream log(messageService(), "LArPedestalCompleteCnv" ); 
    log << MSG::DEBUG << "LArPedestalCompleteCnv::createTransient orig " << orig << endreq; 

    LArConditionsSubset<LArPedestalP1>* result = new LArConditionsSubset<LArPedestalP1>();
    
    // Copy LArPedestalP subset to LArPedestalP1
    LArPedestalCopy copier;
    copier.copyOldtoNew(orig, result);

    return (result);
}
    

// Copy LArPedestalP subset to LArPedestalP1
void 
LArPedestalCopy::copyOldtoNew(const LArConditionsSubset<LArPedestalP>* oldPedestal,
                              LArConditionsSubset<LArPedestalP1>* newPedestal)
{
    // Get the number of febs and corrections
    unsigned int nFebs       = oldPedestal->m_subset.size();
    unsigned int nCorrs      = oldPedestal->m_correctionVec.size();

    //log << MSG::DEBUG << "LArPedestalCompleteCnv::createTransient oldPedestal 1, nFebs, nCorrs " 
    //    << nFebs << " " << nCorrs << endreq; 

    // Copy conditions

    // Resize subset
    newPedestal->m_subset.resize(nFebs);
    
    // Loop over febs
    for (unsigned int i = 0; i < nFebs; ++i){
        newPedestal->m_subset[i].first = oldPedestal->m_subset[i].first;
	unsigned nChannels=oldPedestal->m_subset[i].second.size();
        newPedestal->m_subset[i].second.resize(nChannels);
        // Loop over channels in feb
        for (unsigned int j = 0; j < nChannels; ++j){
	  if (oldPedestal->m_subset[i].second[j].m_vPedestal.size()>0)
	    newPedestal->m_subset[i].second[j].m_Pedestal=oldPedestal->m_subset[i].second[j].m_vPedestal[0];
	  if (oldPedestal->m_subset[i].second[j].m_vPedestalRMS.size()>0)
	    newPedestal->m_subset[i].second[j].m_PedestalRMS=oldPedestal->m_subset[i].second[j].m_vPedestalRMS[0];
	  
	}//end loop over channels in feb
    }//end loop over febs
    
    // Copy corrections
    newPedestal->m_correctionVec.resize(nCorrs);

    // Loop over corrections
    for (unsigned int i = 0; i < nCorrs; ++i){
        newPedestal->m_correctionVec[i].first = oldPedestal->m_correctionVec[i].first;
	if (oldPedestal->m_correctionVec[i].second.m_vPedestal.size()>0)
	  newPedestal->m_correctionVec[i].second.m_Pedestal=oldPedestal->m_correctionVec[i].second.m_vPedestal[0];
	
	if (oldPedestal->m_correctionVec[i].second.m_vPedestalRMS.size()>0)
	  newPedestal->m_correctionVec[i].second.m_PedestalRMS=oldPedestal->m_correctionVec[i].second.m_vPedestalRMS[0];
    }//end loop over corrections


    // Copy the rest
    newPedestal->m_gain          = oldPedestal->m_gain; 
    newPedestal->m_channel       = oldPedestal->m_channel;
    newPedestal->m_groupingType  = oldPedestal->m_groupingType;
}



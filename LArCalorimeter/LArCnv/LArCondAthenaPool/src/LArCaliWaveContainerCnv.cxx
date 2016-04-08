/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArWaveCnv.cxx
 * @brief AthenaPool converter LArWave
 * @author Ilija Vukotic <ivukotic@cern.ch>
 */

#define private public
#define protected public
#include "LArRawConditions/LArConditionsSubset.h"
#undef private
#undef protected  

#include "LArCaliWaveContainerCnv.h"
#include "LArCondTPCnv/LArCaliWaveSubsetCnv_p1.h"
#include "LArCondTPCnv/LArCaliWaveSubsetCnv_p2.h"

static LArCaliWaveSubsetCnv_p1   TPconverter1;
static LArCaliWaveSubsetCnv_p2   TPconverter2;

LArCaliWavePersType*
LArCaliWaveContainerCnv::createPersistent (LArCaliWaveTransType* transObj)
{
    MsgStream log(messageService(), "LArCaliWaveContainerCnv" ); 
    log << MSG::DEBUG << "WRITING LArCaliWaveContainer" << endreq;
    LArCaliWavePersType* persObj = TPconverter2.createPersistent( transObj, log );
    log << MSG::DEBUG << "WRITING LArCaliWaveContainer Success !" << endreq;
    return persObj; 
}

LArConditionsSubset<LArCaliWaveVec>*
LArCaliWaveContainerCnv::createTransient ()
{   
    MsgStream log(messageService(), "LArCaliWaveContainerCnv" ); 

    static pool::Guid   p2_guid("6CF01BBF-85A9-45FA-B321-6A98B0D719FB");
    static pool::Guid   p1_guid("9E61BE2D-3274-4459-A5C2-3BBFB7056EBA");
	static pool::Guid   p0_guid("ECB4AD6C-FF3A-4255-A0E3-7BD566B96A77");
	
    if( compareClassGuid(p2_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< LArCaliWaveSubset_p2 > col_vect( poolReadObject< LArCaliWaveSubset_p2 >() );
        log << MSG::DEBUG << "READING LArCaliWaveSubset_p2" << endreq; 
		LArCaliWaveTransType* transObj = TPconverter2.createTransient( col_vect.get(), log );
        log << MSG::DEBUG << "READING LArCaliWaveSubset_p2 Success !" << endreq;
        return transObj;
    }    
	else if( compareClassGuid(p1_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< LArCaliWaveSubset_p1 > col_vect( poolReadObject< LArCaliWaveSubset_p1 >() );
        log << MSG::DEBUG << "READING LArCaliWaveSubset_p1" << endreq; 
		LArCaliWaveTransType* transObj = TPconverter1.createTransient( col_vect.get(), log );
        log << MSG::DEBUG << "READING LArCaliWaveSubset_p1 Success !" << endreq;
        return transObj;
    }
    else if(compareClassGuid(p0_guid)){
        MsgStream log(messageService(), "LArCaliWaveContainerCnv" ); 
        log << MSG::DEBUG << " READING LArCaliWaveSubset (before TP split)" << endreq; 

        std::auto_ptr< LArConditionsSubset<LArCaliWaveVec> > subset ( poolReadObject< LArConditionsSubset<LArCaliWaveVec> >() );
        
        return (createTransient(subset.get()));
    } 
    throw std::runtime_error("Unsupported persistent version of LArCaliWaveCnv");
}




LArConditionsSubset<LArCaliWaveVec>* 
LArCaliWaveContainerCnv::createTransient(LArConditionsSubset<LArCaliWaveVec>* orig)
{

    MsgStream log(messageService(), "LArPedestalCompleteCnv" ); 
    log << MSG::DEBUG << "LArCaliWaveContainerCnv::createTransient orig " << orig << endreq; 

    LArConditionsSubset<LArCaliWaveVec>* result = new LArConditionsSubset<LArCaliWaveVec>();
    
    // Copy from orig to result
	
    result->m_gain          = orig->m_gain; 
    result->m_channel       = orig->m_channel;
    result->m_groupingType  = orig->m_groupingType;
	
	
	for (unsigned int w=0;w<orig->m_subset.size();++w){ // copy of subset
		std::vector<LArCaliWaveVec> ochvec;
		for (unsigned int q=0;q<orig->m_subset[w].second.size();++q){
			ochvec.push_back(orig->m_subset[w].second[q]);
			}
		unsigned int ofebid=orig->m_subset[w].first;
		std::pair<unsigned int, std::vector<LArCaliWaveVec> > opair(ofebid,ochvec);
		result->m_subset.push_back(opair);
		}
		
	
	for(unsigned int w=0;w<orig->m_correctionVec.size();++w){ // copy of correctionVec
		LArCaliWaveVec ovec=orig->m_correctionVec[w].second;
		unsigned int ofebid=orig->m_correctionVec[w].first;
		std::pair<unsigned int, LArCaliWaveVec> opair(ofebid, ovec);
		result->m_correctionVec.push_back(opair);
		}		
	
	result->m_subsetMap.swap(orig->m_subsetMap); // copy of subsetMap (what's this?)
	
    return (result);
}

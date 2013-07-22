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

#include "LArPhysWaveContainerCnv.h"
#include "LArCondTPCnv/LArPhysWaveSubsetCnv_p1.h"

static LArPhysWaveSubsetCnv_p1   TPconverter1;

LArPhysWavePersType*
LArPhysWaveContainerCnv::createPersistent (LArPhysWaveTransType* transObj)
{
    MsgStream log(messageService(), "LArPhysWaveContainerCnv" ); 
    log << MSG::DEBUG << "WRITING LArPhysWaveContainer" << endreq;
    LArPhysWavePersType* persObj = TPconverter1.createPersistent( transObj, log );
    log << MSG::DEBUG << "WRITING LArPhysWaveContainer Success !" << endreq;
    return persObj; 
}

LArConditionsSubset<LArPhysWave>*
LArPhysWaveContainerCnv::createTransient ()
{   
    MsgStream log(messageService(), "LArPhysWaveContainerCnv" ); 
	log<<MSG::DEBUG<<"READING PHYS WAVE"<<endreq;
    static pool::Guid   p1_guid("87E436E2-6FF4-42D3-BC70-6650C076E589");
	static pool::Guid   p0_guid("C1108D27-6D30-41E8-892D-2AB127B868C9");
	if( compareClassGuid(p1_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< LArPhysWaveSubset_p1 > col_vect( poolReadObject< LArPhysWaveSubset_p1 >() );
        log << MSG::DEBUG << "READING LArPhysWaveSubset_p1" << endreq; 
		LArPhysWaveTransType* transObj = TPconverter1.createTransient( col_vect.get(), log );
        log << MSG::DEBUG << "READING LArPhysWaveSubset_p1 Success !" << endreq;
        return transObj;
    }
    else if(compareClassGuid(p0_guid)){
        MsgStream log(messageService(), "LArPhysWaveContainerCnv" ); 
        log << MSG::DEBUG << " READING LArPhysWaveSubset (before TP split)" << endreq; 

        std::auto_ptr< LArConditionsSubset<LArPhysWave> > subset ( poolReadObject< LArConditionsSubset<LArPhysWave> >() );
        
        return (createTransient(subset.get()));
    } 
    throw std::runtime_error("Unsupported persistent version of LArPhysWaveCnv");
}




LArConditionsSubset<LArPhysWave>* 
LArPhysWaveContainerCnv::createTransient(LArConditionsSubset<LArPhysWave>* orig)
{

    MsgStream log(messageService(), "LArPedestalCompleteCnv" ); 
    log << MSG::DEBUG << "LArPhysWaveContainerCnv::createTransient orig " << endreq; 

    LArConditionsSubset<LArPhysWave>* result = new LArConditionsSubset<LArPhysWave>();
    
    // Copy from orig to result
	
    result->m_gain          = orig->m_gain; 
    result->m_channel       = orig->m_channel;
    result->m_groupingType  = orig->m_groupingType;
	
	
	for (unsigned int w=0;w<orig->m_subset.size();++w){ // copy of subset
		std::vector<LArPhysWave> ochvec;
		for (unsigned int q=0;q<orig->m_subset[w].second.size();++q){
			ochvec.push_back(orig->m_subset[w].second[q]);
			}
		unsigned int ofebid=orig->m_subset[w].first;
		std::pair<unsigned int, std::vector<LArPhysWave> > opair(ofebid,ochvec);
		result->m_subset.push_back(opair);
		}
		
	
	for(unsigned int w=0;w<orig->m_correctionVec.size();++w){ // copy of correctionVec
		LArPhysWave ovec=orig->m_correctionVec[w].second;
		unsigned int ofebid=orig->m_correctionVec[w].first;
		std::pair<unsigned int, LArPhysWave> opair(ofebid, ovec);
		result->m_correctionVec.push_back(opair);
		}		
	
	result->m_subsetMap.swap(orig->m_subsetMap); // copy of subsetMap (what's this?)
	
	log << MSG::DEBUG <<"No T/P split copy ok."<<endreq;

    return (result);
}

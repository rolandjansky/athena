/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArWaveCnv.cxx
 * @brief AthenaPool converter LArWave
 * @author Ilija Vukotic <ivukotic@cern.ch>
 */

#include "LArRawConditions/LArConditionsSubset.h"
#include "LArPhysWaveContainerCnv.h"
#include "LArCondTPCnv/LArPhysWaveSubsetCnv_p1.h"

static const LArPhysWaveSubsetCnv_p1   TPconverter1;

LArPhysWavePersType*
LArPhysWaveContainerCnv::createPersistent (LArPhysWaveTransType* transObj)
{
    MsgStream log(msgSvc(), "LArPhysWaveContainerCnv" ); 
    log << MSG::DEBUG << "WRITING LArPhysWaveContainer" << endmsg;
    LArPhysWavePersType* persObj = TPconverter1.createPersistentConst( transObj, log );
    log << MSG::DEBUG << "WRITING LArPhysWaveContainer Success !" << endmsg;
    return persObj; 
}

LArConditionsSubset<LArPhysWave>*
LArPhysWaveContainerCnv::createTransient ()
{   
    MsgStream log(msgSvc(), "LArPhysWaveContainerCnv" ); 
	log<<MSG::DEBUG<<"READING PHYS WAVE"<<endmsg;
    static const pool::Guid   p1_guid("87E436E2-6FF4-42D3-BC70-6650C076E589");
    static const pool::Guid   p0_guid("C1108D27-6D30-41E8-892D-2AB127B868C9");
	if( compareClassGuid(p1_guid) ) {
        // using unique_ptr ensures deletion of the persistent object
        std::unique_ptr< LArPhysWaveSubset_p1 > col_vect( poolReadObject< LArPhysWaveSubset_p1 >() );
        log << MSG::DEBUG << "READING LArPhysWaveSubset_p1" << endmsg; 
		LArPhysWaveTransType* transObj = TPconverter1.createTransientConst( col_vect.get(), log );
        log << MSG::DEBUG << "READING LArPhysWaveSubset_p1 Success !" << endmsg;
        return transObj;
    }
    else if(compareClassGuid(p0_guid)){
        MsgStream log(msgSvc(), "LArPhysWaveContainerCnv" ); 
        log << MSG::DEBUG << " READING LArPhysWaveSubset (before TP split)" << endmsg; 

        std::unique_ptr< LArConditionsSubset<LArPhysWave> > subset ( poolReadObject< LArConditionsSubset<LArPhysWave> >() );
        
        return (createTransient(subset.get()));
    } 
    throw std::runtime_error("Unsupported persistent version of LArPhysWaveCnv");
}




LArConditionsSubset<LArPhysWave>* 
LArPhysWaveContainerCnv::createTransient(LArConditionsSubset<LArPhysWave>* orig)
{

    MsgStream log(msgSvc(), "LArPedestalCompleteCnv" ); 
    log << MSG::DEBUG << "LArPhysWaveContainerCnv::createTransient orig " << endmsg; 

    LArConditionsSubset<LArPhysWave>* result = new LArConditionsSubset<LArPhysWave>();
    
    // Copy from orig to result

    result->assign (*orig,
                    [] (const LArPhysWave& from,
                        LArPhysWave& to)
                    { to = from; });
#if 0
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
#endif
	
	log << MSG::DEBUG <<"No T/P split copy ok."<<endmsg;

    return (result);
}

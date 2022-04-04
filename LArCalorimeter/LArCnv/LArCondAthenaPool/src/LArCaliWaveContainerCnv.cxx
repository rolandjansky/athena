/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArWaveCnv.cxx
 * @brief AthenaPool converter LArWave
 * @author Ilija Vukotic <ivukotic@cern.ch>
 */

#include "LArRawConditions/LArConditionsSubset.h"
#include "LArCaliWaveContainerCnv.h"
#include "LArCondTPCnv/LArCaliWaveSubsetCnv_p1.h"
#include "LArCondTPCnv/LArCaliWaveSubsetCnv_p2.h"

static const LArCaliWaveSubsetCnv_p1   TPconverter1;
static const LArCaliWaveSubsetCnv_p2   TPconverter2;

LArCaliWavePersType*
LArCaliWaveContainerCnv::createPersistent (LArCaliWaveTransType* transObj)
{
    MsgStream log(msgSvc(), "LArCaliWaveContainerCnv" ); 
    log << MSG::DEBUG << "WRITING LArCaliWaveContainer" << endmsg;
    LArCaliWavePersType* persObj = TPconverter2.createPersistentConst( transObj, log );
    log << MSG::DEBUG << "WRITING LArCaliWaveContainer Success !" << endmsg;
    return persObj; 
}

LArConditionsSubset<LArCaliWaveVec>*
LArCaliWaveContainerCnv::createTransient ()
{   
    MsgStream log(msgSvc(), "LArCaliWaveContainerCnv" ); 

    static const pool::Guid   p2_guid("6CF01BBF-85A9-45FA-B321-6A98B0D719FB");
    static const pool::Guid   p1_guid("9E61BE2D-3274-4459-A5C2-3BBFB7056EBA");
    static const pool::Guid   p0_guid("ECB4AD6C-FF3A-4255-A0E3-7BD566B96A77");
	
    if( compareClassGuid(p2_guid) ) {
        // using unique_ptr ensures deletion of the persistent object
        std::unique_ptr< LArCaliWaveSubset_p2 > col_vect( poolReadObject< LArCaliWaveSubset_p2 >() );
        log << MSG::DEBUG << "READING LArCaliWaveSubset_p2" << endmsg; 
		LArCaliWaveTransType* transObj = TPconverter2.createTransientConst( col_vect.get(), log );
        log << MSG::DEBUG << "READING LArCaliWaveSubset_p2 Success !" << endmsg;
        return transObj;
    }    
	else if( compareClassGuid(p1_guid) ) {
        // using unique_ptr ensures deletion of the persistent object
        std::unique_ptr< LArCaliWaveSubset_p1 > col_vect( poolReadObject< LArCaliWaveSubset_p1 >() );
        log << MSG::DEBUG << "READING LArCaliWaveSubset_p1" << endmsg; 
		LArCaliWaveTransType* transObj = TPconverter1.createTransientConst( col_vect.get(), log );
        log << MSG::DEBUG << "READING LArCaliWaveSubset_p1 Success !" << endmsg;
        return transObj;
    }
    else if(compareClassGuid(p0_guid)){
        MsgStream log(msgSvc(), "LArCaliWaveContainerCnv" ); 
        log << MSG::DEBUG << " READING LArCaliWaveSubset (before TP split)" << endmsg; 

        std::unique_ptr< LArConditionsSubset<LArCaliWaveVec> > subset ( poolReadObject< LArConditionsSubset<LArCaliWaveVec> >() );
        
        return (createTransient(subset.get()));
    } 
    throw std::runtime_error("Unsupported persistent version of LArCaliWaveCnv");
}




LArConditionsSubset<LArCaliWaveVec>* 
LArCaliWaveContainerCnv::createTransient(LArConditionsSubset<LArCaliWaveVec>* orig)
{

    MsgStream log(msgSvc(), "LArPedestalCompleteCnv" ); 
    log << MSG::DEBUG << "LArCaliWaveContainerCnv::createTransient orig " << orig << endmsg; 

    LArConditionsSubset<LArCaliWaveVec>* result = new LArConditionsSubset<LArCaliWaveVec>();
    result->assign (*orig,
                    [] (const LArCaliWaveVec& from,
                        LArCaliWaveVec& to)
                    { to = from; });
	
    return (result);
}

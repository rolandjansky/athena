/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT1_RawDataContainerCnv_p1.h"

#include "CreateTransientTemplate.h"
#include "MsgUtil.h"
#include "SCT1_RawDataCnv_p1.h"

#include "InDetEventAthenaPool/InDetRawData_p1.h"
#include "InDetEventAthenaPool/InDetRawDataCollection_p1.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetRawData/SCT1_RawData.h"
#include "InDetRawData/SCT_RDO_Collection.h"
#include "InDetRawData/SCT_RDO_Container.h"

void SCT1_RawDataContainerCnv_p1::transToPers(const SCT_RDO_Container* transCont, InDetRawDataContainer_p1* persCont, MsgStream& log)
{

    // The transient model has a container holding collections and the
    // collections hold channels.
    //
    // The persistent model flattens this so that the persistent
    // container has two vectors:
    //   1) all collections, and
    //   2) all RDO
    //
    // The persistent collections, then only maintain indexes into the
    // container's vector of all channels. 
    //
    // So here we loop over all collection and add their channels
    // to the container's vector, saving the indexes in the
    // collection. 

    typedef SCT_RDO_Container TRANS;

    SCT1_RawDataCnv_p1  chanCnv;
    TRANS::const_iterator it_Coll     = transCont->begin();
    TRANS::const_iterator it_CollEnd  = transCont->end();
    unsigned int collIndex;
    unsigned int chanBegin = 0;
    unsigned int chanEnd = 0;
    persCont->m_collections.resize(transCont->numberOfCollections());
    MSG_DEBUG(log," Preparing " << persCont->m_collections.size() << "Collections");
  
    for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, it_Coll++)  {
        // Add in new collection
        const SCT_RDO_Collection& collection = (**it_Coll);
        chanBegin  = chanEnd;
        chanEnd   += collection.size();
        InDetRawDataCollection_p1& pcollection = persCont->m_collections[collIndex];
        pcollection.m_id    = collection.identify().get_compact();
        pcollection.m_hashId = static_cast<unsigned int>(collection.identifyHash());
        pcollection.m_begin = chanBegin;
        pcollection.m_end   = chanEnd;
        // Add in channels
        persCont->m_rawdata.resize(chanEnd);
        for (unsigned int i = 0; i < collection.size(); ++i) {
            InDetRawData_p1* pchan = &(persCont->m_rawdata[i + chanBegin]);
            const SCT1_RawData* chan = dynamic_cast<const SCT1_RawData*>(collection[i]);
            if (chan) {
                chanCnv.transToPers(chan, pchan, log);
            }
        }
    }
    MSG_DEBUG(log," ***  Writing SCT_RDO_Container (SCT1_RawData concrete type)");
}

void  SCT1_RawDataContainerCnv_p1::persToTrans(const InDetRawDataContainer_p1* persCont, SCT_RDO_Container* transCont, MsgStream& log)
{

    // The transient model has a container holding collections and the
    // collections hold channels.
    //
    // The persistent model flattens this so that the persistent
    // container has two vectors:
    //   1) all collections, and
    //   2) all channels
    //
    // The persistent collections, then only maintain indexes into the
    // container's vector of all channels. 
    //
    // So here we loop over all collection and extract their channels
    // from the vector.


    SCT1_RawDataCnv_p1  chanCnv;
    MSG_DEBUG(log," Reading " << persCont->m_collections.size() << "Collections");
    for (unsigned int icoll = 0; icoll < persCont->m_collections.size(); ++icoll) {

        // Create trans collection - in NOT owner of SCT_RDO_RawData (SG::VIEW_ELEMENTS)
	// IDet collection don't have the Ownership policy c'tor
        const InDetRawDataCollection_p1& pcoll = persCont->m_collections[icoll];        
        Identifier collID(Identifier(pcoll.m_id));
        IdentifierHash collIDHash(IdentifierHash(pcoll.m_hashId));
        std::unique_ptr<SCT_RDO_Collection> coll = std::make_unique<SCT_RDO_Collection>(collIDHash);
        coll->setIdentifier(Identifier(collID));
        unsigned int nchans = pcoll.m_end - pcoll.m_begin;
        coll->resize(nchans);
        // Fill with channels
        for (unsigned int ichan = 0; ichan < nchans; ++ ichan) {
            const InDetRawData_p1* pchan = &(persCont->m_rawdata[ichan + pcoll.m_begin]);
            std::unique_ptr<SCT1_RawData> chan = std::make_unique<SCT1_RawData>();
            chanCnv.persToTrans(pchan, chan.get(), log);
            (*coll)[ichan] = chan.release();
        }
        
        // register the rdo collection in IDC with hash - faster addCollection
        StatusCode sc = transCont->addCollection(coll.release(), collIDHash);
        if (sc.isFailure()) {
            throw std::runtime_error("Failed to add collection to ID Container");
        }
	MSG_VERBOSE(log,"AthenaPoolTPCnvIDCont::persToTrans, collection, hash_id/coll id = " << collIDHash.value()
		    << " / " << collID.get_compact() << ", added to Identifiable container.");
    }

    MSG_DEBUG(log," ***  Reading SCT_RDO_Container (SCT1_RawData concrete type)");
}

//================================================================
SCT_RDO_Container* SCT1_RawDataContainerCnv_p1::createTransient(const InDetRawDataContainer_p1* persObj, MsgStream& log) {
    std::unique_ptr<SCT_RDO_Container> trans(std::make_unique<SCT_RDO_Container>(m_sctId->wafer_hash_max()));
    persToTrans(persObj, trans.get(), log);
    return trans.release();
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetRawData/Pixel1RawData.h"
#include "InDetEventAthenaPool/InDetRawData_p1.h"
#include "InDetEventAthenaPool/InDetRawDataCollection_p1.h"
#include "InDetRawData/PixelRDO_Container.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetRawData/PixelRDO_Collection.h"
#include "Pixel1RawDataCnv_p1.h"
#include "Pixel1RawDataContainerCnv_p1.h"
#include "MsgUtil.h"
#include "CreateTransientTemplate.h"

void Pixel1RawDataContainerCnv_p1::transToPers(const PixelRDO_Container* transCont, InDetRawDataContainer_p1* persCont, MsgStream &log) 
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

    typedef PixelRDO_Container TRANS;

    Pixel1RawDataCnv_p1  chanCnv;
    TRANS::const_iterator it_Coll     = transCont->begin();
    TRANS::const_iterator it_CollEnd  = transCont->end();
    unsigned int collIndex;
    unsigned int chanBegin = 0;
    unsigned int chanEnd = 0;
    int numColl = transCont->numberOfCollections();
    //if(numColl == transCont->fullSize() ) { // let's count how many collections we have:
    // numColl = 0;
    // for ( ; it_Coll != it_CollEnd; it_Coll++)
    //    numColl++;
    // it_Coll     = transCont->begin(); // reset the iterator, we used it!
    //}
    persCont->m_collections.resize(numColl);
    MSG_DEBUG(log," Preparing " << persCont->m_collections.size() << "Collections");
  
    for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, it_Coll++)  {
        // Add in new collection
        const PixelRDO_Collection& collection = (**it_Coll);
        chanBegin  = chanEnd;
        chanEnd   += collection.size();
        InDetRawDataCollection_p1& pcollection = persCont->m_collections[collIndex];
        pcollection.m_id    = collection.identify().get_identifier32().get_compact();
        pcollection.m_hashId = (unsigned int) collection.identifyHash();
        pcollection.m_begin = chanBegin;
        pcollection.m_end   = chanEnd;
        // Add in channels
        persCont->m_rawdata.resize(chanEnd);
        for (unsigned int i = 0; i < collection.size(); ++i) {
            InDetRawData_p1* pchan = &(persCont->m_rawdata[i + chanBegin]);
            const Pixel1RawData* chan = dynamic_cast<const Pixel1RawData*>(collection[i]);
            chanCnv.transToPers(chan, pchan, log);
        }
    }
    MSG_DEBUG(log," ***  Writing PixelRDO_Container (Pixel1RawData concrete type)");
}

void  Pixel1RawDataContainerCnv_p1::persToTrans(const InDetRawDataContainer_p1* persCont, PixelRDO_Container* transCont, MsgStream &log) 
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


    PixelRDO_Collection* coll = 0;

    Pixel1RawDataCnv_p1  chanCnv;
    MSG_DEBUG(log," Reading " << persCont->m_collections.size() << "Collections");
    for (unsigned int icoll = 0; icoll < persCont->m_collections.size(); ++icoll) {

        // Create trans collection - in NOT owner of PixelRDO_RawData (SG::VIEW_ELEMENTS)
	// IDet collection don't have the Ownership policy c'tor
        const InDetRawDataCollection_p1& pcoll = persCont->m_collections[icoll];        
        Identifier collID(pcoll.m_id);
        IdentifierHash collIDHash(IdentifierHash(pcoll.m_hashId));
        coll = new PixelRDO_Collection(collIDHash );
        coll->setIdentifier(collID);
        unsigned int nchans           = pcoll.m_end - pcoll.m_begin;
        coll->resize(nchans);
        // Fill with channels
        for (unsigned int ichan = 0; ichan < nchans; ++ ichan) {
            const InDetRawData_p1* pchan = &(persCont->m_rawdata[ichan + pcoll.m_begin]);
            Pixel1RawData* chan = new Pixel1RawData();
            chanCnv.persToTrans(pchan, chan, log);
            (*coll)[ichan] = chan;
        }
        
        // register the rdo collection in IDC with hash - faster addCollection
        StatusCode sc = transCont->addCollection(coll, collIDHash);
        if (sc.isFailure()) {
            throw std::runtime_error("Failed to add collection to ID Container");
        }
	MSG_VERBOSE(log,"AthenaPoolTPCnvIDCont::persToTrans, collection, hash_id/coll id = "
		    << (int) collIDHash << " / " << collID.get_compact() << ", added to Identifiable container.");
    }

    MSG_DEBUG(log," ***  Reading PixelRDO_Container (Pixel1RawData concrete type)");
}

//================================================================
PixelRDO_Container* Pixel1RawDataContainerCnv_p1::createTransient(const InDetRawDataContainer_p1* persObj, MsgStream& log) {
    std::unique_ptr<PixelRDO_Container> trans(new PixelRDO_Container(m_pixId->wafer_hash_max()));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}


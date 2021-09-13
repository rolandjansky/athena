/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArRawChannel.h"
#include "LArTPCnv/LArRawChannel_p1.h"
#include "LArRawEvent/LArRawChannelContainer.h"


// LArRawChannelContainerCnv_p2, used for T/P separation
// author R.Seuster <seuster@cern.ch>

#include "LArTPCnv/LArRawChannelCnv_p1.h"
#include "LArTPCnv/LArRawChannelContainerCnv_p2.h"
//#include "LArTPCnv/LArRawChannelCollection_p2.h"
//#include "DataModel/DataPool.h"

void LArRawChannelContainerCnv_p2::transToPers(const LArRawChannelContainer* /*transCont*/, LArRawChannelContainer_p2* /*persCont*/, MsgStream &log) const
{

  log << MSG::ERROR << "Writing LArRawChannelContainer_p2 not supported any more. Current version is LArRawChannelContainer_p3" << endmsg;
  return;
}

/*
    //LArRawChannelContainer_Cnvp1_base_t::transToPers(trans,pers,log);

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
    // So here we loop over all collection and add their channels
    // to the container's vector, saving the indexes in the
    // collection. 

    typedef LArRawChannelContainer TRANS;

    LArRawChannelCnv_p1  chanCnv;
    TRANS::const_iterator it_Coll     = transCont->begin();
    TRANS::const_iterator it_CollEnd  = transCont->end();
    unsigned int collIndex;
    unsigned int chanBegin = 0;
    unsigned int chanEnd = 0;
    persCont->m_collections.resize(transCont->numberOfCollections());
    for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, it_Coll++)  {
        // Add in new collection
        const LArRawChannelCollection& collection = (**it_Coll);
        chanBegin  = chanEnd;
        chanEnd   += collection.size();
        LArRawChannelCollection_p2& pcollection = persCont->m_collections[collIndex];
        pcollection.m_id    = collection.identify().id();
        pcollection.m_begin = chanBegin;
        pcollection.m_end   = chanEnd;
        // Add in channels
        persCont->m_channels.resize(chanEnd);
        for (unsigned int i = 0; i < collection.size(); ++i) {
            LArRawChannel_p1* pchan = &(persCont->m_channels[i + chanBegin]);
            const LArRawChannel* chan = collection[i];
            chanCnv.transToPers(chan, pchan, log);
        }
    }


  log << MSG::DEBUG  << " ***  Writing LArRawChannelContainer" << endmsg;
}

*/


void LArRawChannelContainerCnv_p2::persToTrans(const LArRawChannelContainer_p2* persCont, LArRawChannelContainer* transCont, MsgStream &log) const
{
    //LArRawChannelContainer_Cnvp1_base_t::persToTrans(pers,trans,log);

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


  
  std::size_t nChannels=persCont->m_channels.size();
  transCont->resize(nChannels);
  for (std::size_t i=0;i<nChannels;++i)
    m_larRawChannelCnv_p1.persToTrans(&(persCont->m_channels[i]),&(transCont->at(i)),log);

  log << MSG::DEBUG  << " ***  Reading LArRawChannelContainer" << endmsg;
}

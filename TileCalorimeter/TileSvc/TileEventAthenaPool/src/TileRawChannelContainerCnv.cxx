/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileTPCnv/TileRawChannelContainerCnv_p1.h"
#include "TileRawChannelContainerCnv.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"

TileRawChannelContainer_PERS* TileRawChannelContainerCnv::createPersistent(TileRawChannelContainer* transCont) {
    MsgStream mlog(msgSvc(), "TileRawChannelContainerConverter" );
    TileRawChannelContainerCnv_p1   converter;
    TileRawChannelContainer_PERS *persObj = converter.createPersistent( transCont, mlog );
    return persObj;
}

TileRawChannelContainer* TileRawChannelContainerCnv::createTransient() {
    MsgStream mlog(msgSvc(), "TileRawChannelContainerConverter" );
    TileRawChannelContainerCnv_p1   converter_p1;

    TileRawChannelContainer       *trans_cont(0);

    static const pool::Guid   p1_guid("BF727F06-9F94-4989-9C1F-9E59023988EA");
    static const pool::Guid   p0_guid("E18095F6-01D9-4E8B-AD51-A8628F92E7FF");

    if( this->compareClassGuid(p1_guid)) {
        std::auto_ptr< TileRawChannelContainer_p1 >   cont( this->poolReadObject< TileRawChannelContainer_p1 >() );
        trans_cont = converter_p1.createTransient( cont.get(), mlog );
    }
    else if( this->compareClassGuid(p0_guid)) {
        // old version from before TP separation
        TileRawChannelCollectionVec* rdoV = this->poolReadObject<TileRawChannelCollectionVec>();
        bool lDebug = (mlog.level()<=MSG::DEBUG);

        if (lDebug) 
          mlog << MSG::DEBUG << "Read IDC, size " << rdoV->size() << endmsg;

        // fixing bug with units in 13.0.20 - should be removed once all RDO files produced with 13.0.20 disappear
        if (rdoV->m_type == 104) {
          if (lDebug) 
            mlog << MSG::DEBUG << "fixing units bug in 13.0.20 RDO data: if (type=104) type=4;" << endmsg;
          rdoV->m_type = (TileFragHash::TYPE)4;
        }

        int hashType = rdoV->m_type    & 0xF;
        int type = (rdoV->m_type >> 4) & 0xF;
        int unit = (rdoV->m_type >> 8) & 0xF;
        uint32_t bsflags = rdoV->m_type & 0xFFFFF000;

        if (lDebug) 
          mlog << MSG::DEBUG << MSG::hex << "m_type=" << rdoV->m_type 
               << " " <<  bsflags << " " << unit << " " << type << " " << hashType << MSG::dec << endmsg;
    
        // create the Tile IdentifiableContainer to contain the rdo collections
        trans_cont = new TileRawChannelContainer(false,hashType,unit,SG::OWN_ELEMENTS);
        if (type != 0) trans_cont->set_type((TileFragHash::TYPE)type);
        trans_cont->set_bsflags(bsflags);
    
        TileRawChannelCollectionVec::iterator it   = rdoV->begin();
        TileRawChannelCollectionVec::iterator last = rdoV->end();

        for (; it != last; ++it) {

          TileRawChannelCollection* rdoColl = *it;
          rdoColl->setOwnership(SG::OWN_ELEMENTS);

          // register the rdo collection in StoreGate
          TileRawChannelCollection::ID id_coll = rdoColl->identify();
          StatusCode sc = trans_cont->addCollection(rdoColl,trans_cont->hashFunc()(id_coll));
          if(sc.isFailure()) {
            mlog << MSG::ERROR << "Can't add collection " << id_coll << " to container" << endmsg;
          }
        }
        delete rdoV;
    }  else {
        throw std::runtime_error("Unsupported persistent version of Data container");
    }
    return trans_cont;
}

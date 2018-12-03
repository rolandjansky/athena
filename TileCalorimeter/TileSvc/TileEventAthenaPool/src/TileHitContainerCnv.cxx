/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileTPCnv/TileHitContainerCnv_p1.h"
#include "TileHitContainerCnv.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"

TileHitContainer_PERS* TileHitContainerCnv::createPersistent(TileHitContainer* transCont) {
    MsgStream mlog(msgSvc(), "TileHitContainerConverter" );
    TileHitContainerCnv_p1   converter;
    TileHitContainer_PERS *persObj = converter.createPersistent( transCont, mlog );
    return persObj;
}

TileHitContainer* TileHitContainerCnv::createTransient() {
    MsgStream mlog(msgSvc(), "TileHitContainerConverter" );
    TileHitContainerCnv_p1   converter_p1;

    TileHitContainer       *trans_cont(0);

    static const pool::Guid   p1_guid("E347580F-BBF0-441E-A799-9AC0256F69DF");
    static const pool::Guid   p0_guid("704A373C-EA65-4721-A9B8-F577B683699E");

    if( this->compareClassGuid(p1_guid)) {
        std::auto_ptr< TileHitContainer_p1 >   cont( this->poolReadObject< TileHitContainer_p1 >() );
        trans_cont = converter_p1.createTransient( cont.get(), mlog );
    }
    else if( this->compareClassGuid(p0_guid)) {
        // old version from before TP separation
        TileHitCollectionVec* rdoV = this->poolReadObject<TileHitCollectionVec>();

        if (mlog.level()<=MSG::DEBUG)
          mlog << MSG::DEBUG << "Read IDC, size " << rdoV->size() << endmsg;

        // TileFragHash::TYPE type = rdoV->m_type; 
        
        // create the Tile IdentifiableContainer to contain the rdo collections
        trans_cont = new TileHitContainer(false,SG::OWN_ELEMENTS);

        TileHitCollectionVec::iterator it   = rdoV->begin();
        TileHitCollectionVec::iterator last = rdoV->end();

        for (; it != last; ++it) {

          TileHitCollection* rdoColl = *it;
          rdoColl->setOwnership(SG::OWN_ELEMENTS);

          // register the rdo collection in StoreGate
          TileHitCollection::ID id_coll = rdoColl->identify();
          StatusCode sc = trans_cont->addCollection(rdoColl,trans_cont->hashFunc()(id_coll));
          if(sc.isFailure()) {
            mlog << MSG::ERROR << "Can't add collection " << id_coll << " to container " << endmsg;
          }
        }
        delete rdoV;
    }  else {
        throw std::runtime_error("Unsupported persistent version of Data container");
    }
    return trans_cont;
}

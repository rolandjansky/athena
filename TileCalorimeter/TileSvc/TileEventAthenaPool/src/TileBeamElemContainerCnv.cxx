/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileTPCnv/TileBeamElemContainerCnv_p1.h"
#include "TileBeamElemContainerCnv.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"

TileBeamElemContainer_PERS* TileBeamElemContainerCnv::createPersistent(TileBeamElemContainer* transCont) {
    MsgStream mlog(msgSvc(), "TileBeamElemContainerConverter" );
    TileBeamElemContainerCnv_p1   converter;
    TileBeamElemContainer_PERS *persObj = converter.createPersistent( transCont, mlog );
    return persObj;
}

TileBeamElemContainer* TileBeamElemContainerCnv::createTransient() {
    MsgStream mlog(msgSvc(), "TileBeamElemContainerConverter" );
    TileBeamElemContainerCnv_p1   converter_p1;

    TileBeamElemContainer       *trans_cont(0);

    static const pool::Guid   p1_guid("CF8DE6AB-8E15-4B5F-881B-39B736EAB4E0");
    static const pool::Guid   p0_guid("7FCE8F30-B59E-41E6-9A66-0DCD6134552E");

    if( this->compareClassGuid(p1_guid)) {
        std::auto_ptr< TileBeamElemContainer_p1 >   cont( this->poolReadObject< TileBeamElemContainer_p1 >() );
        trans_cont = converter_p1.createTransient( cont.get(), mlog );
    }
    else if( this->compareClassGuid(p0_guid)) {
        // old version from before TP separation
        TileBeamElemCollectionVec* rdoV = this->poolReadObject<TileBeamElemCollectionVec>();

        if (mlog.level()<=MSG::DEBUG)
          mlog << MSG::DEBUG << "Read IDC, size " << rdoV->size() << endmsg;

        // TileFragHash::TYPE type = rdoV->m_type; 
        
        // create the Tile IdentifiableContainer to contain the rdo collections
        trans_cont = new TileBeamElemContainer(false,SG::OWN_ELEMENTS);

        TileBeamElemCollectionVec::iterator it   = rdoV->begin();
        TileBeamElemCollectionVec::iterator last = rdoV->end();

        for (; it != last; ++it) {

          TileBeamElemCollection* rdoColl = *it;
          rdoColl->setOwnership(SG::OWN_ELEMENTS);

          // register the rdo collection in StoreGate
          TileBeamElemCollection::ID id_coll = rdoColl->identify();
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


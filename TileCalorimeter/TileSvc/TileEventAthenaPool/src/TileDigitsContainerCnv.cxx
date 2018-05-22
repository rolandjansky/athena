/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileTPCnv/TileDigitsContainerCnv_p1.h"
#include "TileTPCnv/TileDigitsContainerCnv_p2.h"
#include "TileTPCnv/TileDigitsContainerCnv_p3.h"
#include "TileDigitsContainerCnv.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"

TileDigitsContainer_PERS* TileDigitsContainerCnv::createPersistent(TileDigitsContainer* transCont) {
    MsgStream mlog(msgSvc(), "TileDigitsContainerConverter" );
    TileDigitsContainerCnv_p3   converter;
    TileDigitsContainer_PERS *persObj = converter.createPersistent( transCont, mlog );
    return persObj;
}

TileDigitsContainer* TileDigitsContainerCnv::createTransient() {
    MsgStream mlog(msgSvc(), "TileDigitsContainerConverter" );
    TileDigitsContainerCnv_p1   converter_p1;
    TileDigitsContainerCnv_p2   converter_p2;
    TileDigitsContainerCnv_p3   converter_p3;

    TileDigitsContainer       *trans_cont(0);

    static const pool::Guid   p3_guid("8CC01C7A-B330-413E-BEF9-DFA18F15B1FA");
    static const pool::Guid   p2_guid("2F82DE4C-65FA-4F01-BCB5-15330224E357");
    static const pool::Guid   p1_guid("F9386A42-43A7-4482-8A5B-F33EDE586FB2");
    static const pool::Guid   p0_guid("EB51ED70-5EC0-4F31-97D3-14453F77C88B");

    if( this->compareClassGuid(p3_guid)) {
        std::auto_ptr< TileDigitsContainer_p3 >   cont( this->poolReadObject< TileDigitsContainer_p3 >() );
        trans_cont = converter_p3.createTransient( cont.get(), mlog );
    }
    else if( this->compareClassGuid(p2_guid)) {
        std::auto_ptr< TileDigitsContainer_p2 >   cont( this->poolReadObject< TileDigitsContainer_p2 >() );
        trans_cont = converter_p2.createTransient( cont.get(), mlog );
    }
    else if( this->compareClassGuid(p1_guid)) {
        std::auto_ptr< TileDigitsContainer_p1 >   cont( this->poolReadObject< TileDigitsContainer_p1 >() );
        trans_cont = converter_p1.createTransient( cont.get(), mlog );
    }
    else if( this->compareClassGuid(p0_guid)) {
        // old version from before TP separation
        TileDigitsCollectionVec* rdoV = this->poolReadObject<TileDigitsCollectionVec>();

        if (mlog.level()<=MSG::DEBUG)
          mlog << MSG::DEBUG << "Read IDC, size " << rdoV->size() << endmsg;

        // TileFragHash::TYPE type = rdoV->m_type; 
        
        // create the Tile IdentifiableContainer to contain the rdo collections
        trans_cont = new TileDigitsContainer(false,SG::OWN_ELEMENTS);

        TileDigitsCollectionVec::iterator it   = rdoV->begin();
        TileDigitsCollectionVec::iterator last = rdoV->end();

        for (; it != last; ++it) {

          TileDigitsCollection* rdoColl = *it;
          rdoColl->setOwnership(SG::OWN_ELEMENTS);

          // register the rdo collection in StoreGate
          TileDigitsCollection::ID id_coll = rdoColl->identify();
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

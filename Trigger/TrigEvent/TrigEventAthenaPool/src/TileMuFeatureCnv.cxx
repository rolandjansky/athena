/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TileMuFeatureCnv.cxx
 * @brief Generated implementation file which includes header files needed by TileMuFeatureCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *         Heuijin Lim <hjlim@hep.anl.gov>
 */

#include "TileMuFeatureCnv.h"
#include "TrigMuonEventTPCnv/TileMuFeatureCnv_p1.h"

TileMuFeatureCnv::TileMuFeatureCnv(ISvcLocator *svcloc): TileMuFeatureCnvBase(svcloc) {}
TileMuFeatureCnv::~TileMuFeatureCnv(){}

//createPersistent
TileMuFeature_PERS* TileMuFeatureCnv::createPersistent(TileMuFeature* transObj) {
    MsgStream mlog(messageService(), "TileMuFeatureConverter" );

    mlog << MSG::DEBUG << "TileMuFeatureCnv::createPersistent" << endreq;

    TileMuFeatureCnv_p1   converter;
    TileMuFeature_PERS *persObj = converter.createPersistent( transObj, mlog );
    return persObj;
}

//createTransient
TileMuFeature* TileMuFeatureCnv::createTransient() {
    MsgStream mlog(messageService(), "TileMuFeatureConverter" );

    mlog << MSG::DEBUG << "TileMuFeatureCnv::createTransient " << endreq;
  
    static pool::Guid p1_guid("F853E605-AFD5-44F9-98BC-4D777CBCBEE4");
        // from "TrigEvent/TrigMuonEventTPCnv/TrigMuonEventTPCnv/selection.xml"

    static pool::Guid p0_guid("526B0709-442D-4D2C-8C1F-8C3922149656");
	// from "TrigEvent/TrigMuonEvent/TrigMuonEvent/selection.xml"

    if( compareClassGuid(p1_guid) ) {
      std::auto_ptr< TileMuFeature_p1 > col_vect( poolReadObject< TileMuFeature_p1 >() );
      TileMuFeatureCnv_p1 converter;
      return converter.createTransient( col_vect.get(), mlog );

    }
    else if( compareClassGuid(p0_guid) ) {
      // old version from before TP separation, just return it
      return this->poolReadObject<TileMuFeature>();
    }
    else {
      throw std::runtime_error("Unsupported persistent version");
    }

}


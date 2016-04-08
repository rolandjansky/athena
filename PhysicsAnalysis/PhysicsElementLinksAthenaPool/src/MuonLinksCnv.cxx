/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLinksCnv.h"

MuonLinksCnv::MuonLinksCnv(ISvcLocator* svcloc):ParticleLinksCnv<Analysis::MuonContainer>(svcloc){
    MsgStream msg( this->msgSvc(), typeid(this).name() );
  msg << MSG::DEBUG << " constructor" << endreq;
}


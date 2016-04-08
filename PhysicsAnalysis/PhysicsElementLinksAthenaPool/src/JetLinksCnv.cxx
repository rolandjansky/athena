/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetLinksCnv.h"

JetLinksCnv::JetLinksCnv(ISvcLocator* svcloc):ParticleLinksCnv<JetCollection>(svcloc){
    MsgStream msg( this->msgSvc(), typeid(this).name() );
  msg << MSG::DEBUG << " constructor" << endreq;
}


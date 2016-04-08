/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronLinksCnv.h"

ElectronLinksCnv::ElectronLinksCnv(ISvcLocator* svcloc):ParticleLinksCnv<ElectronContainer>(svcloc){
    MsgStream msg( this->msgSvc(), typeid(this).name() );
  msg << MSG::DEBUG << " constructor" << endreq;

}


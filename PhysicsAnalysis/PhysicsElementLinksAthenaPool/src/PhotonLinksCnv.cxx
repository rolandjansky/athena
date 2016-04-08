/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PhotonLinksCnv.h"

PhotonLinksCnv::PhotonLinksCnv(ISvcLocator* svcloc):ParticleLinksCnv<PhotonContainer>(svcloc){
    MsgStream msg( this->msgSvc(), typeid(this).name() );
  msg << MSG::DEBUG << " constructor" << endreq;

}


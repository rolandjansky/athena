/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackLinksCnv.h"

TrackLinksCnv::TrackLinksCnv(ISvcLocator* svcloc):ParticleLinksCnv<Rec::TrackParticleContainer>(svcloc){
    MsgStream msg( this->msgSvc(), typeid(this).name() );
  msg << MSG::DEBUG << " constructor" << endreq;
}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthParticleLinksCnv.h"

TruthParticleLinksCnv::TruthParticleLinksCnv(ISvcLocator* svcloc):ParticleLinksCnv<TruthParticleContainer>(svcloc){
    MsgStream msg( this->msgSvc(), typeid(this).name() );
  msg << MSG::DEBUG << " constructor" << endreq;

}


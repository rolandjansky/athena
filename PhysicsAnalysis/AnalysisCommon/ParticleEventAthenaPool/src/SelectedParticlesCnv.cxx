/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SelectedParticlesCnv.h"
#include "GaudiKernel/MsgStream.h"
 
SelectedParticlesCnv::SelectedParticlesCnv(ISvcLocator* svcLocator) :
  T_AthenaPoolCustomCnv<SelectedParticles, 
                        SelectedParticles_PERS>(svcLocator)
{
  if ( 0 == svcLocator ) {
    throw std::runtime_error("NULL pointer to ISvcLocator !!");
  }

}

SelectedParticles_PERS* 
SelectedParticlesCnv::createPersistent(SelectedParticles* transCont)
{
    MsgStream msg( this->msgSvc(), typeid(this).name() );
  msg << MSG::DEBUG << "::createPersistent called" << endreq;
  SelectedParticlesCnv_p1 cnv;
  SelectedParticles_PERS *persObj = cnv.createPersistent(transCont, msg);

  msg << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}

SelectedParticles* SelectedParticlesCnv::createTransient() 
{
   MsgStream msg( this->msgSvc(), "SelectedParticlesConverter" );
  msg << MSG::DEBUG << "::createTansient called" << endreq;

   SelectedParticles *transObj = 0;

   static pool::Guid p1_guid("DEC1562B-16E6-4ACE-9E89-9B1E733696F5");

   if ( this->compareClassGuid(p1_guid) ) {

       // using auto_ptr ensures deletion of the persistent object
       // note hairy syntax

       std::auto_ptr<SelectedParticles_PERS> persObj(poolReadObject<SelectedParticles_PERS>());
       SelectedParticlesCnv_p1 cnv;
       transObj = cnv.createTransient( persObj.get(), msg );
       msg << MSG::DEBUG << "::createTransient [Success]" << endreq;
   } else {
       throw std::runtime_error("Unsupported persistent version of SelectedParticlesy");
   }

   return transObj;
}

 


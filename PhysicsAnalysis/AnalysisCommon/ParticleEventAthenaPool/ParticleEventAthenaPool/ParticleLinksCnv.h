///////// -*- c++ -*- /////////////////////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleLinksCnv.h 
// Header file for class ParticlesLinksCnv
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTATHENAPOOL_PARTICLELINKSCNV_H 
#define PARTICLEEVENTATHENAPOOL_PARTICLELINKSCNV_H 1

// STL includes
 
 
// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

// ParticleLinks includes
#include "ParticleEvent/ParticleLinks.h"
#include "ParticleEventTPCnv/ParticleLinks_p1.h"
#include "ParticleEventTPCnv/ParticleLinksCnv_p1.h"
 
// Forward declaration
 
// the latest persistent representation type of ParticleLinks
typedef ParticleLinks_p1  ParticleLinks_PERS;
 
template<class Container>
class ParticleLinksCnv: public T_AthenaPoolCustomCnv<
                                           ParticleLinks<Container>, 
                                           ParticleLinks_PERS 
                                           > 
 
{
   // make the factory for this converter our friend
  friend class CnvFactory<ParticleLinksCnv<Container> >;
  typedef T_AthenaPoolCustomCnv<ParticleLinks<Container>, ParticleLinks_PERS> Base_t;
  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
  protected:
 
   /** Create the converter from the service locator
   */
  ParticleLinksCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual ParticleLinks_PERS*
  createPersistent( ParticleLinks<Container>* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual ParticleLinks<Container>* createTransient();
 
};
 
/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
///////////////////////////////////////////////////////////////////
template<class Container> 
ParticleLinksCnv<Container>::ParticleLinksCnv(ISvcLocator* svcLocator) :
  T_AthenaPoolCustomCnv<ParticleLinks<Container>, 
                        ParticleLinks_PERS>(svcLocator)
{
  if ( 0 == svcLocator ) {
    throw std::runtime_error("NULL pointer to ISvcLocator !!");
  }

}

template<class Container>
ParticleLinks_PERS* 
ParticleLinksCnv<Container>::createPersistent(ParticleLinks<Container>* transCont)
{
    MsgStream msg( this->msgSvc(), typeid(this).name() );
  msg << MSG::DEBUG << "::createPersistent called" << endreq;
  ParticleLinksCnv_p1<Container> cnv;
  ParticleLinks_PERS *persObj = cnv.createPersistent(transCont, msg);

  msg << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}

template<class Container>
ParticleLinks<Container>* ParticleLinksCnv<Container>::createTransient() 
{
   MsgStream msg( this->msgSvc(), "ParticleLinksConverter" );
  msg << MSG::DEBUG << "::createTansient called" << endreq;

   ParticleLinks<Container> *transObj = 0;

   static pool::Guid p1_guid("1368A875-F458-45C6-9A87-09E0C24A9C67");

   if ( this->compareClassGuid(p1_guid) ) {

       // using auto_ptr ensures deletion of the persistent object
       // note hairy syntax

       std::auto_ptr<ParticleLinks_PERS> persObj (Base_t::template poolReadObject<ParticleLinks_PERS>());
       ParticleLinksCnv_p1<Container> cnv;
       transObj = cnv.createTransient( persObj.get(), msg );
       msg << MSG::DEBUG << "::createTransient [Success]" << endreq;
   } else {
       throw std::runtime_error("Unsupported persistent version of ParticleLinks");
   }

   return transObj;
}



#endif //> !PARTICLEEVENTATHENAPOOL_PARTICLELINKSCNV_H 
 


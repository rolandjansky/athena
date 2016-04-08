///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleContainerCnv.cxx 
// Implementation file for class TruthParticleContainerCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

// McParticleKernel includes
#include "McParticleKernel/ITruthParticleCnvTool.h"

// McParticleEventTPCnv includes
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p1.h"
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p2.h"
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p3.h"
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p4.h"
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p5.h"
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p6.h"

// McParticleEventAthenaPool includes
#include "TruthParticleContainerCnv.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

TruthParticleContainerCnv::TruthParticleContainerCnv(ISvcLocator* svcLocator) :
  T_AthenaPoolCustomCnv<TruthParticleContainer, 
                        TruthParticleContainer_PERS>(svcLocator),
  m_storeGate( 0 ),
  m_cnvTool  ( 0 )
{
  if ( 0 == svcLocator ) {
    throw std::runtime_error("NULL pointer to ISvcLocator !!");
  }

  IToolSvc* toolSvc = 0; 
  if ( svcLocator->service("ToolSvc",toolSvc).isFailure() || 0 == toolSvc ) {
    throw std::runtime_error("Could not fetch ToolSvc !!");
  } 

  IAlgTool * algTool = 0;
  if ( toolSvc->retrieveTool( "TruthParticleCnvTool", algTool ).isFailure() ) {
    throw std::runtime_error("Could not fetch TruthParticleCnvTool !!");
  } else {
    m_cnvTool = dynamic_cast<ITruthParticleCnvTool*>(algTool);
    if ( 0 == m_cnvTool ) {
      throw std::runtime_error("Could not dyn-cast to ITruthParticleCnvTool !!");
    }
  }

  if ( svcLocator->service("StoreGateSvc",m_storeGate).isFailure() || 
       0 == m_storeGate ) {
    throw std::runtime_error("Could not fetch StoreGateSvc !!");
  }
}


TruthParticleContainer_PERS* 
TruthParticleContainerCnv::createPersistent(TruthParticleContainer* trans)
{
  MsgStream log( msgSvc(), "TruthParticleContainerCnv" );

  TruthParticleContainerCnv_p6 cnv( m_cnvTool );
  TruthParticleContainer_PERS *pers = cnv.createPersistent(trans, log);

  log << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return pers; 
}

TruthParticleContainer* TruthParticleContainerCnv::createTransient() 
{
   MsgStream msg( msgSvc(), "TruthParticleContainerCnv" );

   TruthParticleContainer *trans = 0;

   static pool::Guid tr_guid("20FF7533-4D02-4DAE-AABB-8C62260D4E75");

   static pool::Guid p1_guid("6B56623E-05D2-4BEA-9EBA-86F5E9BD8047");
   static pool::Guid p2_guid("D835E375-E3D3-41DB-A781-8C55768553E1");
   static pool::Guid p3_guid("E52F9F90-71CD-46C0-8572-587D9F768E1A");
   static pool::Guid p4_guid("545F1E63-AA6C-4490-B0E4-1BF71F1A170D");
   static pool::Guid p5_guid("2D25E3D9-950B-49E0-A51F-2B6EC93D1A23");
   static pool::Guid p6_guid("97AC2CEE-7E8A-4E2E-B6B5-FD8545D77FC4");

   if ( compareClassGuid(p6_guid) ) {
     
     // using auto_ptr ensures deletion of the persistent object
     std::auto_ptr<TruthParticleContainer_p6> pers( poolReadObject<TruthParticleContainer_p6>() );
     TruthParticleContainerCnv_p6 cnv( m_cnvTool );
     trans = cnv.createTransient( pers.get(), msg );

   } else if ( compareClassGuid(p5_guid) ) {
     
     // using auto_ptr ensures deletion of the persistent object
     std::auto_ptr<TruthParticleContainer_p5> pers( poolReadObject<TruthParticleContainer_p5>() );
     TruthParticleContainerCnv_p5 cnv( m_cnvTool );
     trans = cnv.createTransient( pers.get(), msg );

   } else if ( compareClassGuid(tr_guid) ) {
     // a word of explanation: the TruthParticleContainer was kind of T/P
     // separated before the official formalization of the T/P separation
     // process (for releases before 11.0.5).
     // Indeed, the POOL files containing the GUID of the transient class
     // are just containing an empty DataVector<TruthParticle> which
     // was eventually filled/re-created on the fly from a
     // @c McEventCollection.
     // So here we load the TruthParticleContainer to fetch the name of this
     // McEventCollection and setup the TruthParticleContainer_p1 with it.

     std::auto_ptr<TruthParticleContainer> tmpObj( poolReadObject<TruthParticleContainer>() );
     std::auto_ptr<TruthParticleContainer_p1> pers( new TruthParticleContainer_p1( tmpObj.get()->m_genEvent.dataID() ) );

     TruthParticleContainerCnv_p1 cnv( m_storeGate, m_cnvTool );
     trans = cnv.createTransient( pers.get(), msg );
     
   } else if ( compareClassGuid(p4_guid) ) {
     
     // using auto_ptr ensures deletion of the persistent object
     std::auto_ptr<TruthParticleContainer_p4> pers( poolReadObject<TruthParticleContainer_p4>() );
     TruthParticleContainerCnv_p4 cnv( m_storeGate, m_cnvTool );
     trans = cnv.createTransient( pers.get(), msg );

   } else if ( compareClassGuid(p3_guid) ) {
     
     // using auto_ptr ensures deletion of the persistent object
     std::auto_ptr<TruthParticleContainer_p3> pers( poolReadObject<TruthParticleContainer_p3>() );
     TruthParticleContainerCnv_p3 cnv( m_storeGate, m_cnvTool );
     trans = cnv.createTransient( pers.get(), msg );

   } else if ( compareClassGuid(p2_guid) ) {
     
     // using auto_ptr ensures deletion of the persistent object
     std::auto_ptr<TruthParticleContainer_p2> pers( poolReadObject<TruthParticleContainer_p2>() );
     TruthParticleContainerCnv_p2 cnv( m_storeGate, m_cnvTool );
     trans = cnv.createTransient( pers.get(), msg );

   } else if ( compareClassGuid(p1_guid) ) {

     // using auto_ptr ensures deletion of the persistent object
     std::auto_ptr<TruthParticleContainer_p1> pers( poolReadObject<TruthParticleContainer_p1>() );
     TruthParticleContainerCnv_p1 cnv( m_storeGate, m_cnvTool );
     trans = cnv.createTransient( pers.get(), msg );
     
   } else {
     throw std::runtime_error("Unsupported persistent version of TruthParticleContainer");
   }

   return trans;
}

///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p5.h"
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p6.h"

// McParticleEventAthenaPool includes
#include "TruthParticleContainerCnv.h"

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

  log << MSG::DEBUG << "::createPersistent [Success]" << endmsg;
  return pers; 
}

TruthParticleContainer* TruthParticleContainerCnv::createTransient() 
{
   MsgStream msg( msgSvc(), "TruthParticleContainerCnv" );

   TruthParticleContainer *trans = 0;

   static const pool::Guid p5_guid("2D25E3D9-950B-49E0-A51F-2B6EC93D1A23");
   static const pool::Guid p6_guid("97AC2CEE-7E8A-4E2E-B6B5-FD8545D77FC4");

   if ( compareClassGuid(p6_guid) ) {
     
     std::unique_ptr<TruthParticleContainer_p6> pers( poolReadObject<TruthParticleContainer_p6>() );
     TruthParticleContainerCnv_p6 cnv( m_cnvTool );
     trans = cnv.createTransient( pers.get(), msg );

   } else if ( compareClassGuid(p5_guid) ) {
     
     std::unique_ptr<TruthParticleContainer_p5> pers( poolReadObject<TruthParticleContainer_p5>() );
     TruthParticleContainerCnv_p5 cnv( m_cnvTool );
     trans = cnv.createTransient( pers.get(), msg );
     
   } else {
     throw std::runtime_error("Unsupported persistent version of TruthParticleContainer");
   }

   return trans;
}

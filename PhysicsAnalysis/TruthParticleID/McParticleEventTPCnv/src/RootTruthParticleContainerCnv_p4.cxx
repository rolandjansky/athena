///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootTruthParticleContainerCnv_p4.cxx 
// Implementation file for root converters.
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

// HepMC includes
#include "HepMC/GenParticle.h"
#include "GeneratorObjects/McEventCollection.h"

// McParticleKernel includes
#include "McParticleKernel/ITruthParticleCnvTool.h"

// McParticleEvent includes
#define private public
#define protected public
#include "McParticleEvent/TruthParticle.h"
#undef private
#undef protected

// McParticleEventTPCnv includes
#include "McEtIsolVisitor.h"
#include "McParticleEventTPCnv/RootTruthParticleContainerCnv_p4.h"
#include "RootTruthParticleCnvTool.h"
#include "TError.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

T_TPCnv<TruthParticleContainer, TruthParticleContainer_p4>::T_TPCnv()
{
  m_cnvTool = new RootTruthParticleCnvTool;
}

// Destructor
///////////////

T_TPCnv<TruthParticleContainer, TruthParticleContainer_p4>::~T_TPCnv()
{
  delete m_cnvTool;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

void 
T_TPCnv<TruthParticleContainer, TruthParticleContainer_p4>
  ::persToTrans( const TruthParticleContainer_p4* pers, 
                 TruthParticleContainer* trans, 
                 MsgStream& msg ) 
{
  // convert the ElementLink<McEventCollection>
  m_genEvtCnv.persToTrans( &pers->m_genEvent, 
			   &trans->m_genEvent, 
			   msg );

  const McEventCollection* evt = trans->m_genEvent.getStorableObjectPointer();
  if ( 0 == evt ) {
    ::Error ("RootTruthParticleContainerCnv_p4",
             "NULL pointer to McEventCollection !!");
    throw std::runtime_error("conversion error");
  }

#if 0
  // now extract the etIsolations from the persistent representation of
  // the TruthParticleContainer and put them into a TruthEtIsolations container
  TruthEtIsolationsContainer * etIsols = new TruthEtIsolationsContainer;
  const std::string etIsolName = "TruthEtIsol_" + trans->genEventName();
  if ( !m_storeGate->record( etIsols, etIsolName ).isSuccess() ) {
    msg << MSG::ERROR
	<< "Could not record a TruthEtIsolationsContainer at ["
	<< etIsolName << "] !!"
	<< endreq;
    delete etIsols; etIsols = 0;
    throw std::runtime_error("No TruthEtIsolationsContainer could be stored !!");
  }
  if ( !m_storeGate->setConst( etIsols ).isSuccess() ) {
    msg << MSG::WARNING
	<< "Could not setConst the TruthEtIsolationsContainer at ["
	<< etIsolName << "] !!"
	<< endreq;
  }
#endif

  const unsigned int genEventIndex = pers->m_genEvent.m_elementIndex;

#if 0
  TruthEtIsolations * etIsol = new TruthEtIsolations( trans->genEventName(),
						      genEventIndex );
  etIsols->push_back( etIsol );

  // create a visitor which takes the persistent representation of the 
  // TruthParticleContainer as a parameter.
  // This visitor will setup each TruthParticle with its EtIsol cone vector.
  const McEtIsolVisitor<TruthParticleContainer_p4> visitor( *pers,
							    etIsol,
							    msg );
#endif

  if ( !m_cnvTool->convert( evt, 
			    genEventIndex,
			    trans,
			    0/*&visitor*/ ).isSuccess() ) {
    ::Error ("RootTruthParticleContainerCnv_p4",
             "Problem converting HepMC::GenEvent [%s][%d] "
             "to TruthParticleContainer !",
             pers->m_genEvent.m_contName.c_str(),
             pers->m_genEvent.m_elementIndex);
    throw std::runtime_error("conversion error");
  }

  if ( !trans->m_genEvent.isValid() ) {
    ::Error ("RootTruthParticleContainerCnv_p4",
             "ElementLink to McEventCollection is not valid !");
    throw std::runtime_error("conversion error");
  }

#if 0
  // connect the TruthParticleContainer to the container of TruthEtIsolations
  trans->setEtIsolations( etIsols, genEventIndex );
#endif

  return;
}

void 
T_TPCnv<TruthParticleContainer, TruthParticleContainer_p4>
  ::transToPers( const TruthParticleContainer*, 
                 TruthParticleContainer_p4*, 
                 MsgStream& /*msg*/ ) 
{
  abort();
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 


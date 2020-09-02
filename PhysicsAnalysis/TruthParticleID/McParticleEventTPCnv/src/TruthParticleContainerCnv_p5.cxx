///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleContainerCnv_p5.cxx 
// Implementation file for class TruthParticleContainerCnv_p5
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// HepMC includes
#include "AtlasHepMC/GenParticle.h"
#include "GeneratorObjects/McEventCollection.h"

// McParticleKernel includes
#include "McParticleKernel/ITruthParticleCnvTool.h"

// McParticleEvent includes
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "McParticleEvent/TruthEtIsolationsContainer.h"

// McParticleEventTPCnv includes
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p5.h"


void 
TruthParticleContainerCnv_p5::persToTrans( const TruthParticleContainer_p5* pers, 
					   TruthParticleContainer* trans, 
					   MsgStream& msg ) const
{
  msg << MSG::DEBUG 
      << "Loading TruthParticleContainer from persistent state..."
      << endmsg;

  if ( 0 == m_cnvTool ) {
    msg << MSG::ERROR
	<< "NULL pointer to ITruthParticleCnvTool !!" 
	<< endmsg
	<< "Can't create *ANY* TruthParticleContainer !!" 
	<< endmsg;
    throw std::runtime_error("NULL pointer to ITruthParticleCnvTool !!");
  }

  // convert the ElementLink<McEventCollection>
  ElementLink<McEventCollection> genEventEL;
  m_genEvtCnv.persToTrans( &pers->m_genEvent, 
			   &genEventEL,
			   msg );
  trans->setGenEvent (genEventEL);

  const McEventCollection* evt = trans->genEventLink().getStorableObjectPointer();
  if ( 0 == evt ) {
    const std::string error("NULL pointer to McEventCollection !!");
    msg << MSG::ERROR << error
	<< endmsg;
    throw std::runtime_error(error);
  }

  if ( !m_cnvTool->convert( evt, 
			    pers->m_genEvent.m_elementIndex,
			    trans ).isSuccess() ) {
    msg << MSG::ERROR
	<< "Problem converting HepMC::GenEvent ["
	<< pers->m_genEvent.m_contName     << "][" 
	<< pers->m_genEvent.m_elementIndex << "]"
	<< " to TruthParticleContainer !"
	<< endmsg;
    throw std::runtime_error("No TruthParticleContainer created !!");
  }

  if ( !trans->genEventLink().isValid() ) {
    const std::string err = "ElementLink to McEventCollection is not valid !";
    msg << MSG::ERROR << err << endmsg;
    throw std::runtime_error(err);
  }

  // convert the ElementLink<TruthEtIsolationsContainer>
  // this needs to be done *AFTER* the GenEvent->TPContainer conversion !!
  ElementLink<TruthEtIsolationsContainer> etIsolEL;
  m_etIsolCnv.persToTrans( &pers->m_etIsolations, 
			   &etIsolEL,
			   msg );
  trans->setEtIsolations (etIsolEL);

  msg << MSG::DEBUG 
      << "Loaded TruthParticleContainer from persistent state [OK]"
      << endmsg;
  return;
}

void 
TruthParticleContainerCnv_p5::transToPers( const TruthParticleContainer*,
					   TruthParticleContainer_p5*, 
					   MsgStream& msg ) const
{
  msg << MSG::DEBUG 
      << "Creating persistent state of TruthParticleContainer..."
      << endmsg;
  
  msg << MSG::ERROR
      << "This transient-to-persistent converter method has been RETIRED !!"
      << endmsg
      << "You are not supposed to end-up here ! Go away !"
      << endmsg;

  throw std::runtime_error( "Retired TruthParticleContainerCnv_p5::transToPers() !!" );

  return;
}




#include "RootTruthParticleCnvTool.h"
T_TPCnv<TruthParticleContainer, TruthParticleContainer_p5>::T_TPCnv()
{
  m_cnvTool = new RootTruthParticleCnvTool;
}


T_TPCnv<TruthParticleContainer, TruthParticleContainer_p5>::~T_TPCnv()
{
  delete m_cnvTool;
}

///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleContainerCnv_p6.cxx 
// Implementation file for class TruthParticleContainerCnv_p6
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
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p6.h"

#include "McParticleEvent/PileUpClassification.h"


void 
TruthParticleContainerCnv_p6::persToTrans( const TruthParticleContainer_p6* pers, 
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

  // retrieve the pileup type of this container : it is the same as the GenEvent it is linked to.
  PileUpClassification::PileuType_t putype = PileUpClassification::pileUpType(evt,pers->m_genEvent.m_elementIndex);
  // retrieve all GenEvent corresponding to this pile-up type :
  size_t firstEvt, lastEvt;
  PileUpClassification::findEventIterators(putype, evt, firstEvt, lastEvt);
  msg << MSG::DEBUG << " Converting pile-up type = "<< putype << " from McEventCollection index "<< firstEvt << " to "<< lastEvt << endmsg;
  // loop on the GenEvent to fill the container.
  for( ; firstEvt != lastEvt; firstEvt++){

    if ( !m_cnvTool->convert( evt, 
                              firstEvt,
                              trans ).isSuccess() ) {
      msg << MSG::ERROR
          << "Problem converting HepMC::GenEvent ["
          << trans->genEventLink().dataID()      << "][" 
          << firstEvt << "]"
          << " to TruthParticleContainer !"
          << endmsg;
      throw std::runtime_error("No TruthParticleContainer created !!");
    }
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
TruthParticleContainerCnv_p6::transToPers( const TruthParticleContainer* trans,
					   TruthParticleContainer_p6* pers, 
					   MsgStream& msg ) const
{
  msg << MSG::DEBUG 
      << "Creating persistent state of TruthParticleContainer..."
      << endmsg;

  // convert the ElementLink<McEventCollection>
  m_genEvtCnv.transToPers( &trans->genEventLink(), 
                           &pers->m_genEvent, 
                           msg );

  if ( !trans->genEventLink().isValid() ) {
    msg << MSG::WARNING
        << "Transient ElementLink is NOT valid !!" << endmsg;
  }

  // convert the ElementLink<TruthEtIsolationsContainer>
  m_etIsolCnv.transToPers( &trans->etIsolationsLink(), 
                           &pers->m_etIsolations, 
                           msg );

  return;
}




#include "RootTruthParticleCnvTool.h"
T_TPCnv<TruthParticleContainer, TruthParticleContainer_p6>::T_TPCnv()
{
  m_cnvTool = new RootTruthParticleCnvTool;
}


T_TPCnv<TruthParticleContainer, TruthParticleContainer_p6>::~T_TPCnv()
{
  delete m_cnvTool;
}

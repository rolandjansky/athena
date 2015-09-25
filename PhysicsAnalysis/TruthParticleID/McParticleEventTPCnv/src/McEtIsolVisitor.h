///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McEtIsolVisitor.h 
// Header file for class McEtIsolVisitor
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENTTPCNV_MCETISOLVISITOR_H 
#define MCPARTICLEEVENTTPCNV_MCETISOLVISITOR_H 

// STL includes
#include <stdexcept>

// HepMC / CLHEP includes

// Gaudi includes
#include "GaudiKernel/MsgStream.h"

// McParticleKernel includes
#include "McParticleKernel/ITruthParticleVisitor.h"

// McParticleEvent includes
#include "McParticleEvent/TruthEtIsolations.h"

// Forward declaration

template <class TruthParticleContainer_pX>
class McEtIsolVisitor : public ITruthParticleVisitor
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Copy constructor: 
   */
  McEtIsolVisitor( const McEtIsolVisitor& rhs ) :
    ITruthParticleVisitor(rhs),
    m_msg    ( rhs.m_msg     ),
    m_persObj( rhs.m_persObj ),
    m_etIsols( rhs.m_etIsols )
  {}

  /** Constructor with parameters: 
   */
  McEtIsolVisitor( const TruthParticleContainer_pX& persObj,
		   TruthEtIsolations* etIsols,
		   MsgStream& msg ) :
    ITruthParticleVisitor(),
    m_msg    ( &msg    ),
    m_persObj( persObj ),
    m_etIsols( etIsols )
  {}

  /** Destructor: 
   */
  virtual ~McEtIsolVisitor(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** The method to visit a @c TruthParticle to apply further modifications
   *  to the instance at hand.
   */
  virtual void visit( TruthParticle* truthParticle ) const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Default constructor: 
   */
  McEtIsolVisitor();

  /** Assignment operator: 
   */
  McEtIsolVisitor& operator=( const McEtIsolVisitor& rhs ); 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  mutable MsgStream* m_msg;
  const TruthParticleContainer_pX& m_persObj;

  mutable TruthEtIsolations* m_etIsols;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

template<class TruthParticleContainer_pX>
McEtIsolVisitor<TruthParticleContainer_pX>::~McEtIsolVisitor()
{}

template<class TruthParticleContainer_pX>
void 
McEtIsolVisitor<TruthParticleContainer_pX>::visit( TruthParticle* mc ) const
{
  if ( 0 == mc ) {
    return;
  }

  const int barcode = mc->barcode();
  try {
    const std::vector<float>& etIsolations = m_persObj.etIsol(barcode);
    if ( etIsolations.empty() ) {
      // do nothing: no particle will be registered in the TruthEtIsolations.
      } else {
	const HepMC::GenParticle* hepMcPart = mc->genParticle();
	for ( std::size_t j = 0;
	      j != static_cast<std::size_t>(TruthParticleParameters::NbrOfCones);
	      ++j ) {
	  m_etIsols->setEtIsol( hepMcPart, 
				static_cast<TruthParticleParameters::ConeSize>(j),
				etIsolations[j] );

	}
      }
  } catch (std::out_of_range& e) {
    *m_msg << MSG::WARNING 
           << "Caught an out of range exception for this barcode: " << barcode
           << endreq
           << e.what() << endreq
           << "This particle won't have any et isolation informations !!"
           << endreq;
    // we don't want to have a buggy McTruth, do we ?
    throw std::runtime_error("Reqested an UNKNOWN TruthParticle/barcode");
  }
    
  return;
}

#endif //> MCPARTICLEEVENTTPCNV_MCETISOLVISITOR_H

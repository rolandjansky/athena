///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthEtIsolationsCnv_p1.cxx 
// Implementation file for class TruthEtIsolationsCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// HepMc includes
#include "GeneratorObjects/McEventCollection.h"

// McParticleEvent includes
#define private public
#define protected public
#include "McParticleEvent/TruthEtIsolations.h"
#undef private
#undef protected

// McParticleEventTPCnv includes
#include "McParticleEventTPCnv/TruthEtIsolationsCnv_p1.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

typedef TruthEtIsolations_p1::EtIsolMap_t    EtIsolMap_p;
typedef TruthEtIsolations_p1::EtIsolBc_t     EtIsolBc_p;
typedef TruthEtIsolations_p1::EtIsolations_t EtIsolations_p;
typedef TruthEtIsolations::EtIsolMap_t       EtIsolMap_t;

void 
TruthEtIsolationsCnv_p1::persToTrans( const TruthEtIsolations_p1* pers, 
                                      TruthEtIsolations* trans,
                                      MsgStream& msg ) 
{
  msg << MSG::DEBUG 
      << "Loading TruthEtIsolations from persistent state..."
      << endreq;

  if (0==pers || 0==trans) {
    msg << MSG::WARNING 
        << "null pointer(s) received ! trans=[" << trans << "] pers=["
        << pers << "]"
        << endmsg;
    return;
  }

  // convert the ElementLink<McEventCollection>
  m_genEvtCnv.persToTrans( &pers->m_genEvent, 
			   &trans->m_genEvtLink, 
			   msg );

  const McEventCollection* evt= trans->m_genEvtLink.getStorableObjectPointer();
  if ( 0 == evt ) {
    const std::string error("NULL pointer to McEventCollection !!");
    msg << MSG::ERROR << error
	<< endreq;
    throw std::runtime_error(error);
  }

  for ( EtIsolMap_p::const_iterator 
	  i    = pers->m_etIsolations.begin(),
	  iEnd = pers->m_etIsolations.end();
	i != iEnd;
	++i ) {
    McAod::EtIsolations& etIsols = trans->m_etIsolations[i->first];
    std::copy( i->second.begin(), i->second.end(),
	       etIsols.begin() );
  }
  
  msg << MSG::DEBUG 
      << "Loaded TruthEtIsolations from persistent state [OK]"
      << endreq;
  return;
}

void 
TruthEtIsolationsCnv_p1::transToPers( const TruthEtIsolations* trans,
				      TruthEtIsolations_p1* pers, 
				      MsgStream& msg ) 
{
  msg << MSG::DEBUG 
      << "Creating persistent state of TruthEtIsolations..."
      << endreq;
  
  // convert the ElementLink<McEventCollection>
  m_genEvtCnv.transToPers( &trans->m_genEvtLink, 
                           &pers->m_genEvent, 
                           msg );

  if ( !trans->m_genEvtLink.isValid() ) {
    msg << MSG::WARNING
        << "Transient ElementLink to McEventCollection is NOT valid !!" 
        << endreq;
  }

  pers->m_etIsolations.reserve( trans->size() );
  for ( EtIsolMap_t::const_iterator 
	  i    = trans->m_etIsolations.begin(),
	  iEnd = trans->m_etIsolations.end();
	i != iEnd;
	++i ) {
    pers->m_etIsolations.push_back( EtIsolBc_p( i->first, EtIsolations_p() ) );
    EtIsolations_p& etIsols = pers->m_etIsolations.back().second;
    std::copy( i->second.begin(), 
	       i->second.end(),
	       etIsols.begin() );
  }
  return;
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


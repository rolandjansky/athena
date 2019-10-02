///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// HepLorentzVectorCnv_p1.cxx 
// Implementation file for class HepLorentzVectorCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/HepLorentzVectorCnv_p1.h"


void 
HepLorentzVectorCnv_p1::persToTrans( const HepLorentzVector_p1* pers, 
				     CLHEP::HepLorentzVector* trans, 
				     MsgStream &/*log*/ ) const
{
  trans->set( pers->m_px, pers->m_py, pers->m_pz, pers->m_ene );
  return;
}

void 
HepLorentzVectorCnv_p1::transToPers( const CLHEP::HepLorentzVector* trans, 
				     HepLorentzVector_p1* pers, 
				     MsgStream &/*log*/ ) const
{
  pers->m_px  = trans->px();
  pers->m_py  = trans->py();
  pers->m_pz  = trans->pz();
  pers->m_ene = trans->e() ;
  return;
}

///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// P4PtEtaPhiMCnv_p2.cxx 
// Implementation file for class P4PtEtaPhiMCnv_p2
// Author: I. Vukotic <ivukotic@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// NavFourMom includes
#include "FourMom/P4PtEtaPhiM.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p2.h"


void 
P4PtEtaPhiMCnv_p2::persToTrans( const P4PtEtaPhiMFloat_p2* persObj, 
				P4PtEtaPhiM* transObj, 
				MsgStream &/*log*/ ) const
{
  transObj->setPt  ((double) persObj->m_pt);
  transObj->setEta ((double) persObj->m_eta);
  transObj->setPhi ((double) persObj->m_phi);
  transObj->setM   ((double) persObj->m_mass);
  return;
}

void 
P4PtEtaPhiMCnv_p2::transToPers( const P4PtEtaPhiM* transObj, 
				P4PtEtaPhiMFloat_p2* persObj, 
				MsgStream &/*log*/ ) const
{
  persObj->m_pt   = (float) transObj->pt();
  persObj->m_eta  = (float) transObj->eta();
  persObj->m_phi  = (float) transObj->phi();
  persObj->m_mass = (float) transObj->m();
  return;
}

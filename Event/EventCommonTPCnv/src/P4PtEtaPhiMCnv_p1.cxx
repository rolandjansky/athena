///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// P4PtEtaPhiMCnv_p1.cxx 
// Implementation file for class P4PtEtaPhiMCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// NavFourMom includes
#include "FourMom/P4PtEtaPhiM.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4PtEtaPhiMCnv_p1.h"


void 
P4PtEtaPhiMCnv_p1::persToTrans( const P4PtEtaPhiM_p1* persObj, 
				P4PtEtaPhiM* transObj, 
				MsgStream &/*log*/ ) const
{
  transObj->setPt  (persObj->m_pt);
  transObj->setEta (persObj->m_eta);
  transObj->setPhi (persObj->m_phi);
  transObj->setM   (persObj->m_mass);
  return;
}

void 
P4PtEtaPhiMCnv_p1::transToPers( const P4PtEtaPhiM* transObj, 
				P4PtEtaPhiM_p1* persObj, 
				MsgStream &/*log*/ ) const
{
  persObj->m_pt   = transObj->pt();
  persObj->m_eta  = transObj->eta();
  persObj->m_phi  = transObj->phi();
  persObj->m_mass = transObj->m();
  return;
}

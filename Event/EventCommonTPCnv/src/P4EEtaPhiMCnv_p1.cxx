///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// P4EEtaPhiMCnv_p1.cxx 
// Implementation file for class P4EEtaPhiMCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// NavFourMom includes
#include "FourMom/P4EEtaPhiM.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4EEtaPhiMCnv_p1.h"


void 
P4EEtaPhiMCnv_p1::persToTrans( const P4EEtaPhiM_p1* persObj, 
                               P4EEtaPhiM* transObj, 
                               MsgStream &/*log*/ ) const
{
  transObj->setE   ((double) persObj->m_e);
  transObj->setEta ((double) persObj->m_eta);
  transObj->setPhi ((double) persObj->m_phi);
  transObj->setM   ((double) persObj->m_m);
  return;
}

void 
P4EEtaPhiMCnv_p1::transToPers( const P4EEtaPhiM* transObj, 
                               P4EEtaPhiM_p1* persObj, 
                               MsgStream &/*log*/ ) const
{
  persObj->m_e   = (float) transObj->e();
  persObj->m_eta = (float) transObj->eta();
  persObj->m_phi = (float) transObj->phi();
  persObj->m_m   = (float) transObj->m();
  return;
}


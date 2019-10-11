///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// P4IPtCotThPhiMCnv_p1.cxx 
// Implementation file for class P4IPtCotThPhiMCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// NavFourMom includes
#include "FourMom/P4IPtCotThPhiM.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4IPtCotThPhiMCnv_p1.h"


void 
P4IPtCotThPhiMCnv_p1::persToTrans( const P4IPtCotThPhiM_p1* persObj, 
				   P4IPtCotThPhiM* transObj, 
				   MsgStream &/*log*/ ) const
{
  transObj->setIPt   ((double)persObj->m_iPt);
  transObj->setCotTh ((double)persObj->m_cotTh);
  transObj->setPhi   ((double)persObj->m_phi);
  transObj->setM     ((double)persObj->m_mass);
  return;
}

void 
P4IPtCotThPhiMCnv_p1::transToPers( const P4IPtCotThPhiM* transObj, 
				   P4IPtCotThPhiM_p1* persObj, 
				   MsgStream &/*log*/ ) const
{
  persObj->m_iPt   = (float)transObj->iPt();
  persObj->m_cotTh = (float)transObj->cotTh();
  persObj->m_phi   = (float)transObj->phi();
  persObj->m_mass  = (float)transObj->m();
  return;
}

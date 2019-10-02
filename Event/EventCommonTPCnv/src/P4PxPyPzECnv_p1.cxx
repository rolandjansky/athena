///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// P4PxPyPzECnv_p1.cxx 
// Implementation file for class P4PxPyPzECnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// NavFourMom includes
#include "FourMom/P4PxPyPzE.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4PxPyPzECnv_p1.h"


void 
P4PxPyPzECnv_p1::persToTrans( const P4PxPyPzE_p1* persObj, 
			      P4PxPyPzE* transObj, 
			      MsgStream &/*log*/ ) const
{
  transObj->setPx ((double)persObj->m_px);
  transObj->setPy ((double)persObj->m_py);
  transObj->setPz ((double)persObj->m_pz);
  transObj->setE  ((double)persObj->m_ene);
  return;
}

void 
P4PxPyPzECnv_p1::transToPers( const P4PxPyPzE* transObj, 
			      P4PxPyPzE_p1* persObj, 
			      MsgStream &/*log*/ ) const
{
  persObj->m_px  = (float)transObj->px();
  persObj->m_py  = (float)transObj->py();
  persObj->m_pz  = (float)transObj->pz();
  persObj->m_ene = (float)transObj->e();
  return;
}

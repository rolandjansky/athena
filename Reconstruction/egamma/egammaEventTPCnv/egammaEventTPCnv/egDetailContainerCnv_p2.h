///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egDetailContainerCnv_p2.h 
// Header file for class egDetailContainerCnv_p2
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_EGDETAILCONTAINERCNV_P2_H 
#define EGAMMAEVENTTPCNV_EGDETAILCONTAINERCNV_P2_H 

// STL includes

// Gaudi includes

// egammaEvent includes
#include "egammaEvent/egDetailContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egDetailContainer_p2.h"

class MsgStream;

class egDetailContainerCnv_p2 : public T_AthenaPoolTPCnvBase<egDetailContainer, egDetailContainer_p2>
{
public:
  egDetailContainerCnv_p2() {};
  virtual void persToTrans( const egDetailContainer_p2     *persObj,
			    egDetailContainer  *transObj,
			    MsgStream                    &msg );

  virtual void transToPers( const egDetailContainer *transObj,
			    egDetailContainer_p2                *persObj,
			    MsgStream                         &msg );
};

// Template for access to T/P converter from dictionary
template<>
class T_TPCnv<egDetailContainer, egDetailContainer_p2>
  : public egDetailContainerCnv_p2
{
public:
};

#endif //> EGAMMAEVENTTPCNV_EGDETAILCONTAINERCNV_P2_H

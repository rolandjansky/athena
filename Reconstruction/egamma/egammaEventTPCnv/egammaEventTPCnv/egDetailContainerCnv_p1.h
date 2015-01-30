///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egDetailContainerCnv_p1.h 
// Header file for class egDetailContainerCnv_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_EGDETAILCONTAINERCNV_P1_H 
#define EGAMMAEVENTTPCNV_EGDETAILCONTAINERCNV_P1_H 

// STL includes

// Gaudi includes

// egammaEvent includes
#include "egammaEvent/egDetailContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egDetailContainer_p1.h"

class MsgStream;

class egDetailContainerCnv_p1 : public T_AthenaPoolTPCnvBase<egDetailContainer, egDetailContainer_p1>
{
public:
  egDetailContainerCnv_p1() {};
  virtual void persToTrans( const egDetailContainer_p1     *persObj,
			    egDetailContainer  *transObj,
			    MsgStream                    &msg );

  virtual void transToPers( const egDetailContainer *transObj,
			    egDetailContainer_p1                *persObj,
			    MsgStream                         &msg );
};

#endif //> EGAMMAEVENTTPCNV_EGDETAILCONTAINERCNV_P1_H

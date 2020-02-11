/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARDIGITCONTAINERCNV_P2_H
#define LARTPCNV_LARDIGITCONTAINERCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArTPCnv/LArDigitContainer_p2.h"

class MsgStream;
class LArDigitContainer;
class LArOnlineID_Base;

class LArDigitContainerCnv_p2  : public T_AthenaPoolTPCnvBase<LArDigitContainer, LArDigitContainer_p2>
{
public:

  LArDigitContainerCnv_p2(const LArOnlineID_Base* idHelper);

  virtual void          persToTrans(const LArDigitContainer_p2* pers, LArDigitContainer* trans, 
				    MsgStream &log);
  virtual void          transToPers(const LArDigitContainer* trans, LArDigitContainer_p2* pers, 
				    MsgStream &log);

 private:
  const LArOnlineID_Base* m_idHelper;
  

};

#endif

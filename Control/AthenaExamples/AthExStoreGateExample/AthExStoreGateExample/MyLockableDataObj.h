// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ATHEXSTOREGATEEXAMPLE_MYLOCKABLEDATAOBJ
#define ATHEXSTOREGATEEXAMPLE_MYLOCKABLEDATAOBJ


#include "AthenaKernel/ILockable.h"


/**
 * @brief To test SG lock functionality.
 */
class MyLockableDataObj : public ILockable
{
public: 
  MyLockableDataObj() : m_locked (false) {}
  virtual void lock() { m_locked = true; }

  bool m_locked;
};

#include "SGTools/CLASS_DEF.h"
CLASS_DEF(MyLockableDataObj, 8009, 1)


#endif // not ATHEXSTOREGATEEXAMPLE_MYLOCKABLEDATAOBJ



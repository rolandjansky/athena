/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENTATHENAPOOL_JETMOMENTMAPCNV_H
#define JETEVENTATHENAPOOL_JETMOMENTMAPCNV_H


#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "JetEventTPCnv/JetMomentMap_p6.h"

#include "JetEvent/JetMomentMap.h"

#include <string>
#include <vector>

// the latest persistent representation type of JetMomentMap_p1
typedef  JetMomentMap_p6  JetMomentMap_PERS;
typedef  T_AthenaPoolCustomCnv<JetMomentMap, JetMomentMap_PERS>  JetMomentMapCnvBase;

class JetMomentMapCnv : public JetMomentMapCnvBase
{
  friend class CnvFactory<JetMomentMapCnv>;
  
 protected:

  JetMomentMapCnv (ISvcLocator* svcloc) : JetMomentMapCnvBase(svcloc) {};
  virtual JetMomentMap_PERS*   createPersistent (JetMomentMap* transObj);
  virtual JetMomentMap*        createTransient ();
  
};

#endif

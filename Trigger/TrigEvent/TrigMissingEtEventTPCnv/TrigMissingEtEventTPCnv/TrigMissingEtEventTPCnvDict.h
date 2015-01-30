/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMISSINGETEVENTTPCNV_TRIGMISSINGETEVENTTPCNVDICT_H
#define TRIGMISSINGETEVENTTPCNV_TRIGMISSINGETEVENTTPCNVDICT_H

#include "TrigMissingEtEventTPCnv/TrigMissingET_p1.h"
#include "TrigMissingEtEventTPCnv/TrigMissingET_p2.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainer_tlp1.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainer_p1.h"

#include "TrigMissingEtEventTPCnv/TrigMissingET_p3.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainer_p3.h"

//dummy instances of all objects using std::vector<TPObjRef> for the dictionary
struct dummy_TrigMissingEtEventTPCnvDict
{

  std::vector<TrigMissingET_p1>	              m_dummyMissingET_p1;
  std::vector<TrigMissingET_p2>	              m_dummyMissingET_p2;
  std::vector<TrigMissingET_p3>	              m_dummyMissingET_p3;

};

#endif

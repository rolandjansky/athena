/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRDOCONTAINERCNV_P1_H
#define MUON_TGCRDOCONTAINERCNV_P1_H

/*

Transient/Persistent converter for a container of collections of TGC,
Author: Marcin Nowak
        CERN, February, 2006

*/

#include "MuonRdoContainerTPCnv.h"


#include "MuonEventAthenaPool/TgcRdoContainer_p1.h"
#include "TgcRdoCnv_p1.h"
#include "MuonRDO/TgcRdoContainer.h"

typedef T_AthenaPoolTPCnvIDCont< TgcRdoContainer, TgcRdoContainer_p1, TgcRdoCnv_p1>  TgcRdoContainerCnv_p1;



#endif





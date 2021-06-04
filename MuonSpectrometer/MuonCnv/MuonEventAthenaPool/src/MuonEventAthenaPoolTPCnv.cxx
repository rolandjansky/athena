/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "CscRawDataCnv_p1.h"
#include "CscRawDataCnv_p2.h"
#include "CscRawDataCnv_p3.h"
#include "CscRawDataCnv_p4.h"

#include "CscRawDataCollectionCnv_p1.h"
#include "CscRawDataCollectionCnv_p2.h"
#include "CscRawDataCollectionCnv_p3.h"
#include "CscRawDataCollectionCnv_p4.h"

#include "CscRawDataContainerCnv_p1.h"
#include "CscRawDataContainerCnv_p2.h"
#include "CscRawDataContainerCnv_p3.h"
#include "CscRawDataContainerCnv_p4.h"

#include "MdtAmtHitCnv_p1.h"
#include "MdtCsmCnv_p1.h"
#include "MdtCsmContainerCnv_p1.h"
#include "RpcCoinMatrixCnv_p1.h"
#include "RpcFiredChannelCnv_p1.h"
#include "RpcPadCnv_p1.h"
#include "RpcPadContainerCnv_p1.h"
#include "RpcPadContainerCnv_p2.h"
#include "RpcSectorLogicContainerCnv_p1.h"

#include "TgcRawDataCnv_p1.h"
#include "TgcRawDataCnv_p2.h"
#include "TgcRawDataCnv_p3.h"

#include "TgcRdoCnv_p1.h"
#include "TgcRdoCnv_p2.h"
#include "TgcRdoCnv_p3.h"

#include "TgcRdoContainerCnv_p1.h"
#include "TgcRdoContainerCnv_p2.h"
#include "TgcRdoContainerCnv_p3.h"

DECLARE_TPCNV_FACTORY(CscRawDataCnv_p1,
                      CscRawData,
                      CscRawData_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(CscRawDataCnv_p2,
                      CscRawData,
                      CscRawData_p2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(CscRawDataCnv_p3,
                      CscRawData,
                      CscRawData_p3,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(CscRawDataCnv_p4,
                      CscRawData,
                      CscRawData_p4,
                      Athena::TPCnvVers::Current)


DECLARE_TPCNV_FACTORY(CscRawDataCollectionCnv_p1,
                      CscRawDataCollection,
                      CscRawDataCollection_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(CscRawDataCollectionCnv_p2,
                      CscRawDataCollection,
                      CscRawDataCollection_p2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(CscRawDataCollectionCnv_p3,
                      CscRawDataCollection,
                      CscRawDataCollection_p3,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(CscRawDataCollectionCnv_p4,
                      CscRawDataCollection,
                      CscRawDataCollection_p4,
                      Athena::TPCnvVers::Current)


DECLARE_TPCNV_FACTORY(CscRawDataContainerCnv_p1,
                      CscRawDataContainer,
                      CscRawDataContainer_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(CscRawDataContainerCnv_p2,
                      CscRawDataContainer,
                      CscRawDataContainer_p2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(CscRawDataContainerCnv_p3,
                      CscRawDataContainer,
                      CscRawDataContainer_p3,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(CscRawDataContainerCnv_p4,
                      CscRawDataContainer,
                      CscRawDataContainer_p4,
                      Athena::TPCnvVers::Current)


DECLARE_TPCNV_FACTORY(MdtAmtHitCnv_p1,
                      MdtAmtHit,
                      MdtAmtHit_p1,
                      Athena::TPCnvVers::Current)


DECLARE_TPCNV_FACTORY(MdtCsmCnv_p1,
                      MdtCsm,
                      MdtCsm_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(MdtCsmContainerCnv_p1,
                      MdtCsmContainer,
                      MdtCsmContainer_p1,
                      Athena::TPCnvVers::Current)


DECLARE_TPCNV_FACTORY(RpcCoinMatrixCnv_p1,
                      RpcCoinMatrix,
                      RpcCoinMatrix_p1,
                      Athena::TPCnvVers::Current)


DECLARE_TPCNV_FACTORY(RpcFiredChannelCnv_p1,
                      RpcFiredChannel,
                      RpcFiredChannel_p1,
                      Athena::TPCnvVers::Current)


DECLARE_TPCNV_FACTORY(RpcPadCnv_p1,
                      RpcPad,
                      RpcPad_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(RpcPadContainerCnv_p1,
                      RpcPadContainer,
                      RpcPadContainer_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(RpcPadContainerCnv_p2,
                      RpcPadContainer,
                      RpcPadContainer_p2,
                      Athena::TPCnvVers::Current)


DECLARE_TPCNV_FACTORY(RpcSectorLogicContainerCnv_p1,
                      RpcSectorLogicContainer,
                      RpcSectorLogicContainer_p1,
                      Athena::TPCnvVers::Current)


DECLARE_TPCNV_FACTORY(TgcRawDataCnv_p1,
                      TgcRawData,
                      TgcRawData_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TgcRawDataCnv_p2,
                      TgcRawData,
                      TgcRawData_p2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TgcRawDataCnv_p3,
                      TgcRawData,
                      TgcRawData_p3,
                      Athena::TPCnvVers::Current)


DECLARE_TPCNV_FACTORY(TgcRdoCnv_p1,
                      TgcRdo,
                      TgcRdo_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TgcRdoCnv_p2,
                      TgcRdo,
                      TgcRdo_p2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TgcRdoCnv_p3,
                      TgcRdo,
                      TgcRdo_p3,
                      Athena::TPCnvVers::Current)


DECLARE_TPCNV_FACTORY(TgcRdoContainerCnv_p1,
                      TgcRdoContainer,
                      TgcRdoContainer_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TgcRdoContainerCnv_p2,
                      TgcRdoContainer,
                      TgcRdoContainer_p2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TgcRdoContainerCnv_p3,
                      TgcRdoContainer,
                      TgcRdoContainer_p3,
                      Athena::TPCnvVers::Current)

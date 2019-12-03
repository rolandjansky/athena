# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# File: xAODEventInfoCnv/python/xAODEventInfoCnvAlgDefault.py
# Created: Jun 2019, sss
# Purpose: Configure xAODEventInfoCnvAlg
#

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


def xAODEventInfoCnvAlgDefault (sequence = topSequence):
    from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvTool, xAODMaker__EventInfoCnvAlg

    lumiDataKey = ''
    from IOVDbSvc.CondDB import conddb
    if not conddb.isMC:
        from LumiBlockComps.LuminosityCondAlgDefault import LuminosityCondAlgDefault
        condalg = LuminosityCondAlgDefault()
        lumiDataKey = condalg.LuminosityOutputKey

    tool = xAODMaker__EventInfoCnvTool (LumiDataKey = lumiDataKey)
    alg = xAODMaker__EventInfoCnvAlg (CnvTool = tool)
    sequence += alg
    return alg

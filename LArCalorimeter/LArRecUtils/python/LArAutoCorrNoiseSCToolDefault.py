# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging

def LArAutoCorrNoiseSCToolDefault (name="LArAutoCorrNoiseSCToolDefault", **kw): 
    # call base class constructor
    from AthenaCommon import CfgMgr
    kw['name'] = name
    tool = CfgMgr.LArAutoCorrNoiseTool (**kw)

    mlog = logging.getLogger( 'LArAutoCorrNoiseSCToolDefault::__init__ ' )
    mlog.info("entering")

    # do the configuration
    tool.MCSym = False
    tool.SuperCell=True
    tool.keyAutoCorr = "LArAutoCorrSC"

    from LArROD.LArRODFlags import larRODFlags
    tool.NSamples = larRODFlags.nSamples()
    from IOVDbSvc.CondDB import conddb
    if ( conddb.isMC and not conddb.folderRequested('/LAR/IdentifierOfl/OnOffIdMap_SC') ) :
        conddb.addFolder("LAR_OFL","<tag>LARIdentifierOflOnOffIdMap_SC-000</tag>/LAR/IdentifierOfl/OnOffIdMap_SC")
    if ( conddb.isMC and not conddb.folderRequested('/LAR/ElecCalibMCSC/AutoCorr') ) :
            conddb.addFolder("LAR_OFL","<tag>LARElecCalibMCSCAutoCorr-000</tag>/LAR/ElecCalibMCSC/AutoCorr")

    return tool

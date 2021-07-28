# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AthSequencer
from AthenaCommon.Logging import logging


def BunchCrossingCondAlgDefault():
    name = 'BunchCrossingCondAlgDefault'
    mlog = logging.getLogger(name)

    condSeq = AthSequencer ('AthCondSeq')

    if hasattr (condSeq, name):
        return getattr (condSeq, name)

    
    from LumiBlockComps.LumiBlockCompsConf import BunchCrossingCondAlg
    from IOVDbSvc.CondDB import conddb

    isMC=conddb.isMC
    run1=(conddb.dbdata == 'COMP200')

    if (isMC):
        folder = "/Digitization/Parameters"
        if not conddb.folderRequested(folder):
            mlog.info("Adding folder ", folder)
            conddb.addFolderWithTag('', folder, 'HEAD',
                                    className = 'AthenaAttributeList')
        else:
            mlog.info("Folder ",folder," already requested")
    else:
        folder = '/TDAQ/OLC/LHC/FILLPARAMS'
        # Mistakenly created as multi-version folder, must specify HEAD 
        conddb.addFolderWithTag('TDAQ', folder, 'HEAD',
                                className = 'AthenaAttributeList')
        
    alg = BunchCrossingCondAlg(name,
                               isMC=isMC,
                               Run1=run1,
                               FillParamsFolderKey =folder )

    condSeq += alg

    return alg

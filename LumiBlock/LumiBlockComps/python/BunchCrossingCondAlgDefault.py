# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AthSequencer
#from AthenaCommon.Logging import logging


def BunchCrossingCondAlgDefault():
    name = 'BunchCrossingCondAlgDefault'
    condSeq = AthSequencer ('AthCondSeq')

    if hasattr (condSeq, name):
        return getattr (condSeq, name)

    
    from LumiBlockComps.LumiBlockCompsConf import BunchCrossingCondAlg
    from IOVDbSvc.CondDB import conddb
 
    run1=(conddb.dbdata == 'COMP200')


    folder = '/TDAQ/OLC/LHC/FILLPARAMS'

    # Mistakenly created as multi-version folder, must specify HEAD 
    conddb.addFolderWithTag('TDAQ', folder, 'HEAD',
                            className = 'AthenaAttributeList')

    alg = BunchCrossingCondAlg(name,
                               Run1=run1,
                               FillParamsFolderKey =folder )

    condSeq += alg

    return alg

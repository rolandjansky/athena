# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# File: CoolLumiUtilities/python/FillParamsCondAlgDefault.py
# Created: May 2019, sss
# Purpose: Configure FillParamsCondAlg.
#


from AthenaCommon.AlgSequence import AthSequencer
from AthenaCommon.Logging import logging


def FillParamsCondAlgDefault():
    log = logging.getLogger ('FillParamsCondAlgDefault')
    name = 'FillParamsCondAlg'
    condSeq = AthSequencer ('AthCondSeq')

    if hasattr (condSeq, name):
        return getattr (condSeq, name)


    # Should only be used for Run 1.
    from IOVDbSvc.CondDB import conddb
    if conddb.dbdata != 'COMP200':
        return None

    folder = '/TDAQ/OLC/LHC/FILLPARAMS'

    # Mistakenly created as multi-version folder, must specify HEAD 
    conddb.addFolderWithTag('TDAQ', folder, 'HEAD',
                            className = 'AthenaAttributeList')
    log.info("FillParamsToolDefault requested %s", folder)

    from CoolLumiUtilities.CoolLumiUtilitiesConf import \
         FillParamsCondAlg

    alg = FillParamsCondAlg (name,
                             FillParamsFolderInputKey = folder,
                             FillParamsOutputKey = 'FillParamsCondData')
    condSeq += alg

    return alg

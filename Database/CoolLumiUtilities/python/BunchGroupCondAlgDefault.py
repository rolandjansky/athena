# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# File: CoolLumiUtilities/python/BunchGroupCondAlgDefault.py
# Created: May 2019, sss
# Purpose: Configure BunchGroupCondAlg.
#


from AthenaCommon.AlgSequence import AthSequencer


def BunchGroupCondAlgDefault():
    name = 'BunchGroupCondAlg'
    condSeq = AthSequencer ('AthCondSeq')

    if hasattr (condSeq, name):
        return getattr (condSeq, name)

    folder = ''

    from IOVDbSvc.CondDB import conddb
    if conddb.dbdata == 'COMP200':
        folder = '/TRIGGER/LVL1/BunchGroupContent'

        # Mistakenly created as multi-version folder, must specify HEAD 
        from IOVDbSvc.CondDB import conddb
        conddb.addFolderWithTag('TRIGGER', folder, 'HEAD',
                                className = 'AthenaAttributeList')

    from CoolLumiUtilities.CoolLumiUtilitiesConf import \
         BunchGroupCondAlg

    alg = BunchGroupCondAlg (name,
                             BunchGroupFolderInputKey = folder,
                             BunchGroupOutputKey = 'BunchGroupCondData')
    condSeq += alg

    return alg

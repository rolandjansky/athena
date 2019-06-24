# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# File: LumiBlockComps/python/TrigLiveFractionCondAlgDefault.py
# Created: Jun 2019, sss, from existing TrigLivefractionToolDefault.
# Purpose: Configure TrigLiveFractionCondAlg.
#


from AthenaCommon.AlgSequence import AthSequencer


# Configuration for TrigLiveFractionCondAlg
def TrigLiveFractionCondAlgDefault (name = 'TrigLiveFractionCondAlg'):
    condSeq = AthSequencer ('AthCondSeq')

    if hasattr (condSeq, name):
        return getattr (condSeq, name)

    from IOVDbSvc.CondDB import conddb

    kwargs = {}
    if conddb.dbdata == "COMP200":
        # Mistakenly created as multi-version folder, must specify HEAD
        conddb.addFolderWithTag('TRIGGER', '/TRIGGER/LUMI/PerBcidDeadtime', 'HEAD',
                                className = 'AthenaAttributeList')
        kwargs['DeadtimeFolderInputKey'] = '/TRIGGER/LUMI/PerBcidDeadtime'
        kwargs['LuminosityInputKey'] = 'LuminosityCondData'

        from LumiBlockComps.LuminosityCondAlgDefault import LuminosityCondAlgDefault
        LuminosityCondAlgDefault()

    else:
        kwargs['DeadtimeFolderInputKey'] = ''
        kwargs['LuminosityInputKey'] = ''


    from LumiBlockComps.LumiBlockCompsConf import TrigLiveFractionCondAlg
    alg = TrigLiveFractionCondAlg (name,
                                   TrigLiveFractionOutputKey = 'TrigLiveFractionCondData',
                                   **kwargs)
    condSeq += alg

    return alg


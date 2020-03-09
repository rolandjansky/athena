# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# File: CoolLumiUtilities/python/BunchLumisCondAlgDefault.py
# Created: May 2019, sss
# Purpose: Configure BunchLumisCondAlg.
#


from AthenaCommon.AlgSequence import AthSequencer


def BunchLumisCondAlgDefault():
    name = 'BunchLumisCondAlg'
    condSeq = AthSequencer ('AthCondSeq')

    if hasattr (condSeq, name):
        return getattr (condSeq, name)


    # Should only be used for Run 1.
    from IOVDbSvc.CondDB import conddb
    if conddb.dbdata != 'COMP200':
        return None

    folder = '/TDAQ/OLC/BUNCHLUMIS'

    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.isOverlay():
        # Load reduced channel list for overlay jobs to try to reduce COOL access
        # Need Lucid AND, OR, HitOR, BcmH OR, BcmV OR
        conddb.addFolder('TDAQ', '<channelSelection>101,102,103,201,211</channelSelection> /TDAQ/OLC/BUNCHLUMIS',
                         className = 'CondAttrListCollection')

    else:
        conddb.addFolder('TDAQ', folder,
                         className = 'CondAttrListCollection')

    from CoolLumiUtilities.CoolLumiUtilitiesConf import \
         BunchLumisCondAlg

    from CoolLumiUtilities.FillParamsCondAlgDefault import FillParamsCondAlgDefault
    fpalg = FillParamsCondAlgDefault()

    alg = BunchLumisCondAlg (name,
                             BunchLumisFolderInputKey = folder,
                             FillParamsInputKey = fpalg.FillParamsOutputKey,
                             BunchLumisOutputKey = 'BunchLumisCondData')
    condSeq += alg

    return alg

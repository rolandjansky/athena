# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# File: CoolLumiUtilities/python/OnlineLumiCalibrationCondAlgDefault.py
# Created: May 2019, sss
# Purpose: Configure OnlineLumiCalibrationCondAlg.
#


from AthenaCommon.AlgSequence import AthSequencer


def OnlineLumiCalibrationCondAlgDefault():
    name = 'OnlineLumiCalibrationCondAlg'
    condSeq = AthSequencer ('AthCondSeq')

    from IOVDbSvc.CondDB import conddb
    if conddb.isMC:
        return None

    if hasattr (condSeq, name):
        return getattr (condSeq, name)

    from CoolLumiUtilities.CoolLumiUtilitiesConf import OnlineLumiCalibrationCondAlg

    # For both runs 1 and 2
    folder = '/TDAQ/OLC/CALIBRATIONS'

    alg = OnlineLumiCalibrationCondAlg (name,
                                        CalibrationFolderInputKey = folder,
                                        LumiCalibOutputKey = 'OnlineLumiCalibrationCondData')
    condSeq += alg

    conddb.addFolder ('TDAQ', folder, className='CondAttrListCollection')

    return alg

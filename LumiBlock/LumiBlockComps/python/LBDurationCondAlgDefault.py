# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# File: LumiBlockComps/python/LBDurationCondAlgDefault.py
# Created: May 2019, sss
# Purpose: Configure LBDurationCondAlg.
#

from AthenaCommon.AlgSequence import AthSequencer

def LBDurationCondAlgDefault():
    name = 'LBDurationCondAlg'
    condSeq = AthSequencer ('AthCondSeq')

    if hasattr (condSeq, name):
        return getattr (condSeq, name)

    from IOVDbSvc.CondDB import conddb
    folder = "/TRIGGER/LUMI/LBLB"
    if not conddb.folderRequested( folder ):
        conddb.addFolder ('TRIGGER', folder,
                          className = 'AthenaAttributeList')

    from LumiBlockComps.LumiBlockCompsConf import LBDurationCondAlg
    alg = LBDurationCondAlg (name,
                             LBLBFolderInputKey = folder,
                             LBDurationOutputKey = 'LBDurationCondData')
    condSeq += alg

    return alg

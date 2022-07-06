# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# File: LumiBlockComps/python/LBDurationCondAlgConfig.py
# Created: May 2019, sss
# Purpose: Configure LBDurationCondAlg.
#

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFolders

def LBDurationCondAlgCfg (configFlags):
    name = 'LBDurationCondAlg'
    result = ComponentAccumulator()

    from IOVDbSvc.CondDB import conddb   # noqa: F401
    folder = "/TRIGGER/LUMI/LBLB"
    result.merge (addFolders (configFlags, folder, 'TRIGGER',
                              className = 'AthenaAttributeList'))

    LBDurationCondAlg=CompFactory.LBDurationCondAlg
    alg = LBDurationCondAlg (name,
                             LBLBFolderInputKey = folder,
                             LBDurationOutputKey = 'LBDurationCondData')

    result.addCondAlgo (alg)
    return result


if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    print ('--- data')
    flags1 = ConfigFlags.clone()
    flags1.Input.Files = defaultTestFiles.RAW
    flags1.lock()
    acc1 = LBDurationCondAlgCfg (flags1)
    acc1.printCondAlgs(summariseProps=True)
    print ('IOVDbSvc:', acc1.getService('IOVDbSvc').Folders)
    acc1.wasMerged()

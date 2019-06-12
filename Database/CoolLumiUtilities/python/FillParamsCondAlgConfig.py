# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# File: CoolLumiUtilities/python/FillParamsCondAlgConfig.py
# Created: May 2019, sss
# Purpose: Configure FillParamsCondAlg.
#


from __future__ import print_function


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFolders
from AthenaCommon.Logging import logging


def FillParamsCondAlgCfg (configFlags):
    log = logging.getLogger ('FillParamsCondAlgCfg')
    name = 'FillParamsCondAlg'
    result = ComponentAccumulator()

    # Should only be used for Run 1.
    if configFlags.IOVDb.DatabaseInstance != 'COMP200':
        return result

    folder = '/TDAQ/OLC/LHC/FILLPARAMS'
    # Mistakenly created as multi-version folder, must specify HEAD 
    result.merge (addFolders (configFlags, folder, 'TDAQ', tag='HEAD',
                              className='AthenaAttributeList'))

    from CoolLumiUtilities.CoolLumiUtilitiesConf import \
         FillParamsCondAlg
    alg = FillParamsCondAlg (name,
                             FillParamsFolderInputKey = folder,
                             FillParamsOutputKey = 'FillParamsCondData')

    result.addCondAlgo (alg)
    return result


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    print ('--- data')
    flags1 = ConfigFlags.clone()
    flags1.Input.Files = defaultTestFiles.RAW
    flags1.Input.ProjectName = 'data12_8TeV'
    flags1.lock()
    acc1 = FillParamsCondAlgCfg (flags1)
    acc1.printCondAlgs(summariseProps=True)
    print ('IOVDbSvc:', acc1.getService('IOVDbSvc').Folders)
    acc1.wasMerged()

    print ('--- default')
    flags2 = ConfigFlags.clone()
    flags2.Input.Files = defaultTestFiles.RAW
    flags2.lock()
    acc2 = FillParamsCondAlgCfg (flags2)
    acc2.printCondAlgs(summariseProps=True)
    print ('IOVDbSvc:', acc2.getServices())
    acc2.wasMerged()

#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
'''Functions setting default flags for generating online HLT python configuration'''

def setDefaultOnlineFlagsOldStyle():
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
    from AthenaCommon.GlobalFlags import globalflags as gf
    acf.isOnline.set_Value_and_Lock(True)
    acf.FilesInput.set_Value_and_Lock([])
    gf.InputFormat.set_Value_and_Lock('bytestream')
    gf.DataSource.set_Value_and_Lock('data')


def setDefaultOnlineFlagsNewStyle(flags):
    from AthenaConfiguration.Enums import Format
    flags.Common.isOnline = True
    flags.Input.Files = []
    flags.Input.isMC = False
    flags.Input.Format = Format.BS
    flags.Trigger.doHLT = True  # This distinguishes the HLT setup from online reco (GM, EventDisplay)


def defaultOnlineFlags(flags = None):
    setDefaultOnlineFlagsOldStyle()
    if not flags:
        from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    setDefaultOnlineFlagsNewStyle(flags)
    return flags

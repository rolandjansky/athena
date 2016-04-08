#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def rod_version_setup(trigger_release):

    from TrigSimTransforms.rodconfig import get_rod_config

    rodconfig = get_rod_config(trigger_release)

    # LAr setup
    # taken from https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TriggerRelease/trunk/share/Trigger_topOptions_writeBS.py
    lar_rodconfig = rodconfig["LAR"]
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc.LArRawDataContByteStreamTool.DSPRunMode      = lar_rodconfig["DSPRunMode"]
    ToolSvc.LArRawDataContByteStreamTool.RodBlockVersion = lar_rodconfig["RodBlockVersion"]
    ToolSvc.LArRawDataContByteStreamTool.IncludeDigits   = lar_rodconfig["IncludeDigits"]






def main():
    from TrigSimTransforms.rodconfig import get_rod_config
    print get_rod_config("17.1.3.4")
    return 0


if __name__=="__main__":
    import sys
    sys.exit(main())

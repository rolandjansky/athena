# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import *

try:
    from TrigTileMuId.TrigTileMuIdConf import TrigTileRODMuAlg
except:
    mlog.error("could not load TrigTileMuIdConf")
    import traceback
    print traceback.format_exc()


class TrigTileRODMuAlg_L2(TrigTileRODMuAlg):
    __slot__ = []
    def __init__(self, name='TrigTileRODMuAlg_L2'):
        TrigTileRODMuAlg.__init__(self, name)

        self.ReadRoIsFromL1 = True
        #self.ReadRoIsFromL1 = False

        self.TileMuTagsOutputName = "TileL2Cnt"

        self.UseRS = False

        self.ROBIDLIST = [0x510000,0x510001,0x510002,0x510003,0x510004,0x510005,0x510006,0x510007,  # LBA (0x100-0x11f)
                          0x510008,0x510009,0x51000a,0x51000b,0x51000c,0x51000d,0x51000e,0x51000f,  # LBA (0x120-0x13f)
                          0x520000,0x520001,0x520002,0x520003,0x520004,0x520005,0x520006,0x520007,  # LBC (0x200-0x21f)
                          0x520008,0x520009,0x52000a,0x52000b,0x52000c,0x52000d,0x52000e,0x52000f,  # LBC (0x220-0x23f)
                          0x530000,0x530001,0x530002,0x530003,0x530004,0x530005,0x530006,0x530007,  # EBA (0x300-0x31f)
                          0x530008,0x530009,0x53000a,0x53000b,0x53000c,0x53000d,0x53000e,0x53000f,  # EBA (0x320-0x33f)
                          0x540000,0x540001,0x540002,0x540003,0x540004,0x540005,0x540006,0x540007,  # EBC (0x400-0x41f)
                          0x540008,0x540009,0x54000a,0x54000b,0x54000c,0x54000d,0x54000e,0x54000f,  # EBC (0x420-0x43f)
                          ]

        self.EtaMin = -1.609430

        self.EtaMax = 1.609430

        self.PhiMin = 0.0

        self.PhiMax = 6.283185

        self.LooseSelection = True

        from TrigTileMuId.TrigTileMuIdMonitoring import TrigTileRODMuAlgOnlineMonitoring, TrigTileRODMuAlgCosmicMonitoring, TrigTileRODMuAlgValidationMonitoring
        online = TrigTileRODMuAlgOnlineMonitoring()
        cosmic = TrigTileRODMuAlgCosmicMonitoring()
        validation = TrigTileRODMuAlgValidationMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 70]

        self.AthenaMonTools = [ time, cosmic, validation, online ]

class TrigTileRODMuAlg_All(TrigTileRODMuAlg):
    __slot__ = []
    def __init__(self, name='TrigTileRODMuAlg_All'):
        TrigTileRODMuAlg.__init__(self, name)

        self.ReadRoIsFromL1 = True
        #self.ReadRoIsFromL1 = False

        self.TileMuTagsOutputName = "TileL2Cnt"

        self.UseRS = False

        self.ROBIDLIST = [0x510000,0x510001,0x510002,0x510003,0x510004,0x510005,0x510006,0x510007,  # LBA (0x100-0x11f)
                          0x510008,0x510009,0x51000a,0x51000b,0x51000c,0x51000d,0x51000e,0x51000f,  # LBA (0x120-0x13f)
                          0x520000,0x520001,0x520002,0x520003,0x520004,0x520005,0x520006,0x520007,  # LBC (0x200-0x21f)
                          0x520008,0x520009,0x52000a,0x52000b,0x52000c,0x52000d,0x52000e,0x52000f,  # LBC (0x220-0x23f)
                          0x530000,0x530001,0x530002,0x530003,0x530004,0x530005,0x530006,0x530007,  # EBA (0x300-0x31f)
                          0x530008,0x530009,0x53000a,0x53000b,0x53000c,0x53000d,0x53000e,0x53000f,  # EBA (0x320-0x33f)
                          0x540000,0x540001,0x540002,0x540003,0x540004,0x540005,0x540006,0x540007,  # EBC (0x400-0x41f)
                          0x540008,0x540009,0x54000a,0x54000b,0x54000c,0x54000d,0x54000e,0x54000f,  # EBC (0x420-0x43f)
                          ]

        self.EtaMin = -1.609430

        self.EtaMax = 1.609430

        self.PhiMin = 0.0

        self.PhiMax = 6.283185

        self.LooseSelection = True

        from TrigTileMuId.TrigTileMuIdMonitoring import TrigTileRODMuAlgOnlineMonitoring, TrigTileRODMuAlgCosmicMonitoring, TrigTileRODMuAlgValidationMonitoring
        online = TrigTileRODMuAlgOnlineMonitoring()
        cosmic = TrigTileRODMuAlgCosmicMonitoring()
        validation = TrigTileRODMuAlgValidationMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 70]

        self.AthenaMonTools = [ time, cosmic, validation, online ]

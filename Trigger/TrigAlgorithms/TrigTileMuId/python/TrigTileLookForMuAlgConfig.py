# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import *

try:
    from TrigTileMuId.TrigTileMuIdConf import TrigTileLookForMuAlg
except:
    mlog.error("could not load TrigTileMuIdConf")
    import traceback
    print traceback.format_exc()

from AthenaCommon.SystemOfUnits import MeV


class TrigTileLookForMuAlg_L2(TrigTileLookForMuAlg):
    __slot__ = []
    def __init__(self, name='TrigTileLookForMuAlg_L2'):
        TrigTileLookForMuAlg.__init__(self, name)

        #self.ReadRoIsFromL1 = False
        self.ReadRoIsFromL1 = True
        self.DelEta_ForRoIs = 0.3
        self.DelPhi_ForRoIs = 0.2
 
        #self.AddCosmicLVL2 = True
        self.AddCosmicLVL2 = False
         
        self.LooseSelection = True
 
        self.LowerTreshForCosmic = 150.0*MeV

        self.TileMuTagsOutputName = "TileMuObj"

        self.LowerTresh0MeV = 150.0*MeV

        self.LowerTresh1MeV = 150.0*MeV

        self.LowerTresh2MeV = 150.0*MeV

        self.LowerTreshScinMeV = 160.0*MeV

        # 3rd layer
        self.UpperTresh2MeV = [2370.0*MeV, 2100.0*MeV,  900.0*MeV, 1050.0*MeV, 1050.0*MeV,
                               1110.0*MeV, 1590.0*MeV, 1110.0*MeV, 1050.0*MeV, 1050.0*MeV,
                                900.0*MeV, 2100.0*MeV, 2370.0*MeV]

        # 2nd layer
        self.UpperTresh1MeV = [1680.0*MeV, 1500.0*MeV, 1440.0*MeV, 1380.0*MeV, 1050.0*MeV,
                                390.0*MeV, 1110.0*MeV, 1860.0*MeV, 1890.0*MeV, 1800.0*MeV,
                               1860.0*MeV, 1890.0*MeV, 1770.0*MeV, 1980.0*MeV, 2550.0*MeV,
                               2550.0*MeV, 1980.0*MeV, 1770.0*MeV, 1890.0*MeV, 1860.0*MeV,
                               1800.0*MeV, 1890.0*MeV, 1860.0*MeV, 1110.0*MeV,  390.0*MeV,
                               1050.0*MeV, 1380.0*MeV, 1440.0*MeV, 1500.0*MeV, 1680.0*MeV]

        # 1st layer
        self.UpperTresh0MeV = [1680.0*MeV, 1380.0*MeV, 1230.0*MeV, 1140.0*MeV,  210.0*MeV,
                                900.0*MeV,  960.0*MeV,  840.0*MeV,  930.0*MeV,  840.0*MeV,
                                840.0*MeV,  750.0*MeV,  870.0*MeV,  960.0*MeV, 1350.0*MeV,
                               1350.0*MeV,  960.0*MeV,  870.0*MeV,  750.0*MeV,  840.0*MeV,
                                840.0*MeV,  930.0*MeV,  840.0*MeV,  960.0*MeV,  900.0*MeV,
                                210.0*MeV, 1140.0*MeV, 1230.0*MeV, 1380.0*MeV, 1680.0*MeV]

        self.From3to2 = [5,0,1,2,3,4,
                         5,2,3,4,5,6,
                         2,6,7,0,0,0,
                         3,7,8,9,0,0,
                         3,9,10,11,0,0,
                         2,12,13,0,0,0,
                         2,14,15,0,0,0,
                         2,16,17,0,0,0,
                         3,18,19,20,0,0,
                         3,20,21,22,0,0,
                         2,22,23,0,0,0,
                         5,23,24,25,26,27,
                         5,25,26,27,28,29]

        self.From2to1 = [2,1,2,0,0,0,    3,1,2,3,0,0,     3,2,3,4,0,0,     3,3,4,5,0,0,     2,4,5,0,0,0,
                         1,5,0,0,0,0,    1,6,0,0,0,0,     1,7,0,0,0,0,     1,8,0,0,0,0,     1,9,0,0,0,0,
                         1,10,0,0,0,0,   1,11,0,0,0,0,    1,12,0,0,0,0,    1,13,0,0,0,0,    1,14,0,0,0,0,
                         1,15,0,0,0,0,   1,16,0,0,0,0,    1,17,0,0,0,0,    1,18,0,0,0,0,    1,19,0,0,0,0,
                         1,20,0,0,0,0,   1,21,0,0,0,0,    1,22,0,0,0,0,    1,23,0,0,0,0,    1,24,0,0,0,0,
                         2,24,25,0,0,0,  3,24,25,26,0,0,  3,25,26,27,0,0,  3,26,27,28,0,0,  2,27,28,0,0,0]

        self.TimerNtuple = "TileMuId_time"

        self.RoISearch = False

        self.RoISizePhi = 0.3

        self.RoISizeEta = 0.3
   
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

        self.DRAWERIDLIST = [ 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  # LBA (0x100-0x10f)
                             16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  # LBA (0x110-0x11f)
                             32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  # LBA (0x120-0x12f)
                             48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  # LBA (0x130-0x13f)
                             64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  # LBC (0x200-0x20f)
                             80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  # LBC (0x210-0x21f)
                             96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,  # LBC (0x220-0x22f)
                            112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,  # LBC (0x230-0x23f)
                            128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,  # EBA (0x300-0x30f)
                            144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,  # EBA (0x310-0x31f)
                            160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,  # EBA (0x320-0x32f)
                            176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,  # EBA (0x330-0x33f)
                            192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,  # EBC (0x400-0x40f)
                            208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,  # EBC (0x410-0x41f)
                            224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,  # EBC (0x420-0x42f)
                            240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,  # EBC (0x430-0x43f)
                              ]

        from TrigTileMuId.TrigTileMuIdMonitoring import TrigTileLookForMuAlgOnlineMonitoring, TrigTileLookForMuAlgCosmicMonitoring, TrigTileLookForMuAlgValidationMonitoring
        online = TrigTileLookForMuAlgOnlineMonitoring()
        cosmic = TrigTileLookForMuAlgCosmicMonitoring()
        validation = TrigTileLookForMuAlgValidationMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 70]

        self.AthenaMonTools = [ time, cosmic, validation, online ] 


class TrigTileLookForMuAlg_All(TrigTileLookForMuAlg):
    __slot__ = []
    def __init__(self, name='TrigTileLookForMuAlg_All'):
        TrigTileLookForMuAlg.__init__(self, name)

        #self.ReadRoIsFromL1 = False
        self.ReadRoIsFromL1 = True
        self.DelEta_ForRoIs = 0.3
        self.DelPhi_ForRoIs = 0.2
 
        #self.AddCosmicLVL2 = True
        self.AddCosmicLVL2 = False
         
        self.LooseSelection = True
 
        self.LowerTreshForCosmic = 150.0*MeV

        self.TileMuTagsOutputName = "TileMuObj"

        self.LowerTresh0MeV = 150.0*MeV

        self.LowerTresh1MeV = 150.0*MeV

        self.LowerTresh2MeV = 150.0*MeV

        self.LowerTreshScinMeV = 160.0*MeV

        # 3rd layer
        self.UpperTresh2MeV = [2370.0*MeV, 2100.0*MeV,  900.0*MeV, 1050.0*MeV, 1050.0*MeV,
                               1110.0*MeV, 1590.0*MeV, 1110.0*MeV, 1050.0*MeV, 1050.0*MeV,
                                900.0*MeV, 2100.0*MeV, 2370.0*MeV]

        # 2nd layer
        self.UpperTresh1MeV = [1680.0*MeV, 1500.0*MeV, 1440.0*MeV, 1380.0*MeV, 1050.0*MeV,
                                390.0*MeV, 1110.0*MeV, 1860.0*MeV, 1890.0*MeV, 1800.0*MeV,
                               1860.0*MeV, 1890.0*MeV, 1770.0*MeV, 1980.0*MeV, 2550.0*MeV,
                               2550.0*MeV, 1980.0*MeV, 1770.0*MeV, 1890.0*MeV, 1860.0*MeV,
                               1800.0*MeV, 1890.0*MeV, 1860.0*MeV, 1110.0*MeV,  390.0*MeV,
                               1050.0*MeV, 1380.0*MeV, 1440.0*MeV, 1500.0*MeV, 1680.0*MeV]

        # 1st layer
        self.UpperTresh0MeV = [1680.0*MeV, 1380.0*MeV, 1230.0*MeV, 1140.0*MeV,  210.0*MeV,
                                900.0*MeV,  960.0*MeV,  840.0*MeV,  930.0*MeV,  840.0*MeV,
                                840.0*MeV,  750.0*MeV,  870.0*MeV,  960.0*MeV, 1350.0*MeV,
                               1350.0*MeV,  960.0*MeV,  870.0*MeV,  750.0*MeV,  840.0*MeV,
                                840.0*MeV,  930.0*MeV,  840.0*MeV,  960.0*MeV,  900.0*MeV,
                                210.0*MeV, 1140.0*MeV, 1230.0*MeV, 1380.0*MeV, 1680.0*MeV]

        self.From3to2 = [5,0,1,2,3,4,
                         5,2,3,4,5,6,
                         2,6,7,0,0,0,
                         3,7,8,9,0,0,
                         3,9,10,11,0,0,
                         2,12,13,0,0,0,
                         2,14,15,0,0,0,
                         2,16,17,0,0,0,
                         3,18,19,20,0,0,
                         3,20,21,22,0,0,
                         2,22,23,0,0,0,
                         5,23,24,25,26,27,
                         5,25,26,27,28,29]

        self.From2to1 = [2,1,2,0,0,0,    3,1,2,3,0,0,     3,2,3,4,0,0,     3,3,4,5,0,0,     2,4,5,0,0,0,
                         1,5,0,0,0,0,    1,6,0,0,0,0,     1,7,0,0,0,0,     1,8,0,0,0,0,     1,9,0,0,0,0,
                         1,10,0,0,0,0,   1,11,0,0,0,0,    1,12,0,0,0,0,    1,13,0,0,0,0,    1,14,0,0,0,0,
                         1,15,0,0,0,0,   1,16,0,0,0,0,    1,17,0,0,0,0,    1,18,0,0,0,0,    1,19,0,0,0,0,
                         1,20,0,0,0,0,   1,21,0,0,0,0,    1,22,0,0,0,0,    1,23,0,0,0,0,    1,24,0,0,0,0,
                         2,24,25,0,0,0,  3,24,25,26,0,0,  3,25,26,27,0,0,  3,26,27,28,0,0,  2,27,28,0,0,0]

        self.TimerNtuple = "TileMuId_time"

        self.RoISearch = False

        self.RoISizePhi = 0.3

        self.RoISizeEta = 0.3
   
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

        self.DRAWERIDLIST = [ 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  # LBA (0x100-0x10f)
                             16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  # LBA (0x110-0x11f)
                             32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  # LBA (0x120-0x12f)
                             48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  # LBA (0x130-0x13f)
                             64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  # LBC (0x200-0x20f)
                             80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  # LBC (0x210-0x21f)
                             96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,  # LBC (0x220-0x22f)
                            112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,  # LBC (0x230-0x23f)
                            128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,  # EBA (0x300-0x30f)
                            144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,  # EBA (0x310-0x31f)
                            160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,  # EBA (0x320-0x32f)
                            176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,  # EBA (0x330-0x33f)
                            192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,  # EBC (0x400-0x40f)
                            208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,  # EBC (0x410-0x41f)
                            224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,  # EBC (0x420-0x42f)
                            240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,  # EBC (0x430-0x43f)
                              ]

        from TrigTileMuId.TrigTileMuIdMonitoring import TrigTileLookForMuAlgOnlineMonitoring, TrigTileLookForMuAlgCosmicMonitoring, TrigTileLookForMuAlgValidationMonitoring
        online = TrigTileLookForMuAlgOnlineMonitoring()
        cosmic = TrigTileLookForMuAlgCosmicMonitoring()
        validation = TrigTileLookForMuAlgValidationMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 70]

        self.AthenaMonTools = [ time, cosmic, validation, online ] 

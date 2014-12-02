# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


compNames_all = [ "PreSamplB", "EMB1", "EMB2", "EMB3",   # LAr barrel
                  "PreSamplE", "EME1", "EME2", "EME3",   # LAr EM endcap
                  "HEC0",      "HEC1", "HEC2", "HEC3",   # Hadronic end cap cal.
                  "TileBar0", "TileBar1", "TileBar2",    # Tile barrel
                  "TileGap1", "TileGap2", "TileGap3",    # Tile gap (ITC & scint)
                  "TileExt0", "TileExt1", "TileExt2",    # Tile extended barrel
                  "FCalEM",   "FCalHad1", "FCalHad2",    # Forward cal endcap
                  "Muons" ]                              # Muons

compNames_FEB = [ "PreSamplB", "EMB1", "EMB2", "EMB3",    # LAr barrel
                  "PreSamplE", "EME1", "EME2", "EME3",    # LAr EM endcap
                  "HEC", "TileBar", "TileExt",            # Hadronic end cap cal. Tile cal.
                  "FCalEM", "FCalHad1", "FCalHad2",       # Forward cal endcap
                  "Muons" ]                               #Muons

compNames_topocl = [ "TCLCWB1  ", "TCLCWB2  ",              #pos. and neg. eta barrel
                     "TCLCWE1  ", "TCLCWE2  ",              #pos. and neg. eta endcap
                     "TCEMB1   ", "TCEMB2   ",              #pos. and neg. eta barrel
                     "TCEME1   ", "TCEME2   ",              #pos. and neg. eta endcap
                     "Muons" ]                              #Muons

bitNames_allHLT= [ 
             "Processing",         # bit  0
             "ErrBSconv",          # bit  1
             "ErrMuon",            # bit  2
             "ErrFEB",             # bit  3
             "Skipped",            # bit  4
             "CompBigMEtSEtRatio", # bit  5
             "BadCompEnergy",      # bit  6
             "BadEnergyRatio",     # bit  7
             "spare",              # bit  8
             "BadCellQuality",     # bit  9
             "BadCellEnergy",      # bit 10
             "BadCellTime",        # bit 11
             "NoMuonTrack",        # bit 12
             "spare",              # bit 13
             "Processed",          # bit 14
             "CompError",          # bit 15
             "EMB_A_Missing",      # bit 16
             "EMB_C_Missing",      # bit 17
             "EME_A_Missing",      # bit 18
             "EME_C_Missing",      # bit 19
             "HEC_A_Missing",      # bit 20
             "HEC_C_Missing",      # bit 21
             "FCAL_A_Missing",     # bit 22
             "FCAL_C_Missing",     # bit 23
             "TileB_A_Missing",    # bit 24
             "TileB_C_Missing",    # bit 25
             "TileE_A_Missing",    # bit 26
             "TileE_C_Missing",    # bit 27
             "BadEMfraction",      # bit 28
             "GlobBigMEtSEtRatio", # bit 29
             "ObjInCrack",         # bit 30
             "GlobError"           # bit 31
             ]

def HLTMETMonitoringTool():
        from TrigMETMonitoring.TrigMETMonitoringConf import HLTMETMonTool

        HLTMETMon = HLTMETMonTool(name          = 'HLTMETMon',
                                  histoPathBase = "/Trigger/HLT", 
                                  MonPathBase   = "/HLT/METMon",
                                  CompNames   = compNames_all,
                                  BitNames    = bitNames_allHLT,

                                  METTriggers = ["L1_XE35", "L1_XE50", "L1_XE70",
                                                 "HLT_xe50", "HLT_xe70"],

                                  SampleSelectionTriggers = ["HLT_J10",     # jet trigger
                                                             "HLT_MbSp",    # minbias trigger
                                                             "HLT_mu6_rpc"] # other physics trigger
                                  );
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += HLTMETMon;
        list = [ "HLTMETMonTool/HLTMETMon" ];

        # HLTMETMon_FEB = HLTMETMonTool(name          = 'HLTMETMon_FEB',
        #                           histoPathBase = "/Trigger/HLT", 
        #                           MonPathBase   = "/HLT/METMon_FEB",
        #                           #L2METKey      = "HLT_T2MissingET",
        #                           L2FEBKey      = "HLT_L2MissingET_FEB",
        #                           EFMETKey      = "HLT_TrigEFMissingET_FEB",
        #                           compNamesEF   = compNames_all,
        #                           compNamesL2   = compNames_L2FEB,
        #                           bitNamesL2    = bitNames_allL2,
        #                           bitNamesEF    = bitNames_allEF
        #                           );
        # ToolSvc += HLTMETMon_FEB;
        # list += [ "HLTMETMonTool/HLTMETMon_FEB" ];


        # HLTMETMon_topocl = HLTMETMonTool(name          = 'HLTMETMon_topocl',
        #                           histoPathBase = "/Trigger/HLT", 
        #                           MonPathBase   = "/HLT/METMon_topocl",
        #                           #L2METKey      = "HLT_T2MissingET",
        #                           #L2FEBKey      = "HLT_L2MissingET_FEB",
        #                           EFMETKey      = "HLT_TrigEFMissingET_topocl",
        #                           #compNamesEF   = compNames_all,
        #                           compNamesEF   = compNames_topocl,
        #                           compNamesL2   = compNames_L2FEB,
        #                           bitNamesL2    = bitNames_allL2,
        #                           bitNamesEF    = bitNames_allEF
        #                           );
        # ToolSvc += HLTMETMon_topocl;
        # list += [ "HLTMETMonTool/HLTMETMon_topocl" ];
        return list

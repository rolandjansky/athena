DiMuMonDoROOTFile = False
doFits = True
doSaveFits = False
doJpsi = True
doZmumu = True

#---------------------------
# ["All","BB","EAEA","ECEC"]
#---------------------------
regionsJpsi = ["All","BB","EAEA","ECEC"]
regionsZmumu = ["All","BB","EAEA","ECEC"]

#---------------------------------------------------------------------------------------------------------------------------------------------------
# ["eta","etaAll","etaPos","etaNeg","phi","phiAll","phiPos","phiNeg","pt","ptAll","ptPos","ptNeg","etaDiff","etaSumm","phiDiff","phiSumm","crtDiff"]
#---------------------------------------------------------------------------------------------------------------------------------------------------
varsVSmeanJpsi = ["eta","etaAll","etaPos","etaNeg","phi","phiAll","phiPos","phiNeg","pt","ptAll","ptPos","ptNeg","etaDiff","etaSumm","phiDiff","phiSumm","crtDiff"]
varsVSwidthJpsi = ["etaAll","etaPos","etaNeg","phiAll","phiPos","phiNeg","ptAll","ptPos","ptNeg","etaDiff","phiDiff","crtDiff"]
varsDistrJpsi = ["etaAll","etaPos","etaNeg","phiAll","phiPos","phiNeg","ptAll","ptPos","ptNeg"]

varsVSmeanZmumu = ["eta","etaAll","etaPos","etaNeg","phi","phiAll","phiPos","phiNeg","pt","ptAll","ptPos","ptNeg","etaDiff","etaSumm","phiDiff","phiSumm","crtDiff"]
varsVSwidthZmumu = ["etaAll","etaPos","etaNeg","phiAll","phiPos","phiNeg","ptAll","ptPos","ptNeg","etaDiff","phiDiff","crtDiff"]
varsDistrZmumu = ["etaAll","etaPos","etaNeg","phiAll","phiPos","phiNeg","ptAll","ptPos","ptNeg"]
#include( "TrackIsolationTools/TrackIsolationTool_jobOptions.py" )


#
# pp collisions
#
if not rec.doHeavyIon():
    from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
    InDetDiMuMonManager = AthenaMonManager( name = "InDetDiMuMonManager",
                                    FileKey = "GLOBAL",
                                    ManualDataTypeSetup = True,
                                    DataType            = "userDefined", # use this for collision data for now
                                    Environment         = "user",
                                    ManualRunLBSetup    = True,
                                    Run                 = 1,
                                    LumiBlock           = 1)
    
    
    from InDetDiMuonMonitoring.InDetDiMuonMonitoringConf import DiMuMon
    if doJpsi:
        JpsiMon_NoTrig = DiMuMon (name = "JpsiMon_NoTrig",
                                resonName = "Jpsi",
                                minInvmass = 2.5,
                                maxInvmass = 3.5,
                                nMassBins = 50,
                                triggerChainName = "NoTrig",
                                regions = regionsJpsi,
                                varsVSmean = varsVSmeanJpsi,
                                varsVSwidth = varsVSwidthJpsi,
                                varsDistr = varsDistrJpsi,
                                doFits = doFits,
                                doSaveFits = doSaveFits)
    #                              OutputLevel = 1)
    
    
        ToolSvc += JpsiMon_NoTrig
        if (InDetFlags.doPrintConfigurables()):
            print JpsiMon_NoTrig
        InDetDiMuMonManager.AthenaMonTools += [ JpsiMon_NoTrig ]
    
    if doZmumu:
        ZmumuMon_NoTrig = DiMuMon (name = "ZmumuMon_NoTrig",
                                resonName = "Zmumu",
                                minInvmass = 60.,
                                maxInvmass = 120.,
                                nMassBins = 60,
                                triggerChainName = "NoTrig",
                                regions = regionsZmumu,
                                varsVSmean = varsVSmeanZmumu,
                                varsVSwidth = varsVSwidthZmumu,
                                varsDistr = varsDistrZmumu,
                                doFits = doFits,
                                doSaveFits = doSaveFits)
    #                               OutputLevel = 1)
    
        ToolSvc += ZmumuMon_NoTrig
        if (InDetFlags.doPrintConfigurables()):
            print ZmumuMon_NoTrig
        InDetDiMuMonManager.AthenaMonTools += [ ZmumuMon_NoTrig ]
    
    
    
    if not DQMonFlags.useTrigger():
        print "DiMuMon_jobOptions.py: trigger decision tool not found: don't run trigger-aware monitoring"
    else:
        if doJpsi:
            JpsiMon_XpressTrig_idperf = DiMuMon (name = "JpsiMon_XpressTrig_idperf",
                                        resonName = "Jpsi",
                                        minInvmass = 2.5,
                                        maxInvmass = 3.5,
                                        nMassBins = 50,
                                        triggerChainName = "XpressTrig_idperf",
                                        regions = regionsJpsi,
                                        varsVSmean = varsVSmeanJpsi,
                                        varsVSwidth = varsVSwidthJpsi,
                                        varsDistr = varsDistrJpsi,
                                        doFits = doFits,
                                        doSaveFits = doSaveFits)
            JpsiMon_XpressTrig_idperf.TriggerChain = "HLT_mu4_mu4_idperf_bJpsimumu_noid"
            JpsiMon_XpressTrig_idperf.TrigDecisionTool = monTrigDecTool
            ToolSvc += JpsiMon_XpressTrig_idperf
            if (InDetFlags.doPrintConfigurables()):
                print JpsiMon_XpressTrig_idperf
            InDetDiMuMonManager.AthenaMonTools += [ JpsiMon_XpressTrig_idperf ]
    
            JpsiMon_XpressTrig_2mu = DiMuMon (name = "JpsiMon_XpressTrig_2mu",
                                        resonName = "Jpsi",
                                        minInvmass = 2.5,
                                        maxInvmass = 3.5,
                                        nMassBins = 50,
                                        triggerChainName = "XpressTrig_2mu",
                                        regions = regionsJpsi,
                                        varsVSmean = varsVSmeanJpsi,
                                        varsVSwidth = varsVSwidthJpsi,
                                        varsDistr = varsDistrJpsi,
                                        doFits = doFits,
                                        doSaveFits = doSaveFits)
            JpsiMon_XpressTrig_2mu.TriggerChain = "HLT_2mu4"
            JpsiMon_XpressTrig_2mu.TrigDecisionTool = monTrigDecTool
            ToolSvc += JpsiMon_XpressTrig_2mu
            if (InDetFlags.doPrintConfigurables()):
                print JpsiMon_XpressTrig_2mu
            InDetDiMuMonManager.AthenaMonTools += [ JpsiMon_XpressTrig_2mu ]
    
    
        if doZmumu:
            ZmumuMon_XpressTrig_idperf = DiMuMon (name = "ZmumuMon_XpressTrig_idperf",
                                        resonName = "Zmumu",
                                        minInvmass = 60.,
                                        maxInvmass = 120.,
                                        nMassBins = 60,
                                        triggerChainName = "XpressTrig_idperf",
                                        regions = regionsZmumu,
                                        varsVSmean = varsVSmeanZmumu,
                                        varsVSwidth = varsVSwidthZmumu,
                                        varsDistr = varsDistrZmumu,
                                        doFits = doFits,
                                        doSaveFits = doSaveFits)
            ZmumuMon_XpressTrig_idperf.TriggerChain = "HLT_mu13_mu13_idperf_Zmumu"
            ZmumuMon_XpressTrig_idperf.TrigDecisionTool = monTrigDecTool
            ToolSvc += ZmumuMon_XpressTrig_idperf
            if (InDetFlags.doPrintConfigurables()):
                print ZmumuMon_XpressTrig_idperf
            InDetDiMuMonManager.AthenaMonTools += [ ZmumuMon_XpressTrig_idperf ]
    
    
            ZmumuMon_XpressTrig_2mu = DiMuMon (name = "ZmumuMon_XpressTrig_2mu",
                                        resonName = "Zmumu",
                                        minInvmass = 60.,
                                        maxInvmass = 120.,
                                        nMassBins = 60,
                                        triggerChainName = "XpressTrig_2mu",
                                        regions = regionsZmumu,
                                        varsVSmean = varsVSmeanZmumu,
                                        varsVSwidth = varsVSwidthZmumu,
                                        varsDistr = varsDistrZmumu,
                                        doFits = doFits,
                                        doSaveFits = doSaveFits)
            ZmumuMon_XpressTrig_2mu.TriggerChain = "HLT_2mu14"
            ZmumuMon_XpressTrig_2mu.TrigDecisionTool = monTrigDecTool
            ToolSvc += ZmumuMon_XpressTrig_2mu
            if (InDetFlags.doPrintConfigurables()):
                print ZmumuMon_XpressTrig_2mu
            InDetDiMuMonManager.AthenaMonTools += [ ZmumuMon_XpressTrig_2mu ]
    
    ## Setup the output histogram file(s):
    if not hasattr(ServiceMgr, 'THistSvc'):
        from GaudiSvc.GaudiSvcConf import THistSvc
        ServiceMgr += THistSvc()
    
    if DiMuMonDoROOTFile:
        THistSvc = Service( "THistSvc" )
        histOutput = "DiMuMon DATAFILE='./DiMuMon.root' OPT='RECREATE'"
        THistSvc.Output += [histOutput]
        InDetDiMuMonManager.FileKey = "DiMuMon"
    
    topSequence +=InDetDiMuMonManager
    if (InDetFlags.doPrintConfigurables()):
        print InDetDiMuMonManager




#
# HI collisions
#
if rec.doHeavyIon():

    from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
    InDetDiMuMonManager = AthenaMonManager( name = "InDetDiMuMonManager",
                                    FileKey = "GLOBAL",
                                    ManualDataTypeSetup = True,
                                    DataType            = "userDefined", # use this for collision data for now
                                    Environment         = "user",
                                    ManualRunLBSetup    = True,
                                    Run                 = 1,
                                    LumiBlock           = 1)
    
    
    from InDetDiMuonMonitoring.InDetDiMuonMonitoringConf import DiMuMon
    if doJpsi:
        JpsiMon_NoTrig = DiMuMon (name = "JpsiMon_NoTrig",
                                resonName = "Jpsi",
                                minInvmass = 2.5,
                                maxInvmass = 3.5,
                                nMassBins = 50,
                                triggerChainName = "NoTrig",
                                regions = regionsJpsi,
                                varsVSmean = varsVSmeanJpsi,
                                varsVSwidth = varsVSwidthJpsi,
                                varsDistr = varsDistrJpsi,
                                doFits = doFits,
                                doSaveFits = doSaveFits)
    #                              OutputLevel = 1)
    
    
        ToolSvc += JpsiMon_NoTrig
        if (InDetFlags.doPrintConfigurables()):
            print JpsiMon_NoTrig
        InDetDiMuMonManager.AthenaMonTools += [ JpsiMon_NoTrig ]
    
    if doZmumu:
        ZmumuMon_NoTrig = DiMuMon (name = "ZmumuMon_NoTrig",
                                resonName = "Zmumu",
                                minInvmass = 60.,
                                maxInvmass = 120.,
                                nMassBins = 60,
                                triggerChainName = "NoTrig",
                                regions = regionsZmumu,
                                varsVSmean = varsVSmeanZmumu,
                                varsVSwidth = varsVSwidthZmumu,
                                varsDistr = varsDistrZmumu,
                                doFits = doFits,
                                doSaveFits = doSaveFits)
    #                               OutputLevel = 1)
    
        ToolSvc += ZmumuMon_NoTrig
        if (InDetFlags.doPrintConfigurables()):
            print ZmumuMon_NoTrig
        InDetDiMuMonManager.AthenaMonTools += [ ZmumuMon_NoTrig ]
    
    
    
    if not hasattr(ToolSvc, 'monTrigDecTool'):
        print "DiMuMon_jobOptions.py: trigger decision tool not found: don't run trigger-aware monitoring"
    else:
        if doJpsi:
            JpsiMon_XpressTrig_idperf = DiMuMon (name = "JpsiMon_XpressTrig_idperf",
                                        resonName = "Jpsi",
                                        minInvmass = 2.5,
                                        maxInvmass = 3.5,
                                        nMassBins = 50,
                                        triggerChainName = "XpressTrig_idperf",
                                        regions = regionsJpsi,
                                        varsVSmean = varsVSmeanJpsi,
                                        varsVSwidth = varsVSwidthJpsi,
                                        varsDistr = varsDistrJpsi,
                                        doFits = doFits,
                                        doSaveFits = doSaveFits)
            JpsiMon_XpressTrig_idperf.TriggerChain = "HLT_mu4_mu4_idperf_bJpsimumu_noid"
            JpsiMon_XpressTrig_idperf.TrigDecisionTool = monTrigDecTool
            ToolSvc += JpsiMon_XpressTrig_idperf
            if (InDetFlags.doPrintConfigurables()):
                print JpsiMon_XpressTrig_idperf
            InDetDiMuMonManager.AthenaMonTools += [ JpsiMon_XpressTrig_idperf ]
    
            JpsiMon_XpressTrig_2mu = DiMuMon (name = "JpsiMon_XpressTrig_2mu",
                                        resonName = "Jpsi",
                                        minInvmass = 2.5,
                                        maxInvmass = 3.5,
                                        nMassBins = 50,
                                        triggerChainName = "XpressTrig_2mu",
                                        regions = regionsJpsi,
                                        varsVSmean = varsVSmeanJpsi,
                                        varsVSwidth = varsVSwidthJpsi,
                                        varsDistr = varsDistrJpsi,
                                        doFits = doFits,
                                        doSaveFits = doSaveFits)
            JpsiMon_XpressTrig_2mu.TriggerChain = "HLT_mu4_mu4noL1"
            JpsiMon_XpressTrig_2mu.TrigDecisionTool = monTrigDecTool
            ToolSvc += JpsiMon_XpressTrig_2mu
            if (InDetFlags.doPrintConfigurables()):
                print JpsiMon_XpressTrig_2mu
            InDetDiMuMonManager.AthenaMonTools += [ JpsiMon_XpressTrig_2mu ]
    
    
        if doZmumu:
            ZmumuMon_XpressTrig_idperf = DiMuMon (name = "ZmumuMon_XpressTrig_idperf",
                                        resonName = "Zmumu",
                                        minInvmass = 60.,
                                        maxInvmass = 120.,
                                        nMassBins = 60,
                                        triggerChainName = "XpressTrig_idperf",
                                        regions = regionsZmumu,
                                        varsVSmean = varsVSmeanZmumu,
                                        varsVSwidth = varsVSwidthZmumu,
                                        varsDistr = varsDistrZmumu,
                                        doFits = doFits,
                                        doSaveFits = doSaveFits)
            ZmumuMon_XpressTrig_idperf.TriggerChain = "HLT_mu13_mu13_idperf_Zmumu"
            ZmumuMon_XpressTrig_idperf.TrigDecisionTool = monTrigDecTool
            ToolSvc += ZmumuMon_XpressTrig_idperf
            if (InDetFlags.doPrintConfigurables()):
                print ZmumuMon_XpressTrig_idperf
            InDetDiMuMonManager.AthenaMonTools += [ ZmumuMon_XpressTrig_idperf ]
    
    
            ZmumuMon_XpressTrig_2mu = DiMuMon (name = "ZmumuMon_XpressTrig_2mu",
                                        resonName = "Zmumu",
                                        minInvmass = 60.,
                                        maxInvmass = 120.,
                                        nMassBins = 60,
                                        triggerChainName = "XpressTrig_2mu",
                                        regions = regionsZmumu,
                                        varsVSmean = varsVSmeanZmumu,
                                        varsVSwidth = varsVSwidthZmumu,
                                        varsDistr = varsDistrZmumu,
                                        doFits = doFits,
                                        doSaveFits = doSaveFits)
            ZmumuMon_XpressTrig_2mu.TriggerChain = "HLT_mu4_mu4noL1"
            ZmumuMon_XpressTrig_2mu.TrigDecisionTool = monTrigDecTool
            ToolSvc += ZmumuMon_XpressTrig_2mu
            if (InDetFlags.doPrintConfigurables()):
                print ZmumuMon_XpressTrig_2mu
            InDetDiMuMonManager.AthenaMonTools += [ ZmumuMon_XpressTrig_2mu ]
    
    ## Setup the output histogram file(s):
    if not hasattr(ServiceMgr, 'THistSvc'):
        from GaudiSvc.GaudiSvcConf import THistSvc
        ServiceMgr += THistSvc()
    
    if DiMuMonDoROOTFile:
        THistSvc = Service( "THistSvc" )
        histOutput = "DiMuMon DATAFILE='./DiMuMon.root' OPT='RECREATE'"
        THistSvc.Output += [histOutput]
        InDetDiMuMonManager.FileKey = "DiMuMon"
    
    topSequence +=InDetDiMuMonManager
    if (InDetFlags.doPrintConfigurables()):
        print InDetDiMuMonManager
















        

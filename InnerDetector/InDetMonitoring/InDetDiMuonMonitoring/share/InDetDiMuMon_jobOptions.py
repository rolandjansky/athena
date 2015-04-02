DiMuMonDoROOTFile = True
doFits = False
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
                              doSaveFits = doSaveFits,
                              OutputLevel = 1)


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
                               doSaveFits = doSaveFits,
                               OutputLevel = 1)

    ToolSvc += ZmumuMon_NoTrig
    if (InDetFlags.doPrintConfigurables()):
        print ZmumuMon_NoTrig
    InDetDiMuMonManager.AthenaMonTools += [ ZmumuMon_NoTrig ]


if not hasattr(ToolSvc, 'monTrigDecTool'):
    print "DiMuMon_jobOptions.py: trigger decision tool not found: don't run trigger-aware monitoring"
else:
    if doJpsi:
        JpsiMon_XpressTrig = DiMuMon (name = "JpsiMon_XpressTrig",
                                      resonName = "Jpsi",
                                      minInvmass = 2.5,
                                      maxInvmass = 3.5,
                                      nMassBins = 50,
                                      triggerChainName = "XpressTrig",
                                      regions = regionsJpsi,
                                      varsVSmean = varsVSmeanJpsi,
                                      varsVSwidth = varsVSwidthJpsi,
                                      varsDistr = varsDistrJpsi,
                                      doFits = doFits,
                                      doSaveFits = doSaveFits)
        JpsiMon_XpressTrig.TriggerChain = "EF_2mu4T_Jpsimumu_IDTrkNoCut"
        JpsiMon_XpressTrig.TrigDecisionTool = monTrigDecTool
        ToolSvc += JpsiMon_XpressTrig
        if (InDetFlags.doPrintConfigurables()):
            print JpsiMon_XpressTrig
        InDetDiMuMonManager.AthenaMonTools += [ JpsiMon_XpressTrig ]

    if doZmumu:
        ZmumuMon_XpressTrig = DiMuMon (name = "ZmumuMon_XpressTrig",
                                       resonName = "Zmumu",
                                       minInvmass = 60.,
                                       maxInvmass = 120.,
                                       nMassBins = 60,
                                       triggerChainName = "XpressTrig",
                                       regions = regionsZmumu,
                                       varsVSmean = varsVSmeanZmumu,
                                       varsVSwidth = varsVSwidthZmumu,
                                       varsDistr = varsDistrZmumu,
                                       doFits = doFits,
                                       doSaveFits = doSaveFits)
        ZmumuMon_XpressTrig.TriggerChain = "EF_2mu13_Zmumu_IDTrkNoCut"
        ZmumuMon_XpressTrig.TrigDecisionTool = monTrigDecTool
        ToolSvc += ZmumuMon_XpressTrig
        if (InDetFlags.doPrintConfigurables()):
            print ZmumuMon_XpressTrig
        InDetDiMuMonManager.AthenaMonTools += [ ZmumuMon_XpressTrig ]

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

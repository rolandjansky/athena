
#from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
#athenaCommonFlags.RuntimeStrictness.set_Value_and_Lock('none')

if (not("WhichInput" in dir())):
  WhichInput = "RDO"
  
if (not("RunOffline" in dir())):
  RunOffline = True

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
    
if (WhichInput == "ESD") :
    from RecExConfig.RecFlags import rec
    rec.doCBNT=False
    
    rec.doTruth = False
    rec.doTrigger = False
    rec.doESD = False
    rec.doWriteESD = False
    rec.doAOD = False
    rec.doWriteAOD = False
    rec.doWriteTAG = False
    rec.doHist = False
    
    rec.readESD = True
    rec.readRDO = False
    rec.readAOD = False

    from RecExConfig.RecAlgsFlags import recAlgs
    recAlgs.doMissingET = False
    recAlgs.doCaloTrkMuId = False
    recAlgs.doMissingETSig = False
    recAlgs.doMuGirl = False
    recAlgs.doMuTag = False
    recAlgs.doMuidLowPt = False
    recAlgs.doMuonIDCombined = False
    recAlgs.doMuonIDStandAlone = False
    recAlgs.doMuonSpShower = False
    recAlgs.doObjMissingET = False
    recAlgs.doTileMuID = False

    include ("RecExCommon/RecExCommon_flags.py")

    TriggerFlags.doCalo = True
    TriggerFlags.doID = True
    TriggerFlags.doEF = True
    TriggerFlags.doLVL2 = True
    #TriggerFlags.enableMonitoring = [ ]
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.EgammaSlice.setAll()
#    TriggerFlags.triggerMenuSetup = "default"
    
    include ("RecExCommon/RecExCommon_topOptions.py")

    # Our bytestreams don't have MBTS data! Disable it!
    #from TrigT1MBTS.TrigT1MBTSConf import LVL1__TrigT1MBTS
    #LVL1__TrigT1MBTS().Enable = False

elif (WhichInput == "RDO"):
    doTrigger = True
    
    from RecExConfig.RecFlags import rec
    rec.doCBNT=False
    
    rec.doTruth=True
    
    if (RunOffline == True):
      rec.doESD = True
    else:
      rec.doESD = False
    rec.doWriteESD = False
    rec.doAOD = False
    rec.doWriteAOD = False
    rec.doWriteTAG = False
    rec.doHist = True
    rec.doTrigger = True
    
    rec.readESD = False
    rec.readRDO = True
    rec.readAOD = False
    
    from RecExConfig.RecAlgsFlags import recAlgs
    recAlgs.doMissingET = False
    recAlgs.doCaloTrkMuId = False
    recAlgs.doMissingETSig = False
    recAlgs.doMuGirl = False
    recAlgs.doMuTag = False
    recAlgs.doMuidLowPt = False
    recAlgs.doMuonIDCombined = False
    recAlgs.doMuonIDStandAlone = False
    recAlgs.doMuonSpShower = False
    recAlgs.doObjMissingET = False
    recAlgs.doTileMuID = False
    recAlgs.doTrigger = True
    
    include ("RecExCommon/RecExCommon_flags.py")

    TriggerFlags.doCalo = True
    TriggerFlags.doID = True
    TriggerFlags.doEF = True
    TriggerFlags.doLVL2 = True
    #TriggerFlags.enableMonitoring = [ ]
    #TriggerFlags.triggerMenuSetup = "MC_lumi1E31_no_prescale"
    TriggerFlags.triggerMenuSetup = 'default'

    def electrons():
        TriggerFlags.Slices_all_setOff()
        TriggerFlags.EgammaSlice.setAll()
        TriggerFlags.Lvl1.items = TriggerFlags.Lvl1.items() + [ 'L1_2EM14I', ]
        TriggerFlags.EgammaSlice.signatures =  ['eNoCut', 'e5_NoCut', 'e5_NoCut_cells', 'e5_NoCut_L2SW', 'e5_NoCut_FwdBackTrk', 'e5_NoCut_SiTrk', 'e5_NoCut_TRT', 
                                            'e5_NoCut_IdScan', 'em15_passHLT', 'em15i_passHLT', '2em15_passHLT', 'em20_passHLT', 'em20i_passHLT', '2em20_passHLT', 'em105_passHLT', '2e5_medium', 
                                            '2e5_medium1', '2e6_medium', '2e6_medium1', 'e5_medium_e10_loose', 'e5_medium', 'e5_medium1', 'e6_medium', 'e6_medium1', 'e7_medium', 
                                            'e10_loose', 'e10_loose_SiTrk', 'e10_loose_TRT', 'e10_loose_IdScan', 'e10_loose_FwdBackTrk', 'e10_loose_passL2', 'e10_loose_passEF', 
                                            'e10_medium', '2e10_loose', 'e12_medium', 'e15_medium', 'e15_loose', 'e15_medium_passL2', 'e15_medium_passEF', 'e15_loose_passL2', 
                                            'e15_loose_passEF', 'e15i_medium', 'e15i_loose', 'e15i_medium_passL2', 'e15i_medium_passEF', '2e15_medium', '3e15_medium', '2e15i_medium', 
                                            '2e15i_medium_passL2', '2e15i_medium_passEF', 'e17i_medium', 'e20_loose', 'e20_loose_passL2', 'e20_loose_passEF', 'e20_medium', 'e20_medium_passL2', 
                                            'e20_medium_passEF', '2e20_loose', '2e20_loose_passL2', '2e20_loose_passEF', '2e20i_loose', '2e20i_loose_passL2', '2e20i_loose_passEF', 'e20i_loose', 
                                            'e20i_loose_passL2', 'e20i_loose_passEF', 'e20i_medium', 'e20i_medium_passL2', 'e20i_medium_passEF', 'e25_loose', 'e25_medium', 'e25i_loose', 
                                            'e25i_loose_passL2', 'e25i_loose_passEF', 'e25i_medium', 'e25i_medium_passL2', 'e25i_medium_passEF', '2e25i_loose', '2e25i_loose_passL2', 
                                            '2e25i_loose_passEF', 'e40_loose1', 'e105_loose1', 'e140_loose1', '2e12_tight', 'e22i_tight', 'e55_loose1', 
 'Zee', 'Jpsiee', 'e10_medium_Ringer', 'e5_medium_Ringer', 'e5_medium_L2SW', 'e10_medium_L2SW', 'e20_loose_L2SW',
                                            'e5_EFfullcalo', 'e10_medium_SiTrk_robust', 'e10_medium_SiTrk', 
                                            'e20_loose_SiTrk_robust', 'e20_loose_SiTrk', 'e20_loose_IdScan']

    from TriggerMenuPython.GenerateMenu import GenerateMenu
    GenerateMenu.overwriteSignaturesWith(electrons)

    include ("RecExCommon/RecExCommon_topOptions.py")

    # Our bytestreams don't have MBTS data! Disable it!
    #from TrigT1MBTS.TrigT1MBTSConf import LVL1__TrigT1MBTS
    #LVL1__TrigT1MBTS().Enable = False

    from AnalysisTriggerAlgs.AnalysisTriggerAlgsConfig import RoIBResultToAOD
    theRoIBResultToAOD = RoIBResultToAOD()
    theRoIBResultToAOD.Enable=True
    theRoIBResultToAOD.DoMuon=False
    theRoIBResultToAOD.DoCalo=True
    topSequence += theRoIBResultToAOD
    
elif (WhichInput == "BS"):
    doESD = True
    doWriteESD = False
    doAOD = False
    doWriteAOD = False
    doWriteTAG = False

    readESD = False
    readRDO = True
    readAOD = False

    from AthenaCommon.GlobalFlags import GlobalFlags
    GlobalFlags.InputFormat.set_bytestream()

    from AthenaCommon.GlobalFlags import globalflags
    globalflags.ConditionsTag = ConditionsTag

    include ("RecExCommon/RecExCommon_flags.py")

    from TriggerJobOpts.TriggerFlags import TriggerFlags
    TriggerFlags.configurationSourceList = ['xml']
    TriggerFlags.configurationSourceList.lock()
    TriggerFlags.doLVL2 = True
    TriggerFlags.doEF = True
    TriggerFlags.doFEX = True
    TriggerFlags.doHypo = True

    #TriggerFlags.triggerMenuSetup = "MC_lumi1E31_no_prescale"
    TriggerFlags.triggerMenuSetup = 'default'


    def electrons():
        TriggerFlags.Slices_all_setOff()
        TriggerFlags.EgammaSlice.setAll()
        TriggerFlags.Lvl1.items = TriggerFlags.Lvl1.items() + [ 'L1_2EM14I', ]
        TriggerFlags.EgammaSlice.signatures =  ['eNoCut', 'e5_NoCut', 'e5_NoCut_cells', 'e5_NoCut_L2SW', 'e5_NoCut_FwdBackTrk', 'e5_NoCut_SiTrk', 'e5_NoCut_TRT', 
                                            'e5_NoCut_IdScan', 'em15_passHLT', 'em15i_passHLT', '2em15_passHLT', 'em20_passHLT', 'em20i_passHLT', '2em20_passHLT', 'em105_passHLT', '2e5_medium', 
                                            '2e5_medium1', '2e6_medium', '2e6_medium1', 'e5_medium_e10_loose', 'e5_medium', 'e5_medium1', 'e6_medium', 'e6_medium1', 'e7_medium', 
                                            'e10_loose', 'e10_loose_SiTrk', 'e10_loose_TRT', 'e10_loose_IdScan', 'e10_loose_FwdBackTrk', 'e10_loose_passL2', 'e10_loose_passEF', 
                                            'e10_medium', '2e10_loose', 'e12_medium', 'e15_medium', 'e15_loose', 'e15_medium_passL2', 'e15_medium_passEF', 'e15_loose_passL2', 
                                            'e15_loose_passEF', 'e15i_medium', 'e15i_loose', 'e15i_medium_passL2', 'e15i_medium_passEF', '2e15_medium', '3e15_medium', '2e15i_medium', 
                                            '2e15i_medium_passL2', '2e15i_medium_passEF', 'e17i_medium', 'e20_loose', 'e20_loose_passL2', 'e20_loose_passEF', 'e20_medium', 'e20_medium_passL2', 
                                            'e20_medium_passEF', '2e20_loose', '2e20_loose_passL2', '2e20_loose_passEF', '2e20i_loose', '2e20i_loose_passL2', '2e20i_loose_passEF', 'e20i_loose', 
                                            'e20i_loose_passL2', 'e20i_loose_passEF', 'e20i_medium', 'e20i_medium_passL2', 'e20i_medium_passEF', 'e25_loose', 'e25_medium', 'e25i_loose', 
                                            'e25i_loose_passL2', 'e25i_loose_passEF', 'e25i_medium', 'e25i_medium_passL2', 'e25i_medium_passEF', '2e25i_loose', '2e25i_loose_passL2', 
                                            '2e25i_loose_passEF', 'e40_loose1', 'e105_loose1', 'e140_loose1', '2e12_tight', 'e22i_tight', 'e55_loose1',  'Zee', 'Jpsiee', 'e10_medium_Ringer', 'e5_medium_Ringer', 'e5_medium_L2SW', 'e10_medium_L2SW', 'e20_loose_L2SW',
                                            'e5_EFfullcalo', 'e10_medium_SiTrk_robust', 'e10_medium_SiTrk', 
                                            'e20_loose_SiTrk_robust', 'e20_loose_SiTrk', 'e20_loose_IdScan']

    from TriggerMenuPython.GenerateMenu import GenerateMenu
    GenerateMenu.overwriteSignaturesWith(electrons)

    include ("RecExCommon/RecExCommon_topOptions.py")

    from TrigBSExtraction.TrigBSExtractionConf import TrigBSExtraction
    extr = TrigBSExtraction()
    extr.Enable = False

    from AnalysisTriggerAlgs.AnalysisTriggerAlgsConfig import RoIBResultToAOD
    theRoIBResultToAOD = RoIBResultToAOD()
    theRoIBResultToAOD.Enable=True
    theRoIBResultToAOD.DoMuon=False
    theRoIBResultToAOD.DoCalo=True
    topSequence += theRoIBResultToAOD

    ByteStreamInputSvc = ServiceMgr.ByteStreamInputSvc
    
    #if hasattr(ToolSvc,"PixelFillCablingData_Final"):
    #  ToolSvc.PixelFillCablingData_Final.MappingFile='PixelCabling/Pixels_Atlas_IdMapping.dat'

    #if hasattr(ToolSvc,"TRT_FillCablingData_DC3"):
    #  ToolSvc.TRT_FillCablingData_DC3.RealData=False

    #if hasattr(ToolSvc,"SCT_CablingSelector"):
    #  ToolSvc.SCT_CablingSelector.Filename = "SCT_Jan08Cabling.dat" 

jobproperties.PerfMonFlags.doMonitoring = True

print ">>>>>>>>== generalJobOption.py DEBUG ==<<<<<<<<<"
print "Algorithms on Top:"
print theApp.TopAlg
print ">>>>>>>>== generalJobOption.py DEBUG ==<<<<<<<<<"


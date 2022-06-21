#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
def BSMonitoringConfig(inputFlags):
    '''Function to configure LVL1 BSMonitoring algorithm in the monitoring system.'''

    # local printing
    import logging
    local_logger = logging.getLogger('AthenaMonitoringCfg')
    info = local_logger.info
    #import math #only needed for phi definition  
    # get the component factory - used for getting the algorithms
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # make the athena monitoring helper
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'BSMonitoringCfg')

    # get any algorithms
    BSMonAlg = helper.addAlgorithm(CompFactory.TrigT1CTMonitoring.BSMonitoringAlgorithm,'BSMonAlg')


    # default values:
    ProcessRoIBResult = True
    InclusiveTriggerThresholds = True
    ProcessMuctpiData = True
    ProcessMuctpiDataRIO = True
    RunOnESD = False
    CompareRerun = False
    ProcessCTPData = True
    isSimulation = inputFlags.Input.isMC

    info('In BSMonitoringConfig SIM or not?: %s', isSimulation)
    #-----------ONLINE CODE---------------------
    if inputFlags.Common.isOnline:
        #info('In BSMonitoringConfig: isOnline')
        ProcessRoIBResult = True
        InclusiveTriggerThresholds = True
        ProcessMuctpiData = False #True
        ProcessMuctpiDataRIO = False #True
        RunOnESD = False
        CompareRerun = False
    #-----------OFFLINE CODE---------------------
    else:
        #info('In BSMonitoringConfig: NOT isOnline')
        ## add pre algorithms for rerunning CTP simulation  
        #if 'IS_SIMULATION' not in metadata['eventTypes']:
            #info('In BSMonitoringConfig: eventTypes: %s ', metadata['eventTypes'] )
            # Wrap everything in a sequence which will force algs to execute in order, even in MT mode
            #from AthenaCommon.AlgSequence import AthSequencer
            #CTPMonSeq=AthSequencer('CTPMonSeq')
            #from AthenaConfiguration.AllConfigFlags import ConfigFlags
            #if 'IS_SIMULATION' not in metadata['eventTypes']:
            
            #none of these 2 work - pretty unsatisfying! - to be fixed asap!!!!! 
            #if inputFlags.DQ.Environment not in ('online', 'tier0', 'tier0Raw'): #, 'tier0ESD'):
                ## Wrap everything in a sequence which will force algs to execute in order, even in MT mode
                #from AthenaCommon.AlgSequence import AthSequencer
                #CTPMonSeq=AthSequencer('CTPMonSeq')

                ##only used for compareRerun()
                ##currently not yet working due to CTPVersion not set
                #from TrigT1CTP.TrigT1CTPConfig import CTPSimulationOnData
                #CTPMonSeq += CTPSimulationOnData("CTPSimulation")
                
                #from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Config import L1MuctpiPhase1_on_Data #MUCTPI_AthAlgCfg #L1MuctpiPhase1 #L1MuctpiPhase1_on_Data
                #CTPMonSeq += L1MuctpiPhase1_on_Data("MUCTPI_AthTool") # MUCTPI_AthAlgCfg(ConfigFlags) #L1MuctpiPhase1() #L1MuctpiPhase1_on_Data()
                
        # check if global muons are on 
        if not inputFlags.Reco.EnableCombinedMuon:
            if isSimulation:
                info('In BSMonitoringConfig: rec.doMuon=True & SIM')
                ProcessRoIBResult = False
                InclusiveTriggerThresholds = False
                ProcessMuctpiData = False
                ProcessMuctpiDataRIO = False
                CompareRerun = False #True
            else:
                info('In BSMonitoringConfig: rec.doMuon=True & DATA')
                ProcessRoIBResult = False
                InclusiveTriggerThresholds = False
                ProcessMuctpiData = False
                ProcessMuctpiDataRIO = False
                RunOnESD = False
                CompareRerun = False
        else:
            if isSimulation:
                info('In BSMonitoringConfig: rec.doMuon=False & SIM')
                ProcessRoIBResult = True
                ProcessMuctpiData = False #True
                ProcessMuctpiDataRIO = False #True
                RunOnESD = True
                CompareRerun = False #True
            else:
                info('In BSMonitoringConfig: rec.doMuon=False & DATA')
                ProcessRoIBResult = True
                ProcessMuctpiData = False #True
                ProcessMuctpiDataRIO = False
                RunOnESD = False #True
                CompareRerun = False

    BSMonAlg.isSimulation = isSimulation
    BSMonAlg.ProcessRoIBResult = ProcessRoIBResult
    BSMonAlg.InclusiveTriggerThresholds = InclusiveTriggerThresholds
    BSMonAlg.ProcessMuctpiData = ProcessMuctpiData
    BSMonAlg.ProcessMuctpiDataRIO = ProcessMuctpiDataRIO
    BSMonAlg.RunOnESD = RunOnESD
    BSMonAlg.CompareRerun = CompareRerun
    BSMonAlg.ProcessCTPData = ProcessCTPData

    DefaultBcIntervalInNs = 24.9507401
    BSMonAlg.DefaultBcIntervalInNs = DefaultBcIntervalInNs
    BCsPerTurn = 3564
    BSMonAlg.BCsPerTurn = BCsPerTurn
    LumiBlockTimeCoolFolderName = '/TRIGGER/LUMI/LBLB"'
    BSMonAlg.LumiBlockTimeCoolFolderName = LumiBlockTimeCoolFolderName
    FillStateCoolFolderName = '/LHC/DCS/FILLSTATE'
    BSMonAlg.FillStateCoolFolderName = FillStateCoolFolderName
    DataTakingModeCoolFolderName = '/TDAQ/RunCtrl/DataTakingMode'
    BSMonAlg.DataTakingModeCoolFolderName = DataTakingModeCoolFolderName
    IgnorePatterns = ["L1_TRT", "L1_ZB", "_AFP", "L1_BPTX", "L1_BCM", "L1_LUCID"]
    BSMonAlg.IgnorePatterns = IgnorePatterns 

    # add monitoring algorithm to group, with group name and main directory 
    groupName = 'CTPMonitor' # the monitoring group name is also used for the package name
    BSMonAlg.PackageName = groupName
    mainDir = 'CT'
    myGroup = helper.addGroup(BSMonAlg, groupName , mainDir)

    errorSummaryBinLabels = [
        "CTP/ROD BCID Offset",
        "No BCs in Readout",
        "CTP/MuCTPI BCID Offset",
        "Wrong Cand Word Number",
        "Wrong Pt in Mult Word",
        "Wrong Num of RoI",
        "No Cand for RoI",
        "Cand BC Out of Range",
        "Invalid Lumi Block",
        "LB Out of Time",
        "Nanosec > 1e9",
        "TAP w/out TBP",
        "TAV w/out TAP",
        "CTP sim. mismatch",
        "Incomplete fragment",
        "Missing orbit pulse",
        "MuCTPI/noRPC candidate mismatch",
        "MuCTPI/noTGC candidate mismatch",
        "RPC/noMuCTPI candidate mismatch",
        "GC/noMuCTPI candidate mismatch"
    ]
    incompleteFragmentTypeBinLabels = [
        "CTP RIO",
        "CTP RDO",
        #"MuCTPI RIO",
        "MuCTPI RDO",
        "RoIBResult",
        "TGC SL RDO",
        "RPC SL RDO"
    ]

    # ERRORS
    # TProfile or TEfficiency? I guess both work as we only casre about he value not correct errors
    # CHECK( registerTProfile("errorSummary", "CTP and MuCTPI errors; ; Error rate", 20, 0.5, 20.5, -1, 2) );
    myGroup.defineHistogram('errorSummaryX,errorSummaryY;errorSummary',title='CTP and MuCTPI errors; ; Error ratio',
                            type='TProfile', path="", xbins=20, xmin=0.5, xmax=20.5, ymin=-1., ymax=2., xlabels=errorSummaryBinLabels, opt='kAlwaysCreate')
    # 2D
    # CHECK( registerTH2("errorSummaryPerLumiBlock", "Errors per lumi block; LB number; Errors", 2000, 0.5, 2000.5, 20, 0.5, 20.5) );
    myGroup.defineHistogram('errorSummaryPerLumiBlockX,errorSummaryPerLumiBlockY;errorSummaryPerLumiBlock',title='Errors per lumi block; LB number; Errors',
                            type='TH2F',path='',xbins=2000,xmin=0.5,xmax=2000.5,ybins=20,ymin=0.5,ymax=20.5,ylabels=errorSummaryBinLabels, opt='kAlwaysCreate')
    # 1d 
    # CHECK( registerTH1("errorPerLumiBlock", "Number of errors per lumi block; LB number; Errors", 2001, -0.5, 2000.5) );
    myGroup.defineHistogram('errorPerLumiBlockX;errorPerLumiBlock', title='Number of errors per lumi block; LB number; Errors',
                            path='',xbins=2001,xmin=-0.5,xmax=2000.5,opt='kAlwaysCreate') 
    # CHECK( registerTH1("incompleteFragmentType", "Number of missing fragments per type; Fragment type; Number of incomplete fragments", 8, -0.5, 7.5) );
    myGroup.defineHistogram('incompleteFragmentTypeX;incompleteFragmentType', title='Number of missing fragments per type; Fragment type; Number of incomplete fragments ',
                            path='',xbins=7,xmin=-0.5,xmax=7.5, xlabels=incompleteFragmentTypeBinLabels, opt='kAlwaysCreate')

    monCTPPath="/CTP/"
    # CHECK( registerTH1("deltaBcid", "CTP Data BCID - ROD Header BCID; #DeltaBCID; Entries", 401, -200.5, 200.5) );
    myGroup.defineHistogram('deltaBcidX;deltaBcid', title='CTP Data BCID - ROD Header BCID; #DeltaBCID; Entries',
                            path=monCTPPath,xbins=401,xmin=-200.5,xmax=200.5,opt='kAlwaysCreate')
    # CHECK( registerTH1("triggerType", "Trigger Type; Trigger Type; Entries", 256, -0.5, 255.5) );
    myGroup.defineHistogram('triggerTypeX;triggerType', title='Trigger Type; Trigger Type; Entries',
                            path=monCTPPath,xbins=256,xmin=-0.5,xmax=255.5,opt='kAlwaysCreate')
    # CHECK( registerTH1("timeSinceLBStart", "Time Since LB Start; Time After New LB (ms); Entries", 1000, -500, 1500) );
    myGroup.defineHistogram('timeSinceLBStartX;timeSinceLBStart', title='Time Since LB Start; Time After New LB (ms); Entries',
                            path=monCTPPath,xbins=1000,xmin=-500,xmax=1500,opt='kAlwaysCreate')
    # CHECK( registerTH1("timeUntilLBEnd", "Time Until LB End; Time Until Next LB (ms); Entries", 1000, -500, 1500) );
    myGroup.defineHistogram('timeUntilLBEndX;timeUntilLBEnd', title='Time Until LB End; Time Until Next LB (ms); Entries',
                            path=monCTPPath,xbins=1000,xmin=-500,xmax=1500,opt='kAlwaysCreate')
    # CHECK( registerTH1("timeSinceL1A", "Time since last L1A; Time since last L1A (ms); Entries", 2000, -1, 30) );
    myGroup.defineHistogram('timeSinceL1AX;timeSinceL1A', title='Time since last L1A; Time since last L1A (ms); Entries',
                            path=monCTPPath,xbins=2000,xmin=-1,xmax=30,opt='kAlwaysCreate')
    # CHECK( registerTH1("turnCounterTimeError", "Error of time based on turn counter and BCID; t_{TC+BCID}-t_{GPS} [#mus]; Entries", 2000, -1000., 1000.) );
    myGroup.defineHistogram('turnCounterTimeErrorX;turnCounterTimeError', title='Error of time based on turn counter and BCID; t_{TC+BCID}-t_{GPS} [#mus]; Entries',
                            path=monCTPPath,xbins=2000,xmin=-1000.,xmax=1000.,opt='kAlwaysCreate')
    #patrick check how this one now looks and used to look! - compare to  error Profile!
    # CHECK( registerTProfile("turnCounterTimeErrorVsLb", "Error of (TC+BCID)-based time vs. LB; LB; t_{TC+BCID}-t_{GPS} [#mus]", 2001, -0.5, 2000.5, -1000., 1000.) );
    myGroup.defineHistogram('turnCounterTimeErrorVsLbX,turnCounterTimeErrorVsLbY,turnCounterTimeErrorVsLbZ;turnCounterTimeErrorVsLb',
                            title='Error of (TC+BCID)-based time vs. LB; LB; t_{TC+BCID}-t_{GPS} [#mus]',
                            type='TProfile2D', path=monCTPPath, xbins=2001, ybins=2001, xmin=-0.5, xmax=2000.5, ymin=-1000., ymax=1000, opt='kAlwaysCreate')
    # CHECK( registerTH2("pitBC", "CTP BC vs. PIT; PIT; BC", 320, -0.5, 319.5, 127, -63.5, 63.5) );
    myGroup.defineHistogram('pitBCX,pitBCY;pitBC',title='CTP BC vs. PIT; PIT; BC',
                            type='TH2F',path=monCTPPath, xbins=320,xmin=-0.5,xmax=319.5,ybins=127,ymin=-63.5,ymax=63.5,opt='kAlwaysCreate')
    # CHECK( registerTH2("pitFirstBC", "First CTP BC vs. PIT; PIT; BC", 320, -0.5, 319.5, 127, -63.5, 63.5) );
    myGroup.defineHistogram('pitFirstBCX,pitFirstBCY;pitFirstBC',title='First CTP BC vs. PIT; PIT; BC',
                            type='TH2F',path=monCTPPath, xbins=320,xmin=-0.5,xmax=319.5,ybins=127,ymin=-63.5,ymax=63.5,opt='kAlwaysCreate')
    # CHECK( registerTH1("tav", "Trigger Items After Veto; CTP TAV; Entries", 512, -0.5, 511.5) );
    myGroup.defineHistogram('tavX;tav', title='Trigger Items After Veto; CTP TAV; Entries',
                            path=monCTPPath,xbins=512,xmin=-0.5,xmax=511.5,opt='kAlwaysCreate')
    # CHECK( registerTH1("ctpStatus1", "CTP Status Word 1; Bit; Number of times ON", 24, -0.5, 23.5) );
    myGroup.defineHistogram('ctpStatus1X;ctpStatus1', title='CTP Status Word 1; Bit; Number of times ON',
                            path=monCTPPath,xbins=24,xmin=-0.5,xmax=23.5,opt='kAlwaysCreate')
    # CHECK( registerTH1("ctpStatus2", "CTP Status Word 2; Bit; Number of times ON", 24, -0.5, 23.5) );
    myGroup.defineHistogram('ctpStatus2X;ctpStatus2', title='CTP Status Word 2; Bit; Number of times ON',
                            path=monCTPPath,xbins=24,xmin=-0.5,xmax=23.5,opt='kAlwaysCreate')
    # CHECK( registerTH1("l1ItemsBPSimMismatch","Sim mismatch L1 Items before prescale", 512, 0, 512) );
    myGroup.defineHistogram('l1ItemsBPSimMismatchX;l1ItemsBPSimMismatch', title='Sim mismatch L1 Items before prescale',
                            path=monCTPPath,xbins=512,xmin=0,xmax=512,opt='kAlwaysCreate')
    # CHECK( registerTH1("l1ItemsBPSimMismatchItems","Sim mismatch L1 Items before prescale, mismatched ones only", 512, 0, 512) );
    myGroup.defineHistogram('l1ItemsBPSimMismatchItemsX;l1ItemsBPSimMismatchItems', title='Sim mismatch L1 Items before prescale, mismatched ones only',
                            path=monCTPPath,xbins=512,xmin=0,xmax=512,opt='kAlwaysCreate')

    monMUCTPIPath="/MUCTPI/"
    #  CHECK( registerTH1("nCandidates", "Number of MuCTPI candidates from mult word; Multiplicity; Entries", 11, -0.5, 10.5) );
    myGroup.defineHistogram('nCandidatesX;nCandidates', title='Number of MuCTPI candidates from mult word; Multiplicity; Entries',
                            path=monMUCTPIPath,xbins=11,xmin=-0.5,xmax=10.5,opt='kAlwaysCreate')
    #  CHECK( registerTH1("nCandidatesDataWord", "Number of MuCTPI candidates from data word; Multiplicity; Entries", 100, -0.5, 99.5) );
    myGroup.defineHistogram('nCandidatesDataWordX;nCandidatesDataWord', title='Number of MuCTPI candidates from data word; Multiplicity; Entries',
                            path=monMUCTPIPath,xbins=100,xmin=-0.5,xmax=99.5,opt='kAlwaysCreate')
    #  CHECK( registerTH1("pt", "Candidate p_{T} distribution from mult word; p_{T} threshold; Entries", 6, 0.5, 6.5) );
    myGroup.defineHistogram('ptX;pt', title='Candidate p_{T} distribution from mult word; p_{T} threshold; Entries',
                            path=monMUCTPIPath,xbins=6,xmin=0.5,xmax=6.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("nCandidatesPt", "Number of MuCTPI candidates from mult word vs p_{T}; p_{T} threshold; MuCTPI candidates", 6, 0.5, 6.5, 8, -0.5, 7.5) );
    myGroup.defineHistogram('nCandidatesPtX,nCandidatesPtY;nCandidatesPt', title='Number of MuCTPI candidates from mult word vs p_{T}; p_{T} threshold; MuCTPI candidates',
                            type='TH2F',path=monMUCTPIPath,xbins=6,xmin=0.5,xmax=6.5,ybins=8,ymin=-0.5,ymax=7.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("nCandidatesDataWordPt", "Number of MuCTPI candidates from data word vs p_{T}; p_{T} threshold; MuCTPI candidates", 6, 0.5, 6.5, 25, -0.5, 99.5) );
    myGroup.defineHistogram('nCandidatesDataWordPtX,nCandidatesDataWordPtY;nCandidatesDataWordPt', title='Number of MuCTPI candidates from data word vs p_{T}; p_{T} threshold; MuCTPI candidates',
                            type='TH2F',path=monMUCTPIPath,xbins=6,xmin=0.5,xmax=6.5,ybins=8,ymin=-0.5,ymax=7.5,opt='kAlwaysCreate')
    #  CHECK( registerTH1("nCandidatesMictpMioct", "Diff between MICTP and MIOCT candidates (same BC); MICTP - MIOCT; Entries", 21, -10.5, 10.5) );
    myGroup.defineHistogram('nCandidatesMictpMioctX;nCandidatesMictpMioct', title='Diff between MICTP and MIOCT candidates (same BC); MICTP - MIOCT; Entries',
                            path=monMUCTPIPath,xbins=21,xmin=-10.5,xmax=12.5,opt='kAlwaysCreate')
    #  CHECK( registerTH1("bcidMictpMioct", "BCID diff between MICTP and all MIOCT candidate words; MIOCT - MICTP; Entries", 8, -3.5, 4.5) );
    myGroup.defineHistogram('bcidMictpMioctX;bcidMictpMioct', title='BCID diff between MICTP and all MIOCT candidate words; MIOCT - MICTP; Entries',
                            path=monMUCTPIPath,xbins=8,xmin=-3.5,xmax=4.5,opt='kAlwaysCreate')
    #  CHECK( registerTH1("bcidMictpHeader", "BCID Diff between MICTP and ROD header; MICTP - ROD header; Entries", 15, -7.5, 7.5) );
    #myGroup.defineHistogram('bcidMictpHeaderX;bcidMictpHeader', title='BCID Diff between MICTP and ROD header; MICTP - ROD header; Entries',
    #                        path=monMUCTPIPath,xbins=15,xmin=-7.5,xmax=7.5,opt='kAlwaysCreate')
    #  CHECK( registerTH1("muctpiStatus1", "MuCTPI Status Word 1; Bit; Number of times ON", 24, -0.5, 23.5) );
    #myGroup.defineHistogram('muctpiStatus1X;muctpiStatus1', title='MuCTPI Status Word 1; Bit; Number of times ON',
    #                        path=monMUCTPIPath,xbins=24,xmin=-0.5,xmax=23.5,opt='kAlwaysCreate')
    #  CHECK( registerTH1("muctpiStatus2", "MuCTPI Status Word 2; Bit; Number of times ON", 24, -0.5, 23.5) );
    #myGroup.defineHistogram('muctpiStatus2X;muctpiStatus2', title='MuCTPI Status Word 2; Bit; Number of times ON',
    #                        path=monMUCTPIPath,xbins=24,xmin=-0.5,xmax=23.5,opt='kAlwaysCreate')

    #  //RoI numbers
    #  CHECK( registerTH2("barrelRoiSectorID", "MIOCT RoI Number vs Barrel Sector ID (Trigger BC); Sector ID; RoI Number", 64, -0.5, 63.5, 32, -0.5, 31.5) );
    myGroup.defineHistogram('barrelRoiSectorIDX,barrelRoiSectorIDY;barrelRoiSectorID', title='MIOCT RoI Number vs Barrel Sector ID (Trigger BC); Sector ID; RoI Number',
                            type='TH2F',path=monMUCTPIPath,xbins=64,xmin=-0.5,xmax=63.5,ybins=32,ymin=-0.5,ymax=31.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("endcapRoiSectorID", "MIOCT RoI Number vs Endcap Sector ID (Trigger BC); Sector ID; RoI Number", 96, -0.5, 95.5, 32, -0.5, 31.5) );
    myGroup.defineHistogram('endcapRoiSectorIDX,endcapRoiSectorIDY;endcapRoiSectorID', title='MIOCT RoI Number vs Endcap Sector ID (Trigger BC); Sector ID; RoI Number',
                            type='TH2F',path=monMUCTPIPath,xbins=96,xmin=-0.5,xmax=95.5,ybins=32,ymin=-0.5,ymax=31.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("forwardRoiSectorID", "MIOCT RoI Number vs Forward Sector ID (Trigger BC); Sector ID; RoI Number", 48, -0.5, 47.5, 32, -0.5, 31.5) );
    myGroup.defineHistogram('forwardRoiSectorIDX,forwardRoiSectorIDY;forwardRoiSectorID', title='MIOCT RoI Number vs Forward Sector ID (Trigger BC); Sector ID; RoI Number',
                            type='TH2F',path=monMUCTPIPath,xbins=48,xmin=-0.5,xmax=47.5,ybins=32,ymin=-0.5,ymax=31.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("barrelRoiSectorIDAll", "MIOCT RoI Number vs Barrel Sector ID (All BC); Sector ID; RoI Number", 64, -0.5, 63.5, 32, -0.5, 31.5) );
    myGroup.defineHistogram('barrelRoiSectorIDAllX,barrelRoiSectorIDAllY;barrelRoiSectorIDAll', title='MIOCT RoI Number vs Barrel Sector ID (All BC); Sector ID; RoI Number',
                            type='TH2F',path=monMUCTPIPath,xbins=64,xmin=-0.5,xmax=63.5,ybins=32,ymin=-0.5,ymax=31.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("endcapRoiSectorIDAll", "MIOCT RoI Number vs Endcap Sector ID (All BC); Sector ID; RoI Number", 96, -0.5, 95.5, 32, -0.5, 31.5) );
    myGroup.defineHistogram('endcapRoiSectorIDAllX,endcapRoiSectorIDAllY;endcapRoiSectorIDAll', title='MIOCT RoI Number vs Endcap Sector ID (All BC); Sector ID; RoI Number',
                            type='TH2F',path=monMUCTPIPath,xbins=96,xmin=-0.5,xmax=95.5,ybins=32,ymin=-0.5,ymax=31.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("forwardRoiSectorIDAll", "MIOCT RoI Number vs Forward Sector ID (All BC); Sector ID; RoI Number", 48, -0.5, 47.5, 32, -0.5, 31.5) );
    myGroup.defineHistogram('forwardRoiSectorIDAllX,forwardRoiSectorIDAllY;forwardRoiSectorIDAll', title='MIOCT RoI Number vs Forward Sector ID (All BC); Sector ID; RoI Number',
                            type='TH2F',path=monMUCTPIPath,xbins=48,xmin=-0.5,xmax=47.5,ybins=32,ymin=-0.5,ymax=31.5,opt='kAlwaysCreate')

    #  //Candidate multiplicity within sectors
    #  CHECK( registerTH2("barrelNCandSectorID", "MIOCT Candidate mult vs Sector ID (barrel); Sector ID; Candidates", 64, -0.5, 63.5, 8, -0.5, 7.5) );
    myGroup.defineHistogram('barrelNCandSectorIDX,barrelNCandSectorIDY;barrelNCandSectorID', title='MIOCT Candidate mult vs Sector ID (barrel); Sector ID; Candidates',
                            type='TH2F',path=monMUCTPIPath,xbins=64,xmin=-0.5,xmax=63.5,ybins=8,ymin=-0.5,ymax=7.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("endcapNCandSectorID", "MIOCT Candidate mult vs Sector ID (endcap); Sector ID; Candidates", 96, -0.5, 95.5, 8, -0.5, 7.5) );
    myGroup.defineHistogram('endcapNCandSectorIDX,endcapNCandSectorIDY;endcapNCandSectorID', title='MIOCT Candidate mult vs Sector ID (endcap); Sector ID; Candidates',
                            type='TH2F',path=monMUCTPIPath,xbins=96,xmin=-0.5,xmax=95.5,ybins=8,ymin=-0.5,ymax=7.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("forwardNCandSectorID", "MIOCT Candidate mult vs Sector ID (forward); Sector ID; Candidates", 48, -0.5, 47.5, 8, -0.5, 7.5) );
    myGroup.defineHistogram('forwardNCandSectorIDX,forwardNCandSectorIDY;forwardNCandSectorID', title='MIOCT Candidate mult vs Sector ID (forward); Sector ID; Candidates',
                            type='TH2F',path=monMUCTPIPath,xbins=48,xmin=-0.5,xmax=47.5,ybins=8,ymin=-0.5,ymax=7.5,opt='kAlwaysCreate')

    #  //Overlap regions
    #  CHECK( registerTH2("barrelSectorIDOverlapBits", "Barrel Sector ID vs overlap bits; Sector ID; Overlap bits", 64, -0.5, 63.5, 3, 0.5, 3.5) );
    myGroup.defineHistogram('barrelSectorIDOverlapBitsX,barrelSectorIDOverlapBitsY;barrelSectorIDOverlapBits', title='Barrel Sector ID vs overlap bits; Sector ID; Overlap bits',
                            type='TH2F',path=monMUCTPIPath,xbins=64,xmin=-0.5,xmax=63.5,ybins=3,ymin=0.5,ymax=3.5,opt='kAlwaysCreate')
    #  CHECK( registerTH1("endcapSectorIDOverlapBit", "Endcap Sector ID vs number of candidates with overlap; Sector ID; Candidates", 48, -0.5, 47.5) );
    myGroup.defineHistogram('endcapSectorIDOverlapBitX;endcapSectorIDOverlapBit', title='Endcap Sector ID vs number of candidates with overlap; Sector ID; Candidates',
                            path=monMUCTPIPath,xbins=48,xmin=-0.5,xmax=47.5,opt='kAlwaysCreate')

    #  //Sector occupancy
    #  CHECK( registerTH1("barrelSectorID", "Barrel Sector ID (Trigger BC); Sector ID; Candidates", 64, -0.5, 63.5) );
    myGroup.defineHistogram('barrelSectorIDX;barrelSectorID', title='Barrel Sector ID (Trigger BC); Sector ID; Candidates',
                            path=monMUCTPIPath,xbins=64,xmin=-0.5,xmax=63.5,opt='kAlwaysCreate')
    #  CHECK( registerTH1("endcapSectorID", "Endcap Sector ID (Trigger BC); Sector ID; Candidates", 96, -0.5, 95.5) );
    myGroup.defineHistogram('endcapSectorIDX;endcapSectorID', title='Endcap Sector ID (Trigger BC); Sector ID; Candidates',
                            path=monMUCTPIPath,xbins=96,xmin=-0.5,xmax=95.5,opt='kAlwaysCreate')
    #  CHECK( registerTH1("forwardSectorID", "Forward Sector ID (Trigger BC); Sector ID; Candidates", 48, -0.5, 47.5) );  
    myGroup.defineHistogram('forwardSectorIDX;forwardSectorID', title='Forward Sector ID (Trigger BC); Sector ID; Candidates',
                            path=monMUCTPIPath,xbins=48,xmin=-0.5,xmax=47.5,opt='kAlwaysCreate')
    #  CHECK( registerTH1("barrelSectorIDAll", "Barrel Sector ID (All BC); Sector ID; Candidates", 64, -0.5, 63.5) );
    myGroup.defineHistogram('barrelSectorIDAllX;barrelSectorIDAll', title='Barrel Sector ID (All BC); Sector ID; Candidates',
                            path=monMUCTPIPath,xbins=64,xmin=-0.5,xmax=63.5,opt='kAlwaysCreate')
    #  CHECK( registerTH1("endcapSectorIDAll", "Endcap Sector ID (All BC); Sector ID; Candidates", 96, -0.5, 95.5) );
    myGroup.defineHistogram('endcapSectorIDAllX;endcapSectorIDAll', title='Endcap Sector ID (All BC); Sector ID; Candidates',
                            path=monMUCTPIPath,xbins=96,xmin=-0.5,xmax=95.5,opt='kAlwaysCreate')
    #  CHECK( registerTH1("forwardSectorIDAll", "Forward Sector ID (All BC); Sector ID; Candidates", 48, -0.5, 47.5) );
    myGroup.defineHistogram('forwardSectorIDAllX;forwardSectorIDAll', title='Forward Sector ID (All BC); Sector ID; Candidates',
                            path=monMUCTPIPath,xbins=48,xmin=-0.5,xmax=47.5,opt='kAlwaysCreate')

    #  //Candidate word pT
    #  CHECK( registerTH1("barrelPt", "Barrel candidate p_{T} (all candidates); p_{T} threshold; Candidates", 6, 0.5, 6.5) );
    myGroup.defineHistogram('barrelPtX;barrelPt', title='Barrel candidate p_{T} (all candidates); p_{T} threshold; Candidates',
                            path=monMUCTPIPath,xbins=6,xmin=0.5,xmax=6.5,opt='kAlwaysCreate')
    #  CHECK( registerTH1("endcapPt", "Endcap candidate p_{T} (all candidates); p_{T} threshold; Candidates", 6, 0.5, 6.5) );
    myGroup.defineHistogram('endcapPtX;endcapPt', title='Endcap candidate p_{T} (all candidates); p_{T} threshold; Candidates',
                            path=monMUCTPIPath,xbins=6,xmin=0.5,xmax=6.5,opt='kAlwaysCreate')
    #  CHECK( registerTH1("forwardPt", "Forward candidate p_{T} (all candidates); p_{T} threshold; Candidates", 6, 0.5, 6.5) );
    myGroup.defineHistogram('forwardPtX;forwardPt', title='Forward candidate p_{T} (all candidates); p_{T} threshold; Candidates',
                            path=monMUCTPIPath,xbins=6,xmin=0.5,xmax=6.5,opt='kAlwaysCreate')
    nCandidates_secLocBinabels = [
        "Barrel",
        "Endcap",
        "Forward"
    ]
    #  //Number of candidates in each region
    #  CHECK( registerTH1("nCandidates_secLoc", "All candidates by sector location; Sector location; Candidates", 3, -0.5, 2.5) );
    myGroup.defineHistogram('nCandidates_secLocX;nCandidates_secLoc', title='"All candidates by sector location; Sector location; Candidates',
                            path=monMUCTPIPath,xbins=3,xmin=-0.5,xmax=2.5,xlabels=nCandidates_secLocBinabels,opt='kAlwaysCreate')
    #  
    #  //Plots for comparing MuCTPI output to output of RPC and TGC SL
    #  CHECK( registerTH2("muctpiNoRPCCandfound", "MuCTPI BA candidate with no equivalent candidate in RPC SL r/o, Sector ID vs. RoI number", 30, -0.5, 29.5, 64, -0.5, 63.5) );
    myGroup.defineHistogram('muctpiNoRPCCandfoundX,muctpiNoRPCCandfoundY;muctpiNoRPCCandfound', title='MuCTPI BA candidate with no equivalent candidate in RPC SL r/o, Sector ID vs. RoI number',
                            type='TH2F',path=monMUCTPIPath,xbins=30,xmin=-0.5,xmax=29.5,ybins=64,ymin=-0.5,ymax=63.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("rpcNoMuCTPICandfound", "RPC SL r/o candidate with no equivalent candidate in MuCTPI r/o, Sector ID vs. RoI number", 30, -0.5, 29.5, 64, -0.5, 63.5) );
    myGroup.defineHistogram('rpcNoMuCTPICandfoundX,rpcNoMuCTPICandfoundY;rpcNoMuCTPICandfound', title='RPC SL r/o candidate with no equivalent candidate in MuCTPI r/o, Sector ID vs. RoI number',
                            type='TH2F',path=monMUCTPIPath,xbins=30,xmin=-0.5,xmax=29.5,ybins=64,ymin=-0.5,ymax=63.5,opt='kAlwaysCreate')
    #
    #  CHECK( registerTH2("muctpiNoTGCecCandfound", "MuCTPI EC candidate with no equivalent candidate in TGC SL r/o, Sector ID vs. RoI number", 150, -0.5, 149.5, 100, -0.5, 99.5) );
    myGroup.defineHistogram('muctpiNoTGCecCandfoundX,muctpiNoTGCecCandfoundY;muctpiNoTGCecCandfound', title='MuCTPI EC candidate with no equivalent candidate in TGC SL r/o, Sector ID vs. RoI number',
                            type='TH2F',path=monMUCTPIPath,xbins=150,xmin=-0.5,xmax=149.5,ybins=100,ymin=-0.5,ymax=99.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("tgcecNoMuCTPICandfound", "TGC EC SL r/o candidate with no equivalent candidate in MuCTPI r/o, Sector ID vs. RoI number", 150, -0.5, 149.5, 100, -0.5, 99.5) );
    myGroup.defineHistogram('tgcecNoMuCTPICandfoundX,tgcecNoMuCTPICandfoundY;tgcecNoMuCTPICandfound', title='TGC EC SL r/o candidate with no equivalent candidate in MuCTPI r/o, Sector ID vs. RoI number',
                            type='TH2F',path=monMUCTPIPath,xbins=150,xmin=-0.5,xmax=149.5,ybins=100,ymin=-0.5,ymax=99.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("muctpiNoTGCfwCandfound", "MuCTPI FW candidate with no equivalent candidate in TGC SL r/o, Sector ID vs. RoI number", 64, -0.5, 63.5, 50, -0.5, 49.5) );
    myGroup.defineHistogram('muctpiNoTGCfwCandfoundX,muctpiNoTGCfwCandfoundY;muctpiNoTGCfwCandfound', title='MuCTPI FW candidate with no equivalent candidate in TGC SL r/o, Sector ID vs. RoI number',
                            type='TH2F',path=monMUCTPIPath,xbins=150,xmin=-0.5,xmax=149.5,ybins=100,ymin=-0.5,ymax=99.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("tgcfwNoMuCTPICandfound", "TGC FW SL r/o candidate with no equivalent candidate in MuCTPI r/o, Sector ID vs. RoI number", 64, -0.5, 63.5, 50, -0.5, 49.5) );
    myGroup.defineHistogram('tgcfwNoMuCTPICandfoundX,tgcfwNoMuCTPICandfoundY;tgcfwNoMuCTPICandfound', title='TGC FW SL r/o candidate with no equivalent candidate in MuCTPI r/o, Sector ID vs. RoI number',
                            type='TH2F',path=monMUCTPIPath,xbins=150,xmin=-0.5,xmax=149.5,ybins=100,ymin=-0.5,ymax=99.5,opt='kAlwaysCreate')

    monCTPMUCTPIPath="/CTPMUCTPI/"    
    # CHECK( registerTH1("headerL1IdDifference", "ROD header LVL1 ID CTP - MuCTPI; #DeltaL1ID; Entries", 101, -50.5, 50.5) );
    myGroup.defineHistogram('headerL1IdDifferenceX;headerL1IdDifference', title='ROD header LVL1 ID CTP - MuCTPI; #DeltaL1ID; Entries',
                            path=monCTPMUCTPIPath,xbins=101,xmin=-50.5,xmax=50.5,opt='kAlwaysCreate')
    # CHECK( registerTH1("headerBCIDDifference", "ROD header BCID CTP - MuCTPI; #DeltaBCID; Entries", 401, -200.5, 200.5) );
    myGroup.defineHistogram('headerBCIDDifferenceX;headerBCIDDifference', title='ROD header BCID CTP - MuCTPI; #DeltaBCID; Entries',
                            path=monCTPMUCTPIPath,xbins=401,xmin=-200.5,xmax=200.5,opt='kAlwaysCreate')
    # CHECK( registerTH1("bcidDifference", "BCID CTP - MICTP; #DeltaBCID; Entries", 15, -7.5, 7.5) );
    myGroup.defineHistogram('bcidDifferenceX;bcidDifference', title='BCID CTP - MICTP; #DeltaBCID; Entries',
                            path=monCTPMUCTPIPath,xbins=15,xmin=-7.5,xmax=7.5,opt='kAlwaysCreate')

    monROIPath="/ROI/"
    #  //RoI eta / phi
    #  CHECK( registerTH2("roiEtaPhi","Coordinates of all RoIs; #eta; #phi",32,-2.5,2.5,32,-TMath::Pi(),TMath::Pi()) );
    #myGroup.defineHistogram('roiEtaPhiX,roiEtaPhiY;roiEtaPhi', title='Coordinates of all RoIs; #eta; #phi',
    #                        type='TH2F',path=monROIPath,xbins=32,xmin=-2.5,xmax=2.5,ybins=32,ymin=-math.pi,ymax=math.pi,opt='kAlwaysCreate')
    #  CHECK( registerTH1("roiEta","#eta of all RoI's; #eta; Entries",32,-2.5,2.5) );
    #myGroup.defineHistogram('roiEtaX;roiEta', title="#eta of all RoI's; #eta; Entries",
    #                        path=monROIPath,xbins=32,xmin=-2.5,xmax=2.5,opt='kAlwaysCreate')
    #  CHECK( registerTH1("roiPhi","#phi of all RoI's; #phi; Entries",32,-TMath::Pi(),TMath::Pi()) );
    #myGroup.defineHistogram('roiPhiX;roiPhi', title="#phi of all RoI's; #phi; Entries",
    #                        path=monROIPath,xbins=32,xmin=-math.pi,xmax=math.pi,opt='kAlwaysCreate')
    #
    #  //Sector vs. RoI
    #  CHECK( registerTH2("barrelSectorIDRoi","Sector ID vs. RoI for barrel muons (matched cand only); Sector ID; RoI", 64,-0.5,63.5,32,-0.5,31.5) );
    myGroup.defineHistogram('barrelSectorIDRoiX,barrelSectorIDRoiY;barrelSectorIDRoi', title='Sector ID vs. RoI for barrel muons (matched cand only); Sector ID; RoI',
                            type='TH2F',path=monROIPath,xbins=64,xmin=-0.5,xmax=63.5,ybins=32,ymin=-0.5,ymax=31.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("endcapSectorIDRoi","Sector ID vs. RoI for endcap muons (matched cand only); Sector ID; RoI", 96,-0.5,95.5,32,-0.5,31.5) );
    myGroup.defineHistogram('endcapSectorIDRoiX,endcapSectorIDRoiY;endcapSectorIDRoi', title='Sector ID vs. RoI for endcap muons (matched cand only); Sector ID; RoI',
                            type='TH2F',path=monROIPath,xbins=96,xmin=-0.5,xmax=95.5,ybins=32,ymin=-0.5,ymax=31.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("forwardSectorIDRoi","Sector ID vs. RoI for forward muons (matched cand only); Sector ID; RoI", 48,-0.5,47.5,32,-0.5,31.5) );
    myGroup.defineHistogram('forwardSectorIDRoiX,forwardSectorIDRoiY;forwardSectorIDRoi', title='Sector ID vs. RoI for forward muons (matched cand only); Sector ID; RoI',
                            type='TH2F',path=monROIPath,xbins=48,xmin=-0.5,xmax=47.5,ybins=32,ymin=-0.5,ymax=31.5,opt='kAlwaysCreate')
    #  
    #  //Sector vs. RoI Eta
    #  CHECK( registerTH2("barrelSectorIDRoiEta","Sector ID vs. RoI #eta for barrel muons (matched cand only); Sector ID; #eta", 64,-0.5,63.5,32,-2.5,2.5) );
    #myGroup.defineHistogram('barrelSectorIDRoiEtaX,barrelSectorIDRoiEtaY;barrelSectorIDRoiEta', title='Sector ID vs. RoI #eta for barrel muons (matched cand only); Sector ID; #eta',
    #                        type='TH2F',path=monROIPath,xbins=64,xmin=-0.5,xmax=63.5,ybins=32,ymin=-0.5,ymax=31.5,opt='kAlwaysCreate')
    myGroup.defineHistogram('barrelSectorIDRoiEtaX;barrelSectorIDRoiEta', title='Sector ID for barrel muons (matched cand only); Sector ID',
                            path=monROIPath,xbins=64,xmin=-0.5,xmax=63.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("endcapSectorIDRoiEta","Sector ID vs. RoI #eta for endcap muons (matched cand only); Sector ID; #eta", 96,-0.5,95.5,32,-2.5,2.5) );
    #myGroup.defineHistogram('endcapSectorIDRoiEtaX,endcapSectorIDRoiEtaY;endcapSectorIDRoiEta', title='Sector ID vs. RoI #eta for endcap muons (matched cand only); Sector ID; #eta',
    #                        type='TH2F',path=monROIPath,xbins=96,xmin=-0.5,xmax=95.5,ybins=32,ymin=-0.5,ymax=31.5,opt='kAlwaysCreate')
    myGroup.defineHistogram('endcapSectorIDRoiEtaX;endcapSectorIDRoiEta', title='Sector ID for endcap muons (matched cand only); Sector ID',
                            path=monROIPath,xbins=96,xmin=-0.5,xmax=95.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("forwardSectorIDRoiEta","Sector ID vs. RoI #eta for forward muons (matched cand only); Sector ID; #eta", 48,-0.5,47.5,32,-2.5,2.5) );
    #myGroup.defineHistogram('forwardSectorIDRoiEtaX,forwardSectorIDRoiEtaY;forwardSectorIDRoiEta', title='Sector ID vs. RoI #eta for forward muons (matched cand only); Sector ID; #eta',
    #                        type='TH2F',path=monROIPath,xbins=48,xmin=-0.5,xmax=47.5,ybins=32,ymin=-0.5,ymax=31.5,opt='kAlwaysCreate')
    myGroup.defineHistogram('forwardSectorIDRoiEtaX;forwardSectorIDRoiEta', title='Sector IDfor forward muons (matched cand only); Sector ID',
                            path=monROIPath,xbins=48,xmin=-0.5,xmax=47.5,opt='kAlwaysCreate')
    #
    #  //Sector vs. RoI Phi
    #  CHECK( registerTH2("barrelSectorIDRoiPhi","Sector ID vs. RoI #phi for barrel (matched cand only); Sector ID; #phi", 64,-0.5,63.5,32,-TMath::Pi(),TMath::Pi()) );
    #myGroup.defineHistogram('barrelSectorIDRoiPhiX,barrelSectorIDRoiPhiY;barrelSectorIDRoiPhi', title='Sector ID vs. RoI #phi for barrel (matched cand only); Sector ID; #phi',
    #                        type='TH2F',path=monROIPath,xbins=64,xmin=-0.5,xmax=63.5,ybins=32,ymin=-math.pi,ymax=math.pi,opt='kAlwaysCreate')
    myGroup.defineHistogram('barrelSectorIDRoiPhiX;barrelSectorIDRoiPhi', title='Sector ID for barrel (matched cand only); Sector ID',
                            type='TH2F',path=monROIPath,xbins=64,xmin=-0.5,xmax=63.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("endcapSectorIDRoiPhi","Sector ID vs. RoI #phi for endcap (matched cand only); Sector ID; #phi", 96,-0.5,95.5,32,-TMath::Pi(),TMath::Pi()) );
    #myGroup.defineHistogram('endcapSectorIDRoiPhiX,endcapSectorIDRoiPhiY;endcapSectorIDRoiPhi', title='Sector ID vs. RoI #phi for endcap (matched cand only); Sector ID; #phi',
    #                        type='TH2F',path=monROIPath,xbins=96,xmin=-0.5,xmax=95.5,ybins=32,ymin=-math.pi,ymax=math.pi,opt='kAlwaysCreate')
    myGroup.defineHistogram('endcapSectorIDRoiPhiX;endcapSectorIDRoiPhi', title='Sector ID for endcap (matched cand only); Sector ID',
                            path=monROIPath,xbins=96,xmin=-0.5,xmax=95.5,opt='kAlwaysCreate')
    #  CHECK( registerTH2("forwardSectorIDRoiPhi","Sector ID vs. RoI #phi for forward (matched cand only); Sector ID; #phi", 48,-0.5,47.5,32,-TMath::Pi(),TMath::Pi()) );
    #myGroup.defineHistogram('forwardSectorIDRoiPhiX,forwardSectorIDRoiPhiY;forwardSectorIDRoiPhi', title='Sector ID vs. RoI #phi for forward (matched cand only); Sector ID; #phi',
    #                        type='TH2F',path=monROIPath,xbins=48,xmin=-0.5,xmax=47.5,ybins=32,ymin=-math.pi,ymax=math.pi,opt='kAlwaysCreate')  
    myGroup.defineHistogram('forwardSectorIDRoiPhiX;forwardSectorIDRoiPhi', title='Sector ID for forward (matched cand only); Sector ID',
                            path=monROIPath,xbins=48,xmin=-0.5,xmax=47.5,opt='kAlwaysCreate')

    acc = helper.result()
    result.merge(acc)
    return result
    

if __name__=='__main__':

    # set input file and config options
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    import glob
    inputs = glob.glob('/eos/atlas/atlastier0/rucio/data18_13TeV/physics_Main/00354311/data18_13TeV.00354311.physics_Main.recon.ESD.f1129/data18_13TeV.00354311.physics_Main.recon.ESD.f1129._lb0013._SFO-8._0001.1')

    ConfigFlags.Input.Files = inputs
    ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput_CTPMonitoring.root'
    ConfigFlags.lock()
    #ConfigFlags.dump() # print all the configs

    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.Dump = False

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg  
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    BSMonitorCfg = BSMonitoringConfig(ConfigFlags)
    cfg.merge(BSMonitorCfg)
    # message level for algorithm
    BSMonitorCfg.getEventAlgo('BSMonAlg').OutputLevel = 1 # 1/2 INFO/DEBUG
    # options - print all details of algorithms, very short summary 
    cfg.printConfig(withDetails=False, summariseProps = True)
    nevents=-1
    cfg.run(nevents)

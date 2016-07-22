# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# ExtendedJetCommon.py
# Schedules special jet tools
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from JetRec.JetRecFlags import jetFlags

##################################################################

def addDefaultTrimmedJets(sequence,outputlist,dotruth=True,writeUngroomed=False):
    if jetFlags.useTruth and dotruth:
        addTrimmedJets('AntiKt', 1.0, 'Truth', rclus=0.2, ptfrac=0.05, algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed)
    addTrimmedJets('AntiKt', 1.0, 'LCTopo', rclus=0.2, ptfrac=0.05, algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed)

##################################################################

def applyJetAugmentation(jetalg,algname,sequence,jetaugtool):
    if hasattr(sequence,algname):
        jetaug = getattr(sequence,algname)
    else:
        jetaug = CfgMgr.DerivationFramework__CommonAugmentation(algname)
        sequence += jetaug

    if not jetaugtool in jetaug.AugmentationTools:
        jetaug.AugmentationTools.append(jetaugtool)

def getJetAugmentationTool(jetalg):
    jetaugtoolname = 'DFJetAug_'+jetalg
    jetaugtool = None
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,jetaugtoolname):
        jetaugtool = getattr(ToolSvc,jetaugtoolname)
    else:
        jetaugtool = CfgMgr.DerivationFramework__JetAugmentationTool(jetaugtoolname,
                                                                     InputJets=jetalg+'Jets')
        ToolSvc += jetaugtool

    return jetaugtool

def applyJetCalibration(jetalg,algname,sequence):
    calibtoolname = 'DFJetCalib_'+jetalg
    jetaugtool = getJetAugmentationTool(jetalg)

    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,calibtoolname):
        jetaugtool.JetCalibTool = getattr(ToolSvc,calibtoolname)
    else:
        isdata=False

        configdict = {'AntiKt4EMTopo':('JES_MC15cRecommendation_May2016.config',
                                       'JetArea_Residual_Origin_EtaJES_GSC'),
#                      'AntiKt4LCTopo':('JES_2015dataset_recommendation_Feb2016.config',
#                                       'JetArea_Residual_Origin_EtaJES_GSC'),
                      'AntiKt4EMPFlow':('JES_MC15Prerecommendation_PFlow_July2015.config',
                                        'JetArea_Residual_EtaJES_GSC'),
                      'AntiKt10LCTopoTrimmedPtFrac5SmallR20':('JES_MC15recommendation_FatJet_June2015.config',
                                                              'EtaJES_JMS'),
                      }
        isMC = "IS_SIMULATION" in inputFileSummary["evt_type"]
        isAF2 = False
        if isMC:
            isAF2 = 'ATLFASTII' in inputFileSummary['metadata']['/Simulation/Parameters']['SimulationFlavour'].upper()
        if isMC and isAF2: configdict['AntiKt4EMTopo'] = ('JES_MC15Prerecommendation_AFII_June2015.config',
                                                          'JetArea_Residual_Origin_EtaJES_GSC')

        config,calibseq = configdict[jetalg]
        if (not isMC) and jetalg=='AntiKt4EMTopo':
            calibseq+='_Insitu'
            isdata=True

        calibtool = CfgMgr.JetCalibrationTool(
            calibtoolname,
            JetCollection=jetalg,
            ConfigFile=config,
            CalibSequence=calibseq,
            IsData=isdata
            )
        ToolSvc += calibtool
        jetaugtool.JetCalibTool = calibtool

    print 'ExtendedJetCommon: Applying calibration to jet collection:', jetalg+'Jets'
    applyJetAugmentation(jetalg,algname,sequence,jetaugtool)

def applyJetCalibration_xAODColl(jetalg='AntiKt4EMTopo',sequence=DerivationFrameworkJob):
    supportedJets = ['AntiKt4EMTopo','AntiKt4EMPFlow']#'AntiKt4LCTopo'
    if not jetalg in supportedJets:
        print 'ExtendedJetCommon: ExtendedJetCommon: *** WARNING: Calibration requested for unsupported jet collection! ***'
        return
    else:
        applyJetCalibration(jetalg,'JetCommonKernel_xAODJets',sequence)

def applyJetCalibration_CustomColl(jetalg='AntiKt10LCTopoTrimmedPtFrac5SmallR20',sequence=None):
    supportedJets = ['AntiKt10LCTopoTrimmedPtFrac5SmallR20']
    if not jetalg in supportedJets:
        print 'ExtendedJetCommon: *** WARNING: Calibration requested for unsupported jet collection! ***'
        print 'ExtendedJetCommon: Supported custom jets:', supportedJets
        return
    else:
        applyJetCalibration(jetalg,'JetCommonKernel_OTFJets',sequence)

def updateJVT(jetalg,algname,sequence):
    jetaugtool = getJetAugmentationTool(jetalg)
    if(jetaugtool==None or jetaugtool.JetCalibTool==''):
        print 'ExtendedJetCommon: *** WARNING: JVT update called but corresponding augmentation tool does not exist!'
        print 'ExtendedJetCommon: *** WARNING: You must apply jet calibration before scheduling JVT!'

    jvttoolname = 'DFJetJvt_'+jetalg
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,jvttoolname):
        jetaugtool.JetJvtTool = getattr(ToolSvc,jvttoolname)
    else:
        jvttool = CfgMgr.JetVertexTaggerTool()
        ToolSvc += jvttool
        jetaugtool.JetJvtTool = jvttool

    print 'ExtendedJetCommon: Updating JVT for jet collection:', jetalg+'Jets'
    applyJetAugmentation(jetalg,algname,sequence,jetaugtool)

def updateJVT_xAODColl(jetalg='AntiKt4EMTopo',sequence=DerivationFrameworkJob):
    supportedJets = ['AntiKt4EMTopo']
    if not jetalg in supportedJets:
        print 'ExtendedJetCommon: *** WARNING: JVT update requested for unsupported jet collection! ***'
        return
    else:
        updateJVT(jetalg,'JetCommonKernel_xAODJets',sequence)

def applyBTaggingAugmentation(jetalg,algname='JetCommonKernel_xAODJets',sequence=DerivationFrameworkJob,btagtooldict={}):
    jetaugtool = getJetAugmentationTool(jetalg)

    if(jetaugtool==None or jetaugtool.JetCalibTool=='' or jetaugtool.JetJvtTool==''):
        print 'ExtendedJetCommon: *** WARNING: B-tagging called but corresponding augmentation tool does not exist!'
        print 'ExtendedJetCommon: *** WARNING: You must apply jet calibration and JVT !'

    btagWPs = []
    btagtools = []
    for WP,tool in sorted(btagtooldict.iteritems()):
        btagWPs.append(WP)
        btagtools.append(tool)
    jetaugtool.JetBtagTools = btagtools
    jetaugtool.JetBtagWPs = btagWPs

    print 'ExtendedJetCommon: Applying b-tagging working points for jet collection:', jetalg+'Jets'
    applyJetAugmentation(jetalg,algname,sequence,jetaugtool)

def addTrackSumMoments(jetalg,algname='JetCommonKernel_xAODJets',sequence=DerivationFrameworkJob):
    jetaugtool = getJetAugmentationTool(jetalg)
    jetaugtool.JetTrackSumMomentsTool = jtm.trksummoms
    print jetaugtool
    print jetaugtool.JetTrackSumMomentsTool
    print 'ExtendedJetCommon: Applying track sum moments for jet collection:', jetalg+'Jets'
    applyJetAugmentation(jetalg,algname,sequence,jetaugtool)

##################################################################

def replaceBuggyAntiKt4TruthWZJets(algseq,outputGroup='BugFix'):
    if jetFlags.useTruth:

        jetnamebase = 'AntiKt4TruthWZ'
        jetname = jetnamebase+'Jets'
        algname = 'jetalg'+jetnamebase
        OutputJets.setdefault(outputGroup , [] ).append(jetname)

        # return if the alg is already scheduled here :
        if algseq is None:
            print 'ExtendedJetCommon: No algsequence passed! Will not schedule', algname
            return
        elif algname in DFJetAlgs:
            if hasattr(algseq,algname):
                print 'ExtendedJetCommon:    Algsequence', algseq, 'already has an instance of', algname
            else:
                print 'ExtendedJetCommon:    Added', algname, 'to sequence', algseq
                algseq += DFJetAlgs[algname]
            return DFJetAlgs[algname]

        if not jetname in jtm.tools:

            finderTool= jtm.addJetFinder(jetname, 'AntiKt', 0.4, 'truthwz',ptmin=5000,
                                         overwrite=True, warnIfDuplicate=False)

            from JetRec.JetRecConf import JetAlgorithm
            alg = JetAlgorithm(algname, Tools = [finderTool])
            print 'ExtendedJetCommon:    Added', algname, 'to sequence', algseq
            algseq += alg
            DFJetAlgs[algname] = alg;

def replaceBuggyAntiKt10TruthWZJets(algseq,outputGroup='BugFix'):
    if jetFlags.useTruth:

        jetnamebase = 'AntiKt10TruthWZ'
        jetname = jetnamebase+'Jets'
        algname = 'jetalg'+jetnamebase
        OutputJets.setdefault(outputGroup , [] ).append(jetname)

        # return if the alg is already scheduled here :
        if algseq is None:
            print 'ExtendedJetCommon: No algsequence passed! Will not schedule', algname
            return
        elif algname in DFJetAlgs:
            if hasattr(algseq,algname):
                print 'ExtendedJetCommon:    Algsequence', algseq, 'already has an instance of', algname
            else:
                print 'ExtendedJetCommon:    Added', algname, 'to sequence', algseq
                algseq += DFJetAlgs[algname]
            return DFJetAlgs[algname]

        if not jetname in jtm.tools:

            finderTool= jtm.addJetFinder(jetname, 'AntiKt', 1.0, 'truthwz',ptmin=40000,
                                         overwrite=True, warnIfDuplicate=False)

            from JetRec.JetRecConf import JetAlgorithm
            alg = JetAlgorithm(algname, Tools = [finderTool])
            print 'ExtendedJetCommon:    Added', algname, 'to sequence', algseq
            algseq += alg
            DFJetAlgs[algname] = alg;

##################################################################

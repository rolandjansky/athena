# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# ExtendedJetCommon.py
# Schedules special jet tools
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from JetRec.JetRecFlags import jetFlags

from AthenaCommon import Logging
extjetlog = Logging.logging.getLogger('ExtendedJetCommon')

##################################################################

def addDefaultTrimmedJets(sequence,outputlist,dotruth=True,writeUngroomed=False):
    if jetFlags.useTruth and dotruth:
        addTrimmedJets('AntiKt', 1.0, 'Truth', rclus=0.2, ptfrac=0.05, algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed)
    addTrimmedJets('AntiKt', 1.0, 'LCTopo', rclus=0.2, ptfrac=0.05, algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed)

##################################################################              
# Add AntiKt jets                                                               
##################################################################              

def addAntiKt10LCTopoJets(sequence, outputlist):
        addStandardJets("AntiKt", 1.0, "LCTopo", 40000, mods="lctopo_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt2PV0TrackJets(sequence, outputlist):
        addStandardJets("AntiKt", 0.2, "PV0Track", 2000, mods="track_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt4PV0TrackJets(sequence, outputlist):
        addStandardJets("AntiKt", 0.4, "PV0Track", 2000, mods="track_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt10PV0TrackJets(sequence, outputlist):
        addStandardJets("AntiKt", 1.0, "PV0Track", 2000, mods="track_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt4TruthJets(sequence,outputlist):
        addStandardJets("AntiKt", 0.4, "Truth", 5000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt10TruthJets(sequence,outputlist):
        addStandardJets("AntiKt", 1.0, "Truth", 40000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt4TruthWZJets(sequence,outputlist):
        addStandardJets("AntiKt", 0.4, "TruthWZ", 5000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

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

def getJetExternalAssocTool(jetalg, extjetalg, **options):
    jetassoctoolname = 'DFJetExternalAssoc_%s_From_%s' % (jetalg, extjetalg)
    jetassoctool = None
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,jetassoctoolname):
        jetassoctool = getattr(ToolSvc,jetassoctoolname)
    else:
        jetassoctool = CfgMgr.DerivationFramework__JetExternalAssocTool(jetassoctoolname,
                                                                        InputJets=jetalg+'Jets',
                                                                        ExternalJetCollectionName = extjetalg+'Jets',
                                                                        **options)
        ToolSvc += jetassoctool

    return jetassoctool

def applyJetCalibration(jetalg,algname,sequence):
    calibtoolname = 'DFJetCalib_'+jetalg
    jetaugtool = getJetAugmentationTool(jetalg)

    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,calibtoolname):
        jetaugtool.JetCalibTool = getattr(ToolSvc,calibtoolname)
    else:
        isdata=False

        configdict = {'AntiKt4EMTopo':('JES_data2016_data2015_Recommendation_Dec2016_rel21.config',
                                       'JetArea_Residual_EtaJES_GSC'),
                      'AntiKt4LCTopo':('JES_data2016_data2015_Recommendation_Dec2016_rel21.config',
                                       'JetArea_Residual_EtaJES_GSC'),
                      'AntiKt4EMPFlow':('JES_MC15Prerecommendation_PFlow_Aug2016.config',
                                        'JetArea_Residual_EtaJES_GSC'),
                      'AntiKt10LCTopoTrimmedPtFrac5SmallR20':('JES_MC15recommendation_FatJet_Nov2016_QCDCombinationUncorrelatedWeights.config',
                                                              'EtaJES_JMS'),
                      }
        isMC = "IS_SIMULATION" in inputFileSummary["evt_type"]
        isAF2 = False
        if isMC:
            isAF2 = 'ATLFASTII' in inputFileSummary['metadata']['/Simulation/Parameters']['SimulationFlavour'].upper()
        if isMC and isAF2:
            configdict['AntiKt4EMTopo'] = ('JES_MC15Prerecommendation_AFII_June2015.config',
                                           'JetArea_Residual_EtaJES_GSC')

        config,calibseq = configdict[jetalg]
        if (not isMC) and jetalg in ['AntiKt4EMTopo','AntiKt4LCTopo']:
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

    extjetlog.info('Applying calibration to jet collection: '+jetalg+'Jets')
    applyJetAugmentation(jetalg,algname,sequence,jetaugtool)

def applyJetCalibration_xAODColl(jetalg='AntiKt4EMTopo',sequence=DerivationFrameworkJob):
    supportedJets = ['AntiKt4EMTopo','AntiKt4LCTopo','AntiKt4EMPFlow']
    if not jetalg in supportedJets:
        extjetlog.warning('*** Calibration requested for unsupported jet collection '+jetalg+'! ***')
        return
    else:
        applyJetCalibration(jetalg,'JetCommonKernel_xAODJets',sequence)

def applyJetCalibration_CustomColl(jetalg='AntiKt10LCTopoTrimmedPtFrac5SmallR20',sequence=None):
    supportedJets = ['AntiKt10LCTopoTrimmedPtFrac5SmallR20']
    if not jetalg in supportedJets:
        extjetlog.warning('*** Calibration requested for unsupported jet collection! ***')
        extjetlog.warning('Supported custom jets: '+supportedJets)
        return
    else:
        applyJetCalibration(jetalg,'JetCommonKernel_OTFJets',sequence)

def updateJVT(jetalg,algname,sequence):
    jetaugtool = getJetAugmentationTool(jetalg)
    if(jetaugtool==None or jetaugtool.JetCalibTool==''):
        extjetlog.warning('*** JVT update called but corresponding augmentation tool does not exist! ***')
        extjetlog.warning('*** You must apply jet calibration before scheduling JVT! ***')

    jvttoolname = 'DFJetJvt_'+jetalg
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,jvttoolname):
        jetaugtool.JetJvtTool = getattr(ToolSvc,jvttoolname)
    else:
        jvttool = CfgMgr.JetVertexTaggerTool()
        ToolSvc += jvttool
        jetaugtool.JetJvtTool = jvttool

    extjetlog.info('ExtendedJetCommon: Updating JVT for jet collection: '+jetalg+'Jets')
    applyJetAugmentation(jetalg,algname,sequence,jetaugtool)

def updateJVT_xAODColl(jetalg='AntiKt4EMTopo',sequence=DerivationFrameworkJob):
    supportedJets = ['AntiKt4EMTopo']
    if not jetalg in supportedJets:
        extjetlog.warning('*** JVT update requested for unsupported jet collection! ***')
        return
    else:
        updateJVT(jetalg,'JetCommonKernel_xAODJets',sequence)

def applyBTaggingAugmentation(jetalg,algname='JetCommonKernel_xAODJets',sequence=DerivationFrameworkJob,btagtooldict={}):
    jetaugtool = getJetAugmentationTool(jetalg)

    if(jetaugtool==None or jetaugtool.JetCalibTool=='' or jetaugtool.JetJvtTool==''):
        extjetlog.warning('*** B-tagging called but corresponding augmentation tool does not exist! ***')
        extjetlog.warning('*** You must apply jet calibration and JVT! ***')

    btagWPs = []
    btagtools = []
    for WP,tool in sorted(btagtooldict.iteritems()):
        btagWPs.append(WP)
        btagtools.append(tool)
    jetaugtool.JetBtagTools = btagtools
    jetaugtool.JetBtagWPs = btagWPs

    extjetlog.info('ExtendedJetCommon: Applying b-tagging working points for jet collection: '+jetalg+'Jets')
    applyJetAugmentation(jetalg,algname,sequence,jetaugtool)

def addTrackSumMoments(jetalg,algname='JetCommonKernel_xAODJets',sequence=DerivationFrameworkJob):
    extjetlog.warning('addTrackSumMoments is now deprecated. Its no longer used for Release 21')

##################################################################

def replaceBuggyAntiKt4TruthWZJets(algseq,outputGroup='BugFix'):
    extjetlog.warning('replaceBuggyAntiKt4TruthWZJets is now deprecated')

def replaceBuggyAntiKt10TruthWZJets(algseq,outputGroup='BugFix'):
    extjetlog.warning('replaceBuggyAntiKt10TruthWZJets is now deprecated')

##################################################################

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# ExtendedJetCommon.py
# Schedules special jet tools
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
import DerivationFrameworkEGamma.EGammaCommon
import DerivationFrameworkMuons.MuonsCommon
import DerivationFrameworkTau.TauCommon
from DerivationFrameworkFlavourTag.FlavourTagCommon import applyBTagging_xAODColl
from JetRec.JetRecFlags import jetFlags

from AthenaCommon import Logging
extjetlog = Logging.logging.getLogger('ExtendedJetCommon')

# for debugging output
from AthenaCommon.Constants import INFO,DEBUG,WARNING

##################################################################
# Jet helpers for large-radius groomed jets
##################################################################

def addDefaultTrimmedJets(sequence,outputlist,dotruth=True,writeUngroomed=False):
    if DerivationFrameworkIsMonteCarlo and dotruth:
        addTrimmedJets('AntiKt', 1.0, 'Truth', rclus=0.2, ptfrac=0.05, mods="truth_groomed",
                       algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed)
    addTrimmedJets('AntiKt', 1.0, 'LCTopo', rclus=0.2, ptfrac=0.05, mods="lctopo_groomed",
                   algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed)

def addTCCTrimmedJets(sequence,outputlist,dotruth=True,writeUngroomed=False):
    addTrimmedJets('AntiKt', 1.0, 'TrackCaloCluster', rclus=0.2, ptfrac=0.05, mods="tcc_groomed",
                   algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed)

##################################################################
# Jet helpers for ungroomed jets (removed in xAOD reduction)
##################################################################

from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4TopoEM->AntiKt4EMTopo" ]
BTaggingFlags.Jets=[]
from BTagging.BTaggingConfiguration import getConfiguration

ConfInst=getConfiguration()
ConfInst.doNotCheckForTaggerObstacles()

from JetRec.JetRecStandard import jtm

def addAntiKt10LCTopoJets(sequence, outputlist):
    addStandardJets("AntiKt", 1.0, "LCTopo", ptmin=40000, ptminFilter=50000, mods="lctopo_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt10TrackCaloClusterJets(sequence, outputlist):
    addStandardJets("AntiKt", 1.0, "TrackCaloCluster", ptmin=40000, ptminFilter=50000, mods="tcc_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt2PV0TrackJets(sequence, outputlist):
    if not "akt2track" in jtm.modifiersMap.keys():
        from AthenaCommon.AppMgr import ToolSvc
        btag_akt2trk = ConfInst.setupJetBTaggerTool(ToolSvc, JetCollection="AntiKt2Track", AddToToolSvc=True,
                                                    Verbose=True,
                                                    options={"name"         : "btagging_antikt2track",
                                                             "BTagName"     : "BTagging_AntiKt2Track",
                                                             "BTagJFVtxName": "JFVtx",
                                                             "BTagSVName"   : "SecVtx",
                                                             },
                                                    SetupScheme = "",
                                                    TaggerList = ['IP2D', 'IP3D', 'MultiSVbb1',  'MultiSVbb2', 'SV1', 'JetFitterNN', 'SoftMu', 'MV2c10', 'MV2c10mu', 'MV2c10rnn', 'JetVertexCharge', 'MV2cl100' , 'MVb', 'DL1', 'DL1rnn', 'DL1mu', 'RNNIP', 'MV2c10Flip']
                                                    )
        jtm.modifiersMap["akt2track"] = jtm.modifiersMap["track_ungroomed"] + [btag_akt2trk]
    addStandardJets("AntiKt", 0.2, "PV0Track", ptmin=2000, mods="akt2track",
                    algseq=sequence, outputGroup=outputlist)

def addAntiKt4PV0TrackJets(sequence, outputlist):
    if not "akt4track" in jtm.modifiersMap.keys():
        from AthenaCommon.AppMgr import ToolSvc
        btag_akt4trk = ConfInst.setupJetBTaggerTool(ToolSvc, JetCollection="AntiKt4Track", AddToToolSvc=True,
                                                    Verbose=True,
                                                    options={"name"         : "btagging_antikt4track",
                                                             "BTagName"     : "BTagging_AntiKt4Track",
                                                             "BTagJFVtxName": "JFVtx",
                                                             "BTagSVName"   : "SecVtx",
                                                             },
                                                    SetupScheme = "",
                                                    TaggerList = ['IP2D', 'IP3D', 'MultiSVbb1',  'MultiSVbb2', 'SV1', 'JetFitterNN', 'SoftMu', 'MV2c10', 'MV2c10mu', 'MV2c10rnn', 'JetVertexCharge', 'MV2cl100' , 'MVb', 'DL1', 'DL1rnn', 'DL1mu', 'RNNIP', 'MV2c10Flip']
                                                    )
        jtm.modifiersMap["akt4track"] = jtm.modifiersMap["track_ungroomed"] + [btag_akt4trk]
    addStandardJets("AntiKt", 0.4, "PV0Track", ptmin=2000, mods="akt4track", algseq=sequence, outputGroup=outputlist)

def addAntiKt10PV0TrackJets(sequence, outputlist):
    addStandardJets("AntiKt", 1.0, "PV0Track", ptmin=2000, ptminFilter=40000, mods="track_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt4TruthJets(sequence,outputlist):
    if DerivationFrameworkIsMonteCarlo:
        addStandardJets("AntiKt", 0.4, "Truth", ptmin=5000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt4TruthWZJets(sequence,outputlist):
    if DerivationFrameworkIsMonteCarlo:
        addStandardJets("AntiKt", 0.4, "TruthWZ", ptmin=5000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt10TruthJets(sequence,outputlist):
    if DerivationFrameworkIsMonteCarlo:
        addStandardJets("AntiKt", 1.0, "Truth", ptmin=40000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt10TruthWZJets(sequence,outputlist):
    if DerivationFrameworkIsMonteCarlo:
        addStandardJets("AntiKt", 1.0, "TruthWZ", ptmin=40000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt4TruthDressedWZJets(sequence,outputlist):
    if DerivationFrameworkIsMonteCarlo:
        addStandardJets("AntiKt", 0.4, "TruthDressedWZ", ptmin=5000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

def replaceAODReducedJets(jetlist,sequence,outputlist):
    extjetlog.info( "Replacing AOD-reduced jet collections: {0}".format(",".join(jetlist)))
    if "AntiKt2PV0TrackJets" in jetlist:
        addAntiKt2PV0TrackJets(sequence,outputlist)
    if "AntiKt4PV0TrackJets" in jetlist:
        addAntiKt4PV0TrackJets(sequence,outputlist)
    if "AntiKt10PV0TrackJets" in jetlist:
        addAntiKt10PV0TrackJets(sequence,outputlist)
    if "AntiKt4TruthJets" in jetlist:
        addAntiKt4TruthJets(sequence,outputlist)
    if "AntiKt4TruthWZJets" in jetlist:
        addAntiKt4TruthWZJets(sequence,outputlist)
    if "AntiKt10TruthJets" in jetlist:
        addAntiKt10TruthJets(sequence,outputlist)
    if "AntiKt10TruthWZJets" in jetlist:
        addAntiKt10TruthWZJets(sequence,outputlist)
    if "AntiKt10LCTopoJets" in jetlist:
        addAntiKt10LCTopoJets(sequence,outputlist)
    if "AntiKt10TrackCaloClusterJets" in jetlist:
        addAntiKt10TrackCaloClusterJets(sequence,outputlist)

##################################################################
# Jet helpers for adding low-pt jets needed for calibration
##################################################################


def addAntiKt4LowPtJets(sequence,outputlist):
    addStandardJets("AntiKt", 0.4, "EMTopo",  namesuffix="LowPt", ptmin=2000, ptminFilter=2000,
                    mods="emtopo_ungroomed", algseq=sequence, outputGroup=outputlist,calibOpt="ar")
    addStandardJets("AntiKt", 0.4, "LCTopo",  namesuffix="LowPt", ptmin=2000, ptminFilter=2000,
                    mods="lctopo_ungroomed", algseq=sequence, outputGroup=outputlist,calibOpt="ar")
    # Commented for now because of problems with underlying PFlow collections
    # addStandardJets("AntiKt", 0.4, "EMPFlow", namesuffix="LowPt", ptmin=2000, ptminFilter=2000,
    #                 mods="pflow_ungroomed", algseq=sequence, outputGroup=outputlist="ar:pflow")

##################################################################

def applyJetAugmentation(jetalg,algname,sequence,jetaugtool):
    if hasattr(sequence,algname):
        jetaug = getattr(sequence,algname)
    else:
        jetaug = CfgMgr.DerivationFramework__CommonAugmentation(algname)
        sequence += jetaug

    if not jetaugtool in jetaug.AugmentationTools:
        jetaug.AugmentationTools.append(jetaugtool)

def getJetAugmentationTool(jetalg, suffix=''):
    jetaugtoolname = 'DFJetAug_'+jetalg+suffix
    jetaugtool = None
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,jetaugtoolname):
        jetaugtool = getattr(ToolSvc,jetaugtoolname)
    else:
        jetaugtool = CfgMgr.DerivationFramework__JetAugmentationTool(jetaugtoolname,
                                                                     InputJets=jetalg+'Jets')
        ToolSvc += jetaugtool

    return jetaugtool

def getJetCleaningTool(cleaningLevel):
    jetcleaningtoolname = 'JetCleaningTool_'+cleaningLevel
    jetcleaningtool = None
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,jetcleaningtoolname):
        jetcleaningtool = getattr(ToolSvc,jetcleaningtoolname)
    else:
        jetcleaningtool = CfgMgr.JetCleaningTool(jetcleaningtoolname,CutLevel=cleaningLevel)
        ToolSvc += jetcleaningtool

    return jetcleaningtool

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

        configdict = {'AntiKt4EMTopo':('JES_MC16Recommendation_Aug2017.config',
                                       'JetArea_Residual_EtaJES_GSC'),
                      'AntiKt4LCTopo':('JES_data2016_data2015_Recommendation_Dec2016_rel21.config',
                                       'JetArea_Residual_EtaJES_GSC'),
                      'AntiKt4EMPFlow':('JES_MC15Prerecommendation_PFlow_Aug2016.config',
                                        'JetArea_Residual_EtaJES_GSC'),
                      'AntiKt10LCTopoTrimmedPtFrac5SmallR20':('JES_MC15recommendation_FatJet_Nov2016_QCDCombinationUncorrelatedWeights.config',
                                                              'EtaJES_JMS'),
                      }
        isMC = DerivationFrameworkIsMonteCarlo
        isAF2 = False
        if isMC:
            isAF2 = 'ATLFASTII' in inputFileSummary['metadata']['/Simulation/Parameters']['SimulationFlavour'].upper()
        if isMC and isAF2:
            configdict['AntiKt4EMTopo'] = ('JES_MC15Prerecommendation_AFII_June2015_rel21.config', 
                                           'JetArea_Residual_EtaJES_GSC')

        config,calibseq = configdict[jetalg]
        # As of 11 Sept 2017, the in situ calibration for R21
        # is not yet ready.
        # When this is available, it should be reenabled -- for PFlow as well.
        # if (not isMC) and jetalg in ['AntiKt4EMTopo','AntiKt4LCTopo']:
        #     calibseq+='_Insitu'
        #     isdata=True

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

def addJetPtAssociation(jetalg, truthjetalg, sequence, algname):
    jetaugtool = getJetAugmentationTool(jetalg, '_PtAssoc')
    if(jetaugtool==None):
        extjetlog.warning('*** addJetPtAssociation called but corresponding augmentation tool does not exist! ***')

    jetptassociationtoolname = 'DFJetPtAssociation_'+truthjetalg
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,jetptassociationtoolname):
        jetaugtool.JetPtAssociationTool = getattr(ToolSvc,jetptassociationtoolname)
    else:
        jetptassociationtool = CfgMgr.JetPtAssociationTool(jetptassociationtoolname, InputContainer=truthjetalg, AssociationName="GhostTruth")
        ToolSvc += jetptassociationtool
        jetaugtool.JetPtAssociationTool = jetptassociationtool

    extjetlog.info('ExtendedJetCommon: Adding JetPtAssociationTool for jet collection: '+jetalg+'Jets')
    applyJetAugmentation(jetalg,algname,sequence,jetaugtool)

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

def addOriginCorrection(jetalg, sequence, algname,vertexPrefix):
    jetaugtool = getJetAugmentationTool(jetalg,'_OriginCorr'+vertexPrefix)
    if(jetaugtool==None):
        extjetlog.warning('*** addOriginCorrection called but corresponding augmentation tool does not exist! ***')

    origincorrectiontoolname = 'DFOriginCorrection'+vertexPrefix+'_'+jetalg
    jetaugtool.MomentPrefix = vertexPrefix+'_'
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,origincorrectiontoolname):
        jetaugtool.JetOriginCorrectionTool = getattr(ToolSvc,origincorrectiontoolname)
    else:
        origincorrectiontool = CfgMgr.JetOriginCorrectionTool(origincorrectiontoolname, VertexContainer=vertexPrefix+'PrimaryVertices',OriginCorrectedName=vertexPrefix+'_JetOriginConstitScaleMomentum',ForceEMScale=True)
        ToolSvc += origincorrectiontool
        jetaugtool.JetOriginCorrectionTool = origincorrectiontool

    extjetlog.info('ExtendedJetCommon: Adding OriginCorrection for jet collection: '+jetalg)
    applyJetAugmentation(jetalg,algname,sequence,jetaugtool)

def applyOverlapRemoval(sequence=DerivationFrameworkJob):
    from AssociationUtils.config import recommended_tools
    from AssociationUtils.AssociationUtilsConf import OverlapRemovalGenUseAlg
    outputLabel = 'DFCommonJets_passOR'
    bJetLabel = 'isBJet'
    orTool = recommended_tools(outputLabel=outputLabel,bJetLabel=bJetLabel)
    algOR = OverlapRemovalGenUseAlg('OverlapRemovalGenUseAlg',
			    OverlapLabel=outputLabel,
                            OverlapRemovalTool=orTool,
                            BJetLabel=bJetLabel)
    sequence += algOR

def eventCleanLoose_xAODColl(jetalg='AntiKt4EMTopo',sequence=DerivationFrameworkJob):
    from JetSelectorTools.JetSelectorToolsConf import ECUtils__EventCleaningTool as EventCleaningTool
    from JetSelectorTools.JetSelectorToolsConf import EventCleaningTestAlg
    jetcleaningtoolname = "EventCleaningTool_Loose"
    prefix = "DFCommonJets_"
    ecToolLoose = EventCleaningTool('EventCleaningTool_Loose',CleaningLevel='LooseBad')
    ecToolLoose.JetCleanPrefix = prefix
    ecToolLoose.JetCleaningTool = getJetCleaningTool("LooseBad")
    algCleanLoose = EventCleaningTestAlg('EventCleaningTestAlg_loose',
                            EventCleaningTool=ecToolLoose,
                            JetCollectionName="AntiKt4EMTopoJets",
			    EventCleanPrefix=prefix)
    sequence += algCleanLoose

def eventCleanTight_xAODColl(jetalg='AntiKt4EMTopo',sequence=DerivationFrameworkJob):
    from JetSelectorTools.JetSelectorToolsConf import ECUtils__EventCleaningTool as EventCleaningTool
    from JetSelectorTools.JetSelectorToolsConf import EventCleaningTestAlg
    jetcleaningtoolname = "EventCleaningTool_Tight"
    prefix = "DFCommonJets_"
    ecToolTight = EventCleaningTool('EventCleaningTool_Tight',CleaningLevel='TightBad')
    ecToolTight.JetCleanPrefix = prefix
    ecToolTight.JetCleaningTool = getJetCleaningTool("TightBad")
    algCleanTight = EventCleaningTestAlg('EventCleaningTestAlg_tight',
                            EventCleaningTool=ecToolTight,
                            JetCollectionName="AntiKt4EMTopoJets",
			    EventCleanPrefix=prefix,
			    CleaningLevel="TightBad",
		  	    doEvent=False)
    sequence += algCleanTight

def addRscanJets(jetalg,radius,inputtype,sequence,outputlist):
    jetname = "{0}{1}{2}Jets".format(jetalg,int(radius*10),inputtype)
    algname = "jetalg"+jetname

    if not hasattr(sequence,algname):
        if inputtype == "Truth":
            addStandardJets(jetalg, radius, "Truth", mods="truth_ungroomed", ptmin=5000, algseq=sequence, outputGroup=outputlist)
        if inputtype == "TruthWZ":
            addStandardJets(jetalg, radius, "TruthWZ", mods="truth_ungroomed", ptmin=5000, algseq=sequence, outputGroup=outputlist)
        elif inputtype == "LCTopo":
            addStandardJets(jetalg, radius, "LCTopo", mods="lctopo_ungroomed",
                            ghostArea=0.01, ptmin=2000, ptminFilter=7000, calibOpt="none", algseq=sequence, outputGroup=outputlist)

##################################################################
applyJetCalibration_xAODColl("AntiKt4EMTopo")
updateJVT_xAODColl("AntiKt4EMTopo")
applyBTagging_xAODColl("AntiKt4EMTopo")
applyOverlapRemoval()
eventCleanLoose_xAODColl("AntiKt4EMTopo")
eventCleanTight_xAODColl("AntiKt4EMTopo")


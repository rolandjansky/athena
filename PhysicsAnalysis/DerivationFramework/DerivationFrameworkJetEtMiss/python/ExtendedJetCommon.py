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
from DerivationFrameworkFlavourTag.HbbCommon import (
    buildVRJets, linkVRJetsToLargeRJets)
from JetRec.JetRecFlags import jetFlags
from JetJvtEfficiency.JetJvtEfficiencyToolConfig import (getJvtEffTool, getJvtEffToolName)

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

from ParticleJetTools.ParticleJetToolsConf import JetParticleShrinkingConeAssociation

ConfInst=getConfiguration()
ConfInst.doNotCheckForTaggerObstacles()

from JetRec.JetRecStandard import jtm

def addAntiKt10LCTopoJets(sequence, outputlist):
    addStandardJets("AntiKt", 1.0, "LCTopo", ptmin=40000, ptminFilter=50000, mods="lctopo_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt10TrackCaloClusterJets(sequence, outputlist):
    addStandardJets("AntiKt", 1.0, "TrackCaloCluster", ptmin=40000, ptminFilter=50000, mods="tcc_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt2PV0TrackJets(sequence, outputlist, extendedFlag = 0):
    if not "akt2track" in jtm.modifiersMap.keys():
        from AthenaCommon.AppMgr import ToolSvc
        from BTagging.BTaggingFlags import BTaggingFlags

        if(extendedFlag == 0) :
           Full_TaggerList = BTaggingFlags.StandardTaggers
        if(extendedFlag == 1) :
           Full_TaggerList = BTaggingFlags.ExpertTaggers

        btag_akt2trk = ConfInst.setupJetBTaggerTool(ToolSvc, JetCollection="AntiKt2Track", AddToToolSvc=True,
                                                    Verbose=True,
                                                    options={"name"         : "btagging_antikt2track",
                                                             "BTagName"     : "BTagging_AntiKt2Track",
                                                             "BTagJFVtxName": "JFVtx",
                                                             "BTagSVName"   : "SecVtx",
                                                             },
                                                    SetupScheme = "",
                                                    TaggerList = Full_TaggerList
                                                    )

        from BTagging.BTaggingConfiguration import defaultTrackAssoc, defaultMuonAssoc
        jtm.modifiersMap["akt2track"] = jtm.modifiersMap["track_ungroomed"] + [defaultTrackAssoc, defaultMuonAssoc, btag_akt2trk]

    addStandardJets("AntiKt", 0.2, "PV0Track", ptmin=2000, mods="akt2track",
                    algseq=sequence, outputGroup=outputlist)

def addAntiKt4PV0TrackJets(sequence, outputlist, extendedFlag = 0):
    if not "akt4track" in jtm.modifiersMap.keys():
        from AthenaCommon.AppMgr import ToolSvc
        from BTagging.BTaggingFlags import BTaggingFlags

        if(extendedFlag == 0) :
           Full_TaggerList = BTaggingFlags.StandardTaggers
        if(extendedFlag == 1) :
           Full_TaggerList = BTaggingFlags.ExpertTaggers

        btag_akt4trk = ConfInst.setupJetBTaggerTool(ToolSvc, JetCollection="AntiKt4Track", AddToToolSvc=True,
                                                    Verbose=True,
                                                    options={"name"         : "btagging_antikt4track",
                                                             "BTagName"     : "BTagging_AntiKt4Track",
                                                             "BTagJFVtxName": "JFVtx",
                                                             "BTagSVName"   : "SecVtx",
                                                             },
                                                    SetupScheme = "",
                                                    TaggerList = Full_TaggerList
                                                    )

        from BTagging.BTaggingConfiguration import defaultTrackAssoc, defaultMuonAssoc
        jtm.modifiersMap["akt4track"] = jtm.modifiersMap["track_ungroomed"] + [defaultTrackAssoc, defaultMuonAssoc, btag_akt4trk]

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

def replaceAODReducedJets(jetlist,sequence,outputlist, extendedFlag = 0):
    extjetlog.info( "Replacing AOD-reduced jet collections: {0}".format(",".join(jetlist)))
    if "AntiKt2PV0TrackJets" in jetlist:
        addAntiKt2PV0TrackJets(sequence,outputlist, extendedFlag)
    if "AntiKt4PV0TrackJets" in jetlist:
        addAntiKt4PV0TrackJets(sequence,outputlist, extendedFlag)
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

# 2 GeV cut after pileup suppression for in-situ Z
def addAntiKt4LowPtJets(sequence,outputlist):
    addStandardJets("AntiKt", 0.4, "EMTopo",  namesuffix="LowPt", ptmin=2000, ptminFilter=2000,
                    mods="emtopo_ungroomed", algseq=sequence, outputGroup=outputlist,calibOpt="ar")
    addStandardJets("AntiKt", 0.4, "LCTopo",  namesuffix="LowPt", ptmin=2000, ptminFilter=2000,
                    mods="lctopo_ungroomed", algseq=sequence, outputGroup=outputlist,calibOpt="ar")
    # Commented for now because of problems with underlying PFlow collections
    addCHSPFlowObjects()
    addStandardJets("AntiKt", 0.4, "EMPFlow", namesuffix="LowPt", ptmin=2000, ptminFilter=2000,
                    mods="pflow_ungroomed", algseq=sequence, outputGroup=outputlist,calibOpt="ar:pflow")


# 1 MeV cut at constituent level for MCJES
def addAntiKt4NoPtCutJets(sequence,outputlist):
    addStandardJets("AntiKt", 0.4, "EMTopo",  namesuffix="NoPtCut", ptmin=0, ptminFilter=1,
                    mods="emtopo_ungroomed", algseq=sequence, outputGroup=outputlist,calibOpt="none")
    addStandardJets("AntiKt", 0.4, "LCTopo",  namesuffix="NoPtCut", ptmin=0, ptminFilter=1,
                    mods="lctopo_ungroomed", algseq=sequence, outputGroup=outputlist,calibOpt="none")
    # Commented for now because of problems with underlying PFlow collections
    addCHSPFlowObjects()
    addStandardJets("AntiKt", 0.4, "EMPFlow", namesuffix="NoPtCut", ptmin=0, ptminFilter=1,
                    mods="pflow_ungroomed", algseq=sequence, outputGroup=outputlist,calibOpt="none")

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
        jetcleaningtool.UseDecorations = False
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

def applyJetCalibration(jetalg,algname,sequence,fatjetconfig = 'comb'):
    calibtoolname = 'DFJetCalib_'+jetalg
    jetaugtool = getJetAugmentationTool(jetalg)

    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,calibtoolname):
        jetaugtool.JetCalibTool = getattr(ToolSvc,calibtoolname)
    else:
        isdata=False

        #fatjetconfig selects config file for AntiKt10LCTopoTrimmedPtFrac5SmallR20, default is JES_MC16recommendation_FatJet_JMS_comb_19Jan2018.config
        if not fatjetconfig in ['comb','calo','TA']:
            extjetlog.warning('*** Wrong value for fatjetconfig!  Only \'comb\' (default), \'calo\' or \'TA\' can be used. ***')

        configdict = {'AntiKt4EMTopo':('JES_data2017_2016_2015_Recommendation_Feb2018_rel21.config',
                                       'JetArea_Residual_EtaJES_GSC'),
                      'AntiKt4LCTopo':('JES_MC16Recommendation_28Nov2017.config',
                                       'JetArea_Residual_EtaJES_GSC'),
                      'AntiKt4EMPFlow':('JES_data2017_2016_2015_Recommendation_PFlow_Feb2018_rel21.config',
                                        'JetArea_Residual_EtaJES_GSC'),
                      'AntiKt10LCTopoTrimmedPtFrac5SmallR20':('JES_MC16recommendation_FatJet_JMS_comb_19Jan2018.config',
                                                              'EtaJES_JMS'),
                      'AntiKt2LCTopo':('JES_2015_2016_data_Rscan2LC_18Dec2018_R21.config',
                                       'JetArea_Residual_EtaJES_GSC'),
                      }
        if fatjetconfig=='calo': #Choose JES_MC16recommendation_FatJet_JMS_calo_29Nov2017.config for AntiKt10LCTopoTrimmedPtFrac5SmallR20
            configdict.update({'AntiKt10LCTopoTrimmedPtFrac5SmallR20':('JES_MC16recommendation_FatJet_JMS_calo_29Nov2017.config',
                                                                       'EtaJES_JMS')
                              })
        if fatjetconfig=='TA': #Choose JES_MC16recommendation_FatJet_JMS_TA_29Nov2017.config for AntiKt10LCTopoTrimmedPtFrac5SmallR20
            configdict.update({'AntiKt10LCTopoTrimmedPtFrac5SmallR20':('JES_MC16recommendation_FatJet_JMS_TA_29Nov2017.config',
                                                                       'EtaJES_JMS')
                              })
        isMC = DerivationFrameworkIsMonteCarlo
        isAF2 = False
        if isMC:
            isAF2 = 'ATLFASTII' in inputFileSummary['metadata']['/Simulation/Parameters']['SimulationFlavour'].upper()
        if isMC and isAF2:
            configdict['AntiKt4EMTopo'] = ('JES_MC15Prerecommendation_AFII_June2015_rel21.config',
                                           'JetArea_Residual_EtaJES_GSC')

        config,calibseq = configdict[jetalg]

        if (not isMC) and jetalg in ['AntiKt4EMTopo','AntiKt4LCTopo','AntiKt4EMPFlow']:
            isdata=True
            if not jetalg=='AntiKt4LCTopo': calibseq+='_Insitu'

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
        applyJetCalibration(jetalg,'JetCommonKernel_{0}'.format(jetalg),sequence)

def applyJetCalibration_CustomColl(jetalg='AntiKt10LCTopoTrimmedPtFrac5SmallR20',sequence=None):
    supportedJets = ['AntiKt10LCTopoTrimmedPtFrac5SmallR20','AntiKt2LCTopo']
    if not jetalg in supportedJets:
        extjetlog.warning('*** Calibration requested for unsupported jet collection! ***')
        extjetlog.warning('Supported custom jets: '+supportedJets)
        return
    else:
        applyJetCalibration(jetalg,'JetCommonKernel_{0}'.format(jetalg),sequence)

def updateJVT(jetalg,algname,sequence):
    jetaugtool = getJetAugmentationTool(jetalg)
    if(jetaugtool==None or jetaugtool.JetCalibTool==''):
        extjetlog.warning('*** JVT update called but corresponding augmentation tool does not exist! ***')
        extjetlog.warning('*** You must apply jet calibration before scheduling JVT! ***')

    jvttoolname = 'DFJetJvt_'+jetalg
    #use the standard name defined by the config helper
    jvtefftoolname = getJvtEffToolName(jetalg)

    from AthenaCommon.AppMgr import ToolSvc

    #setup the jvt updating tools if not already done
    if hasattr(ToolSvc,jvttoolname):
        jetaugtool.JetJvtTool = getattr(ToolSvc,jvttoolname)
    else:
        jvttool = CfgMgr.JetVertexTaggerTool(jvttoolname) 
        ToolSvc += jvttool
        jetaugtool.JetJvtTool = jvttool

    #now do the same for the efftool, but this has an auto-config function
    if hasattr(ToolSvc,jvtefftoolname):
        jetaugtool.JetJvtEffTool = getattr(ToolSvc,jvtefftoolname)
        extjetlog.info('Setup the jvt eff tool {}'.format(jvtefftoolname))
    else:
        extjetlog.info('Setting up the jvt eff tool {}'.format(jvtefftoolname))
        jvtefftool = getJvtEffTool(jetalg)
        ToolSvc += jvtefftool
        jetaugtool.JetJvtEffTool = jvtefftool
        
    extjetlog.info('ExtendedJetCommon: Updating JVT for jet collection: '+jetalg+'Jets')
    applyJetAugmentation(jetalg,algname,sequence,jetaugtool)

def updateJVT_xAODColl(jetalg='AntiKt4EMTopo',sequence=DerivationFrameworkJob):
    supportedJets = ['AntiKt4EMTopo','AntiKt4EMPFlow']
    if not jetalg in supportedJets:
        extjetlog.warning('*** JVT update requested for unsupported jet collection {}! ***'.format(jetalg))
        return
    else:
        updateJVT(jetalg,'JetCommonKernel_{0}'.format(jetalg),sequence)

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

def applyBTaggingAugmentation(jetalg,algname='default',sequence=DerivationFrameworkJob,btagtooldict={}):
    if algname == 'default':
      algname = 'JetCommonKernel_{0}'.format(jetalg)
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

### Schedule Q/G-tagging decorations ### QGTaggerTool #####
def addQGTaggerTool(jetalg, sequence, algname ):
    jetaugtool = getJetAugmentationTool(jetalg)
    if(jetaugtool==None):
        extjetlog.warning('*** addQGTaggerTool called but corresponding augmentation tool does not exist! ***')

    QGTaggerToolName = 'DFQGTaggerTool' + '_InDetTrackSelectionTool_' + jetalg
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc, QGTaggerToolName):
        jetaugtool.TrackSelectionTool = getattr(ToolSvc, QGTaggerToolName)
    else:
        trackselectiontool = CfgMgr.InDet__InDetTrackSelectionTool( QGTaggerToolName )
        trackselectiontool.CutLevel = "Loose"
        ToolSvc += trackselectiontool
        jetaugtool.TrackSelectionTool = trackselectiontool

    extjetlog.info('ExtendedJetCommon: Adding QGTaggerTool for jet collection: '+jetalg)
    applyJetAugmentation(jetalg, algname, sequence, jetaugtool)
#############################################################

def applyOverlapRemoval(sequence=DerivationFrameworkJob):
    from AssociationUtils.config import recommended_tools
    from AssociationUtils.AssociationUtilsConf import OverlapRemovalGenUseAlg
    outputLabel = 'DFCommonJets_passOR'
    bJetLabel = '' #default
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
    algCleanLoose = EventCleaningTestAlg('EventCleaningTestAlg_Loose',
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
    algCleanTight = EventCleaningTestAlg('EventCleaningTestAlg_Tight',
                            EventCleaningTool=ecToolTight,
                            JetCollectionName="AntiKt4EMTopoJets",
			    EventCleanPrefix=prefix,
			    CleaningLevel="TightBad",
		  	    doEvent=False)
    sequence += algCleanTight

def eventCleanLooseLLP_xAODColl(jetalg='AntiKt4EMTopo',sequence=DerivationFrameworkJob):
    from JetSelectorTools.JetSelectorToolsConf import ECUtils__EventCleaningTool as EventCleaningTool
    from JetSelectorTools.JetSelectorToolsConf import EventCleaningTestAlg
    jetcleaningtoolname = "EventCleaningTool_LooseLLP"
    prefix = "DFCommonJets_"
    #Do not save decorations, which are anyway not listed in AntiKt4EMTopoJetsCPContent.py
    ecToolLooseLLP = EventCleaningTool('EventCleaningTool_LooseLLP',CleaningLevel='LooseBadLLP', DoDecorations=False)
    ecToolLooseLLP.JetCleanPrefix = prefix
    ecToolLooseLLP.JetCleaningTool = getJetCleaningTool("LooseBadLLP")
    algCleanLooseLLP = EventCleaningTestAlg('EventCleaningTestAlg_LooseLLP',
                            EventCleaningTool=ecToolLooseLLP,
                            JetCollectionName="AntiKt4EMTopoJets",
			    EventCleanPrefix=prefix,
			    CleaningLevel="LooseBadLLP",
		  	    doEvent=True) #Save the event level decoration
    sequence += algCleanLooseLLP

def eventCleanVeryLooseLLP_xAODColl(jetalg='AntiKt4EMTopo',sequence=DerivationFrameworkJob):
    from JetSelectorTools.JetSelectorToolsConf import ECUtils__EventCleaningTool as EventCleaningTool
    from JetSelectorTools.JetSelectorToolsConf import EventCleaningTestAlg
    jetcleaningtoolname = "EventCleaningTool_VeryLooseLLP"
    prefix = "DFCommonJets_"
    #Do not save decorations, which are anyway not listed in AntiKt4EMTopoJetsCPContent.py
    ecToolVeryLooseLLP = EventCleaningTool('EventCleaningTool_VeryLooseLLP',CleaningLevel='VeryLooseBadLLP')
    ecToolVeryLooseLLP.JetCleanPrefix = prefix
    ecToolVeryLooseLLP.JetCleaningTool = getJetCleaningTool("VeryLooseBadLLP")
    algCleanVeryLooseLLP = EventCleaningTestAlg('EventCleaningTestAlg_VeryLooseLLP',
                            EventCleaningTool=ecToolVeryLooseLLP,
                            JetCollectionName="AntiKt4EMTopoJets",
                EventCleanPrefix=prefix,
                CleaningLevel="VeryLooseBadLLP",
                doEvent=False) #Save the event level decoration
    sequence += algCleanVeryLooseLLP

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

def addConstModJets(jetalg,radius,inputtype,constmods,sequence,outputlist,
                    addGetters=None, **kwargs):
    extjetlog.info("Building jet collection with modifier sequence {0}".format(constmods))
    constmodstr = "".join(constmods)
    jetname = "{0}{1}{2}{3}Jets".format(jetalg,int(radius*10),constmodstr,inputtype)
    algname = "jetalg"+jetname

    # Avoid scheduling twice
    if hasattr(sequence,algname):
        extjetlog.warning("Sequence {0} already has an instance of const mod jet alg {1}".format(sequence,algname))
        return

    # Get the constituent mod sequence
    from JetRecTools import ConstModHelpers
    constmodseq = ConstModHelpers.getConstModSeq(constmods,inputtype)
    label = inputtype + constmodstr

    # Add the const mod sequence to the input preparation jetalg instance
    # Could add the event shape computation here
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()
    from JetRec.JetRecStandard import jtm
    if not hasattr(jtm,"jetconstit"+label):
        from JetRec.JetRecConf import JetToolRunner
        jetrun = JetToolRunner("jetconstit"+label,
                               EventShapeTools=[],
                               Tools=[constmodseq])
        jtm += jetrun
        # Add this tool runner to the JetAlgorithm instance "jetalg"
        # which runs all preparatory tools
        # This was added by JetCommon
        job.jetalg.Tools.append(jetrun)
        extjetlog.info("Added const mod sequence {0} to \'jetalg\'".format(constmodstr))

    # Get the PseudoJetGetter
    pjname = label.lower().replace("topo","")
    pjg = ConstModHelpers.getPseudoJetGetter(label,pjname)
    # Generate the getter list including ghosts from that for the standard list for the input type
    getterbase = inputtype.lower()
    getters = [pjg]+list(jtm.gettersMap[getterbase])[1:]
    if addGetters:
        getters += addGetters

    # Pass the configuration to addStandardJets
    # The modifiers will be taken from the
    jetfindargs = {"jetalg":        jetalg,
                   "rsize":         radius,
                   "inputtype":     inputtype,
                   "customGetters": getters,
                   "namesuffix":    constmodstr,
                   "algseq":        sequence,
                   "outputGroup":   outputlist
                   }
    jetfindargs.update(kwargs)

    addStandardJets(**jetfindargs)

def addCSSKSoftDropJets(sequence, seq_name, logger=extjetlog):
    vrJetName, vrGhostLabel = buildVRJets(
        sequence, do_ghost=True, logger=logger)

    linkVRJetsToLargeRJets(sequence, vrJetName, vrGhostLabel)

    addConstModJets("AntiKt", 1.0, "LCTopo", ["CS", "SK"], sequence, seq_name,
                    ptmin=40000, ptminFilter=50000, mods="lctopo_ungroomed",
                    addGetters=[vrGhostLabel.lower()])
    addSoftDropJets("AntiKt", 1.0, "LCTopo", beta=1.0, zcut=0.1,
                    algseq=sequence, outputGroup=seq_name,
                    writeUngroomed=True, mods="lctopo_groomed",
                    constmods=["CS", "SK"])

##################################################################
applyJetCalibration_xAODColl("AntiKt4EMTopo")
updateJVT_xAODColl("AntiKt4EMTopo")
applyBTagging_xAODColl("AntiKt4EMTopo")
applyOverlapRemoval()
eventCleanLoose_xAODColl("AntiKt4EMTopo")
eventCleanTight_xAODColl("AntiKt4EMTopo")
eventCleanVeryLooseLLP_xAODColl("AntiKt4EMTopo")

##################################################################
# Helper to add origin corrected clusters
##################################################################
def addOriginCorrectedClusters(slimhelper,writeLC=False,writeEM=False):

    slimhelper.ExtraVariables.append('CaloCalTopoClusters.calE.calEta.calPhi.calM')

    if writeLC:
        if not slimhelper.AppendToDictionary.has_key("LCOriginTopoClusters"):
            slimhelper.AppendToDictionary["LCOriginTopoClusters"]='xAOD::CaloClusterContainer'
            slimhelper.AppendToDictionary["LCOriginTopoClustersAux"]='xAOD::ShallowAuxContainer'
            slimhelper.ExtraVariables.append('LCOriginTopoClusters.calEta.calPhi')

    if writeEM:
        if not slimhelper.AppendToDictionary.has_key("EMOriginTopoClusters"):
            slimhelper.AppendToDictionary["EMOriginTopoClusters"]='xAOD::CaloClusterContainer'
            slimhelper.AppendToDictionary["EMOriginTopoClustersAux"]='xAOD::ShallowAuxContainer'
            slimhelper.ExtraVariables.append('EMOriginTopoClusters.calE.calEta.calPhi')

##################################################################
# Helper to manually schedule PFO constituent modifications
# Only use this while the automatic addition in JetAlgorithm.py
# is disabled
##################################################################
def addCHSPFlowObjects():
    # Only act if the collection does not already exist
    from RecExConfig.AutoConfiguration import IsInInputFile
    if not IsInInputFile("xAOD::PFOContainer","CHSParticleFlowObjects"):
        # Check that an alg doing this has not already been inserted
        from AthenaCommon.AlgSequence import AlgSequence
        job = AlgSequence()
        from JetRec.JetRecStandard import jtm
        if not hasattr(job,"jetalgCHSPFlow") and not hasattr(jtm,"jetconstitCHSPFlow"):
            from JetRec.JetRecConf import JetToolRunner
            jtm += JetToolRunner("jetconstitCHSPFlow",
                                 EventShapeTools=[],
                                 Tools=[jtm.JetConstitSeq_PFlowCHS])
            # Add this tool runner to the JetAlgorithm instance "jetalg"
            # which runs all preparatory tools
            # This was added by JetCommon
            job.jetalg.Tools.append(jtm.jetconstitCHSPFlow)
            extjetlog.info("Added CHS PFlow sequence to \'jetalg\'")
            extjetlog.info(job.jetalg.Tools)

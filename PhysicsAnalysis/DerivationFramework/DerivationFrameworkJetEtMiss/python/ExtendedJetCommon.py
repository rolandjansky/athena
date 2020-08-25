# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# ExtendedJetCommon.py
# Schedules special jet tools
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from JetRec.JetRecFlags import jetFlags
from JetJvtEfficiency.JetJvtEfficiencyToolConfig import (getJvtEffTool, getJvtEffToolName)

from AthenaCommon import Logging
extjetlog = Logging.logging.getLogger('ExtendedJetCommon')

def nameJetsFromAlg(alg):
    """ Name a jet collection from its algorithm
        
        The jet code likes to pass around the algorithm name (e.g.
        AntiKt4EMTopo) rather than the jet collection. This was previously fine
        as all that was needed to do was to append 'Jets' to the name, however
        now that we have date-stamped b-tagging containers, this simple rule is
        not so simple...
    """
    if "_BTagging" in alg:
        return alg.replace("_BTagging", "Jets_BTagging")
    else:
        return alg+"Jets"


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

def addCSSKSoftDropJets(sequence, seq_name, logger=extjetlog):
    vrJetName, vrGhostLabel = buildVRJets(
        sequence, do_ghost=True, logger=logger)

    addVRJets(sequence, do_ghost=True, logger=logger)

    addConstModJets("AntiKt", 1.0, "LCTopo", ["CS", "SK"], sequence, seq_name,
                    ptmin=40000, ptminFilter=50000, mods="lctopo_ungroomed",
                    addGetters=[vrGhostLabel.lower()])
    addSoftDropJets("AntiKt", 1.0, "LCTopo", beta=1.0, zcut=0.1,
                    algseq=sequence, outputGroup=seq_name,
                    writeUngroomed=True, mods="lctopo_groomed",
                    constmods=["CS", "SK"])

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

def addAntiKt2PV0TrackJets(sequence, outputlist):
    
    #To be fixed by FTAG
    #btag_akt2trk = ConfInst.setupJetBTaggerTool(ToolSvc, JetCollection="AntiKt2Track", AddToToolSvc=True,
    #                                            Verbose=True,
    #                                            options={"name"         : "btagging_antikt2track",
    #                                                     "BTagName"     : "BTagging_AntiKt2Track",
    #                                                     "BTagJFVtxName": "JFVtx",
    #                                                     "BTagSVName"   : "SecVtx",
    #                                                     },
    #                                            SetupScheme = "",
    #                                            TaggerList = ['IP2D', 'IP3D', 'MultiSVbb1',  'MultiSVbb2', 'SV1', 'JetFitterNN', 'SoftMu', 'MV2c10', 'MV2c10mu', 'MV2c10rnn', 'JetVertexCharge', 'MV2cl100' , 'MVb', 'DL1', 'DL1rnn', 'DL1mu', 'RNNIP', 'MV2c10Flip']
    #)
    #jtm.modifiersMap["akt2track"] = jtm.modifiersMap["track_ungroomed"] + [btag_akt2trk]
    jtm.modifiersMap["akt2track"] = jtm.modifiersMap["track_ungroomed"]
    addStandardJets("AntiKt", 0.2, "PV0Track", ptmin=2000, mods="akt2track",
                    algseq=sequence, outputGroup=outputlist)

def addAntiKt4PV0TrackJets(sequence, outputlist):
    addStandardJets("AntiKt", 0.4, "PV0Track", ptmin=2000, mods="track_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt10PV0TrackJets(sequence, outputlist):
    addStandardJets("AntiKt", 1.0, "PV0Track", ptmin=2000, ptminFilter=40000, mods="track_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt2TruthJets(sequence,outputlist):
    if DerivationFrameworkIsMonteCarlo:
        addStandardJets("AntiKt", 0.2, "Truth", ptmin=5000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt4TruthJets(sequence,outputlist):
    if DerivationFrameworkIsMonteCarlo:
        addStandardJets("AntiKt", 0.4, "Truth", ptmin=5000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt4TruthWZJets(sequence,outputlist):
    if DerivationFrameworkIsMonteCarlo:
        addStandardJets("AntiKt", 0.4, "TruthWZ", ptmin=5000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt4TruthDressedWZJets(sequence,outputlist):
    if DerivationFrameworkIsMonteCarlo:
        addStandardJets("AntiKt", 0.4, "TruthDressedWZ", ptmin=5000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt10TruthJets(sequence,outputlist):
    if DerivationFrameworkIsMonteCarlo:
        addStandardJets("AntiKt", 1.0, "Truth", ptmin=40000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt10TruthWZJets(sequence,outputlist):
    if DerivationFrameworkIsMonteCarlo:
        addStandardJets("AntiKt", 1.0, "TruthWZ", ptmin=40000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

##################################################################  

def replaceAODReducedJets(jetlist,sequence,outputlist):
    extjetlog.info( "Replacing AOD-reduced jet collections: {0}".format(",".join(jetlist)))
    if "AntiKt2PV0TrackJets" in jetlist:
        addAntiKt2PV0TrackJets(sequence,outputlist)
    if "AntiKt4PV0TrackJets" in jetlist:
        addAntiKt4PV0TrackJets(sequence,outputlist)
    if "AntiKt10PV0TrackJets" in jetlist:
        addAntiKt10PV0TrackJets(sequence,outputlist)
    if "AntiKt2TruthJets" in jetlist:
        addAntiKt2TruthJets(sequence,outputlist)
    if "AntiKt4TruthJets" in jetlist:
        addAntiKt4TruthJets(sequence,outputlist)
    if "AntiKt4TruthWZJets" in jetlist:
        addAntiKt4TruthWZJets(sequence,outputlist)
    if "AntiKt10TruthJets" in jetlist:
        addAntiKt10TruthJets(sequence,outputlist)
    if "AntiKt10TruthWZJets" in jetlist:
        addAntiKt10TruthWZJets(sequence,outputlist)
    if "AntiKt2LCTopoJets" in jetlist:
        addAntiKt2LCTopoJets(sequence,outputlist)
    if "AntiKt10LCTopoJets" in jetlist:
        addAntiKt10LCTopoJets(sequence,outputlist)

##################################################################
# Jet helpers for adding low-pt jets needed for calibration 
##################################################################

# 2 GeV cut after pileup suppression for in-situ Z
def addAntiKt4LowPtJets(sequence,outputlist):
    addStandardJets("AntiKt", 0.4, "EMTopo",  namesuffix="LowPt", ptmin=2000, ptminFilter=2000,
                    mods="emtopo_ungroomed", algseq=sequence, outputGroup=outputlist,calibOpt="ar")
    addStandardJets("AntiKt", 0.4, "LCTopo",  namesuffix="LowPt", ptmin=2000, ptminFilter=2000,
                    mods="lctopo_ungroomed", algseq=sequence, outputGroup=outputlist,calibOpt="ar")

    addCHSPFlowObjects()
    addStandardJets("AntiKt", 0.4, "EMPFlow", namesuffix="LowPt", ptmin=2000, ptminFilter=2000,
                    mods="pflow_ungroomed", algseq=sequence, outputGroup=outputlist,calibOpt="ar:pflow")

################################################################## 

# 1 MeV cut at constituent level for MCJES
def addAntiKt4NoPtCutJets(sequence,outputlist):
    addStandardJets("AntiKt", 0.4, "EMTopo",  namesuffix="NoPtCut", ptmin=0, ptminFilter=1,
                    mods="emtopo_ungroomed", algseq=sequence, outputGroup=outputlist,calibOpt="none")
    addStandardJets("AntiKt", 0.4, "LCTopo",  namesuffix="NoPtCut", ptmin=0, ptminFilter=1,
                    mods="lctopo_ungroomed", algseq=sequence, outputGroup=outputlist,calibOpt="none")
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
        inJets = jetalg+'Jets'
        if '_BTagging' in jetalg:
          inJets = jetalg.replace('_BTagging','Jets_BTagging')
        jetaugtool = CfgMgr.DerivationFramework__JetAugmentationTool(jetaugtoolname,
                                                                     InputJets=inJets)
        ToolSvc += jetaugtool

    return jetaugtool

##################################################################

def getJetExternalAssocTool(jetalg, extjetalg, **options):
    jetassoctoolname = 'DFJetExternalAssoc_%s_From_%s' % (jetalg, extjetalg)
    jetassoctool = None
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,jetassoctoolname):
        jetassoctool = getattr(ToolSvc,jetassoctoolname)
    else:
        extjetname = extjetalg + 'Jets' if 'BTagging' not in extjetalg else extjetalg.replace('_BTagging','Jets_BTagging')
        jetassoctool = CfgMgr.DerivationFramework__JetExternalAssocTool(jetassoctoolname,
                                                                        InputJets=jetalg+'Jets',
                                                                        ExternalJetCollectionName = extjetname,
                                                                        **options)
        ToolSvc += jetassoctool

    return jetassoctool

##################################################################
# Calibration related functions 
##################################################################

def applyJetCalibration(jetalg,algname,sequence,largeRjetconfig = 'comb', suffix = ''):

    calibtoolname = 'DFJetCalib_'+jetalg
    jetaugtool = getJetAugmentationTool(jetalg, suffix)

    rhoKey = 'auto'
    if '_BTagging' in jetalg:
        jetalg_basename = jetalg[:jetalg.find('_BTagging')]
    elif 'PFlowCustomVtx' in jetalg:
        jetalg_basename = 'AntiKt4EMPFlow'
        rhoKey = 'Kt4PFlowCustomVtxEventShape'
    else:
        jetalg_basename = jetalg

    from AthenaCommon.AppMgr import ToolSvc

    from LumiBlockComps.LumiBlockMuWriterDefault import LumiBlockMuWriterDefault
    LumiBlockMuWriterDefault()

    if hasattr(ToolSvc,calibtoolname):
        jetaugtool.JetCalibTool = getattr(ToolSvc,calibtoolname)
    else:
        isdata=False

        #largeRconfig selects config file for AntiKt10LCTopoTrimmedPtFrac5SmallR20, default is JES_MC16recommendation_FatJet_JMS_comb_19Jan2018.config
        if not largeRjetconfig in ['comb','calo','TA']:
            extjetlog.warning('*** Wrong value for fatjetconfig!  Only \'comb\' (default), \'calo\' or \'TA\' can be used. ***')

        #Warning: these are quite outdated ... leaving for validation purposes for now
        configdict = {'AntiKt4EMTopo':('JES_data2016_data2015_Recommendation_Dec2016_rel21.config',
                                       'JetArea_Residual_EtaJES_GSC'),
                      'AntiKt4LCTopo':('JES_data2016_data2015_Recommendation_Dec2016_rel21.config',
                                       'JetArea_Residual_EtaJES_GSC'),
                      'AntiKt4EMPFlow':('JES_MC16Recommendation_Consolidated_PFlow_Apr2019_Rel21.config',
                                        'JetArea_Residual_EtaJES_GSC_Smear'),
                      'AntiKt10LCTopoTrimmedPtFrac5SmallR20':('JES_MC15recommendation_FatJet_Nov2016_QCDCombinationUncorrelatedWeights.config',
                                                              'EtaJES_JMS'),
                      'AntiKt2LCTopo':('JES_2015_2016_data_Rscan2LC_18Dec2018_R21.config',
                                       'JetArea_Residual_EtaJES_GSC'),
                      }

        if largeRjetconfig=='calo': #Choose JES_MC16recommendation_FatJet_JMS_calo_29Nov2017.config for AntiKt10LCTopoTrimmedPtFrac5SmallR20
            configdict.update({'AntiKt10LCTopoTrimmedPtFrac5SmallR20':('JES_MC16recommendation_FatJet_JMS_calo_29Nov2017.config',
                                                                       'EtaJES_JMS')
                              })
        if largeRjetconfig=='TA': #Choose JES_MC16recommendation_FatJet_JMS_TA_29Nov2017.config for AntiKt10LCTopoTrimmedPtFrac5SmallR20
            configdict.update({'AntiKt10LCTopoTrimmedPtFrac5SmallR20':('JES_MC16recommendation_FatJet_JMS_TA_29Nov2017.config',
                                                                       'EtaJES_JMS')
                              })
        isMC = DerivationFrameworkIsMonteCarlo
        isAF2 = False
        if isMC:
            isAF2 = 'ATLFASTII' in inputFileSummary['metadata']['/Simulation/Parameters']['SimulationFlavour'].upper()
        if isMC and isAF2:
            ## Warning: these are quite outdated ... leaving for validation purposes for now
            configdict['AntiKt4EMTopo'] = ('JES_MC15Prerecommendation_AFII_June2015.config',
                                           'JetArea_Residual_EtaJES_GSC')
            configdict['AntiKt4EMPFlow'] = ('JES_MC16Recommendation_AFII_PFlow_Apr2019_Rel21.config',
                                            'JetArea_Residual_EtaJES_GSC_Smear')

        config,calibseq = configdict[jetalg_basename]

        if (not isMC) and jetalg_basename in ['AntiKt4EMTopo','AntiKt4LCTopo','AntiKt4EMPFlow']:
            isdata=True
            if not jetalg_basename=='AntiKt4LCTopo': calibseq = calibseq+'_Insitu'

        calibtool = CfgMgr.JetCalibrationTool(
            calibtoolname,
            JetCollection=jetalg_basename,
            RhoKey=rhoKey,
            ConfigFile=config,
            CalibSequence=calibseq,
            IsData=isdata
            )
        ToolSvc += calibtool
        jetaugtool.JetCalibTool = calibtool

    extjetlog.info('Applying calibration to jet collection: '+jetalg+'Jets')
    applyJetAugmentation(jetalg,algname,sequence,jetaugtool)

##################################################################

def applyJetCalibration_xAODColl(jetalg='AntiKt4EMTopo',sequence=DerivationFrameworkJob):
    supportedJets = ['AntiKt4EMTopo','AntiKt4LCTopo','AntiKt4EMPFlow','AntiKt4EMTopo_BTagging201810','AntiKt4EMPFlow_BTagging201810']
    if not jetalg in supportedJets:
        extjetlog.warning('*** Calibration requested for unsupported jet collection '+jetalg+'! ***')
        return
    else:
        applyJetCalibration(jetalg,'JetCommonKernel_{0}'.format(jetalg),sequence)

##################################################################

def applyJetCalibration_CustomColl(jetalg='AntiKt10LCTopoTrimmedPtFrac5SmallR20',sequence=None):
    supportedJets = ['AntiKt10LCTopoTrimmedPtFrac5SmallR20','AntiKt2LCTopo']
    if not jetalg in supportedJets:
        extjetlog.warning('*** Calibration requested for unsupported jet collection! ***')
        extjetlog.warning('Supported custom jets: '+supportedJets)
        return
    else:
        applyJetCalibration(jetalg,'JetCommonKernel_{0}'.format(jetalg),sequence)

##################################################################
# JVT
##################################################################

def updateJVT(jetalg,algname,sequence, suffix = '',customVxColl = 'PrimaryVertices'):
    jetaugtool = getJetAugmentationTool(jetalg, suffix)
    if(jetaugtool==None or jetaugtool.JetCalibTool==''):
        extjetlog.warning('*** JVT update called but corresponding augmentation tool does not exist! ***')
        extjetlog.warning('*** You must apply jet calibration before scheduling JVT! ***')

    jvttoolname = 'DFJetJvt_'+jetalg

    pvxName = customVxColl

    if '_BTagging' in jetalg:
        jetalg_basename = jetalg[:jetalg.find('_BTagging')]
    elif 'PFlowCustomVtx' in jetalg:
        jetalg_basename = 'AntiKt4EMPFlow'
    else:
        jetalg_basename = jetalg

    #use the standard name defined by the config helper
    jvtefftoolname = getJvtEffToolName(jetalg_basename)

    from AthenaCommon.AppMgr import ToolSvc

    #setup the jvt updating tools if not already done
    if hasattr(ToolSvc,jvttoolname):
        jetaugtool.JetJvtTool = getattr(ToolSvc,jvttoolname)
    else:
        jvttool = CfgMgr.JetVertexTaggerTool(jvttoolname,JetContainer=jetalg_basename+'Jets',VertexContainer=pvxName)
        ToolSvc += jvttool
        jetaugtool.JetJvtTool = jvttool

    #now do the same for the efftool, but this has an auto-config function                                                                                                                                 
    
    if hasattr(ToolSvc,jvtefftoolname):
        jetaugtool.JetJvtEffTool = getattr(ToolSvc,jvtefftoolname)
        extjetlog.info('Setup the jvt eff tool {}'.format(jvtefftoolname))
    else:
        extjetlog.info('Setting up the jvt eff tool {}'.format(jvtefftoolname))
        jvtefftool = getJvtEffTool(jetalg_basename)
        ToolSvc += jvtefftool
        jetaugtool.JetJvtEffTool = jvtefftool

    extjetlog.info('ExtendedJetCommon: Updating JVT for jet collection: '+jetalg+'Jets')
    applyJetAugmentation(jetalg,algname,sequence,jetaugtool)

################################################################## 

def updateJVT_xAODColl(jetalg='AntiKt4EMTopo',sequence=DerivationFrameworkJob):
    supportedJets = ['AntiKt4EMTopo','AntiKt4EMPFlow','AntiKt4EMTopo_BTagging201810','AntiKt4EMPFlow_BTagging201810']
    if not jetalg in supportedJets:
        extjetlog.warning('*** JVT update requested for unsupported jet collection {}! ***'.format(jetalg))
        return
    else:
        updateJVT(jetalg,'JetCommonKernel_{0}'.format(jetalg),sequence)

##################################################################

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

##################################################################

def addJetTruthLabel(jetalg,algname,labelname,sequence):
    supportedLabelNames = ['R10TruthLabel_R21Consolidated']
    supportedTruthJets = ['AntiKt10TruthTrimmedPtFrac5SmallR20']
    supportedRecoJets = ['AntiKt10LCTopoTrimmedPtFrac5SmallR20','AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20','AntiKt10UFOCSSKTrimmedPtFrac5SmallR20','AntiKt10UFOCSSKSoftDropBeta100Zcut10','AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5','AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5Ninf','AntiKt10UFOCHSTrimmedPtFrac5SmallR20']
    supportedJets = supportedRecoJets + supportedTruthJets
    if not jetalg in supportedJets:
        extjetlog.warning('*** JetTruthLabeling augmentation requested for unsupported jet collection {}! ***'.format(jetalg))
        return
    elif not labelname in supportedLabelNames:
        extjetlog.warning('*** JetTruthLabeling augmentation requested for unsupported label definition {}! ***'.format(labelname))
        return
    else:
        isTruthJet = False
        if jetalg in supportedTruthJets:
            isTruthJet = True

        jetaugtool = getJetAugmentationTool(jetalg)

        if(jetaugtool==None):
            extjetlog.warning('*** addJetTruthLabel called but corresponding augmentation tool does not exist! ***')
            return

        jettruthlabeltoolname = 'DFJetTruthLabel_'+jetalg+'_'+labelname

        from AthenaCommon.AppMgr import ToolSvc

        if hasattr(ToolSvc,jettruthlabeltoolname):
            jetaugtool.JetTruthLabelingTool = getattr(ToolSvc,jettruthlabeltoolname)
        else:
            jettruthlabeltool = CfgMgr.JetTruthLabelingTool(jettruthlabeltoolname,IsTruthJetCollection=isTruthJet,TruthLabelName=labelname)
            ToolSvc += jettruthlabeltool
            jetaugtool.JetTruthLabelingTool = jettruthlabeltool

        extjetlog.info('ExtendedJetCommon: Applying JetTruthLabel augmentation to jet collection: ' + jetalg + 'Jets' + ' using ' + labelname +' definition')
        applyJetAugmentation(jetalg,algname,sequence,jetaugtool)

################################################################## 

def applyBTaggingAugmentation(jetalg,algname='default',sequence=DerivationFrameworkJob,btagtooldict={}):
    if algname == 'default':
      algname = 'JetCommonKernel_{0}'.format(jetalg)
    jetaugtool = getJetAugmentationTool(jetalg)

    if(jetaugtool==None or jetaugtool.JetCalibTool=='' or jetaugtool.JetJvtTool==''):
        extjetlog.warning('*** B-tagging called but corresponding augmentation tool does not exist! ***')
        extjetlog.warning('*** You must apply jet calibration and JVT! ***')

    btagWPs = []
    btagtools = []
    for WP,tool in sorted(btagtooldict.items()):
        btagWPs.append(WP)
        btagtools.append(tool)
    jetaugtool.JetBtagTools = btagtools
    jetaugtool.JetBtagWPs = btagWPs

    inJets = jetalg+'Jets'
    if '_BTagging' in jetalg:
      inJets = jetalg.replace('_BTagging','Jets_BTagging')
    extjetlog.info('ExtendedJetCommon: Applying b-tagging working points for jet collection: '+inJets)
    applyJetAugmentation(jetalg,algname,sequence,jetaugtool)

#################################################################
### Schedule Q/G-tagging decorations ### QGTaggerTool ##### 
#################################################################
def addQGTaggerTool(jetalg, sequence, algname, truthjetalg=None ):
    jetaugtool = getJetAugmentationTool(jetalg,'_qgTag')
    if(jetaugtool==None):
        extjetlog.warning('*** addQGTaggerTool called but corresponding augmentation tool does not exist! ***')

    from AthenaCommon.AppMgr import ToolSvc

    if truthjetalg!=None:
        jetptassociationtoolname = 'DFJetPtAssociation_'+truthjetalg+'_'+jetalg
        if hasattr(ToolSvc,jetptassociationtoolname):
            jetaugtool.JetPtAssociationTool = getattr(ToolSvc,jetptassociationtoolname)
        else:
            jetptassociationtool = CfgMgr.JetPtAssociationTool(jetptassociationtoolname, InputContainer=truthjetalg, AssociationName="GhostTruth")
            ToolSvc += jetptassociationtool
            jetaugtool.JetPtAssociationTool = jetptassociationtool

    #Track selection tool
    TrackSelectionToolName = 'DFQGTaggerTool' + '_InDetTrackSelectionTool_' + jetalg
    if hasattr(ToolSvc, TrackSelectionToolName):
        jetaugtool.TrackSelectionTool = getattr(ToolSvc, TrackSelectionToolName)
    else:
        trackselectiontool = CfgMgr.InDet__InDetTrackSelectionTool( TrackSelectionToolName )
        trackselectiontool.CutLevel = "Loose"
        ToolSvc += trackselectiontool
        jetaugtool.TrackSelectionTool = trackselectiontool
    
    #Track-vertex association tool
    TrackVertexAssociationToolName = 'DFQGTaggerTool' + '_InDetTrackVertexAssosciationTool_' + jetalg
    if hasattr(ToolSvc, TrackVertexAssociationToolName):
        jetaugtool.TrackVertexAssociationTool = getattr(ToolSvc, TrackVertexAssociationToolName)
    else:
        trackvertexassoctool = CfgMgr.CP__TrackVertexAssociationTool(TrackVertexAssociationToolName)
        trackvertexassoctool.WorkingPoint = "Loose"
        ToolSvc += trackvertexassoctool
        jetaugtool.TrackVertexAssociationTool = trackvertexassoctool

    QGTaggerToolName = 'DFQGTaggerTool_' + jetalg
    pvxName = "PrimaryVertices"

    #calculate variables for q/g tagging
    if hasattr(ToolSvc,QGTaggerToolName):
        jetaugtool.JetQGTaggerTool = getattr(ToolSvc,QGTaggerToolName)
    else:
        qgtool = CfgMgr.JetQGTaggerVariableTool(QGTaggerToolName,JetContainer=jetalg+'Jets',VertexContainer=pvxName,TVATool=trackvertexassoctool,TrkSelTool=trackselectiontool)
        ToolSvc += qgtool
        jetaugtool.JetQGTaggerTool = qgtool

    extjetlog.info('ExtendedJetCommon: Adding QGTaggerTool for jet collection: '+jetalg)
    applyJetAugmentation(jetalg, algname, sequence, jetaugtool)

################################################################## 

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

    from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__MuonJetDrTool
    MuonJetDrTool = DerivationFramework__MuonJetDrTool( name = "MuonJetDrTool")
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += MuonJetDrTool
    DFCommonMuonJetTools = []
    DFCommonMuonJetTools.append(MuonJetDrTool)
    sequence += CfgMgr.DerivationFramework__CommonAugmentation("DFCommonMuonsKernel2",AugmentationTools = DFCommonMuonJetTools)

################################################################## 
# Jet cleaning tool
################################################################## 

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

##################################################################
# Event cleaning variables
################################################################## 
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

##################################################################  

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

##################################################################  

def eventCleanLooseLLP_xAODColl(jetalg='AntiKt4EMTopo',sequence=DerivationFrameworkJob):
    from JetSelectorTools.JetSelectorToolsConf import ECUtils__EventCleaningTool as EventCleaningTool
    from JetSelectorTools.JetSelectorToolsConf import EventCleaningTestAlg
    jetcleaningtoolname = "EventCleaningTool_LooseLLP"
    prefix = "DFCommonJets_"
    ecToolLooseLLP = EventCleaningTool('EventCleaningTool_LooseLLP',CleaningLevel='LooseBadLLP')
    ecToolLooseLLP.JetCleanPrefix = prefix
    ecToolLooseLLP.JetCleaningTool = getJetCleaningTool("LooseBadLLP")
    algCleanLooseLLP = EventCleaningTestAlg('EventCleaningTestAlg_LooseLLP',
                            EventCleaningTool=ecToolLooseLLP,
                            JetCollectionName="AntiKt4EMTopoJets",
                            EventCleanPrefix=prefix,
                            CleaningLevel="LooseBadLLP",
                            doEvent=True) #Save the event level decoration
    sequence += algCleanLooseLLP

##################################################################  

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

################################################################## 

def eventCleanSuperLooseLLP_xAODColl(jetalg='AntiKt4EMTopo',sequence=DerivationFrameworkJob):
    from JetSelectorTools.JetSelectorToolsConf import ECUtils__EventCleaningTool as EventCleaningTool
    from JetSelectorTools.JetSelectorToolsConf import EventCleaningTestAlg
    jetcleaningtoolname = "EventCleaningTool_SuperLooseLLP"
    prefix = "DFCommonJets_"
    #Do not save decorations, which are anyway not listed in AntiKt4EMTopoJetsCPContent.py
    ecToolSuperLooseLLP = EventCleaningTool('EventCleaningTool_SuperLooseLLP',CleaningLevel='SuperLooseBadLLP')
    ecToolSuperLooseLLP.JetCleanPrefix = prefix
    ecToolSuperLooseLLP.JetCleaningTool = getJetCleaningTool("SuperLooseBadLLP")
    algCleanSuperLooseLLP = EventCleaningTestAlg('EventCleaningTestAlg_SuperLooseLLP',
                            EventCleaningTool=ecToolSuperLooseLLP,
                            JetCollectionName="AntiKt4EMTopoJets",
                EventCleanPrefix=prefix,
                CleaningLevel="SuperLooseBadLLP",
                doEvent=False) #Save the event level decoration
    sequence += algCleanSuperLooseLLP

################################################################## 

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

def addConstModJets(jetalg,radius,inputtype,constmods,sequence,outputlist,customVxColl="",
                    addGetters=None, **kwargs):

    if len(constmods)>0:
        extjetlog.info("Building jet collection with modifier sequence {0}".format(constmods))
    if customVxColl:
        extjetlog.info("Building jet collection with custom vx collection {0}".format(customVxColl))

    constmodstr = "".join(constmods)
    if customVxColl and "CustomVtx" not in inputtype:
        inputtype=inputtype+"CustomVtx"
    jetname = "{0}{1}{2}{3}{4}Jets".format(jetalg,int(radius*10),constmodstr,inputtype,customVxColl)
    algname = "jetalg"+jetname

    # Avoid scheduling twice
    if hasattr(sequence,algname):
        extjetlog.warning("Sequence {0} already has an instance of const mod jet alg {1}".format(sequence,algname))
        return

    from JetRecConfig import ConstModHelpers
    from JetRecConfig.JetDefinition import xAODType, JetConstit

    if inputtype == "EMTopo":
        constit = JetConstit( xAODType.CaloCluster, ["EM","Origin"])
    elif inputtype == "LCTopo":
        constit = JetConstit( xAODType.CaloCluster, ["LC","Origin"])
    elif inputtype == "EMPFlow":
        constit = JetConstit( xAODType.ParticleFlow )

    constit.modifiers += constmods

    constitalg = ConstModHelpers.getConstitModAlg(constit)
    if not hasattr(sequence, constitalg.name()):
        sequence += constitalg

    # Get the PseudoJetGetter
    from JetRecConfig import JetRecConfig
    constitpjalg = JetRecConfig.getConstitPJGAlg( constit )
    if not hasattr(sequence,constitpjalg.name()):
        sequence += constitpjalg

    getterbase = inputtype.lower()
    if inputtype == "PFlowCustomVtx": getterbase = "empflow_reduced"

    getters = [constitpjalg]+list(jtm.gettersMap[getterbase])[1:]

    if addGetters:
        getters += addGetters

    suffix = customVxColl+constmodstr

    # Pass the configuration to addStandardJets
    # The modifiers will be taken from the
    jetfindargs = {"jetalg":        jetalg,
                   "rsize":         radius,
                   "inputtype":     inputtype,
                   "customGetters": getters,
                   "namesuffix":    suffix,
                   "algseq":        sequence,
                   "outputGroup":   outputlist
                   }
    jetfindargs.update(kwargs)

    addStandardJets(**jetfindargs)

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
##################################################################
applyJetCalibration_xAODColl("AntiKt4EMTopo")
updateJVT_xAODColl("AntiKt4EMTopo")

# Need DFCommonElectronsLHLoose for these to work
#applyOverlapRemoval()
#eventCleanLoose_xAODColl("AntiKt4EMTopo")
#eventCleanTight_xAODColl("AntiKt4EMTopo")
#eventCleanLooseLLP_xAODColl("AntiKt4EMTopo")
#eventCleanSuperLooseLLP_xAODColl("AntiKt4EMTopo")
#eventCleanVeryLooseLLP_xAODColl("AntiKt4EMTopo")

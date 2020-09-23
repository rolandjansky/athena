# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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
from DerivationFrameworkFlavourTag.HbbCommon import (buildVRJets, addVRJets)
from JetRec.JetRecFlags import jetFlags
from JetJvtEfficiency.JetJvtEfficiencyToolConfig import (getJvtEffTool, getJvtEffToolName)

from AthenaCommon import Logging
extjetlog = Logging.logging.getLogger('ExtendedJetCommon')

# for debugging output
from AthenaCommon.Constants import INFO,DEBUG,WARNING

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
    if DerivationFrameworkHasTruth and dotruth:
        addTrimmedJets('AntiKt', 1.0, 'Truth', rclus=0.2, ptfrac=0.05, mods="truth_groomed",
                       algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed)
    addTrimmedJets('AntiKt', 1.0, 'LCTopo', rclus=0.2, ptfrac=0.05, mods="lctopo_groomed",
                   algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed)

def addTCCTrimmedJets(sequence,outputlist,dotruth=True,writeUngroomed=False):
    addTrimmedJets('AntiKt', 1.0, 'TrackCaloCluster', rclus=0.2, ptfrac=0.05, mods="tcc_groomed",
                   algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed)


##################################################################
# Helpers for UFOs
##################################################################

def addUFOs(sequence,outputlist,doCHS=False):
    addCHSPFlowObjects()

    ## Add PFlow constituents
    from JetRecTools.ConstModHelpers import getConstModSeq, xAOD
    pflowCSSKSeq = getConstModSeq(["CS","SK"], "EMPFlow")

    job = AlgSequence()
    if pflowCSSKSeq.getFullName() not in [t.getFullName() for t in job.jetalg.Tools]:
        job.jetalg.Tools += [pflowCSSKSeq]

    # Add UFO constituents
    from TrackCaloClusterRecTools.TrackCaloClusterConfig import runUFOReconstruction
    ufocsskAlg = runUFOReconstruction(sequence, ToolSvc, PFOPrefix="CSSK", caloClusterName="LCOriginTopoClusters")
    if doCHS:
        ufoAlg = runUFOReconstruction(sequence, ToolSvc, PFOPrefix="CHS", caloClusterName="LCOriginTopoClusters")

##################################################################
# Ungroomed large-R UFO CSSK jets
##################################################################

def addDefaultUFOJets(sequence,outputlist,doCHS=False):
    addUFOs(sequence,outputlist,doCHS=doCHS)
    addAntiKt10UFOCSSKJets(sequence,outputlist)
    if doCHS:
        addAntiKt10UFOCHSJets(sequence,outputlist)

##################################################################
# Precisionm recommendation large-R jets
##################################################################

def addDefaultUFOSoftDropJets(sequence,outputlist,dotruth=True,writeUngroomed=False):

    if DerivationFrameworkHasTruth and dotruth:
        addSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.1, mods="truth_groomed",
                        algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed)
    
    addDefaultUFOJets(sequence,outputlist)

    addSoftDropJets('AntiKt', 1.0, 'UFOCSSK', beta=1.0, zcut=0.1, mods="tcc_groomed", 
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

def addAntiKt2LCTopoJets(sequence, outputlist):
    addStandardJets("AntiKt", 0.2, "LCTopo", ptmin=2000, ptminFilter=7000, mods="lctopo_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt10LCTopoJets(sequence, outputlist):
    addStandardJets("AntiKt", 1.0, "LCTopo", ptmin=40000, ptminFilter=50000, mods="lctopo_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt10TrackCaloClusterJets(sequence, outputlist):
    addStandardJets("AntiKt", 1.0, "TrackCaloCluster", ptmin=40000, ptminFilter=50000, mods="tcc_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt10UFOCSSKJets(sequence, outputlist):
    addStandardJets("AntiKt", 1.0, "UFOCSSK", ptmin=40000, ptminFilter=50000, algseq=sequence, outputGroup=outputlist, constmods=["CSSK"])

def addAntiKt10UFOCHSJets(sequence, outputlist):
    addCHSPFlowObjects()
    addStandardJets("AntiKt", 1.0, "UFOCHS", ptmin=40000, ptminFilter=50000, algseq=sequence, outputGroup=outputlist, constmods=["CHS"])

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

def addAntiKt2TruthJets(sequence,outputlist):
    if DerivationFrameworkHasTruth:
        addStandardJets("AntiKt", 0.2, "Truth", ptmin=5000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt4TruthJets(sequence,outputlist):
    if DerivationFrameworkHasTruth:
        addStandardJets("AntiKt", 0.4, "Truth", ptmin=5000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt4TruthWZJets(sequence,outputlist):
    if DerivationFrameworkHasTruth:
        addStandardJets("AntiKt", 0.4, "TruthWZ", ptmin=5000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt10TruthJets(sequence,outputlist):
    if DerivationFrameworkHasTruth:
        addStandardJets("AntiKt", 1.0, "Truth", ptmin=40000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt10TruthWZJets(sequence,outputlist):
    if DerivationFrameworkHasTruth:
        addStandardJets("AntiKt", 1.0, "TruthWZ", ptmin=40000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt4TruthDressedWZJets(sequence,outputlist):
    if DerivationFrameworkHasTruth:
        addStandardJets("AntiKt", 0.4, "TruthDressedWZ", ptmin=5000, mods="truth_ungroomed", algseq=sequence, outputGroup=outputlist)

def replaceAODReducedJets(jetlist,sequence,outputlist, extendedFlag = 0):
    extjetlog.info( "Replacing AOD-reduced jet collections: {0}".format(",".join(jetlist)))
    if "AntiKt2PV0TrackJets" in jetlist:
        addAntiKt2PV0TrackJets(sequence,outputlist, extendedFlag)
    if "AntiKt4PV0TrackJets" in jetlist:
        addAntiKt4PV0TrackJets(sequence,outputlist, extendedFlag)
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
    if "AntiKt10TrackCaloClusterJets" in jetlist:
        addAntiKt10TrackCaloClusterJets(sequence,outputlist)
    if "AntiKt10UFOCSSKJets" in jetlist:
        addAntiKt10UFOCSSKJets(sequence,outputlist)
    if "AntiKt10UFOCHSJets" in jetlist:
        addAntiKt10UFOCHSJets(sequence,outputlist)

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
    #Commented for now as we don't recommend LCTopo jets but working on its optimisation
    #addStandardJets("AntiKt", 0.4, "LCTopo",  namesuffix="NoPtCut", ptmin=0, ptminFilter=1,
    #                mods="lctopo_ungroomed", algseq=sequence, outputGroup=outputlist,calibOpt="none")
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
        extjetname = extjetalg + 'Jets' if 'BTagging' not in extjetalg else extjetalg.replace('_BTagging','Jets_BTagging')
        jetassoctool = CfgMgr.DerivationFramework__JetExternalAssocTool(jetassoctoolname,
                                                                        InputJets=jetalg+'Jets',
                                                                        ExternalJetCollectionName = extjetname,
                                                                        **options)
        ToolSvc += jetassoctool

    return jetassoctool

def applyJetCalibration(jetalg,algname,sequence,fatjetconfig = 'comb', suffix = ''):
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
    if hasattr(ToolSvc,calibtoolname):
        jetaugtool.JetCalibTool = getattr(ToolSvc,calibtoolname)
    else:
        isdata=False

        #fatjetconfig selects config file for AntiKt10LCTopoTrimmedPtFrac5SmallR20, default is JES_MC16recommendation_FatJet_JMS_comb_19Jan2018.config
        if not fatjetconfig in ['comb','calo','TA']:
            extjetlog.warning('*** Wrong value for fatjetconfig!  Only \'comb\' (default), \'calo\' or \'TA\' can be used. ***')

        configdict = {'AntiKt4EMTopo':('JES_MC16Recommendation_Consolidated_EMTopo_Apr2019_Rel21.config',
                                       'JetArea_Residual_EtaJES_GSC_Smear'),
                      'AntiKt4LCTopo':('JES_MC16Recommendation_28Nov2017.config',
                                       'JetArea_Residual_EtaJES_GSC'),
                      'AntiKt4EMPFlow':('JES_MC16Recommendation_Consolidated_PFlow_Apr2019_Rel21.config',
                                        'JetArea_Residual_EtaJES_GSC_Smear'),
                      'AntiKt10LCTopoTrimmedPtFrac5SmallR20':('JES_MC16recommendation_FatJet_Trimmed_JMS_comb_17Oct2018.config',
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
        # Comment from ATLASG-1532 that we currently use the MC calibration for overlay
        isMC = DerivationFrameworkHasTruth or DerivationFrameworkIsDataOverlay
        if DerivationFrameworkIsDataOverlay:
            extjetlog.warning('Using MC calibration for overlay data.')

        isAF2 = False
        if isMC:
            isAF2 = 'ATLFASTII' in inputFileSummary['metadata']['/Simulation/Parameters']['SimulationFlavour'].upper()
        if isMC and isAF2:
            configdict['AntiKt4EMTopo'] = ('JES_MC15Prerecommendation_AFII_June2015_rel21.config',
                                           'JetArea_Residual_EtaJES_GSC')
            configdict['AntiKt4EMPFlow'] = ('JES_MC16Recommendation_AFII_PFlow_Apr2019_Rel21.config',
                                            'JetArea_Residual_EtaJES_GSC_Smear')

        config,calibseq = configdict[jetalg_basename]

        if (not isMC) and jetalg_basename in ['AntiKt4EMTopo','AntiKt4LCTopo','AntiKt4EMPFlow']:
            isdata=True
            if not jetalg_basename=='AntiKt4LCTopo': calibseq = calibseq[:-6]+'_Insitu'

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

def applyJetCalibration_xAODColl(jetalg='AntiKt4EMTopo',sequence=DerivationFrameworkJob):
    supportedJets = ['AntiKt4EMTopo','AntiKt4LCTopo','AntiKt4EMPFlow','AntiKt4EMTopo_BTagging201810','AntiKt4EMPFlow_BTagging201810']
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
        jvttool = CfgMgr.JetVertexTaggerTool(jvttoolname,VertexContainer=pvxName) 
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

def updateJVT_xAODColl(jetalg='AntiKt4EMTopo',sequence=DerivationFrameworkJob):
    supportedJets = ['AntiKt4EMTopo','AntiKt4EMPFlow','AntiKt4EMTopo_BTagging201810','AntiKt4EMPFlow_BTagging201810']
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

def applyMVfJvtAugmentation(jetalg,sequence,algname):
    supportedJets = ['AntiKt4EMTopo']
    if not jetalg in supportedJets:
        extjetlog.error('*** MVfJvt augmentation requested for unsupported jet collection {}! ***'.format(jetalg))
        return
    else:
        jetaugtool = getJetAugmentationTool(jetalg)

        if(jetaugtool==None or jetaugtool.JetCalibTool=='' or jetaugtool.JetJvtTool==''):
            extjetlog.error('*** MVfJvt called but required augmentation tool does not exist! ***')
            extjetlog.error('*** You must apply jet calibration and JVT! ***')
            return

        mvfjvttoolname = 'DFJetMVfJvt_'+jetalg    
        
        from AthenaCommon.AppMgr import ToolSvc

        if hasattr(ToolSvc,mvfjvttoolname):
            jetaugtool.JetForwardJvtToolBDT = getattr(ToolSvc,mvfjvttoolname)
        else:
            mvfjvttool = CfgMgr.JetForwardJvtToolBDT(mvfjvttoolname)
            ToolSvc += mvfjvttool
            jetaugtool.JetForwardJvtToolBDT = mvfjvttool
            
        extjetlog.info('ExtendedJetCommon: Applying MVfJVT augmentation to jet collection: '+jetalg+'Jets')
        applyJetAugmentation(jetalg,algname,sequence,jetaugtool)

def getPFlowfJVT(jetalg,algname,sequence,primaryVertexCont="PrimaryVertices",overlapLabel="",outLabel="fJvt",includePV=False):
    supportedJets = ['AntiKt4EMPFlow','AntiKt4PFlowCustomVtxHgg']
    if not jetalg in supportedJets:
        extjetlog.error('*** PFlow fJvt augmentation requested for unsupported jet collection {}! ***'.format(jetalg))
        return
    else:
        applyJetCalibration(jetalg,algname,sequence,suffix=algname)
        updateJVT(jetalg,algname,sequence,customVxColl=primaryVertexCont,suffix=algname)
        jetaugtool = getJetAugmentationTool(jetalg,suffix=algname)

        if(jetaugtool==None or jetaugtool.JetCalibTool=='' or jetaugtool.JetJvtTool==''):
            extjetlog.error('*** PFlow fJvt called but required augmentation tool does not exist! ***')
            extjetlog.error('*** Jet calibration and JVT have to be applied ! ***')
            return

        pffjvttoolname = 'DFJetPFfJvt_'+jetalg+algname

        from AthenaCommon.AppMgr import ToolSvc

        if hasattr(ToolSvc,pffjvttoolname):
            jetaugtool.JetForwardPFlowJvtTool = getattr(ToolSvc,pffjvttoolname)
            jetaugtool.JetForwardPFlowJvtTool.verticesName=primaryVertexCont
            jetaugtool.JetForwardPFlowJvtTool.orLabel=overlapLabel
            jetaugtool.JetForwardPFlowJvtTool.includePV=includePV
            jetaugtool.JetForwardPFlowJvtTool.outLabel=outLabel
            jetaugtool.fJvtMomentKey = outLabel
        else:
            pffjvttool = CfgMgr.JetForwardPFlowJvtTool(pffjvttoolname,verticesName=primaryVertexCont,jetsName=pffjvttoolname+'Jets',orLabel=overlapLabel,outLabel=outLabel,includePV=includePV)
            ToolSvc += pffjvttool
            jetaugtool.JetForwardPFlowJvtTool = pffjvttool
            jetaugtool.fJvtMomentKey = outLabel

        extjetlog.info('ExtendedJetCommon: Applying PFlow fJvt augmentation to jet collection: '+jetalg+'Jets')
        applyJetAugmentation(jetalg,algname,sequence,jetaugtool)

def getCustomJvt(jetalg,sequence,algname,outLabel="customJvt",z0cutLowEta=3.,z0cutHighEta=3.,z0etaDiv=1.5):
    supportedJets = ['AntiKt4EMPFlow']
    if not jetalg in supportedJets:
        extjetlog.error('*** PFlow fJvt augmentation requested for unsupported jet collection {}! ***'.format(jetalg))
        return
    else:
        jetaugtool = getJetAugmentationTool(jetalg,suffix=algname)

        if(jetaugtool==None):
            extjetlog.error('*** Custom Jvt called but required augmentation tool does not exist! ***')
            return

        customSumPtToolname = 'DFJetCustomJvt_'+jetalg+algname
        customJvfToolname = 'DFJetCustomJvf_'+jetalg+algname

        from AthenaCommon.AppMgr import ToolSvc

        if hasattr(ToolSvc,customSumPtToolname):
            jetaugtool.JetCustomSumTrkTool = getattr(ToolSvc,customSumPtToolname)
            jetaugtool.JetCustomJvfTool = getattr(ToolSvc,customJvfToolname)
        else:
            customSumPtTool = CfgMgr.JetCustomVertexTaggerTool(customSumPtToolname)
            customJvfTool = CfgMgr.JetCustomVertexFractionTool(customJvfToolname)
            ToolSvc += customSumPtTool
            ToolSvc += customJvfTool
            jetaugtool.CustomSumTrkTool = customSumPtTool
            jetaugtool.CustomJvfTool = customJvfTool
        jetaugtool.customSumTrkMomentKey = "SumPtTrkPt500_"+outLabel
        jetaugtool.customJvfMomentKey = "JVF"+outLabel+"Corr"

        customSumPtTool.customSumPtTrkName = "SumPtTrkPt500_"+outLabel
        customSumPtTool = getattr(ToolSvc,customSumPtToolname)
        customSumPtTool.z0cutLowEta = z0cutLowEta
        customSumPtTool.z0cutHighEta = z0cutHighEta
        customSumPtTool.z0etaDiv = z0etaDiv

        customJvfTool.JVFName = "JVF"+outLabel
        customJvfTool = getattr(ToolSvc,customJvfToolname)
        customJvfTool.z0cutLowEta = z0cutLowEta
        customJvfTool.z0cutHighEta = z0cutHighEta
        customJvfTool.z0etaDiv = z0etaDiv

        TrackSelToolName = 'DFTrackSelection_CustomJvt'
        if hasattr(ToolSvc, TrackSelToolName):
          customSumPtTool.TrackSelector = getattr(ToolSvc, TrackSelToolName)
          customJvfTool.TrackSelector = getattr(ToolSvc, TrackSelToolName)
        else:
          IDTrackSelToolName = 'DFIDTrackSelection_CustomJvt'
          idtrackselectiontool = CfgMgr.InDet__InDetTrackSelectionTool( IDTrackSelToolName )
          idtrackselectiontool.CutLevel = "Loose"
          ToolSvc += idtrackselectiontool

          trackselectiontool = CfgMgr.JetTrackSelectionTool( TrackSelToolName )
          trackselectiontool.Selector = idtrackselectiontool
          ToolSvc += trackselectiontool

          customSumPtTool.TrackSelector = getattr(ToolSvc, TrackSelToolName)
          customJvfTool.TrackSelector = getattr(ToolSvc, TrackSelToolName)

        extjetlog.info('ExtendedJetCommon: Applying Custom Jvt augmentation to jet collection: '+jetalg+'Jets')
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

    inJets = jetalg+'Jets'
    if '_BTagging' in jetalg:
      inJets = jetalg.replace('_BTagging','Jets_BTagging')
    extjetlog.info('ExtendedJetCommon: Applying b-tagging working points for jet collection: '+inJets)
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
def addQGTaggerTool(jetalg, sequence, algname, truthjetalg=None ):
    jetaugtool = getJetAugmentationTool(jetalg,'_qgTag')
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

    if truthjetalg!=None:
        jetptassociationtoolname = 'DFJetPtAssociation_'+truthjetalg+'_'+jetalg
        if hasattr(ToolSvc,jetptassociationtoolname):
            jetaugtool.JetPtAssociationTool = getattr(ToolSvc,jetptassociationtoolname)
        else:
            jetptassociationtool = CfgMgr.JetPtAssociationTool(jetptassociationtoolname, InputContainer=truthjetalg, AssociationName="GhostTruth")
            ToolSvc += jetptassociationtool
            jetaugtool.JetPtAssociationTool = jetptassociationtool

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

    from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__MuonJetDrTool
    MuonJetDrTool = DerivationFramework__MuonJetDrTool( name = "MuonJetDrTool")
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += MuonJetDrTool
    DFCommonMuonJetTools = []
    DFCommonMuonJetTools.append(MuonJetDrTool)
    sequence += CfgMgr.DerivationFramework__CommonAugmentation("DFCommonMuonsKernel2",
                                                                             AugmentationTools = DFCommonMuonJetTools
                                                                            )

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

    # Get the constituent mod sequence
    from JetRecTools import ConstModHelpers
    constmodseq = ConstModHelpers.getConstModSeq(constmods,inputtype,customVxColl)
    label = inputtype + constmodstr

    # Add the const mod sequence to the input preparation jetalg instance
    # Could add the event shape computation here
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()
    from JetRec.JetRecStandard import jtm
    if constmodseq.getFullName() not in [t.getFullName() for t in job.jetalg.Tools]:
        # Add this tool runner to the JetAlgorithm instance "jetalg"
        # which runs all preparatory tools
        # This was added by JetCommon
        job.jetalg.Tools.append(constmodseq)
        extjetlog.info("Added const mod sequence {0} to \'jetalg\'".format(constmodstr))

    # Get the PseudoJetGetter
    pjname = label.lower().replace("topo","")
    pjg = ConstModHelpers.getPseudoJetGetter(label,pjname)

    # Generate the getter list including ghosts from that for the standard list for the input type
    getterbase = inputtype.lower()
    if inputtype == "PFlowCustomVtx": getterbase = "empflow_reduced"
    getters = [pjg]+list(jtm.gettersMap[getterbase])[1:]
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
applyJetCalibration_xAODColl("AntiKt4EMTopo")
updateJVT_xAODColl("AntiKt4EMTopo")
applyJetCalibration_xAODColl("AntiKt4EMPFlow")
updateJVT_xAODColl("AntiKt4EMPFlow")
applyOverlapRemoval()
eventCleanLoose_xAODColl("AntiKt4EMTopo")
eventCleanTight_xAODColl("AntiKt4EMTopo")
eventCleanLooseLLP_xAODColl("AntiKt4EMTopo")
eventCleanSuperLooseLLP_xAODColl("AntiKt4EMTopo")
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

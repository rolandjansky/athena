# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# ExtendedJetCommon.py
# Schedules special jet tools
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob, DerivationFrameworkIsMonteCarlo
from DerivationFrameworkJetEtMiss.JetCommon import addStandardJets, addStandardVRTrackJets, addTrimmedJets, defineEDAlg
from JetJvtEfficiency.JetJvtEfficiencyToolConfig import getJvtEffTool

from AthenaCommon import CfgMgr
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

def addDefaultTrimmedJets(sequence,outputlist,dotruth=True,writeUngroomed=False,linkVRGhosts=False):
    if DerivationFrameworkIsMonteCarlo and dotruth:
        if not hasattr(sequence,'jetalgAntiKt10TruthTrimmedPtFrac5SmallR20'):
            addTrimmedJets('AntiKt', 1.0, 'Truth', rclus=0.2, ptfrac=0.05, mods="truth_groomed",
                           algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed)
    addTrimmedJets('AntiKt', 1.0, 'LCTopo', rclus=0.2, ptfrac=0.05, mods="lctopo_groomed",
                   algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed, includeVRGhosts=linkVRGhosts,
                   customGetters=jtm.gettersMap["lctopo_reduced"])

def addTCCTrimmedJets(sequence,outputlist,dotruth=True,writeUngroomed=False):
    addTrimmedJets('AntiKt', 1.0, 'TrackCaloCluster', rclus=0.2, ptfrac=0.05, mods="tcc_groomed",
                   algseq=sequence, outputGroup=outputlist, writeUngroomed=writeUngroomed,
                   customGetters=jtm.gettersMap["lctopo_reduced"])

##################################################################              
# Jet helpers for ungroomed jets (removed in xAOD reduction)
##################################################################              

from JetRec.JetRecStandard import jtm

def addAntiKt10LCTopoJets(sequence, outputlist):
    addStandardJets("AntiKt", 1.0, "LCTopo", ptmin=40000, ptminFilter=50000, mods="lctopo_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt2PV0TrackJets(sequence, outputlist):
    jtm.modifiersMap["akt2track"] = jtm.modifiersMap["track_ungroomed"]
    addStandardJets("AntiKt", 0.2, "PV0Track", ptmin=2000, mods="akt2track",
                    algseq=sequence, outputGroup=outputlist)

def addAntiKt4PV0TrackJets(sequence, outputlist):
    addStandardJets("AntiKt", 0.4, "PV0Track", ptmin=2000, mods="track_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKt10PV0TrackJets(sequence, outputlist):
    addStandardJets("AntiKt", 1.0, "PV0Track", ptmin=2000, ptminFilter=40000, mods="track_ungroomed", algseq=sequence, outputGroup=outputlist)

def addAntiKtVR30Rmax4Rmin02PV0TrackJets(sequence, outputlist):
    addStandardVRTrackJets("AntiKt", 30000, 0.4, 0.02, "PV0Track", ptmin=4000, algseq=sequence, outputGroup=outputlist)

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
    from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkHasTruth
    if DerivationFrameworkHasTruth:
        addStandardJets("AntiKt", 1.0, "Truth", ptmin=40000, mods="truth_ungroomed_larger", algseq=sequence, outputGroup=outputlist)

def addAntiKt10TruthWZJets(sequence,outputlist):
    from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkHasTruth
    if DerivationFrameworkHasTruth:
        addStandardJets("AntiKt", 1.0, "TruthWZ", ptmin=40000, mods="truth_ungroomed_larger", algseq=sequence, outputGroup=outputlist)

def addAntiKt4EMTopoJets(sequence, outputlist):
    addEventShape(0.4, "EMTopo", sequence)
    ptCutCalibrated = 10000
    from JetRec.JetRecFlags import jetFlags
    if not jetFlags.useCalibJetThreshold:
        ptCutCalibrated = 1
    addStandardJets("AntiKt", 0.4, "EMTopo", ptmin=5000, ptminFilter=ptCutCalibrated, calibOpt="arj", algseq=sequence, outputGroup=outputlist, customGetters=jtm.gettersMap["emtopo_reduced"])

def addAntiKt4LCTopoJets(sequence, outputlist):
    addEventShape(0.4, "LCTopo", sequence)
    ptCutCalibrated = 10000
    from JetRec.JetRecFlags import jetFlags
    if not jetFlags.useCalibJetThreshold:
        ptCutCalibrated = 1
    addStandardJets("AntiKt", 0.4, "LCTopo", ptmin=5000, ptminFilter=ptCutCalibrated, calibOpt="arj", algseq=sequence, outputGroup=outputlist, customGetters=jtm.gettersMap["lctopo_reduced"])

def addAntiKt4EMPFlowJets(sequence, outputlist):
    addCHSPFlowObjects()
    addEventShape(0.4, "EMPFlow", sequence)
    #New rho definition for precision recommendations
    addEventShape(0.4, "EMPFlowPUSB", sequence)
    ptCutCalibrated = 10000
    from JetRec.JetRecFlags import jetFlags
    if not jetFlags.useCalibJetThreshold:
        ptCutCalibrated = 1
    addStandardJets("AntiKt", 0.4, "EMPFlow", ptmin=5000, ptminFilter=ptCutCalibrated, mods="pflow_ungroomed", calibOpt="arj:pflow", algseq=sequence, outputGroup=outputlist, customGetters=jtm.gettersMap["empflow_reduced"])

##################################################################  

def addDAODJets(jetlist,sequence,outputlist):
    extjetlog.info( "Replacing AOD-reduced jet collections: {0}".format(",".join(jetlist)))
    if "AntiKt4EMTopoJets" in jetlist:
        addAntiKt4EMTopoJets(sequence,outputlist)
    if "AntiKt4LCTopoJets" in jetlist:
        addAntiKt4LCTopoJets(sequence,outputlist)
    if "AntiKt4EMPFlowJets" in jetlist:
        addAntiKt4EMPFlowJets(sequence,outputlist)
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
    if "AntiKt10LCTopoJets" in jetlist:
        addAntiKt10LCTopoJets(sequence,outputlist)
    if "AntiKtVR30Rmax4Rmin02PV0TrackJets" in jetlist:
        addAntiKtVR30Rmax4Rmin02PV0TrackJets(sequence,outputlist)


##################################################################
# Jet helpers for adding low-pt jets needed for calibration 
##################################################################

# 2 GeV cut after pileup suppression for in-situ Z
def addAntiKt4LowPtJets(jetlist, sequence,outputlist):
    if "AntiKt4EMTopoJets" in jetlist:
        addStandardJets("AntiKt", 0.4, "EMTopo",  namesuffix="LowPt", ptmin=2000, ptminFilter=2000,
                        mods="emtopo_ungroomed", customGetters=jtm.gettersMap["emtopo_reduced"],
                        algseq=sequence, outputGroup=outputlist,calibOpt="ar")
    if "AntiKt4LCTopoJets" in jetlist:
        addStandardJets("AntiKt", 0.4, "LCTopo",  namesuffix="LowPt", ptmin=2000, ptminFilter=2000,
                        mods="lctopo_ungroomed", customGetters=jtm.gettersMap["lctopo_reduced"],
                        algseq=sequence, outputGroup=outputlist, calibOpt="ar")
    if "AntiKt4EMPFlowJets" in jetlist:
        addCHSPFlowObjects()
        addStandardJets("AntiKt", 0.4, "EMPFlow", namesuffix="LowPt", ptmin=2000, ptminFilter=2000,
                        mods="pflow_ungroomed", customGetters=jtm.gettersMap["empflow_reduced"],
                        algseq=sequence, outputGroup=outputlist,calibOpt="ar:pflow")

################################################################## 

# 1 MeV cut at constituent level for MCJES
def addAntiKt4NoPtCutJets(jetlist, sequence,outputlist):
    if "AntiKt4EMTopoJets" in jetlist:
        addStandardJets("AntiKt", 0.4, "EMTopo",  namesuffix="NoPtCut", ptmin=0, ptminFilter=1,
                        mods="emtopo_ungroomed", customGetters=jtm.gettersMap["emtopo_reduced"],
                        algseq=sequence, outputGroup=outputlist,calibOpt="none")
    if "AntiKt4LCTopoJets" in jetlist:
        addStandardJets("AntiKt", 0.4, "LCTopo",  namesuffix="NoPtCut", ptmin=0, ptminFilter=1,
                        mods="lctopo_ungroomed", customGetters=jtm.gettersMap["lctopo_reduced"],
                        algseq=sequence, outputGroup=outputlist,calibOpt="none")
    if "AntiKt4EMPFlowJets" in jetlist:
        addCHSPFlowObjects()
        addStandardJets("AntiKt", 0.4, "EMPFlow", namesuffix="NoPtCut", ptmin=0, ptminFilter=1,
                        mods="pflow_ungroomed", customGetters=jtm.gettersMap["empflow_reduced"],
                        algseq=sequence, outputGroup=outputlist,calibOpt="none")

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

###################################################################

def addJetPtAssociation(jetalg, truthjetalg, sequence):

    truthAssocAlgName = 'DFJetPtAssociationAlg_'+ truthjetalg + '_' + jetalg
    if hasattr(sequence, truthAssocAlgName):
        return
    jetTruthAssocTool = CfgMgr.JetPtAssociationTool('DFJetPtAssociation_' + truthjetalg + '_' + jetalg,
                                                    JetContainer = jetalg + 'Jets',
                                                    MatchingJetContainer = truthjetalg,
                                                    AssociationName = "GhostTruth")

    extjetlog.info('ExtendedJetCommon: Adding JetPtAssociationTool for jet collection: '+jetalg+'Jets')
    sequence += CfgMgr.JetDecorationAlg(truthAssocAlgName, JetContainer=jetalg+'Jets', Decorators=[jetTruthAssocTool])

##################################################################

def addJetTruthLabel(jetalg,labelname,sequence):
    supportedLabelNames = ['R10TruthLabel_R21Consolidated','R10TruthLabel_R21Precision']
    supportedTruthJets = ['AntiKt10Truth','AntiKt10TruthTrimmedPtFrac5SmallR20']
    supportedRecoJets = ['AntiKt10LCTopoTrimmedPtFrac5SmallR20','AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20','AntiKt10UFOCSSKTrimmedPtFrac5SmallR20','AntiKt10UFOCSSKSoftDropBeta100Zcut10','AntiKt10UFOCSSKBottomUpSoftDropBeta100Zcut5','AntiKt10UFOCSSKRecursiveSoftDropBeta100Zcut5Ninf','AntiKt10UFOCHSTrimmedPtFrac5SmallR20']
    supportedJets = supportedRecoJets + supportedTruthJets
    if jetalg not in supportedJets:
        extjetlog.warning('*** JetTruthLabeling augmentation requested for unsupported jet collection {}! ***'.format(jetalg))
        return
    elif labelname not in supportedLabelNames:
        extjetlog.warning('*** JetTruthLabeling augmentation requested for unsupported label definition {}! ***'.format(labelname))
        return

    truthLabelAlgName = 'DFJetTruthLabelAlg_' + jetalg + '_' + labelname
    if hasattr(sequence, truthLabelAlgName):
        return

    isTruthJet = False
    if jetalg in supportedTruthJets:
        isTruthJet = True

    jetTruthLabelTool = CfgMgr.JetTruthLabelingTool('DFJetTruthLabel_'+jetalg+'_'+labelname,
                                                    IsTruthJetCollection = isTruthJet,
                                                    TruthLabelName = labelname)
    if not isTruthJet:
        jetTruthLabelTool.RecoJetContainer = jetalg + 'Jets'
    extjetlog.info('ExtendedJetCommon: Applying JetTruthLabel augmentation to jet collection: ' + jetalg + 'Jets' + ' using ' + labelname +' definition')
    sequence += CfgMgr.JetDecorationAlg(truthLabelAlgName, JetContainer=jetalg+'Jets', Decorators=[jetTruthLabelTool])

##################################################################  

def getPFlowfJVT(jetalg,sequence,primaryVertexCont="PrimaryVertices",overlapLabel="",outLabel="fJvt",includePV=False):
    supportedJets = ['AntiKt4EMPFlow']
    if jetalg not in supportedJets:
        extjetlog.error('*** PFlow fJvt augmentation requested for unsupported jet collection {}! ***'.format(jetalg))
        return
    
    fJVTAlgName = "DFJetFJVTAlg_" + jetalg
    if hasattr(sequence, fJVTAlgName):
        return

    # Calibration tool specific for pFlow fJVT: without GSC and smearing
    jetCalibrationTool = CfgMgr.JetCalibrationTool('DFJetFJVT_' + jetalg + '_CalibTool',
                                                    JetCollection = 'AntiKt4EMPFlow',
                                                    ConfigFile = "JES_MC16Recommendation_Consolidated_PFlow_Apr2019_Rel21.config",
                                                    CalibSequence = "JetArea_Residual_EtaJES",
                                                    CalibArea = "00-04-82",
                                                    RhoKey = 'Kt4EMPFlowEventShape',
                                                    IsData = False)

    wPFOTool = CfgMgr.CP__WeightPFOTool("DFJetFJVT_" + jetalg + "_wPFO")

    fJVTTool = CfgMgr.JetForwardPFlowJvtTool('DFJetFJVT_' + jetalg,
                                                verticesName = primaryVertexCont,
                                                JetContainer = jetalg+"Jets",
                                                TrackVertexAssociation = jtm.tvassoc.TrackVertexAssociation,
                                                WeightPFOTool = wPFOTool,
                                                JetCalibrationTool = jetCalibrationTool,
                                                FEName = 'CHSParticleFlowObjects',
                                                ORName = overlapLabel,
                                                FjvtRawName = 'DFCommonJets_' + outLabel,
                                                includePV = includePV)

    extjetlog.info('ExtendedJetCommon: Applying PFlow fJvt augmentation to jet collection: ' + jetalg + 'Jets')
    sequence += CfgMgr.JetDecorationAlg(fJVTAlgName, JetContainer=jetalg+'Jets', Decorators=[fJVTTool])

#################################################################
### Schedule Q/G-tagging decorations ### QGTaggerTool ##### 
#################################################################
def addQGTaggerTool(jetalg, sequence, truthjetalg=None):

    qgAlgName = 'DFQGTaggerAlg_' + jetalg
    if hasattr(sequence, qgAlgName):
        return

    if truthjetalg is not None:
        addJetPtAssociation(jetalg, truthjetalg, sequence)

    trackselectiontool = CfgMgr.InDet__InDetTrackSelectionTool('DFQGTaggerTool_InDetTrackSelectionTool_' + jetalg, CutLevel = "Loose" )
    trackvertexassoctool = CfgMgr.CP__TrackVertexAssociationTool('DFQGTaggerTool_InDetTrackVertexAssosciationTool_' + jetalg, WorkingPoint = "Loose")
    qgTool = CfgMgr.JetQGTaggerVariableTool('DFQGTaggerTool_' + jetalg,
                                            JetContainer=jetalg + 'Jets',
                                            VertexContainer='PrimaryVertices',
                                            TVATool=trackvertexassoctool,
                                            TrkSelTool=trackselectiontool)

    extjetlog.info('ExtendedJetCommon: Adding QGTaggerTool for jet collection: '+jetalg)
    sequence += CfgMgr.JetDecorationAlg(qgAlgName, JetContainer=jetalg+'Jets', Decorators=[qgTool])

##################################################################

def addPassJvtForCleaning(sequence=DerivationFrameworkJob):
    algName = "DFJet_EventCleaning_passJvtAlg"
    if hasattr(sequence, algName):
        return

    passJvtTool = getJvtEffTool('AntiKt4EMTopo')
    passJvtTool.PassJVTKey = "AntiKt4EMTopoJets.DFCommonJets_passJvt"

    extjetlog.info('ExtendedJetCommon: Adding passJvt decoration to AntiKt4EMTopoJets for event cleaning')
    sequence += CfgMgr.JetDecorationAlg(algName, JetContainer='AntiKt4EMTopoJets', Decorators=[passJvtTool])

##################################################################

def applyOverlapRemoval(sequence=DerivationFrameworkJob):

    from  DerivationFrameworkTau.TauCommon import AddTauAugmentation
    AddTauAugmentation(sequence,doLoose=True)

    from AssociationUtils.config import recommended_tools
    from AssociationUtils.AssociationUtilsConf import OverlapRemovalGenUseAlg
    outputLabel = 'DFCommonJets_passOR'
    bJetLabel = '' #default
    tauLabel = 'DFTauLoose'
    orTool = recommended_tools(outputLabel=outputLabel,bJetLabel=bJetLabel)
    algOR = OverlapRemovalGenUseAlg('OverlapRemovalGenUseAlg',
                                    OverlapLabel=outputLabel,
                                    OverlapRemovalTool=orTool,
                                    TauLabel=tauLabel,
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
                            ghostArea=0.01, ptmin=2000, ptminFilter=7000, calibOpt="none",
                            algseq=sequence, outputGroup=outputlist, customGetters=jtm.gettersMap["lctopo_reduced"])

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
        constit = JetConstit( xAODType.FlowElement )

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
        if "LCOriginTopoClusters" not in slimhelper.AppendToDictionary:
            slimhelper.AppendToDictionary["LCOriginTopoClusters"]='xAOD::CaloClusterContainer'
            slimhelper.AppendToDictionary["LCOriginTopoClustersAux"]='xAOD::ShallowAuxContainer'
            slimhelper.ExtraVariables.append('LCOriginTopoClusters.calEta.calPhi')

    if writeEM:
        if "EMOriginTopoClusters" not in slimhelper.AppendToDictionary:
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
    if not IsInInputFile("xAOD::FlowElementContainer","CHSParticleFlowObjects"):
        # Check that an alg doing this has not already been inserted
        from AthenaCommon.AlgSequence import AlgSequence
        job = AlgSequence()
        from JetRec.JetRecStandard import jtm
        if not hasattr(job,"jetalgCHSPFlow") and not hasattr(jtm,"jetconstitCHSPFlow"):
            from JetRec.JetRecConf import JetToolRunner
            from JetRec.JetRecFlags import jetFlags
            if jetFlags.useTrackVertexTool:
                jtm += JetToolRunner("jetconstitCHSPFlow",
                                         EventShapeTools=[],
                                         Tools=[jtm.ttvaassocNew,jtm.JetConstitSeq_PFlowCHS])
            else:
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
# Helper to schedule event shapes, needed for calibration
# Automatically chooses the origin corrected clusters if jet reco
# is configured for them.
##################################################################
def addEventShape(radius, inputType, sequence):
    from JetRec.JetRecFlags import jetFlags
    if jetFlags.useTracks() and jetFlags.useVertices():
        if inputType == "EMTopo":
            inputType = "EMTopoOrigin"
        elif inputType == "LCTopo":
            inputType = "LCTopoOrigin"
    if not hasattr(sequence, "EDTool"+str(int(radius*10))+inputType):
        # Schedule pseudojets if necessary
        pjAlg = getPseudoJetAlg(inputType)
        if not hasattr(sequence, pjAlg.name()):
            sequence += pjAlg
        sequence += defineEDAlg(R=radius, inputtype=inputType)

def getPseudoJetAlg(inputType):
    return {"LCTopo" : jtm.lcget,
            "EMTopo" : jtm.emget,
            "LCTopoOrigin" : jtm.lcoriginget,
            "EMTopoOrigin" : jtm.emoriginget,
            "EMPFlow": jtm.empflowget,
            "EMPFlowPUSB": jtm.empflowpusbget,
            "EMPFlowNeut": jtm.empflowneutget}[inputType]

def addEventCleanFlags(sequence, workingPoints = ['Loose', 'Tight', 'LooseLLP']):
    
    # Prereqs
    addPassJvtForCleaning(sequence)
    applyOverlapRemoval(sequence)

    from JetSelectorTools.JetSelectorToolsConf import ECUtils__EventCleaningTool as EventCleaningTool
    from JetSelectorTools.JetSelectorToolsConf import EventCleaningTestAlg
    supportedWPs = ['Loose', 'Tight', 'LooseLLP', 'VeryLooseLLP', 'SuperLooseLLP']
    prefix = "DFCommonJets_"

    for wp in workingPoints:
        if wp not in supportedWPs:
            extjetlog.warning('*** Unsupported event cleaning WP {} requested! Skipping it.***'.format(wp))
            continue
        algName = 'EventCleaningTestAlg_' + wp
        if hasattr(sequence, algName):
            continue

        cleaningLevel = wp + 'Bad'
        # LLP WPs have a slightly different name format
        if 'LLP' in wp:
            cleaningLevel = wp.replace('LLP', 'BadLLP')

        ecTool = EventCleaningTool('EventCleaningTool_' + wp, CleaningLevel=cleaningLevel)
        ecTool.JetCleanPrefix = prefix
        ecTool.JetCleaningTool = getJetCleaningTool(cleaningLevel)
        ecTool.JetContainer = "AntiKt4EMTopoJets"
        algClean = EventCleaningTestAlg(algName,
                                        EventCleaningTool=ecTool,
                                        JetCollectionName="AntiKt4EMTopoJets",
                                        EventCleanPrefix=prefix,
                                        CleaningLevel=cleaningLevel,
                                        doEvent = ('Loose' in wp)) # Only store event-level flags for Loose and LooseLLP
        sequence += algClean


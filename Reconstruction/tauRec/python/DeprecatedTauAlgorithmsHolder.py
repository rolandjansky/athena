# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

########################################################################
# calibrate tau at EM scale
def getEnergyCalibrationEM():
    _name = sPrefix + 'EnergyCalibrationEM'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauCalibrateEM
    TauCalibrateEM = TauCalibrateEM(name = _name, response_functions_file = "EMTES_Fits_Oct2010.root")
    
    cached_instances[_name] = TauCalibrateEM
    return TauCalibrateEM

########################################################################
# lock tau containers
def getContainerLock():
    _name = sPrefix + 'TauContainerLock'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import LockTauContainers
    LockTauContainers = LockTauContainers(name = _name)
    
    cached_instances[_name] = LockTauContainers 
    return LockTauContainers       

#########################################################################
# Tau ID Variable Pileup Correction
# Not used
def getIDPileUpCorrection():
    _name = sPrefix + 'TauIDPileupCorrection'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauIDPileupCorrection
    IDPileUpCorrection = TauIDPileupCorrection(name = _name, 
                                               calibrationFile1Prong = "TF2pileupForOfflineID_1p.root", 
                                               calibrationFile3Prong = "TF2pileupForOfflineID_3p.root", 
                                               useMu = True)
    
    cached_instances[_name] = IDPileUpCorrection
    return IDPileUpCorrection

########################################################################
#Old TauJetBDT from TauDiscriminant
def getTauJetBDT():
    _name = sPrefix + 'TauJetBDT'
    from TauDiscriminant.TauDiscriminantConf import TauJetBDT
    myTauJetBDT = TauJetBDT(name=_name,
                            jetBDT="uncorrBDTAODfix.bin",#update
                            jetSigTrans="",
                            jetSigBits="TauJetBDT2016FinalCuts_v1.txt")
    cached_instances[_name] = myTauJetBDT
    return myTauJetBDT


########################################################################
#Old TauEleBDT from TauDiscriminant
def getTauEleBDT(_n='TauEleBDT', eBDT='ele.BDT.bin', only_decorate=False, eBits="", eBitsRoot="cuts.eBDT.root"):
    _name = sPrefix + _n 
    from TauDiscriminant.TauDiscriminantConf import TauEleBDT
    myTauEleBDT = TauEleBDT(name=_name, 
                            eleBDT=eBDT,
                            eleBits=eBits, 
                            eleBitsRoot=eBitsRoot,
                            onlyDecorateScore=only_decorate)
    cached_instances[_name] = myTauEleBDT
    return myTauEleBDT

""" obsolete methods

########################################################################
# Tau Origin Cell Correction Tool
def getTauCellCorrection():
    _name = sPrefix + 'TauCellCorrection'
    
    from AthenaCommon.AppMgr import ToolSvc
    
    if _name in cached_instances:
        return cached_instances[_name]

    from tauRecTools.tauRecToolsConf import TauOriginCorrectionTool
    TauCellCorrectionTool = TauOriginCorrectionTool(name = _name,
            UseJVA = False,  #not using JetVertexAssociation, b/c JetRec doesn't use it too
            UsePrimaryVertex = True,
            UseBeamSpot = True,
            VertexContainerKey = "PrimaryVertices")
    
    ToolSvc += TauCellCorrectionTool
    cached_instances[_name] = TauCellCorrectionTool         
    return TauCellCorrectionTool

#########################################################################
# CaloNoiseTool
def getCaloNoiseTool():
    _name = 'CaloNoiseTool'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
    theCaloNoiseTool = CaloNoiseToolDefault()
    
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += theCaloNoiseTool
    
    cached_instances[_name] = theCaloNoiseTool
    return theCaloNoiseTool

#########################################################################
# tau1p3p track match cells
def getTauEflowTrackMatchCells():
    _name = sPrefix + 'EflowTrackMatchCells'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauEflowTrackMatchCells
    TauEflowTrackMatchCells = TauEflowTrackMatchCells(name = _name,
                detRIsolCaloCut   = 0.4,
                useNoiseSigma     = 1,
                AbsNoiseSigma_cut = 2,
                CaloNoiseTool     = getCaloNoiseTool(),
                selectConeSize    = 0.45, #not used anymore
                CellCorrection = True,
                OriginCorrectionTool = getTauCellCorrection()) 
    
    cached_instances[_name] = TauEflowTrackMatchCells         
    return TauEflowTrackMatchCells

#########################################################################
# tau1p3p AddCaloInfo
def getTauEflowAddCaloInfo():
    _name = sPrefix + 'EflowAddCaloInfo'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauEflowAddCaloInfo
    TauEflowAddCaloInfo = TauEflowAddCaloInfo(name = _name,
                detRCoreCaloCut     = 0.2,
                detRIsolCaloCut     = 0.4,
                ETCellMinCut        = 0.1*GeV,
                ETStripMinCut       = 0.2*GeV,
                detaStripCut        = 0.2,
                CellCorrection = True,
                OriginCorrectionTool = getTauCellCorrection())  
    
    cached_instances[_name] = TauEflowAddCaloInfo         
    return TauEflowAddCaloInfo

#########################################################################
# tau1p3p eflow info
def getTauEflowVariables():
    _name = sPrefix + 'EflowVariables'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauEflowVariables
    TauEflowVariables = TauEflowVariables(name = _name,
                detRCoreCaloCut = 0.2,
                dphiEMCLCut     = 0.0375,
                detaEMCLCut     = 0.0375,
                dphiEMCLFACCut  = 2,
                detaEMCLFACCut  = 3,
                dphiChrgEMCut   = 0.0375,
                detaChrgEMCut   = 0.0375,
                CaloClusterContainerName = "EMTopoForTaus", #TODO: rec.scoping<3 case??
                RecoTopoClusterETCut     = 0.2*GeV,
                RecoEtaCut               = 2.5,
                TrackTopoClusPhi2Cut     = 0.0375,
                TrackTopoClusEta1Cut     = 0.01,
                MVisEflowCut             = 10.*GeV,
                MTrk3PCut                = 10.*GeV,
                ETeflow_ETcaloCut        = 10.,
                ETeflow_ETcaloCutMin     = 0.1,
                useEMTopoClusters        = True,
                CellCorrection = True,
                OriginCorrectionTool = getTauCellCorrection())
    
    cached_instances[_name] = TauEflowVariables         
    return TauEflowVariables  
    
#####################
# Pi0 Creator Chooser
def getPi0CreatorChooser():
    _name = sPrefix + 'Pi0CreatorChooser'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += getPi0ClusterCreator()
    ToolSvc += getCrakowPi0ClusterCreator()
    
    from tauRecTools.tauRecToolsConf import TauPi0CreatorChooser
    TauPi0CreatorChooser = TauPi0CreatorChooser(name = _name,
                Tau1p3pCreatePi0ClusTool = getCrakowPi0ClusterCreator(),
                TauCommonCreatePi0ClusTool = getPi0ClusterCreator())
    
    cached_instances[_name] = TauPi0CreatorChooser
    return TauPi0CreatorChooser 

#########################################################################
# Crakow Pi0/eflow algorithm
# Cluster/Cellfinder for Pi0/Eflow algos
def getPi0EflowCreateROI():
    _name = sPrefix + 'TauPi0EflowCreateROI'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauPi0EflowCreateROI
    TauPi0EflowCreateROI = TauPi0EflowCreateROI( name = _name,
                detRIsolCaloCut   = 0.4,
                detRCoreCaloCut   = 0.2,
                useNoiseSigma     = 0,
                AbsNoiseSigma_cut = 2,
                removeChrgEM01    = 1,
                removeChrgEM2     = 1,
                detRChrgEMCut     = 0.0375,
                # Added by SL
                fillCellContainer = TRUE,
                CellsOutputContainerName = "TauCells",
                CaloNoiseTool     = getCaloNoiseTool(),
                CaloCellMakerToolNames = ["CaloCellContainerFinalizerTool/cellfinalizerForTaus","CaloCellContainerCheckerTool/cellcheckForTaus"],
                CellCorrection = True,
                OriginCorrectionTool = getTauCellCorrection())   
    
    cached_instances[_name] = TauPi0EflowCreateROI
    return TauPi0EflowCreateROI

################
# Pi0 Clustering
def getCrakowPi0ClusterCreator():
    _name = sPrefix + 'CrakowPi0ClusterCreator'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauPi0CrakowClusterCreator
    TauPi0CrakowClusterCreator = TauPi0CrakowClusterCreator( name = _name,
                detRCoreCaloCut = 0.2,
                CaloClusterContainerName = "EMTopoForTaus",  #TODO: rec.scoping<3 case??
                RecoTopoClusterETCut     = 1.0*GeV,
                RecoEtaCut               = 2.5,
                detTrkClusMin            = 0.0375,
                fracEM01verEM            = 0.1)
    
    cached_instances[_name] = TauPi0CrakowClusterCreator
    return TauPi0CrakowClusterCreator  

########################################################################
# set track infos (charge + global track collection)
def getTauSetTracksAndCharge():
    _name = sPrefix + 'TauSetTracksAndCharge'
    
    if _name in cached_instances:
        return cached_instances[_name] 
    
    from tauRecTools.tauRecToolsConf import TauSetTracksAndCharge
    TauSetTracksAndCharge = TauSetTracksAndCharge(name = _name, TrackContainer    = "TrackParticleCandidate")
    
    cached_instances[_name] = TauSetTracksAndCharge        
    return TauSetTracksAndCharge

"""

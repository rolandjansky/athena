# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Creation: George Redlinger, Lucian Ancu & David Cote, September 2010 
## This file defines the SUSY filters without any instanciation
## The default values are for the newest cut version, but older cut versions are also listed at the bottom

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units
from D2PDMaker.D2PDFlags import D2PDFlags

# Muons filters
from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
###  STACO  ###
StacoMuonSelectorSUSY_forEMu = D2PDMuonSelector( "StacoMuonSelectorSUSY_forEMu",
                                                 inputCollection   = 'StacoMuonCollection',
                                                 outputLinkCollection  = 'SelectedStacoMuonLinks_forEMu',
                                                 muonRequireIsCombined = True,
                                                 etaMin = -2.5,
                                                 etaMax = 2.5,
                                                 minNumberPassed   = 1,
                                                 ptMin  = 6.0*Units.GeV
                                                 )

#StacoMuonSelectorSUSY is a sub-set of StacoMuonSelectorSUSY_forEMu, with tighter pT cut
StacoMuonSelectorSUSY = D2PDMuonSelector( "StacoMuonSelectorSUSY",
                                          inputCollection   = 'SelectedStacoMuonLinks_forEMu',
                                          outputLinkCollection  = 'SelectedStacoMuonLinks',
                                          ptMin  = 12.0*Units.GeV,
                                          minNumberPassed = 1
                                          )

#StacoDiMuonSelectorSUSY is a sub-set of StacoMuonSelectorSUSY_forEMu, requiring two muons
StacoDiMuonSelectorSUSY = D2PDMuonSelector( "StacoDiMuonSelectorSUSY",
                                            inputCollection   = 'SelectedStacoMuonLinks_forEMu',
                                            outputLinkCollection  = 'SelectedStacoDiMuonLinks',
                                            minNumberPassed   = 2
                                            )

####  MuID  ###
MuidMuonSelectorSUSY_forEMu = D2PDMuonSelector( "MuidMuonSelectorSUSY_forEMu",
                                                inputCollection   = 'MuidMuonCollection',
                                                outputLinkCollection  = 'SelectedMuidMuonLinks_forEMu',
                                                muonRequireIsCombined = True,
                                                etaMin = -2.5,
                                                etaMax = 2.5,
                                                minNumberPassed   = 1,
                                                ptMin  = 6.0*Units.GeV
                                                )

#MuidMuonSelectorSUSY is a sub-set of MuidMuonSelectorSUSY_forEMu, with tighter pT cut
MuidMuonSelectorSUSY = D2PDMuonSelector( "MuidMuonSelectorSUSY",
                                         inputCollection   = 'SelectedMuidMuonLinks_forEMu',
                                         outputLinkCollection  = 'SelectedMuidMuonLinks',
                                         ptMin  = 12.0*Units.GeV,
                                         minNumberPassed = 1
                                         )

#MuidDiMuonSelectorSUSY is a sub-set of MuidMuonSelectorSUSY_forEMu, requiring two muons
MuidDiMuonSelectorSUSY = D2PDMuonSelector( "MuidDiMuonSelectorSUSY",
                                           inputCollection   = 'SelectedMuidMuonLinks_forEMu',
                                           outputLinkCollection  = 'SelectedMuidDiMuonLinks',
                                           minNumberPassed   = 2
                                           )


# Electron filters
import PyUtils.RootUtils as ru
ROOT = ru.import_root()
import cppyy
cppyy.loadDictionary('egammaEnumsDict')
from ROOT import egammaPID
from ROOT import egammaParameters

from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
ElectronSelectorSUSY_forEMu = D2PDElectronSelector( "ElectronSelectorSUSY_forEMu",
                                                    inputCollection      = 'ElectronAODCollection',
                                                    outputLinkCollection = 'SelectedElectronLinks_forEMu',
                                                    electronIsEM         = egammaPID.ElectronMedium,
                                                    #electronAuthor       = egammaParameters.AuthorElectron,
						    #requested by Jovan
                                                    clusterEMCaloLayerEtaMin = -2.5,
                                                    clusterEMCaloLayerEtaMax = 2.5,
                                                    clusterEMCaloLayer   = 2,
                                                    minNumberPassed      = 1,
                                                    etMin                = 6.0*Units.GeV
                                                    )

#ElectronSelectorSUSY is a sub-set of ElectronSelectorSUSY_forEMu, with tighter Et cut
ElectronSelectorSUSY = D2PDElectronSelector( "ElectronSelectorSUSY",
                                             inputCollection      = 'SelectedElectronLinks_forEMu',
                                             outputLinkCollection = 'SelectedElectronLinks',
                                             etMin                = 12.0*Units.GeV,
                                             minNumberPassed =1 
                                             )

#DiElectronSelectorSUSY is a sub-set of ElectronSelectorSUSY_forEMu, requinring two muons
DiElectronSelectorSUSY = D2PDElectronSelector( "DiElectronSelectorSUSY",
                                               inputCollection      = 'SelectedElectronLinks_forEMu',
                                               outputLinkCollection = 'SelectedDiElectronLinks',
                                               minNumberPassed      = 2
                                               )

# Photon filter(s)
from D2PDMaker.D2PDMakerConf import D2PDPhotonSelector
PhotonSelectorSUSY = D2PDPhotonSelector( "PhotonSelectorSUSY",
                                         inputCollection      = 'PhotonAODCollection',
                                         outputLinkCollection = 'SelectedPhotonLinks',
                                         minNumberPassed      = 1,
                                         photonIsEM           = egammaPID.PhotonLoose,
                                         #photonAuthor         = egammaParameters.AuthorPhoton,
					#requested by Jovan 
                                         etMin                = 20.0*Units.GeV,
                                         clusterEMCaloLayer   = 2,
                                         clusterEMCaloLayerEtaMin        = -2.5,
                                         clusterEMCaloLayerEtaMax        = 2.5
                                         )



# Jet filters
from D2PDMaker.D2PDMakerConf import D2PDJetSelector
JetSelectorSUSY = D2PDJetSelector( "JetSelectorSUSY",
                                   inputCollection   = D2PDFlags.InputSUSYJetCollection() ,
                                   outputLinkCollection  = 'AntiKt4H1TopoJets_SelectedJetLinks',
                                   minNumberPassed   = 1,
                                   useJetSignalState = 0,
                                   jetScaledEtMin = 80.0*Units.GeV,
                                   jetSignalStateAbsEtaMax = 3.0
                                   )

DiJetSelectorSUSY = D2PDJetSelector( "DiJetSelectorSUSY",
                                     inputCollection   = D2PDFlags.InputSUSYJetCollection() ,
                                     outputLinkCollection  = 'AntiKt4H1TopoJets_SelectedDijetLinks',
                                     minNumberPassed   = 2,
                                     useJetSignalState = 0,
                                     jetScaledEtMin = 30.0*Units.GeV,
                                     jetSignalStateAbsEtaMax = 3.0
                                     )

#FourJet not used yet, but likely to be soon when dijet filter tightens
FourJetSelectorSUSY = D2PDJetSelector( "FourJetSelectorSUSY",
                                       inputCollection   = D2PDFlags.InputSUSYJetCollection() ,
                                       outputLinkCollection  = 'AntiKt4H1TopoJets_SelectedFourjetLinks',
                                       minNumberPassed   = 4,
                                       useJetSignalState = 0,
                                       jetScaledEtMin = 30.0*Units.GeV,
                                       jetSignalStateAbsEtaMax = 3.0
                                       )



# MET filter(s)
from D2PDMaker.D2PDMakerConf import D2PDMissingEtSelector
METSelectorSUSY = D2PDMissingEtSelector( "METSelectorSUSY",
                                         inputCollection   = 'MET_Topo',
                                         outputCollection  = 'MET_Topo_SUSYselection',
                                         minNumberPassed   = 1,
                                         missingEtMin      = 40.0*Units.GeV
                                         )



# Combined filters
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner

#MuonPlusJet and ElectronPlusJet are not needed anymore! 
#MuonPlusJetSelectorSUSY = LogicalFilterCombiner("MuonPlusJetSelectorSUSY")
#MuonPlusJetSelectorSUSY.cmdstring = "(StacoMuonSelectorSUSY or MuidMuonSelectorSUSY) and DiJetSelectorSUSY_WithLep"

#ElectronPlusJetSelectorSUSY = LogicalFilterCombiner("ElectronPlusJetSelectorSUSY")
#ElectronPlusJetSelectorSUSY.cmdstring = "ElectronSelectorSUSY and DiJetSelectorSUSY_WithLep"

ElectronPlusMuonSelectorSUSY = LogicalFilterCombiner("ElectronPlusMuonSelectorSUSY")
ElectronPlusMuonSelectorSUSY.cmdstring = "(MuidMuonSelectorSUSY_forEMu or StacoMuonSelectorSUSY_forEMu) and ElectronSelectorSUSY_forEMu"

##################################################################################
# Helper functions for the streams
def GetOldV1Setup(seq):
    #This is the same for all streams
    #>0 electron(s)
    if not hasattr(seq,"ElectronSelectorSUSY"): seq+=ElectronSelectorSUSY
    seq.ElectronSelectorSUSY.etMin=10.0*Units.GeV
    #>0 muon(s)
    if not hasattr(seq,"StacoMuonSelectorSUSY"): seq += StacoMuonSelectorSUSY
    if not hasattr(seq,"MuidMuonSelectorSUSY"): seq += MuidMuonSelectorSUSY
    seq.StacoMuonSelectorSUSY.ptMin = 6.0*Units.GeV
    seq.MuidMuonSelectorSUSY.ptMin = 6.0*Units.GeV
    #jets or MET
    if not hasattr(seq,"JetSelectorSUSY"): seq += JetSelectorSUSY
    if not hasattr(seq,"DiJetSelectorSUSY"): seq += DiJetSelectorSUSY
    if not hasattr(seq,"METSelectorSUSY"): seq += METSelectorSUSY
    seq.JetSelectorSUSY.jetSignalStateEtMin=40.0*Units.GeV
    seq.DiJetSelectorSUSY.jetSignalStateEtMin=20.0*Units.GeV
    seq.METSelectorSUSY.missingEtMin=20.0*Units.GeV
    #Then take an OR of all above filters
    return ["ElectronSelectorSUSY","StacoMuonSelectorSUSY","MuidMuonSelectorSUSY","JetSelectorSUSY","DiJetSelectorSUSY","METSelectorSUSY"]
    
def SetupEGammaStreamFilters(seq):
    acceptAlgs=[]
    cutVersion=D2PDFlags.WriteDAOD_SUSYEGammaStream.CutVersion
    if cutVersion=='newest':
        #photon:
        if not hasattr(seq,"PhotonSelectorSUSY"): seq += PhotonSelectorSUSY
        #el+mu (must execute before other selectors because of elementlink nesting (see above))
        if not hasattr(seq,"MuidMuonSelectorSUSY_forEMu"): seq += MuidMuonSelectorSUSY_forEMu
        if not hasattr(seq,"StacoMuonSelectorSUSY_forEMu"): seq += StacoMuonSelectorSUSY_forEMu
        if not hasattr(seq,"ElectronSelectorSUSY_forEMu"): seq += ElectronSelectorSUSY_forEMu
        if not hasattr(seq,"ElectronPlusMuonSelectorSUSY"): seq += ElectronPlusMuonSelectorSUSY
        seq += ElectronSelectorSUSY
        seq.ElectronSelectorSUSY.etMin=12.0*Units.GeV
        #di-electrons
        seq += DiElectronSelectorSUSY
        seq.ElectronSelectorSUSY.OutputLevel=D2PDFlags.VerboseLevel()
        seq.DiElectronSelectorSUSY.OutputLevel=D2PDFlags.VerboseLevel()
        seq.ElectronPlusMuonSelectorSUSY.OutputLevel=D2PDFlags.VerboseLevel()
        seq.PhotonSelectorSUSY.OutputLevel=D2PDFlags.VerboseLevel()
        acceptAlgs=["ElectronSelectorSUSY","DiElectronSelectorSUSY","PhotonSelectorSUSY","ElectronPlusMuonSelectorSUSY"]
    elif cutVersion=='OldV1':
        acceptAlgs = GetOldV1Setup( seq )
    else:
        raise RuntimeError("Unknown cut version '%s'"%cutVersion)

    return acceptAlgs

def SetupMuonsStreamFilters(seq):
    acceptAlgs=[]
    cutVersion=D2PDFlags.WriteDAOD_SUSYMuonsStream.CutVersion
    if cutVersion=='newest':
 	#photon:
        if not hasattr(seq,"PhotonSelectorSUSY"): seq += PhotonSelectorSUSY
        #el+mu (must execute before other selectors because of elementlink nesting (see above))
        if not hasattr(seq,"MuidMuonSelectorSUSY_forEMu"): seq += MuidMuonSelectorSUSY_forEMu
        if not hasattr(seq,"StacoMuonSelectorSUSY_forEMu"): seq += StacoMuonSelectorSUSY_forEMu
        if not hasattr(seq,"ElectronSelectorSUSY_forEMu"): seq += ElectronSelectorSUSY_forEMu
        if not hasattr(seq,"ElectronPlusMuonSelectorSUSY"): seq += ElectronPlusMuonSelectorSUSY        
        seq +=StacoMuonSelectorSUSY
        seq +=MuidMuonSelectorSUSY
        #dimuons
        seq += MuidDiMuonSelectorSUSY
        seq += StacoDiMuonSelectorSUSY
        seq.MuidMuonSelectorSUSY.OutputLevel=D2PDFlags.VerboseLevel()
        seq.StacoMuonSelectorSUSY.OutputLevel=D2PDFlags.VerboseLevel()
        seq.MuidDiMuonSelectorSUSY.OutputLevel=D2PDFlags.VerboseLevel()
        seq.StacoDiMuonSelectorSUSY.OutputLevel=D2PDFlags.VerboseLevel()
        seq.ElectronPlusMuonSelectorSUSY.OutputLevel=D2PDFlags.VerboseLevel()
        seq.PhotonSelectorSUSY.OutputLevel=D2PDFlags.VerboseLevel()

        acceptAlgs=["MuidMuonSelectorSUSY","StacoMuonSelectorSUSY","MuidDiMuonSelectorSUSY","StacoDiMuonSelectorSUSY","ElectronPlusMuonSelectorSUSY","PhotonSelectorSUSY"]
    elif cutVersion=='OldV1':
        acceptAlgs = GetOldV1Setup( seq )
    else:
        raise RuntimeError("Unknown cut version '%s'"%cutVersion)
    return acceptAlgs

def SetupJetsStreamFilters(seq):
    acceptAlgs=[]
    cutVersion=D2PDFlags.WriteDAOD_SUSYJetsStream.CutVersion
    if cutVersion=='newest':
        seq += JetSelectorSUSY
        seq += DiJetSelectorSUSY
        seq += METSelectorSUSY
        #seq += FourJetSelectorSUSY         <--not used yet
        #seq.FourJetSelectorSUSY.OutputLevel=D2PDFlags.VerboseLevel()
        seq.JetSelectorSUSY.OutputLevel=D2PDFlags.VerboseLevel()
        seq.DiJetSelectorSUSY.OutputLevel=D2PDFlags.VerboseLevel()
        seq.METSelectorSUSY.OutputLevel=D2PDFlags.VerboseLevel()
        acceptAlgs=["JetSelectorSUSY","DiJetSelectorSUSY","METSelectorSUSY"]#,"FourJetSelectorSUSY"
    elif cutVersion=='OldV1':
        acceptAlgs = GetOldV1Setup( seq )
    else:
        raise RuntimeError("Unknown cut version '%s'"%cutVersion)
    return acceptAlgs



# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import AthenaCommon.SystemOfUnits as Units
from D2PDMaker.D2PDMakerConf import D2PDMuonSelector, D2PDElectronSelector

from ROOT import egammaPID

class MuonSelector( object ) :

    def __init__( self,
                  name = 'HSG5_MuonSelector',
                  inputCollection = 'MuidMuonCollection',
                  outputLinkCollection = 'HSG5_ElectronLinkCollection',
                  minNumberPassed = 1,
                  ptMin = 18.0,
                  absEtaMax = 2.5,
                  acceptIsCombined = False,
                  acceptIsSegmentTagged = False,
                  doRelPtCone20  = False,
                  relPtCone20Max = 0.15,
                  requireIsLoose = False,
                  requireIsTight = False) :
        
        self.name = name
        self.inputCollection = inputCollection
        self.outputLinkCollection = outputLinkCollection
        self.minNumberPassed = minNumberPassed
        self.ptMin = ptMin
        self.absEtaMax = absEtaMax
        self.acceptIsCombined = acceptIsCombined
        self.acceptIsSegmentTagged = acceptIsSegmentTagged
        self.doRelPtCone20 = doRelPtCone20
        self.relPtCone20Max = relPtCone20Max
        self.muonRequireIsLoose = requireIsLoose
        self.muonRequireIsTight = requireIsTight

    def getMuonSelector ( self, name="", inputCollection="", outputLinkCollection="" ) :
    
        if name!="":
            self.name=name
        if inputCollection!="":
            self.inputCollection=inputCollection
        if outputLinkCollection!="":
            self.outputLinkCollection=outputLinkCollection

        muonParametersList = []
        muonParametersMinList = []
        muonParametersMaxList = []
        muonParametersUseRelativeCutsList = []
        if self.doRelPtCone20:
            muonParametersList.append(17)   # ptcone20
            muonParametersMinList.append(0.)  
            muonParametersMaxList.append(self.relPtCone20Max)
            muonParametersUseRelativeCutsList.append(True)
            
        return D2PDMuonSelector(self.name,
                                inputCollection      = self.inputCollection,
                                outputLinkCollection = self.outputLinkCollection,
                                minNumberPassed      = self.minNumberPassed,
                                ptMin                = self.ptMin*Units.GeV,
                                absEtaMax            = self.absEtaMax,
                                muonAcceptIsCombined = self.acceptIsCombined,
                                muonAcceptIsSegmentTagged = self.acceptIsSegmentTagged,
                                muonRequireIsLoose   = self.muonRequireIsLoose,
                                muonRequireIsTight   = self.muonRequireIsTight,
                                muonParametersList = muonParametersList,
                                muonParametersMinList = muonParametersMinList,
                                muonParametersMaxList = muonParametersMaxList,
                                muonParametersUseRelativeCutsList = muonParametersUseRelativeCutsList )
    
    
class ElectronSelector( object ) :
    
    def __init__( self,
                  name = 'HSG5_ElectronSelector',
                  outputLinkCollection = 'HSG5_ElectronLinkCollection',
                  minNumberPassed = 1,
                  ptMin = 20.0,
                  clusterEtaMax = 2.5,
                  requireTight = False) :

        self.name = name
        self.outputLinkCollection = outputLinkCollection
        self.minNumberPassed = minNumberPassed
        self.ptMin = ptMin
        self.clusterEtaMax = clusterEtaMax
        self.requireTight = requireTight

    def getElectronSelector ( self, name="", outputLinkCollection="" ) :
        
        if name!="":
            self.name=name
        if outputLinkCollection!="":
            self.outputLinkCollection=outputLinkCollection
        
        electronIsEM = egammaPID.isEM
        if self.requireTight:
            electronIsEM = egammaPID.ElectronTight

        return D2PDElectronSelector( self.name,
                                     inputCollection      = 'ElectronAODCollection',
                                     outputLinkCollection = self.outputLinkCollection,
                                     minNumberPassed      = self.minNumberPassed,
                                     ptMin                = self.ptMin*Units.GeV,
                                     clusterEtaMax        = self.clusterEtaMax,
                                     electronIsEM         = electronIsEM )


class JetSelector( object ) :
    
    def __init__( self,
                  name = 'HSG5_JetSelector',
                  inputCollection = 'AntiKt4TopoEMJets',
                  outputLinkCollection = 'HSG5_JetLinkCollection',
                  minNumberPassed = 1,
                  ptMin = 20.0,
                  etaMax = 2.5,
                  emVeto = False,
                  emCollection = 'HSG5_ElectronLinkCollection',
                  emJetDeltaRMax = 0.3) :
        
        self.name = name
        self.inputCollection = inputCollection,
        self.outputLinkCollection = 'HSG5_JetLinkCollection'
        self.minNumberPassed = minNumberPassed
        self.ptMin = ptMin
        self.etaMax = 2.5
        self.emVeto = emVeto
        self.emCollection = emCollection
        self.emJetDeltaRMax = emJetDeltaRMax

    def getJetSelector( self, name="", outputLinkCollection="") :
        
        if name!="":
            self.name=name
        if outputLinkCollection!="":
            self.outputLinkCollection=outputLinkCollection

            
        outputAssociationContainerList = []
        numberOfAssociationsContainerList = []
        
        if self.emVeto:
            from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
            ToolSvc += DeltaRAssociationTool( self.name+"_emDeltaRAssocTool",
                                              OutputLevel = INFO,
                                              inputAssociateToCollection = self.emCollection,
                                              deltaRMax = self.emJetDeltaRMax,
                                              writeUserData = False)
            outputAssociationContainerList.append(self.name+"_emDeltaRAssocTool")            
            numberOfAssociationsContainerList.append(0)

        return D2PDJetSelector( self.name,
                                inputCollection      = self.inputCollection,
                                outputLinkCollection = self.outputLinkCollection,
                                associationToolList = [ ToolSvc.HSG5WHU_emDeltaRAssociationTool ],
                                outputAssociationContainerList = outputAssociationContainerList,
                                numberOfAssociationsMaxCutList = numberOfAssociationsMaxCutList,
                                minNumberPassed      = self.minNumberPassed,
                                ptMin                = self.ptMin*Units.GeV,
                                etaMax               = self.etaMax)

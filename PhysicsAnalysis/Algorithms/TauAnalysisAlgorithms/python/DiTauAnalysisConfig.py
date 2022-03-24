# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnalysisAlgorithmsConfig.ConfigBlock import ConfigBlock
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool


class DiTauCalibrationConfig (ConfigBlock):
    """the ConfigBlock for the tau four-momentum correction"""

    def __init__ (self, containerName, postfix) :
        super (DiTauCalibrationConfig, self).__init__ ()
        self.containerName = containerName
        self.postfix = postfix
        self.rerunTruthMatching = True

    def collectReferences (self, config) :
        self._tauRef1 = config.updateRef (self.containerName, "DiTauJets")


    def makeAlgs (self, config) :

        postfix = self.postfix
        if postfix != '' and postfix[0] != '_' :
            postfix = '_' + postfix

        # Set up the tau 4-momentum smearing algorithm:
        alg = createAlgorithm( 'CP::DiTauSmearingAlg', 'DiTauSmearingAlg' + postfix )
        addPrivateTool( alg, 'smearingTool', 'TauAnalysisTools::DiTauSmearingTool' )
        alg.taus = self._tauRef1.input()
        alg.tausOut = self._tauRef1.output()
        alg.preselection = config.getSelection (self.containerName, '')
        config.addAlg (alg)

        # Set up the tau truth matching algorithm:
        if self.rerunTruthMatching and config.dataType() != 'data':
            alg = createAlgorithm( 'CP::DiTauTruthMatchingAlg',
                                   'DiTauTruthMatchingAlg' + postfix )
            addPrivateTool( alg, 'matchingTool',
                            'TauAnalysisTools::DiTauTruthMatchingTool' )
            alg.matchingTool.WriteTruthTaus = 1
            alg.taus = self._tauRef1.input()
            alg.preselection = config.getSelection (self.containerName, '')
            config.addAlg (alg)





class DiTauWorkingPointConfig (ConfigBlock) :
    """the ConfigBlock for the tau working point

    This may at some point be split into multiple blocks (16 Mar 22)."""

    def __init__ (self, containerName, postfix, quality) :
        super (DiTauWorkingPointConfig, self).__init__ ()
        self.containerName = containerName
        self.selectionName = postfix
        self.postfix = postfix
        self.quality = quality
        self.legacyRecommendations = False

    def collectReferences (self, config) :
        if config.dataType() != 'data':
            self._tauRef1 = config.readRef (self.containerName)


    def makeAlgs (self, config) :

        postfix = self.postfix
        if postfix != '' and postfix[0] != '_' :
            postfix = '_' + postfix

        # using enum value from: https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/TauID/TauAnalysisTools/TauAnalysisTools/Enums.h
        # the dictionary is missing in Athena, so hard-coding values here
        if self.quality == 'Tight' :
            IDLevel = 4 # ROOT.TauAnalysisTools.JETIDBDTTIGHT
        elif self.quality == 'Medium' :
            IDLevel = 3 # ROOT.TauAnalysisTools.JETIDBDTMEDIUM
        elif self.quality == 'Loose' :
            IDLevel = 2 # ROOT.TauAnalysisTools.JETIDBDTLOOSE
        else :
            raise ValueError ("invalid tau quality: \"" + self.quality +
                              "\", allowed values are Tight, Medium, Loose")


        # Set up the algorithm calculating the efficiency scale factors for the
        # taus:
        if config.dataType() != 'data':
            alg = createAlgorithm( 'CP::DiTauEfficiencyCorrectionsAlg',
                                   'DiTauEfficiencyCorrectionsAlg' + postfix )
            addPrivateTool( alg, 'efficiencyCorrectionsTool',
                            'TauAnalysisTools::DiTauEfficiencyCorrectionsTool' )
            alg.efficiencyCorrectionsTool.IDLevel = IDLevel
            alg.scaleFactorDecoration = 'tau_effSF' + postfix
            # alg.outOfValidity = 2 #silent
            # alg.outOfValidityDeco = "bad_eff"
            alg.taus = self._tauRef1.input()
            alg.preselection = config.getSelection (self.containerName, self.selectionName)
            config.addAlg (alg)





def makeDiTauCalibrationConfig( seq, containerName, postfix = '',
                              rerunTruthMatching = True):
    """Create tau calibration analysis algorithms

    This makes all the algorithms that need to be run first befor
    all working point specific algorithms and that can be shared
    between the working points.

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is mostly used/needed when using this
                 sequence with multiple working points to ensure all
                 names are unique.
      rerunTruthMatching -- Whether or not to rerun truth matching
    """

    config = DiTauCalibrationConfig (containerName, postfix)
    config.rerunTruthMatching = rerunTruthMatching
    seq.append (config)





def makeDiTauWorkingPointConfig( seq, containerName, workingPoint, postfix,
                               legacyRecommendations = False):
    """Create tau analysis algorithms for a single working point

    Keyword arguments:
      dataType -- The data type to run on ("data", "mc" or "afii")
      legacyRecommendations -- use legacy tau BDT and electron veto recommendations
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is mostly used/needed when using this
                 sequence with multiple working points to ensure all
                 names are unique.
    """

    splitWP = workingPoint.split ('.')
    if len (splitWP) != 1 :
        raise ValueError ('working point should be of format "quality", not ' + workingPoint)

    config = DiTauWorkingPointConfig (containerName, postfix, splitWP[0])
    config.legacyRecommendations = legacyRecommendations
    seq.append (config)

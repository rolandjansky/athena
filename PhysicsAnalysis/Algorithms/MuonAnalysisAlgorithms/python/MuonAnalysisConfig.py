# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnaAlgorithm.DualUseConfig import createAlgorithm, addPrivateTool
from AnalysisAlgorithmsConfig.ConfigBlock import ConfigBlock
import ROOT


class MuonCalibrationConfig (ConfigBlock):
    """the ConfigBlock for the muon four-momentum correction"""

    def __init__ (self, containerName, postfix) :
        super (MuonCalibrationConfig, self).__init__ ()
        self.containerName = containerName
        self.postfix = postfix
        self.ptSelectionOutput = False

    def collectReferences (self, config) :
        self._muonRef1 = config.readOrUpdateRef (self.containerName, "Muons")
        self._muonRef2 = config.updateRef (self.containerName, "Muons")

    def makeAlgs (self, config) :
        # Set up the eta-cut on all muons prior to everything else
        alg = createAlgorithm( 'CP::AsgSelectionAlg',
                               'MuonEtaCutAlg' + self.postfix )
        addPrivateTool( alg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
        alg.selectionTool.maxEta = 2.5
        alg.selectionDecoration = 'selectEta' + self.postfix + ',as_bits'
        alg.particles = self._muonRef1.input()
        if self._muonRef1.wantCopy() :
            alg.particlesOut = self._muonRef1.output()
        alg.preselection = config.getSelection (self.containerName, '')
        config.addSelection (self.containerName, '', alg.selectionDecoration, 2)
        config.addSelection (self.containerName, 'output', alg.selectionDecoration, 2)
        config.addAlg (alg)

        # Set up the track selection algorithm:
        alg = createAlgorithm( 'CP::AsgLeptonTrackSelectionAlg',
                               'MuonTrackSelectionAlg' + self.postfix )
        alg.selectionDecoration = 'trackSelection' + self.postfix + ',as_bits'
        alg.maxD0Significance = 3
        alg.maxDeltaZ0SinTheta = 0.5
        alg.particles = self._muonRef2.input()
        alg.preselection = config.getSelection (self.containerName, '')
        config.addSelection (self.containerName, '', alg.selectionDecoration, 3)
        config.addSelection (self.containerName, 'output', alg.selectionDecoration, 3)
        config.addAlg (alg)

        # Set up the muon calibration and smearing algorithm:
        alg = createAlgorithm( 'CP::MuonCalibrationAndSmearingAlg',
                               'MuonCalibrationAndSmearingAlg' + self.postfix )
        addPrivateTool( alg, 'calibrationAndSmearingTool',
                        'CP::MuonCalibrationPeriodTool' )
        alg.calibrationAndSmearingTool.calibrationMode = 2 # choose ID+MS with no sagitta bias
        alg.muons = self._muonRef2.input()
        alg.muonsOut = self._muonRef2.output()
        alg.preselection = config.getSelection (self.containerName, '')
        config.addAlg (alg)

        # Set up the the pt selection
        alg = createAlgorithm( 'CP::AsgSelectionAlg', 'MuonPtCutAlg' + self.postfix )
        alg.selectionDecoration = 'selectPt' + self.postfix + ',as_bits'
        addPrivateTool( alg, 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
        alg.particles = self._muonRef2.output()
        alg.selectionTool.minPt = 3e3
        alg.preselection = config.getSelection (self.containerName, '')
        config.addSelection (self.containerName, '', alg.selectionDecoration, 2)
        if self.ptSelectionOutput :
            config.addSelection (self.containerName, 'output', alg.selectionDecoration, 2)
        config.addAlg (alg)



class MuonWorkingPointConfig (ConfigBlock) :
    """the ConfigBlock for the muon working point

    This may at some point be split into multiple blocks (10 Mar 22)."""

    def __init__ (self, containerName, postfix, quality, isolation) :
        super (MuonWorkingPointConfig, self).__init__ ()
        self.containerName = containerName
        self.selectionName = postfix
        self.postfix = postfix
        self.quality = quality
        self.isolation = isolation
        self.qualitySelectionOutput = False

    def collectReferences (self, config) :
        self._muonRef1 = config.readRef (self.containerName)

    def makeAlgs (self, config) :

        if self.quality == 'Tight' :
            quality = ROOT.xAOD.Muon.Tight
        elif self.quality == 'Medium' :
            quality = ROOT.xAOD.Muon.Medium
        elif self.quality == 'Loose' :
            quality = ROOT.xAOD.Muon.Loose
        elif self.quality == 'VeryLoose' :
            quality = ROOT.xAOD.Muon.VeryLoose
        elif self.quality == 'HighPt' :
            quality = 4
        elif self.quality == 'LowPtEfficiency' :
            quality = 5
        else :
            raise ValueError ("invalid muon quality: \"" + self.quality +
                              "\", allowed values are Tight, Medium, Loose, " +
                              "VeryLoose, HighPt, LowPtEfficiency")

        postfix = self.postfix
        if postfix != '' and postfix[0] != '_' :
            postfix = '_' + postfix

        if self.isolation not in ["Iso", "NonIso"] :
            raise ValueError ('invalid muon isolation \"' + self.isolation +
                              '\", allowed values are Iso, NonIso')

        # Setup the muon quality selection
        alg = createAlgorithm( 'CP::MuonSelectionAlgV2',
                               'MuonSelectionAlg' + postfix )
        addPrivateTool( alg, 'selectionTool', 'CP::MuonSelectionTool' )
        alg.selectionTool.MuQuality = quality
        alg.selectionDecoration = 'good_muon' + postfix + ',as_bits'
        alg.badMuonVetoDecoration = 'is_bad' + postfix + ',as_char'
        alg.muons = self._muonRef1.input()
        alg.preselection = config.getSelection (self.containerName, self.selectionName)
        config.addSelection (self.containerName, self.selectionName, alg.selectionDecoration, 4)
        if self.qualitySelectionOutput:
            config.addSelection (self.containerName, 'output', alg.selectionDecoration, 4)
        config.addAlg (alg)

        # Set up the isolation calculation algorithm:
        if self.isolation != 'NonIso' :
            alg = createAlgorithm( 'CP::MuonIsolationAlg',
                                   'MuonIsolationAlg' + postfix )
            addPrivateTool( alg, 'isolationTool', 'CP::IsolationSelectionTool' )
            alg.isolationDecoration = 'isolated_muon' + postfix + ',as_bits'
            alg.muons = self._muonRef1.input()
            alg.preselection = config.getSelection (self.containerName, self.selectionName)
            config.addSelection (self.containerName, self.selectionName, alg.isolationDecoration, 1)
            if self.qualitySelectionOutput:
                config.addSelection (self.containerName, 'output', alg.isolationDecoration, 1)
            config.addAlg (alg)

        # Set up the efficiency scale factor calculation algorithm:
        if config.dataType() != 'data':
            alg = createAlgorithm( 'CP::MuonEfficiencyScaleFactorAlg',
                                   'MuonEfficiencyScaleFactorAlg' + postfix )
            addPrivateTool( alg, 'efficiencyScaleFactorTool',
                            'CP::MuonEfficiencyScaleFactors' )
            alg.scaleFactorDecoration = 'muon_effSF' + postfix + "_%SYS%"
            alg.outOfValidity = 2 #silent
            alg.outOfValidityDeco = 'bad_eff' + postfix
            alg.efficiencyScaleFactorTool.WorkingPoint = self.quality
            alg.muons = self._muonRef1.input()
            alg.preselection = config.getSelection (self.containerName, self.selectionName)
            config.addAlg (alg)





def makeMuonCalibrationConfig( seq, containerName,
                               postfix = '', ptSelectionOutput = False):
    """Create muon calibration analysis algorithms

    This makes all the algorithms that need to be run first befor
    all working point specific algorithms and that can be shared
    between the working points.

    Keyword arguments:
      containerName -- name of the output container
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is mostly used/needed when using this
                 sequence with multiple working points to ensure all
                 names are unique.
      ptSelectionOutput -- Whether or not to apply pt selection when creating
                           output containers.
    """

    config = MuonCalibrationConfig (containerName, postfix)
    config.ptSelectionOutput = ptSelectionOutput
    seq.append (config)





def makeMuonWorkingPointConfig( seq, containerName, workingPoint, postfix,
                                qualitySelectionOutput = True):
    """Create muon analysis algorithms for a single working point

    Keyword arguments:
      workingPoint -- The working point to use
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is mostly used/needed when using this
                 sequence with multiple working points to ensure all
                 names are unique.
      qualitySelectionOutput -- Whether or not to apply muon quality selection
                                when creating output containers.
    """

    splitWP = workingPoint.split ('.')
    if len (splitWP) != 2 :
        raise ValueError ('working point should be of format "quality.isolation", not ' + workingPoint)

    config = MuonWorkingPointConfig (containerName, postfix, splitWP[0], splitWP[1])
    config.qualitySelectionOutput = qualitySelectionOutput
    seq.append (config)

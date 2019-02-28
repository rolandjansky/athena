# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class _BTaggingFlags:

# default is analysis
    StandardTaggers = \
      [ 'IP2D', 'IP3D', 'MultiSVbb1',  'MultiSVbb2', 'SV1', 'JetFitterNN',
        'SoftMu', 'MV2c10', 'MV2r', 'MV2rmu', 'JetVertexCharge',
        'MV2c100', 'MV2cl100', 'MVb', 'DL1', 'DL1r', 'DL1rmu',  'RNNIP','DL1mu' ]

    ExpertTaggers = \
      StandardTaggers + \
      [ 'SV1Flip', 'JetFitterNNFlip', 'IP2DNeg', 'IP3DNeg',
        'MV2c10Flip', 'MV2c100Flip', 'DL1Flip', 'RNNIPFlip', 'RNNIPNeg',
        'DL1rFlip', 'MV2rFlip']
      # do we have these already?
      # 'DL1muFlip', 'RNNIPFlip', 'DL1rnnFlip'



    _Active = [ 'Active' ]

    _mode = [ 'Runmodus'  ]

    _DecorateMvaInputs = [ 'DecorateMvaInputs' ]

    _ReferenceType = [ 'ReferenceType' ]

    _JetPtMinRef = [ 'JetPtMinRef' ]

    _tags = [ 'IP1D',
              'IP2D', 'IP2DFlip', 'IP2DPos', 'IP2DNeg', 'IP2DSpc', 'IP2DSpcFlip', 'IP2DSpcPos', 'IP2DSpcNeg',
              'IP3D', 'IP3DFlip', 'IP3DPos', 'IP3DNeg', 'IP3DSpc', 'IP3DSpcFlip', 'IP3DSpcPos', 'IP3DSpcNeg',
              'SV0', 'MultiSVbb1','MultiSVbb2',
              'SV1', 'SV1Flip',
              'SV2', 'SV2Flip',
              'JetProb', 'JetProbFlip',
              'IPFordG', 'IPFordGNeg',
              'SoftEl',
              'SoftMu', 'SoftMuChi2',
              'BasicJetFitter', 'JetFitterTag', 'JetFitterTagFlip','JetFitterNN', 'JetFitterCOMBNN', 'JetFitterNNFlip',
              'TrackCounting', 'TrackCountingFlip',
              'QGTag',
              'MV1', 'MV1c', 'MV2', 'MV2c10', 'MV2c20', 'MV2c00', 'MV2c10hp', 'MV2m',
              'MV2rmu','MV2r','MV2c100','MV2cl100',
              'MV1Flip', 'MV1cFlip', 'MV2Flip',
              'MV2c10Flip','MV2rFlip','MV2rmuFlip','MV2c100Flip','MV2cl100Flip',
              'RNNIP', 'RNNIPNeg', 'RNNIPFlip',
              'DL1', 'DL1Flip','DL1r', 'DL1rmuFlip','DL1rmu', 'DL1rFlip','DL1mu',
              'TagNtupleDumper',
              'JetVertexCharge']

    _BaselineTagger = [ 'BaselineTagger' ]

    _jetFinderBasedOn = [ 'jetFinderBasedOn' ]

    _doStandardAssoc = [ 'doStandardAssoc' ]

    _jetTruthMatching = [ 'jetTruthMatching' ]

    _TrackParticleCollectionName = [ 'TrackParticleCollectionName' ]

    _TrackParticleTruthCollectionName = [ 'TrackParticleTruthCollectionName' ]

    _PrimaryVertexCollectionName = [ 'PrimaryVertexCollectionName' ]

    _MuonCollectionName = [ 'MuonCollectionName' ]

    _ElectronCollectionName = [ 'ElectronCollectionName' ]

    _PhotonCollectionName = [ 'PhotonCollectionName' ]

    _CalibrationJetFitterFile = [ 'CalibrationJetFitterFile' ]

    _Outputlevel = [ 'OutputLevel' ]

    _ConvertParticleJets = [ 'ConvertParticleJets' ]

    _Jets = [ 'Jets' ]

    _JetsWithInfoPlus = [ 'JetsWithInfoPlus' ]

    _RetagJets = [ 'RetagJets' ]

    _JetsForNtuple = [ 'JetsForNtuple' ]

    _writeSecondaryVertices = [ 'writeSecondaryVertices' ]

    _CalibrationFolderRoot = [ 'CalibrationFolderRoot' ]

    _TrigCalibrationFolderRoot = [ 'TrigCalibrationFolderRoot' ]

    _AODFixCalibrationFolderRoot = [ 'AODFixCalibrationFolderRoot' ]

    _CalibrationFromLocalReplica = [ 'CalibrationFromLocalReplica' ]

    _CalibrationFromCERN = [ 'CalibrationFromCERN' ]

    _CalibrationTag = [ 'CalibrationTag' ]

    _TrigCalibrationTag = [ 'TrigCalibrationTag' ]

    _AODFixCalibrationTag = [ 'AODFixCalibrationTag' ]

    _CalibrationChannelAliases = [ 'CalibrationChannelAliases' ]

    _CalibrationSingleFolder = [ 'CalibrationSingleFolder' ]

    _ForceMV2CalibrationAlias = [ 'ForceMV2CalibrationAlias' ]

    _MV2CalibAlias = [ 'MV2CalibAlias' ]

    _MV2cTrainingConfig = [ 'MV2cTrainingConfig' ]

    _MV2mTrainingConfig = [ 'MV2mTrainingConfig' ]

    _HighPriorityTaggers = [ 'HighPriorityTaggers' ]
    _MediumPriorityTaggers = [ 'MediumPriorityTaggers' ]
    _LowPriorityTaggers = [ 'LowPriorityTaggers' ]

    _TriggerTaggers = [ 'TriggerTaggers' ]

    _doJetTagNtuple = [ 'doJetTagNtuple' ]
    _doJetTagSlimNtuple = [ 'doJetTagSlimNtuple' ]
    _doJetTagEfficiencyNtuple = [ 'doJetTagEfficiencyNtuple' ]
    _doJetTagFakeRateNtuple = [ 'doJetTagFakeRateNtuple' ]
    _JetTagNtupleName = [ 'JetTagNtupleName' ]
    _JetTagSlimNtupleName = [ 'JetTagSlimNtupleName' ]
    _JetTagEfficiencyNtupleName = [ 'JetTagEfficiencyNtupleName' ]
    _JetTagFakeRateNtupleName = [ 'JetTagFakeRateNtupleName' ]
    _UseLatestJetCollsForNtuple = [ 'UseLatestJetCollsForNtuple' ]

    _AutoInspectInputFile = [ 'AutoInspectInputFile' ]

    _ConfigurationScheme = [ 'ConfigurationScheme' ] #Which configuration scheme to use by default.

    DoNotSetupBTagging = False # WOUTER: This is a temporary measure. It allows the JetRec people to test setting up b-tagging from their end without our job options setting it also up (and therefore ending up with a double setup). Do not switch to True unless you know what you are doing.

    # WOUTER: These govern the adding of jet collections to lists that decide which containers get persistified.
    OutputFilesPrefix = "BTagging_"
    OutputFilesSVname = "SecVtx"
    OutputFilesJFVxname = "JFVtx"
    OutputFilesBaseName = "xAOD::BTaggingContainer#"
    OutputFilesBaseAuxName = "xAOD::BTaggingAuxContainer#"
    OutputFilesBaseNameSecVtx = "xAOD::VertexContainer#"
    OutputFilesBaseAuxNameSecVtx = "xAOD::VertexAuxContainer#"
    OutputFilesBaseNameJFSecVtx = "xAOD::BTagVertexContainer#"
    OutputFilesBaseAuxNameJFSecVtx = "xAOD::BTagVertexAuxContainer#"

    btaggingAODList = [ ]

    btaggingESDList = [ ]

    def __init__ (self):
      # NOTE FROM DAN: This way of defining instance variabes is a bit
      # different from how it's done in the rest of the BTaggingFlags,
      # but I don't see the rational for defining everything
      # twice. See https://its.cern.ch/jira/browse/AFT-191 for
      # discussion.
      self.DL1LocalNNConfig = ''
      self.ForceDL1CalibrationAlias = True
      self.DL1CalibAlias = 'AntiKt4EMTopo'
      self.DL1 = True
      self.DL1Flip = False

      self.DL1mu = True

      self.DL1rmu = True
      self.DL1rmuFlip = False

      self.DL1r = True
      self.DL1rFlip = False

      self.TagNtupleDumper = False
      self.TagNtupleStream = 'FTAG'

      self.RNNIP = True
      self.RNNIPNeg = False
      self.RNNIPFlip = False
      self.RNNIPConfig = {'rnnip':''}
      self.WriteRNNInputs = False

      rnnip_outputs = ['b','c','u','tau']
      self.MultivariateTagManagerAuxBranches=[
          'rnnip_p' + x for x in rnnip_outputs
      ]
      self.MultivariateTagManagerAuxBranches+= ['SMT_discriminant']
      self.MultivariateTagManagerAuxNameMap = {
          'rnnipneg_p' + x: 'rnnip_p' + x for x in rnnip_outputs
      }
      self.MultivariateTagManagerAuxNameMap.update({
          'rnnipflip_p' + x: 'rnnip_p' + x for x in rnnip_outputs
      })
      self.MultivariateFlipTagManagerAuxBranches = [
          'rnnipflip_p' + x for x in rnnip_outputs
      ]

      # you can force the NN tool off with this flag (avoids loading
      # old jetfitter nns which sometimes crash
      self.RunJetFitterNNTool = True

      for attr in self._Active:
        setattr(self, attr, True)

      for attr in self._mode:
        setattr(self, attr, 'analysis')

      for attr in self._DecorateMvaInputs:
        setattr(self, attr, False)

      for attr in self._ReferenceType:
        setattr(self, attr, 'ALL')
      for attr in self._JetPtMinRef:
        setattr(self, attr, 15e3) # in MeV for uncalibrated pt

      for attr in self._tags:
        if attr == 'IP1D':
          setattr(self, attr, False)
        if attr == 'IP2D':
          setattr(self, attr, True)
        if attr == 'IP2DFlip':
          setattr(self, attr, False)
        if attr == 'IP2DPos':
          setattr(self, attr, False)
        if attr == 'IP2DNeg':
          setattr(self, attr, False)
        if attr == 'IP2DSpc':
          setattr(self, attr, False)
        if attr == 'IP2DSpcFlip':
          setattr(self, attr, False)
        if attr == 'IP2DSpcPos':
          setattr(self, attr, False)
        if attr == 'IP2DSpcNeg':
          setattr(self, attr, False)
        if attr == 'IP3D':
          setattr(self, attr, True)
        if attr == 'IP3DFlip':
          setattr(self, attr, False)
        if attr == 'IP3DPos':
          setattr(self, attr, False)
        if attr == 'IP3DNeg':
          setattr(self, attr, False)
        if attr == 'IP3DSpc':
          setattr(self, attr, False)
        if attr == 'IP3DSpcFlip':
          setattr(self, attr, False)
        if attr == 'IP3DSpcPos':
          setattr(self, attr, False)
        if attr == 'IP3DSpcNeg':
          setattr(self, attr, False)
        if attr == 'SV0':
          setattr(self, attr, False)
        if attr == 'MultiSVbb1':
          setattr(self, attr, True)
        if attr == 'MultiSVbb2':
          setattr(self, attr, True)
        if attr == 'SV1':
          setattr(self, attr, True)
        if attr == 'SV1Flip':
          setattr(self, attr, False)
        if attr == 'SV2':
          setattr(self, attr, False)
        if attr == 'SV2Flip':
          setattr(self, attr, False)
        if attr == 'JetProb':
          setattr(self, attr, False)
        if attr == 'JetProbFlip':
          setattr(self, attr, False)
        if attr == 'SoftEl':
          setattr(self, attr, False)
        if attr == 'SoftMu':
          setattr(self, attr, True)
        if attr == 'SoftMuChi2':
          setattr(self, attr, False)
        if attr == 'BasicJetFitter':
          setattr(self, attr, False)
        if attr == 'JetFitterTag':
          setattr(self, attr, False)
        if attr == 'JetFitterNN':
          setattr(self, attr, True)
        if attr == 'JetFitterTagFlip':
          setattr(self, attr, False)
        if attr == 'JetFitterNNFlip':
          setattr(self, attr, False)
        if attr == 'JetFitterCOMBNN':
          setattr(self, attr, False)
        if attr == 'IPFordG':
          setattr(self, attr, False)
        if attr == 'IPFordGNeg':
          setattr(self, attr, False)
        if attr == 'TrackCounting':
          setattr(self, attr, False)
        if attr == 'TrackCountingFlip':
          setattr(self, attr, False)
        if attr == 'QGTag':
          setattr(self, attr, False)
        if attr == 'MV1':
          setattr(self, attr, False)
        if attr == 'MV1c':
          setattr(self, attr, False)
        if attr == 'MV2c00':
          setattr(self, attr, False)
        if attr == 'MV2c10':
          setattr(self, attr, True)
        if attr == 'MV2rmu':
          setattr(self, attr, True)
        if attr == 'MV2c20':
          setattr(self, attr, False)
        if attr == 'MV2c100':
          setattr(self, attr, True)
        if attr == 'MV2r':
          setattr(self, attr, True)
        if attr == 'MV2cl100':
          setattr(self, attr, True)
        if attr == 'MV2m':
          setattr(self, attr, False)
        if attr == 'MV2c10hp':
          setattr(self, attr, False)
        if attr == 'MV2c00Flip':
          setattr(self, attr, False)
        if attr == 'MV2c10Flip':
          setattr(self, attr, False)
        if attr == 'MV2rmuFlip':
          setattr(self, attr, False)
        if attr == 'MV2rFlip':
          setattr(self, attr, False)
        if attr == 'MV2c20Flip':
          setattr(self, attr, False)
        if attr == 'MV2c100Flip':
          setattr(self, attr, False)
        if attr == 'MV2cl100Flip':
          setattr(self, attr, False)
        if attr == 'MV2mFlip':
          setattr(self, attr, False)
        if attr == 'MV2c10hpFlip':
          setattr(self, attr, False)
        if attr == 'MV2':
          setattr(self, attr, False)
        if attr == 'MV1Flip':
          setattr(self, attr, False)
        if attr == 'MV1cFlip':
          setattr(self, attr, False)
        if attr == 'MV2Flip':
          setattr(self, attr, False)
        if attr == 'JetVertexCharge':
          setattr(self, attr, True)


      for attr in self._BaselineTagger:
          setattr(self, attr, 'MV1')

      for attr in self._HighPriorityTaggers:
        setattr(self, attr, ['IP3D','SV1','BasicJetFitter','JetFitterTag','JetFitterNN','MV2c10','MV2r','MV2c100','MV2rmu','MV2cl100', 'IP3DFlip','IP3DPos','IP3DNeg','IP3DSpc','IP3DSpcPos','IP3DSpcNeg','SV1Flip','JetFitterTagFlip','JetFitterNNFlip','MV1Flip','MV1cFlip','MV2c10Flip','MV2rmuFlip','MV2rFlip','MV2c100Flip','MV2cl100Flip','JetVertexCharge', 'DL1', 'DL1Flip','DL1rmu', 'DL1rmuFlip','DL1r', 'DL1rFlip','DL1mu', 'SoftMu'])
      for attr in self._MediumPriorityTaggers:
        setattr(self, attr, ['SV0','IP2D','IP2DFlip','IP2DPos','IP2DNeg','IP2DSpc','IP2DSpcPos','IP2DSpcNeg','SoftEl','SoftMuChi2','MV2','MV2Flip'])
      for attr in self._LowPriorityTaggers:
        setattr(self, attr, ['IP1D','SV2','SV2Flip','TrackCounting','TrackCountingFlip','JetProb','JetProbFlip'])

      for attr in self._TriggerTaggers:
        setattr(self, attr, ['IP2D','IP3D','SV1','MV2c00','MV2c10','MV2c20'])

      for attr in self._jetFinderBasedOn:
        setattr(self, attr, 'Cells')

      for attr in self._doStandardAssoc:
        setattr(self, attr, True)

      for attr in self._writeSecondaryVertices:
        setattr(self, attr, False)

      for attr in self._jetTruthMatching:
        setattr(self, attr, 'matchQuark')

      for attr in self._TrackParticleCollectionName:
        setattr(self, attr, 'InDetTrackParticles')

      for attr in self._TrackParticleTruthCollectionName:
        setattr(self, attr, 'TrackParticleTruthCandidate')

      for attr in self._PrimaryVertexCollectionName:
        setattr(self, attr, 'PrimaryVertices')

      for attr in self._MuonCollectionName:
        setattr(self, attr, 'Muons')

      for attr in self._ElectronCollectionName:
        setattr(self, attr, 'ElectronAODCollection')

      for attr in self._PhotonCollectionName:
        setattr(self, attr, 'PhotonAODCollection')

      for attr in self._CalibrationJetFitterFile:
        setattr(self, attr, 'JetFitterCalibrationCoefficientsRel13')

      for attr in self._ForceMV2CalibrationAlias:
        setattr(self, attr, False)

      for attr in self._MV2CalibAlias:
        setattr(self, attr, 'AntiKt4EMTopo')

      for attr in self._MV2cTrainingConfig:
        setattr(self, attr, 'NoJF_NoSV0NoSv1_V3')

      for attr in self._MV2mTrainingConfig:
        setattr(self, attr, 'NoJF_NoSV0NoSv1_V3')

      for attr in self._Outputlevel:
        from AthenaCommon.Constants import INFO
        setattr(self, attr, INFO)

      for attr in self._ConvertParticleJets:
        setattr(self, attr, False)


      #defaultJetCollections = ['AntiKt4LCTopo','AntiKt6LCTopo','AntiKt4TopoEM','AntiKt6TopoEM']
      defaultJetCollections = ['AntiKt4EMTopo']

      for attr in self._Jets:
        setattr(self, attr, defaultJetCollections[:])
      for attr in self._JetsWithInfoPlus:
        setattr(self, attr, defaultJetCollections[:])
      for attr in self._RetagJets:
        setattr(self, attr, defaultJetCollections[:])
      for attr in self._JetsForNtuple:
        setattr(self, attr, 'AntiKt4EMTopo')

      for attr in self._CalibrationFolderRoot:
        setattr(self, attr, '/GLOBAL/BTagCalib/')

      for attr in self._TrigCalibrationFolderRoot:
        setattr(self, attr, '/GLOBAL/TrigBTagCalib/')

      for attr in self._AODFixCalibrationFolderRoot:
        setattr(self, attr, '/GLOBAL/TrigBTagCalib/') #FIXME

      for attr in self._CalibrationFromLocalReplica:
        setattr(self, attr, False)

      for attr in self._CalibrationFromCERN:
        setattr(self, attr, False)

      for attr in self._CalibrationTag:
        setattr(self, attr, "")

      for attr in self._TrigCalibrationTag:
        setattr(self, attr, "")

      for attr in self._AODFixCalibrationTag:
        setattr(self, attr, "")

      for attr in self._CalibrationChannelAliases:

        setattr(self, attr, [ "myOwnCollection->AntiKt4TopoEM,AntiKt4EMTopo",
                              "AntiKt4Tower->AntiKt4Tower,AntiKt4H1Tower,AntiKt4TopoEM,AntiKt4EMTopo",
                              "AntiKt4Topo->AntiKt4Topo,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4H1Topo",
                              "AntiKt4LCTopo->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4Topo,AntiKt4H1Topo",
                              "AntiKt6Tower->AntiKt6Tower,AntiKt6H1Tower,AntiKt4TopoEM,AntiKt4EMTopo",
                              "AntiKt6Topo->AntiKt6Topo,AntiKt6TopoEM,AntiKt6H1Topo,AntiKt6H1Tower,AntiKt4TopoEM,AntiKt4EMTopo",
                              "AntiKt6LCTopo->AntiKt6LCTopo,AntiKt6TopoEM,AntiKt6Topo,AntiKt6H1Topo,AntiKt6H1Tower,AntiKt4TopoEM,AntiKt4EMTopo",
                              "AntiKt4TopoEM->AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4H1Topo,AntiKt4LCTopo",
                              "AntiKt6TopoEM->AntiKt6TopoEM,AntiKt6H1Topo,AntiKt6H1Tower,AntiKt4TopoEM,AntiKt4EMTopo",
                              #WOUTER: I added some more aliases here that were previously set up at ./python/BTagging_jobOptions.py. But it cannot
                              #stay there if we want support for JetRec to setup b-tagging from their end.
                              "AntiKt4EMTopo->AntiKt4EMTopo,AntiKt4TopoEM,AntiKt4LCTopo",
                              "AntiKt4LCTopo->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo",
                              "AntiKt4EMTopoOrigin->AntiKt4EMTopoOrigin,AntiKt4EMTopo,AntiKt4TopoEM,AntiKt4LCTopo",
                              "AntiKt4LCTopoOrigin->AntiKt4LCTopoOrigin,AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo",
                              "AntiKt10LCTopo->AntiKt10LCTopo,AntiKt6LCTopo,AntiKt6TopoEM,AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo",
                              "AntiKt10Truth->AntiKt6TopoEM,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                              "AntiKt10TruthWZ->AntiKt10TruthWZ,AntiKt6TopoEM,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                              "AntiKt4Truth->AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                              "AntiKt4TruthWZ->AntiKt4TruthWZ,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                              "AntiKt4Track->AntiKt4Track,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                              "AntiKt3Track->AntiKt3Track,AntiKt4Track,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                              "AntiKt2Track->AntiKt2Track,AntiKt4Track,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                              "AntiKt4EMPFlow->AntiKt4EMPFlow,AntiKt4EMTopo,AntiKt4TopoEM,AntiKt4LCTopo",
                              "AntiKt4EMPFlow_PFlowTune->AntiKt4EMPFlow_PFlowTune,AntiKt4EMTopo,AntiKt4TopoEM,AntiKt4LCTopo",
                              "AntiKt4HI->AntiKt4HI,AntiKt4EMTopo,AntiKt4TopoEM,AntiKt4LCTopo"]) 

      for attr in self._CalibrationSingleFolder:
        setattr(self, attr, True)

      for attr in self._doJetTagNtuple:
          setattr(self,attr,False)
      for attr in self._doJetTagSlimNtuple:
          setattr(self,attr,False)
      for attr in self._doJetTagEfficiencyNtuple:
          setattr(self,attr,False)
      for attr in self._doJetTagFakeRateNtuple:
          setattr(self,attr,False)

      for attr in self._JetTagNtupleName:
          setattr(self,attr,"JetTagNtuple.root")

      for attr in self._JetTagSlimNtupleName:
          setattr(self,attr,"JetTagSlimNtuple.root")

      for attr in self._JetTagEfficiencyNtupleName:
          setattr(self,attr,"JetTagEfficiencyNtuple.root")

      for attr in self._JetTagFakeRateNtupleName:
          setattr(self,attr,"JetTagFakeRateNtuple.root")

      for attr in self._UseLatestJetCollsForNtuple:
          setattr(self,attr,True)

      for attr in self._AutoInspectInputFile:
          setattr(self,attr,True)

      for attr in self._ConfigurationScheme:
          setattr(self,attr,"Default")

    def Print (self):
      if not BTaggingFlags.Active:
        print '#BTAG# b-tagging is OFF'
        return
      print '#BTAG# ---------------------------------------------------------------------------------------------'
      print '#BTAG# BTagging configuration:'
      for attr in self._Outputlevel:
        print '#BTAG# -> Output level: '+str(getattr(self, attr))
      for attr in self._DecorateMvaInputs:
        _tmpdec=getattr(self, attr)
        print '#BTAG# -> attach MVA inputs to the BTagging object: ', _tmpdec
      for attr in self._mode:
        _tmpmode=getattr(self, attr)
        print '#BTAG# -> Running in mode: '+_tmpmode
        if _tmpmode == 'reference':
          for attr in self._ReferenceType:
            print '#BTAG#    reference type: '+getattr(self, attr)
          for attr in self._JetPtMinRef:
            print '#BTAG#    minimum uncalibrated jet pt for reference in MeV: ', getattr(self, attr)
      for attr in self._AutoInspectInputFile:
        print '#BTAG# -> Automatic inspection of input file: '+str(getattr(self, attr))
      for attr in self._jetFinderBasedOn:
        print '#BTAG# -> JetFinder based on: '+getattr(self, attr)
      for attr in self._Jets:
        print '#BTAG# -> List of jet types considered: '+str(getattr(self, attr))
      for attr in self._JetsWithInfoPlus:
        print '#BTAG# -> List of jet types with InfoPlus: '+str(getattr(self, attr))
      for attr in self._ConvertParticleJets:
        if getattr(self, attr):
          print '#BTAG# -> Legacy converter for ParticleJets -> Jets is activated'
      for attr in self._JetsForNtuple:
        print '#BTAG# -> List of jet collections to ntuple: '+str(getattr(self, attr))
      for attr in self._jetTruthMatching:
        print '#BTAG# -> TruthMatching algorithm: '+getattr(self, attr)
      for attr in self._PrimaryVertexCollectionName:
        print '#BTAG# -> Primary vertex collection: '+getattr(self, attr)
      for attr in self._TrackParticleCollectionName:
        print '#BTAG# -> TrackParticle collection: '+getattr(self, attr)
      for attr in self._TrackParticleTruthCollectionName:
        print '#BTAG# -> TrackParticleTruth collection: '+getattr(self, attr)
      for attr in self._MuonCollectionName:
        print '#BTAG# -> Muon collection: '+getattr(self, attr)
      for attr in self._ElectronCollectionName:
        print '#BTAG# -> Electron collection: '+getattr(self, attr)
      for attr in self._PhotonCollectionName:
        print '#BTAG# -> Photon collection: '+getattr(self, attr)
      #These will be updated later in BTagCalibrationBroker_xxx_jobOptions.py,
      # no point in showing wrong values now
      #for attr in self._CalibrationFolderRoot:
      #  print '#BTAG# -> Calibration folder root: '+getattr(self, attr)
      #for attr in self._TrigCalibrationFolderRoot:
      #  print '#BTAG# -> Trigger calibration folder root: '+getattr(self, attr)
      for attr in self._CalibrationSingleFolder:
        if getattr(self, attr):
          print '#BTAG# -> Calibration schema: single actual DB folder + one shadow folder per tagger'
        else:
          print '#BTAG# -> Calibration schema: one actual DB folder per tagger'
      for attr in self._CalibrationFromLocalReplica:
        if getattr(self, attr):
          print '#BTAG# -> Calibration from local user-defined sqlite replica'
        else:
          print '#BTAG# -> Calibration from COOL database',
          for attr in self._CalibrationFromCERN:
            if getattr(self, attr):
              print ' (using CERN Oracle DB. ATTENTION: not for production jobs!)'
            else:
              print ' (using standard replication mechanism)'
      for attr in self._CalibrationTag:
        if getattr(self, attr) == '':
          print '#BTAG# -> Calibration tag: automatic'
        else:
          print '#BTAG# -> Calibration tag: '+getattr(self,attr)
      for attr in self._TrigCalibrationTag:
        if getattr(self, attr) == '':
          print '#BTAG# -> Trigger calibration tag: automatic'
        else:
          print '#BTAG# -> Trigger calibration tag: '+getattr(self,attr)
      for attr in self._AODFixCalibrationTag:
        if getattr(self, attr) == '':
          print '#BTAG# -> AODFix calibration tag: automatic'
        else:
          print '#BTAG# -> AODFix calibration tag: '+getattr(self,attr)
      for attr in self._CalibrationJetFitterFile:
        print '#BTAG# -> JetFitter calibration files: '+getattr(self, attr)
      for attr in self._CalibrationChannelAliases:
        print '#BTAG# -> Calibration channel aliases: '+str(getattr(self, attr))
      for attr in self._writeSecondaryVertices:
        print '#BTAG# -> Store secondary vertices: '+str(getattr(self, attr))
      format = "%25s : %s"
      print '#BTAG# -> List of active tagger flags: ',
      for attr in self._tags:
        if getattr(self, attr):
          print attr,
      print
      print '#BTAG# -> List of active trigger tagger flags: ',
      for attr in self._TriggerTaggers:
        print(" ".join([tag for tag in getattr(self, attr) if (tag in self._tags and getattr(self, tag))]))
      print

      print
      for attr in self._doJetTagNtuple:
          print '#BTAG# -> doJetTagNtuple: '+str(getattr(self,attr))
      for attr in self._JetTagNtupleName:
          print '#BTAG# -> JetTagNtupleName: '+str(getattr(self,attr))
      for attr in self._doJetTagSlimNtuple:
          print '#BTAG# -> doJetTagSlimNtuple: '+str(getattr(self,attr))
      for attr in self._JetTagSlimNtupleName:
          print '#BTAG# -> JetTagSlimNtupleName: '+str(getattr(self,attr))
      for attr in self._UseLatestJetCollsForNtuple:
          print '#BTAG# -> Use latest re-tagged jet collections for ntuple: '+str(getattr(self,attr))

      self.btaggingAODList = list()
      self.btaggingESDList = list()

      # Jet collections not in this list (sans -Jets), and which do not have a calibration
      # alias to one of these, will print a warning during the configuration. This is to help
      # people trying to setup jet collections which we do not support (these would sometimes
      # simply crash during execution without a helpful error remark).
      # The function which uses this list is JetCollectionIsSupported() in
      # ./python/BTaggingConfiguration.py
      self.SupportedJetCollections = ["AntiKt4LCTopo",
                                      "AntiKt4EMTopo",
                                      "AntiKt4Track",
                                      "AntiKt4EMPFlow",
                                      "AntiKt10TruthWZ",
                                      "AntiKt4TruthWZ",
                                      ]

      print '#BTAG# ---------------------------------------------------------------------------------------------'

    def IsEnabled(self, tagger):
      if ( tagger in self._tags):
        return getattr(self, tagger)
      else:
        return False

    def Disable(self, tagger):
      if ( tagger in self._tags):
        setattr(self, tagger, False)

    def Enable(self, tagger):
      if ( tagger in self._tags):
        setattr(self, tagger, True)

BTaggingFlags = _BTaggingFlags()

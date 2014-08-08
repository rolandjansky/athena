# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class _BTaggingFlags:

# default is analysis

    _Active = [ 'Active' ]

    _mode = [ 'Runmodus'  ]

    _ReferenceType = [ 'ReferenceType' ]


    _tags = [ 'lifetime1D', 'lifetime2D', 'lifetime3D', 'secVtxFitBU', 'secVtxFitTD', 'IP1D', 
              'IP2D', 'IP2DFlip', 'IP2DPos', 'IP2DNeg', 'IP2DSpc', 'IP2DSpcFlip', 'IP2DSpcPos', 'IP2DSpcNeg',
              'IP3D', 'IP3DFlip', 'IP3DPos', 'IP3DNeg', 'IP3DSpc', 'IP3DSpcFlip', 'IP3DSpcPos', 'IP3DSpcNeg',
              'SV0', 'MultiSV',
              'SV1', 'SV1Flip', 
              'SV2', 'SV2Flip', 
              'JetProb', 'JetProbFlip', 
              'JetFitterCharm', 'JetFitterCharmNeg', 
              'IPFordG', 'IPFordGNeg', 
              'Gaia', 'GaiaNeg', 
              'SoftEl', 
              'SoftMu', 'SoftMuChi2', 
              'BasicJetFitter', 'JetFitterTag', 'JetFitterTagFlip','JetFitterNN', 'JetFitterNNFlip',
              'TrackCounting', 'TrackCountingFlip',
              'GbbNNTag', 'QGTag', 'NewGbbNNTag',
              'MV1', 'MV1c', 'MV2',
              'MV2c00','MV2c10','MV2c20',
              'MV3_bVSu', 'MV3_bVSc', 'MV3_cVSu',
              'MV1Flip', 'MV1cFlip', 'MV2Flip',
              'MV2c00Flip','MV2c10Flip','MV2c20Flip',
              'MV3_bVSuFlip', 'MV3_bVScFlip', 'MV3_cVSuFlip',
              'MVb','MVbFlip','MVbPrime','MVbPrimeFlip']

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

    _CalibrationFromLocalReplica = [ 'CalibrationFromLocalReplica' ]

    _CalibrationFromCERN = [ 'CalibrationFromCERN' ]

    _CalibrationTag = [ 'CalibrationTag' ]

    _CalibrationChannelAliases = [ 'CalibrationChannelAliases' ]

    _CalibrationSingleFolder = [ 'CalibrationSingleFolder' ]

    _ForceMV2CalibrationAlias = [ 'ForceMV2CalibrationAlias' ]

    _MV2CalibAlias = [ 'MV2CalibAlias' ]

    _ForceMV3CalibrationAlias = [ 'ForceMV3CalibrationAlias' ]

    _MV3CalibAlias = [ 'MV3CalibAlias' ]

    _HighPriorityTaggers = [ 'HighPriorityTaggers' ]
    _MediumPriorityTaggers = [ 'MediumPriorityTaggers' ]
    _LowPriorityTaggers = [ 'LowPriorityTaggers' ]

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

    btaggingAODList = [ ]
    
    btaggingESDList = [ ]

    def __init__ (self):

      for attr in self._Active:
        setattr(self, attr, True)

      for attr in self._mode:
        setattr(self, attr, 'analysis')

      for attr in self._ReferenceType:
        setattr(self, attr, 'ALL')

      for attr in self._tags:
        if attr == 'lifetime1D':
          setattr(self, attr, False)
        if attr == 'lifetime2D':
          setattr(self, attr, False)
        if attr == 'lifetime3D':
          setattr(self, attr, False)
        if attr == 'secVtxFitBU':
          setattr(self, attr, False)
        if attr == 'secVtxFitTD':
          setattr(self, attr, False)
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
          setattr(self, attr, True)
        if attr == 'MultiSV':
          setattr(self, attr, True)
        if attr == 'SV1':
          setattr(self, attr, True)
        if attr == 'SV1Flip':
          setattr(self, attr, False)
        if attr == 'SV2':
          setattr(self, attr, True)
        if attr == 'SV2Flip':
          setattr(self, attr, False)
        if attr == 'JetProb':
          setattr(self, attr, False)
        if attr == 'JetProbFlip':
          setattr(self, attr, False)
        if attr == 'SoftEl':
          setattr(self, attr, False)
        if attr == 'SoftMu':
          setattr(self, attr, False)
        if attr == 'SoftMuChi2':
          setattr(self, attr, True)
        if attr == 'BasicJetFitter':
          setattr(self, attr, True)
        if attr == 'JetFitterTag':
          setattr(self, attr, True)
        if attr == 'JetFitterNN':
          setattr(self, attr, True)
        if attr == 'JetFitterTagFlip':
          setattr(self, attr, False)
        if attr == 'JetFitterNNFlip':
          setattr(self, attr, False)
        if attr == 'JetFitterCharm': 
          setattr(self, attr, False)
        if attr == 'JetFitterCharmNeg': 
          setattr(self, attr, False)
        if attr == 'Gaia': 
          setattr(self, attr, False)
        if attr == 'GaiaNeg': 
          setattr(self, attr, False)
        if attr == 'IPFordG': 
          setattr(self, attr, False)
        if attr == 'IPFordGNeg':
          setattr(self, attr, False)
        if attr == 'TrackCounting':
          setattr(self, attr, False)
        if attr == 'TrackCountingFlip':
          setattr(self, attr, False)
        if attr == 'GbbNNTag':
          setattr(self, attr, True)
        if attr == 'NewGbbNNTag':
          setattr(self, attr, False)
        if attr == 'QGTag':
          setattr(self, attr, False)
        if attr == 'MV1':
          setattr(self, attr, True)
        if attr == 'MV1c':
          setattr(self, attr, True)
        if attr == 'MV2c00':
          setattr(self, attr, True)
        if attr == 'MV2c10':
          setattr(self, attr, True)
        if attr == 'MV2c20':
          setattr(self, attr, True)
        if attr == 'MV2c00Flip':
          setattr(self, attr, False)
        if attr == 'MV2c10Flip':
          setattr(self, attr, False)
        if attr == 'MV2c20Flip':
          setattr(self, attr, False)
        if attr == 'MV2':
          setattr(self, attr, True)
        if attr == 'MV3_bVSu':
          setattr(self, attr, False)
        if attr == 'MV3_bVSc':
          setattr(self, attr, False)
        if attr == 'MV3_cVSu':
          setattr(self, attr, False)
        if attr == 'MV1Flip':
          setattr(self, attr, False)
        if attr == 'MV1cFlip':
          setattr(self, attr, False)
        if attr == 'MV2Flip':
          setattr(self, attr, False)
        if attr == 'MV3_bVSuFlip':
          setattr(self, attr, False)
        if attr == 'MV3_bVScFlip':
          setattr(self, attr, False)
        if attr == 'MV3_cVSuFlip':
          setattr(self, attr, False)
        if attr == 'MVb':
          setattr(self, attr, True)
        if attr == 'MVbFlip':
          setattr(self, attr, False)
        if attr == 'MVbPrime':
          setattr(self, attr, False)
        if attr == 'MVbPrimeFlip':
          setattr(self, attr, False)


      for attr in self._BaselineTagger:
          setattr(self, attr, 'MV1')

      for attr in self._HighPriorityTaggers:
        setattr(self, attr, ['IP3D','SV1','BasicJetFitter','JetFitterTag','JetFitterNN','MV1','MV1c','MV2c00','MV2c10','MV2c20','IP3DFlip','IP3DPos','IP3DNeg','IP3DSpc','IP3DSpcPos','IP3DSpcNeg','SV1Flip','JetFitterTagFlip','JetFitterNNFlip','MV1Flip','MV1cFlip','MV2c00Flip','MV2c10Flip','MV2c20Flip'])
      for attr in self._MediumPriorityTaggers:
        setattr(self, attr, ['SV0','IP2D','IP2DFlip','IP2DPos','IP2DNeg','IP2DSpc','IP2DSpcPos','IP2DSpcNeg','SoftEl','SoftMu','SoftMuChi2','MV2','MV2Flip'])
      for attr in self._LowPriorityTaggers:
        setattr(self, attr, ['IP1D','SV2','SV2Flip','TrackCounting','TrackCountingFlip','JetProb','JetProbFlip','MV3_bVSu','MV3_bVSc','MV3_cVSu','MV3_bVSuFlip','MV3_bVScFlip','MV3_cVSuFlip','MVb','MVbFlip','MVbPrime','MVbPrimeFlip'])

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
        setattr(self, attr, 'VxPrimaryCandidate')

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
        setattr(self, attr, 'AntiKt4TopoEM')

      for attr in self._ForceMV3CalibrationAlias:
        setattr(self, attr, True)

      for attr in self._MV3CalibAlias:
        setattr(self, attr, 'AntiKt4TopoEM')

      for attr in self._Outputlevel:
        from AthenaCommon.Constants import INFO
        setattr(self, attr, INFO)

      for attr in self._ConvertParticleJets:
        setattr(self, attr, False)


      defaultJetCollections = ['AntiKt4LCTopo','AntiKt6LCTopo','AntiKt4TopoEM','AntiKt6TopoEM']


      for attr in self._Jets:
        setattr(self, attr, defaultJetCollections[:])
      for attr in self._JetsWithInfoPlus:
        setattr(self, attr, defaultJetCollections[:])
      for attr in self._RetagJets:
        setattr(self, attr, defaultJetCollections[:])
      for attr in self._JetsForNtuple:
        setattr(self, attr, 'AntiKt4TopoEM')

      for attr in self._CalibrationFolderRoot:
        setattr(self, attr, '/GLOBAL/BTagCalib/')

      for attr in self._CalibrationFromLocalReplica:
        setattr(self, attr, False)

      for attr in self._CalibrationFromCERN:
        setattr(self, attr, False)

      for attr in self._CalibrationTag:
        setattr(self, attr, "")

      for attr in self._CalibrationChannelAliases:

        setattr(self, attr, [ "myOwnCollection->Cone4H1Tower",
                              "AntiKt4Tower->AntiKt4Tower,AntiKt4H1Tower",
                              "AntiKt4Topo->AntiKt4Topo,AntiKt4TopoEM,AntiKt4H1Topo",
                              "AntiKt4LCTopo->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4Topo,AntiKt4H1Topo",
                              "AntiKt6Tower->AntiKt6Tower,AntiKt6H1Tower",
                              "AntiKt6Topo->AntiKt6Topo,AntiKt6TopoEM,AntiKt6H1Topo,AntiKt6H1Tower",
                              "AntiKt6LCTopo->AntiKt6LCTopo,AntiKt6TopoEM,AntiKt6Topo,AntiKt6H1Topo,AntiKt6H1Tower",
                              "AntiKt4TopoEM->AntiKt4TopoEM,AntiKt4H1Topo",
                              "AntiKt6TopoEM->AntiKt6TopoEM,AntiKt6H1Topo,AntiKt6H1Tower"])


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

    def Print (self):
      if not BTaggingFlags.Active:
        print '#BTAG# b-tagging is OFF'
        return
      print '#BTAG# ---------------------------------------------------------------------------------------------'
      print '#BTAG# BTagging configuration:'
      for attr in self._Outputlevel:
        print '#BTAG# -> Output level: '+str(getattr(self, attr))
      for attr in self._mode:
        _tmpmode=getattr(self, attr)
        print '#BTAG# -> Running in mode: '+_tmpmode
        if _tmpmode == 'reference':
          for attr in self._ReferenceType:
            print '#BTAG#    reference type: '+getattr(self, attr)
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
      for attr in self._CalibrationFolderRoot:
        print '#BTAG# -> Calibration folder root: '+getattr(self, attr)
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
      for attr in self._CalibrationJetFitterFile:
        print '#BTAG# -> JetFitter calibration files: '+getattr(self, attr)
      for attr in self._CalibrationChannelAliases:
        print '#BTAG# -> Calibration channel aliases: '+str(getattr(self, attr))
      for attr in self._writeSecondaryVertices:
        print '#BTAG# -> Store secondary vertices: '+str(getattr(self, attr))
      format = "%25s : %s"
      print '#BTAG# -> List of active taggers: ',
      for attr in self._tags:
        if getattr(self, attr):
          print attr,
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

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import MissingETD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject                   import D3PDObject
from D3PDMakerCoreComps.D3PDObject                   import make_SG_D3PDObject
from D3PDMakerCoreComps.resolveSGKey                 import testSGKey
from D3PDMakerConfig.D3PDMakerFlags                  import D3PDMakerFlags
from RecExConfig.RecFlags                            import rec
from MissingETD3PDMaker.MissingETD3PDMakerFlags      import *
from MissingETD3PDMaker.MissingETCompAssociation     import *
from MissingETD3PDMaker.MissingETD3PDMakerConf       import *
from TriggerD3PDMaker.defineTriggerBits              import defineTriggerBits

##The includable block names are all of named the same as the MET storegate key for each MET object.
##additional blocks are of the form MET_RefFinal_Regions, or MET_Topo_EMFracSet
##While only RefFinal or Topo are named below, the object MissingETD3PDObject can handle included blocks
##such as MET_RefEle_Regions or just MET_RefEle.

##see doc/instructions.txt for a full list of available MET blocks

Level1METList = [
    MissingETD3PDMakerFlags.METCryoSGKey(),
    MissingETD3PDMakerFlags.METRefEleSGKey(),
    MissingETD3PDMakerFlags.METRefJetSGKey(),
    MissingETD3PDMakerFlags.METSoftJetsSGKey(),
    MissingETD3PDMakerFlags.METRefMuonSGKey(),
    MissingETD3PDMakerFlags.METRefMuonStacoSGKey(),
    MissingETD3PDMakerFlags.METRefMuonMuidSGKey(),
    MissingETD3PDMakerFlags.METRefMuonsSGKey(),
    MissingETD3PDMakerFlags.METRefGammaSGKey(),
    MissingETD3PDMakerFlags.METRefTauSGKey(),
    MissingETD3PDMakerFlags.METCellOutSGKey(),
    MissingETD3PDMakerFlags.METTrackSGKey(),
    MissingETD3PDMakerFlags.METRefFinalEMSGKey(),
    MissingETD3PDMakerFlags.METRefEleEMSGKey(),
    MissingETD3PDMakerFlags.METRefJetEMSGKey(),
    MissingETD3PDMakerFlags.METSoftJetsEMSGKey(),
    MissingETD3PDMakerFlags.METRefMuonEMSGKey(),
    MissingETD3PDMakerFlags.METRefMuonTrackEMSGKey(),
    MissingETD3PDMakerFlags.METRefGammaEMSGKey(),
    MissingETD3PDMakerFlags.METRefTauEMSGKey(),
    MissingETD3PDMakerFlags.METCellOutEMSGKey()
    ]

Level1MuonMETList = [
    MissingETD3PDMakerFlags.METMuonIsolStacoSGKey(),
    MissingETD3PDMakerFlags.METMuonNonIsolStacoSGKey(),
    MissingETD3PDMakerFlags.METMuonTotalStacoSGKey(),
    MissingETD3PDMakerFlags.METMuonIsolMuidSGKey(),
    MissingETD3PDMakerFlags.METMuonsIsolSGKey(),
    MissingETD3PDMakerFlags.METMuonNonIsolMuidSGKey(),
    MissingETD3PDMakerFlags.METMuonsNonIsolSGKey(),
    MissingETD3PDMakerFlags.METMuonTotalMuidSGKey(),
    MissingETD3PDMakerFlags.METMuonsTotalSGKey(),
    MissingETD3PDMakerFlags.METMuonBoySGKey()
    ]

Level1STVFMETList = [
    'MET_RefFinal_STVF', 'MET_RefGamma_STVF', 'MET_RefEle_STVF',
    'MET_RefTau_STVF', 'MET_RefJet_STVF',
    'MET_RefMuon_Staco_STVF', 'MET_Muon_Isol_Staco_STVF',
    'MET_Muon_NonIsol_Staco_STVF', 'MET_Muon_Total_Staco_STVF',
    'MET_RefMuon_Track_Staco_STVF',
    'MET_CellOut_STVF', 'MET_CellOut_Track_STVF',
    'MET_CellOut_TrackPV_STVF', 'MET_CellOutCorr_STVF',
    'MET_Cryo_STVF'
    ]

Level1SpecialMETList = [
    MissingETD3PDMakerFlags.METRefJetJVFSGKey(),
    MissingETD3PDMakerFlags.METRefJetJVFCutSGKey(),
    MissingETD3PDMakerFlags.METCellOutEflowSTVFSGKey(),
    MissingETD3PDMakerFlags.METCellOutEflowJetAreaSGKey(),
    MissingETD3PDMakerFlags.METCellOutEflowJetAreaJVFSGKey(),
    MissingETD3PDMakerFlags.METCellOutEflowJetAreaRhoEta5JVFSGKey(),
    MissingETD3PDMakerFlags.METRefFinalSTVFSGKey()
    ]

Level2METList = [
    MissingETD3PDMakerFlags.METRefMuonTrackSGKey(),
    MissingETD3PDMakerFlags.METRefMuonTrackStacoSGKey(),
    MissingETD3PDMakerFlags.METRefMuonTrackMuidSGKey(),
    MissingETD3PDMakerFlags.METRefMuonsTrackSGKey(),
    MissingETD3PDMakerFlags.METCryoConeSGKey(),
    MissingETD3PDMakerFlags.METFinalSGKey()
    ]

Level2MuonMETList = [
    MissingETD3PDMakerFlags.METMuonBoySpectroSGKey(),
    MissingETD3PDMakerFlags.METMuonBoyTrackSGKey(),
    MissingETD3PDMakerFlags.METMuonMuidSGKey(),
    MissingETD3PDMakerFlags.METMuidSGKey(),
    MissingETD3PDMakerFlags.METMuidSpectroSGKey(),
    MissingETD3PDMakerFlags.METMuidTrackSGKey(),
    MissingETD3PDMakerFlags.METMuonMuonsSGKey(),
    MissingETD3PDMakerFlags.METMuonsSGKey(),
    MissingETD3PDMakerFlags.METMuonsSpectroSGKey(),
    MissingETD3PDMakerFlags.METMuonsTrackSGKey(),
    MissingETD3PDMakerFlags.METMuonSGKey()
    ]

Level3METList = [
    MissingETD3PDMakerFlags.METTopoObjSGKey(),
    MissingETD3PDMakerFlags.METLocHadTopoObjSGKey()
    ]

Level3CaloMETList = [
    MissingETD3PDMakerFlags.METTopoSGKey(),
    MissingETD3PDMakerFlags.METBaseSGKey(),
    MissingETD3PDMakerFlags.METBase0SGKey(),
    MissingETD3PDMakerFlags.METCorrTopoSGKey(),
    MissingETD3PDMakerFlags.METLocHadTopoSGKey(),
    MissingETD3PDMakerFlags.METCalibSGKey()
    ]

Level3TruthMETList = [
    MissingETD3PDMakerFlags.METDMCrack1SGKey(),
    MissingETD3PDMakerFlags.METDMCrack2SGKey(),
    MissingETD3PDMakerFlags.METDMAllSGKey(),
    MissingETD3PDMakerFlags.METDMCryoSGKey()
    ]

# Hook setting up the STVF MET reconstruction if needed:
def __stvfReconstruction( c, sgkey, **kw ):
    # This flag changes MET results.
    # Make sure that it's always on, if available, so that we get
    # consistent results.
    from JetRec.JetRecFlags import jetFlags
    if hasattr('jetFlags', 'jetPerformanceJob'):
        jetFlags.jetPerformanceJob = True

    # STVF requires AntiKt4LCTopoJets as input.
    # We can't schedule it if we don't have those jets.
    if not testSGKey ('JetCollection', 'AntiKt4LCTopoJets'):
        from AthenaCommon.Logging    import logging
        log = logging.getLogger ('D3PD')
        log.warning ('AntiKt4LCTopoJets not in input; not scheduling STVF.')
        return

    # STVF objects always have a StoreGate key ending in "_STVF".
    # This is just a convention though...
    if sgkey.endswith( "_STVF" ):
        from MissingETD3PDMaker.STVFMETGetter import STVFMETGetter
        STVFMETGetter()
        pass
    return

#Ref final forms the base for the generic MissingET object.
RefFinalMETD3PDObject = \
                    make_SG_D3PDObject ('MissingET',
                                        MissingETD3PDMakerFlags.METRefFinalSGKey(),
                                        'MET_RefFinal_',
                                        'RefFinalMETD3PDObject')
RefFinalMETD3PDObject.defineHook( __stvfReconstruction )
RefFinalMETD3PDObject.defineBlock (0, 'MET',
                                 MissingETD3PDMaker.MissingETFillerTool)
RefFinalMETD3PDObject.defineBlock (0, 'MET_Phi',
                                 MissingETD3PDMaker.MissingETPhiFillerTool)
RefFinalMETD3PDObject.defineBlock (0, 'MET_Et',
                                 MissingETD3PDMaker.ScalarMissingETFillerTool)
RefFinalMETD3PDObject.defineBlock (0, 'MET_SumEt',
                                 MissingETD3PDMaker.SumETFillerTool)
RefFinalMETD3PDObject.defineBlock (0, 'MET_Regions',
                                   MissingETD3PDMaker.MissingETRegionFillerTool)

if D3PDMakerFlags.DoTrigger():
    defineTriggerBits (RefFinalMETD3PDObject, 0,
                       ['EF.*_xe[0-9].*'])
    defineTriggerBits (RefFinalMETD3PDObject, 0,
                       ['L1.*_XE[0-9].*',
                        'L2.*_xe[0-9].*'])

##MET_Topo forms the base for the generic MissingEtCalo object. 
TopoMETD3PDObject = make_SG_D3PDObject ('MissingEtCalo',
                                        MissingETD3PDMakerFlags.METTopoSGKey(),
                                        'MET_Topo_',
                                        'TopoMETD3PDObject')
TopoMETD3PDObject.defineBlock (3, 'MET_Calo',
                               MissingETD3PDMaker.MissingETFillerTool)
TopoMETD3PDObject.defineBlock (3, 'MET_Calo_Phi',
                               MissingETD3PDMaker.MissingETPhiFillerTool)
TopoMETD3PDObject.defineBlock (3, 'MET_Calo_Et',
                               MissingETD3PDMaker.ScalarMissingETFillerTool)
TopoMETD3PDObject.defineBlock (3, 'MET_Calo_SumEt',
                               MissingETD3PDMaker.SumETFillerTool)
TopoMETD3PDObject.defineBlock (3 if MissingETD3PDMakerFlags.doCaloEMFracSumet()
                               else 99,
                               'MET_Calo_EMFracSET',
                               MissingETD3PDMaker.MissingETCaloEMFracSETFillerTool)
TopoMETD3PDObject.defineBlock (3 if MissingETD3PDMakerFlags.doCaloSamplings()
                               else 99,
                               'MET_Calo_Samplings',
                               MissingETD3PDMaker.MissingETCaloSamplingFillerTool)
TopoMETD3PDObject.defineBlock (3 if MissingETD3PDMakerFlags.doCaloRegions()
                               else 99,
                               'MET_Calo_Regions',
                               MissingETD3PDMaker.MissingETRegionFillerTool)

###Truth object, can also be used for customized MET Truth objects
MissingETTruthD3PDObject = \
                   make_SG_D3PDObject ('MissingEtTruth',
                                       MissingETD3PDMakerFlags.METTruthSGKey(),
                                       'MET_Truth_',
                                       'MissingETTruthD3PDObject')
MissingETTruthD3PDObject.defineBlock (1, 'MET_Truth_NonInt',
                           MissingETD3PDMaker.MissingETTruthNonIntFillerTool)
MissingETTruthD3PDObject.defineBlock (1, 'MET_Truth_NonInt_Phi',
                           MissingETD3PDMaker.MissingETTruthNonIntPhiFillerTool)
MissingETTruthD3PDObject.defineBlock (1, 'MET_Truth_NonInt_Et',
                           MissingETD3PDMaker.ScalarMissingETTruthNonIntFillerTool)
MissingETTruthD3PDObject.defineBlock (1, 'MET_Truth_NonInt_SumEt',
                           MissingETD3PDMaker.SumETTruthNonIntFillerTool)
MissingETTruthD3PDObject.defineBlock (3, 'MET_Truth_Int',
                           MissingETD3PDMaker.MissingETTruthIntFillerTool)
MissingETTruthD3PDObject.defineBlock (3, 'MET_Truth_Int_Phi',
                           MissingETD3PDMaker.MissingETTruthIntPhiFillerTool)
MissingETTruthD3PDObject.defineBlock (3, 'MET_Truth_Int_Et',
                           MissingETD3PDMaker.ScalarMissingETTruthIntFillerTool)
MissingETTruthD3PDObject.defineBlock (3, 'MET_Truth_Int_SumEt',
                           MissingETD3PDMaker.SumETTruthIntFillerTool)




#### Level 5 extras - Each of these can be customized too.
JetsInfoMETD3PDObject = \
                      make_SG_D3PDObject ('JetCollection',
                                          MissingETD3PDMakerFlags.METDefaultJetCollectionSGKey(),
                                          'METJetsInfo_',
                                          'JetsInfoMETD3PDObject')
JetsInfoMETD3PDObject.defineBlock (5, 'JetsInfo',
                                   MissingETD3PDMaker.MissingETJetsInfoFillerTool)


EtaRingsMETD3PDObject = \
                      make_SG_D3PDObject ('CaloCellContainer',
                                          MissingETD3PDMakerFlags.AllCaloCellsSGKey(),
                                          'MET_EtaRings_',
                                          'EtaRingsMETD3PDObject')
EtaRingsMETD3PDObject.defineBlock (5, 'EtaRings',
                           MissingETD3PDMaker.MissingETEtaRingsFillerTool,
                           DoNoiseCut = MissingETD3PDMakerFlags.doEtaRingsNoiseCut())
    
##For MissingET type objects
def MissingETCustomizer (sgkey, keyLevel, level=10, prefix = 'MET_', allowMissing = False,
                         include = [], exclude = [], specExclude = []):
    subMET = []
    keyInclude = []
    keyExclude = []
    keyExclude += specExclude
    if sgkey.endswith ('*'):
        allowMissing = True
        sgkey = sgkey[:-1]
    for word in exclude:
        if word == sgkey: return subMET
        if word == sgkey+'_Comps': keyExclude += ['MET']
        if word == sgkey+'_Phi': keyExclude += ['MET_Phi']
        if word == sgkey+'_Et': keyExclude += ['MET_Et']
        if word == sgkey+'_SumEt': keyExclude += ['MET_SumEt']
        if word == sgkey+'_Regions': keyExclude += ['MET_Regions']
        if word == 'MET_Regions': keyExclude += ['MET_Regions']
        if word == 'MET_Phi': keyExclude += ['MET_Phi']
        if word == 'MET_Et': keyExclude += ['MET_Et']
        if word == 'MET_Comps': keyExclude += ['MET']
        if word == 'MET_SumEt': keyExclude += ['MET_SumEt']
    for word in include:
        if word == sgkey: keyInclude += ['MET','MET_Phi','MET_Et','MET_SumEt']
        if word == sgkey+'_Comps': keyInclude += ['MET']
        if word == sgkey+'_Phi': keyInclude += ['MET_Phi']
        if word == sgkey+'_Et': keyInclude += ['MET_Et']
        if word == sgkey+'_SumEt': keyInclude += ['MET_SumEt']        
        if word == sgkey+'_Regions': keyInclude += ['MET_Regions']
    if level >= keyLevel:
        subMET += [RefFinalMETD3PDObject(level=0, sgkey=sgkey, prefix=prefix+sgkey[4:]+'_',
                                         allowMissing=allowMissing, include=keyInclude,
                                         exclude=keyExclude)]
    else:
        subMET += [RefFinalMETD3PDObject(level=-1, sgkey=sgkey, prefix=prefix+sgkey[4:]+'_',
                                         allowMissing=allowMissing, include=keyInclude,
                                         exclude=keyExclude)]

    return subMET
 

##For MissingEtTruth type objects
def TruthMissingETCustomizer (sgkey, keyLevel, level=10, prefix = 'MET_', allowMissing = False,
                              include = [], exclude = []):
    subMET = []
    keyInclude = []
    keyExclude = []
    for word in exclude:
        if word == sgkey: return subMET
        if word == sgkey+'_Int': keyExclude += ['MET_Truth_Int','MET_Truth_Int_Phi',
                                                'MET_Truth_Int_Et','MET_Truth_Int_SumEt']
        if word == sgkey+'_Int': keyExclude += ['MET_Truth_NonInt','MET_Truth_NonInt_Phi',
                                                'MET_Truth_NonInt_Et','MET_Truth_NonInt_SumEt']
        if word == sgkey+'_Int_Comps': keyExclude += ['MET_Truth_Int']
        if word == sgkey+'_Int_Phi': keyExclude += ['MET_Truth_Int_Phi']
        if word == sgkey+'_Int_Et': keyExclude += ['MET_Truth_Int_Et']
        if word == sgkey+'_SumEt': keyExclude += ['MET_Truth_Int_SumEt']
        if word == sgkey+'_NonInt_Comps': keyExclude += ['MET_Truth_NonInt']
        if word == sgkey+'_NonInt_Phi': keyExclude += ['MET_Truth_NonInt_Phi']
        if word == sgkey+'_NonInt_Et': keyExclude += ['MET_Truth_NonInt_Et']
        if word == sgkey+'_NonInt_SumEt': keyExclude += ['MET_Truth_NonInt_SumEt']
    for word in include:
        if word == sgkey: keyInclude += ['MET_Truth_Int','MET_Truth_Int_Phi','MET_Truth_Int_Et',
                                         'MET_Truth_Int_SumEt','MET_Truth_NonInt','MET_Truth_NonInt_Phi',
                                         'MET_Truth_NonInt_Et','MET_Truth_NonInt_SumEt']
        if word == sgkey+'_Int': keyInclude += ['MET_Truth_Int','MET_Truth_Int_Phi',
                                                'MET_Truth_Int_SumEt','MET_Truth_Int_Et']
        if word == sgkey+'_Int': keyInclude += ['MET_Truth_NonInt','MET_Truth_NonInt_Phi',
                                                'MET_Truth_NonInt_SumEt','MET_Truth_NonInt_Et']
        if word == sgkey+'_NonInt_Comps': keyInclude += ['MET_Truth_NonInt']
        if word == sgkey+'_Int_Comps': keyInclude += ['MET_Truth_Int']
        if word == sgkey+'_Int_Phi': keyInclude += ['MET_Truth_Int_Phi']
        if word == sgkey+'_Int_Et': keyInclude += ['MET_Truth_Int_Et']
        if word == sgkey+'_Int_SumEt': keyInclude += ['MET_Truth_Int_SumEt']
        if word == sgkey+'_NonInt_Phi': keyInclude += ['MET_Truth_NonInt_Phi']
        if word == sgkey+'_NonInt_Et': keyInclude += ['MET_Truth_NonInt_Et']
        if word == sgkey+'_NonInt_SumEt': keyInclude += ['MET_Truth_NonInt_SumEt']
    if level >= keyLevel:
        subMET += [MissingETTruthD3PDObject(level=10, sgkey=sgkey, prefix=prefix+sgkey[4:]+'_',
                                            allowMissing=allowMissing, include=keyInclude,
                                            exclude=keyExclude)]
    else:
        subMET += [MissingETTruthD3PDObject(level=0, sgkey=sgkey, prefix=prefix+sgkey[4:]+'_',
                                            allowMissing=allowMissing, include=keyInclude,
                                            exclude=keyExclude)]

    return subMET

##For MissingEtCalo type objects
def CaloMissingETCustomizer (sgkey, keyLevel, level=10, prefix = 'MET_', allowMissing = False,
                             include = [], exclude = []):
    subMET = []
    keyInclude = []
    keyExclude = []
    for word in exclude:
        if word == sgkey: return subMET
        if word == sgkey+'_Comps': keyExclude += ['MET_Calo']
        if word == sgkey+"_EMFracSET": keyExclude += ['MET_Calo_EMFracSET']
        if word == sgkey+"_Samplings": keyExclude += ['MET_Calo_Samplings']
        if word == sgkey+"_Regions": keyExclude += ['MET_Calo_Regions']
        if word == sgkey+"_Et": keyExclude += ['MET_Calo_Et']
        if word == sgkey+"_Phi": keyExclude += ['MET_Calo_Phi']
        if word == sgkey+"_SumEt": keyExclude += ['MET_Calo_SumEt']
    for word in include:
        if word == sgkey: keyInclude = ['MET_Calo','MET_Calo_Et','MET_Calo_Phi','MET_Calo_SumEt']
        if word == sgkey+"_Comps": keyInclude = ['MET_Calo']
        if word == sgkey+"_EMFracSET": keyInclude += ['MET_Calo_EMFracSET']
        if word == sgkey+"_Samplings": keyInclude += ['MET_Calo_Samplings']
        if word == sgkey+"_Regions": keyInclude += ['MET_Calo_Regions']
        if word == sgkey+"_Et": keyInclude += ['MET_Calo_Et']
        if word == sgkey+"_Phi": keyInclude += ['MET_Calo_Phi']
        if word == sgkey+"_SumEt": keyInclude += ['MET_Calo_SumEt']

    if level >= keyLevel:
        subMET += [TopoMETD3PDObject(level=3, sgkey=sgkey, prefix=prefix+sgkey[4:]+'_',
                                     allowMissing=allowMissing, include=keyInclude,
                                     exclude=keyExclude)]
    else:
        subMET += [TopoMETD3PDObject(level=0, sgkey=sgkey, prefix=prefix+sgkey[4:]+'_',
                                     allowMissing=allowMissing, include=keyInclude,
                                     exclude=keyExclude)]

    return subMET
   

def MissingETCaloD3PDObject (level=10, sgkey = None, prefix = 'MET', allowMissing = False,
                             include = [], exclude = []):
    MET = []
    prefix=prefix+'_'
    if sgkey != None:
        MET += [TopoMETD3PDObject(level=level, sgkey=sgkey, prefix=prefix,
                                  allowMissing=allowMissing, include=include, exclude=exclude)]
    if sgkey == None:
        for key in Level3CaloMETList:
            MET += CaloMissingETCustomizer (key, 3, level=level, prefix=prefix,
                                            allowMissing=allowMissing, include=include,
                                            exclude=exclude)
    return MET


###############################################################################


from D3PDMakerCoreComps.D3PDObject          import make_SGDataVector_D3PDObject

def make_METCompositionD3PDObject (type, objectType):
    obj = \
        make_SGDataVector_D3PDObject (type, '', '',
                                      default_object_name = 'MissingETCompositionD3PDObject')
    assoc = MissingETCompAssociation(obj,
                                     'MissingETComposition',
                                     MissingETD3PDMakerFlags.METCompositionSGKey(),
                                     level = 4,
                                     objectType = objectType,
                                     blockname = 'MET_Weights',
                                     allowMissing = True)
    return obj


#
# object type
# SG data type
# SG key (trailing * means set allowMissing to true).
# prefix
# list of (pseudo-) block names, for include/exclude processing.
#
_met_composition_objects = [
    ['egamma',
     'ElectronContainer',
     D3PDMakerFlags.ElectronSGKey(),
     MissingETD3PDMakerFlags.METElectronPrefix(),
     ['MET_Electron_Weights']],

    ['egamma',
     'PhotonContainer',
     D3PDMakerFlags.PhotonSGKey(),
     MissingETD3PDMakerFlags.METPhotonPrefix(),
     ['MET_Photon_Weights']],
    
    ['Muon',
     'Analysis::MuonContainer',
     'StacoMuonCollection*',
     MissingETD3PDMakerFlags.METStacoPrefix(),
     ['MET_Staco_Weights', 'MET_Muon_Weights']],

    ['Muon',
     'Analysis::MuonContainer',
     'MuidMuonCollection*',
     MissingETD3PDMakerFlags.METMuidPrefix(),
     ['MET_Muid_Weights', 'MET_Muon_Weights']],

    ['Muon',
     'Analysis::MuonContainer',
     'Muons*',
     MissingETD3PDMakerFlags.METMuonsPrefix(),
     ['MET_Muons_Weights', 'MET_Muon_Weights']],

    ['TauJet',
     'Analysis::TauJetContainer',
     D3PDMakerFlags.TauSGKey(),
     MissingETD3PDMakerFlags.METTauPrefix(),
     ['MET_Tau_Weights']],

    ['Jet',
     'JetCollection',
     '_jet',
     '_jet',
     ['MET_Jet_Weights']],

    ['CaloCluster',
     'CaloClusterContainer',
     D3PDMakerFlags.ClusterSGKey(),
     MissingETD3PDMakerFlags.METClusterPrefix(),
     ['MET_Cluster_Weights']],
    
    #['TrackParticle',
    # 'Rec::TrackParticleContainer',
    # D3PDMakerFlags.TrackSGKey(),
    # MissingETD3PDMakerFlags.METTrackPrefix(),
    # ['MET_Track_Weights']],

    ['TrackParticle',
     'Rec::TrackParticleContainer',
     '_trk',
     '_trk',
     ['MET_Track_Weights']],
]


def _any_in (a, b):
    return [x for x in a if x in b]


def MissingETCompositionD3PDObject (level=10, sgkey = None, prefix = '', allowMissing = False,
                                    include = [], exclude = [], suffix = None, jetSGKey = None,
                                    jetPrefix=None, trackSGKey = None, trackPrefix=None):
    METComp = []

    if suffix == None: suffix=MissingETD3PDMakerFlags.METCompositionSGKey()[19:]
    if jetSGKey == None: jetSGKey = MissingETD3PDMakerFlags.METDefaultJetCollectionSGKey()+suffix
    if jetPrefix == None: jetPrefix = MissingETD3PDMakerFlags.METDefaultJetPrefix()
    #if trackSGKey == None: trackSGKey = MissingETD3PDMakerFlags.METDefaultTrackCollectionSGKey()+suffix
    if trackSGKey == None: trackSGKey = MissingETD3PDMakerFlags.METDefaultTrackCollectionSGKey()
    if trackPrefix == None: trackPrefix = MissingETD3PDMakerFlags.METDefaultTrackPrefix()

    if sgkey == None: sgkey = MissingETD3PDMakerFlags.METCompositionSGKey()

    for desc in _met_composition_objects:
        bnames = desc[4] + ['MET_Composition']

        if _any_in (bnames, exclude): continue
        if level >= 4 or _any_in (bnames, include):
            obj = make_METCompositionD3PDObject (desc[1], desc[0])
            allow_missing = False
            obj_sgkey = desc[2]
            if obj_sgkey.endswith ('*'):
                allow_missing = True
                obj_sgkey = obj_sgkey[:-1]
            prefix = desc[3]
            if prefix == '_jet':
                obj_sgkey = jetSGKey
                prefix = jetPrefix
            if prefix == '_trk':
                obj_sgkey = trackSGKey
                prefix = trackPrefix
            METComp += [obj (level,
                             prefix = prefix + suffix,
                             MET_Weights_sgkey = sgkey,
                             sgkey = obj_sgkey,
                             allowMissing = allow_missing)]

    return METComp


###############################################################################



def MissingETD3PDObject (level=10, sgkey = None, prefix = 'MET', allowMissing = False,
                         include = [], exclude = []):
    MET = []
    prefix=prefix+'_'
    if sgkey != None:
        MET += [RefFinalMETD3PDObject(level=level, sgkey=sgkey, prefix=prefix,
                                      allowMissing=allowMissing, include=include,
                                      exclude=exclude)] 
    else:
        MET += MissingETCustomizer (MissingETD3PDMakerFlags.METRefFinalSGKey(), 0,
                                    level=level, prefix=prefix, allowMissing=allowMissing,
                                    include=include, exclude=exclude)

        ###Level 1 Objects
        for key in Level1METList:
            MET += MissingETCustomizer (key, 1, level=level, prefix=prefix,
                                        allowMissing=allowMissing, include=include,
                                        exclude=exclude)
        for key in Level1MuonMETList:
            MET += MissingETCustomizer (key, 1, level=level, prefix=prefix,
                                        allowMissing=allowMissing, include=include,
                                        exclude=exclude)
        #for key in Level1STVFMETList:
        #    MET += MissingETCustomizer (key, 1, level=level, prefix=prefix,
        #                                allowMissing=allowMissing, include=include,
        #                                exclude=exclude)
        for key in Level1SpecialMETList:
            MET += MissingETCustomizer (key, 1, level=level, prefix=prefix,
                                        allowMissing=allowMissing, include=include,
                                        exclude=exclude)

        if MissingETD3PDMakerFlags.doCellOutEflow():
            MET += MissingETCustomizer (MissingETD3PDMakerFlags.METCellOutEflowSGKey(), 1,
                                        level=level, prefix=prefix, allowMissing=allowMissing,
                                        include=include, exclude=exclude)
            MET += MissingETCustomizer (MissingETD3PDMakerFlags.METCellOutEflowMuidSGKey(), 1,
                                        level=level, prefix=prefix, allowMissing=allowMissing,
                                        include=include, exclude=exclude)
            MET += MissingETCustomizer (MissingETD3PDMakerFlags.METCellOutEflowMuonsSGKey(), 1,
                                        level=level, prefix=prefix, allowMissing=allowMissing,
                                        include=include, exclude=exclude)

        ##Level 2 Objects
        if MissingETD3PDMakerFlags.doCellOutMiniJet():
            MET += MissingETCustomizer (MissingETD3PDMakerFlags.METCellOutMiniJetSGKey(), 2,
                                        level=level, prefix=prefix, allowMissing=allowMissing,
                                        include=include, exclude=exclude)

        for key in Level2METList:
            MET += MissingETCustomizer (key, 2, level=level, prefix=prefix, allowMissing=allowMissing,
                                        include=include, exclude=exclude)
        for key in Level2MuonMETList:
            MET += MissingETCustomizer (key, 2, level=level, prefix=prefix, allowMissing=allowMissing,
                                        include=include, exclude=exclude)

        ###Level 3, including MissingEtCalo types
        for key in Level3METList:
            MET += MissingETCustomizer (key, 3, level=level, prefix=prefix, allowMissing=allowMissing,
                                        include=include, exclude=exclude)
        for key in Level3CaloMETList:
            MET += CaloMissingETCustomizer (key, 3, level=level, prefix=prefix,
                                            allowMissing=allowMissing, include=include,
                                            exclude=exclude)

        #########Truth objects, levels 1 and 3
        if rec.doTruth():
            MET += [MissingETTruthD3PDObject(level=level, allowMissing=allowMissing,
                                             include=include, exclude=exclude)]
            MET += TruthMissingETCustomizer (MissingETD3PDMakerFlags.METTruthPileUpSGKey(), 3,
                                             level=level, prefix=prefix, allowMissing=allowMissing,
                                             include=include, exclude=exclude)
            for key in Level3TruthMETList:
               MET += MissingETCustomizer (key, 3, level=level, prefix=prefix,
                                           allowMissing=allowMissing, include=include,
                                           exclude=exclude, specExclude = ['MET_Regions'])

        ###Level 4 object - MissingET Composition
        MET += MissingETCompositionD3PDObject (level=level, include=include, exclude=exclude,
                                               allowMissing=allowMissing)

        ###Level 5 objects, jet info and eta rings
        MET += [JetsInfoMETD3PDObject(level=level, allowMissing=allowMissing, include=include,
                                      exclude=exclude)]

        if MissingETD3PDMakerFlags.doCells():
            MET += [EtaRingsMETD3PDObject(level=level, allowMissing=allowMissing,
                                          include=include, exclude=exclude)]

    return MET

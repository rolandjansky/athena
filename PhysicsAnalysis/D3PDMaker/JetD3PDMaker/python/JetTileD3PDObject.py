# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import JetD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject, \
                                          make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.release_version import release_version
from D3PDMakerCoreComps.resolveSGKey import resolveSGKey
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from EventCommonD3PDMaker.DRAssociation import DRAssociation

from JetD3PDMaker import JetD3PDMakerConf
from JetRecTools.JetRecToolsConf import *
from JetMomentTools.JetMomentToolsConf import *
from RecExConfig.RecFlags import rec
from AthenaCommon.AlgSequence import AlgSequence
from JetRec import JetRecConf
from JetRecTools import JetRecToolsConf
from JetTagD3PDMaker.AddBTagD3PDInfo import addBTagInfoToJetObject


jetMinEta=-2.0
jetMaxEta=2.0
jetMinPhi=-4.0
jetMaxPhi=4.0


#
# Useful options:
#  Specify ConstitIndex_Target to fill a list of indices of jet constituents.
#  Target should be target collection for the association.
#


def makeJetTileFilterAlg(inSGkey):
    jetTileFilter=JetD3PDMakerConf.JetTileFilterAlg(JetCollectionSGkey=inSGkey,
    JetCollOutSGkey='JToutkey')#,
#    jetEtaMin=jetMinEta,
#    jetEtaMax=jetMaxEta,
 #   jetPhiMin=jetMinPhi,
 #   jetPhiMax=jetMaxPhi)
    
    return jetTileFilter

# Reconstruction versions prior to 15.6.8 didn't fill in the jet timing/quality.
# Fix up such jets here.
def _fixJetTiming (c, **kw):
    sgkey_raw = c.Getter.SGKey
    jetfilter=makeJetTileFilterAlg(sgkey_raw)
    seq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
    seq+=jetfilter
    sgkey_in=jetfilter.JetCollOutSGkey
    c.Getter.SGKey =sgkey_in
    # Check version number of the input data.
    # If no version is found, assume old.
    rlist = release_version()
    if rlist:
        if rlist[0] >= 16: return
        if rlist[0] == 15 and rlist[1] >= 7: return
        if rlist[0] == 15 and rlist[1] == 6 and rlist[2] >= 8: return

    try:
        sgkey_in = resolveSGKey ('JetCollection', sgkey_in)
    except:
        if not kw.get('allowMissing'): raise
        return
    sgkey_out = sgkey_in + '_timing'

    if not hasattr (seq, sgkey_out):
        tools = []
        caloqual_kw = {'doCellBasedVariables' : True,
                       'doConstituentBasedVariables' : True,
                       'doSamplingBasedVariables' : True,
                       'cutOnTile' : 254,
                       'cutOnLAr' : 4000,
                       'doTileQuality' : True,
                       'doLArQuality' : True}

        if hasattr (JetRecToolsConf, 'JetTimeQualityTool'):
            # rel 15 setup
            tool1 = JetRecToolsConf.JetTimeQualityTool (sgkey_out + 'TimeTool',
                                                        DoTime = False,
                                                        DoQuality = False,
                                                        UseCells = False)
            if rec.readESD() or rec.readRDO():
                tool1.DoTime    = True
                tool1.DoQuality = True
                tool1.UseCells  = True
            if rec.readAOD() and (sgkey_in.find ('H1Topo') >= 0 or
                                  sgkey_in.find ('LCTopo') >= 0):
                tool1.DoTime    = True
            tools.append (tool1)
        else:
            # rel 16
            caloqual_kw['doTiming'] = True

        tool2 = JetCaloQualityTool (sgkey_out + 'QualTool',
                                    **caloqual_kw)
        tools.append (tool2)

        alg = JetD3PDMaker.JetFixupAlg (sgkey_out,
                                        InputKey = sgkey_in,
                                        OutputKey = sgkey_out,
                                        Tools = tools)
        for t in tools:
            alg += t

        seq += alg

    c.Getter.SGKey = sgkey_out
    return

def _getJvfTool():
    from JetMomentTools.SetupJetMomentTools import getJetVertexAssociationTool
    jvatool = getJetVertexAssociationTool('AntiKt', 0.4, 'Topo') # parameters are irrelevant, these will work for any jets. 
    return jvatool

# Don't make this unless target has been specified.
def _constitAssocLevel (reqlev, args):
    if reqlev < 1: return False
    if not args.has_key ('Target'): return False
    return True

def getJetTileD3PDObject(objectname='JetTileD3PDObject', prefix='jet_', btagleveloffset=7):


    object = make_SGDataVector_D3PDObject ('JetCollection',
                                           D3PDMakerFlags.JetSGKey(),
                                           prefix, objectname)

    object.defineHook (_fixJetTiming)

    object.defineBlock(0, 'Kinematics',
                              EventCommonD3PDMaker.FourMomFillerTool,
                              WriteE  = True)

    object.defineBlock(1, 'OriginCorrection',
                              JetD3PDMaker.JetMomentFillerTool,
                              Moments=['EtaOrigin'  , 'PhiOrigin'  ,  'MOrigin'
                                       ])

    object.defineBlock(1, 'DQMoments',
                              JetD3PDMaker.JetMomentFillerTool,
                              Moments=['WIDTH',      'n90',
                                       'Timing',     'LArQuality',
                                       'nTrk',       'sumPtTrk',
                                       'OriginIndex',
                                       'HECQuality', 'NegativeE', 'AverageLArQF',
                                       'BCH_CORR_CELL', 'BCH_CORR_DOTX','BCH_CORR_JET', 'BCH_CORR_JET_FORCELL', 
                                       'ENG_BAD_CELLS', 'N_BAD_CELLS', 'N_BAD_CELLS_CORR','BAD_CELLS_CORR_E',
                                       'NumTowers',
                                       'ootFracCells5', 'ootFracCells10', 'ootFracClusters5', 'ootFracClusters10'
                                       ])
    
    object.defineBlock(1, 'JetSamplingsFrac',
                              JetD3PDMaker.JetSamplingsFracFillerTool)
    object.defineBlock(1, 'JetQual',
                              JetD3PDMaker.JetCaloUtilsFillerTool)
    object.defineBlock(1, 'EMFraction',
                              JetD3PDMaker.JetEMFractionFillerTool)
    
    object.defineBlock(1, 'JES',
                              JetD3PDMaker.JetMomentFillerTool,
                              Moments=[ 'Offset','EMJES','EMJES_EtaCorr','EMJESnooffset' ])
    
    
    object.defineBlock(1, 'EMScale',
                              JetD3PDMaker.JetSignalStateFillerTool,
                              WriteE = True,
                              SignalStateNumber = 0,
                              SignalStatePrefix = 'emscale')
                              
    object.defineBlock(2, 'JetTileFiller',
                              JetD3PDMaker.JetTileCellFillerTool)
                              
#    object.defineBlock(3, 'JVtx',
#                              JetD3PDMaker.JetVtxFillerTool,
#                              JetVertexAssociationTool=_getJvfTool())
    
    
    object.defineBlock(3, 'Layer',
                              JetD3PDMaker.JetMomentFillerTool,
                              Moments=['GSCFactorF','WidthFraction'])
    
    object.defineBlock(4, 'Samplings',
                              JetD3PDMaker.JetSamplingsFillerTool)
    
    object.defineBlock(5, 'JESMoments',
                              JetD3PDMaker.JetMomentFillerTool,
                              Moments=['LCJES'])
#                              Moments=['GCWJES', 'GCWJES_EtaCorr','CB'])

    object.defineBlock(5, 'JetShape',
                              JetD3PDMaker.JetShapeFillerTool)
    
    
    object.defineBlock(5, 'Constituents',
                              JetD3PDMaker.JetConstituentFillerTool,
                              SignalStateNumber = 0,
                              SignalStatePrefix = 'emscale')
    
    object.defineBlock(5, 'ConstituentScale',
                              JetD3PDMaker.JetSignalStateFillerTool,
                              WriteE = True,
                              SignalStateNumber = 2,
                              SignalStatePrefix = 'constscale')
    
    object.defineBlock(6, 'JetLArHVMoment',
                              JetD3PDMaker.JetMomentFillerTool,
                              Moments=['LArBadHVEnergy','LArBadHVRatio'])
    
    addBTagInfoToJetObject(object,btagleveloffset)


    # Track association cannot be done unless there are tracks.
    # As often this is not the case track association will be done only
    # if this is explicitly asked for. by defining it in the include list.
    #
    # You can also enable it by passing an argument to JetD3PDObject
    # like:
    #   AssocTrackCont_target='mytarget_'
    def _jetAssocLevel (reqlev, args):
        return args.has_key ('target') or (reqlev >= 999)
    IndexMultiAssociation (object,
                           JetD3PDMaker.FlavorAssociationTool,
                           'trk',
                           prefix = 'flavor_assoctrk_',
                           level = _jetAssocLevel,
                           blockname = 'AssocTrackCont')


    object.defineBlock(10, 'JetRoIword',
                              JetD3PDMaker.JetROIWordFillerTool
                              )


    JetElAssoc = DRAssociation(object,"ElectronContainer",D3PDMakerFlags.ElectronSGKey(),0.2,prefix='el_',matched="matched",blockname="El02Match")

    JetMuAssoc = DRAssociation(object,"Analysis::MuonContainer",D3PDMakerFlags.MuonSGKey(),0.2,prefix='mu_',matched="matched",blockname="Mu02Match")

#JetTrkAssoc = DRAssociation(object,"Rec::TrackParticleContainer",D3PDMakerFlags.TrackSGKey(),0.4,prefix='trk_',matched="matched",blockname="Trk04Match")


#---------------- Trigger Object ----------------------------

    JetL1Assoc = SimpleAssociation(object,
                                   JetD3PDMaker.JetL1TriggerObjectAssociationTool,
                                   matched = 'matched',
                                   blockname = 'L1Info',
                                   prefix = 'L1_',
                                   MaxDR = 0.6,
                                   ChainPattern = 'L1_J.*')
    
    JetL1Assoc.defineBlock (99, 'L1Kinematics',
                            EventCommonD3PDMaker.FourMomFillerTool,
                            WriteE = True,
                            WriteEt = False,
                            WriteM = False)
    
    
    JetL2Assoc = SimpleAssociation(object,
                                   JetD3PDMaker.JetL2TriggerObjectAssociationTool,
                                   matched = 'matched',
                                   blockname = 'L2Info',
                                   prefix = 'L2_',
                                   MaxDR = 0.25,
                                   ChainPattern = 'L2_j.*')
    
    JetL2Assoc.defineBlock (99, 'L2Kinematics',
                            EventCommonD3PDMaker.FourMomFillerTool,
                            WriteE = True,
                            WriteEt = False,
                            WriteM = False)
    
    JetEFAssoc = SimpleAssociation(object,
                                   JetD3PDMaker.JetEFTriggerObjectAssociationTool,
                                   matched = 'matched',
                                   blockname = 'EFInfo',
                                   prefix = 'EF_',
                                   MaxDR = 0.25,
                                   ChainPattern = 'EF_j.*')
    
    JetEFAssoc.defineBlock (99, 'EFKinematics',
                            EventCommonD3PDMaker.FourMomFillerTool,
                            WriteE = True,
                            WriteEt = False,
                            WriteM = False)
    
    
    ConstitAssoc = ContainedVectorMultiAssociation \
        (object,
         #JetD3PDMaker.JetConstituentAssociationTool,
         EventCommonD3PDMaker.NavigableConstituentAssociationTool,
         'constit_',
         TypeName = 'CaloCluster',
         nrowName = '',
         level = 1)
    

    ConstitAssoc.defineBlock (_constitAssocLevel, 'ConstitIndex',
                              D3PDMakerCoreComps.IndexFillerTool,
                              Target = '')


    return object


JetTileD3PDObject = getJetTileD3PDObject()

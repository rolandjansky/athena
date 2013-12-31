# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import JetD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject, \
                                          make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.release_version import release_version
from D3PDMakerCoreComps.resolveSGKey import resolveSGKey
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from EventCommonD3PDMaker.DRAssociation import DRAssociation
from JetD3PDMaker import JetD3PDMakerConf
from JetRecTools.JetRecToolsConf import *
from JetMomentTools.JetMomentToolsConf import *
#from JetRec.JetMomentGetter import make_JetMomentGetter
from RecExConfig.RecFlags import rec
from AthenaCommon.AlgSequence import AlgSequence
from JetRec import JetRecConf
from JetRecTools import JetRecToolsConf

def makeTrackParticleToJetAssoc (name, prefix, object_name,
                                 assoc = None,
                                 label = "jet_"):

    if not assoc:
        assoc = JetD3PDMaker.FlavorAssociationTool(name + 'Assoc')

    return D3PDMakerCoreComps.IndexMultiAssociationFillerTool (name,
                                                               Prefix = "flavor_assoctrk_",
                                                               Associator = assoc,
                                                               Target = "trk_")

JetD3PDObject = make_SGDataVector_D3PDObject ('JetCollection',
                                              D3PDMakerFlags.JetSGKey(),
                                              'jet_', 'QcdJetD3PDObject')

# Reconstruction versions prior to 15.6.8 didn't fill in the jet timing/quality.
# Fix up such jets here.
def _fixJetTiming (c, **kw):
    # Check version number of the input data.
    # If no version is found, assume old.
    rlist = release_version()
    if rlist:
        if rlist[0] >= 16: return
        if rlist[0] == 15 and rlist[1] >= 7: return
        if rlist[0] == 15 and rlist[1] == 6 and rlist[2] >= 8: return

    sgkey_in = c.Getter.SGKey
    sgkey_in = resolveSGKey ('JetCollection', sgkey_in)
    sgkey_out = sgkey_in + '_timing'

    seq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
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
JetD3PDObject.defineHook (_fixJetTiming)

TrackParticleToJetD3PDAssoc = D3PDObject (makeTrackParticleToJetAssoc, "trk_")


JetD3PDObject.defineBlock(0, 'Kinematics',
                          EventCommonD3PDMaker.FourMomFillerTool,
                          WriteE  = True)

JetD3PDObject.defineBlock(1, 'OriginCorrection',
                          JetD3PDMaker.JetMomentFillerTool,
                          Moments=['MOrigin'  , 'EtaOrigin'  , 'PhiOrigin'  ,  
                                   'MOriginEM', 'EtaOriginEM', 'PhiOriginEM'
                                   ])

JetD3PDObject.defineBlock(1, 'DQMoments',
                          JetD3PDMaker.JetMomentFillerTool,
                          Moments=['WIDTH',      'n90',
                                   'Timing',     'LArQuality',
                                   'nTrk',       'sumPtTrk',
                                   'OriginIndex',
                                   'HECQuality', 'NegativeE',
                                   'BCH_CORR_CELL', 'BCH_CORR_DOTX','BCH_CORR_JET', 'BCH_CORR_JET_FORCELL', 
                                   'ENG_BAD_CELLS', 'N_BAD_CELLS', 'N_BAD_CELLS_CORR','BAD_CELLS_CORR_E'
                                   ])

JetD3PDObject.defineBlock(1, 'JetSamplingsFrac',
                          JetD3PDMaker.JetSamplingsFracFillerTool)
JetD3PDObject.defineBlock(1, 'JetQual',
                          JetD3PDMaker.JetCaloUtilsFillerTool)
JetD3PDObject.defineBlock(1, 'EMFraction',
                          JetD3PDMaker.JetEMFractionFillerTool)

JetD3PDObject.defineBlock(1, 'JES',
                          JetD3PDMaker.JetMomentFillerTool,
                          Moments=[ 'Offset', 'EMJES', 'EMJES_EtaCorr','EMJESnooffset' ])

JetD3PDObject.defineBlock(1, 'JESMoments',
                          JetD3PDMaker.JetMomentFillerTool,
                          Moments=['GCWJES', 'GCWJES_EtaCorr','CB'])
                           
JetD3PDObject.defineBlock(1, 'EMScale',
                          JetD3PDMaker.JetSignalStateFillerTool,
                          WriteE = True,
                          SignalStateNumber = 0,
                          SignalStatePrefix = 'emscale')

JetD3PDObject.defineBlock(3, 'JVtx',
                          JetD3PDMaker.JetVtxFillerTool)


JetD3PDObject.defineBlock(3, 'Layer',
                          JetD3PDMaker.JetMomentFillerTool,
                          Moments=['GSCFactorF','WidthFraction'])

JetD3PDObject.defineBlock(4, 'Samplings',
                          JetD3PDMaker.JetSamplingsFillerTool)

JetD3PDObject.defineBlock(4, 'JetShape',
                          JetD3PDMaker.JetShapeFillerTool)


JetD3PDObject.defineBlock(4, 'Constituents',
                          JetD3PDMaker.JetConstituentFillerTool,
                          SignalStateNumber = 0,
                          SignalStatePrefix = 'emscale')

JetD3PDObject.defineBlock(4, 'ConstituentScale',
                          JetD3PDMaker.JetSignalStateFillerTool,
                          WriteE = True,
                          SignalStateNumber = 2,
                          SignalStatePrefix = 'constscale')

JetD3PDObject.defineBlock(5, 'JetLArHVMoment',
                          JetD3PDMaker.JetMomentFillerTool,
                          Moments=['LArBadHVEnergy','LArBadHVRatio'])

JetD3PDObject.defineBlock(7, 'BTagComponents',
                          JetD3PDMaker.JetFlavorTagComponentsFillerTool,
                          BTagKey = '',
                          DoAllTags = True)

JetD3PDObject.defineBlock(7, 'BTagDefault',
                          JetD3PDMaker.JetBTagWeightFillerTool,
                          BTagKey = '',
                          DoAllTags = False)

JetD3PDObject.defineBlock(7, 'BTag',
                          JetD3PDMaker.JetBTagWeightFillerTool,
                          BTagKey = '',
                          DoAllTags = True)


#Track association cannot be done unless there are tracks.
#As often this is not the case track association will be done only
#if this is explicitly asked for. by defining it in the include list. 
JetD3PDObject.defineBlock(999, 'AssocTrackCont', TrackParticleToJetD3PDAssoc)


JetD3PDObject.defineBlock(10, 'JetRoIword',
                          JetD3PDMaker.JetROIWordFillerTool
                          )


if rec.doTruth:
    JetD3PDObject.defineBlock(6, 'TrueFlavorComponents',
                              JetD3PDMaker.JetTrueTagFillerTool)
    



JetElAssoc = DRAssociation(JetD3PDObject,"ElectronContainer",D3PDMakerFlags.ElectronSGKey(),0.2,prefix='el_',matched="matched",blockname="El02Match")

JetMuAssoc = DRAssociation(JetD3PDObject,"Analysis::MuonContainer",D3PDMakerFlags.MuonSGKey(),0.2,prefix='mu_',matched="matched",blockname="Mu02Match")

#JetTrkAssoc = DRAssociation(JetD3PDObject,"Rec::TrackParticleContainer",D3PDMakerFlags.TrackSGKey(),0.4,prefix='trk_',matched="matched",blockname="Trk04Match")



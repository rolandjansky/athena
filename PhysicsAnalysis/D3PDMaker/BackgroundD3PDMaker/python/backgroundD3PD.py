# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: backgroundD3PD.py 535243 2013-01-29 13:14:01Z salekd $
#
# @file BackgroundD3PDMaker/python/backgroundD3PD.py
# @author Mark Tibbetts <Mark.James.Tibbetts@cern.ch>
# @date Mar, 2011
# @brief Construct a Background D3PD.
#

import D3PDMakerCoreComps
from D3PDMakerConfig.D3PDMakerFlags                     import D3PDMakerFlags
from RecExConfig.RecFlags                               import rec
from RecExConfig.ObjKeyStore                            import cfgKeyStore

from EventCommonD3PDMaker.EventInfoD3PDObject           import EventInfoD3PDObject
from EventCommonD3PDMaker.LBLengthD3PDObject            import LBLengthD3PDObject

from TriggerD3PDMaker.TrigDecisionD3PDObject            import TrigDecisionD3PDObject
from TriggerD3PDMaker.TrigConfMetadata                  import addTrigConfMetadata
from TriggerD3PDMaker.BunchStructureMetadata            import addBunchStructureMetadata
from TriggerD3PDMaker.BGCodeD3PDObject                  import BGCodeD3PDObject

from BackgroundD3PDMaker.BackgroundWordD3PDObject       import BackgroundWordD3PDObject
from BackgroundD3PDMaker.Lucid_RawDataD3PDObject        import Lucid_RawDataD3PDObject
from BackgroundD3PDMaker.RawInfoSummaryForTagD3PDObject import RawInfoSummaryForTagD3PDObject
from BackgroundD3PDMaker.BkgdTriggerBitsD3PDObject      import getBkgdTriggerBitsD3PDObject

from CaloD3PDMaker.MBTSD3PDObject                       import MBTSD3PDObject
from CaloD3PDMaker.LArCollisionTimeD3PDObject           import LArCollisionTimeD3PDObject

from MissingETD3PDMaker.MissingETD3PDObject             import MissingETD3PDObject 

from MuonD3PDMaker.MuonD3PDObject                       import MuonD3PDObject
from MuonD3PDMaker.MuonSegmentD3PDObject                import MuonSegmentD3PDObject
from MuonD3PDMaker.MdtPrepDataD3PDObject                import MdtPrepDataD3PDObject
from MuonD3PDMaker.RpcPrepDataD3PDObject                import RpcPrepDataD3PDObject
from MuonD3PDMaker.TgcPrepDataD3PDObject                import TgcPrepDataD3PDObject
from MuonD3PDMaker.CscPrepDataD3PDObject                import CscPrepDataD3PDObject

from JetD3PDMaker.JetD3PDObject                         import JetD3PDObject

from TrackD3PDMaker.TrackD3PDObject                     import TrackParticleD3PDObject
from TrackD3PDMaker.TrackD3PDObject                     import TrackD3PDObject
from TrackD3PDMaker.TrackD3PDObject                     import TrackD3PDFlags
from TrackD3PDMaker.VertexD3PDObject                    import PrimaryVertexD3PDObject

from HeavyIonD3PDMaker.ZdcD3PDObject                    import ZdcD3PDObject

from TrigMbD3PDMaker.BcmRdoFlatD3PDObject               import BcmRdoFlatD3PDObject

from BackgroundD3PDMaker.BackgroundD3PDMakerFlags       import BackgroundD3PDMakerFlags

from AthenaCommon.GlobalFlags                           import globalflags
from AthenaCommon.AlgSequence                           import AlgSequence
topSequence = AlgSequence()

from BackgroundD3PDMaker.BeamBackgroundD3PDObject import BeamBackgroundD3PDObject

def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw

def backgroundD3PD (alg=None,
                    level = 10,
                    file = 'background.root',
                    tuplename = 'background',
                    flags=BackgroundD3PDMakerFlags,
                    **kw):

    preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
    if (not hasattr (topSequence, 'BeamBackgroundFiller') and
        not hasattr (preseq, 'BeamBackgroundFiller')):
      if cfgKeyStore.isInInput( "Trk::SegmentCollection", "ConvertedMBoySegments" ):
        from RecBackgroundAlgs import RecBackgroundAlgsConf
        preseq += RecBackgroundAlgsConf.BeamBackgroundFiller()

    if (not hasattr (topSequence, 'BeamBackgroundFillerMoore') and
        not hasattr (preseq, 'BeamBackgroundFillerMoore')):
      if cfgKeyStore.isInInput( "Trk::SegmentCollection", "MooreSegments" ):
        from RecBackgroundAlgs.RecBackgroundAlgsConf import BeamBackgroundFiller
        BeamBackgroundFillerMoore = BeamBackgroundFiller('BeamBackgroundFillerMoore',
                                                        muonSegmentContainerKey='MooreSegments',
                                                        BeamBackgroundKey='BeamBackgroundDataMoore')
        preseq += BeamBackgroundFillerMoore


    # The core algorithm
    if not alg:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        alg = MSMgr.NewRootStream(tuplename, file)

    #Event level info
    alg += EventInfoD3PDObject (**_args(1, 'EventInfo', kw))
    if globalflags.DataSource == "data" :
      alg += LBLengthD3PDObject (**_args(0, 'LBLength', kw))

    if rec.doTruth():
      from TruthD3PDMaker.GenEventD3PDObject import GenEventD3PDObject
      alg += GenEventD3PDObject (**_args (0, 'GenEvent', kw))


    if rec.doTrigger():
        alg += TrigDecisionD3PDObject  (**_args(2, 'TrigDecision', kw))
        addTrigConfMetadata( alg )

        bcToolType = ""
        if globalflags.DataSource == "data" :
          bcToolType = "LHC"
        addBunchStructureMetadata( alg, bcToolType )

        alg += BGCodeD3PDObject (**_args (2, 'BGCode', kw))

        if flags.AddTriggerBits():
            alg += getBkgdTriggerBitsD3PDObject([])


    alg += BackgroundWordD3PDObject (**_args(0, 'BkgWord', kw))
    if flags.doLUCID():
        alg += Lucid_RawDataD3PDObject (**_args(0, 'Lucid', kw))
    alg += RawInfoSummaryForTagD3PDObject(**_args(0, 'SumForTag', kw))

    alg += MBTSD3PDObject (**_args(0, 'MBTS', kw))
    alg += LArCollisionTimeD3PDObject (**_args(0, 'LArTimeDiff', kw))
    
    if flags.doMet():
        alg += MissingETD3PDObject    (**_args (1, 'MET_RefFinal', kw,
                                                sgkey  = 'MET_RefFinal',
                                                prefix = 'MET_RefFinal',
                                                allowMissing = True))
 

    alg += MuonD3PDObject (**_args(0, 'Muon', kw,
                                   sgkey='MuonCollection',
                                   prefix='mu_',
                                   include=["Isolation","Quality",
                                            "MuonTimingFillerTool","MuonTileTimeTool",
                                            "ChamberT0"],
                                   allowMissing = True))
    alg += MuonD3PDObject (**_args(0, 'MuidMuon', kw,
                                   sgkey='MuidMuonCollection',
                                   prefix='mu_muid_',
                                   include=["Isolation","Quality",
                                            "MuonTimingFillerTool","MuonTileTimeTool",
                                            "ChamberT0"],
                                   allowMissing = True))
    alg += MuonD3PDObject (**_args(0, 'StacoMuon', kw,
                                   sgkey='StacoMuonCollection',
                                   prefix='mu_staco_',
                                   include=["Isolation","Quality",
                                            "MuonTimingFillerTool","MuonTileTimeTool",
                                            "ChamberT0"],
                                   allowMissing = True))
    if flags.doCaloMuon():
        alg += MuonD3PDObject (**_args(0, 'CaloMuon', kw,
                                       sgkey='CaloMuonCollection',
                                       prefix='mu_calo_',
                                       include=["Isolation","Quality",
                                                "MuonTimingFillerTool","MuonTileTimeTool",
                                                "ChamberT0"],
                                       allowMissing = True))
       

    alg += MuonSegmentD3PDObject ( **_args(1, 'MuonSeg', kw,
                                           sgkey='MuonSegments',
                                           prefix='museg_',
                                           # MooreSegmentT0 takes the default time
                                           # MuonboySegmentT0 recalculates time (see MuonSegmentD3PDObject.py and MuonSegmentT0FillerTool.cxx)
                                           include = ['MooreSegmentT0'],
                                           exclude = ['MuonboySegmentT0'],
                                           allowMissing = True))
    alg += MuonSegmentD3PDObject ( **_args(1, 'MooreSeg', kw,
                                           sgkey='MooreSegments',
                                           prefix='mooreseg_',
                                           include = ['MooreSegmentT0'],
                                           exclude = ['MuonboySegmentT0'],
                                           allowMissing = True))
    alg += MuonSegmentD3PDObject (**_args (1, 'MboySeg', kw,
                                           sgkey='ConvertedMBoySegments',
                                           prefix='mboyseg_',
                                           include = ['MuonboySegmentT0'],
                                           exclude = ['MooreSegmentT0'],
                                           allowMissing = True))
    if flags.doMuGirlSeg():
        alg += MuonSegmentD3PDObject (**_args (1, 'MuGirlSeg', kw,
                                               sgkey='MuGirlSegments',
                                               prefix='mgseg_',
                                               include = ['MooreSegmentT0'],
                                               exclude = ['MuonboySegmentT0'],
                                               allowMissing = True))

    if flags.doTrk():    
        alg += TrackParticleD3PDObject    (**_args (0,  'TrackParticle', kw,
                                                    prefix='trk_'))

    if flags.doMuonTrk():
        MooreTrackPartD3PDObject =   TrackD3PDObject(_label='mooreTP',
                                          _prefix='mooreTP_',
                                          _sgkey='MooreTrackParticles',
                                          typeName='Rec::TrackParticleContainer',
                                          truthTarget='mc',
                                          truthPrefix='mc_',
                                          detailedTruthPrefix='detailed_mc_',
                                          truthMapKey='MooreTrackParticlesTruth',
                                          detailedTruthMapKey='MooreTracksTruth',
                                          flags=TrackD3PDFlags)

        MooreTrackD3PDObject =   TrackD3PDObject(_label='moore',
                                          _prefix='mooreTrk_',
                                          _sgkey='MooreTracks',
                                          typeName='TrackCollection',
                                          truthTarget='mc',
                                          truthPrefix='mc_',
                                          detailedTruthPrefix='detailed_mc_',
                                          truthMapKey='MooreTrackTruthCollection',
                                          detailedTruthMapKey='DetailedTrackTruth',
                                          flags=TrackD3PDFlags)

        MboyTrackPartD3PDObject =   TrackD3PDObject(_label='mboyTP',
                                          _prefix='mboyTP_',
                                          _sgkey='MuonboyTrackParticles',
                                          typeName='Rec::TrackParticleContainer',
                                          truthTarget='mc',
                                          truthPrefix='mc_',
                                          detailedTruthPrefix='detailed_mc_',
                                          truthMapKey='MuonboyTrackTruthCollection',
                                          detailedTruthMapKey='DetailedTrackTruth',
                                          flags=TrackD3PDFlags)

        alg += MooreTrackPartD3PDObject(0)
        alg += MooreTrackD3PDObject(0)
        alg += MboyTrackPartD3PDObject(0)


    if flags.doMuonHits():
        alg += MdtPrepDataD3PDObject (0)
        alg += RpcPrepDataD3PDObject (0)
        alg += CscPrepDataD3PDObject (0)
        alg += TgcPrepDataD3PDObject (0)

        alg += TgcPrepDataD3PDObject (**_args(0, 'TGC_MeasurementsPriorBC', kw,
                                              sgkey='TGC_MeasurementsPriorBC',
                                              prefix='tgcPriorBC_',
                                              label='TgcPrepDataPriorBC'))
        alg += TgcPrepDataD3PDObject (**_args(0, 'TGC_MeasurementsNextBC', kw,
                                              sgkey='TGC_MeasurementsNextBC',
                                              prefix='tgcNextBC_',
                                              label='TgcPrepDataNextBC'))


    if flags.doCaloJet():
      #alg += JetD3PDObject (**_args(4,'AntiKt4TopoEMJets', kw,  sgkey='AntiKt4TopoEMJets',   prefix='AntiKt4TopoEM_', include='BeamBackground' ))
      if cfgKeyStore.isInInput( "Trk::SegmentCollection", "ConvertedMBoySegments" ):
        alg += JetD3PDObject (**_args(4, 'AntiKt4TopoEMJets', kw,
                                      sgkey='AntiKt4TopoEMJets',
                                      prefix='AntiKt4TopoEM_',
                                      include = ['BeamBackground'],
                                      exclude = ['JetVertexFraction','JVtx','ActiveArea','TruthMF','TracksMoments'],
                                      BeamBackground_BeamBackgroundKey='BeamBackgroundData'))
      else :
        alg += JetD3PDObject (**_args(4, 'AntiKt4TopoEMJets', kw,
                                      sgkey='AntiKt4TopoEMJets',
                                      prefix='AntiKt4TopoEM_',
                                      exclude = ['JetVertexFraction','JVtx','ActiveArea','TruthMF','TracksMoments']))
    else:
        alg += JetD3PDObject (**_args(4,'AntiKt4TrackJets', kw,  sgkey='AntiKt4TrackJets',   prefix='AntiKt4Track_',
                                      exclude = ['JetVertexFraction','JVtx','ActiveArea','TruthMF','TracksMoments']))


    alg += PrimaryVertexD3PDObject (0)


    if flags.doBeamBkgd():
      #alg += BeamBackgroundD3PDObject(1)
      if cfgKeyStore.isInInput( "Trk::SegmentCollection", "ConvertedMBoySegments" ):
        alg += BeamBackgroundD3PDObject (**_args(1, 'BeamBackgroundData', kw,
                                                 sgkey='BeamBackgroundData',
                                                 prefix='bb_',
                                                 bbJetIndex_Target='AntiKt4TopoEM_',
                                                 bbClusterIndex_Target='cl_'))
      if cfgKeyStore.isInInput( "Trk::SegmentCollection", "MooreSegments" ):
        alg += BeamBackgroundD3PDObject (**_args(1, 'BeamBackgroundData', kw,
                                                 sgkey='BeamBackgroundDataMoore',
                                                 prefix='bbMoore_',
                                                 bbJetIndex_Target='AntiKt4TopoEM_',
                                                 bbClusterIndex_Target='cl_'))
  

    if flags.doBCM():
        alg += BcmRdoFlatD3PDObject (**_args(0, 'BCMRdo', kw))

    if flags.doZDC():    
        if globalflags.DataSource == "data": alg += ZdcD3PDObject(10)

    if flags.doPixelClus():
      from TrackD3PDMaker.PixelClusterD3PDObject import PixelClusterD3PDObject
      alg += PixelClusterD3PDObject( 0 )

    if flags.doCaloClus():
      from CaloD3PDMaker.ClusterD3PDObject import ClusterD3PDObject
      alg += ClusterD3PDObject( 1 )

    return alg

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file PhysicsAnalysis/D3PDMaker/TrigTauD3PDMaker/python/TrigTauD3PD.py
# @author E. Ptacek <eptacek@uoregon.edu>
# @date December, 2009
# @brief Construct a tau trigger D3PD.
#

from TrigTauD3PDMaker.TrigEFTauD3PDObject     import TrigEFTauD3PDObject
from TrigTauD3PDMaker.TrigL2TauD3PDObject     import TrigL2TauD3PDObject
from TrigInDetD3PDMaker.TrigInDetTrackD3PDObject import TrigInDetTrackD3PDObject

from AthenaCommon.AlgSequence           import AlgSequence
from D3PDMakerConfig.D3PDMakerFlags     import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject      import make_Void_D3PDObject
InitTrigD3PDObject = make_Void_D3PDObject (default_name = 'InitTrigTau')

def _initTrigHook(c, **kw):
    #check egamma flags still filled per trigNav_roi
    #import TrigEgammaD3PDMaker.Setup_RoILinksCnvTool_IBv2
    import TrigTauD3PDMaker.Setup_RoILinksCnvTool_tau
    preseq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName())
    if not hasattr(preseq, 'FlatHltNaviMaker'):
        from TriggerMenuAnalysis.TriggerMenuAnalysisConf import FlatHltNaviMaker
        preseq += FlatHltNaviMaker ('FlatHltNaviMaker')
    return
InitTrigD3PDObject.defineHook(_initTrigHook)

def TrigTauD3PDObjects(d3pdalg = None, level=10, addNavInfo = True, addConfigInfo = True ):

    if not hasattr(d3pdalg, 'trig_L1_emtau_Filler'):
        from TriggerD3PDMaker.EmTauROID3PDObject import EmTauROID3PDObject
        emlev=level+1
        d3pdalg += EmTauROID3PDObject    (emlev, prefix = "trig_L1_emtau_" )

    if addNavInfo:
        d3pdalg += InitTrigD3PDObject( level )
        
    d3pdalg += TrigEFTauD3PDObject   (level)
    d3pdalg += TrigInDetTrackD3PDObject(level, name='trig_L2_trk_idscan_tau_',
                                        sgkey='HLT_TrigIDSCAN_Tau',
                                        prefix='trig_L2_trk_idscan_tau_',
                                        allowMissing=True)
    d3pdalg += TrigInDetTrackD3PDObject(level, name='trig_L2_trk_sitrack_tau_',
                                        sgkey='HLT_TrigSiTrack_Tau_robust',
                                        prefix='trig_L2_trk_sitrack_tau_',
                                        allowMissing=True)

    d3pdalg += TrigInDetTrackD3PDObject(level, name='trig_L2_trk_TauBcore_tau_',
                                        sgkey='HLT_TrigL2SiTrackFinder_TauCore',
                                        prefix='trig_L2_trk_TauBcore_tau_',
                                        allowMissing=True)
    
    d3pdalg += TrigInDetTrackD3PDObject(level, name='trig_L2_trk_TauBiso_tau_',
                                        sgkey='HLT_TrigL2SiTrackFinder_TauIso',
                                        prefix='trig_L2_trk_TauBiso_tau_',
                                        allowMissing=True)
    d3pdalg += TrigInDetTrackD3PDObject(level, name='trig_L2_trk_TauB_tau_',
                                        sgkey='HLT_TrigL2SiTrackFinder_Tau',
                                        prefix='trig_L2_trk_TauB_tau_',
                                        allowMissing=True)
    d3pdalg += TrigL2TauD3PDObject   (level)

    if addNavInfo:
        from TriggerD3PDMaker.TrigNaviD3PDObject import \
             ChainEntryD3PDObject, CombLinksD3PDObjects
        if not hasattr(d3pdalg, 'trig_Nav_Filler'):
            d3pdalg += ChainEntryD3PDObject( 1 )
        d3pdalg += CombLinksD3PDObjects['L2_tau']( 1 )
        d3pdalg += CombLinksD3PDObjects['EF_tau']( 1 )

    if addConfigInfo:
        from TriggerD3PDMaker.TrigConfMetadata import addTrigConfMetadata
        addTrigConfMetadata(d3pdalg)
        from TriggerD3PDMaker.TrigDecisionD3PDObject import \
             TrigDecisionD3PDObject
        if not hasattr(d3pdalg, 'TrigDecisionFiller'):
            d3pdalg += TrigDecisionD3PDObject( 1 )
                    
    return d3pdalg


def TrigTauD3PD (file,
                 level = 10,
                 tuplename = 'trigtau',
                 seq = AlgSequence() ):
    import D3PDMakerCoreComps
    alg = D3PDMakerCoreComps.MakerAlg(tuplename, seq,
                                      file = file)
    TrigTauD3PDObjects (alg, level)
    return alg

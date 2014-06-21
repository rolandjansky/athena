# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AlgSequence
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject

InitTrigD3PDObject = make_Void_D3PDObject( default_name = 'InitTauPerfTrigger' )

def _initTrigHook( c, **kw ):
    import TrigEgammaD3PDMaker.Setup_RoILinksCnvTool_Pppv1
    preseq = AlgSequence( D3PDMakerFlags.PreD3PDAlgSeqName() )
    if not hasattr( preseq, 'FlatHltNaviMaker' ):
        from TriggerMenuAnalysis.TriggerMenuAnalysisConf import FlatHltNaviMaker
        preseq += FlatHltNaviMaker( 'FlatHltNaviMaker' )

    return

InitTrigD3PDObject.defineHook( _initTrigHook )

def TauPerfTriggerD3PDObjects( d3pdalg = None, level = 0, addNaviInfo=True):
    from TrigEgammaD3PDMaker.TrigElectronD3PDObject import TrigElectronD3PDObject
    from TrigEgammaD3PDMaker.EFElectronD3PDObject   import EFElectronD3PDObject
    d3pdalg += TrigElectronD3PDObject (level)
    d3pdalg += EFElectronD3PDObject(level, include=['Triggers'],allowMissing = True) 

    if addNaviInfo:
        d3pdalg+=InitTrigD3PDObject(level+1)
        from TriggerD3PDMaker.TrigNaviD3PDObject import CombLinksD3PDObjects, ChainEntryD3PDObject
        if not hasattr( d3pdalg, 'trig_Nav_Filler' ):
            d3pdalg += ChainEntryD3PDObject(level+1)
            #d3pdalg += CombLinksD3PDObjects[ 'EF_mu' ](level+1)
        d3pdalg += CombLinksD3PDObjects['EF_e'](level+1)

    from TriggerD3PDMaker.TrigConfMetadata import addTrigConfMetadata
    addTrigConfMetadata( d3pdalg )

    return d3pdalg

def TauPerfTriggerD3PD( file,
                        level = 0,
                        tuplename = 'trigger',
                        seq = AlgSequence() ):
    import D3PDMakerCoreComps
    alg = D3PDMakerCoreComps.MakerAlg( tuplename, seq,
                                       file = file )
    TauPerfTriggerD3PDObjects( alg, level )
    return alg

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from PhotonD3PDMaker.PhotonD3PDMakerFlags import PhotonD3PDMakerFlags
from PhotonD3PDMaker.PhotonD3PD import _args
from RecExConfig.RecFlags import rec

def AddMETToD3PD(alg, CustomMETCollections=[], **kw):
    from MissingETD3PDMaker.MissingETD3PDMakerFlags import MissingETD3PDMakerFlags
    MissingETD3PDMakerFlags.METDefaultJetPrefix.set_Value_and_Lock('jet_AntiKt4LCTopo_MET_')

    # does this really need to be 10?
    from D3PDMakerConfig.METD3PDObject                   import METD3PDObject
    alg += METD3PDObject              (**_args (10, 'MET'              , kw))

    if PhotonD3PDMakerFlags.FillFullMET():
        from MissingETD3PDMaker.MissingETD3PDObject import MissingETD3PDObject
        alg += MissingETD3PDObject        (**_args (   10, 'MissingET'        , kw,
                                                       allowMissing = True))
    else:
        # But we want to customize the MET information a bit....
        MET_ALGS=[]
        Level1METList = [MissingETD3PDMakerFlags.METCryoSGKey(),
                         MissingETD3PDMakerFlags.METRefFinalSGKey(),
                         MissingETD3PDMakerFlags.METRefEleSGKey(),
                         MissingETD3PDMakerFlags.METRefJetSGKey(),
                         MissingETD3PDMakerFlags.METRefMuonSGKey(),
                         MissingETD3PDMakerFlags.METRefMuonStacoSGKey(),
                         MissingETD3PDMakerFlags.METRefMuonMuidSGKey(),
                         MissingETD3PDMakerFlags.METRefGammaSGKey(),
                         MissingETD3PDMakerFlags.METRefTauSGKey(),
                         MissingETD3PDMakerFlags.METCellOutSGKey(),
                         MissingETD3PDMakerFlags.METMuonBoySGKey(),
                         MissingETD3PDMakerFlags.METMuonBoyTrackSGKey()]
        
        Level2METList = [MissingETD3PDMakerFlags.METFinalSGKey()]
        
        Level3CaloMETList = [MissingETD3PDMakerFlags.METTopoSGKey(),
                             MissingETD3PDMakerFlags.METLocHadTopoSGKey()]
    
        from MissingETD3PDMaker.MissingETD3PDObject import MissingETCustomizer
        from MissingETD3PDMaker.MissingETD3PDObject import CaloMissingETCustomizer
        for key in Level1METList:
            MET_ALGS += MissingETCustomizer (key, 1, prefix='MET_', allowMissing=True)
        for key in Level2METList:
            MET_ALGS += MissingETCustomizer (key, 2, prefix='MET_', allowMissing=True)
        for key in Level3CaloMETList:
            MET_ALGS += CaloMissingETCustomizer (key, 3, prefix='MET_', allowMissing=True)
        
        alg += MET_ALGS
    
        if rec.doTruth():
            from MissingETD3PDMaker.MissingETD3PDObject          import MissingETTruthD3PDObject
            alg += MissingETTruthD3PDObject   (**_args (    1, 'METTruth'         , kw))

        # FIXME
        # This really doesn't seem to add anything, except warnings at run time
        # We should re-evaluate if this is at all useful to us, and if it is, how to configure it.
        # 
        # from MissingETD3PDMaker.MissingETGoodnessD3PDObject  import MissingETGoodnessD3PDObject
        # alg += MissingETGoodnessD3PDObject(**_args (    1, 'METGoodness'      , kw))

    # Now, fill in some custom MET collections
    METFlavors=['MET_RefGamma',
                'MET_RefFinal',
                'MET_RefFinal_STVF',
                'MET_RefJet',
                'MET_RefJet_JVF',
                'MET_RefJet_JVFCut',
                'MET_RefEle', 
                'MET_Muon_Total_Staco',
                'MET_Muon_Isol_Staco',
                'MET_Muon_NonIsol_Staco', 
                'MET_Muon_Isol_Muid',
                'MET_Muon_NonIsol_Muid',
                'MET_Muon_Total_Muid',
                'MET_Track',
                'MET_CellOut',
                'MET_CellOut_Eflow',
                'MET_CellOut_Eflow_STVF',
                'MET_CellOut_Eflow_JetArea',
                'MET_CellOutCorr',
                'MET_CellOut_Track',
                'MET_CellOut_TrackPV',
                'MET_RefTau',
                'MET_RefMuon']

    from MissingETD3PDMaker.MissingETD3PDObject import MissingETCompositionD3PDObject 
    for coll in CustomMETCollections:
        for flavor in METFlavors:
            alg += MissingETD3PDObject (level=0,
                                        sgkey = flavor+coll,
                                        prefix='MET'+coll+flavor[3:],
                                        allowMissing=True)
            pass
        alg += MissingETCompositionD3PDObject (level=4,
                                               sgkey = 'MET_RefComposition'+coll,
                                               suffix=coll[1:]+"_",
                                               allowMissing = True,
                                               exclude = [])


    

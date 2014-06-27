# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import PhotonD3PDMaker
import EventCommonD3PDMaker
import TrackD3PDMaker
import CaloD3PDMaker
from PhotonD3PDMaker.PhotonD3PDMakerFlags            import PhotonD3PDMakerFlags
from D3PDMakerConfig.D3PDMakerFlags                  import D3PDMakerFlags
from RecExConfig.RecFlags                            import rec

def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw


def PhotonD3PD (alg = None,
                level = 10,
                tuplename = 'photon',
                file = 'photon.root',
                stacoMuonContainer   = 'StacoMuonCollection', 
                caloMuonContainer    = 'CaloMuonCollection',
                CustomMETCollections = [],
                **kw):

    from AthenaCommon.AlgSequence import AlgSequence
    preseq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName())

    
    #-------------------------------------------------------------------------------------
    # Core Algorithm
    #
    # By default the user of this function should have created an algorithm already.
    # But for backwards compatibility let the function create its own algorithm if
    # needed...
    if not alg:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        alg = MSMgr.NewRootStream( tuplename, file )
    #-------------------------------------------------------------------------------------


    #-------------------------------------------------------------------------------------
    # Some standard stuff.
    #
    from EventCommonD3PDMaker.EventInfoD3PDObject        import EventInfoD3PDObject
    alg += EventInfoD3PDObject        (**_args (level, 'EventInfo'        , kw))

    from CaloD3PDMaker.LArCollisionTimeD3PDObject        import LArCollisionTimeD3PDObject
    alg += LArCollisionTimeD3PDObject (**_args (level, 'LArCollisionTime' , kw))

    from CaloD3PDMaker.MBTSD3PDObject                    import MBTSD3PDObject
    alg += MBTSD3PDObject             (**_args (level, 'MBTS'             , kw))

    from CaloD3PDMaker.MBTSTimeD3PDObject                import MBTSTimeD3PDObject
    alg += MBTSTimeD3PDObject         (**_args (level, 'MBTSTime'         , kw))

    from CaloD3PDMaker.MBTSTriggerBitsD3PDObject         import MBTSTriggerBitsD3PDObject
    alg += MBTSTriggerBitsD3PDObject  (**_args (    1, 'MBTSTriggerBits'  , kw))

    from CaloD3PDMaker.CollisionDecisionD3PDObject       import CollisionDecisionD3PDObject
    alg += CollisionDecisionD3PDObject(**_args (level, 'CollisionDecision', kw))

    from TrackD3PDMaker.BeamSpotD3PDObject               import BeamSpotD3PDObject
    alg += BeamSpotD3PDObject(10)

    from TriggerD3PDMaker.BunchStructureMetadata         import addBunchStructureMetadata
    addBunchStructureMetadata( alg )

    from QcdD3PDMaker.QcdEventShapeD3PDObject            import EventShapeD3PDObject
    alg += EventShapeD3PDObject(**_args(0,'rho',kw))

    from TopInputsD3PDMaker.HforD3PDObject               import HforD3PDObject
    alg += HforD3PDObject (**_args (0, 'HforInfo', kw))
    #-------------------------------------------------------------------------------------


    #-------------------------------------------------------------------------------------
    # Diphoton information...  in order to run this, need to have setup the diphoton
    # analysis algorithm.  See PhotonD3PD_prodJobOFragment.py.
    #
    from PhotonD3PDMaker.DiPhotonD3PDObject              import DiPhotonD3PDObject
    alg += DiPhotonD3PDObject         (**_args (level, 'DiPhoton'         , kw))
    #-------------------------------------------------------------------------------------


    #-------------------------------------------------------------------------------------
    # Primary vertex info.  May be missing in single-beam data.
    #
    from TrackD3PDMaker.VertexD3PDObject    import PrimaryVertexD3PDObject
    alg += PrimaryVertexD3PDObject (**_args (0, 'PrimaryVertex', kw,
                                             allowMissing = True,
                                             sgkey = D3PDMakerFlags.VertexSGKey(),
                                             prefix = 'PV_'))
    from PhotonD3PDMaker.PhotonPVD3PDObject import PVD3PDObject
    alg += PVD3PDObject (**_args (0, 'PrimaryVertex', kw,
                                  allowMissing = True,
                                  sgkey = D3PDMakerFlags.VertexSGKey(),
                                  prefix = 'PhotonPV_'))
    #-------------------------------------------------------------------------------------


    #-------------------------------------------------------------------------------------
    # For dumping tracks
    #
    from PhotonD3PDMaker.AddTracksToD3PD import AddTracksToD3PD
    AddTracksToD3PD(alg)
    #-------------------------------------------------------------------------------------


    #-------------------------------------------------------------------------------------
    # I'm not even sure what this stuff is....
    #
    from EventCommonD3PDMaker.LBMetadataConfig           import LBMetadataConfig
    alg.MetadataTools += [LBMetadataConfig()]

    # I think this is usually false, so leave it in for now
    if D3PDMakerFlags.FilterCollCand():
        from CaloD3PDMaker.CollisionFilterAlg import CollisionFilterAlg
        alg.filterSeq += CollisionFilterAlg (tuplename + '_CollCandFilter')
    #-------------------------------------------------------------------------------------


    #-------------------------------------------------------------------------------------
    # Trigger output.
    #
    if D3PDMakerFlags.DoTrigger():
        from PhotonD3PDMaker.AddTriggerToD3PD import AddTriggerToD3PD
        AddTriggerToD3PD(alg, **kw)
    #-------------------------------------------------------------------------------------


    #-------------------------------------------------------------------------------------
    # Missing ET information
    #
    from PhotonD3PDMaker.AddMETToD3PD import AddMETToD3PD
    AddMETToD3PD(alg,CustomMETCollections, **kw)
    #-------------------------------------------------------------------------------------

    
    #-------------------------------------------------------------------------------------
    # For dumping cells.  Note that cell filter is configured in prodJO
    #
    from CaloD3PDMaker.CaloCellD3PDObject import AllCaloCellD3PDObject
    alg += AllCaloCellD3PDObject(**_args (3, 'SelCells', kw,
                                          prefix="cell_",
                                          sgkey=PhotonD3PDMakerFlags.SelectedCellsContainerName(),
                                          include=['RawPosition']))
    #-------------------------------------------------------------------------------------


    #-------------------------------------------------------------------------------------
    # Muon information
    #
    from PhotonD3PDMaker.AddMuonsToD3PD import AddMuonsToD3PD
    AddMuonsToD3PD(alg, stacoMuonContainer, caloMuonContainer, **kw)
    #-------------------------------------------------------------------------------------


    #-------------------------------------------------------------------------------------
    # Tau information
    #
    if PhotonD3PDMakerFlags.DumpTauInformation():
        from TauD3PDMaker.TauD3PDObject                        import TauD3PDObject
        from TauD3PDMaker.makeTrackFilterAlg                   import makeTrackFilterAlg
        alg += TauD3PDObject(**_args ( 0, 'Tau', kw,
                                       include=['Kinematics','Charge','PID','TauJet',
                                                'TauCommonDetails','TauCalcVars']))
        if not hasattr( preseq, "GoodTracks" ):
            filter1 = makeTrackFilterAlg(TracksName = D3PDMakerFlags.TrackSGKey(),
                                         OutputTracksName='GoodTracks',
                                         ptCut=0.,nSCTPix=4)
            preseq += filter1
    #-------------------------------------------------------------------------------------


    #-------------------------------------------------------------------------------------
    # Jet information
    #
    # See prodJobOFragment for jet algorithm config.
    #
    from PhotonD3PDMaker.AddJetsToD3PD import AddJetsToD3PD
    AddJetsToD3PD(alg, **kw)
    #-------------------------------------------------------------------------------------


    #-------------------------------------------------------------------------------------
    # add some extra information to the photons
    #
    from PhotonD3PDMaker.AddPhotonsToD3PD import AddPhotonsToD3PD
    AddPhotonsToD3PD(alg, **kw)
    #-------------------------------------------------------------------------------------


    #-------------------------------------------------------------------------------------
    # add electron->photon extrapolation stuff
    #
    if PhotonD3PDMakerFlags.DumpElectronToPhoton():
        from PhotonD3PDMaker.ElectronToPhotonD3PDObject import ElectronToUnconvertedPhotonD3PDObject,ElectronToConvertedPhotonD3PDObject
        alg += ElectronToUnconvertedPhotonD3PDObject(**_args(0, 'ElectronToUnconvertedPhoton', kw))
        alg += ElectronToConvertedPhotonD3PDObject  (**_args(0, 'ElectronToConvertedPhoton', kw))
    #-------------------------------------------------------------------------------------


    #-------------------------------------------------------------------------------------
    # Electrons, including GSF electrons
    #
    from PhotonD3PDMaker.AddElectronsToD3PD import AddElectronsToD3PD
    AddElectronsToD3PD(alg, **kw)
    #-------------------------------------------------------------------------------------

    
    # ------------------------------------------------------------------------------------
    # Some truth filters, dumpers, and so on.
    #
    if rec.doTruth():
        from PhotonD3PDMaker.AddTruthToD3PD import AddTruthToD3PD
        AddTruthToD3PD(alg, **kw)
    #-------------------------------------------------------------------------------------

    return alg

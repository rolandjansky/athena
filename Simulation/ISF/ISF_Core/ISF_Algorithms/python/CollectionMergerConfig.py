# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

def ISFCollectionMergerCfg(flags,name="ISF_CollectionMerger", **kwargs):
    kwargs.setdefault( "InputBCMHits",              [ ] )
    kwargs.setdefault( "InputBLMHits",              [ ] )
    kwargs.setdefault( "InputPixelHits",            [ ] )
    kwargs.setdefault( "InputSCTHits",              [ ] )
    kwargs.setdefault( "InputTRTUncompressedHits",  [ ] )

    kwargs.setdefault( "InputLArEMBHits",           [ ] )
    kwargs.setdefault( "InputLArEMECHits",          [ ] )
    kwargs.setdefault( "InputLArFCALHits",          [ ] )
    kwargs.setdefault( "InputLArHECHits",           [ ] )

    kwargs.setdefault( "InputTileHits",             [ ] )
    kwargs.setdefault( "InputMBTSHits",             [ ] )

    kwargs.setdefault( "InputCSCHits",              [ ] )
    kwargs.setdefault( "InputMDTHits",              [ ] )
    kwargs.setdefault( "InputRPCHits",              [ ] )
    kwargs.setdefault( "InputTGCHits",              [ ] )
    hardscatterSG=""
    if flags.Digitization.Pileup is True:
        hardscatterSG = "OriginalEvent_SG+"
    kwargs.setdefault( "OutputBCMHits",             hardscatterSG+"BCMHits"             )
    kwargs.setdefault( "OutputBLMHits",             hardscatterSG+"BLMHits"             )
    kwargs.setdefault( "OutputPixelHits",           hardscatterSG+"PixelHits"           )
    kwargs.setdefault( "OutputSCTHits",             hardscatterSG+"SCT_Hits"            )
    kwargs.setdefault( "OutputTRTUncompressedHits", hardscatterSG+"TRTUncompressedHits" )

    kwargs.setdefault( "OutputLArEMBHits",          hardscatterSG+"LArHitEMB"           )
    kwargs.setdefault( "OutputLArEMECHits",         hardscatterSG+"LArHitEMEC"          )
    kwargs.setdefault( "OutputLArFCALHits",         hardscatterSG+"LArHitFCAL"          )
    kwargs.setdefault( "OutputLArHECHits",          hardscatterSG+"LArHitHEC"           )

    kwargs.setdefault( "OutputTileHits",            hardscatterSG+"TileHitVec"          )
    kwargs.setdefault( "OutputMBTSHits",            hardscatterSG+"MBTSHits"            )

    kwargs.setdefault( "OutputCSCHits",             hardscatterSG+"CSC_Hits"            )
    kwargs.setdefault( "OutputMDTHits",             hardscatterSG+"MDT_Hits"            )
    kwargs.setdefault( "OutputRPCHits",             hardscatterSG+"RPC_Hits"            )
    kwargs.setdefault( "OutputTGCHits",             hardscatterSG+"TGC_Hits"            )
    return CompFactory.ISF.CollectionMerger(name, **kwargs)


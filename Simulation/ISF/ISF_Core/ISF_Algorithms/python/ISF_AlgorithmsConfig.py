# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getCollectionMerger(name="ISF_CollectionMerger", **kwargs):
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

    kwargs.setdefault( "OutputBCMHits",             "BCMHits"             )
    kwargs.setdefault( "OutputBLMHits",             "BLMHits"             )
    kwargs.setdefault( "OutputPixelHits",           "PixelHits"           )
    kwargs.setdefault( "OutputSCTHits",             "SCT_Hits"            )
    kwargs.setdefault( "OutputTRTUncompressedHits", "TRTUncompressedHits" )

    kwargs.setdefault( "OutputLArEMBHits",          "LArHitEMB"           )
    kwargs.setdefault( "OutputLArEMECHits",         "LArHitEMEC"          )
    kwargs.setdefault( "OutputLArFCALHits",         "LArHitFCAL"          )
    kwargs.setdefault( "OutputLArHECHits",          "LArHitHEC"           )

    kwargs.setdefault( "OutputTileHits",            "TileHitVec"          )
    kwargs.setdefault( "OutputMBTSHits",            "MBTSHits"            )

    kwargs.setdefault( "OutputCSCHits",             "CSC_Hits"            )
    kwargs.setdefault( "OutputMDTHits",             "MDT_Hits"            )
    kwargs.setdefault( "OutputRPCHits",             "RPC_Hits"            )
    kwargs.setdefault( "OutputTGCHits",             "TGC_Hits"            )
    return CfgMgr.ISF__CollectionMerger(name, **kwargs)

def getSimHitTreeCreator(name="ISF_SimHitTreeCreator", **kwargs):
    return CfgMgr.ISF__SimHitTreeCreator(name, **kwargs)

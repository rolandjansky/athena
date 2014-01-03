# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# D3PD object saving information about RpcCoinData.
#
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
import TrigMuonD3PDMaker

from RecExConfig.ObjKeyStore import cfgKeyStore
if cfgKeyStore.isInInput( "Muon::RpcCoinDataContainer", "RPC_triggerHits" ):
    _haveRDO = True
else:
    _haveRDO = False
    from AthenaCommon.Logging import logging
    _mlog = logging.getLogger( "RpcCoinDataD3PDObject" )
    _mlog.warning( "No Muon::RpcCoinDataContainer object in input; skipping." )



RpcCoinDataD3PDObject = make_SG_D3PDObject( "Muon::RpcCoinDataContainer",
                                            "RPC_triggerHits",
                                            "rpc_coindata_",
                                            "RpcCoinDataD3PDObject",
                                            default_allowMissing = not _haveRDO )

if _haveRDO:
    #
    # Associate RpcCoinData objects to TrigT1CoinDataCollection objects:
    # for SL
    RpcSLAssoc = \
       ContainedVectorMultiAssociation( RpcCoinDataD3PDObject,
                                        TrigMuonD3PDMaker.RpcCoinDataContainerAssociationTool,
                                        blockname = "RpcAssoc" )

    RpcSLAssoc.defineBlock( 1, "BasicInfo",
                            TrigMuonD3PDMaker.RpcCoinDataFillerTool)

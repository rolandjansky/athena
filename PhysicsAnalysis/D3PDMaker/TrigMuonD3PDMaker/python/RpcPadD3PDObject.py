# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: RpcPadD3PDObject.py 439443 2011-05-26 01:35:49Z kanno $
#
# D3PD object saving information about RpcPad.
#
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
import TrigMuonD3PDMaker

from RecExConfig.ObjKeyStore import cfgKeyStore
if cfgKeyStore.isInInput( "RpcPadContainer", "RPCPAD" ):
    _haveRDO = True
else:
    _haveRDO = False
    from AthenaCommon.Logging import logging
    _mlog = logging.getLogger( "RpcPadD3PDObject" )
    _mlog.warning( "No RpcPadContainer object in input; skipping." )



RpcPadD3PDObject = make_SG_D3PDObject( "RpcPadContainer",
                                            "RPCPAD",
                                            "rpc_pad_",
                                            "RpcPadD3PDObject",
                                            default_allowMissing = not _haveRDO )


if _haveRDO:
    #
    # Associate RpcCoinData objects to TrigT1CoinDataCollection objects:
    # for SL
    RpcSLAssoc = \
       ContainedVectorMultiAssociation( RpcPadD3PDObject,
                                        TrigMuonD3PDMaker.RpcPadContainerAssociationTool,
                                        blockname = "RpcAssoc" )

    RpcSLAssoc.defineBlock( 1, "BasicInfo",
                            TrigMuonD3PDMaker.RpcPadFillerTool)

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# D3PD object saving the MuCTPI_RDO information into the D3PD
# Note that this can only be used on ESD files (or if back-navigation
# is active...)
#

from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
import TriggerD3PDMaker

# This object can only be used on (D)ESD files, when the MuCTPI_RDO object is
# available:
from RecExConfig.ObjKeyStore import cfgKeyStore
if cfgKeyStore.isInInputFile( "MuCTPI_RDO", "MUCTPI_RDO" ):
    _haveRDO = True
else:
    _haveRDO = False
    from AthenaCommon.Logging import logging
    _mlog = logging.getLogger( "MuCTPID3PDObject" )
    _mlog.warning( "No MuCTPI_RDO object in input; skipping." )

MuCTPID3PDObject = make_SG_D3PDObject( "MuCTPI_RDO", "MUCTPI_RDO",
                                       "muctpi_", "MuCTPID3PDObject",
                                       default_allowMissing = not _haveRDO )

if _haveRDO:
    # Make sure the cabling services are configured:
    import TrigT1RPCRecRoiSvc.TrigT1RPCRecRoiConfig  # noqa: F401
    import TrigT1TGCRecRoiSvc.TrigT1TGCRecRoiConfig  # noqa: F401

    # Define the blocks:
    MuCTPID3PDObject.defineBlock( 0, "RDOInfo",
                                  TriggerD3PDMaker.MuCTPI_RDOFillerTool,
                                  WriteRawInfo = True )

    MuCTPID3PDObject.defineBlock( 1, "DecodedInfo",
                                  TriggerD3PDMaker.MuCTPI_RDOFillerTool,
                                  WriteReconstructedInfo = True )
    pass

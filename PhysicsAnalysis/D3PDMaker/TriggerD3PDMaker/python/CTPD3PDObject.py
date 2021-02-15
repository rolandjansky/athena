# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# D3PD object saving the CTP_RDO information into the D3PD
# Note that this can only be used on ESD files (or if back-navigation
# is active...)
#

from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
import TriggerD3PDMaker

# This object can only be used on (D)ESD files, when the CTP_RDO object is
# available:
from RecExConfig.ObjKeyStore import cfgKeyStore
if cfgKeyStore.isInInput( "CTP_RDO", "CTP_RDO" ):
    _haveRDO = True
else:
    _haveRDO = False
    from AthenaCommon.Logging import logging
    _mlog = logging.getLogger( "CTPD3PDObject" )
    _mlog.warning( "No CTP_RDO object in input; skipping." )

CTPD3PDObject = make_SG_D3PDObject( "CTP_RDO", "CTP_RDO",
                                    "ctp_", "CTPD3PDObject",
                                    default_allowMissing = not _haveRDO )

if _haveRDO:
    CTPD3PDObject.defineBlock( 0, "RDOInfo",
                               TriggerD3PDMaker.CTP_RDOFillerTool )

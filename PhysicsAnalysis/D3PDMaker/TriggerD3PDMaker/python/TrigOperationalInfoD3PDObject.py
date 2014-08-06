# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigOperationalInfoD3PDObject.py 338058 2010-12-20 13:20:43Z krasznaa $
#
# D3PD object capable of saving the names of the chains which caused an event to
# end up in the express stream. Should only be useful for producing first trigger
# efficiency plots out of express stream data.
#

from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
import TriggerD3PDMaker

# This object can only be used on express stream (D)ESD files. So let's check
# whether the source object is available, because most of the time it isn't.
from RecExConfig.ObjKeyStore import cfgKeyStore
if cfgKeyStore.isInInput( "TrigOperationalInfoCollection", "HLT_EXPRESS_OPI_EF" ):
    _haveOpInfo = True
else:
    _haveOpInfo = False
    from AthenaCommon.Logging import logging
    _mlog = logging.getLogger( "TrigOperationalInfoD3PDObject" )
    _mlog.warning( "No TrigOperationalInfoCollection object in input; skipping." )

TrigOperationalInfoD3PDObject = make_SG_D3PDObject( "TrigOperationalInfoCollection",
                                                    "HLT_EXPRESS_OPI_EF",
                                                    "trig_express_",
                                                    "TrigOperationalInfoD3PDObject",
                                                    default_allowMissing = not _haveOpInfo )

if _haveOpInfo:
    TrigOperationalInfoD3PDObject.defineBlock( 0, "ExpressChains",
                                               TriggerD3PDMaker.TrigOperationalInfoFillerTool )

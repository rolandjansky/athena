# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Lucid_RawDataD3PDObject.py 352093 2011-03-16 13:03:57Z mtibbett $
#
# @file BackgroundD3PDMaker/python/Lucid_RawDataD3PDObject.py
# @author Mark Tibbets <mark.james.tibbetts@cern.ch>
# @date Feb, 2011
# @brief D3PD object for LUCID_RawData.
#

import D3PDMakerCoreComps    
import BackgroundD3PDMaker
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject

from RecExConfig.ObjKeyStore import cfgKeyStore
if cfgKeyStore.isInInput( "LUCID_RawDataContainer", "Lucid_RawData" ):
    _haveLucid = True
else:
    _haveLucid = False
    from AthenaCommon.Logging import logging
    _mlog = logging.getLogger( "Lucid_RawDataD3PDObject" )
    _mlog.warning( "No LUCID_RawDataContainer object in input; skipping." )

Lucid_RawDataD3PDObject = \
    make_SG_D3PDObject ('LUCID_RawDataContainer',
                        'Lucid_RawData',
                        'lucid_', 'Lucid_RawDataD3PDObject',
                        default_allowMissing = not _haveLucid )

if _haveLucid:
    Lucid_RawDataD3PDObject.defineBlock (0, 'LucidPMTHits',
                                         BackgroundD3PDMaker.LucidPMTHitFillerTool)

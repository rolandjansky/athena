# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# D3PD object saving information about the LVL2 MuonFeatureDetails objects.
#

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
import TrigMuonD3PDMaker

# This object can only be used on (D)ESD files, when the object is
# available:
from RecExConfig.ObjKeyStore import cfgKeyStore
if cfgKeyStore.isInInput( "MuonFeatureDetailsContainer", "HLT" ):
    _haveESD = True
else:
    _haveESD = False
    from AthenaCommon.Logging import logging
    _mlog = logging.getLogger( "MuonFeatureDetailsD3PDObject" )
    _mlog.warning( "No MuonFeatureDetails object in input; skipping." )

# Since the previous code doesn't seem to work in 16.6.X.Y on mc10a AODs, we should
# always allow the container to be missing:
MuonFeatureDetailsD3PDObject = make_SGDataVector_D3PDObject( "MuonFeatureDetailsContainer",
                                                             "HLT",
                                                             "trig_L2_muonfeaturedetails_",
                                                             "MuonFeatureDetailsD3PDObject",
                                                             default_allowMissing = True )

if _haveESD:
    MuonFeatureDetailsD3PDObject.defineBlock( 2, "DetailInfo",
                                              TrigMuonD3PDMaker.MuonFeatureDetailsFillerTool )

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file MissingETD3PDMaker/python/MissingETD3PDObject.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2014
# @brief Simple xAOD missing ET dumper.
#

from D3PDMakerCoreComps.D3PDObject  import make_SG_D3PDObject
#from D3PDMakerCoreComps.D3PDObject  import DeferArg
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
import D3PDMakerCoreComps
import MissingETD3PDMaker


MissingETD3PDObject = make_SG_D3PDObject ('xAOD::MissingETContainer_v1',
                                          D3PDMakerFlags.MissingETSGKey(),
                                          'met_',
                                          'MissingETD3PDObject')

def _metLevel (req_lod, blockargs, objargs):
    blockargs['Getter'].SGKey = objargs['sgkey']
    return req_lod >= 0
MissingETD3PDObject.defineBlock (
    _metLevel,
    'MET',
    MissingETD3PDMaker.MissingETContainerFillerTool,
    Getter = D3PDMakerCoreComps.SGObjGetterTool
      ('METAssocGetter',
       SGKey = '',
       TypeName = 'xAOD::MissingETContainer_v1'),
    )

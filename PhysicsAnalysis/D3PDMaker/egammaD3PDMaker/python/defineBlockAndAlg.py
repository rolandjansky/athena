# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file egammaD3PDMaker/python/defineBlockAndAlg.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2011
# @brief Helper to schedule an algorithm only if a block is included.
#

from AthenaCommon.AlgSequence                import AlgSequence
from D3PDMakerConfig.D3PDMakerFlags          import D3PDMakerFlags

def defineAlgLODFunc (level, algconfig,
                      algpkg = 'egammaD3PDAnalysis'):
    """Return a level-of-detail function that also schedules an algorithm.

    LEVEL is the level of detail.

    ALGCONFIG is the name of a configuration file in egammaD3PDAnalysis
    that schedules the needed algorithm.

    ALGPKG is the name of the package containing ALGCONFIG.
"""

    def lodfunc (reqlev, blockargs, hookargs):
        if reqlev < level: return False
        preseq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName())
        algmod = __import__ ('egammaD3PDAnalysis.' + algconfig,
                             fromlist = [algconfig])
        algfunc = getattr (algmod, algconfig)
        algfunc (seq = hookargs.get('seq', preseq),
                 prefix = hookargs['prefix'],
                 sgkey = hookargs['sgkey'],
                 typeName = hookargs['typeName'],
                 allowMissing = hookargs.get('allowMissing',False))
        return True
    return lodfunc

def defineBlockAndAlg (d3pdo, level, blockname, blockfunc, algconfig,
                       algpkg = 'egammaD3PDAnalysis',
                       **kw):
    """Define a block, and schedule an algorithm if the block is used.

    D3PDO is the D3PDObject for which the block should be defined.

    LEVEL is the level of detail.

    BLOCKNAME is the name of the block.

    BLOCKFUNC is the function that creates the block.

    ALGCONFIG is the name of a configuration file
    that schedules the needed algorithm.

    ALGPKG is the name of the package containing ALGCONFIG.
"""

    lodfunc = defineAlgLODFunc (level, algconfig, algpkg)
    d3pdo.defineBlock (lodfunc, blockname, blockfunc, **kw)
    return

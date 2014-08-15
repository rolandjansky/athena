# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file CaloD3PDMaker/python/LArCollisionTimeD3PDObject.py
# @author scott snyder <snyder@bnl.gov>
# @date Mar, 2010
# @brief D3PD object for LAr collision time variables.
#


import CaloD3PDMaker
from D3PDMakerConfig.D3PDMakerFlags    import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject     import make_SG_D3PDObject
from LArCellRec.LArCollisionTimeGetter import LArCollisionTimeGetter
from AthenaCommon.AlgSequence          import AlgSequence
from RecExConfig.ObjKeyStore           import cfgKeyStore


# We can only do this if we have the cell container.
if cfgKeyStore.isInInput ('CaloCellContainer', 'AllCalo'):
    _haveCells = True
else:
    _haveCells = False
    from AthenaCommon.Logging import logging
    mlog = logging.getLogger( 'LArCollisionTimeD3PDObject' )
    mlog.warning ('No AllCalo cell container; skipping.')


LArCollisionTimeD3PDObject = \
           make_SG_D3PDObject ('LArCollisionTime',
                               D3PDMakerFlags.LArCollisionTimeSGKey(),
                               'lar_', 'LArCollisionTimeD3PDObject',
                               default_allowMissing = not _haveCells)

if _haveCells:
    def _larCollTimeAlgHook (c,
                             seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
                             *args, **kw):
        LArCollisionTimeGetter (seq)
        return
    LArCollisionTimeD3PDObject.defineHook (_larCollTimeAlgHook)


    LArCollisionTimeD3PDObject.defineBlock \
                  (0, 'LArCollisionTime',
                   CaloD3PDMaker.LArCollisionTimeFillerTool)

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#
# @file TileRecUtils/python/TileDQstatusAlgDefault.py
# @author scott snyder <snyder@bnl.gov>
# @date Nov, 2018
# @brief Configure TileDQstatusAlg
#

from TileRecUtils.TileRecUtilsConf import TileDQstatusAlg


def _updateProp (dq, name, val):
    if dq.properties()[name] == dq.propertyNoValue or getattr (dq, name) == '':
        setattr (dq, name, val)
        return
    if val == '':
        return
    oval = getattr (dq, name)
    if oval == val:
        return
    if oval.find ('ServiceHandle') >= 0 or oval.find ('ToolHandle') >= 0:
        setattr (dq, name, val)
        return
    from AthenaCommon.Logging import logging
    log = logging.getLogger ('TileDQstatusAlgDefault')
    log.error ('Conflicting settings for %s.%s; old: %s; new: %s' %
               (dq.name(), name, oval, val))
    import traceback
    traceback.print_stack()
    assert 0


def TileDQstatusAlgDefault (name = 'TileDQstatusAlg',
                            TileRawChannelContainer = '',
                            TileDigitsContainer = '',
                            TileBeamElemContainer = '',
                            TileDQstatus = 'TileDQstatus',
                            **kw):
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    dq = getattr (topSequence, name, None)
    if not dq:
        dq = TileDQstatusAlg (name,
                              TileRawChannelContainer = TileRawChannelContainer,
                              TileDigitsContainer = TileDigitsContainer,
                              TileBeamElemContainer = TileBeamElemContainer,
                              TileDQstatus = TileDQstatus,
                              **kw)
        topSequence += dq
    else:
        _updateProp (dq, 'TileRawChannelContainer', TileRawChannelContainer)
        _updateProp (dq, 'TileDigitsContainer',     TileDigitsContainer)
        _updateProp (dq, 'TileBeamElemContainer',   TileBeamElemContainer)
        _updateProp (dq, 'TileDQstatus',            TileDQstatus)
        for (k, v) in kw.items():
            _updateProp (dq, k, v)
    return dq

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigHypoCommonTools.TrigHypoCommonToolsConf import TrigComboHypoTool
from AthenaCommon.Logging import logging

log = logging.getLogger('TrigHypoCommonTools')

## Parsing variable cut function
def setVarCut (obj, var, val):
    log.debug(obj.name)
    for i in range(len(var)):
        log.debug('Update variable cut: %s = %f', var[i], val[i])
        obj.__setattr__(var[i], val[i])

class TrigComboHypoTool (TrigComboHypoTool):
    __slots__ = []
    def __init__(self, name = "TrigComboHypoTool", var = [], val = []):
        super(TrigComboHypoTool, self).__init__(name)
        setVarCut(self, var, val)

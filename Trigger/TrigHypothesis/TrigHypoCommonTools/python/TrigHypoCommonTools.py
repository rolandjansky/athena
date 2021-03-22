# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# flake8: noqa  (legacy trigger)

######################################################################
#
#  Settings of  Slice HYPO
# Author: G. Pezzullo
######################################################################

from TrigHypoCommonTools.TrigHypoCommonToolsConf import *
from AthenaCommon.Logging import logging

## Parsing variable cut function
def setVarCut (obj, var, val):
    logging.getLogger("[TrigComboHyp::setVarCut] TrigHypoCommonToolsConfig").debug(obj.name)
    for i in range(len(var)):
        logging.getLogger("[TrigComboHyp::setVarCut] TrigHypoCommonToolsConfig").debug('Update variable cut: %s = %f'%(var[i],val[i]))
        obj.__setattr__(var[i], val[i])

class TrigComboHypoTool (TrigComboHypoTool):
    __slots__ = []
    def __init__(self, name = "TrigComboHypoTool", var = [], val = []):
        logging.getLogger("[TrigComboHypoTool] intialize").debug(self.name)
        super(TrigComboHypoTool, self).__init__(name)
        setVarCut(self, var, val)

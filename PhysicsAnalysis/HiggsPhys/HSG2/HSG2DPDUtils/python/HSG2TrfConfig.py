# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
__all__ = []

from PyJobTransformsCore.TransformConfig import *
import PyJobTransformsCore.basic_trfarg as trfarg


class ElectronQualityArg(trfarg.StringChoicesArg):
    def __init__(self, choices, caseSensitive=False, help='electron quality to be used', name='electronQuality'):
        trfarg.StringChoicesArg.__init__(self, choices, help, name, caseSensitive=True)
    def isFullArgument(self):
        return True

class MuonQualityArg(trfarg.StringChoicesArg):
    def __init__(self, choices, caseSensitive=False, help='muon quality to be used', name='muonQuality'):
        trfarg.StringChoicesArg.__init__(self, choices, help, name, caseSensitive=True)
    def isFullArgument(self):
        return True

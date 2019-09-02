# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigOnlineMonitor.TrigOnlineMonitorConf import TrigOpMonitor as _TrigOpMonitor
from AthenaCommon.AlgSequence import AthSequencer

class TrigOpMonitor(_TrigOpMonitor):
   __slots__ = ()

   def __init__(self, name='TrigOpMonitor', **kwargs):
      super(TrigOpMonitor, self).__init__(name, **kwargs)

      # Only monitor lumi if available
      self.LuminosityCondDataKey = ''
      condSeq = AthSequencer('AthCondSeq')
      for a in condSeq:
         if a.getType()=='LuminosityCondAlg':
            self.LuminosityCondDataKey = a.LuminosityOutputKey
            break

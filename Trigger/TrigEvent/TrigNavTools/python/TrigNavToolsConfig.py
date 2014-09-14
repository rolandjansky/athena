# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigNavTools.TrigNavToolsConf import HLT__TrigNavigationSlimming
from TrigNavTools.TrigNavToolsConf import HLT__TrigNavigationSlimmingTool
from TrigNavTools.TrigNavToolsConf import HLT__StreamTrigNavSlimming

class HLTTrigNavigationSlimmingTool ( HLT__TrigNavigationSlimmingTool ):
  def __init__(self, name="TrigNavigationSlimmingTool"):
    super( HLTTrigNavigationSlimmingTool, self).__init__( name )


class HLTTrigNavigationSlimming ( HLT__TrigNavigationSlimming ):
  def __init__(self, name="TrigNavigationSlimming"):
    super( HLTTrigNavigationSlimming, self).__init__( name )
    self.WriteTree = 1


class HLTStreamTrigNavSlimming ( HLT__StreamTrigNavSlimming ):
  def __init__(self, name="StreamTrigNavSlimming"):
    super( HLTStreamTrigNavSlimming, self ).__init__(name)

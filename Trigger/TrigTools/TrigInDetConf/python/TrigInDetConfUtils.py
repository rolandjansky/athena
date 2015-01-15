# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigGenericAlgs.TrigGenericAlgsConf import PESA__InDetTrigUpdater

from AthenaCommon.Logging import logging
log = logging.getLogger("IDTrig")

class InDetTrigUpdater_IDTrig ( PESA__InDetTrigUpdater ):
  __slots__ = []
  def __init__(self, name="InDetTrigUpdater_Electron_IDTrig", type="electron"):
    super( PESA__InDetTrigUpdater, self).__init__(name)

    self.EtaHalfWidth = 0.1
    self.PhiHalfWidth = 0.1


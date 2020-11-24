# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from PFlowUtils.PFlowUtilsConf import CP__WeightPFOTool as WeightPFOTool

def declareDefaultTools():

  from JetRecConfig.JetRecStandardToolManager import jtm

  # Weight tool for charged pflow objects.
  jtm += WeightPFOTool("pflowweighter")
  jtm += WeightPFOTool("pflowweighter_LC",NeutralPFOScale="LC")

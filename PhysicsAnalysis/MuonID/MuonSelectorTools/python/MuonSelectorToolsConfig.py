# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
  
#********************************************************************
# MuonsSelectionToolConfig.py 
# Configures muon selection tool which is used to select muons 
# for use in physics analysis
#********************************************************************

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import LHCPeriod

### Standard configuration of the MuonSelectionTool used in reconstruction & validation jobs
### The snippet is not meant for analysis jobs as it inherently switches off important cuts ensuring 
### best muon selection quality
def MuonSelectionToolCfg(flags, name="MuonSelectionTool", **kwargs):
   """Configure the muon selection tool"""
   acc = ComponentAccumulator()
   kwargs.setdefault("IsRun3Geo", flags.GeoModel.Run >= LHCPeriod.Run3 )
   kwargs.setdefault("DisablePtCuts", True)
   kwargs.setdefault("TurnOffMomCorr", True)
   the_tool = CompFactory.CP.MuonSelectionTool(name, **kwargs)   
   acc.setPrivateTools(the_tool)
   return acc   


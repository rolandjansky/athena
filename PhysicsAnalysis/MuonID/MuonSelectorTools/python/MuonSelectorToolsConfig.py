# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
  
#********************************************************************
# MuonsSelectionToolConfig.py 
# Configures muon selection tool which is used to select muons 
# for use in physics analysis
#********************************************************************

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def MuonSelectionToolCfg(ConfigFlags, name, **kwargs):
   """Configure the muon selection tool"""
   acc = ComponentAccumulator()
   MuonSelectionTool = CompFactory.CP.MuonSelectionTool
   acc.setPrivateTools(MuonSelectionTool(name, **kwargs))
   return acc   


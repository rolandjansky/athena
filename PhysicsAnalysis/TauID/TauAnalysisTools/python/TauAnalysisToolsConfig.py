# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TauSelectionToolCfg(ConfigFlags, name, **kwargs):
   """Configure the tau selection tool"""
   acc = ComponentAccumulator()
   TauSelectionTool = CompFactory.TauAnalysisTools.TauSelectionTool
   acc.setPrivateTools(TauSelectionTool(name, **kwargs))
   return acc   


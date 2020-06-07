# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ToolSvc


def slimmingTool( config ):
  from TrigNavTools.TrigNavToolsConf import HLT__TrigNavigationSlimmingTool
  global ToolSvc
  assert 'name' in config, 'name of the configuration is missing'
  assert 'mode' in config, 'mode of slimming has to be configured'

  SlimTool=HLT__TrigNavigationSlimmingTool( config['name']+'Slim' )  
  if 'chains' in  config:
    SlimTool.ChainsRegex = config['chains']
  if 'features' in config:
    SlimTool.FeatureInclusionList=config['features']  

  if config['mode'] == 'drop':    
    SlimTool.Actions = [ 'Drop' ]
  if config['mode'] == 'slimming':    
    SlimTool.Actions = [  'DropFeatures', 'Squeeze', 'Reload', 'SyncThinning', 'DropChains', 'Save', 'Restore']
  if config['mode'] == 'trigger':    
    SlimTool.Actions = [ 'DropFeatures', 'Reload', 'SyncThinning', 'DropChains', 'Save', 'Restore']
  if config['mode'] == 'cleanup':    
    SlimTool.Actions = [ 'DropFeatures', 'Reload', 'SyncThinning', 'Save']

  if 'Print' in SlimTool.Actions:
    from AthenaCommon.Constants import DEBUG
    SlimTool.OutputLevel=DEBUG
  ToolSvc += SlimTool
  return SlimTool


def navigationThinningSvc (config):
  from TrigNavTools.TrigNavToolsConf import TrigNavigationThinningSvc
  slimTool = slimmingTool (config)
  svc = TrigNavigationThinningSvc (config['name'] + 'ThinSvc',
                                   SlimmingTool = slimTool)
  from AthenaCommon.AppMgr import ServiceMgr
  ServiceMgr += svc
  return svc

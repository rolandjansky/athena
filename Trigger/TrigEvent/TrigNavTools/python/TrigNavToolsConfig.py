# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ToolSvc
from TrigNavTools.TrigNavToolsConf import HLT__TrigNavigationSlimmingTool, TrigNavigationThinningTool


def slimmingTool( config ):
  global ToolSvc
  assert 'name' in config, 'name of the configuration is missing'
  assert 'mode' in config, 'mode of slimming has to be configured'
  assert 'ThinningSvc' in config, 'Instance of thinning svc to talk to is indispensable'
  assert config['ThinningSvc'], 'No ThinningSvc given'

  SlimTool=HLT__TrigNavigationSlimmingTool( config['name']+'Slim' )  
  SlimTool.ThinningSvc = config['ThinningSvc']
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

def navigationSlimming( config ):  
  global ToolSvc

  SlimTool = slimmingTool(config)

  ThinTool = TrigNavigationThinningTool(config['name']+'Thin')
  ThinTool.ThinningSvc = config['ThinningSvc']
  #  ThinTool.ActInPlace=False

  if 'xAOD' in config:
    ThinTool.ResultKey=''
    ThinTool.xAODNavigationKey=config['result']
  
  if 'result' in config:
    ThinTool.ResultKey=config['result']
  else:
    ThinTool.ResultKey='HLTResult_HLT'
  ThinTool.SlimmingTool = SlimTool
  ToolSvc += ThinTool

  return ThinTool




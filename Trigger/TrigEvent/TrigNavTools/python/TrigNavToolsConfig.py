# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


def navigationThinningSvc (config):
  assert 'name' in config, 'name of the configuration is missing'
  assert 'mode' in config, 'mode of slimming has to be configured'

  from TrigNavTools.TrigNavToolsConf import TrigNavigationThinningSvc
  svc = TrigNavigationThinningSvc (config['name'] + 'ThinSvc')

  if 'chains' in  config:
    svc.ChainsRegex = config['chains']
  if 'features' in config:
    svc.FeatureInclusionList=config['features']  

  if config['mode'] == 'drop':    
    svc.Actions = [ 'Drop' ]
  if config['mode'] == 'slimming':    
    svc.Actions = [  'DropFeatures', 'Squeeze', 'Reload', 'SyncThinning', 'DropChains', 'Save', 'Restore']
  if config['mode'] == 'trigger':    
    svc.Actions = [ 'DropFeatures', 'Reload', 'SyncThinning', 'DropChains', 'Save', 'Restore']
  if config['mode'] == 'cleanup':    
    svc.Actions = [ 'DropFeatures', 'Reload', 'SyncThinning', 'Save']

  if 'Print' in svc.Actions:
    from AthenaCommon.Constants import DEBUG
    svc.OutputLevel=DEBUG

  from AthenaCommon.AppMgr import ServiceMgr
  ServiceMgr += svc
  return svc

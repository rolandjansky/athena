# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
def _actions(mode):
  options =   {'drop':    [ 'Drop' ],
    'slimming': [  'DropFeatures', 'Squeeze', 'Reload', 'SyncThinning', 'DropChains', 'Save', 'Restore'],
    'trigger':  [ 'DropFeatures', 'Reload', 'SyncThinning', 'DropChains', 'Save', 'Restore'],
    'cleanup':  [ 'DropFeatures', 'Reload', 'SyncThinning', 'Save'],
    'cleanup_noreload':    [ 'DropFeatures', 'SyncThinning', 'Save']
    }
  return options[mode]


def navigationThinningSvc (config):
  assert 'name' in config, 'name of the configuration is missing'
  assert 'mode' in config, 'mode of slimming has to be configured'

  from TrigNavTools.TrigNavToolsConf import TrigNavigationThinningSvc
  svc = TrigNavigationThinningSvc (config['name'] + 'ThinSvc')

  if 'chains' in  config:
    svc.ChainsRegex = config['chains']
  if 'features' in config:
    svc.FeatureInclusionList=config['features']  
  svc.Actions = _actions(config['mode'])
  if 'Print' in svc.Actions:
    from AthenaCommon.Constants import DEBUG
    svc.OutputLevel=DEBUG

  from AthenaCommon.AppMgr import ServiceMgr
  ServiceMgr += svc
  return svc


def TrigNavigationThinningSvcCfg(flags, thinningConfig):
  assert 'name' in thinningConfig, 'name of the configuration is missing'
  assert 'mode' in thinningConfig, 'mode of slimming has to be configured'

  acc = ComponentAccumulator()
  svc = CompFactory.TrigNavigationThinningSvc(thinningConfig['name']+'ThinSvc', 
          Actions = _actions(thinningConfig['mode']) )
  if 'chains' in thinningConfig:
    svc.ChainsRegex = thinningConfig['chains']
  if 'features' in thinningConfig:
    svc.FeatureInclusionList = thinningConfig['features']
  if 'Print' in svc.Actions:
    from AthenaCommon.Constants import DEBUG
    svc.OutputLevel=DEBUG
  acc.addService(svc, primary=True)
  return acc

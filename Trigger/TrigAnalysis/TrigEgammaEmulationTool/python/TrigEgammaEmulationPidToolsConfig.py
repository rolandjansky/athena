# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# TrigEgammaPidTools
# Add all pid selectors to ToolSvc
# Using versioned configuration -- over rides the configuration defined in mapping
# Conf files should be moved to $CALIBPATH and seperated into directories
#
# Trigger specific interface methods have been removed
# Behavior now set via properties
# Results is multiple instances of tools to apply calo-only and trigger threshold
###############################################################

from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ToolSvc

import PyUtils.RootUtils as ru
ROOT = ru.import_root()
import cppyy
cppyy.loadDictionary('ElectronPhotonSelectorToolsDict')
from ROOT import LikeEnum
from ROOT import egammaPID

from ElectronPhotonSelectorTools.TrigEGammaPIDdefs import SelectionDefElectron
from ElectronPhotonSelectorTools.ElectronPhotonSelectorToolsConf import AsgElectronIsEMSelector
from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping import ElectronIsEMMap,electronPIDmenu

###################################################################################################

# http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/ElectronPhotonSelectorTools/trigger/rel21_20170214/
# http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/ElectronPhotonSelectorTools/trigger/rel21_20170217/
# http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/ElectronPhotonSelectorTools/trigger/rel21_20170217_mc16a/
def getEgammaIsEMSelectorCaloOnly( calibPath ):

  from AthenaCommon.AppMgr import ToolSvc
  calibname = calibPath.split('/')[-1]
  configList = [calibPath+"/ElectronIsEMTightSelectorCutDefs.conf",
                calibPath+"/ElectronIsEMMediumSelectorCutDefs.conf",
                calibPath+"/ElectronIsEMLooseSelectorCutDefs.conf",
                calibPath+"/ElectronIsEMVLooseSelectorCutDefs.conf",
                ]
  asgTools = []
  mask = [SelectionDefElectron.ElectronTightHLT, SelectionDefElectron.ElectronMediumHLT, 
          SelectionDefElectron.ElectronLooseHLT, SelectionDefElectron.ElectronLooseHLT]
  for idx, config in enumerate(configList):
    name = config.split('/')[-1].replace('.conf','_EFCalo_'+calibname)
    asg = CfgMgr.AsgElectronIsEMSelector(name)
    asg.caloOnly=True; asg.isEMMask=mask[idx];  asg.ConfigFile=config; ToolSvc+=asg; asgTools.append(asg)
  return asgTools


# http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/ElectronPhotonSelectorTools/trigger/rel21_20170214/
# http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/ElectronPhotonSelectorTools/trigger/rel21_20170217/
# http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/ElectronPhotonSelectorTools/trigger/rel21_20170217_mc16a/
def getElectronIsEMSelector( calibPath ):

  from AthenaCommon.AppMgr import ToolSvc
  calibname = calibPath.split('/')[-1]
  configList = [calibPath+"/ElectronIsEMTightSelectorCutDefs.conf",
                calibPath+"/ElectronIsEMMediumSelectorCutDefs.conf",
                calibPath+"/ElectronIsEMLooseSelectorCutDefs.conf",
                calibPath+"/ElectronIsEMVLooseSelectorCutDefs.conf"
                ]
  asgTools = []
  mask = [SelectionDefElectron.ElectronTightHLT, SelectionDefElectron.ElectronMediumHLT, 
      SelectionDefElectron.ElectronLooseHLT, SelectionDefElectron.ElectronLooseHLT]
  for idx, config in enumerate(configList):
    name = config.split('/')[-1].replace('.conf','_HLT_'+calibname)
    asg = CfgMgr.AsgElectronIsEMSelector(name)
    asg.isEMMask=mask[idx];  asg.ConfigFile=config; ToolSvc+=asg; asgTools.append(asg)
  return asgTools

# http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/ElectronPhotonSelectorTools/trigger/rel21_20170214/
# http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/ElectronPhotonSelectorTools/trigger/rel21_20170217/
# http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/ElectronPhotonSelectorTools/trigger/rel21_20170217_mc16a/
def getEgammaLikelihoodSelectorCaloOnly( calibPath ):
  
  from AthenaCommon.AppMgr import ToolSvc
  calibname = calibPath.split('/')[-1]
  configList = [
                calibPath+'/ElectronLikelihoodTightTriggerConfig_CaloOnly.conf',
                calibPath+'/ElectronLikelihoodMediumTriggerConfig_CaloOnly.conf',
                calibPath+'/ElectronLikelihoodLooseTriggerConfig_CaloOnly.conf',
                calibPath+'/ElectronLikelihoodVeryLooseTriggerConfig_CaloOnly.conf',
                ]
  asgTools = []
  for idx, config in enumerate(configList):
    name = config.split('/')[-1].replace('.conf','_EFCalo_'+calibname)
    asg = CfgMgr.AsgElectronLikelihoodTool(name)
    asg.usePVContainer = False;  asg.ConfigFile=config; asg.caloOnly = True; ToolSvc+=asg; asgTools.append(asg)
  return asgTools

# http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/ElectronPhotonSelectorTools/trigger/rel21_20170214/
# http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/ElectronPhotonSelectorTools/trigger/rel21_20170217/
# http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/ElectronPhotonSelectorTools/trigger/rel21_20170217_mc16a/
def getElectronLikelihoodSelector2015( calibPath ):
  
  from AthenaCommon.AppMgr import ToolSvc
  calibname = calibPath.split('/')[-1]
  configList = [
                calibPath+'/ElectronLikelihoodTightTriggerConfig2015.conf',
                calibPath+'/ElectronLikelihoodMediumTriggerConfig2015.conf',
                calibPath+'/ElectronLikelihoodLooseTriggerConfig2015.conf',
                calibPath+'/ElectronLikelihoodVeryLooseTriggerConfig2015.conf',
                ]
  asgTools = []
  for idx, config in enumerate(configList):
    name = config.split('/')[-1].replace('.conf','_HLT_'+calibname)
    asg = CfgMgr.AsgElectronLikelihoodTool(name)
    asg.usePVContainer = False;  asg.ConfigFile=config; asg.caloOnly = True; ToolSvc+=asg; asgTools.append(asg)
  return asgTools

# http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/ElectronPhotonSelectorTools/trigger/rel21_20170214/
# http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/ElectronPhotonSelectorTools/trigger/rel21_20170217/
# http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/ElectronPhotonSelectorTools/trigger/rel21_20170217_mc16a/
def getElectronLikelihoodSelectorNoD0( calibPath ):
  
  from AthenaCommon.AppMgr import ToolSvc
  calibname = calibPath.split('/')[-1]
  configList = [
                calibPath+'/ElectronLikelihoodTightTriggerConfig.conf',
                calibPath+'/ElectronLikelihoodMediumTriggerConfig.conf',
                calibPath+'/ElectronLikelihoodLooseTriggerConfig.conf',
                calibPath+'/ElectronLikelihoodVeryLooseTriggerConfig.conf',
                ]
  asgTools = []
  for idx, config in enumerate(configList):
    name = config.split('/')[-1].replace('.conf','_HLT_'+calibname)
    asg = CfgMgr.AsgElectronLikelihoodTool(name)
    asg.usePVContainer = False;  asg.ConfigFile=config; asg.caloOnly = True; ToolSvc+=asg; asgTools.append(asg)
  return asgTools

#####################################################################################################


# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#====================================================================
# PhysCommonTrigger.py
# This defines the common trigger matching shared by PHYS and PHYSLITE
# Using it avoids name clashes when running in trains
# In principle it can also be used by other formats who want to take
# advantage of PHYS/PHYSLITE containers
#====================================================================
from TriggerMenuMT.TriggerAPI.TriggerAPI import TriggerAPI
from TriggerMenuMT.TriggerAPI.TriggerEnums import TriggerPeriod, TriggerType
from DerivationFrameworkTrigger.TriggerMatchingHelper import TriggerMatchingHelper
from PathResolver import PathResolver
import re

def read_trig_list_file(fname):
   """Read a text file containing a list of triggers
   
   Returns the list of triggers held in the file
   """
   triggers = []
   with open(PathResolver.FindCalibFile(fname)) as fp:
      for line in fp:
         line = line.strip()
         if line == "" or line.startswith("#"):
            continue
         triggers.append(line)
   return triggers

#====================================================================
# TRIGGER CONTENT   
#====================================================================
## See https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerAPI
## Get single and multi mu, e, photon triggers
## Jet, tau, multi-object triggers not available in the matching code
allperiods = TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.y2018 | TriggerPeriod.future2e34
trig_el  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el,  livefraction=0.8)
trig_mu  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu,  livefraction=0.8)
trig_g   = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.g,   livefraction=0.8)
trig_tau = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.tau, livefraction=0.8)
## Add cross-triggers for some sets
trig_em = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.mu,  livefraction=0.8)
trig_et = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.tau, livefraction=0.8)
trig_mt = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu, additionalTriggerType=TriggerType.tau, livefraction=0.8)
## Note that this seems to pick up both isolated and non-isolated triggers already, so no need for extra grabs
trig_txe = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.tau, additionalTriggerType=TriggerType.xe, livefraction=0.8)

extra_notau = read_trig_list_file("DerivationFrameworkPhys/run2ExtraMatchingTriggers.txt")
extra_tau = read_trig_list_file("DerivationFrameworkPhys/run2ExtraMatchingTauTriggers.txt")
#
## Merge and remove duplicates
trigger_names_full_notau = list(set(trig_el+trig_mu+trig_g+trig_em+trig_et+trig_mt+extra_notau))
trigger_names_full_tau = list(set(trig_tau+trig_txe+extra_tau))
#
## Now reduce the list...
trigger_names_notau = []
trigger_names_tau = []
from AthenaConfiguration.AutoConfigFlags import GetFileMD
from AthenaConfiguration.AllConfigFlags import ConfigFlags

if ConfigFlags.Trigger.EDMVersion == 3:
   r_tau = re.compile("HLT_.*tau.*")
   r_notau = re.compile("HLT_[1-9]*(e|mu|g).*") 
   for chain_name in GetFileMD(ConfigFlags.Input.Files)['TriggerMenu']['HLTChains']:
      result_tau = r_tau.match(chain_name)
      result_notau = r_notau.match(chain_name)
      if result_tau is not None: trigger_names_tau.append(chain_name)
      if result_notau is not None: trigger_names_notau.append(chain_name)
   trigger_names_all = set.union(set(trigger_names_notau), set(trigger_names_tau))
   trigger_names_all = list(trigger_names_all)
   trigger_names_notau = set(trigger_names_notau) - set(trigger_names_tau)
   trigger_names_notau = list(trigger_names_notau)

   # Run 3 trigger navigation slimming proposal for in-DAOD trigger matching.
   from TrigNavSlimmingMT.TrigNavSlimmingMTConfig import TrigNavSlimmingMTDerivationCfg
   from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
   CAtoGlobalWrapper(TrigNavSlimmingMTDerivationCfg, ConfigFlags, chainsFilter=trigger_names_all)

else:
   # Note: ['TriggerMenu']['HLTChains'] python access is maintained for compatibility with Run 2 MC 
   # POOL inputs (containing xAOD::TriggerMenu).
   for chain_name in GetFileMD(ConfigFlags.Input.Files)['TriggerMenu']['HLTChains']:
      if chain_name in trigger_names_full_notau: trigger_names_notau.append(chain_name)
      if chain_name in trigger_names_full_tau:   trigger_names_tau.append(chain_name) 
# Create trigger matching decorations
trigmatching_helper_notau = TriggerMatchingHelper(name='PhysCommonTriggerMatchingToolNoTau',
        trigger_list = trigger_names_notau, add_to_df_job=True)
trigmatching_helper_tau = TriggerMatchingHelper(name='PhysCommonTriggerMatchingToolTau',
        trigger_list = trigger_names_tau, add_to_df_job=True, DRThreshold=0.2)


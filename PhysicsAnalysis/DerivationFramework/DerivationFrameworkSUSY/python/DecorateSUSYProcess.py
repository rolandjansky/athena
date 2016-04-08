# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#==============================================================================
# SUSY signal process augmentation
#==============================================================================


def IsSUSYSignal():
  
  from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo
  if not DerivationFrameworkIsMonteCarlo:
    return False

  import PyUtils.AthFile
  from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
  
  fileinfo = PyUtils.AthFile.fopen(athenaCommonFlags.PoolAODInput()[0])
  
  if "mc_channel_number" in fileinfo.infos and len(fileinfo.mc_channel_number) == 1:
    mc_channel_number = fileinfo.mc_channel_number[0]
    issusy = (370000 <= mc_channel_number < 405000) or (406000 <= mc_channel_number < 410000) # https://svnweb.cern.ch/trac/atlasoff/browser/Generators/MC15JobOptions/trunk/share/Blocks.list
    print "DecorateSUSYProcess: fileinfo.mc_channel_number", mc_channel_number, "is SUSY:", issusy
    return issusy
  else:
    print "DecorateSUSYProcess: WARNING: fileinfo.mc_channel_number not filled?"
    return False


def DecorateSUSYProcess(derivationname):

  if IsSUSYSignal():
    
    from AthenaCommon.AppMgr import ToolSvc
    from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__SUSYSignalTagger
    
    SUSYSignalTagger = DerivationFramework__SUSYSignalTagger(name = derivationname + "SignalTagger")
    SUSYSignalTagger.EventInfoName    = "EventInfo"
    SUSYSignalTagger.MCCollectionName = "TruthParticles"
    ToolSvc += SUSYSignalTagger
    
    return [SUSYSignalTagger]

  return []

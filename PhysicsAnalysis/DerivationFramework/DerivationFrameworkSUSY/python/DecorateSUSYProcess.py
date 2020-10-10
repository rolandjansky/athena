# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#==============================================================================
# SUSY signal process augmentation
#==============================================================================


def IsSUSYSignal():
  
  from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkHasTruth
  if not DerivationFrameworkHasTruth:
    return False

  import PyUtils.AthFile
  from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

  # If someone has set evgen input, assume that they want to use it
  if not athenaCommonFlags.PoolEvgenInput.isDefault():
    fileinfo = PyUtils.AthFile.fopen(athenaCommonFlags.PoolEvgenInput()[0])
  elif not athenaCommonFlags.PoolAODInput.isDefault():
    fileinfo = PyUtils.AthFile.fopen(athenaCommonFlags.PoolAODInput()[0])
  else:
    fileinfo = PyUtils.AthFile.fopen(athenaCommonFlags.FilesInput()[0])

  if "mc_channel_number" in fileinfo.infos and len(fileinfo.mc_channel_number) == 1:
    mc_channel_number = fileinfo.mc_channel_number[0]
    if mc_channel_number==0: mc_channel_number = fileinfo.run_number[0]

    # with MC16, there are no dedicated SUSY DSID blocks anymore but blocks for
    # each generator: see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PmgMcSoftware#DSID_blocks
    if mc_channel_number >= 500000:
        # there does not seem to be an elegant way to check whether its a SUSY sample
        # or not, thus run the mark all MadGraph samples as SUSY for the moment
        # to non-SUSY MG samples this adds only an empty decoration and does not break anything
        issusy = (mc_channel_number < 600000)
        print "DecorateSUSYProcess: fileinfo.mc_channel_number", mc_channel_number, "is SUSY (aka is MG):", issusy
    # for pre-MC16 samples use the old way
    else:
        import os
        if not os.access('/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/Blocks.list',os.R_OK):
          # Do it the old-fashioned way
          # https://svnweb.cern.ch/trac/atlasoff/browser/Generators/MC15JobOptions/trunk/share/Blocks.list
          issusy = (370000 <= mc_channel_number < 405000) or (406000 <= mc_channel_number < 410000) \
                   or (436000 <= mc_channel_number < 439000) or (448000 <= mc_channel_number < 450000)
        else:
          # Automatic detection based on cvmfs
          issusy = False
          blocks = open('/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/Blocks.list','r')
          for l in blocks.readlines():
            if not 'SUSY' in l: continue
            myrange = l.split()[0].replace('DSID','').replace('xxx','000',1).replace('xxx','999',1)
            low = int(myrange.split('-')[0])
            high = int(myrange.split('-')[1]) if '-' in myrange else int(myrange.replace('000','999'))
            if low <= mc_channel_number and mc_channel_number <= high:
              issusy=True
              break
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

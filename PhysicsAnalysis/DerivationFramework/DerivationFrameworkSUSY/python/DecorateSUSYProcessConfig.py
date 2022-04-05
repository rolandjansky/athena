# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#==============================================================================
# SUSY signal process augmentation
# Component accumulator version
#==============================================================================

def IsSUSYSignalRun3(flags):
    """Identify SUSY signal sample"""
  
    if not flags.Input.isMC:
        return False
   
    mc_channel_number = int(flags.Input.RunNumber[0])
    # with MC16, there are no dedicated SUSY DSID blocks anymore but blocks for
    # each generator: see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PmgMcSoftware#DSID_blocks
    if mc_channel_number >= 500000:
        # there does not seem to be an elegant way to check whether its a SUSY sample
        # or not, thus run the mark all MadGraph samples as SUSY for the moment
        # to non-SUSY MG samples this adds only an empty decoration and does not break anything
        issusy = (mc_channel_number < 600000)
        print ("DecorateSUSYProcess: fileinfo.mc_channel_number", mc_channel_number, "is SUSY (aka is MG):", issusy)
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
                if 'SUSY' not in l: continue
                myrange = l.split()[0].replace('DSID','').replace('xxx','000',1).replace('xxx','999',1)
                low = int(myrange.split('-')[0])
                high = int(myrange.split('-')[1]) if '-' in myrange else int(myrange.replace('000','999'))
                if low <= mc_channel_number and mc_channel_number <= high:
                    issusy=True
                    break
        print ("DecorateSUSYProcess: fileinfo.mc_channel_number", mc_channel_number, "is SUSY:", issusy)
    return issusy

# Configure SUSY signal tagger
def SUSYSignalTaggerCfg(flags, derivationname):
    """Configure SUSY signal tagger"""
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    acc = ComponentAccumulator()
    acc.addPublicTool(CompFactory.DerivationFramework.SUSYSignalTagger(name             = derivationname + "SignalTagger",
                                                                       EventInfoName    = "EventInfo",
                                                                       MCCollectionName = "TruthParticles"),
                      primary = True)
    return acc

def DecorateSUSYProcessCfg(flags,derivationname):
    """Decorate SUSY processes"""
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()   
    if IsSUSYSignalRun3(flags):
        SUSYSignalTagger = acc.getPrimaryAndMerge(SUSYSignalTaggerCfg(flags, derivationname))
        return [SUSYSignalTagger]
    return []

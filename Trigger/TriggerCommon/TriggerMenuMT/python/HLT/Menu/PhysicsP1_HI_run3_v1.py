# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# PhysicsP1_HI_run3_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],
from TriggerMenuMT.HLT.Config.Utility.ChainDefInMenu import ChainProp
from .SignatureDicts import ChainStore
from .Physics_pp_run3_v1 import (
        SingleMuonGroup,
        MinBiasGroup,
        PrimaryL1MuGroup,
        SupportGroup
)
from .PhysicsP1_pp_run3_v1 import addP1Signatures

HardProbesStream="HardProbes"
MinBiasStream="MinBias"
UPCStream="UPC"
### following stream tags not used yet, need to be implemented in StreamInfo.py before use
#UCCStream="UCC"
#PCStream="PC"
#CCStream="CC"
#PCpebStream="PCPEB"
#CCpebStream="CCPEB"

def setupMenu():

    from AthenaCommon.Logging import logging
    log = logging.getLogger( __name__ )
    log.info('setupMenu ...')

    chains = ChainStore()

    chains['Muon'] = [
            ChainProp(name='HLT_mu4_L1MU3V', stream=[HardProbesStream], groups=SingleMuonGroup+PrimaryL1MuGroup),
    ]


    chains['Streaming'] = [
        ChainProp(name='HLT_noalg_L1RD0_EMPTY',  l1SeedThresholds=['FSNOSEED'], stream=[MinBiasStream], groups=MinBiasGroup+['PS:Online']+SupportGroup),
        ChainProp(name='HLT_noalg_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=[MinBiasStream], groups=MinBiasGroup+['PS:Online']+SupportGroup), 

        ChainProp(name='HLT_noalg_L1ZDC_A', l1SeedThresholds=['FSNOSEED'], stream=[MinBiasStream], groups=MinBiasGroup+['PS:Online']+SupportGroup),
        ChainProp(name='HLT_noalg_L1ZDC_C', l1SeedThresholds=['FSNOSEED'], stream=[MinBiasStream], groups=MinBiasGroup+['PS:Online']+SupportGroup),
        ChainProp(name='HLT_noalg_L1ZDC_AND', l1SeedThresholds=['FSNOSEED'], stream=[MinBiasStream], groups=MinBiasGroup+['PS:Online']+SupportGroup),
    ]

    chains['Monitor'] = [
          ChainProp(name='HLT_noalg_CostMonDS_L1All',        l1SeedThresholds=['FSNOSEED'], stream=['CostMonitoring'], groups=['RATE:Monitoring','BW:Other']),
    ]
    tempChains = ChainStore()
    addP1Signatures(tempChains)
    for sig, chainsInSig in tempChains.items():
        if sig == "Beamspot":  # HI needs special beam spot setup
                continue
        for c in chainsInSig:
                if "IDCalibPEB" in c.name: # heavy tracking
                        continue
                else:
                        chains[sig].append(c)
    return chains

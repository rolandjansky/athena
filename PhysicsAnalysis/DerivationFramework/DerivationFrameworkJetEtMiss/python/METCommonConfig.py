# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# METCommonConfig.py
# Configures DF MET content building and association tools
# Component accumulator version  
#********************************************************************



def METCommonCfg(ConfigFlags):
    """Configure MET for the derivation framework"""

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from METReconstruction.METAssociatorCfg import METAssociatorCfg

    acc = ComponentAccumulator()

    metDefs = ['AntiKt4EMTopo']
    if ConfigFlags.MET.DoPFlow:
        metDefs.append('AntiKt4EMPFlow')
        
    for metDef in metDefs:
        acc.merge(METAssociatorCfg(ConfigFlags, metDef))

    return acc



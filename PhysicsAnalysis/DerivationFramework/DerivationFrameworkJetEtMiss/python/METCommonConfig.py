# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# METCommonConfig.py
# Configures DF MET content building and association tools
# Component accumulator version  
#********************************************************************

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def METAssociationAlgCfg(ConfigFlags, algname = 'METStandardAssociationAlg'):
    """Configure MET for the derivation framework"""
    
    acc = ComponentAccumulator()
    from METReconstruction.METAssocCfg import getMETAssocAlg

    from METReconstruction.METRecoFlags import metFlags
    standardConfigs = {k : v for k, v in metFlags.METAssocConfigs().items() if ("EMTopo" in k or "EMPFlow" in k)}    

    assocAlg = getMETAssocAlg(algname, standardConfigs)
    acc.addEventAlgo(assocAlg)
    return acc



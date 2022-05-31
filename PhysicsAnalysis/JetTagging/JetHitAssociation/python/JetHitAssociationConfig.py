# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# For using GeV units
import AthenaCommon.SystemOfUnits as Units

# Needed to get name of split probability container
from InDetConfig.TrackRecoConfig import ClusterSplitProbabilityContainerName

# We need to add this algorithm to get the TrackMeasurementValidationContainer
from InDetConfig.InDetPrepRawDataToxAODConfig import InDetPixelPrepDataToxAODCfg, InDetSCT_PrepDataToxAODCfg

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def JetHitAssociationCfg(flags, name="JetHitAssociation", **kwargs):
    acc = ComponentAccumulator()
        
    acc.merge(
        InDetPixelPrepDataToxAODCfg(
            flags,
            ClusterSplitProbabilityName = ClusterSplitProbabilityContainerName(flags)
        )
    )
    
    acc.merge(
        InDetSCT_PrepDataToxAODCfg(
            flags
        )
    )

    acc.addEventAlgo(
        CompFactory.JetHitAssociation(
                "JetHitAssociation",
                jetCollectionName = "AntiKt4EMPFlowJets",
                jetPtThreshold = 300 * Units.GeV,
                dRmatchHitToJet = 0.4
        )
    )
    
    return acc

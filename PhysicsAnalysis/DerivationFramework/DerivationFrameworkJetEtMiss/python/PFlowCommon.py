# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DerivationFrameworkCore.DerivationFrameworkMaster import *

def applyPFOAugmentation(sequence=DerivationFrameworkJob):

    # simple set up -- either the alg exists and contains the tool, in which case we exit
    if hasattr(sequence,"PFlowAugmentation"):
        print "PFlowCommon: PFlowAugmentation already scheduled on sequence", sequence.name
        return
    else:
        # otherwise schedule it
        pfaug = CfgMgr.DerivationFramework__CommonAugmentation("PFlowAugmentation")
        sequence += pfaug

        pfoaugtool = None
        from AthenaCommon.AppMgr import ToolSvc        
        # create and add the tool to the alg if needed
        if hasattr(ToolSvc,"PFlowAugmentationTool"):
            pfoaugtool = getattr(ToolSvc,"PFlowAugmentationTool")
        else:
            weightpfotool = CfgMgr.CP__WeightPFOTool("PFAugmentationWeightTool",DoEoverPWeight=False)
            ToolSvc += weightpfotool
            pfoaugtool = CfgMgr.DerivationFramework__PFlowAugmentationTool("PFlowAugmentationTool",
                                                                           WeightPFOTool=weightpfotool)
            ToolSvc += pfoaugtool
        if not pfoaugtool in pfaug.AugmentationTools:
            pfaug.AugmentationTools.append(pfoaugtool)        

##################################################################

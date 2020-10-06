# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from eflowRec.eflowRecFlags import jobproperties

if jobproperties.eflowRecFlags.usePFTauFlowElementAssoc and jobproperties.eflowRecFlags.useFlowElements :
    from eflowRec.eflowRecConf import PFTauFlowElementAssoc
    PFTauFlowElementAssoc=PFTauFlowElementAssoc("PFTauFlowElementAssoc")
    topSequence += PFTauFlowElementAssoc

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration



class CFNaming(object):
    """Static Class to collect all string manipulations on CF object names """
    
    RECO_POSTFIX = "_reco"
    VIEW_POSTFIX = "_view"
    FILTER_POSTFIX = "_filter"
    
    @staticmethod
    def stepName(nstep):
        return "Step%i"%(nstep+1)

    @staticmethod
    def menuSequenceName(Hypo):
        return "S" + Hypo

    @staticmethod
    def filterName(ChainStepName):
        return "F" + ChainStepName

    @staticmethod
    def simplifyOutName( name ):
        """Removes all HLT_decisions except the front one"""        
        return "HLTNAV_" + name.replace("HLTNAV_", "").replace("Trig", "").replace("Alg", "")

    @staticmethod
    def filterOutName(filter_name, filterIn):
        return CFNaming.simplifyOutName("HLTNAV_" + filter_name + "__" + filterIn)

    @staticmethod
    def inputMakerOutName(IMname, filterOut):
        return CFNaming.simplifyOutName("HLTNAV_" + IMname + "__" + filterOut)

    @staticmethod
    def hypoAlgOutName(HypoName, HypoInput):
        return CFNaming.simplifyOutName("HLTNAV_" + HypoName + "__" + HypoInput)

    @staticmethod
    def comboHypoName(HypoName):
        return "ComboHypo_" + HypoName

    @staticmethod
    def comboSequenceCopyName(SequenceName, ncopy, StepName):    
        return "%s%d_for_%s"%(SequenceName, ncopy, StepName)

    @staticmethod
    def comboHypoCopyName(HypoName,ncopy,StepName):
        return "%s%d_for_%s"%(HypoName,ncopy,StepName)

    @staticmethod
    def comboHypoOutputName(inputName):
        return CFNaming.simplifyOutName("HLTNAV_combo_" + inputName )

    @staticmethod
    def stepRecoNodeName(HLTNodeName, StepCFName):
        return HLTNodeName + "_" + StepCFName

    @staticmethod
    def stepSummaryName(StepCFName):
        return "TriggerSummary" + StepCFName

    @staticmethod
    def stepRecoName(stepName):
        return stepName + CFNaming.RECO_POSTFIX

    @staticmethod
    def stepViewName(stepName):
        return stepName + CFNaming.VIEW_POSTFIX

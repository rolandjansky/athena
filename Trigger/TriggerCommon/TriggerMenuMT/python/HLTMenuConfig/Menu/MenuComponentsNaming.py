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
    def simplifyOutName(name):
        """Removes all HLT_decisions except the front one + additiona cleaning of common words"""
        return "HLTNav_" + name.replace("HLTNav_", "").replace("Trig", "").replace("Alg", "")

    @staticmethod
    def filterOutName(filter_name, filterIn):
        return CFNaming.simplifyOutName("HLTNav_" + filter_name + "__" + filterIn)

    @staticmethod
    def inputMakerOutName(IMname):
        return CFNaming.simplifyOutName("HLTNav_" + IMname)

    @staticmethod
    def hypoAlgOutNameOld(HypoName, HypoInput):
        name = CFNaming.simplifyOutName("HLTNav_" + HypoName + "__" + HypoInput)
        # remove the IM parts from hypos output
        return "__".join( filter(lambda frag: not (frag.startswith("IM") or frag.startswith("F")), name.split("__")) )

    @staticmethod
    def hypoAlgOutName(HypoName):
        name = CFNaming.simplifyOutName("HLTNav_" + HypoName)
        return name


    @staticmethod
    def comboHypoName(HypoName):
        return "ComboHypo_" + HypoName

    @staticmethod
    def comboSequenceCopyName(SequenceName, ncopy, StepName):
        if type(SequenceName) is list:
           sequence = []
           for sq in SequenceName:
              sequence.append("%s%d_for_%s"%(sq, ncopy, StepName))
           return sequence
        else:
           return "%s%d_for_%s"%(SequenceName, ncopy, StepName)

    @staticmethod
    def comboHypoCopyName(HypoName,ncopy,StepName):
        return "%s%d_for_%s"%(HypoName,ncopy,StepName)

    @staticmethod
    def comboHypoOutputName(comboName, inputName):
        return CFNaming.simplifyOutName("HLTNav_" + comboName+"_"+ inputName )

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

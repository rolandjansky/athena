# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration



class CFNaming ():
    """Class to collect all string manipulations on CF object names """
    
    RECO_POSTFIX = "_reco"
    VIEW_POSTFIX = "_view"
    FILTER_POSTFIX = "_filter"

    
    @staticmethod
    def reduceName(line):
        splitLine = filter( lambda x: not ( x == "from" or x == "Filter"), line.split("_") ) # eliminate common words
        splitLine =[ k[0] for k in filter( lambda x: not ( "Step" in x[0] or "Step" in x[1]), zip(splitLine, [""]+splitLine) )] # eliminate the word contaning Step and the one after 
        return "_".join(splitLine)

    @staticmethod
    def stepName(nstep):
        return "Step%i"%(nstep+1)

    @staticmethod
    def menuSequenceName(Hypo):
        return ("S_" + Hypo)

    @staticmethod
    def filterName(ChainStepName):
        return ("Filter_" + ChainStepName)

    @staticmethod
    def filterOutName(filter_name, filterIn):
        return (filter_name + "_from_" + filterIn)

    @staticmethod
    def inputMakerOutName(IMname, filterOut):
        return "%s_from_%s"%(IMname, filterOut)
    #    return  (IMname + "_" + reduceName(filterOut))

    @staticmethod
    def hypoAlgOutName(HypoName, HypoInput):
        return  (HypoName + "_" + CFNaming.reduceName(HypoInput))

    @staticmethod
    def comboHypoName(HypoName):
        return ("ComboHypo_" + HypoName)

    @staticmethod
    def comboSequenceCopyName(SequenceName, ncopy, StepName):    
        return "%s%d_for_%s"%(SequenceName, ncopy, StepName)

    @staticmethod
    def comboHypoCopyName(HypoName,ncopy,StepName):
        return "%s%d_for_%s"%(HypoName,ncopy,StepName)

    @staticmethod
    def stepRecoNodeName(HLTNodeName, stepCFname):
        return  (HLTNodeName + "_" + stepCFname)

    @staticmethod
    def stepSummaryName(stepCFname):
        return ("TriggerSummary"+ stepCFname)

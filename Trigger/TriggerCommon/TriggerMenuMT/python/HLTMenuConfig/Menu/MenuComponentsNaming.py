RECO_POSTFIX = "_reco"
VIEW_POSTFIX = "_view"
FILTER_POSTFIX = "_filter"


def ReduceName(line):
    splitLine = filter( lambda x: not ( x == "from" or x == "Filter"), line.split("_") ) # eliminate common words
    splitLine =[ k[0] for k in filter( lambda x: not ( "Step" in x[0] or "Step" in x[1]), zip(splitLine, [""]+splitLine) )] # eliminate the word contaning Step and the one after 
    return "_".join(splitLine)


def StepName(nstep):
    return "Step%i"%(nstep+1)

def MenuSequenceName(Hypo):
    return ("S_" + Hypo)

def FilterName(ChainStepName):
    return ("Filter_" + ChainStepName)

def FilterOutName(filter_name, filterIn):
    return (filter_name + "_from_" + filterIn)

def InputMakerOutName(IMname, filterOut):
    return "%s_from_%s"%(IMname, filterOut)
#    return  (IMname + "_" + ReduceName(filterOut))

def HypoAlgOutName(HypoName, HypoInput):
    return  (HypoName + "_" + ReduceName(HypoInput))

def ComboHypoName(HypoName):
    return ("ComboHypo_" + HypoName)

def ComboSequenceCopyName(SequenceName, ncopy, StepName):    
    return "%s%d_for_%s"%(SequenceName, ncopy, StepName)

def ComboHypoCopyName(HypoName,ncopy,StepName):
    return "%s%d_for_%s"%(HypoName,ncopy,StepName)

def StepRecoNodeName(HLTNodeName, stepCF_name):
    return  (HLTNodeName + "_" + stepCF_name)

def StepSummaryName(stepCF_name):
    return ("TriggerSummary"+ stepCF_name)

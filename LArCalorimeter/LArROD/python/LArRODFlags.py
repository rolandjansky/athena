# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class readDigits(JobProperty):
    ### force reading digits from BS and starting reconstruction from them
    ###
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class readRawChannels(JobProperty):
    ### 
    ###
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doDSP(JobProperty):
    ### reconstructing digits with strict DSP emulation and compare to DSP results
    ###
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doBuildBadChannel(JobProperty):
    ### build bad channel in digits->raw
    ###
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doLArFebErrorSummary(JobProperty):
    ### build LArFebErrorSummary from LArFebHeader 
    ###
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class forceIter(JobProperty):
    ### force use of OFC iteration as in data
    ###
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class keepDSPRaw(JobProperty):
     ### Keep Raw energies from DSP in LArRawChannelContainer 
     ### and copy energies from ADC reco into different container
     statusOn=True
     allowedTypes=['bool']
     StoredValue=True

class RawChannelFromDigitsContainerName(JobProperty):
     ### Name of the raw channel container make from digits if keepDSPRaw is true
     ###
     statusOn=True
     allowedTypes=['str']
     StoredValue='LArRawChannels_FromDigits'     

class doOFCPileupOptimization(JobProperty):
     ### flag to use pileup noise or not in autocorrelation for OFC computation
     ###
     statusOn=True
     allowedTypes=['bool']
     StoredValue=False

class NumberOfCollisions(JobProperty):
     ### Number of collisions per bunch crossing, used to compute pileup autocorrelation
     ###
     statusOn=True
     allowedTypes=['int']
     StoredValue=0

class UseDelta(JobProperty):
     ### option to use average constraint in OFC computation
     ### 0 = not use Delta, 1 = only EMECIW/HEC/FCAL, 2 = all , 3 = only EMECIW/HEC/FCAL1+high eta FCAL2-3
     statusOn=True
     allowedTypes=['int']
     StoredValue=0 

class doOFCAverageConstraint(JobProperty):
     ### flag to use average constraint in OFC computation
     ###     
     statusOn=True
     allowedTypes=['bool']
     StoredValue=False

class doOFCAverageConstraintHighEta(JobProperty):
     ### flag to use average constraint in OFC computation for eta>2.5 only 
     ### (EMEC-IW + HEC + FCAL)
     ###
     statusOn=True
     allowedTypes=['bool']
     StoredValue=False

class doOFCMixedOptimization(JobProperty):
    ### flag not to use pileup noise neither average constrain in EMB and EMEC-OW,
    ### and both pileup noise and average constrain everywhere else
    ### 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class nSamples(JobProperty):
    ### flag to define number of samples to use in LAr digitization + ROD emulation
    statusOn=True
    allowedTypes=['int']
    StoredValue=5

class firstSample(JobProperty):
    ### flag to define location of first samples in the full 32 samples pulse shape in the LAr digitization + ROD emulation
    statusOn=True
    allowedTypes=['int']
    StoredValue=0

class useHighestGainAutoCorr(JobProperty):
    ### flag to force using the highest gain autocorelation function when doing OFC optimization
    statusOn=True
    allowTypes=['bool']
    StoredValue=False
    
class LArRODFlags(JobPropertyContainer):
    """ LAr signal reconstruction information """


#########################################
# fill in the container, add it to global 
#########################################           

jobproperties.add_Container(LArRODFlags)

_list_LArRODFlags=[readDigits,readRawChannels,doDSP,doBuildBadChannel,doLArFebErrorSummary,forceIter,keepDSPRaw,RawChannelFromDigitsContainerName,NumberOfCollisions,UseDelta,doOFCPileupOptimization,doOFCAverageConstraint,doOFCAverageConstraintHighEta,doOFCMixedOptimization,nSamples,firstSample,useHighestGainAutoCorr]


for j in _list_LArRODFlags:
    jobproperties.LArRODFlags.add_JobProperty(j)

del _list_LArRODFlags

larRODFlags = jobproperties.LArRODFlags

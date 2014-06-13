# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class DerivationFrameworkProdFlags(JobPropertyContainer):
    """ The derivation framework flag/job property  container."""
jobproperties.add_Container(DerivationFrameworkProdFlags)

derivationFlags = jobproperties.DerivationFrameworkProdFlags
listAODtoDPD=[]
listAllKnownDPD=[]  

####################################
# Defined by ASG for tests/examples
####################################

class WriteDAOD_TEST1Stream(JobProperty):
    """ Produce the DPD for DAOD_TEST1 - derivation framework test """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_TEST1"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkExamples/SkimmingExample.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TEST1Stream)
listAODtoDPD.append(WriteDAOD_TEST1Stream.StreamName)

class WriteDAOD_TEST2Stream(JobProperty):
    """ Produce the DPD for DAOD_TEST2 - derivation framework test """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_TEST2"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkExamples/SkimmingExampleStrings.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TEST2Stream)
listAODtoDPD.append(WriteDAOD_TEST2Stream.StreamName)

class WriteDAOD_TEST3Stream(JobProperty):
    """ Produce the DPD for DAOD_TEST3 - derivation framework test """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_TEST3"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkExamples/StringsToolsExample.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TEST3Stream)
listAODtoDPD.append(WriteDAOD_TEST3Stream.StreamName)

class WriteDAOD_TEST4Stream(JobProperty):
    """ Produce the DPD for DAOD_TEST4 - derivation framework test """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_TEST4"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkExamples/SlimmingExample.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TEST4Stream)
listAODtoDPD.append(WriteDAOD_TEST4Stream.StreamName)

class WriteDAOD_TEST5Stream(JobProperty):
    """ Produce the DPD for DAOD_TEST5 - derivation framework test """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_TEST5"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkExamples/CPToolsExample.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TEST5Stream)
listAODtoDPD.append(WriteDAOD_TEST5Stream.StreamName)

##################################
# Defined by the Top group
##################################

class WriteDAOD_TOP1Stream(JobProperty):
    """TOP1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_TOP1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkTop/TOP1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TOP1Stream)
listAODtoDPD.append(WriteDAOD_TOP1Stream.StreamName)

##################################
# Defined by Higgs subgroup 3
##################################

class WriteDAOD_HIGGS3D1Stream(JobProperty):
    """HIGGS3D1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGGS3D1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGGS3D1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGGS3D1Stream)
listAODtoDPD.append(WriteDAOD_HIGGS3D1Stream.StreamName)

##################################
# Defined by Higgs subgroup 4
##################################

class WriteDAOD_HIGGS4D2Stream(JobProperty):
    """HIGGS4D2 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGGS4D2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGGS4D2.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGGS4D2Stream)
listAODtoDPD.append(WriteDAOD_HIGGS4D2Stream.StreamName)

##################################
# Defined by Higgs subgroup 5
##################################

class WriteDAOD_HIGGS5D1Stream(JobProperty):
    """HIGGS5D1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGGS5D1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGGS5D1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGGS5D1Stream)
listAODtoDPD.append(WriteDAOD_HIGGS5D1Stream.StreamName)


##################################
# Defined by Higgs subgroup 8
##################################

class WriteDAOD_HIGGS8D1Stream(JobProperty):
    """HIGGS8D1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGGS8D1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGGS8D1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGGS8D1Stream)
listAODtoDPD.append(WriteDAOD_HIGGS8D1Stream.StreamName)

class WriteDAOD_HIGGS8D2Stream(JobProperty):
    """HIGGS8D1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGGS8D2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGGS8D2.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGGS8D2Stream)
listAODtoDPD.append(WriteDAOD_HIGGS8D2Stream.StreamName)

#################################
# Defined by the SM group
#################################

class WriteDNTUP_SM1 (JobProperty):
    """SM jet slim"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDNTUP_SM1'
    FileName = ''
    isVirtual = False
    NTUPScript = "DerivationFrameworkSM/SM1.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']

class WriteDNTUP_SM2 (JobProperty):
    """SM photon skim/slim"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDNTUP_SM2'
    FileName = ''
    isVirtual = False
    NTUPScript = "DerivationFrameworkSM/SM2.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']

class WriteDNTUP_SM3 (JobProperty):
    """SM EW and W/Z dilepton"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDNTUP_SM3'
    FileName = ''
    isVirtual = False
    NTUPScript = "DerivationFrameworkSM/SM3.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']

class WriteDNTUP_SM4 (JobProperty):
    """SM W single lepton"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDNTUP_SM4'
    FileName = ''
    isVirtual = False
    NTUPScript = "DerivationFrameworkSM/SM4.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']

class WriteDNTUP_SM5 (JobProperty):
    """SM tri lepton"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDNTUP_SM5'
    FileName = ''
    isVirtual = False
    NTUPScript = "DerivationFrameworkSM/SM5.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']


#################################
# Defined by the Tau group
#################################

class WriteDAOD_TAU1Stream(JobProperty):
    """TAU1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_TAU1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkTau/TAU1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TAU1Stream)
listAODtoDPD.append(WriteDAOD_TAU1Stream.StreamName)

class WriteDAOD_TAU3Stream(JobProperty):
    """TAU3 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_TAU3'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkTau/TAU3.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TAU3Stream)
listAODtoDPD.append(WriteDAOD_TAU3Stream.StreamName)


#################################
# Defined by the SUSY group
#################################

class WriteDAOD_SUSY1Stream(JobProperty):
    """SUSY1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY1Stream)
listAODtoDPD.append(WriteDAOD_SUSY1Stream.StreamName)

#################################
# Defined by the Exotics group
#################################

class WriteDNTUP_EXOTICS0 (JobProperty):
    """EXOTICS0 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDNTUP_EXOTICS0'
    FileName = ''
    isVirtual = False
    NTUPScript = "DerivationFrameworkExotics/EXOTICS0.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']

class WriteDNTUP_EXOTICS1 (JobProperty):
    """EXOTICS1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDNTUP_EXOTICS1'
    FileName = ''
    isVirtual = False
    NTUPScript = "DerivationFrameworkExotics/EXOTICS1.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']

class WriteDNTUP_EXOTICS2 (JobProperty):
    """EXOTICS2 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDNTUP_EXOTICS2'
    FileName = ''
    isVirtual = False
    NTUPScript = "DerivationFrameworkExotics/EXOTICS2.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']

class WriteDNTUP_EXOTICS3 (JobProperty):
    """EXOTICS3 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDNTUP_EXOTICS3'
    FileName = ''
    isVirtual = False
    NTUPScript = "DerivationFrameworkExotics/EXOTICS3.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']

class WriteDNTUP_EXOTICS5 (JobProperty):
    """EXOTICS5 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDNTUP_EXOTICS5'
    FileName = ''
    isVirtual = False
    NTUPScript = "DerivationFrameworkExotics/EXOTICS5.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']

class WriteDNTUP_EXOTICS6 (JobProperty):
    """EXOTICS6 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDNTUP_EXOTICS6'
    FileName = ''
    isVirtual = False
    NTUPScript = "DerivationFrameworkExotics/EXOTICS6.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']

class WriteDNTUP_EXOTICS7 (JobProperty):
    """EXOTICS7 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDNTUP_EXOTICS7'
    FileName = ''
    isVirtual = False
    NTUPScript = "DerivationFrameworkExotics/EXOTICS7.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']

class WriteDNTUP_EXOTICS8 (JobProperty):
    """EXOTICS8 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDNTUP_EXOTICS8'
    FileName = ''
    isVirtual = False
    NTUPScript = "DerivationFrameworkExotics/EXOTICS8.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']

class WriteDNTUP_EXOTICS9 (JobProperty):
    """EXOTICS9 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDNTUP_EXOTICS9'
    FileName = ''
    isVirtual = False
    NTUPScript = "DerivationFrameworkExotics/EXOTICS9.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']

#################################
# Defined by the Jet/MET group
#################################

class WriteDAOD_JETMET1Stream(JobProperty):
    """JETMET1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETMET1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETMET1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETMET1Stream)
listAODtoDPD.append(WriteDAOD_JETMET1Stream.StreamName)

class WriteDAOD_JETMET2Stream(JobProperty):
    """JETMET2 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETMET2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETMET2.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETMET2Stream)
listAODtoDPD.append(WriteDAOD_JETMET2Stream.StreamName)

class WriteDAOD_JETMET3Stream(JobProperty):
    """JETMET3 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETMET3'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETMET3.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETMET3Stream)
listAODtoDPD.append(WriteDAOD_JETMET3Stream.StreamName)

class WriteDAOD_JETMET4Stream(JobProperty):
    """JETMET4 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETMET4'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETMET4.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETMET4Stream)
listAODtoDPD.append(WriteDAOD_JETMET4Stream.StreamName)

class WriteDAOD_JETMET5Stream(JobProperty):
    """JETMET5 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETMET5'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETMET5.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETMET5Stream)
listAODtoDPD.append(WriteDAOD_JETMET5Stream.StreamName)

from RecExConfig.RecoFunctions import AddValidItemToList
AddValidItemToList(listAODtoDPD,listAllKnownDPD)



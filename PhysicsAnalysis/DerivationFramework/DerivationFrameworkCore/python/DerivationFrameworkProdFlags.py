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

class WriteDAOD_TEST6Stream(JobProperty):
    """ Produce the DPD for DAOD_TEST6 - derivation framework test """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_TEST6"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkExamples/PreSelectionExample.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TEST6Stream)
listAODtoDPD.append(WriteDAOD_TEST6Stream.StreamName)

class WriteDAOD_TEST7Stream(JobProperty):
    """ Produce the DPD for DAOD_TEST7 - derivation framework test """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_TEST7"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkExamples/AugmentationExample.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TEST7Stream)
listAODtoDPD.append(WriteDAOD_TEST7Stream.StreamName)

class WriteDAOD_TEST8Stream(JobProperty):
    """ Produce the DPD for DAOD_TEST8 - derivation framework test """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_TEST8"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkExamples/ThinningExample.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TEST8Stream)
listAODtoDPD.append(WriteDAOD_TEST8Stream.StreamName)

class WriteDAOD_TEST9Stream(JobProperty):
    """ Produce the DPD for DAOD_TEST9 - derivation framework test """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_TEST9"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkExamples/RecoExample.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TEST9Stream)
listAODtoDPD.append(WriteDAOD_TEST9Stream.StreamName)

class WriteDAOD_MCTEST1Stream(JobProperty):
    """ Produce the DPD for DAOD_MCTEST1 - derivation framework test """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_MCTEST1"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkMCTruth/MCTEST1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_MCTEST1Stream)
listAODtoDPD.append(WriteDAOD_MCTEST1Stream.StreamName)

class WriteDAOD_MCTEST2Stream(JobProperty):
    """ Produce the DPD for DAOD_MCTEST2 - derivation framework test """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_MCTEST2"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkMCTruth/MCTEST2.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_MCTEST2Stream)
listAODtoDPD.append(WriteDAOD_MCTEST2Stream.StreamName)

class WriteDAOD_MCTEST3Stream(JobProperty):
    """ Produce the DPD for DAOD_MCTEST3 - derivation framework test """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_MCTEST3"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkMCTruth/MCTEST3.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_MCTEST3Stream)
listAODtoDPD.append(WriteDAOD_MCTEST3Stream.StreamName)


##################################
# Defined by the Top group
##################################

class WriteDAOD_TOPQ1Stream(JobProperty):
    """TOPQ1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_TOPQ1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkTop/TOPQ1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TOPQ1Stream)
listAODtoDPD.append(WriteDAOD_TOPQ1Stream.StreamName)

class WriteDAOD_TOPQ2Stream(JobProperty):
    """TOPQ2 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_TOPQ2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkTop/TOPQ2.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TOPQ2Stream)
listAODtoDPD.append(WriteDAOD_TOPQ2Stream.StreamName)

##################################
# Defined by Higgs subgroup 1
##################################

class WriteDAOD_HIGG1D1Stream(JobProperty):
    """HIGG1D1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG1D1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG1D1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG1D1Stream)
listAODtoDPD.append(WriteDAOD_HIGG1D1Stream.StreamName)

##################################
# Defined by Higgs subgroup 2
##################################

class WriteDAOD_HIGG2D1Stream(JobProperty):
    """HIGG2D1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG2D1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG2D1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG2D1Stream)
listAODtoDPD.append(WriteDAOD_HIGG2D1Stream.StreamName)

class WriteDAOD_HIGG2D2Stream(JobProperty):
    """HIGG2D2 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG2D2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG2D2.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG2D2Stream)
listAODtoDPD.append(WriteDAOD_HIGG2D2Stream.StreamName)

class WriteDAOD_HIGG2D3Stream(JobProperty):
    """HIGG2D3 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG2D3'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG2D3.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG2D3Stream)
listAODtoDPD.append(WriteDAOD_HIGG2D3Stream.StreamName)

class WriteDAOD_HIGG2D4Stream(JobProperty):
    """HIGG2D4 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG2D4'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG2D4.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG2D4Stream)
listAODtoDPD.append(WriteDAOD_HIGG2D4Stream.StreamName)

class WriteDAOD_HIGG2D5Stream(JobProperty):
    """HIGG2D5 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG2D5'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG2D5.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG2D5Stream)
listAODtoDPD.append(WriteDAOD_HIGG2D5Stream.StreamName)


##################################
# Defined by Higgs subgroup 3
##################################

class WriteDAOD_HIGG3D1Stream(JobProperty):
    """HIGG3D1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG3D1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG3D1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG3D1Stream)
listAODtoDPD.append(WriteDAOD_HIGG3D1Stream.StreamName)

##################################
# Defined by Higgs subgroup 4
##################################

class WriteDAOD_HIGG4D1Stream(JobProperty):
    """HIGG4D1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG4D1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG4D1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG4D1Stream)
listAODtoDPD.append(WriteDAOD_HIGG4D1Stream.StreamName)

class WriteDAOD_HIGG4D2Stream(JobProperty):
    """HIGG4D2 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG4D2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG4D2.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG4D2Stream)
listAODtoDPD.append(WriteDAOD_HIGG4D2Stream.StreamName)

##################################
# Defined by Higgs subgroup 5
##################################

class WriteDAOD_HIGG5D1Stream(JobProperty):
    """HIGG5D1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG5D1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG5D1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG5D1Stream)
listAODtoDPD.append(WriteDAOD_HIGG5D1Stream.StreamName)

class WriteDAOD_HIGG5D2Stream(JobProperty):
   """HIGG5D2 derivation"""
   statusOn = True
   allowedTypes = ['bool']
   StoredValue = False
   StreamName = 'StreamDAOD_HIGG5D2'
   FileName = ''
   isVirtual = False
   DPDMakerScript = "DerivationFrameworkHiggs/HIGG5D2.py"
   pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG5D2Stream)
listAODtoDPD.append(WriteDAOD_HIGG5D2Stream.StreamName)

##################################
# Defined by Higgs subgroup 6
##################################

class WriteDAOD_HIGG6D1Stream(JobProperty):
    """HIGG6D1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG6D1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG6D1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG6D1Stream)
listAODtoDPD.append(WriteDAOD_HIGG6D1Stream.StreamName)

class WriteDAOD_HIGG6D2Stream(JobProperty):
   """HIGG6D2 derivation"""
   statusOn = True
   allowedTypes = ['bool']
   StoredValue = False
   StreamName = 'StreamDAOD_HIGG6D2'
   FileName = ''
   isVirtual = False
   DPDMakerScript = "DerivationFrameworkHiggs/HIGG6D2.py"
   pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG6D2Stream)
listAODtoDPD.append(WriteDAOD_HIGG6D2Stream.StreamName)


##################################
# Defined by Higgs subgroup 8
##################################

class WriteDAOD_HIGG8D1Stream(JobProperty):
    """HIGG8D1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG8D1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG8D1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG8D1Stream)
listAODtoDPD.append(WriteDAOD_HIGG8D1Stream.StreamName)

class WriteDAOD_HIGG8D2Stream(JobProperty):
    """HIGG8D1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG8D2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG8D2.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG8D2Stream)
listAODtoDPD.append(WriteDAOD_HIGG8D2Stream.StreamName)

#################################
# Defined by the STDM group
#################################

class WriteDAOD_STDM1Stream (JobProperty):
    """STDM jet slim"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_STDM1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSM/STDM1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_STDM1Stream)
listAODtoDPD.append(WriteDAOD_STDM1Stream.StreamName)

class WriteDAOD_STDM2Stream (JobProperty):
    """STDM photon skim/slim"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_STDM2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSM/STDM2.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_STDM2Stream)
listAODtoDPD.append(WriteDAOD_STDM2Stream.StreamName)

class WriteDAOD_STDM3Stream (JobProperty):
    """STDM EW and W/Z dilepton"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_STDM3'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSM/STDM3.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_STDM3Stream)
listAODtoDPD.append(WriteDAOD_STDM3Stream.StreamName)

class WriteDAOD_STDM4Stream (JobProperty):
    """STDM W single lepton"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_STDM4'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSM/STDM4.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_STDM4Stream)
listAODtoDPD.append(WriteDAOD_STDM4Stream.StreamName)

class WriteDAOD_STDM5Stream (JobProperty):
    """STDM tri lepton"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_STDM5'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSM/STDM5.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_STDM5Stream)
listAODtoDPD.append(WriteDAOD_STDM5Stream.StreamName)

#################################
# Defined by the Tau group
#################################

class WriteDAOD_TAUP1Stream(JobProperty):
    """TAUP1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_TAUP1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkTau/TAUP1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TAUP1Stream)
listAODtoDPD.append(WriteDAOD_TAUP1Stream.StreamName)

class WriteDAOD_TAUP3Stream(JobProperty):
    """TAUP3 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_TAUP3'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkTau/TAUP3.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TAUP3Stream)
listAODtoDPD.append(WriteDAOD_TAUP3Stream.StreamName)


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

class WriteDAOD_SUSY2Stream(JobProperty):
    """SUSY2 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY2.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY2Stream)
listAODtoDPD.append(WriteDAOD_SUSY2Stream.StreamName)

class WriteDAOD_SUSY3Stream(JobProperty):
    """SUSY3 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY3'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY3.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY3Stream)
listAODtoDPD.append(WriteDAOD_SUSY3Stream.StreamName)

class WriteDAOD_SUSY4Stream(JobProperty):
    """SUSY4 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY4'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY4.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY4Stream)
listAODtoDPD.append(WriteDAOD_SUSY4Stream.StreamName)

class WriteDAOD_SUSY5Stream(JobProperty):
    """SUSY5 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY5'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY5.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY5Stream)
listAODtoDPD.append(WriteDAOD_SUSY5Stream.StreamName)


#################################
# Defined by the Exotics group
#################################

class WriteDAOD_EXOT0Stream(JobProperty):
    """EXOT0 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT0'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT0.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT0Stream)
listAODtoDPD.append(WriteDAOD_EXOT0Stream.StreamName)

class WriteDAOD_EXOT1Stream(JobProperty):
    """EXOT1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT1Stream)
listAODtoDPD.append(WriteDAOD_EXOT1Stream.StreamName)

class WriteDAOD_EXOT2Stream(JobProperty):
    """EXOT2 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT2.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT2Stream)
listAODtoDPD.append(WriteDAOD_EXOT2Stream.StreamName)

class WriteDAOD_EXOT3Stream(JobProperty):
    """EXOT3 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT3'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT3.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT3Stream)
listAODtoDPD.append(WriteDAOD_EXOT3Stream.StreamName)

class WriteDAOD_EXOT4Stream(JobProperty):
    """EXOT4 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT4'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT4.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT4Stream)
listAODtoDPD.append(WriteDAOD_EXOT4Stream.StreamName)

class WriteDAOD_EXOT5Stream(JobProperty):
    """EXOT5 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT5'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT5.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT5Stream)
listAODtoDPD.append(WriteDAOD_EXOT5Stream.StreamName)

class WriteDAOD_EXOT6Stream(JobProperty):
    """EXOT6 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT6'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT6.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT6Stream)
listAODtoDPD.append(WriteDAOD_EXOT6Stream.StreamName)

class WriteDAOD_EXOT7Stream(JobProperty):
    """EXOT7 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT7'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT7.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT7Stream)
listAODtoDPD.append(WriteDAOD_EXOT7Stream.StreamName)

class WriteDNTUP_EXOT8 (JobProperty):
    """EXOT8 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDNTUP_EXOT8'
    FileName = ''
    isVirtual = False
    NTUPScript = "DerivationFrameworkExotics/EXOT8.py"
    TreeNames = ['physics']
    SubSteps = ['n2n']

class WriteDAOD_EXOT9Stream(JobProperty):
    """EXOT9 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT9'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT9.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT9Stream)
listAODtoDPD.append(WriteDAOD_EXOT9Stream.StreamName)

class WriteDAOD_EXOT10Stream(JobProperty):
    """EXOT10 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT10'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT10.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT10Stream)
listAODtoDPD.append(WriteDAOD_EXOT10Stream.StreamName)

class WriteDAOD_EXOT11Stream(JobProperty):
    """EXOT11 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT11'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT11.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT11Stream)
listAODtoDPD.append(WriteDAOD_EXOT11Stream.StreamName)

class WriteDAOD_EXOT12Stream(JobProperty):
    """EXOT12 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT12'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT12.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT12Stream)
listAODtoDPD.append(WriteDAOD_EXOT12Stream.StreamName)

class WriteDAOD_EXOT13Stream(JobProperty):
    """EXOT13 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT13'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT13.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT13Stream)
listAODtoDPD.append(WriteDAOD_EXOT13Stream.StreamName)


#################################
# Defined by the Jet/MET group
#################################

class WriteDAOD_JETM1Stream(JobProperty):
    """JETM1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETM1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETM1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETM1Stream)
listAODtoDPD.append(WriteDAOD_JETM1Stream.StreamName)

class WriteDAOD_JETM2Stream(JobProperty):
    """JETM2 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETM2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETM2.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETM2Stream)
listAODtoDPD.append(WriteDAOD_JETM2Stream.StreamName)

class WriteDAOD_JETM3Stream(JobProperty):
    """JETM3 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETM3'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETM3.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETM3Stream)
listAODtoDPD.append(WriteDAOD_JETM3Stream.StreamName)

class WriteDAOD_JETM4Stream(JobProperty):
    """JETM4 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETM4'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETM4.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETM4Stream)
listAODtoDPD.append(WriteDAOD_JETM4Stream.StreamName)

class WriteDAOD_JETM5Stream(JobProperty):
    """JETM5 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETM5'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETM5.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETM5Stream)
listAODtoDPD.append(WriteDAOD_JETM5Stream.StreamName)

class WriteDAOD_JETM6Stream(JobProperty):
    """JETM6 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETM6'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETM6.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETM6Stream)
listAODtoDPD.append(WriteDAOD_JETM6Stream.StreamName)

class WriteDAOD_JETM7Stream(JobProperty):
    """JETM7 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETM7'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETM7.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETM7Stream)
listAODtoDPD.append(WriteDAOD_JETM7Stream.StreamName)

class WriteDAOD_JETM8Stream(JobProperty):
    """JETM8 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETM8'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETM8.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETM8Stream)
listAODtoDPD.append(WriteDAOD_JETM8Stream.StreamName)

#####################################
# Defined by the inner tracking group
#####################################

class WriteDAOD_IDTR1Stream(JobProperty):
    """IDTR1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_IDTR1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkInDet/IDTR1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_IDTR1Stream)
listAODtoDPD.append(WriteDAOD_IDTR1Stream.StreamName)

#####################################
# Defined by the e-gamma group
#####################################

class WriteDAOD_EGAM1Stream(JobProperty):
    """EGAM1 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EGAM1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkEGamma/EGAM1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EGAM1Stream)
listAODtoDPD.append(WriteDAOD_EGAM1Stream.StreamName)

class WriteDAOD_EGAM2Stream (JobProperty):
    """EGAM J/psi->ee derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EGAM2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkEGamma/EGAM2.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EGAM2Stream)
listAODtoDPD.append(WriteDAOD_EGAM2Stream.StreamName)


class WriteDAOD_EGAM3Stream (JobProperty):
    """EGAM Z->eegamma derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EGAM3'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkEGamma/EGAM3.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EGAM3Stream)
listAODtoDPD.append(WriteDAOD_EGAM3Stream.StreamName)

class WriteDAOD_EGAM4Stream (JobProperty):
    """EGAM Z->mumugamma derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EGAM4'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkEGamma/EGAM4.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EGAM4Stream)
listAODtoDPD.append(WriteDAOD_EGAM4Stream.StreamName)

class WriteDAOD_EGAM5Stream (JobProperty):
     """EGAM W->enu derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_EGAM5'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkEGamma/EGAM5.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EGAM5Stream)
listAODtoDPD.append(WriteDAOD_EGAM5Stream.StreamName)

######################################
# Defined by the flavour tagging group
######################################

class WriteDAOD_FTAG1Stream (JobProperty):
     """FTAG1 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_FTAG1'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkFlavourTag/FTAG1.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_FTAG1Stream)
listAODtoDPD.append(WriteDAOD_FTAG1Stream.StreamName)

class WriteDAOD_FTAG2Stream (JobProperty):
     """FTAG2 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_FTAG2'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkFlavourTag/FTAG2.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_FTAG2Stream)
listAODtoDPD.append(WriteDAOD_FTAG2Stream.StreamName)

class WriteDAOD_FTAG3Stream (JobProperty):
     """FTAG3 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_FTAG3'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkFlavourTag/FTAG3.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_FTAG3Stream)
listAODtoDPD.append(WriteDAOD_FTAG3Stream.StreamName)

class WriteDAOD_FTAG4Stream (JobProperty):
     """FTAG4 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_FTAG4'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkFlavourTag/FTAG4.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_FTAG4Stream)
listAODtoDPD.append(WriteDAOD_FTAG4Stream.StreamName)

######################################
# Defined by the B-physics group
######################################

class WriteDAOD_BPHY1Stream (JobProperty):
     """BPHY1 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_BPHY1'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkBPhys/BPHY1.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY1Stream)
listAODtoDPD.append(WriteDAOD_BPHY1Stream.StreamName)

######################################
# Defined by the MCP group
######################################

class WriteDAOD_MUON0Stream (JobProperty):
     """MUON0 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_MUON0'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkMuons/MUON0.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_MUON0Stream)
listAODtoDPD.append(WriteDAOD_MUON0Stream.StreamName)

class WriteDAOD_MUON1Stream (JobProperty):
     """MUON1 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_MUON1'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkMuons/MUON1.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_MUON1Stream)
listAODtoDPD.append(WriteDAOD_MUON1Stream.StreamName)

class WriteDAOD_MUON2Stream (JobProperty):
     """MUON2 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_MUON2'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkMuons/MUON2.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_MUON2Stream)
listAODtoDPD.append(WriteDAOD_MUON2Stream.StreamName)

class WriteDAOD_MUON3Stream (JobProperty):
     """MUON3 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_MUON3'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkMuons/MUON3.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_MUON3Stream)
listAODtoDPD.append(WriteDAOD_MUON3Stream.StreamName)

########################################
# Defined by the Physics Modelling group
########################################

class WriteDAOD_MCGN1Stream (JobProperty):
     """MCGN1 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_MCGN1'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkMCTruth/MCGN1.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_MCGN1Stream)
listAODtoDPD.append(WriteDAOD_MCGN1Stream.StreamName)


from RecExConfig.RecoFunctions import AddValidItemToList
AddValidItemToList(listAODtoDPD,listAllKnownDPD)



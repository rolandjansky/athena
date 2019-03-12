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
# Defined for physics validation
####################################

class WriteDAOD_PHYSVALStream(JobProperty):
    """ DAOD_PHYSVAL - special format for physics validation """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_PHYSVAL"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkPhysicsValidation/PHYSVAL.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_PHYSVALStream)
listAODtoDPD.append(WriteDAOD_PHYSVALStream.StreamName)

####################################
# Run-3 prototypes
####################################

class WriteDAOD_MINIStream(JobProperty):
    """ DAOD_MINI - prototype format for Run 3 """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_MINI"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkExamples/MINI.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_MINIStream)
listAODtoDPD.append(WriteDAOD_MINIStream.StreamName)

class WriteDAOD_NANOStream(JobProperty):
    """ DAOD_NANO - prototype format for Run 3 """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_NANO"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkExamples/NANO.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_NANOStream)
listAODtoDPD.append(WriteDAOD_NANOStream.StreamName)

####################################
# Defined by ASG for tests/examples
####################################

class WriteDAOD_PASSTHRStream(JobProperty):
    """ Produce the DPD for DAOD_PASSTHR - pass through """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_PASSTHR"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkExamples/PassThroughExample.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_PASSTHRStream)
listAODtoDPD.append(WriteDAOD_PASSTHRStream.StreamName)

class WriteDAOD_TEST1Stream(JobProperty):
    """ Produce the DPD for DAOD_TEST1 - skimming example """
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
    """ Produce the DPD for DAOD_TEST2 - skimming example using ExpressionEvaluation """
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
    """ Produce the DPD for DAOD_TEST3 - skimming example using augmentations """
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
    """ Produce the DPD for DAOD_TEST4 - slimming example """
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
    """ Produce the DPD for DAOD_TEST5 - CP tools example """
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
    """ Produce the DPD for DAOD_TEST6 - private sequencer example """
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
    """ Produce the DPD for DAOD_TEST7 - augmentation example """
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
    """ Produce the DPD for DAOD_TEST8 - thinning example """
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
    """ Produce the DPD for DAOD_TEST9 - example of adding reconstruction tools """
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

class WriteDAOD_TEST10Stream(JobProperty):
    """ Produce the DPD for DAOD_TEST10 - example of trigger content management """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_TEST10"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkExamples/TriggerContentExample.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TEST10Stream)
listAODtoDPD.append(WriteDAOD_TEST10Stream.StreamName)


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
    """TOPQ1: fakes, single t"""
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
    """TOPQ2: dilepton"""
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

#class WriteDAOD_TOPQ3Stream(JobProperty):
#    """TOPQ3: l+jets signal region - obsolete"""
#    statusOn = True
#    allowedTypes = ['bool']
#    StoredValue = False
#    StreamName = 'StreamDAOD_TOPQ3'
#    FileName = ''
#    isVirtual = False
#    DPDMakerScript = "DerivationFrameworkTop/TOPQ3.py"
#    pass
#jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TOPQ3Stream)
#listAODtoDPD.append(WriteDAOD_TOPQ3Stream.StreamName)

class WriteDAOD_TOPQ4Stream(JobProperty):
    """TOPQ4: all hadronic"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_TOPQ4'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkTop/TOPQ4.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TOPQ4Stream)
listAODtoDPD.append(WriteDAOD_TOPQ4Stream.StreamName)

class WriteDAOD_TOPQ5Stream(JobProperty):
    """TOPQ5: tt with Jpsi->mumu"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_TOPQ5'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkTop/TOPQ5.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TOPQ5Stream)
listAODtoDPD.append(WriteDAOD_TOPQ5Stream.StreamName)


##################################
# Defined by Higgs subgroup 1
##################################

class WriteDAOD_HIGG1D1Stream(JobProperty):
    """HIGG1D1: diphoton"""
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

class WriteDAOD_HIGG1D2Stream(JobProperty):
    """HIGG1D2: H->Zy"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG1D2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG1D2.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG1D2Stream)
listAODtoDPD.append(WriteDAOD_HIGG1D2Stream.StreamName)

#class WriteDAOD_HIGG1D3Stream(JobProperty):
#    """HIGG1D3: obsolete"""
#    statusOn = True
#    allowedTypes = ['bool']
#    StoredValue = False
#    StreamName = 'StreamDAOD_HIGG1D3'
#    FileName = ''
#    isVirtual = False
#    DPDMakerScript = "DerivationFrameworkHiggs/HIGG1D3.py"
#    pass
#jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG1D3Stream)
#listAODtoDPD.append(WriteDAOD_HIGG1D3Stream.StreamName)

##################################
# Defined by Higgs subgroup 2
##################################

class WriteDAOD_HIGG2D1Stream(JobProperty):
    """HIGG2D1: 2 lepton"""
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

#class WriteDAOD_HIGG2D2Stream(JobProperty):
#    """HIGG2D2: obsolete"""
#    statusOn = True
#    allowedTypes = ['bool']
#    StoredValue = False
#    StreamName = 'StreamDAOD_HIGG2D2'
#    FileName = ''
#    isVirtual = False
#    DPDMakerScript = "DerivationFrameworkHiggs/HIGG2D2.py"
#    pass
#jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG2D2Stream)
#listAODtoDPD.append(WriteDAOD_HIGG2D2Stream.StreamName)

#class WriteDAOD_HIGG2D3Stream(JobProperty):
#    """HIGG2D3: obsolete"""
#    statusOn = True
#    allowedTypes = ['bool']
#    StoredValue = False
#    StreamName = 'StreamDAOD_HIGG2D3'
#    FileName = ''
#    isVirtual = False
#    DPDMakerScript = "DerivationFrameworkHiggs/HIGG2D3.py"
#    pass
#jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG2D3Stream)
#listAODtoDPD.append(WriteDAOD_HIGG2D3Stream.StreamName)

class WriteDAOD_HIGG2D4Stream(JobProperty):
    """HIGG2D4: llqq"""
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
    """HIGG2D5: J/psi gamma"""
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
    """HIGG3D1: WW to lvlv"""
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

#class WriteDAOD_HIGG3D2Stream(JobProperty):
#    """HIGG3D2: obsolete"""
#    statusOn = True
#    allowedTypes = ['bool']
#    StoredValue = False
#    StreamName = 'StreamDAOD_HIGG3D2'
#    FileName = ''
#    isVirtual = False
#    DPDMakerScript = "DerivationFrameworkHiggs/HIGG3D2.py"
#    pass
#jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG3D2Stream)
#listAODtoDPD.append(WriteDAOD_HIGG3D2Stream.StreamName)

class WriteDAOD_HIGG3D3Stream(JobProperty):
    """HIGG3D3: MJ background"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG3D3'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG3D3.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG3D3Stream)
listAODtoDPD.append(WriteDAOD_HIGG3D3Stream.StreamName)

class WriteDAOD_HIGG3D4Stream(JobProperty):
    """HIGG3D4: WW to lvlv"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG3D4'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG3D4.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG3D4Stream)
listAODtoDPD.append(WriteDAOD_HIGG3D4Stream.StreamName)


##################################
# Defined by Higgs subgroup 4
##################################

class WriteDAOD_HIGG4D1Stream(JobProperty):
    """HIGG4D1: SM H->tautau, LFV H->l+tau, H->mumu, VH->tautau"""
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
    """HIGG4D2: SM H->tautau, LFV H->l+tau, BSM H->tautau, HH->tautaubb"""
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


class WriteDAOD_HIGG4D3Stream(JobProperty):
    """HIGG4D3: SM H->tautau, Higgs CP, HH->tautaubb"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG4D3'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG4D3.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG4D3Stream)
listAODtoDPD.append(WriteDAOD_HIGG4D3Stream.StreamName)

class WriteDAOD_HIGG4D4Stream(JobProperty):
    """HIGG4D4: BSM H->tautau"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG4D4'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG4D4.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG4D4Stream)
listAODtoDPD.append(WriteDAOD_HIGG4D4Stream.StreamName)


class WriteDAOD_HIGG4D5Stream(JobProperty):
    """HIGG4D5: W'->tau nu"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG4D5'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG4D5.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG4D5Stream)
listAODtoDPD.append(WriteDAOD_HIGG4D5Stream.StreamName)

class WriteDAOD_HIGG4D6Stream(JobProperty):
    """HIGG4D6: boosted HH->tautaubb"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG4D6'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG4D6.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG4D6Stream)
listAODtoDPD.append(WriteDAOD_HIGG4D6Stream.StreamName)

##################################
# Defined by Higgs subgroup 5
##################################

class WriteDAOD_HIGG5D1Stream(JobProperty):
    """HIGG5D1: VH H to bb"""
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
   """HIGG5D2: VH H to bb"""
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

class WriteDAOD_HIGG5D3Stream(JobProperty):
    """HIGG5D3: VBF H """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HIGG5D3'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HIGG5D3.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG5D3Stream)
listAODtoDPD.append(WriteDAOD_HIGG5D3Stream.StreamName)

##################################
# Defined by Higgs subgroup 6
##################################

class WriteDAOD_HIGG6D1Stream(JobProperty):
    """HIGG6D1: H+ to taujet BSM"""
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
   """HIGG6D2: H+ to taulep BSM"""
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

class WriteDAOD_HDBS1Stream(JobProperty):
    """HDBS1: ttX X to tautau"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_HDBS1'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkHiggs/HDBS1.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HDBS1Stream)
listAODtoDPD.append(WriteDAOD_HDBS1Stream.StreamName)


##################################
# Defined by Higgs subgroup 8
##################################

class WriteDAOD_HIGG8D1Stream(JobProperty):
    """HIGG8D1: ttH, H to WW/tautau/ZZ"""
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

#class WriteDAOD_HIGG8D2Stream(JobProperty):
#    """HIGG8D1: obsolete"""
#    statusOn = True
#    allowedTypes = ['bool']
#    StoredValue = False
#    StreamName = 'StreamDAOD_HIGG8D2'
#    FileName = ''
#    isVirtual = False
#    DPDMakerScript = "DerivationFrameworkHiggs/HIGG8D2.py"
#    pass
#jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HIGG8D2Stream)
#listAODtoDPD.append(WriteDAOD_HIGG8D2Stream.StreamName)

#################################
# Defined by the STDM group
#################################

#class WriteDAOD_STDM1Stream (JobProperty):
#    """STDM jet slim: obsolete"""
#    statusOn = True
#    allowedTypes = ['bool']
#    StoredValue = False
#    StreamName = 'StreamDAOD_STDM1'
#    FileName = ''
#    isVirtual = False
#    DPDMakerScript = "DerivationFrameworkSM/STDM1.py"
#    pass
#jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_STDM1Stream)
#listAODtoDPD.append(WriteDAOD_STDM1Stream.StreamName)

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

class WriteDAOD_STDM6Stream (JobProperty):
    """STDM Diffractive Physics Derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_STDM6'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSM/STDM6.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_STDM6Stream)
listAODtoDPD.append(WriteDAOD_STDM6Stream.StreamName)

class WriteDAOD_STDM7Stream (JobProperty):
    """STDM Low Mass Drell Yan"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_STDM7'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSM/STDM7.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_STDM7Stream)
listAODtoDPD.append(WriteDAOD_STDM7Stream.StreamName)

class WriteDAOD_STDM8Stream (JobProperty):
    """STDM Heavy ION Derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_STDM8'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSM/STDM8.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_STDM8Stream)
listAODtoDPD.append(WriteDAOD_STDM8Stream.StreamName)

class WriteDAOD_STDM9Stream (JobProperty):
    """STDM Heavy ION Derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_STDM9'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSM/STDM9.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_STDM9Stream)
listAODtoDPD.append(WriteDAOD_STDM9Stream.StreamName)

class WriteDAOD_STDM10Stream (JobProperty):
    """STDM Heavy ION Derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_STDM10'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSM/STDM10.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_STDM10Stream)
listAODtoDPD.append(WriteDAOD_STDM10Stream.StreamName)

class WriteDAOD_STDM11Stream (JobProperty):
    """Derivation for inclusive jets and leading jet measurements"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_STDM11'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSM/STDM11.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_STDM11Stream)
listAODtoDPD.append(WriteDAOD_STDM11Stream.StreamName)

#################################
# Defined by the Tau group
#################################

class WriteDAOD_TAUP1Stream(JobProperty):
    """TAUP1: Z to etau"""
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

class WriteDAOD_TAUP2Stream(JobProperty):
    """TAUP2: Z to mutau"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_TAUP2'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkTau/TAUP2.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TAUP2Stream)
listAODtoDPD.append(WriteDAOD_TAUP2Stream.StreamName)

class WriteDAOD_TAUP3Stream(JobProperty):
    """TAUP3: Tau ID training on MC"""
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

class WriteDAOD_TAUP4Stream(JobProperty):
    """TAUP4: Tau ID traning on Data"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_TAUP4'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkTau/TAUP4.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TAUP4Stream)
listAODtoDPD.append(WriteDAOD_TAUP4Stream.StreamName)


#################################
# Defined by the SUSY group
#################################

class WriteDAOD_SUSY1Stream(JobProperty):
    """SUSY1: 0-lepton"""
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
    """SUSY2: Z+MET, EWK searches"""
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
    """SUSY3: SUSY w tau"""
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
    """SUSY4: multi-jet searches"""
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
    """SUSY5: stop / strong 1-lepton"""
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

class WriteDAOD_SUSY6Stream(JobProperty):
    """SUSY6: pixel dE/dx"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY6'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY6.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY6Stream)
listAODtoDPD.append(WriteDAOD_SUSY6Stream.StreamName)

class WriteDAOD_SUSY7Stream(JobProperty):
    """SUSY7: b-jet"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY7'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY7.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY7Stream)
listAODtoDPD.append(WriteDAOD_SUSY7Stream.StreamName)

class WriteDAOD_SUSY8Stream(JobProperty):
    """SUSY8: SMP"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY8'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY8.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY8Stream)
listAODtoDPD.append(WriteDAOD_SUSY8Stream.StreamName)

class WriteDAOD_SUSY9Stream(JobProperty):
    """SUSY9: Low-pt Z+jets"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY9'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY9.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY9Stream)
listAODtoDPD.append(WriteDAOD_SUSY9Stream.StreamName)

class WriteDAOD_SUSY10Stream(JobProperty):
    """SUSY10: Gtt"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY10'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY10.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY10Stream)
listAODtoDPD.append(WriteDAOD_SUSY10Stream.StreamName)

class WriteDAOD_SUSY11Stream(JobProperty):
    """SUSY11: jet smearing"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY11'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY11.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY11Stream)
listAODtoDPD.append(WriteDAOD_SUSY11Stream.StreamName)

class WriteDAOD_SUSY12Stream(JobProperty):
    """SUSY12: stop -> c+LSP"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY12'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY12.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY12Stream)
listAODtoDPD.append(WriteDAOD_SUSY12Stream.StreamName)

class WriteDAOD_SUSY13Stream(JobProperty):
    """SUSY13: stop 1L"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY13'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY13.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY13Stream)
listAODtoDPD.append(WriteDAOD_SUSY13Stream.StreamName)

class WriteDAOD_SUSY14Stream(JobProperty):
    """SUSY14: stop 1L"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY14'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY14.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY14Stream)
listAODtoDPD.append(WriteDAOD_SUSY14Stream.StreamName)

class WriteDAOD_SUSY15Stream(JobProperty):
    """SUSY15: Displaced vertex"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY15'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY15.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY15Stream)
listAODtoDPD.append(WriteDAOD_SUSY15Stream.StreamName)

class WriteDAOD_SUSY16Stream(JobProperty):
    """SUSY16: Higgsino"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY16'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY16.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY16Stream)
listAODtoDPD.append(WriteDAOD_SUSY16Stream.StreamName)

class WriteDAOD_SUSY17Stream(JobProperty):
    """SUSY17: stop 1-lepton R&D"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY17'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY17.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY17Stream)
listAODtoDPD.append(WriteDAOD_SUSY17Stream.StreamName)

class WriteDAOD_SUSY18Stream(JobProperty):
    """SUSY18: Direct stau"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY18'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY18.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY18Stream)
listAODtoDPD.append(WriteDAOD_SUSY18Stream.StreamName)

class WriteDAOD_SUSY19Stream(JobProperty):
    """SUSY19: 1lepton + 1 track"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_SUSY19'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkSUSY/SUSY19.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_SUSY19Stream)
listAODtoDPD.append(WriteDAOD_SUSY19Stream.StreamName)

#################################
# Defined by the Exotics group
#################################

class WriteDAOD_EXOT0Stream(JobProperty):
    """EXOT0: dileptons"""
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

#class WriteDAOD_EXOT1Stream(JobProperty):
#    """EXOT1: obsolete"""
#    statusOn = True
#    allowedTypes = ['bool']
#    StoredValue = False
#    StreamName = 'StreamDAOD_EXOT1'
#    FileName = ''
#    isVirtual = False
#    DPDMakerScript = "DerivationFrameworkExotics/EXOT1.py"
#    pass
#jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT1Stream)
#listAODtoDPD.append(WriteDAOD_EXOT1Stream.StreamName)

class WriteDAOD_EXOT2Stream(JobProperty):
    """EXOT2: dijets"""
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
    """EXOT3: diboson to dijets"""
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
    """EXOT4: ttbar resonances (lepton+jets), single VLQ production, VLQ opposite sign, VLQ pair production -> Wb+X"""
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
    """EXOT5: Monojets"""
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
    """EXOT6: mono-photon, gamma+jet, SM Z(nunu)g"""
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
    """EXOT7: ttbar res., monotop (HQT)"""
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

class WriteDAOD_EXOT8Stream(JobProperty):
    """EXOT8: 4b"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT8'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT8.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT8Stream)
listAODtoDPD.append(WriteDAOD_EXOT8Stream.StreamName)

class WriteDAOD_EXOT9Stream(JobProperty):
    """EXOT9: lepton + MET, leptoquarks"""
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
    """EXOT10: SUSY diphoton + MET, SUSY photon + jets + MET, SUSY C1N2 production with C1 -> W+N1(LSP), N2 -> h(->diphoton) + N1 (LSP)"""
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

#class WriteDAOD_EXOT11Stream(JobProperty):
#    """EXOT11: obsolete"""
#    statusOn = True
#    allowedTypes = ['bool']
#    StoredValue = False
#    StreamName = 'StreamDAOD_EXOT11'
#    FileName = ''
#    isVirtual = False
#    DPDMakerScript = "DerivationFrameworkExotics/EXOT11.py"
#    pass
#jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT11Stream)
#listAODtoDPD.append(WriteDAOD_EXOT11Stream.StreamName)

class WriteDAOD_EXOT12Stream(JobProperty):
    """EXOT12: leptoquarks, SS dilepton, type III seesaw, lepton compositness/contact interaction, heavy neutrinos, heavy leptons"""
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
    """EXOT13: displaced lepton-jet"""
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

#class WriteDAOD_EXOT14Stream(JobProperty):
#    """EXOT14: obsolete"""
#    statusOn = True
#    allowedTypes = ['bool']
#    StoredValue = False
#    StreamName = 'StreamDAOD_EXOT14'
#    FileName = ''
#    isVirtual = False
#    DPDMakerScript = "DerivationFrameworkExotics/EXOT14.py"
#    pass
#jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT14Stream)
#listAODtoDPD.append(WriteDAOD_EXOT14Stream.StreamName)

class WriteDAOD_EXOT15Stream(JobProperty):
    """EXOT15: MS-CR-ID vertices"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT15'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT15.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT15Stream)
listAODtoDPD.append(WriteDAOD_EXOT15Stream.StreamName)

#class WriteDAOD_EXOT16Stream(JobProperty):
#    """EXOT16: obsolete"""
#    statusOn = True
#    allowedTypes = ['bool']
#    StoredValue = False
#    StreamName = 'StreamDAOD_EXOT16'
#    FileName = ''
#    isVirtual = False
#    DPDMakerScript = "DerivationFrameworkExotics/EXOT16.py"
#    pass
#jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT16Stream)
#listAODtoDPD.append(WriteDAOD_EXOT16Stream.StreamName)

class WriteDAOD_EXOT17Stream(JobProperty):
    """EXOT17: muon+MET, multicharge """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT17'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT17.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT17Stream)
listAODtoDPD.append(WriteDAOD_EXOT17Stream.StreamName)

#class WriteDAOD_EXOT18Stream(JobProperty):
#    """EXOT18: obsolete"""
#    statusOn = True
#    allowedTypes = ['bool']
#    StoredValue = False
#    StreamName = 'StreamDAOD_EXOT18'
#    FileName = ''
#    isVirtual = False
#    DPDMakerScript = "DerivationFrameworkExotics/EXOT18.py"
#    pass
#jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT18Stream)
#listAODtoDPD.append(WriteDAOD_EXOT18Stream.StreamName)

class WriteDAOD_EXOT19Stream(JobProperty):
    """EXOT19: leptoquarks, SS dilepton, type III seesaw (electron fakes)"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT19'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT19.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT19Stream)
listAODtoDPD.append(WriteDAOD_EXOT19Stream.StreamName)

class WriteDAOD_EXOT20Stream(JobProperty):
    """EXOT20: displaced non-collimated dimuons"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT20'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT20.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT20Stream)
listAODtoDPD.append(WriteDAOD_EXOT20Stream.StreamName)

class WriteDAOD_EXOT21Stream(JobProperty):
    """EXOT21: lepton-jet prompt"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT21'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT21.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT21Stream)
listAODtoDPD.append(WriteDAOD_EXOT21Stream.StreamName)

class WriteDAOD_EXOT22Stream(JobProperty):
    """EXOT22: SS dilepton, type III seesaw (muon fakes)"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT22'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT22.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT22Stream)
listAODtoDPD.append(WriteDAOD_EXOT22Stream.StreamName)

class WriteDAOD_EXOT23Stream(JobProperty):
    """EXOT23: emerging jets (runs on DAOD_RPVLL)"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT23'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT23.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT23Stream)
listAODtoDPD.append(WriteDAOD_EXOT23Stream.StreamName)

class WriteDAOD_EXOT24Stream(JobProperty):
    """EXOT24: 0L for monoHbb analysis"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT24'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT24.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT24Stream)
listAODtoDPD.append(WriteDAOD_EXOT24Stream.StreamName)

class WriteDAOD_EXOT25Stream(JobProperty):
    """EXOT25: 1L for monoHbb analysis"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT25'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT25.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT25Stream)
listAODtoDPD.append(WriteDAOD_EXOT25Stream.StreamName)

class WriteDAOD_EXOT26Stream(JobProperty):
    """EXOT26: 2L for monoHbb analysis"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT26'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT26.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT26Stream)
listAODtoDPD.append(WriteDAOD_EXOT26Stream.StreamName)

class WriteDAOD_EXOT27Stream(JobProperty):
    """EXOT27: Derivation for Mono-Hbb and Mono-sWW analyses"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EXOT27'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkExotics/EXOT27.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EXOT27Stream)
listAODtoDPD.append(WriteDAOD_EXOT27Stream.StreamName)

#################################
# Defined by the Jet/MET group
#################################

class WriteDAOD_JETM1Stream(JobProperty):
    """JETM1: dijet calibration, JES/JER, small&large R"""
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
    """JETM2: W+j MET ana"""
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
    """JETM3: Z+jets"""
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
    """JETM4: gamma+jet"""
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
    """JETM5: zero bias"""
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
    """JETM6: jet substructure: top"""
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
    """JETM7: top"""
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
    """JETM8: jet substructure: gen"""
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

class WriteDAOD_JETM9Stream(JobProperty):
    """JETM9: Many R values calibration"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETM9'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETM9.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETM9Stream)
listAODtoDPD.append(WriteDAOD_JETM9Stream.StreamName)

class WriteDAOD_JETM10Stream(JobProperty):
    """JETM10: trigger MET rate study"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETM10'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETM10.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETM10Stream)
listAODtoDPD.append(WriteDAOD_JETM10Stream.StreamName)

class WriteDAOD_JETM11Stream(JobProperty):
    """JETM11: trigger MET efficiency study"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETM11'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETM11.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETM11Stream)
listAODtoDPD.append(WriteDAOD_JETM11Stream.StreamName)

class WriteDAOD_JETM12Stream(JobProperty):
    """JETM12: Tau E over p ana"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETM12'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETM12.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETM12Stream)
listAODtoDPD.append(WriteDAOD_JETM12Stream.StreamName)

class WriteDAOD_JETM13Stream(JobProperty):
    """JETM13: jet/MET constituent studies"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETM13'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETM13.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETM13Stream)
listAODtoDPD.append(WriteDAOD_JETM13Stream.StreamName)

class WriteDAOD_JETM14Stream(JobProperty):
    """JETM14: trigger MET efficiency study"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETM14'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETM14.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETM14Stream)
listAODtoDPD.append(WriteDAOD_JETM14Stream.StreamName)

class WriteDAOD_JETM15Stream(JobProperty):
    """JETM16: Grooming scan data derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_JETM15'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkJetEtMiss/JETM15.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_JETM15Stream)
listAODtoDPD.append(WriteDAOD_JETM15Stream.StreamName)


#####################################
# Defined by the inner tracking group
#####################################

#class WriteDAOD_IDTR0Stream(JobProperty):
#    """IDTR0 derivation"""
#    statusOn = True
#    allowedTypes = ['bool']
#    StoredValue = False
#    StreamName = 'StreamDAOD_IDTR0'
#    FileName = ''
#    isVirtual = False
#    DPDMakerScript = "DerivationFrameworkInDet/IDTR0.py"
#    pass
#jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_IDTR0Stream)
#listAODtoDPD.append(WriteDAOD_IDTR0Stream.StreamName)


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
    """EGAM Zee derivation"""
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

class WriteDAOD_EGAM6Stream (JobProperty):
     """EGAM Zee loose derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_EGAM6'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkEGamma/EGAM6.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EGAM6Stream)
listAODtoDPD.append(WriteDAOD_EGAM6Stream.StreamName)

class WriteDAOD_EGAM7Stream (JobProperty):
     """EGAM inclusive electron derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_EGAM7'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkEGamma/EGAM7.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EGAM7Stream)
listAODtoDPD.append(WriteDAOD_EGAM7Stream.StreamName)

class WriteDAOD_EGAM8Stream (JobProperty):
    """EGAM Zee forward e derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EGAM8'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkEGamma/EGAM8.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EGAM8Stream)
listAODtoDPD.append(WriteDAOD_EGAM8Stream.StreamName)

class WriteDAOD_EGAM9Stream (JobProperty):
    """EGAM photon trigger bootstrapping derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_EGAM9'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkEGamma/EGAM9.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_EGAM9Stream)
listAODtoDPD.append(WriteDAOD_EGAM9Stream.StreamName)


######################################
# Defined by the flavour tagging group
######################################

class WriteDAOD_FTAG1Stream (JobProperty):
     """FTAG1: D*, negative tag"""
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
     """FTAG2: top dilepton"""
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
     """FTAG3: pTrel"""
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
     """FTAG4: Wc, single lep top"""
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

class WriteDAOD_FTAG5Stream (JobProperty):
    """FTAG5: Xbb Studies"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_FTAG5'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkFlavourTag/FTAG5.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_FTAG5Stream)
listAODtoDPD.append(WriteDAOD_FTAG5Stream.StreamName)

######################################
# Defined by the B-physics group
######################################

class WriteDAOD_BPHY1Stream (JobProperty):
     """BPHY1: dimuon"""
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

class WriteDAOD_BPHY2Stream (JobProperty):
     """BPHY2: Bs-> Upsilon phi"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_BPHY2'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkBPhys/BPHY2.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY2Stream)
listAODtoDPD.append(WriteDAOD_BPHY2Stream.StreamName)

class WriteDAOD_BPHY3Stream (JobProperty):
     """BPHY3: di-track resonances (not in prod.)"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_BPHY3'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkBPhys/BPHY3.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY3Stream)
listAODtoDPD.append(WriteDAOD_BPHY3Stream.StreamName)

class WriteDAOD_BPHY4Stream (JobProperty):
     """BPHY4: charmed mesons"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_BPHY4'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkBPhys/BPHY4.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY4Stream)
listAODtoDPD.append(WriteDAOD_BPHY4Stream.StreamName)

class WriteDAOD_BPHY5Stream (JobProperty):
     """BPHY5: Bs->J/psi B+ -> J/psiK+; B_c"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_BPHY5'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkBPhys/BPHY5.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY5Stream)
listAODtoDPD.append(WriteDAOD_BPHY5Stream.StreamName)

class WriteDAOD_BPHY6Stream (JobProperty):
     """BPHY6: Trigger-based studies; like BPHY1, wider invariant mass window. Run on demand."""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_BPHY6'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkBPhys/BPHY6.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY6Stream)
listAODtoDPD.append(WriteDAOD_BPHY6Stream.StreamName)

class WriteDAOD_BPHY7Stream (JobProperty):
     """BPHY7: 3 muon"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_BPHY7'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkBPhys/BPHY7.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY7Stream)
listAODtoDPD.append(WriteDAOD_BPHY7Stream.StreamName)

class WriteDAOD_BPHY8Stream (JobProperty):
     """BPHY8: B(s) -> mumu & references"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_BPHY8'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkBPhys/BPHY8.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY8Stream)
listAODtoDPD.append(WriteDAOD_BPHY8Stream.StreamName)

class WriteDAOD_BPHY9Stream (JobProperty):
     """BPHY9: J/psi+vector"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_BPHY9'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkBPhys/BPHY9.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY9Stream)
listAODtoDPD.append(WriteDAOD_BPHY9Stream.StreamName)

class WriteDAOD_BPHY10Stream (JobProperty):
     """BPHY10: Bd->J/psiK*"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_BPHY10'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkBPhys/BPHY10.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY10Stream)
listAODtoDPD.append(WriteDAOD_BPHY10Stream.StreamName)

class WriteDAOD_BPHY11Stream (JobProperty):
     """BPHY11: pentaquark analyses"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_BPHY11'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkBPhys/BPHY11.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY11Stream)
listAODtoDPD.append(WriteDAOD_BPHY11Stream.StreamName)

class WriteDAOD_BPHY12Stream (JobProperty):
     """BPHY12: B->K*mumu"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_BPHY12'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkBPhys/BPHY12.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY12Stream)
listAODtoDPD.append(WriteDAOD_BPHY12Stream.StreamName)

class WriteDAOD_BPHY14Stream (JobProperty):
    """BPHY14 derivation"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    StreamName = 'StreamDAOD_BPHY14'
    FileName = ''
    isVirtual = False
    DPDMakerScript = "DerivationFrameworkBPhys/BPHY14.py"
    pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY14Stream)
listAODtoDPD.append(WriteDAOD_BPHY14Stream.StreamName)



class WriteDAOD_BPHY15Stream (JobProperty):
     """BPHY15: Bc->J/psi D"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_BPHY15'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkBPhys/BPHY15.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY15Stream)
listAODtoDPD.append(WriteDAOD_BPHY15Stream.StreamName)

class WriteDAOD_BPHY16Stream (JobProperty):
     """BPHY16: Upsilon +2Muons"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_BPHY16'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkBPhys/BPHY16.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY16Stream)
listAODtoDPD.append(WriteDAOD_BPHY16Stream.StreamName)

class WriteDAOD_BPHY17Stream (JobProperty):
     """BPHY17: Ds + pi+/-"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_BPHY17'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkBPhys/BPHY17.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY17Stream)
listAODtoDPD.append(WriteDAOD_BPHY17Stream.StreamName)

class WriteDAOD_BPHY18Stream (JobProperty):
     """BPHY18: Kstar +2Electrons"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_BPHY18'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkBPhys/BPHY18.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY18Stream)
listAODtoDPD.append(WriteDAOD_BPHY18Stream.StreamName)

class WriteDAOD_BPHY19Stream (JobProperty):
     """BPHY19: di-muon with photon conversions"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_BPHY19'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkBPhys/BPHY19.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY19Stream)
listAODtoDPD.append(WriteDAOD_BPHY19Stream.StreamName)

class WriteDAOD_BPHY20Stream (JobProperty):
     """BPHY20: R_Jpsi"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_BPHY20'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkBPhys/BPHY20.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_BPHY20Stream)
listAODtoDPD.append(WriteDAOD_BPHY20Stream.StreamName)

######################################
# Defined by the MCP group
######################################

class WriteDAOD_MUON0Stream (JobProperty):
     """MUON0: General MCP use"""
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
     """MUON1: Z to mumu"""
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
     """MUON2: J/psi->mumu"""
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
     """MUON3: J/psi & Upsilon"""
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

class WriteDAOD_MUON4Stream (JobProperty):
     """MUON4: MC only"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_MUON4'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkMuons/MUON4.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_MUON4Stream)
listAODtoDPD.append(WriteDAOD_MUON4Stream.StreamName)

class WriteDAOD_MUON5Stream (JobProperty):
     """MUON5: MC only"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_MUON5'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkMuons/MUON5.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_MUON5Stream)
listAODtoDPD.append(WriteDAOD_MUON5Stream.StreamName)

#############################
# Defined by the L1Calo group
#############################

##############################
# Defined by the TileCal group
##############################

class WriteDAOD_TCAL1Stream (JobProperty):
     """TCAL1 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_TCAL1'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkTileCal/TCAL1.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TCAL1Stream)
listAODtoDPD.append(WriteDAOD_TCAL1Stream.StreamName)

########################################
# Defined by the Physics Modelling group
########################################

class WriteDAOD_TRUTH0Stream (JobProperty):
     """TRUTH0 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_TRUTH0'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkMCTruth/TRUTH0.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TRUTH0Stream)
listAODtoDPD.append(WriteDAOD_TRUTH0Stream.StreamName)

class WriteDAOD_TRUTH1Stream (JobProperty):
     """TRUTH1 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_TRUTH1'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkMCTruth/TRUTH1.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TRUTH1Stream)
listAODtoDPD.append(WriteDAOD_TRUTH1Stream.StreamName)

class WriteDAOD_TRUTH2Stream (JobProperty):
     """TRUTH2 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_TRUTH2'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkMCTruth/TRUTH2.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TRUTH2Stream)
listAODtoDPD.append(WriteDAOD_TRUTH2Stream.StreamName)

# Deliberately in SUSY for now
class WriteDAOD_TRUTH3Stream (JobProperty):
     """TRUTH3 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_TRUTH3'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkMCTruth/TRUTH3.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TRUTH3Stream)
listAODtoDPD.append(WriteDAOD_TRUTH3Stream.StreamName)

class WriteDAOD_TRUTH4Stream (JobProperty):
     """TRUTH4 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_TRUTH4'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkMCTruth/TRUTH4.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TRUTH4Stream)
listAODtoDPD.append(WriteDAOD_TRUTH4Stream.StreamName)

class WriteDAOD_TRUTH5Stream (JobProperty):
     """TRUTH5 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_TRUTH5'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkMCTruth/TRUTH5.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TRUTH5Stream)
listAODtoDPD.append(WriteDAOD_TRUTH5Stream.StreamName)


#############################
# Defined by Data Preparation
#############################

class WriteDAOD_DAPR0Stream (JobProperty):
     """DAPR0 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_DAPR0'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkDataPrep/DAPR0.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_DAPR0Stream)
listAODtoDPD.append(WriteDAOD_DAPR0Stream.StreamName)

class WriteDAOD_DAPR1Stream (JobProperty):
     """DAPR1 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_DAPR1'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkDataPrep/DAPR1.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_DAPR1Stream)
listAODtoDPD.append(WriteDAOD_DAPR1Stream.StreamName)

class WriteDAOD_DAPR2Stream (JobProperty):
     """DAPR2 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_DAPR2'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkDataPrep/DAPR2.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_DAPR2Stream)
listAODtoDPD.append(WriteDAOD_DAPR2Stream.StreamName)



################################
# Defined by the Heavy Ion group
################################

class WriteDAOD_HION1Stream (JobProperty):
     """HION1 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_HION1'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkHI/HION1.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HION1Stream)
listAODtoDPD.append(WriteDAOD_HION1Stream.StreamName)

class WriteDAOD_HION2Stream (JobProperty):
     """HION2 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_HION2'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkHI/HION2.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HION2Stream)
listAODtoDPD.append(WriteDAOD_HION2Stream.StreamName)

class WriteDAOD_HION3Stream (JobProperty):
     """HION3 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_HION3'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkHI/HION3.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HION3Stream)
listAODtoDPD.append(WriteDAOD_HION3Stream.StreamName)

class WriteDAOD_HION4Stream (JobProperty):
     """HION4 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_HION4'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkHI/HION4.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HION4Stream)
listAODtoDPD.append(WriteDAOD_HION4Stream.StreamName)

class WriteDAOD_HION5Stream (JobProperty):
     """HION5 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_HION5'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkHI/HION5.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HION5Stream)
listAODtoDPD.append(WriteDAOD_HION5Stream.StreamName)

class WriteDAOD_HION6Stream (JobProperty):
     """HION6 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_HION6'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkHI/HION6.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HION6Stream)
listAODtoDPD.append(WriteDAOD_HION6Stream.StreamName)

class WriteDAOD_HION7Stream (JobProperty):
     """HION7 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_HION7'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkHI/HION7.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HION7Stream)
listAODtoDPD.append(WriteDAOD_HION7Stream.StreamName)

class WriteDAOD_HION8Stream (JobProperty):
     """HION8 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_HION8'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkHI/HION8.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HION8Stream)
listAODtoDPD.append(WriteDAOD_HION8Stream.StreamName)

class WriteDAOD_HION9Stream (JobProperty):
     """HION9 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_HION9'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkHI/HION9.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HION9Stream)
listAODtoDPD.append(WriteDAOD_HION9Stream.StreamName)

class WriteDAOD_HION10Stream (JobProperty):
     """HION10 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_HION10'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkHI/HION10.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_HION10Stream)
listAODtoDPD.append(WriteDAOD_HION10Stream.StreamName)

################################
# Defined by the trigger group
################################

class WriteDAOD_TRIG1Stream (JobProperty):
     """TRIG1 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_TRIG1'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkTrigger/TRIG1.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TRIG1Stream)
listAODtoDPD.append(WriteDAOD_TRIG1Stream.StreamName)

class WriteDAOD_TRIG2Stream (JobProperty):
     """TRIG2 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_TRIG2'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkTrigger/TRIG2.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TRIG2Stream)
listAODtoDPD.append(WriteDAOD_TRIG2Stream.StreamName)

class WriteDAOD_TRIG3Stream (JobProperty):
     """TRIG3 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_TRIG3'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkTrigger/TRIG3.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TRIG3Stream)
listAODtoDPD.append(WriteDAOD_TRIG3Stream.StreamName)

class WriteDAOD_TRIG4Stream (JobProperty):
     """TRIG4 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_TRIG4'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkTrigger/TRIG4.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TRIG4Stream)
listAODtoDPD.append(WriteDAOD_TRIG4Stream.StreamName)

class WriteDAOD_TRIG5Stream (JobProperty):
     """TRIG5 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_TRIG5'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkTrigger/TRIG5.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TRIG5Stream)
listAODtoDPD.append(WriteDAOD_TRIG5Stream.StreamName)

class WriteDAOD_TRIG6Stream (JobProperty):
     """TRIG6 derivation"""
     statusOn = True
     allowedTypes = ['bool']
     StoredValue = False
     StreamName = 'StreamDAOD_TRIG6'
     FileName = ''
     isVirtual = False
     DPDMakerScript = "DerivationFrameworkTrigger/TRIG6.py"
     pass
jobproperties.DerivationFrameworkProdFlags.add_JobProperty(WriteDAOD_TRIG6Stream)
listAODtoDPD.append(WriteDAOD_TRIG6Stream.StreamName)

from RecExConfig.RecoFunctions import AddValidItemToList
AddValidItemToList(listAODtoDPD,listAllKnownDPD)


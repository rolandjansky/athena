# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags
##
## Author:      Karsten Koeneke (DESY)
## Created:     August 2008
##
## Description: Here, all neccessary job flags for the PrimaryDPDMaker
##              are defined.
##
## $Id: PrimaryDPDFlags.py,v 1.44 2009-05-11 12:20:32 cgatti Exp $
##=============================================================================

__doc__ = """Here, all neccessary job flags for the PrimaryDPDMaker are defined."""
__version__ = "0.0.1"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties


## Create the list that will hold all filter names
desdEventSkimmingFilterNamesList = []
daodEventSkimmingFilterNamesList = []


# ---- Load the egammaPID and egammaParameters information
# This is needed to always be up-to-date with the egamma
# IsEM selections and also the author selections
#import PyUtils.RootUtils as ru
#ROOT = ru.import_root()
#import cppyy
#cppyy.loadDictionary('egammaEnumsDict')
#from ROOT import egammaPID
#from ROOT import egammaParameters


#=====================================================================
# First define container for the PrimaryDPDMaker flags
#=====================================================================
class PrimaryDPDFlags(JobPropertyContainer):
    """ The PrimaryDPDMaker flag/job property  container."""
jobproperties.add_Container(PrimaryDPDFlags)

#short-cut to get the PrimaryDPDFlags container with this one line:
#'from PrimaryDPDMaker.PrimaryDPDFlags import primDPD'
#Note that name has to be different to avoid problems with pickle
primDPD = jobproperties.PrimaryDPDFlags
listRAWtoDPD=[]
listESDtoDPD=[]
listAODtoDPD=[]
listDPDtoTAG=[]
listAllKnownDPD=[]  #<---this one is filled at the very bottom

#=====================================================================
# Now define each flag and add it to the container
#=====================================================================

#------------------------
# Basic setup
#------------------------

class MakePrimaryDPD(JobProperty):
    """ Produce a primary DPD if true. """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(MakePrimaryDPD)

#------------------------
# Input/output
#------------------------
class OutputDirectoryName(JobProperty):
    """ Name of the directory where the output files are written. """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ""
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(OutputDirectoryName)

class OutputMiddleName(JobProperty):
    """ Keyword added in the middle of the output files names. """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ""
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(OutputMiddleName)

class OutputPoolRootFileNamePrefix(JobProperty):
    """ File name prefix for the pool.root output files."""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ""
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(OutputPoolRootFileNamePrefix)

class OutputRAWMaxFileSize(JobProperty):
    """ Maximum output file size in MByte for the RAW DPDs."""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 10000 #This is 5 GByte
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(OutputRAWMaxFileSize)

class OutputRAWMaxNEvents(JobProperty):
    """ Maximum number of events in output file for the RAW DPDs."""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 10000000
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(OutputRAWMaxNEvents)

class WriteEventlessFiles(JobProperty):
    """ This regulates what should be done of no events are accepted for a DRAW output file. """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteEventlessFiles)

class isVirtual(JobProperty):
    """ Whether an output stream to be written or not. Has to be set for each
        DPD stream individually """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(isVirtual)

##--------------------------------
## Flags to turn on/off each possible DPD
## Note: each DPD has its own internal flags.
##--------------------------------

class WriteAllcellsStream(JobProperty):
    """ Produce the primary DPD AllCells DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESDM_ALLCELLS"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "PrimaryDPDMaker/PerfDPD_AllCells.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteAllcellsStream)
listESDtoDPD.append(WriteAllcellsStream.StreamName)

class WriteIDALIGNStream(JobProperty):
    """ Produce the primary DPD AllCells DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESDM_IDALIGN"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "PrimaryDPDMaker/DESDM_IDALIGN.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteIDALIGNStream)
listESDtoDPD.append(WriteIDALIGNStream.StreamName)

class WriteDESDM_EGAMMAStream(JobProperty):
    """ Produce the primary DPD EGamma DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESDM_EGAMMA"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "PrimaryDPDMaker/PerfDPD_EGamma.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDESDM_EGAMMAStream)
listESDtoDPD.append(WriteDESDM_EGAMMAStream.StreamName)

class WriteDESDM_MSPerfStream(JobProperty):
    """ Produce the primary DESDM for Muon Alignment and Trigger commissioning."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESDM_MCP"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "PrimaryDPDMaker/PerfDESDM_MS.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDESDM_MSPerfStream)
listESDtoDPD.append(WriteDESDM_MSPerfStream.StreamName)

class WriteDESDM_TILEMUStream(JobProperty):
    """ Produce the primary DESDM for Tile-Muon trigger commissioning."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESDM_TILEMU"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "PrimaryDPDMaker/PerfDESDM_TILEMU.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDESDM_TILEMUStream)
listESDtoDPD.append(WriteDESDM_TILEMUStream.StreamName)

class WriteDESDM_PHOJETStream(JobProperty):
    """ Produce the primary DPD Photon-Jet DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESDM_PHOJET"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "PrimaryDPDMaker/PerfDPD_PhotonJet.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDESDM_PHOJETStream)
listESDtoDPD.append(WriteDESDM_PHOJETStream.StreamName)

class WriteDESDM_SGLELStream(JobProperty):
    """ Produce the primary DPD single electon DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESDM_SGLEL"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "PrimaryDPDMaker/PerfDPD_SGLEL.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDESDM_SGLELStream)
listESDtoDPD.append(WriteDESDM_SGLELStream.StreamName)

class WriteDESDM_SLTTMUStream(JobProperty):
    """ Produce the primary DPD Ttbar Muon DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESDM_SLTTMU"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "PrimaryDPDMaker/PerfDPD_TtbarMuon.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDESDM_SLTTMUStream)
listESDtoDPD.append(WriteDESDM_SLTTMUStream.StreamName)

class WriteDESDM_CALJETStream(JobProperty):
    """ Produce the primary DPD Jet DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESDM_CALJET"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "PrimaryDPDMaker/PerfDPD_Jet.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDESDM_CALJETStream)
listESDtoDPD.append(WriteDESDM_CALJETStream.StreamName)

class WriteDESDM_EXOTHIPStream(JobProperty):
    """ Produce the primary DPD for Highly Ionizing Particles analysis."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESDM_EXOTHIP"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "LongLivedParticleDPDMaker/PhysDPD_HIPs.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDESDM_EXOTHIPStream)
listESDtoDPD.append(WriteDESDM_EXOTHIPStream.StreamName)

##---------------
## ByteStream DPDs, skimming (event selection) ONLY!
##---------------

class WriteRAWPhysDPD_RPVLL(JobProperty):
    """ Produce the RPVLL in Byte Stream format."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDRAW_RPVLL"
    FileName       = ""
    Prescale       = 1
    isVirtual      = False
    DPDMakerScript = "LongLivedParticleDPDMaker/PhysDRAW_RPVLL.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteRAWPhysDPD_RPVLL)
listRAWtoDPD.append(WriteRAWPhysDPD_RPVLL.StreamName)

class WriteRAWPerfDPD_ZMUMU(JobProperty):
    """ Produce the primary DPD Zmumu in Byte Stream format."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDRAW_ZMUMU"
    FileName       = ""
    Prescale       = 1
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/DRAW_ZMUMU.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteRAWPerfDPD_ZMUMU)
listRAWtoDPD.append(WriteRAWPerfDPD_ZMUMU.StreamName)

class WriteDRAW_EGZ(JobProperty):
    """ Produce the DRAW for EGamma calibration in Z events."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDRAW_EGZ"
    FileName       = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/DRAW_EGZ.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDRAW_EGZ)
listRAWtoDPD.append(WriteDRAW_EGZ.StreamName)

class WriteDRAW_EGJPSI(JobProperty):
    """ Produce the DRAW for EGamma calibration in JPSI events."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDRAW_EGJPSI"
    FileName       = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/DRAW_EGJPSI.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDRAW_EGJPSI)
listRAWtoDPD.append(WriteDRAW_EGJPSI.StreamName)

class WriteDRAW_TAUMUH(JobProperty):
    """ Produce the DRAW for TAUWG Z->tau(mu)tau(had) T&P."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDRAW_TAUMUH"
    FileName       = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/DRAW_TAUMUH.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDRAW_TAUMUH)
listRAWtoDPD.append(WriteDRAW_TAUMUH.StreamName)

class WriteDRAW_EMU(JobProperty):
    """ Produce the primary DPD e-mu in Byte Stream format."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDRAW_EMU"
    FileName       = ""
    Prescale       = 1
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/DRAW_EMU.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDRAW_EMU)
listRAWtoDPD.append(WriteDRAW_EMU.StreamName)

class WriteDRAW_BCID1(JobProperty):
    """ Produce the BCID-filtered DRAW, based on the AcceptBCIDs and RejectBCIDs lists."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDRAW_BCID1"
    FileName       = ""
    Prescale       = 1
    AcceptBCIDs    = []
    RejectBCIDs    = []
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/DRAW_BCID1.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDRAW_BCID1)
listRAWtoDPD.append(WriteDRAW_BCID1.StreamName)

class WriteDRAW_BCID2(JobProperty):
    """ Produce the BCID-filtered DRAW, based on the AcceptBCIDs and RejectBCIDs lists."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDRAW_BCID2"
    FileName       = ""
    Prescale       = 1
    AcceptBCIDs    = []
    RejectBCIDs    = []
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/DRAW_BCID2.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDRAW_BCID2)
listRAWtoDPD.append(WriteDRAW_BCID2.StreamName)

class WriteDRAW_BCID3(JobProperty):
    """ Produce the BCID-filtered DRAW, based on the AcceptBCIDs and RejectBCIDs lists."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDRAW_BCID3"
    FileName       = ""
    Prescale       = 1
    AcceptBCIDs    = []
    RejectBCIDs    = []
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/DRAW_BCID3.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDRAW_BCID3)
listRAWtoDPD.append(WriteDRAW_BCID3.StreamName)


class WriteDRAW_BCID4(JobProperty):
    """ Produce the BCID-filtered DRAW, based on the AcceptBCIDs and RejectBCIDs lists."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDRAW_BCID4"
    FileName       = ""
    Prescale       = 1
    AcceptBCIDs    = []
    RejectBCIDs    = []
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/DRAW_BCID4.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDRAW_BCID4)
listRAWtoDPD.append(WriteDRAW_BCID4.StreamName)

class WriteDRAW_TOPSLMU(JobProperty):
    """ Produce DRAW_TOPSLMU, a top-like single-muon selection for tau embedding used by H+ searches."""
    statusOn       = True    
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDRAW_TOPSLMU"
    FileName       = ""
    Prescale       = 1
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/DRAW_TOPSLMU.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDRAW_TOPSLMU)
listRAWtoDPD.append(WriteDRAW_TOPSLMU.StreamName)

##--------------------------------------------
## Skimmed ESD
##--------------------------------------------

class WriteDAOD_RPVLLStream(JobProperty):
    """ Produce the xAOD for DPD RPVLL and UEH groups searches """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_RPVLL"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "LongLivedParticleDPDMaker/DAOD_RPVLL.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_RPVLLStream)
listESDtoDPD.append(WriteDAOD_RPVLLStream.StreamName)

class WriteDAOD_IDNCBStream(JobProperty):
    """ Produce the DPD for DAOD_IDNCB - AOD with PrepRawData """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_IDNCB"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/DAOD_IDNCB.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_IDNCBStream)
listRAWtoDPD.append(WriteDAOD_IDNCBStream.StreamName)

class WriteDAOD_IDTRKVALIDStream(JobProperty):
    """ Produce the DPD for DAOD_IDTRKVALID - AOD with PrepRawData """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_IDTRKVALID"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "InDetPrepRawDataToxAOD/InDetDxAOD.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_IDTRKVALIDStream)
listESDtoDPD.append(WriteDAOD_IDTRKVALIDStream.StreamName)

class WriteDAOD_SCTVALIDStream(JobProperty):
    """ Produce the DPD for DAOD_SCTVALID - AOD with PrepRawData """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_SCTVALID"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "InDetPrepRawDataToxAOD/SCTxAOD.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_SCTVALIDStream)
listRAWtoDPD.append(WriteDAOD_SCTVALIDStream.StreamName)

class WriteDAOD_IDTIDEStream(JobProperty):
    """ Produce the DPD for DAOD_IDTIDE - AOD with PrepRawData """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_IDTIDE"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkInDet/IDTIDE1.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_IDTIDEStream)
listESDtoDPD.append(WriteDAOD_IDTIDEStream.StreamName)

class WriteDAOD_IDTRKLUMIStream(JobProperty):
    """ Produce the DPD for DAOD_IDTRKLUMI - AOD with PrepRawData """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_IDTRKLUMI"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/DAOD_IDTRKLUMI.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_IDTRKLUMIStream)
listESDtoDPD.append(WriteDAOD_IDTRKLUMIStream.StreamName)

class WriteDAOD_IDPIXLUMIStream(JobProperty):
    """ Produce the DPD for DAOD_IDPIXLUMI - AOD with PrepRawData """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_IDPIXLUMI"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/DAOD_IDPIXLUMI.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_IDPIXLUMIStream)
listESDtoDPD.append(WriteDAOD_IDPIXLUMIStream.StreamName)


class WriteDAOD_L1CALO1(JobProperty):
    """ Produce the DAOD for L1Calo1."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDAOD_L1CALO1"
    FileName       = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkL1Calo/L1CALO1.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_L1CALO1)
listESDtoDPD.append(WriteDAOD_L1CALO1.StreamName)

class WriteDAOD_L1CALO2(JobProperty):
    """ Produce the DAOD for L1Calo2."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDAOD_L1CALO2"
    FileName       = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkL1Calo/L1CALO2.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_L1CALO2)
listESDtoDPD.append(WriteDAOD_L1CALO2.StreamName)

class WriteDAOD_L1CALO3(JobProperty):
    """ Produce the DAOD for L1Calo3."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDAOD_L1CALO3"
    FileName       = ""
    isVirtual      = False
    DPDMakerScript = "DerivationFrameworkL1Calo/L1CALO3.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_L1CALO3)
listESDtoDPD.append(WriteDAOD_L1CALO3.StreamName)

class WriteDESD_DEDX(JobProperty):
    """ Produce pixel dE/dx ESD """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_DEDX"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/DESD_DEDX.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDESD_DEDX)
listESDtoDPD.append(WriteDESD_DEDX.StreamName)


##--------------------------------------
## Turn on/off skimming/thinning/slimming
##--------------------------------------
class ApplySkimming(JobProperty):
    """ Apply the skimming algorithms (event selections)."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(ApplySkimming)

class ApplySlimming(JobProperty):
    """ Apply the slimming algorithms (removing of parts of objects)."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(ApplySlimming)

class ApplyThinning(JobProperty):
    """ Apply the thinning algorithms (removing objects)."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(ApplyThinning)

##--------------------------------
## Switches that contol all DPDs
##--------------------------------
class WritePrimaryDPDTrigger(JobProperty):
    """ Write trigger information into the output."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePrimaryDPDTrigger)

class UseMCTruth(JobProperty):
    """ Write MC truth information in the output."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(UseMCTruth)

class SetupTrigDecisionTool(JobProperty):
    """ Include joboptions necessary to configure services for TrigDecisionTool."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(SetupTrigDecisionTool)

class WriteSkimDecisions(JobProperty):
    """ Writes persistent SkimDecision objects for all algorithms in output pool files."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteSkimDecisions)


try:
    # This is the new way, in 15.4.0
    from RecExConfig.RecoFunctions import AddValidItemToList
    pass
except ImportError :
    # This is here for backwards compatibility
    from RecExCommon.RecoFunctions import AddValidItemToList
    pass

#Add D2PDMakers too
#try:
#    from D2PDMaker.D2PDFlags import listAODtoD2PD,listESDtoD2PD
#    AddValidItemToList(listAODtoD2PD,listAODtoDPD)
#    AddValidItemToList(listESDtoD2PD,listESDtoDPD)
#except:
#    print "Unable to import listAODtoD2PD. This requires D2PDMaker-00-00-55-08 or D2PDMaker-00-00-62"

#Now build the list of all known DPDs
AddValidItemToList(listRAWtoDPD,listAllKnownDPD)
AddValidItemToList(listESDtoDPD,listAllKnownDPD)
AddValidItemToList(listAODtoDPD,listAllKnownDPD)

# The lists below are only for RecExCommon's auto-configuration backward compatibility when reading old DPDs.
# Please don't use these obsolete names to write new DPDs!
listBackwardCompatibleAODtoDPD=['D1PD_SGLEL','D1PD_SGLPH','D1PD_SGLMU','D1PD_SGLTAU','D1PD_ELEL',
                                'D1PD_MUMU','D1PD_ELMU','D1PD_ELTAU','D1PD_MUTAU','D1PD_ELMET',
                                'D1PD_MUMET','D1PD_JETMET','D1PD_JETEL','D1PD_JETMU','D1PD_JETTAU','AOD_SKIMEL',
                                'DAODM_SGLEL','DAODM_SGLPH','DAODM_SGLMU','DAODM_SGLTAU','DAOD_2EL','DAOD_2PH',
                                'DAOD_2MU','DAOD_ELMU','DAOD_ELTAU','DAOD_MUTAU','DAOD_ELMET','DAOD_MUMET',
                                'DAOD_JETMET','DAOD_ELJET','DAOD_MUJET','DAOD_TAUJET','DAOD_SKIMEL',
                                'DAOD_EGLOOSE','DAOD_MU','DAOD_ELLOOSE','DAOD_ONIAMUMU',
                                'StreamDAODM_SGLEL','StreamDAOD_2EL','StreamDAOD_ELMU','StreamDAOD_ELTAU',
                                'StreamDAOD_ELMET','StreamDAOD_ELJET','DAOD_ONIAMUMUHI','D2AOD_DIONIA']


listBackwardCompatibleESDtoDPD=['PERFD1PD_EGAMMA','PERFD1PD_PHOJET','PERFD1PD_SGLEL','PERFD1PD_MUON','PERFD1PD_SGLMU',
                                'PERFD1PD_CALJET','PERFD1PD_TRACK','PERFD1PD_MET','PERFD1PD_MBIAS','DPD_PIXELCOMM',
                                'DPD_SCTCOMM','DPD_IDCOMM','DPD_IDPROJCOMM','DPD_CALOCOMM','DPD_TILECOMM',
                                'DPD_EMCLUSTCOMM','DPD_EGAMTAUCOMM','DPD_RPCCOMM','DPD_TGCCOMM','DPD_MUONCOMM',
                                'DPD_RANDOMCOMM','ESD_SKIMEL','ESD_PRESCALED','DESDM_EGAMMA','DESD_PHOJET',
                                'DESD_SGLEL','DESDM_MUON','DESD_SGLMU','DESDM_CALJET','DESDM_TRACK','DESD_MET',
                                'DESD_MBIAS','DESD_PRESCALED','DESD_COLLCAND','DESD_PIXELCOMM','DESD_SCTCOMM',
                                'DESD_IDCOMM','DESD_IDPROJCOMM','DESD_CALOCOMM','DESD_TILECOMM','DESD_EMCLUSCOMM',
                                'DESD_EGTAUCOMM','DESD_RPCCOMM','DESD_TGCCOMM','DESD_MUONCOMM','DESD_RANDOMCOMM',
                                'DESD_SKIMEL','DESD_CTLFWJET','DESD_FWJET']

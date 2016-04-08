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

class WriteDESD_HIPsStream(JobProperty):
    """ Produce the primary DPD for Highly Ionizing Particles analysis."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_HIPs"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "LongLivedParticleDPDMaker/PhysDPD_HIPs.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDESD_HIPsStream)
listESDtoDPD.append(WriteDESD_HIPsStream.StreamName)


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
    StreamName   = "StreamDESDM_MSPerf"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "PrimaryDPDMaker/PerfDESDM_MS.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDESDM_MSPerfStream)
listESDtoDPD.append(WriteDESDM_MSPerfStream.StreamName)

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

class WriteSingleElectronStream(JobProperty):
    """ Produce the primary DPD Missing Et DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_SGLEL"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "PrimaryDPDMaker/PerfDPD_SingleElectron.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteSingleElectronStream)
listESDtoDPD.append(WriteSingleElectronStream.StreamName)

class WriteMuonStream(JobProperty):
    """ Produce the primary DPD Muon DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESDM_MUON"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "PrimaryDPDMaker/PerfDPD_Muon.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteMuonStream)
listESDtoDPD.append(WriteMuonStream.StreamName)

class WriteSingleMuonStream(JobProperty):
    """ Produce the primary DPD Tight Muon DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_SGLMU"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "PrimaryDPDMaker/PerfDPD_SingleMuon.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteSingleMuonStream)
listESDtoDPD.append(WriteSingleMuonStream.StreamName)

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

class WriteTauMuonStream(JobProperty):
   """ Produce the primary DPD Tau+Muon DPD."""
   statusOn     = True
   allowedTypes = ['bool']
   StoredValue  = False
   StreamName   = "StreamDESD_TAUMUH"
   FileName     = ""
   isVirtual    = False
   DPDMakerScript = "PrimaryDPDMaker/PerfDPD_TauMuon.py"
   pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteTauMuonStream)
listESDtoDPD.append(WriteTauMuonStream.StreamName)

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

class WriteTrackingStream(JobProperty):
    """ Produce the primary DPD Tracking."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESDM_TRACK"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "PrimaryDPDMaker/PerfDPD_Tracking.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteTrackingStream)
listESDtoDPD.append(WriteTrackingStream.StreamName)

class WriteLargeMetStream(JobProperty):
    """ Produce the primary DPD LargeMet DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESDM_MET"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "PrimaryDPDMaker/PerfDPD_LargeMet.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteLargeMetStream)
listESDtoDPD.append(WriteLargeMetStream.StreamName)

class WriteMinBiasStream(JobProperty):
    """ Produce the primary DPD Minimum Bias DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_MBIAS"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "PrimaryDPDMaker/PerfDPD_MinBias.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteMinBiasStream)
listESDtoDPD.append(WriteMinBiasStream.StreamName)

class WritePrescaledESDStream(JobProperty):
    """ Produce the primary DPD PrescaledESD DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_PRESCALED"
    FileName     = ""
    isVirtual    = False
    DPDMakerScript = "PrimaryDPDMaker/PerfDPD_PrescaledESD.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePrescaledESDStream)
listESDtoDPD.append(WritePrescaledESDStream.StreamName)

class WriteDESD_CollCandStream(JobProperty):
    """ Produce the primary DESD_COLLCAND DPD."""
    statusOn          = True
    allowedTypes      = ['bool']
    StoredValue       = False
    StreamName        = "StreamDESD_COLLCAND"
    FileName          = ""
    TimeDiffCut       = 5.0
    MinCellsPerSide   = 2
    ChargeThreshold   = 60.0/222.0
    MinHitsPerSide    = 2
    MaxTimeDifference = 10.0
    Trigger           = "L1_MBTS_1_1"
    isVirtual         = False
    DPDMakerScript    = "PrimaryDPDMaker/DESD_GOODTIME.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDESD_CollCandStream)
listESDtoDPD.append(WriteDESD_CollCandStream.StreamName)


class WriteD2ESD_WENUStream(JobProperty):
    """ Produce the primary DPD W -> e nu DPD."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamD2ESD_WENU"
    FileName       = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/DESD_WENU.py"
    Prescale       = 1
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteD2ESD_WENUStream)
listESDtoDPD.append(WriteD2ESD_WENUStream.StreamName)

class WriteD2ESD_WMUNUStream(JobProperty):
    """ Produce the primary DPD W -> mu nu DPD."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamD2ESD_WMUNU"
    FileName       = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/DESD_WMUNU.py"
    Prescale       = 1
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteD2ESD_WMUNUStream)
listESDtoDPD.append(WriteD2ESD_WMUNUStream.StreamName)


class WriteDiPhotonStream(JobProperty):
    """ Produce the primary DPD Di Photon DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamD2ESD_DIPHO"
    FileName     = ""
    DPDMakerScript = "PrimaryDPDMaker/D2ESD_DiPhoton.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDiPhotonStream)
listESDtoDPD.append(WriteDiPhotonStream.StreamName)

class WriteD2AOD_DIPHOStream(JobProperty):
    """ Produce the primary DPD Di Photon DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamD2AOD_DIPHO"
    FileName     = ""
    DPDMakerScript = "PrimaryDPDMaker/D2AOD_DiPhoton.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteD2AOD_DIPHOStream)
listAODtoDPD.append(WriteD2AOD_DIPHOStream.StreamName)

class WriteDAOD_ZEEGAMMAStream(JobProperty):
    """ Produce the primary DPD Di Electron Photon DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_ZEEGAMMA"
    FileName     = ""
    DPDMakerScript = "PrimaryDPDMaker/DAOD_ZEEGAMMA.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_ZEEGAMMAStream)
listAODtoDPD.append(WriteDAOD_ZEEGAMMAStream.StreamName)

class WriteDAOD_ZMUMUGAMMAStream(JobProperty):
    """ Produce the DPD for DPD Di Muon Photon DPD """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_ZMUMUGAMMA"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/DAOD_ZMUMUGAMMA.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_ZMUMUGAMMAStream)
listAODtoDPD.append(WriteDAOD_ZMUMUGAMMAStream.StreamName)

##--------------------------------------------
###### Commissioning DPD
class WritePixelCommStream(JobProperty):
    """ Produce the primary DPD for Pixel commissioning."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_PIXELCOMM"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/CommDPD_Pixel.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePixelCommStream)
listESDtoDPD.append(WritePixelCommStream.StreamName)

class WriteSCTCommStream(JobProperty):
    """ Produce the primary DPD for SCT commissioning."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_SCTCOMM"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/CommDPD_SCT.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteSCTCommStream)
listESDtoDPD.append(WriteSCTCommStream.StreamName)

class WriteIDCommStream(JobProperty):
    """ Produce the primary DPD for ID commissioning."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_IDCOMM"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/CommDPD_ID.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteIDCommStream)
listESDtoDPD.append(WriteIDCommStream.StreamName)

class WriteIDProjCommStream(JobProperty):
    """ Produce the primary DPD for IDProj commissioning."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_IDPROJCOMM"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/CommDPD_IDProj.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteIDProjCommStream)
listESDtoDPD.append(WriteIDProjCommStream.StreamName)

class WriteCaloCommStream(JobProperty):
    """ Produce the primary DPD for Calo commissioning."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_CALOCOMM"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/CommDPD_Calo.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteCaloCommStream)
listESDtoDPD.append(WriteCaloCommStream.StreamName)

class WriteTileCommStream(JobProperty):
    """ Produce the primary DPD for Tile commissioning."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_TILECOMM"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/CommDPD_Tile.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteTileCommStream)
listESDtoDPD.append(WriteTileCommStream.StreamName)

class WriteEMClustCommStream(JobProperty):
    """ Produce the primary DPD for EMClust commissioning."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_EMCLUSCOMM"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/CommDPD_EMCluster.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteEMClustCommStream)
listESDtoDPD.append(WriteEMClustCommStream.StreamName)

class WriteEGamTauCommStream(JobProperty):
    """ Produce the primary DPD for EGam and Tau commissioning."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_EGTAUCOMM"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/CommDPD_EGamTau.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteEGamTauCommStream)
listESDtoDPD.append(WriteEGamTauCommStream.StreamName)

class WriteRPCCommStream(JobProperty):
    """ Produce the primary DPD for RPC commissioning."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_RPCCOMM"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/CommDPD_RPC.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteRPCCommStream)
listESDtoDPD.append(WriteRPCCommStream.StreamName)

class WriteTGCCommStream(JobProperty):
    """ Produce the primary DPD for TGC commissioning."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_TGCCOMM"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/CommDPD_TGC.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteTGCCommStream)
listESDtoDPD.append(WriteTGCCommStream.StreamName)

class WriteMUONCommStream(JobProperty):
    """ Produce the primary DPD for MUON commissioning."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_MUONCOMM"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/CommDPD_MUON.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteMUONCommStream)
listESDtoDPD.append(WriteMUONCommStream.StreamName)

class WriteRandomCommStream(JobProperty):
    """ Produce the primary DPD for RNDM stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_RANDOMCOMM"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/CommDPD_Random.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteRandomCommStream)
listESDtoDPD.append(WriteRandomCommStream.StreamName)





##--------------------------------------
## DESDs for heavy-ion running of the LHC
##--------------------------------------
class WriteDESD_HIRAREStream(JobProperty):
    """ Produce the DESD for rare heavy-ion events."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = False
    StreamName         = "StreamDESD_HIRARE"
    FileName           = ""
    isVirtual          = False
    DPDMakerScript     = "PrimaryDPDMaker/DESD_HIRARE.py"
    TriggerNames       = [ 'EF_L1EM16_NoAlg' ]
    jetCollection      = "antikt4HI_TowerJets"
    jetMinEt           = 100.0*Units.GeV
    photonCollection   = "PhotonCollection"
    photonMinEt        = 40.0*Units.GeV
    electronCollection = "ElectronCollection"
    electronMinEt      = 40.0*Units.GeV
    trimContainers     = False
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDESD_HIRAREStream)
listESDtoDPD.append(WriteDESD_HIRAREStream.StreamName)





##--------------------------------------
## Primary Physics DPDs
##--------------------------------------
class WritePhysDPDElectron(JobProperty):
    """ Produce the primary physics DPD  thin Electron DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAODM_SGLEM"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    TriggerNames = ["EF_e10_loose", "EF_e10_medium", "EF_e15_loose", "EF_e20_loose", "EF_2e3_loose", "EF_2e5_medium", "EF_g10_loose", "EF_g20_loose", "EF_2g5_loose", "EF_2g10_loose", "EF_2g20_loose"]
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePhysDPDElectron)
listAODtoDPD.append(WritePhysDPDElectron.StreamName)
listDPDtoTAG.append(WritePhysDPDElectron.StreamName)

class WritePhysDPDPhoton(JobProperty):
    """ Produce the primary physics DPD  thin Photon DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAODM_SGLPH"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    TriggerNames = ["EF_g10_loose", "EF_g20_loose", "EF_g20i_loose", "EF_g20_loose_passHTL", "EF_2g10_loose", "EF_2g20_loose"]
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePhysDPDPhoton)
listAODtoDPD.append(WritePhysDPDPhoton.StreamName)

class WritePhysDPDMuon(JobProperty):
    """ Produce the primary physics DPD  thin Muon DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAODM_SGLMU"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    TriggerNames = ["EF_mu4", "EF_mu4_MSonly", "EF_mu6", "EF_mu6_MSonly", "EF_mu10", "EF_mu10_MSonly", "EF_mu15", "EF_mu20", "EF_2mu4", "EF_2mu6"]
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePhysDPDMuon)
listAODtoDPD.append(WritePhysDPDMuon.StreamName)
listDPDtoTAG.append(WritePhysDPDMuon.StreamName)

class WritePhysDPDTau(JobProperty):
    """ Produce the primary physics DPD  thin Tau DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAODM_SGLTAU"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    TriggerNames = ["EF_tau50_loose", "EF_tau16i_loose", "EF_tau16i_xe30", "EF_tau16i_EFxe30", "EF_tau16i_j70", "EF_tau16i_e10", "EF_tau16i_mu10", "EF_2tau20i_loose"]
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePhysDPDTau)
listAODtoDPD.append(WritePhysDPDTau.StreamName)
listDPDtoTAG.append(WritePhysDPDTau.StreamName)

class WritePhysDPDDiElectron(JobProperty):
    """ Produce the primary physics DPD  skimmed DiElectron DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_2EM"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    ElectronIsEM = "ElectronLoose"
    ElectronEt   = 10.0*Units.GeV
    ElectronEta  = 5.0
    PhotonIsEM   = "PhotonLoose"
    PhotonAuthor = ""
    PhotonEt     = 10.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePhysDPDDiElectron)
listAODtoDPD.append(WritePhysDPDDiElectron.StreamName)
listDPDtoTAG.append(WritePhysDPDDiElectron.StreamName)

class WritePhysDPDDiPhoton(JobProperty):
    """ Produce the primary physics DPD  skimmed DiPhoton DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_2PH"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    PhotonIsEM   = "PhotonLoose"
    PhotonEt     = 15.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePhysDPDDiPhoton)
listAODtoDPD.append(WritePhysDPDDiPhoton.StreamName)

class WritePhysDPDDiMuon(JobProperty):
    """ Produce the primary physics DPD  skimmed DiMuon DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_2MU"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    MuonPt       = 10.0*Units.GeV
    MuonEta      = 2.5
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePhysDPDDiMuon)
listAODtoDPD.append(WritePhysDPDDiMuon.StreamName)
listDPDtoTAG.append(WritePhysDPDDiMuon.StreamName)

class WritePhysDPDElectronMuon(JobProperty):
    """ Produce the primary physics DPD  skimmed Electron/Muon DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_EMMU"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    ElectronEt   = 10.0*Units.GeV
    ElectronEta  = 5.0
    ElectronIsEM = "ElectronLoose"
    PhotonIsEM   = "PhotonLoose"
    PhotonEt     = 10.0*Units.GeV
    MuonPt       = 10.0*Units.GeV
    MuonEta      = 2.5
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePhysDPDElectronMuon)
listAODtoDPD.append(WritePhysDPDElectronMuon.StreamName)
listDPDtoTAG.append(WritePhysDPDElectronMuon.StreamName)

class WritePhysDPDElectronTau(JobProperty):
    """ Produce the primary physics DPD  skimmed Electron/Tau DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_EMTAU"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    ElectronIsEM = "ElectronLoose"
    ElectronEt   = 10.0*Units.GeV
    ElectronEta  = 5.0
    PhotonIsEM   = "PhotonLoose"
    PhotonEt     = 10.0*Units.GeV
    TauEt        = 15.0*Units.GeV
    TauId        = 0
    TauPreselection = False
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePhysDPDElectronTau)
listAODtoDPD.append(WritePhysDPDElectronTau.StreamName)
listDPDtoTAG.append(WritePhysDPDElectronTau.StreamName)

class WritePhysDPDMuonTau(JobProperty):
    """ Produce the primary physics DPD  skimmed Muon/Tau DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_MUTAU"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    MuonPt       = 10.0*Units.GeV
    MuonEta      = 2.5
    TauEt        = 15.0*Units.GeV
    TauId        = 0
    TauPreselection = False
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePhysDPDMuonTau)
listAODtoDPD.append(WritePhysDPDMuonTau.StreamName)
listDPDtoTAG.append(WritePhysDPDMuonTau.StreamName)

class WritePhysDPDElectronMET(JobProperty):
    """ Produce the primary physics DPD  skimmed Electron/MET DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_EMMET"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    ElectronIsEM = "Loose"
    ElectronEt   = 10.0*Units.GeV
    ElectronEta  = 5.0
    PhotonIsEM   = "PhotonLoose"
    PhotonEt     = 15.0*Units.GeV
    MissingEt    = 20.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePhysDPDElectronMET)
listAODtoDPD.append(WritePhysDPDElectronMET.StreamName)
listDPDtoTAG.append(WritePhysDPDElectronMET.StreamName)

class WritePhysDPDMuonMET(JobProperty):
    """ Produce the primary physics DPD  skimmed Muon/MET DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_MUMET"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    MuonPt       = 15.0*Units.GeV
    MuonEta      = 2.5
    MissingEt    = 20.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePhysDPDMuonMET)
listAODtoDPD.append(WritePhysDPDMuonMET.StreamName)
listDPDtoTAG.append(WritePhysDPDMuonMET.StreamName)

class WritePhysDPDJetMET(JobProperty):
    """ Produce the primary physics DPD  skimmed Jet/MET DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_JETMET"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    JetCollectionNames     = ["Cone4TowerJets", "Cone4TopoJets", "Cone7TowerJets", "AntiKt4TopoJets", "AntiKt4TowerJets", "AntiKt6TowerJets"]
    JetEt                  = 20.0*Units.GeV
    RemoveOverlap          = False
    DeltaR                 = 0.2
    JetNumber              = 2
    RequireMet             = True
    MissingEt              = 20.0*Units.GeV
    METMinDeltaPhi         = 0.4
    METUseLeadingJet       = True
    METUseUncalibratedJets = True
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePhysDPDJetMET)
listAODtoDPD.append(WritePhysDPDJetMET.StreamName)
listDPDtoTAG.append(WritePhysDPDJetMET.StreamName)

class WritePhysDPDJetElectron(JobProperty):
    """ Produce the primary physics DPD skimmed Jet plus Electron DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_EMJET"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    ElectronIsEM = "ElectronLoose"
    ElectronEt   = 10.0*Units.GeV
    ElectronEta  = 5.0
    PhotonIsEM   = "PhotonLoose"
    PhotonEt     = 10.0*Units.GeV
    JetCollectionNames = ["Cone4TowerJets", "Cone4TopoJets", "Cone7TowerJets", "AntiKt4TopoJets", "AntiKt4TowerJets", "AntiKt6TowerJets"]
    JetEt         = 20.0*Units.GeV
    RemoveOverlap = True
    DeltaR        = 0.2
    JetNumber     = 2
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePhysDPDJetElectron)
listAODtoDPD.append(WritePhysDPDJetElectron.StreamName)
listDPDtoTAG.append(WritePhysDPDJetElectron.StreamName)

class WritePhysDPDJetMuon(JobProperty):
    """ Produce the primary physics DPD skimmed Jet plus Muon DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_MUJET"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    MuonPt       = 10.0*Units.GeV
    MuonEta      = 2.5
    JetCollectionNames = ["Cone4TowerJets", "Cone4TopoJets", "Cone7TowerJets", "AntiKt4TopoJets", "AntiKt4TowerJets", "AntiKt6TowerJets"]
    JetEt         = 20.0*Units.GeV
    RemoveOverlap = True
    DeltaR        = 0.2
    JetNumber     = 2
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePhysDPDJetMuon)
listAODtoDPD.append(WritePhysDPDJetMuon.StreamName)
listDPDtoTAG.append(WritePhysDPDJetMuon.StreamName)

class WritePhysDPDJetTau(JobProperty):
    """ Produce the primary physics DPD skimmed Jet plus Tau DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_TAUJET"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    TauEt        = 15.0*Units.GeV
    TauId        = 0
    TauPreselection = False
    JetCollectionNames = ["Cone4TowerJets", "Cone4TopoJets", "Cone7TowerJets", "AntiKt4TopoJets", "AntiKt4TowerJets", "AntiKt6TowerJets"]
    JetEt         = 20.0*Units.GeV
    RemoveOverlap = True
    DeltaR        = 0.2
    JetNumber     = 2
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePhysDPDJetTau)
listAODtoDPD.append(WritePhysDPDJetTau.StreamName)
listDPDtoTAG.append(WritePhysDPDJetTau.StreamName)


##--------------
## Preselection DPDs
##--------------
class WritePreselectElectronStreamFromESDInput(JobProperty):
    """ Produce the preselection DPD for Electrons."""
    statusOn        = True
    allowedTypes    = ['bool']
    StoredValue     = False
    writeAODContent = False
    writeESDContent = False
    StreamName      = "StreamDESD_SKIMEL"
    FileName        = ""
    isVirtual       = False
    DPDMakerScript  = "PrimaryDPDMaker/PreselDPD_Electron.py"
    ElectronIsEM    = "Loose"
    ElectronEt      = 0.0
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePreselectElectronStreamFromESDInput)
listESDtoDPD.append(WritePreselectElectronStreamFromESDInput.StreamName)

class WritePreselectElectronStreamFromAODInput(JobProperty):
    """ Produce the preselection DPD for Electrons."""
    statusOn        = True
    allowedTypes    = ['bool']
    StoredValue     = False
    StreamName      = "StreamDAOD_SKIMEL"
    FileName        = ""
    isVirtual       = False
    DPDMakerScript  = "PrimaryDPDMaker/aodtodpd.py"
    ElectronIsEM    = "Loose"
    ElectronEt      = 0.0
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WritePreselectElectronStreamFromAODInput)
listAODtoDPD.append(WritePreselectElectronStreamFromAODInput.StreamName)





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


class WriteRAWPerfDPD_ZEE(JobProperty):
    """ Produce the primary DPD Zee in Byte Stream format."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDRAW_ZEE"
    FileName       = ""
    Prescale       = 1
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/RAWPerfDPD_Zee.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteRAWPerfDPD_ZEE)
listRAWtoDPD.append(WriteRAWPerfDPD_ZEE.StreamName)

class WriteRAWPerfDPD_WENU(JobProperty):
    """ Produce the primary DPD W->enu in Byte Stream format."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDRAW_WENU"
    FileName       = ""
    Prescale       = 1
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/RAWPerfDPD_Wenu.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteRAWPerfDPD_WENU)
listRAWtoDPD.append(WriteRAWPerfDPD_WENU.StreamName)

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

class WriteRAWPerfDPD_WMUNU(JobProperty):
    """ Produce the primary DPD W->enu in Byte Stream format."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDRAW_WMUNU"
    FileName       = ""
    Prescale       = 1
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/RAWPerfDPD_Wmunu.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteRAWPerfDPD_WMUNU)
listRAWtoDPD.append(WriteRAWPerfDPD_WMUNU.StreamName)

class WriteRAWCommDPD_IDPROJ(JobProperty):
    """ Produce the primary DPD prescaled bytestream."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDRAW_IDPROJCOMM"
    FileName       = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/RAWCommDPD_IDProj.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteRAWCommDPD_IDPROJ)
listRAWtoDPD.append(WriteRAWCommDPD_IDPROJ.StreamName)

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

##--------------------------------------------
## Skimmed AOD
##--------------------------------------------

class WriteDAOD_EGLOOSEStream(JobProperty):
    """ Produce the DPD for DAOD_EGLOOSE """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_EGLOOSE"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_EGLOOSEStream)
listAODtoDPD.append(WriteDAOD_EGLOOSEStream.StreamName)

class WriteDAOD_MUStream(JobProperty):
    """ Produce the DPD for DAOD_MU """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_MU"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_MUStream)
listAODtoDPD.append(WriteDAOD_MUStream.StreamName)

class WriteDAOD_ELLOOSEStream(JobProperty):
    """ Produce the DPD for DAOD_ELLOOSE """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_ELLOOSE"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_ELLOOSEStream)
listAODtoDPD.append(WriteDAOD_ELLOOSEStream.StreamName)

class WriteDAOD_PHOTONLOOSEStream(JobProperty):
    """ Produce the DPD for DAOD_PHLOOSE """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_PHLOOSE"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_PHOTONLOOSEStream)
listAODtoDPD.append(WriteDAOD_PHOTONLOOSEStream.StreamName)

class WriteDAOD_ONIAMUMUStream(JobProperty):
    """ Produce the DPD for DAOD_ONIAMUMU """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_ONIAMUMU"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "BPhysDAOD/BPhysAODtoDAODOptions.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_ONIAMUMUStream)
listAODtoDPD.append(WriteDAOD_ONIAMUMUStream.StreamName)

class WriteDAOD_JPSIMUMUStream(JobProperty):
    """ Produce the DPD for DAOD_JPSIMUMU """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_JPSIMUMU"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "BPhysDAOD/BPhysAODtoDAODJPSIOptions.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_JPSIMUMUStream)
listAODtoDPD.append(WriteDAOD_JPSIMUMUStream.StreamName)

class WriteDAOD_UPSIMUMUStream(JobProperty):
    """ Produce the DPD for DAOD_UPSIMUMU """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_UPSIMUMU"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "BPhysDAOD/BPhysAODtoDAODUPSIOptions.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDAOD_UPSIMUMUStream)
listAODtoDPD.append(WriteDAOD_UPSIMUMUStream.StreamName)

class WriteD2AOD_DIONIAStream(JobProperty):
    """ Produce the DPD for D2AOD_DIONIA """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamD2AOD_DIONIA"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "BPhysDAOD/BPhysDAODtoD2AODOptions.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteD2AOD_DIONIAStream)
listAODtoDPD.append(WriteD2AOD_DIONIAStream.StreamName)

class WriteDESD_ONIAMUMUHIStream(JobProperty):
    """ Produce the DPD for DESD_ONIAMUMUHI """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_ONIAMUMUHI"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "BPhysDAOD/BPhysESDtoDESDOptionsHI.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDESD_ONIAMUMUHIStream)
listESDtoDPD.append(WriteDESD_ONIAMUMUHIStream.StreamName)


try:
    from LongLivedParticleDPDMaker.LongLivedDPDFlags import *
except(ImportError):
    pass

##--------------------------------------------
## Skimmed ESD
##--------------------------------------------

class WriteDESD_CTLFWJETStream(JobProperty):
    """ Produce the DPD for DESD_CTLFWJET """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_CTLFWJET"
    FileName     = ""
    isVirtual      = False
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDESD_CTLFWJETStream)
listESDtoDPD.append(WriteDESD_CTLFWJETStream.StreamName)

class WriteDESD_FWJETStream(JobProperty):
    """ Produce the DPD for DESD_FWJET """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESD_FWJET"
    FileName     = ""
    isVirtual      = False
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDESD_FWJETStream)
listESDtoDPD.append(WriteDESD_FWJETStream.StreamName)

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

class WriteD2ESD_ZMMStream(JobProperty):
    """ Produce the Z->mumu D2ESD."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamD2ESD_ZMM"
    FileName       = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/D2ESD_ZMM.py"
    Prescale       = 1
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteD2ESD_ZMMStream)
listESDtoDPD.append(WriteD2ESD_ZMMStream.StreamName)

class WriteD2ESD_JPSIUPSMMStream(JobProperty):
    """ Produce the J/Psi or Upsilon -> mumu D2ESD."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamD2ESD_JPSIUPSMM"
    FileName       = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/D2ESD_JPSIUPSMM.py"
    Prescale       = 1
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteD2ESD_JPSIUPSMMStream)
listESDtoDPD.append(WriteD2ESD_JPSIUPSMMStream.StreamName)

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



##--------------------------------------------
## TestDPD
##--------------------------------------------
class WriteTestDPDStream(JobProperty):
    """ Produce the DPD for testing """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDAOD_TEST"
    FileName     = ""
    isVirtual      = False
    DPDMakerScript = "PrimaryDPDMaker/aodtodpd.py"
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteTestDPDStream)
listAODtoDPD.append(WriteTestDPDStream.StreamName)



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

##--------------------------------
## Individual slimming options - this will need to be moved to the individual stream definitions!
##--------------------------------
class ApplyJetSlimming(JobProperty):
    """ Apply the slimming of jets (removing parts of an object) for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(ApplyJetSlimming)

class ApplyTrackSlimming(JobProperty):
    """ Apply the slimming of tracks (removing parts of an object) for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(ApplyTrackSlimming)

class ApplyTruthSlimming(JobProperty):
    """ Apply the slimming of the MC truth (removing parts of an object) for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(ApplyTruthSlimming)

try:
    # This is the new way, in 15.4.0
    from RecExConfig.RecoFunctions import AddValidItemToList
    pass
except ImportError :
    # This is here for backwards compatibility
    from RecExCommon.RecoFunctions import AddValidItemToList
    pass

#Add D2PDMakers too
try:
    from D2PDMaker.D2PDFlags import listAODtoD2PD,listESDtoD2PD
    AddValidItemToList(listAODtoD2PD,listAODtoDPD)
    AddValidItemToList(listESDtoD2PD,listESDtoDPD)
except:
    print "Unable to import listAODtoD2PD. This requires D2PDMaker-00-00-55-08 or D2PDMaker-00-00-62"

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

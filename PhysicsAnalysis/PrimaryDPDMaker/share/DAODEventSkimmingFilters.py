##-----------------------------------------------------------------------------
## Name:  DAODEventSkimmingFilters.py
##
## Author:   Karsten Koeneke <karsten.koeneke_at_cern.ch>
## Created:  January 2010
##
## Description:  Define the event skimming filters for all DAODs. (Not DAODMs!)
##
## $Id$
##-----------------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/DAODEventSkimmingFilters.py")


## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

#define 'topSequence'
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDFlags_PhysicsDPDStreams import primPhysDPD
from RecExConfig.RecFlags import rec

from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner

## Create the list that will hold all filter names
from PrimaryDPDMaker.PrimaryDPDFlags import daodEventSkimmingFilterNamesList





##============================================================================
## Define the skimming (event selection) for the DAODM_SGLEL output stream
##============================================================================
# Have to set this flag to True to set up the trigger info!!!
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")


# Now, define the trigger fitlers
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
cmdString = ""
runningIndex = 0
for trigName in primDPD.WritePhysDPDElectron.TriggerNames :
    runningIndex += 1
    TriggerFilterName = "ElectronThinPhysDPDStream_TriggerFilter_"+trigName
    topSequence += TriggerFilter( TriggerFilterName,
                                  trigger = trigName )
    daodEventSkimmingFilterNamesList.append( TriggerFilterName )
    cmdString += TriggerFilterName
    if not runningIndex == primDPD.WritePhysDPDElectron.TriggerNames.__len__() :
        cmdString += " or "
        pass
    pass

# Create the combined decission
topSequence += LogicalFilterCombiner("StreamDAODM_SGLEM_AcceptEvent")
topSequence.StreamDAODM_SGLEM_AcceptEvent.cmdstring = cmdString
daodEventSkimmingFilterNamesList.append( "StreamDAODM_SGLEM_AcceptEvent" )






##============================================================================
## Define the skimming (event selection) for the DAODM_SGLMU output stream
##============================================================================
# Have to set this flag to True to set up the trigger info!!!
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")

# Now, define the trigger fitlers
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
cmdString = ""
runningIndex = 0
for trigName in primDPD.WritePhysDPDMuon.TriggerNames :
    runningIndex += 1
    TriggerFilterName = "MuonThinPhysDPDStream_TriggerFilter_"+trigName
    topSequence += TriggerFilter( TriggerFilterName,
                                  trigger = trigName )
    daodEventSkimmingFilterNamesList.append( TriggerFilterName )
    cmdString += TriggerFilterName
    if not runningIndex == primDPD.WritePhysDPDMuon.TriggerNames.__len__() :
        cmdString += " or "
        pass
    pass

# Create the combined decission
topSequence += LogicalFilterCombiner("StreamDAODM_SGLMU_AcceptEvent")
topSequence.StreamDAODM_SGLMU_AcceptEvent.cmdstring = cmdString
daodEventSkimmingFilterNamesList.append( "StreamDAODM_SGLMU_AcceptEvent" )









##============================================================================
## Define the skimming (event selection) for the DAODM_SGLPH output stream
##============================================================================
# Have to set this flag to True to set up the trigger info!!!
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")

# Now, define the trigger fitlers
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
cmdString = ""
runningIndex = 0
for trigName in primDPD.WritePhysDPDPhoton.TriggerNames :
    runningIndex += 1
    TriggerFilterName = "PhotonThinPhysDPDStream_TriggerFilter_"+trigName
    topSequence += TriggerFilter( TriggerFilterName,
                                  trigger = trigName )
    daodEventSkimmingFilterNamesList.append( TriggerFilterName )
    cmdString += TriggerFilterName
    if not runningIndex == primDPD.WritePhysDPDPhoton.TriggerNames.__len__() :
        cmdString += " or "
        pass
    pass

# Create the combined decission
topSequence += LogicalFilterCombiner("StreamDAODM_SGLPH_AcceptEvent")
topSequence.StreamDAODM_SGLPH_AcceptEvent.cmdstring = cmdString
daodEventSkimmingFilterNamesList.append( "StreamDAODM_SGLPH_AcceptEvent" )









##============================================================================
## Define the skimming (event selection) for the DAODM_SGLTAU output stream
##============================================================================
# Have to set this flag to True to set up the trigger info!!!
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")

# Now, define the trigger fitlers
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
cmdString = ""
runningIndex = 0
for trigName in primDPD.WritePhysDPDTau.TriggerNames :
    runningIndex += 1
    TriggerFilterName = "TauThinPhysDPDStream_TriggerFilter_"+trigName
    topSequence += TriggerFilter( TriggerFilterName,
                                  trigger = trigName )
    daodEventSkimmingFilterNamesList.append( TriggerFilterName )
    cmdString += TriggerFilterName
    if not runningIndex == primDPD.WritePhysDPDTau.TriggerNames.__len__() :
        cmdString += " or "
        pass
    pass

# Create the combined decission
topSequence += LogicalFilterCombiner("StreamDAODM_SGLTAU_AcceptEvent")
topSequence.StreamDAODM_SGLTAU_AcceptEvent.cmdstring = cmdString
daodEventSkimmingFilterNamesList.append( "StreamDAODM_SGLTAU_AcceptEvent" )









##============================================================================
## Define the skimming (event selection) for the DAOD_2EM output stream
##============================================================================
# Two electrons
from PrimaryDPDMaker.ElectronFilter import ElectronFilter
topSequence += ElectronFilter('PhysDiElectronDPDStream_DiElectronFilter')
topSequence.PhysDiElectronDPDStream_DiElectronFilter.cutEtMin      = primDPD.WritePhysDPDDiElectron.ElectronEt
topSequence.PhysDiElectronDPDStream_DiElectronFilter.cutIsEM       = primDPD.WritePhysDPDDiElectron.ElectronIsEM
topSequence.PhysDiElectronDPDStream_DiElectronFilter.cutEtaMax       = primDPD.WritePhysDPDDiElectron.ElectronEta
topSequence.PhysDiElectronDPDStream_DiElectronFilter.minNumberPassed = 2
daodEventSkimmingFilterNamesList.append( "PhysDiElectronDPDStream_DiElectronFilter" )

# Two photons
from PrimaryDPDMaker.PhotonFilter import PhotonFilter
topSequence += PhotonFilter('PhysDiElectronDPDStream_DiPhotonFilter')
topSequence.PhysDiElectronDPDStream_DiPhotonFilter.cutEtMin      = primDPD.WritePhysDPDDiElectron.PhotonEt
topSequence.PhysDiElectronDPDStream_DiPhotonFilter.cutIsEM       = primDPD.WritePhysDPDDiElectron.PhotonIsEM
topSequence.PhysDiElectronDPDStream_DiPhotonFilter.cutAuthor     = primDPD.WritePhysDPDDiElectron.PhotonAuthor
topSequence.PhysDiElectronDPDStream_DiPhotonFilter.minNumberPassed = 2
daodEventSkimmingFilterNamesList.append( "PhysDiElectronDPDStream_DiPhotonFilter" )

# One electron AND one photon
topSequence += ElectronFilter('PhysDiElectronDPDStream_ElectronFilter')
topSequence.PhysDiElectronDPDStream_ElectronFilter.cutEtMin      = primDPD.WritePhysDPDDiElectron.ElectronEt
topSequence.PhysDiElectronDPDStream_ElectronFilter.cutIsEM       = primDPD.WritePhysDPDDiElectron.ElectronIsEM
topSequence.PhysDiElectronDPDStream_ElectronFilter.cutEtaMax       = primDPD.WritePhysDPDDiElectron.ElectronEta
topSequence.PhysDiElectronDPDStream_ElectronFilter.minNumberPassed = 1
daodEventSkimmingFilterNamesList.append( "PhysDiElectronDPDStream_ElectronFilter" )

topSequence += PhotonFilter('PhysDiElectronDPDStream_PhotonFilter')
topSequence.PhysDiElectronDPDStream_PhotonFilter.cutEtMin      = primDPD.WritePhysDPDDiElectron.PhotonEt
topSequence.PhysDiElectronDPDStream_PhotonFilter.cutIsEM       = primDPD.WritePhysDPDDiElectron.PhotonIsEM
topSequence.PhysDiElectronDPDStream_PhotonFilter.cutAuthor     = 'AuthorPhoton'
topSequence.PhysDiElectronDPDStream_PhotonFilter.minNumberPassed = 1
daodEventSkimmingFilterNamesList.append( "PhysDiElectronDPDStream_PhotonFilter" )

topSequence += LogicalFilterCombiner("PhysDiElectronDPDStream_ElectronPhotonFilter")
topSequence.PhysDiElectronDPDStream_ElectronPhotonFilter.cmdstring = "PhysDiElectronDPDStream_ElectronFilter and PhysDiElectronDPDStream_PhotonFilter"
daodEventSkimmingFilterNamesList.append( "PhysDiElectronDPDStream_ElectronPhotonFilter" )

# Create the combined decission
topSequence += LogicalFilterCombiner("StreamDAOD_2EM_AcceptEvent")
topSequence.StreamDAOD_2EM_AcceptEvent.cmdstring = "PhysDiElectronDPDStream_DiElectronFilter or PhysDiElectronDPDStream_DiPhotonFilter or PhysDiElectronDPDStream_ElectronPhotonFilter"
daodEventSkimmingFilterNamesList.append( "StreamDAOD_2EM_AcceptEvent" )





##============================================================================
## Define the skimming (event selection) for the DAOD_2MU output stream
##============================================================================
from PrimaryDPDMaker.MuonFilter import MuonFilter
topSequence += MuonFilter('PhysDiMuonDPDStream_DiMuonFilter')
topSequence.PhysDiMuonDPDStream_DiMuonFilter.usingAOD        = True
topSequence.PhysDiMuonDPDStream_DiMuonFilter.muonContainers  = "all"
topSequence.PhysDiMuonDPDStream_DiMuonFilter.minNumberPassed = 2
topSequence.PhysDiMuonDPDStream_DiMuonFilter.cutPtMinMu      = primDPD.WritePhysDPDDiMuon.MuonPt
topSequence.PhysDiMuonDPDStream_DiMuonFilter.cutEtaMax       = primDPD.WritePhysDPDDiMuon.MuonEta
daodEventSkimmingFilterNamesList.append( "PhysDiMuonDPDStream_DiMuonFilter" )

# Create the combined decission
topSequence += LogicalFilterCombiner("StreamDAOD_2MU_AcceptEvent")
topSequence.StreamDAOD_2MU_AcceptEvent.cmdstring = "PhysDiMuonDPDStream_DiMuonFilter"
daodEventSkimmingFilterNamesList.append( "StreamDAOD_2MU_AcceptEvent" )





##============================================================================
## Define the skimming (event selection) for the DAOD_2PH output stream
##============================================================================
from PrimaryDPDMaker.PhotonFilter import PhotonFilter
topSequence += PhotonFilter('PhysDiPhotonDPDStream_PhotonFilter')
topSequence.PhysDiPhotonDPDStream_PhotonFilter.cutEtMin      = primDPD.WritePhysDPDDiPhoton.PhotonEt
topSequence.PhysDiPhotonDPDStream_PhotonFilter.cutIsEM       = primDPD.WritePhysDPDDiPhoton.PhotonIsEM
topSequence.PhysDiPhotonDPDStream_PhotonFilter.minNumberPassed = 2
daodEventSkimmingFilterNamesList.append( "PhysDiPhotonDPDStream_PhotonFilter" )

# Create the combined decission
topSequence += LogicalFilterCombiner("StreamDAOD_2PH_AcceptEvent")
topSequence.StreamDAOD_2PH_AcceptEvent.cmdstring = "PhysDiPhotonDPDStream_PhotonFilter"
daodEventSkimmingFilterNamesList.append( "StreamDAOD_2PH_AcceptEvent" )





##============================================================================
## Define the skimming (event selection) for the DAOD_EMMET output stream
##============================================================================
# Now, define the Electron Selection Filter
from PrimaryDPDMaker.ElectronFilter import ElectronFilter
topSequence += ElectronFilter('PhysElectronMETDPDStream_ElectronFilter')
topSequence.PhysElectronMETDPDStream_ElectronFilter.cutEtMin = primDPD.WritePhysDPDElectronMET.ElectronEt
topSequence.PhysElectronMETDPDStream_ElectronFilter.cutEtaMax  = primDPD.WritePhysDPDElectronMET.ElectronEta
topSequence.PhysElectronMETDPDStream_ElectronFilter.cutIsEM  = primDPD.WritePhysDPDElectronMET.ElectronIsEM
daodEventSkimmingFilterNamesList.append( "PhysElectronMETDPDStream_ElectronFilter" )

# Define the photon filter
from PrimaryDPDMaker.PhotonFilter import PhotonFilter
topSequence += PhotonFilter('PhysElectronMETDPDStream_PhotonFilter')
topSequence.PhysElectronMETDPDStream_PhotonFilter.cutEtMin = primDPD.WritePhysDPDElectronMET.PhotonEt
topSequence.PhysElectronMETDPDStream_PhotonFilter.cutIsEM  = primDPD.WritePhysDPDElectronMET.PhotonIsEM
daodEventSkimmingFilterNamesList.append( "PhysElectronMETDPDStream_PhotonFilter" )

# The missing Et filter algorithm
from PrimaryDPDMaker.MissingEtFilter import MissingEtFilter
topSequence += MissingEtFilter('PhysElectronMETDPDStream_MissingEtFilter')
topSequence.PhysElectronMETDPDStream_MissingEtFilter.cutMetMin = primDPD.WritePhysDPDElectronMET.MissingEt
daodEventSkimmingFilterNamesList.append( "PhysElectronMETDPDStream_MissingEtFilter" )

# Create the combined decission
topSequence += LogicalFilterCombiner("StreamDAOD_EMMET_AcceptEvent")
topSequence.StreamDAOD_EMMET_AcceptEvent.cmdstring = "( PhysElectronMETDPDStream_ElectronFilter and PhysElectronMETDPDStream_MissingEtFilter ) or ( PhysElectronMETDPDStream_PhotonFilter and PhysElectronMETDPDStream_MissingEtFilter )"
daodEventSkimmingFilterNamesList.append( "StreamDAOD_EMMET_AcceptEvent" )




##============================================================================
## Define the skimming (event selection) for the DAOD_EMMU output stream
##============================================================================
# Now, define the Electron Selection Filter
from PrimaryDPDMaker.ElectronFilter import ElectronFilter
topSequence += ElectronFilter('PhysElectronMuonDPDStream_ElectronFilter')
topSequence.PhysElectronMuonDPDStream_ElectronFilter.cutEtMin = primDPD.WritePhysDPDElectronMuon.ElectronEt
topSequence.PhysElectronMuonDPDStream_ElectronFilter.cutEtaMax  = primDPD.WritePhysDPDElectronMuon.ElectronEta
topSequence.PhysElectronMuonDPDStream_ElectronFilter.cutIsEM  = primDPD.WritePhysDPDElectronMuon.ElectronIsEM
daodEventSkimmingFilterNamesList.append( "PhysElectronMuonDPDStream_ElectronFilter" )

# Define the photon filter
from PrimaryDPDMaker.PhotonFilter import PhotonFilter
topSequence += PhotonFilter('PhysElectronMuonDPDStream_PhotonFilter')
topSequence.PhysElectronMuonDPDStream_PhotonFilter.cutEtMin = primDPD.WritePhysDPDElectronMuon.PhotonEt
topSequence.PhysElectronMuonDPDStream_PhotonFilter.cutIsEM  = primDPD.WritePhysDPDElectronMuon.PhotonIsEM
daodEventSkimmingFilterNamesList.append( "PhysElectronMuonDPDStream_PhotonFilter" )

# The muon filter algorithm
from PrimaryDPDMaker.MuonFilter import MuonFilter
topSequence += MuonFilter('PhysElectronMuonDPDStream_MuonFilter')
topSequence.PhysElectronMuonDPDStream_MuonFilter.usingAOD       = True
topSequence.PhysElectronMuonDPDStream_MuonFilter.muonContainers = "all"
topSequence.PhysElectronMuonDPDStream_MuonFilter.cutPtMinMu     = primDPD.WritePhysDPDElectronMuon.MuonPt
topSequence.PhysElectronMuonDPDStream_MuonFilter.cutEtaMax      = primDPD.WritePhysDPDElectronMuon.MuonEta
daodEventSkimmingFilterNamesList.append( "PhysElectronMuonDPDStream_MuonFilter" )

# Create the combined decission
topSequence += LogicalFilterCombiner("StreamDAOD_EMMU_AcceptEvent")
topSequence.StreamDAOD_EMMU_AcceptEvent.cmdstring = "( PhysElectronMuonDPDStream_ElectronFilter and PhysElectronMuonDPDStream_MuonFilter ) or ( PhysElectronMuonDPDStream_PhotonFilter and PhysElectronMuonDPDStream_MuonFilter )"
daodEventSkimmingFilterNamesList.append( "StreamDAOD_EMMU_AcceptEvent" )




##============================================================================
## Define the skimming (event selection) for the DAOD_EMTAU output stream
##============================================================================
# Now, define the electron filter
from PrimaryDPDMaker.ElectronFilter import ElectronFilter
topSequence += ElectronFilter('PhysElectronTauStream_ElectronFilter')
topSequence.PhysElectronTauStream_ElectronFilter.cutEtMin = primDPD.WritePhysDPDElectronTau.ElectronEt
topSequence.PhysElectronTauStream_ElectronFilter.cutEtaMax  = primDPD.WritePhysDPDElectronTau.ElectronEta
topSequence.PhysElectronTauStream_ElectronFilter.cutIsEM  = primDPD.WritePhysDPDElectronTau.ElectronIsEM
daodEventSkimmingFilterNamesList.append( "PhysElectronTauStream_ElectronFilter" )

# Define the photon filter
from PrimaryDPDMaker.PhotonFilter import PhotonFilter
topSequence += PhotonFilter('PhysElectronTauStream_PhotonFilter')
topSequence.PhysElectronTauStream_PhotonFilter.cutEtMin = primDPD.WritePhysDPDElectronTau.PhotonEt
topSequence.PhysElectronTauStream_PhotonFilter.cutIsEM  = primDPD.WritePhysDPDElectronTau.PhotonIsEM
daodEventSkimmingFilterNamesList.append( "PhysElectronTauStream_PhotonFilter" )

# Create an instance of the tau fitler and schedule it
from PrimaryDPDMaker.TauFilter import TauFilter
topSequence += TauFilter("PhysElectronTauStream_TauFilter")
topSequence.PhysElectronTauStream_TauFilter.tauCollectionName = "TauRecContainer"    
topSequence.PhysElectronTauStream_TauFilter.cutTauEtMin       = primDPD.WritePhysDPDElectronTau.TauEt
topSequence.PhysElectronTauStream_TauFilter.cutNTrk           = primDPD.WritePhysDPDElectronTau.TauId
topSequence.PhysElectronTauStream_TauFilter.doPreselection    = primDPD.WritePhysDPDElectronTau.TauPreselection
daodEventSkimmingFilterNamesList.append( "PhysElectronTauStream_TauFilter" )

# Create the combined decission
topSequence += LogicalFilterCombiner("StreamDAOD_EMTAU_AcceptEvent")
topSequence.StreamDAOD_EMTAU_AcceptEvent.cmdstring = "( PhysElectronTauStream_ElectronFilter and PhysElectronTauStream_TauFilter ) or ( PhysElectronTauStream_PhotonFilter and PhysElectronTauStream_TauFilter )"
daodEventSkimmingFilterNamesList.append( "StreamDAOD_EMTAU_AcceptEvent" )





##============================================================================
## Define the skimming (event selection) for the DAOD_EMJET output stream
##============================================================================
from PrimaryDPDMaker.ElectronFilter import ElectronFilter
topSequence += ElectronFilter("PhysJetElectronStream_ElectronFilter")
topSequence.PhysJetElectronStream_ElectronFilter.cutEtMin  = primDPD.WritePhysDPDJetElectron.ElectronEt
topSequence.PhysJetElectronStream_ElectronFilter.cutEtaMax = primDPD.WritePhysDPDJetElectron.ElectronEta
topSequence.PhysJetElectronStream_ElectronFilter.cutIsEM   = primDPD.WritePhysDPDJetElectron.ElectronIsEM
daodEventSkimmingFilterNamesList.append( "PhysJetElectronStream_ElectronFilter" )

# Figure out what jet filters should be used based on the settings in the jobproperties flags.
# If multiple jet collection names are given, the same number of filter algorithms are created
# and all combined with a logical OR.
from PrimaryDPDMaker.JetFilter import JetFilter
cmdString = ""
runningIndex = 0
for jetColName in primDPD.WritePhysDPDJetElectron.JetCollectionNames :
    runningIndex += 1
    JetFilterName = "PhysJetElectronStream_"+jetColName+"ElectronFilter"
    topSequence += JetFilter( JetFilterName,
                              OutputLevel             = INFO,
                              cutEtMin                = primDPD.WritePhysDPDJetElectron.JetEt,
                              cutEtMax                = 14.0*Units.TeV,
                              removeOverlap           = primDPD.WritePhysDPDJetElectron.RemoveOverlap,
                              deltaR                  = primDPD.WritePhysDPDJetElectron.DeltaR,
                              leptonContainerTypeList = [topSequence.PhysJetElectronStream_ElectronFilter.electronCollectionType],
                              leptonContainerNameList = [topSequence.PhysJetElectronStream_ElectronFilter.name()],
                              minNumberPassed         = primDPD.WritePhysDPDJetElectron.JetNumber
                              )
    daodEventSkimmingFilterNamesList.append( JetFilterName )
    cmdString += JetFilterName
    if not runningIndex == primDPD.WritePhysDPDJetElectron.JetCollectionNames.__len__() :
        cmdString += " or "
        pass
    pass    
topSequence += LogicalFilterCombiner("PhysJetElectronStream_JetElectronCombinedFilter")
topSequence.PhysJetElectronStream_JetElectronCombinedFilter.cmdstring = cmdString
daodEventSkimmingFilterNamesList.append( "PhysJetElectronStream_JetElectronCombinedFilter" )


# Define the photon filter
from PrimaryDPDMaker.PhotonFilter import PhotonFilter
topSequence += PhotonFilter("PhysJetElectronStream_PhotonFilter")
topSequence.PhysJetElectronStream_PhotonFilter.cutEtMin = primDPD.WritePhysDPDJetElectron.PhotonEt
topSequence.PhysJetElectronStream_PhotonFilter.cutIsEM  = primDPD.WritePhysDPDJetElectron.PhotonIsEM
daodEventSkimmingFilterNamesList.append( "PhysJetElectronStream_PhotonFilter" )

# Figure out what jet filters should be used based on the settings in the jobproperties flags.
# If multiple jet collection names are given, the same number of filter algorithms are created
# and all combined with a logical OR.
from PrimaryDPDMaker.JetFilter import JetFilter
cmdString = ""
runningIndex = 0
for jetColName in primDPD.WritePhysDPDJetElectron.JetCollectionNames :
    runningIndex += 1
    JetFilterName = "PhysJetElectronStream_"+jetColName+"PhotonFilter"
    topSequence += JetFilter( JetFilterName,
                              OutputLevel             = INFO,
                              cutEtMin                = primDPD.WritePhysDPDJetElectron.JetEt,
                              cutEtMax                = 14.0*Units.TeV,
                              removeOverlap           = primDPD.WritePhysDPDJetElectron.RemoveOverlap,
                              deltaR                  = primDPD.WritePhysDPDJetElectron.DeltaR,
                              leptonContainerTypeList = [topSequence.PhysJetElectronStream_PhotonFilter.photonCollectionType],
                              leptonContainerNameList = [topSequence.PhysJetElectronStream_PhotonFilter.name()],
                              minNumberPassed         = primDPD.WritePhysDPDJetElectron.JetNumber
                              )
    daodEventSkimmingFilterNamesList.append( JetFilterName )
    cmdString += JetFilterName
    if not runningIndex == primDPD.WritePhysDPDJetElectron.JetCollectionNames.__len__() :
        cmdString += " or "
        pass
    pass    
topSequence += LogicalFilterCombiner("PhysJetElectronStream_JetPhotonCombinedFilter")
topSequence.PhysJetElectronStream_JetPhotonCombinedFilter.cmdstring = cmdString
daodEventSkimmingFilterNamesList.append( "PhysJetElectronStream_JetPhotonCombinedFilter" )

# Create the combined decission
topSequence += LogicalFilterCombiner("StreamDAOD_EMJET_AcceptEvent")
topSequence.StreamDAOD_EMJET_AcceptEvent.cmdstring = "( PhysJetElectronStream_JetElectronCombinedFilter and PhysJetElectronStream_ElectronFilter ) or ( PhysJetElectronStream_JetPhotonCombinedFilter and PhysJetElectronStream_PhotonFilter )"
daodEventSkimmingFilterNamesList.append( "StreamDAOD_EMJET_AcceptEvent" )





##============================================================================
## Define the skimming (event selection) for the DAOD_JETMET output stream
##============================================================================
# Figure out what jet filters should be used based on the settings in the jobproperties flags.
# If multiple jet collection names are given, the same number of filter algorithms are created
# and all combined with a logical OR.
from PrimaryDPDMaker.JetFilter import JetFilter
cmdString = ""
leadingJets = []
runningIndex = 0
JetFilterTypeList = []
JetFilterNameList = []
for jetColName in primDPD.WritePhysDPDJetMET.JetCollectionNames :
    runningIndex += 1
    JetFilterName = "PhysJetMETDPDStream_"+jetColName+"Filter"
    JetFilterTypeList.append( "JetCollection" )
    JetFilterNameList.append( JetFilterName )
    topSequence += JetFilter( JetFilterName,
                              OutputLevel        = INFO,
                              cutEtMin           = primDPD.WritePhysDPDJetMET.JetEt,
                              cutEtMax           = 14.0*Units.TeV,
                              removeOverlap      = primDPD.WritePhysDPDJetMET.RemoveOverlap,
                              deltaR             = primDPD.WritePhysDPDJetMET.DeltaR,
                              cutMinMultiplicity = primDPD.WritePhysDPDJetMET.JetNumber )
    daodEventSkimmingFilterNamesList.append( JetFilterName )
    cmdString += JetFilterName
    if not runningIndex == primDPD.WritePhysDPDJetMET.JetCollectionNames.__len__() :
        cmdString += " or "
        pass
    pass    
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
topSequence += LogicalFilterCombiner("PhysJetMETDPDStream_JetCombinedFilter")
topSequence.PhysJetMETDPDStream_JetCombinedFilter.cmdstring = cmdString
daodEventSkimmingFilterNamesList.append( "PhysJetMETDPDStream_JetCombinedFilter" )

# The missing Et filter algorithm
from PrimaryDPDMaker.MissingEtFilter import MissingEtFilter
topSequence += MissingEtFilter('PhysJetMETDPDStream_MissingEtFilter')
topSequence.PhysJetMETDPDStream_MissingEtFilter.OutputLevel           = INFO
topSequence.PhysJetMETDPDStream_MissingEtFilter.cutMetMin             = primDPD.WritePhysDPDJetMET.MissingEt
topSequence.PhysJetMETDPDStream_MissingEtFilter.jetCollectionTypeList = JetFilterTypeList
topSequence.PhysJetMETDPDStream_MissingEtFilter.jetCollectionNameList = JetFilterNameList
topSequence.PhysJetMETDPDStream_MissingEtFilter.useUncalibratedJets   = primDPD.WritePhysDPDJetMET.METUseUncalibratedJets
topSequence.PhysJetMETDPDStream_MissingEtFilter.useLeadingJet         = primDPD.WritePhysDPDJetMET.METUseLeadingJet
topSequence.PhysJetMETDPDStream_MissingEtFilter.minDeltaPhi           = primDPD.WritePhysDPDJetMET.METMinDeltaPhi
topSequence.PhysJetMETDPDStream_MissingEtFilter.requireMet            = primDPD.WritePhysDPDJetMET.RequireMet
daodEventSkimmingFilterNamesList.append( "PhysJetMETDPDStream_MissingEtFilter" )

# Create the combined decission
topSequence += LogicalFilterCombiner("StreamDAOD_JETMET_AcceptEvent")
topSequence.StreamDAOD_JETMET_AcceptEvent.cmdstring = "PhysJetMETDPDStream_JetCombinedFilter and PhysJetMETDPDStream_MissingEtFilter"
daodEventSkimmingFilterNamesList.append( "StreamDAOD_JETMET_AcceptEvent" )





##============================================================================
## Define the skimming (event selection) for the DAOD_MUJET output stream
##============================================================================
# The muon filter algorithm
from PrimaryDPDMaker.MuonFilter import MuonFilter
topSequence += MuonFilter('PhysJetMuonDPDStream_MuonFilter')
topSequence.PhysJetMuonDPDStream_MuonFilter.usingAOD       = True
topSequence.PhysJetMuonDPDStream_MuonFilter.muonContainers = "all"
topSequence.PhysJetMuonDPDStream_MuonFilter.cutPtMinMu     = primDPD.WritePhysDPDJetMuon.MuonPt
topSequence.PhysJetMuonDPDStream_MuonFilter.cutEtaMax      = primDPD.WritePhysDPDJetMuon.MuonEta
daodEventSkimmingFilterNamesList.append( "PhysJetMuonDPDStream_MuonFilter" )

# Figure out what jet filters should be used based on the settings in the jobproperties flags.
# If multiple jet collection names are given, the same number of filter algorithms are created
# and all combined with a logical OR.
from PrimaryDPDMaker.JetFilter import JetFilter
cmdString = ""
runningIndex = 0
for jetColName in primDPD.WritePhysDPDJetMuon.JetCollectionNames :
    runningIndex += 1
    JetFilterName = "PhysJetMuonStream_"+jetColName+"Filter"
    topSequence += JetFilter( JetFilterName,
                              OutputLevel             = INFO,
                              cutEtMin                = primDPD.WritePhysDPDJetMuon.JetEt,
                              cutEtMax                = 14.0*Units.TeV,
                              removeOverlap           = primDPD.WritePhysDPDJetMuon.RemoveOverlap,
                              deltaR                  = primDPD.WritePhysDPDJetMuon.DeltaR,
                              leptonContainerTypeList = ['Analysis::MuonContainer'],
                              leptonContainerNameList = [topSequence.PhysJetMuonDPDStream_MuonFilter.name()],
                              cutMinMultiplicity      = primDPD.WritePhysDPDJetMuon.JetNumber )
    daodEventSkimmingFilterNamesList.append( JetFilterName )
    cmdString += JetFilterName
    if not runningIndex == primDPD.WritePhysDPDJetMuon.JetCollectionNames.__len__() :
        cmdString += " or "
        pass
    pass    
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
topSequence += LogicalFilterCombiner("PhysJetMuonStream_JetCombinedFilter")
topSequence.PhysJetMuonStream_JetCombinedFilter.cmdstring = cmdString
daodEventSkimmingFilterNamesList.append( "PhysJetMuonStream_JetCombinedFilter" )

# Create the combined decission
topSequence += LogicalFilterCombiner("StreamDAOD_MUJET_AcceptEvent")
topSequence.StreamDAOD_MUJET_AcceptEvent.cmdstring = "PhysJetMuonDPDStream_MuonFilter and PhysJetMuonStream_JetCombinedFilter"
daodEventSkimmingFilterNamesList.append( "StreamDAOD_MUJET_AcceptEvent" )





##============================================================================
## Define the skimming (event selection) for the DAOD_TAUJET output stream
##============================================================================
from PrimaryDPDMaker.TauFilter import TauFilter
topSequence += TauFilter("PhysJetTauStream_TauFilter")
topSequence.PhysJetTauStream_TauFilter.tauCollectionName = "TauRecContainer"    
topSequence.PhysJetTauStream_TauFilter.cutTauEtMin       = primDPD.WritePhysDPDJetTau.TauEt
topSequence.PhysJetTauStream_TauFilter.cutNTrk           = primDPD.WritePhysDPDJetTau.TauId
topSequence.PhysJetTauStream_TauFilter.doPreselection    = primDPD.WritePhysDPDJetTau.TauPreselection
daodEventSkimmingFilterNamesList.append( "PhysJetTauStream_TauFilter" )

# Figure out what jet filters should be used based on the settings in the jobproperties flags.
# If multiple jet collection names are given, the same number of filter algorithms are created
# and all combined with a logical OR.
from PrimaryDPDMaker.JetFilter import JetFilter
cmdString = ""
runningIndex = 0
for jetColName in primDPD.WritePhysDPDJetTau.JetCollectionNames :
    runningIndex += 1
    JetFilterName = "PhysJetTauStream_"+jetColName+"Filter"
    topSequence += JetFilter( JetFilterName,
                              OutputLevel             = INFO,
                              cutEtMin                = primDPD.WritePhysDPDJetTau.JetEt,
                              cutEtMax                = 14.0*Units.TeV,
                              removeOverlap           = primDPD.WritePhysDPDJetTau.RemoveOverlap,
                              deltaR                  = primDPD.WritePhysDPDJetTau.DeltaR,
                              leptonContainerTypeList = [topSequence.PhysJetTauStream_TauFilter.tauCollectionType],
                              leptonContainerNameList = [topSequence.PhysJetTauStream_TauFilter.name()],
                              cutMinMultiplicity      = primDPD.WritePhysDPDJetTau.JetNumber )
    daodEventSkimmingFilterNamesList.append( JetFilterName )
    cmdString += JetFilterName
    if not runningIndex == primDPD.WritePhysDPDJetTau.JetCollectionNames.__len__() :
        cmdString += " or "
        pass
    pass    
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
topSequence += LogicalFilterCombiner("PhysJetTauStream_JetCombinedFilter")
topSequence.PhysJetTauStream_JetCombinedFilter.cmdstring = cmdString
daodEventSkimmingFilterNamesList.append( "PhysJetTauStream_JetCombinedFilter" )

# Create the combined decission
topSequence += LogicalFilterCombiner("StreamDAOD_TAUJET_AcceptEvent")
topSequence.StreamDAOD_TAUJET_AcceptEvent.cmdstring = "PhysJetTauStream_TauFilter and PhysJetTauStream_JetCombinedFilter"
daodEventSkimmingFilterNamesList.append( "StreamDAOD_TAUJET_AcceptEvent" )






##============================================================================
## Define the skimming (event selection) for the DAOD_MUMET output stream
##============================================================================
# The muon filter algorithm
from PrimaryDPDMaker.MuonFilter import MuonFilter
topSequence += MuonFilter('PhysMuonMETDPDStream_MuonFilter')
topSequence.PhysMuonMETDPDStream_MuonFilter.usingAOD       = True
topSequence.PhysMuonMETDPDStream_MuonFilter.muonContainers = "all"
topSequence.PhysMuonMETDPDStream_MuonFilter.cutPtMinMu     = primDPD.WritePhysDPDMuonMET.MuonPt
topSequence.PhysMuonMETDPDStream_MuonFilter.cutEtaMax      = primDPD.WritePhysDPDMuonMET.MuonEta
daodEventSkimmingFilterNamesList.append( "PhysMuonMETDPDStream_MuonFilter" )

# The missing Et filter algorithm
from PrimaryDPDMaker.MissingEtFilter import MissingEtFilter
topSequence += MissingEtFilter('PhysMuonMETDPDStream_MissingEtFilter')
topSequence.PhysMuonMETDPDStream_MissingEtFilter.cutMetMin = primDPD.WritePhysDPDMuonMET.MissingEt
daodEventSkimmingFilterNamesList.append( "PhysMuonMETDPDStream_MissingEtFilter" )

# Create the combined decission
topSequence += LogicalFilterCombiner("StreamDAOD_MUMET_AcceptEvent")
topSequence.StreamDAOD_MUMET_AcceptEvent.cmdstring = "PhysMuonMETDPDStream_MuonFilter and PhysMuonMETDPDStream_MissingEtFilter"
daodEventSkimmingFilterNamesList.append( "StreamDAOD_MUMET_AcceptEvent" )





##============================================================================
## Define the skimming (event selection) for the DAOD_MUTAU output stream
##============================================================================
# The muon filter algorithm
from PrimaryDPDMaker.MuonFilter import MuonFilter
topSequence += MuonFilter('PhysMuonTauStream_MuonFilter')
topSequence.PhysMuonTauStream_MuonFilter.usingAOD        = True
topSequence.PhysMuonTauStream_MuonFilter.muonContainers  = "all"
topSequence.PhysMuonTauStream_MuonFilter.minNumberPassed = 1
topSequence.PhysMuonTauStream_MuonFilter.cutPtMinMu      = primDPD.WritePhysDPDMuonTau.MuonPt
topSequence.PhysMuonTauStream_MuonFilter.cutEtaMax       = primDPD.WritePhysDPDMuonTau.MuonEta
daodEventSkimmingFilterNamesList.append( "PhysMuonTauStream_MuonFilter" )

# Create an instance of the tau filter and schedule it
from PrimaryDPDMaker.TauFilter import TauFilter
topSequence += TauFilter("PhysMuonTauStream_TauFilter")
topSequence.PhysMuonTauStream_TauFilter.tauCollectionName = "TauRecContainer"    
topSequence.PhysMuonTauStream_TauFilter.cutTauEtMin       = primDPD.WritePhysDPDMuonTau.TauEt
topSequence.PhysMuonTauStream_TauFilter.cutNTrk           = primDPD.WritePhysDPDMuonTau.TauId
topSequence.PhysMuonTauStream_TauFilter.doPreselection    = primDPD.WritePhysDPDMuonTau.TauPreselection
daodEventSkimmingFilterNamesList.append( "PhysMuonTauStream_TauFilter" )

# Create the combined decission
topSequence += LogicalFilterCombiner("StreamDAOD_MUTAU_AcceptEvent")
topSequence.StreamDAOD_MUTAU_AcceptEvent.cmdstring = "PhysMuonTauStream_MuonFilter and PhysMuonTauStream_TauFilter"
daodEventSkimmingFilterNamesList.append( "StreamDAOD_MUTAU_AcceptEvent" )




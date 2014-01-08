##-----------------------------------------------------------------------------
## Name: PhysDESDM_RPVLL.py
##
## Author: Nick Barlow (Cambridge)
## Email:  Nicholas.Robert.Barlow@cern.ch
## Author: Christian Ohm (Stockholm)
## Email:  Christian.Ohm@cern.ch
##
## Description: This defines the content of the DESDM_RPVLL output stream.
##
##-----------------------------------------------------------------------------

import AthenaCommon.SystemOfUnits as Units

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from LongLivedParticleDPDMaker.LongLivedDPDFlags import *
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from LongLivedParticleDPDMaker.PrimaryDPDFlags_RPVLLStream import primRPVLLDESDM
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner

streamName = primDPD.WriteRPVLLStream.StreamName

fileName   = buildFileName( primDPD.WriteRPVLLStream )
RPVLL=MSMgr.NewPoolStream( streamName,fileName )

if primRPVLLDESDM.prescaleFlags.applyPrescale:
    RPVLL.SetPrescale(primRPVLLDESDM.prescaleFlags.prescaleFactor)

topCmdString=""
filtersToBookkeep = []
################# start adding filters #################


## For each analysis, include a job options file that defines the filter 
## for that analysis and adds it to the top filter

## Logics of how selections are activated
#  - for MC, rec.TriggerStream() will "" => use doXyz flags (all True by default)
#  - for data which has trigger stream metadata, activate a selection if the currently processed
#    trigger stream is in the doXyz.TriggerStreams list, OR, if doXyz.ForceToRun is set to True
#    (False by default, can be overridden with preExec)

## MS-agnostic SMP search (contact: Sascha Mehlhase)
if (primRPVLLDESDM.doSmpCaloId() and (rec.triggerStream() == ""  or rec.triggerStream() in primRPVLLDESDM.doSmpCaloId.TriggerStreams)) or primRPVLLDESDM.doSmpCaloId.ForceToRun:
    include("LongLivedParticleDPDMaker/PhysDESDM_SmpCaloId.py")
    pass

## RPV search based on displaced vertex finding in the ID (contact: Nick Barlow)
if (primRPVLLDESDM.doDV() and rec.triggerStream() in primRPVLLDESDM.doDV.TriggerStreams) or primRPVLLDESDM.doDV.ForceToRunMuonsSel or primRPVLLDESDM.doDV.ForceToRunEgammaSel or primRPVLLDESDM.doDV.ForceToRunJetTauEtmissSel :
    include("LongLivedParticleDPDMaker/PhysDESDM_DV.py")
    pass

## VH DV search based on displaced vertex finding in the ID along with a W or Z leptonic decay (contact: Andy Haas)
if (primRPVLLDESDM.doVH_DV() and (rec.triggerStream() == "" or rec.triggerStream() in primRPVLLDESDM.doVH_DV.TriggerStreams)) or primRPVLLDESDM.doVH_DV.ForceToRun:
    include("LongLivedParticleDPDMaker/PhysDESDM_VH_DV.py")
    pass

## Quirks search (contact: Jim Black)
if (primRPVLLDESDM.doQuirks() and (rec.triggerStream() == "" or rec.triggerStream() in primRPVLLDESDM.doQuirks.TriggerStreams)) or primRPVLLDESDM.doQuirks.ForceToRun:
    include("LongLivedParticleDPDMaker/PhysDESDM_Quirks.py")
    pass

## Dilepton search (contact Helen Hayward)
if (primRPVLLDESDM.doDiLep() and (rec.triggerStream() == "" or rec.triggerStream() in primRPVLLDESDM.doDiLep.TriggerStreams)) or primRPVLLDESDM.doDiLep.ForceToRun:
    include("LongLivedParticleDPDMaker/PhysDESDM_DiLep.py")
    pass

## MS-based SMP search (contact: Sofia Vallecorsa)
if (primRPVLLDESDM.doSmpMs() and (rec.triggerStream() == ""  or rec.triggerStream() in primRPVLLDESDM.doSmpMs.TriggerStreams)) or primRPVLLDESDM.doSmpMs.ForceToRun:
    include("LongLivedParticleDPDMaker/PhysDESDM_SmpMs.py")
    pass

## Kink-based chargino search (contact: Shimpei Yamamote, Yuya Azuma)
if (primRPVLLDESDM.doKinkedTrack() and (rec.triggerStream() == "" or rec.triggerStream() in primRPVLLDESDM.doKinkedTrack.TriggerStreams)) or primRPVLLDESDM.doKinkedTrack.ForceToRun:
    include("LongLivedParticleDPDMaker/PhysDESDM_KinkedTrack.py")
    pass

## Non-pointing photon search (contact: Helen Hayward)
if (primRPVLLDESDM.doNonPointing() and (rec.triggerStream() == ""  or rec.triggerStream() in primRPVLLDESDM.doNonPointing.TriggerStreams)) or primRPVLLDESDM.doNonPointing.ForceToRun:
    include("LongLivedParticleDPDMaker/PhysDESDM_NonPointing.py")
    pass

## Highly ionizing particles search (contact: Sergey Burdin)
if (primRPVLLDESDM.doHips() and (rec.triggerStream() == "" or rec.triggerStream() in primRPVLLDESDM.doHips.TriggerStreams)) or primRPVLLDESDM.doHips.ForceToRun:
    include("LongLivedParticleDPDMaker/PhysDESDM_Hips.py")
    pass

## Stopped gluino search (contact: Andy Haas)
if (primRPVLLDESDM.doStopped() and (rec.triggerStream() == "" or rec.triggerStream() in primRPVLLDESDM.doStopped.TriggerStreams)) or primRPVLLDESDM.doStopped.ForceToRun:
    include("LongLivedParticleDPDMaker/PhysDESDM_Stopped.py")
    pass

## Hidden Valley search (contact: Orin Harris, Gordon Watts)
if (primRPVLLDESDM.doHV() and (rec.triggerStream() == ""  or rec.triggerStream() in primRPVLLDESDM.doHV.TriggerStreams)) or primRPVLLDESDM.doHV.ForceToRun:
    include("LongLivedParticleDPDMaker/PhysDESDM_HV.py")
    pass

## Stealth Supersymmetry search (contact: Heather Russell, Henry Lubatti)
if (primRPVLLDESDM.doStealth() and (rec.triggerStream() == ""  or rec.triggerStream() in primRPVLLDESDM.doStealth.TriggerStreams)) or primRPVLLDESDM.doStealth.ForceToRun:
    include("LongLivedParticleDPDMaker/PhysDESDM_Stealth.py")
    pass


################# OR together all the filters into one, if at least one analysis selection is active

if topCmdString != "":
    topFilterName = "RPVLL_DESDMStream_TopFilter"
    topSequence += LogicalFilterCombiner( topFilterName )
    topSequence.RPVLL_DESDMStream_TopFilter.cmdstring=topCmdString
    acceptAlgsNames=[]
    acceptAlgsNames.append( topFilterName )
    RPVLL.AcceptAlgs(acceptAlgsNames)

# if the automatic trigger stream logics or command-line overrides did not activate a single selection,
# choose to not write out any events to the output file (can happen e.g. in Reco_trf.py test jobs)
else:
    from PrimaryDPDMaker.DummyFilter import DummyFilter
    dummyFilterName = "RPVLL_DESDMStream_DummyFilter"
    topSequence += DummyFilter(dummyFilterName)
    filtersToBookkeep += dummyFilterName
    vetoAlgsNames=[]
    vetoAlgsNames.append( dummyFilterName )
    RPVLL.VetoAlgs(vetoAlgsNames)

################# thinnning and event content #################

# thinning of CaloCells
if primRPVLLDESDM.doCaloCellThinning():
    from PrimaryDPDMaker.PrimaryDPDMakerConf import ThinContainers
    topSequence += ThinContainers("ThinContainersInRPVLLStream")
    topSequence.ThinContainersInRPVLLStream.thinSvc  = 'ThinningSvc/RPVLLStreamThinning'
    topSequence.ThinContainersInRPVLLStream.thinCaloCells               = True
    topSequence.ThinContainersInRPVLLStream.caloCellCollectionName      = 'AllCalo'
    topSequence.ThinContainersInRPVLLStream.keepCellsFromLinkContainers = True
    try:
        topSequence.ThinContainersInRPVLLStream.useCaloCellEnergyCut = True
    except(AttributeError):
        pass
    topSequence.ThinContainersInRPVLLStream.cellLinkContainerNames = [ "LArClusterEMSofte_Link", \
                                                                       "egClusterCollection_Link", \
                                                                       "LArClusterEM_Link", \
                                                                       "EMTopoCluster430_Link" ]
    topSequence.ThinContainersInRPVLLStream.keepForwardElectronCells    = True
    
    from AthenaServices.Configurables import ThinningSvc, createThinningSvc
    augStream=MSMgr.GetStream( streamName )
    evtStream=augStream.GetEventStream()
    svcMgr += createThinningSvc( svcName="RPVLLStreamThinning", outStreams=[evtStream] )
    
    pass

from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

excludeList=[]
#    "Track#*",
#    "InDet::Track#*",    
#    "TrackCollection#*",    
#    "Trk::TrackCollection#*",
#    'CaloShowerContainer#*',
#    ]

if primDPD.UseMCTruth() and primRPVLLDESDM.UseAODTruth():
    excludeList+=["McEventCollection#TruthEvent"]
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName,excludeList )

if primDPD.UseMCTruth() and primRPVLLDESDM.UseAODTruth():
    if not "McAodBuilder" in dir(topSequence):
        from McParticleAlgs.JobOptCfg import McAodBuilder
        topSequence+=McAodBuilder()
    RPVLL.AddItem( ['McEventCollection#GEN_AOD'] ) #AOD MC Truth
    RPVLL.AddItem( ['TruthParticleContainer#SpclMC'] ) #AOD MC Truth

# make sure bookkeeping info is kept for additional filters
RPVLL.AddOtherAlgsToBookkeep(filtersToBookkeep)

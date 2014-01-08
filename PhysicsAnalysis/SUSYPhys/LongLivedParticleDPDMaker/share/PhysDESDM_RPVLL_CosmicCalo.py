##-----------------------------------------------------------------------------
## Name: PhysDPD_RPVLL_CosmicCalo.py
##
## Author: Paul Jackson (SLAC) 
## Email:  Paul.Jackson@cern.ch
## Author: Nick Barlow (Cambridge)
## Email:  Nicholas.Robert.Barlow@cern.ch
## Author: Christian Ohm (Stockholm)
## Email:  Christian.Ohm@cern.ch
##
## Description: This defines the content of the RPVLL primary physics
##              Cosmic Calo ESD-basedDPD output stream.
##
##-----------------------------------------------------------------------------

import AthenaCommon.SystemOfUnits as Units

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from LongLivedParticleDPDMaker.LongLivedDPDFlags import *
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from LongLivedParticleDPDMaker.PrimaryDPDFlags_RPVLL_CosmicCaloStream import primRPVLLCosmicCaloDESDM

streamName = primDPD.WriteRPVLL_CosmicCaloStream.StreamName 
fileName   = buildFileName( primDPD.WriteRPVLL_CosmicCaloStream )
RPVLL_CosmicCalo=MSMgr.NewPoolStream( streamName,fileName )

if primDPD.ApplySkimming():
    primDPD.SetupTrigDecisionTool = True

    # Arrays holding the names of all offline object filters, 
    # trigger filters and accept algorithms/filters
    offlineJetFilterNames = []
    offlineMuSegFilterNames=[]
    triggerFilterNames = []
    acceptAlgsNames = []

############# Offline object filters

## first cut on number of muon segments

    from LongLivedParticleDPDMaker.MuonSegmentFilter import MuonSegmentFilter
    muSegFilterName = "RPVLLCC_DESDMStream_MuonSegmentFilter"
    topSequence += MuonSegmentFilter(muSegFilterName,
                                     maxNumberSegments=primRPVLLCosmicCaloDESDM.muonSegmentFilterFlags.cutNsegMax,
                                     minNumberSegments=primRPVLLCosmicCaloDESDM.muonSegmentFilterFlags.cutNsegMin)
    offlineMuSegFilterNames.append(muSegFilterName)

### if required, add a second MuonSegmentFilter instance, with looser cuts, but prescale it.

    if primRPVLLCosmicCaloDESDM.muonSegmentFilterFlags.addPrescaledLooseFilter:
        looseMuSegFilterName = "RPVLLCC_DESDMStream_looseMuSegFilter"
        topSequence += MuonSegmentFilter(looseMuSegFilterName,
                                         maxNumberSegments=primRPVLLCosmicCaloDESDM.muonSegmentFilterFlags.cutNsegMaxLoose,
                                         minNumberSegments=primRPVLLCosmicCaloDESDM.muonSegmentFilterFlags.cutNsegMinLoose)
        from PrimaryDPDMaker.PrimaryDPDMakerConf import PrimaryDPDPrescaler
        prescaledMuSegFilterName = "RPVLLCC_DESDMStream_prescaledMuSegFilter"
        topSequence += PrimaryDPDPrescaler(prescaledMuSegFilterName,Prescale=primRPVLLCosmicCaloDESDM.muonSegmentFilterFlags.prescaleFactorForLooseFilter,
                                           AcceptAlgs=[looseMuSegFilterName])
        offlineMuSegFilterNames.append(prescaledMuSegFilterName)
        pass
    
## jet filter
    
    from PrimaryDPDMaker.JetFilter import JetFilter
    jetFilterName = 'RPVLL_CosmicCaloDESDMStream_JetFilter'
    topSequence += JetFilter(jetFilterName)
    offlineJetFilterNames.append( jetFilterName )
    topSequence.RPVLL_CosmicCaloDESDMStream_JetFilter.cutEtMin = primRPVLLCosmicCaloDESDM.jetFilterFlags.cutEtMin
    topSequence.RPVLL_CosmicCaloDESDMStream_JetFilter.cutEtMax = primRPVLLCosmicCaloDESDM.jetFilterFlags.cutEtMax
    topSequence.RPVLL_CosmicCaloDESDMStream_JetFilter.cutEtaMax = primRPVLLCosmicCaloDESDM.jetFilterFlags.cutEtaMax    
    topSequence.RPVLL_CosmicCaloDESDMStream_JetFilter.minNumberPassed = primRPVLLCosmicCaloDESDM.jetFilterFlags.nJetCut
    topSequence.RPVLL_CosmicCaloDESDMStream_JetFilter.jetCollectionName = primRPVLLCosmicCaloDESDM.jetFilterFlags.jetCollectionName 
    topSequence.RPVLL_CosmicCaloDESDMStream_JetFilter.goodJetCollectionName = primRPVLLCosmicCaloDESDM.jetFilterFlags.goodJetCollectionName

    # Create a combined filter by ORing together all the offline object filters
    from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
    combinedOfflineObjectFilterName = "RPVLL_CosmicCaloDESDMStream_CombinedOfflineObjectFilter"
    topSequence += LogicalFilterCombiner( combinedOfflineObjectFilterName )
    offlineFilterCounter = 0
    cmdstring = "("
    for offlineFilterName in offlineMuSegFilterNames :
        if offlineFilterCounter > 0 :
            cmdstring += " or "
            pass
        cmdstring += offlineFilterName
        offlineFilterCounter += 1
        pass
    cmdstring+= ")"
    for offlineFilterName in offlineJetFilterNames:
        if offlineFilterCounter > 0 :
            cmdstring += " and "
            pass
        cmdstring += offlineFilterName
        offlineFilterCounter += 1
        pass
    topSequence.RPVLL_CosmicCaloDESDMStream_CombinedOfflineObjectFilter.cmdstring = cmdstring    


    # Now, define the trigger filters
    from PrimaryDPDMaker.TriggerFilter import TriggerFilter
    for trigName in primRPVLLCosmicCaloDESDM.triggerFilterFlags.TriggerNames :
        TriggerFilterName = "RPVLL_CosmicCaloDESDMStream_TriggerFilter_"+trigName
        topSequence += TriggerFilter( TriggerFilterName,
                                 trigger = trigName )
        triggerFilterNames.append( TriggerFilterName )
        pass

    # Create a combined filter by ORing together all the trigger filters
    combinedTriggerFilterName = "RPVLL_CosmicCaloDESDMStream_CombinedTriggerFilter"
    topSequence += LogicalFilterCombiner( combinedTriggerFilterName )
    triggerFilterCounter = 0
    cmdstring = ""
    for triggerFilterName in triggerFilterNames :
        if triggerFilterCounter > 0 :
            cmdstring += " or "
            pass
        cmdstring += triggerFilterName
        triggerFilterCounter += 1
        pass
    topSequence.RPVLL_CosmicCaloDESDMStream_CombinedTriggerFilter.cmdstring = cmdstring    

    # Create the final combined filter ANDing together the combined
    # trigger filter with the combined offline object filter
    combinedTriggerAndOfflineObjectFilterName = "RPVLL_CosmicCaloDESDMStream_CombinedTriggerAndOfflineObjectFilter"
    topSequence += LogicalFilterCombiner( combinedTriggerAndOfflineObjectFilterName )
    cmdstring = combinedOfflineObjectFilterName + " and " + combinedTriggerFilterName
    topSequence.RPVLL_CosmicCaloDESDMStream_CombinedTriggerAndOfflineObjectFilter.cmdstring = cmdstring
    acceptAlgsNames.append( combinedTriggerAndOfflineObjectFilterName )

    RPVLL_CosmicCalo.AcceptAlgs(acceptAlgsNames)
    RPVLL_CosmicCalo.AddOtherAlgsToBookkeep( [combinedOfflineObjectFilterName, 
                                       combinedTriggerFilterName, 
                                       combinedTriggerAndOfflineObjectFilterName] )
    pass


from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

excludeList=['TrackCollection#Tracks',
             'TrackCollection#*',
             'Trk::TrackCollection#*',
             'CaloShowerContainer#*'
             ]
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName,excludeList )

if primDPD.UseMCTruth():
    if not "McAodBuilder" in dir(topSequence):
        from McParticleAlgs.JobOptCfg import McAodBuilder
        topSequence+=McAodBuilder()
    RPVLL.AddItem( ['McEventCollection#GEN_AOD'] ) #AOD MC Truth
    RPVLL.AddItem( ['TruthParticleContainer#SpclMC'] ) #AOD MC Truth

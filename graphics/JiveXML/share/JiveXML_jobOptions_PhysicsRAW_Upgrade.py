## RTT jOs run in RecExCommon environment
## (RecExCommon_links.sh), but this doesn't work
## with Cosmics, need to remove static db link
## with shell command:
import os
#### remove db link:
os.system('rm -rf sqlite200')
# link is updated (no need to update jOs):
os.system('cp /afs/cern.ch/atlas/project/Atlantis/rttInputs/RAW_data11_RTT.pool.root .')
#
#### example (not needed):
##os.system('RecExCommission_links.sh')
##
## this is a trick to over-ride .set_Value_and_Lock
## settings in readesd.py: The first setting counts !
#
### From here: myTopOptions.py from RecExCommission_links.sh, rel. 16.6.1.1
###
#example of personal topOptions
#
# to use it  
# RecExCommon_links.sh  # once in a given directory
# athena >! athena.log
#  ( myTopOptions.py is defaulted through jobOptions.py soft link)
# 
# see RecExCommon/share/RecExCommon_flags.py for more available flags
# and https://twiki.cern.ch/twiki/bin/view/Atlas/RecExCommonFlags
# for more complete documentation.
#

#### As advised by David Rousseau to avoid job
#### failures due to Panda issue with DESDs. jpt 18Mar11
jp.Rec.doDPD.set_Value_and_Lock(False)

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#from RecExConfig.RecFlags import rec  #obsolete
from RecExConfig.RecAlgsFlags import recAlgs

#set input file. Note that this triggers autoconfiguration automatically (see RecExCommonAutoConfiguration wiki)
# here a MC RDO is specified, but this can also be a data BS, an ESD AOD or TAG
#if athenaCommonFlags.FilesInput.isDefault(): # check if not already set upstream

### default, but we use our data11 RTT: 
#athenaCommonFlags.FilesInput=["LFN:top_GEO-02-01-00_RDO_extract.pool"] 
###
athenaCommonFlags.FilesInput.set_Value_and_Lock(['RAW_data11_RTT.pool.root'])

athenaCommonFlags.EvtMax=20   # number of events to process run on all file

# switch off detectors
# rec.doForwardDet=False
# rec.doInDet=False
# rec.doCalo=False
# rec.doMuon=False

jp.Rec.doJiveXML.set_Value_and_Lock(True)

# rec.doTrigger = False; recAlgs.doTrigger=False # disable trigger (maybe necessary if detectors switched of)

# include my own algorithm(s). These jobo will be included at the end of topSequence configuration
# rec.UserAlgs=[ "MyPackage/MyAlgorithm_jobOptions.py" ]
# these lines will be executed at the end of topSequence configuration
# rec.UserExecs=[ "from MyPackage.myAlgConf import myAlg","topSequence+=myAlg()" ]
# these lines will be executed after RecExCommon_flags.py has been imported, last chance for flag modification
# note that flag locking mechanism has a better chance to yield consistent configuration
# see https://twiki.cern.ch/twiki/bin/view/Atlas/UserAnalysisTest#The_AOD_Production_Flags
# rec.UserFlags=[ "AODFlags.FastSimulation=False" ] 


# autoconfiguration might trigger undesired feature
# rec.doESD.set_Value_and_Lock(False) # uncomment if do not run ESD making algorithms
# rec.doWriteESD.set_Value_and_Lock(False) # uncomment if do not write ESD
# rec.doAOD.set_Value_and_Lock(False) # uncomment if do not run AOD making algorithms
# rec.doWriteAOD.set_Value_and_Lock(False) # uncomment if do not write AOD
# rec.doWriteTAG.set_Value_and_Lock(False) # uncomment if do not write TAG
# rec.doCBNT.set_Value_and_Lock(False) # uncomment if do not make CBNT


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

# user modifier of properties should come here
#topSequence.myAlg.someProperty=3.14

theAODJetRetriever.FavouriteJetCollection="AntiKt4TopoEMJets"
### 'EM' are up-to-date jets (May 2011), 'LC' could be default in 2012:
theAODJetRetriever.OtherJetCollections=["AntiKt6TopoEMJets","AntiKt4LCTopoJets","AntiKt6LCTopoJets","AntiKt6TowerJets","AntiKt6GhostTowerJets"]

theJetRecJetRetriever.FavouriteJetCollection="AntiKt4TopoEMJets"
### 'EM' are up-to-date jets (May 2011), 'LC' could be default in 2012:
theJetRecJetRetriever.OtherJetCollections=["AntiKt6TopoEMJets","AntiKt4LCTopoJets","AntiKt6LCTopoJets","AntiKt6TowerJets","AntiKt6GhostTowerJets"]

theBJetRetriever.FavouriteJetCollection="AntiKt4TopoEMJets"
### 'EM' are up-to-date jets (May 2011), 'LC' could be default in 2012:
theBJetRetriever.OtherJetCollections=["AntiKt6TopoEMJets","AntiKt4LCTopoJets","AntiKt6LCTopoJets"]

#### job options to retrieve spacepoints from ESDs
#### not done by default ! From Thijs Cornellisen
from InDetRecExample.InDetJobProperties import InDetFlags
jobproperties.InDetJobProperties.Enabled.set_Value_and_Lock(True)
InDetFlags.doSpacePointFormation.set_Value_and_Lock(True)

#### Too many track collections in RAW,
#### write only the most important:
theTrackParticleRetriever.OtherTrackCollections = ["CombinedFitMuonParticles","StacoTrackParticles","MuonboyTrackParticles"]
theTrackRetriever.OtherTrackCollections = ["CombinedFitMuonTracks","MuonSpectrometerTracks","ConvertedStacoTracks","ConvertedMuIdCBTracks","CombinedInDetTracks","ExtendedTracks"]

### initial try for jOs to make Upgrade event xml-files smaller (target <15MB):
#    jpt 27Sep11
#
theEventData2XML.DataTypes.remove("JiveXML::SiSpacePointRetriever/SiSpacePointRetriever")
theEventData2XML.DataTypes.remove("JiveXML::SiClusterRetriever/SiClusterRetriever")
theEventData2XML.DataTypes.remove("JiveXML::PixelClusterRetriever/PixelClusterRetriever")
theEventData2XML.DataTypes.remove("JiveXML::TRTRetriever/TRTRetriever")
theEventData2XML.DataTypes.remove("JiveXML::TrigSiSpacePointRetriever/TrigSiSpacePointRetriever")
#
# for real data input:
theEventData2XML.DataTypes.remove("JiveXML::SCTRDORetriever/SCTRDORetriever")
theEventData2XML.DataTypes.remove("JiveXML::PixelRDORetriever/PixelRDORetriever")
theCaloLArRetriever.LArlCellThreshold = 200.
theCaloHECRetriever.HEClCellThreshold = 200.

### Default collection 'Tracks' has no spacepoints anymore,
### ie wrong tracks in Polyline mode, so switch to this collection:
theVertexRetriever.TrackCollection = "CombinedInDetTracks"

# geometry version written intoxml-file (for upgrade)
from AthenaCommon.GlobalFlags import globalflags
theEventData2XML.GeometryVersion = globalflags.DetDescrVersion()

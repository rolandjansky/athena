#--------------------------------------------------------------------------
#
# MboyPerformance_topOptions.py
#
#--------------------------------------------------------------------------

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs

# CBNT is now off by default
rec.doCBNT=True

# Turn off all reconstruction algorithms
### rec.doESD = False
# remove unused stuff because of problems with rec.doESD = False
doTrigger=False
doxKalman=False
doiPatRec=False
doEmCluster=False
doCaloCluster=False
doCaloTopoCluster=False
doCaloEMTopoCluster=False
doConversion=False
doBtagging=False
doEgamma=False
doJetRec=False
doTauRec=False
doMuonIDStandAlone=False
doMuonIDCombined=False
doMuidLowPt=False
doMuGirl=False
doTileMuID=False
doCaloTrkMuId=False
doMuonSpShower=False
doMissingET=False
doObjMissingET=False
doMissingETSig=False
doEFlow=False
doEFlowJet= False
doAtlfast=False

rec.doLArg = False
rec.doTile = False

# Decide whether to write ESD/AOD/TAG
rec.doWriteESD = False
rec.doWriteAOD = False
rec.doWriteTAG = False
# Decide whether to run AOD-making algorithms
rec.doAOD = False

#rec.doTruth = True
# Turn on TrackRecordFilter to add Muon Spectrometer entry truth
#recAlgs.doTrackRecordFilter = True
#doTrackRecordFilter = True

# Turn on Muonboy
from MuonRecExample.MuonRecFlags import muonRecFlags
muonRecFlags.doMuonboy = True
muonRecFlags.doMoore = False
muonRecFlags.doStandalone = False

from MuonRecExample.MuonCBNTFlags import muonCBNT
muonCBNT.doMdtRaw = False
muonCBNT.doRpcRaw = False
muonCBNT.doTgcRaw = False
muonCBNT.doCscRaw = False

# Optionally run Inner Detector + Muon Spectrometer
# combined reconstruction with staco
#recAlgs.doStaco = True
#recAlgs.doMuTag = True
#doStaco = True
#doMuTag = True

# turn on the new tracking if staco or mutag is requested
#if recAlgs.doStaco() or recAlgs.doMuTag() :
#    recAlgs.donewTracking = True
#    donewTracking = True

# Specify number of events to be processed
if not 'EvtMax' in dir() :
    EvtMax = 50000

#--------------------------------------------------------------------------
# common flags
include ("RecExCommon/RecExCommon_flags.py")

# if no combined muon reco is requested then turn off Calo and ID
# except for their detector description
#if not recAlgs.doStaco() :
#    DetFlags.Calo_setOff()
#    DetFlags.detdescr.Calo_setOn()
#    DetFlags.ID_setOff()
#    DetFlags.detdescr.ID_setOn()
DetFlags.detdescr.all_setOn()

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

#
from MboyStatistics.MboyStatisticsConf import CBNTAA_Muon
theCBNTAA_Muon=CBNTAA_Muon()
#Tracks
theCBNTAA_Muon.TrackStoreLocation	= "Muonboy_TrackStore" ;
theCBNTAA_Muon.MaxNberOfTracks	        = 20
theCBNTAA_Muon.TakeTrackHit             = 1     
theCBNTAA_Muon.MaxNberOfTracksHits      = 400     
theCBNTAA_Muon.FitCov		        = 2
theCBNTAA_Muon.TrackPerigee  	        = 1
theCBNTAA_Muon.TrackCaloEntrance	= 1
theCBNTAA_Muon.TrackMuonSpectroEntrance = 1
#Segments
theCBNTAA_Muon.SegmentStoreLocation     = "Muonboy_SegmentStore" ;
theCBNTAA_Muon.MaxNberOfSegments        = 100                       
theCBNTAA_Muon.TakeSegmentHit           = 0     
theCBNTAA_Muon.MaxNberOfHits            = 400
    
#--------------------------------------------------------------------------

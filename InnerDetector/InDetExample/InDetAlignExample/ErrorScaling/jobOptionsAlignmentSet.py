# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Examples of overriding with different alignment sets

##conddb.addOverride("/Indet/Align","InDetAlign_EoverPv2.3_2011_E") #bc
##conddb.addOverride("/TRT/Align","TRTAlign_EoverPv2_2011_E") #bc
#conddb.addOverride("/PIXEL/PixReco", "PixelOfflineReco-Extended-01")
#conddb.addOverride("/PIXEL/PixReco", "PixelOfflineReco-Extended-000-01")

#conddb.addOverride("/Indet/Align","InDetAlign_Collision_7T_L3_2010_04")
#conddb.addOverride("/TRT/Align","TRTAlign_Collision_7T_L3L2_2010_04")

#conddb.addFolder('TRT_OFL','/TRT/Calib/DX')
##conddb.addOverride('/TRT/Calib/DX','TRTCalibDX_EoverPv2_2011_E') #bc

#conddb.addOverride('/Indet/Beampos', 'IndetBeampos-ES1-UPD2')
#conddb.addOverride('/Indet/PixelDist','InDetPixelDist-000-01')

doMisAlign = False

# uncomment to use misaligned geometry 
##############
if doMisAlign:
    from IOVDbSvc.CondDB import conddb
    conddb.blockFolder("/Indet/Align")
    conddb.blockFolder("/TRT/Align")
##############

# uncomment to use misaligned geometry
##########
if doMisAlign:
    from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr += CondProxyProvider()
    ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]

# uncomment to use misaligned geometry
######## set this to the file containing AlignableTransform objects
if doMisAlign:
    ServiceMgr.CondProxyProvider.InputCollections += ["/afs/cern.ch/user/a/atlidali/public/TRTZ_SiL3/Iter3_AlignmentConstants.root"]
    ServiceMgr.CondProxyProvider.InputCollections += ["MisalignmentSet2.pool.root"] 
    ServiceMgr.CondProxyProvider.OutputLevel = DEBUG
    ServiceMgr.IOVSvc.preLoadData=True
#########

#print ServiceMgr.CondProxyProvider

## This Athena job consists of algorithms that loop over events;
## here, the (default) top sequence is used:
### from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
### job = AlgSequence()
### seq = AthSequencer("AthMasterSeq")

#-----------------
# This block handles good run lists. Uncomment to enable goodrunlist
#-----------------
#from GoodRunsLists.GoodRunsListsConf import *
#ToolSvc += GoodRunsListSelectorTool() 
#GoodRunsListSelectorTool.GoodRunsListVec = [ 'MyLBCollection.xml' ]
#GoodRunsListSelectorTool.OutputLevel = DEBUG

#from GoodRunsListsUser.GoodRunsListsUserConf import *
#seq += GRLTriggerSelectorAlg('GRLTriggerAlg1')
#seq.GRLTriggerAlg1.GoodRunsListArray = ['MyLBCollection'] ## pick up correct name from inside xml file!
#-----------------

## AthMasterSeq is always executed before the top sequence, and is configured such that
## any follow-up sequence (eg. top sequence) is not executed in case GRLTriggerAlg1 does
## not pass the event
## In short, the sequence AthMasterSeq makes sure that all algs in the job sequence
## are skipped when an event gets rejects

## Add the ntuple dumper to the top sequence, as usual
## DummyDumperAlg1 is run in the top sequence, but is not executed in case GRLTriggerAlg1 rejects the event.
#job += DummyDumperAlg('DummyDumperAlg1')
#job.DummyDumperAlg1.RootFileName = 'selection1.root'


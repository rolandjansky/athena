# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSecVertexFinder.set_Value_and_Lock(False)

#set input file. Note that this triggers autoconfiguration automatically (see RecExCommonAutoConfiguration wiki)
# here a MC RDO is specified, but this can also be a data BS, an ESD AOD or TAG
#if athenaCommonFlags.FilesInput.isDefault(): # check if not already set upstream
#    athenaCommonFlags.FilesInput=["LFN:top_GEO-02-01-00_RDO_extract.pool"] 

if 0:
  athenaCommonFlags.FilesInput=["/afs/cern.ch/atlas/offline/test/data11_7TeV.00191920.physics_JetTauEtmiss.merge.RAW._lb0257._SFO-9._0001.1.10evts"]
elif 0:
  athenaCommonFlags.FilesInput=["/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/DATA/AOD/data12_8TeV.00209109.physics_JetTauEtmiss.merge.AOD.19_0_2_1._lb0186._SFO-1._0001.pool.root.1"]
else:
  athenaCommonFlags.FilesInput=["/afs/cern.ch/atlas/offline/test/mc11_valid.105200.T1_McAtNlo_Jimmy.digit.RDO.e835_s1310_s1300_d622.RDO.10evts.pool.root"]

#athenaCommonFlags.jp.AthenaCommonFlags.EvtMax=-1   # number of events to process run on all file


# switch off detectors
# rec.doForwardDet=False
# rec.doInDet=False
# rec.doCalo=False
# rec.doMuon=False
#rec.doEgamma=False
rec.doTrigger = False  # disable trigger (maybe necessary if detectors switched of)
rec.doMuon=False
rec.doMuonCombined=False
#rec.doWriteAOD=False
#rec.doWriteESD=False
#rec.doDPD=False
#rec.doTruth=False

#rec.doNameAuditor=True


# ** Turn PerfMon off for valgrind usage **
#rec.doPerfMon.set_Value_and_Lock(False)
#rec.doDetailedPerfMon.set_Value_and_Lock(False)
#rec.doSemiDetailedPerfMon.set_Value_and_Lock(False)



# include my own algorithm(s). These jobo will be included at the end of topSequence configuration
# rec.UserAlgs=[ "MyPackage/MyAlgorithm_jobOptions.py" ]
# these lines will be executed at the end of topSequence configuration
# rec.UserExecs=[ "from MyPackage.myAlgConf import myAlg","topSequence+=myAlg()" ]
# these lines will be executed after RecExCommon_flags.py has been imported, last chance for flag modification
# note that flag locking mechanism has a better chance to yield consistent configuration
# see https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerFlags
# see https://twiki.cern.ch/twiki/bin/view/Atlas/UserAnalysisTest#The_AOD_Production_Flags
# rec.UserFlags=[ "AODFlags.FastSimulation=False" ] 


# autoconfiguration might trigger undesired feature
# rec.doESD.set_Value_and_Lock(False) # uncomment if do not run ESD making algorithms
# rec.doWriteESD.set_Value_and_Lock(False) # uncomment if do not write ESD
# rec.doAOD.set_Value_and_Lock(False) # uncomment if do not run AOD making algorithms
# rec.doWriteAOD.set_Value_and_Lock(False) # uncomment if do not write AOD
# rec.doWriteTAG.set_Value_and_Lock(False) # uncomment if do not write TAG


#rec.doDumpTES=True

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

# user modifier of properties should come here
#topSequence.myAlg.someProperty=3.14

#svcMgr.MessageSvc.OutputLevel=VERBOSE


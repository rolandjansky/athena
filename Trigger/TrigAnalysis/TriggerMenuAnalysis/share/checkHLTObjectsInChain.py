#example of personal topOptions
#
# to use it  
# athena >! athena.log
#  ( myTopOptions.py is defaulted through jobOptions.py soft link)
# 
# see RecExCommon/share/RecExCommon_flags.py for more available flags
# and https://twiki.cern.ch/twiki/bin/view/Atlas/RecExCommonFlags
# for more complete documentation.
#
# doESD, DetDescrVersion and other flags 
# needs be set before the include, since several
# secondary flags are configured according to that one
#


# DetDescrVersion="ATLAS-CSC-01-02-00" # default
# DetDescrVersion="ATLAS-DC3-02" # 11.0.42 files 

#DetDescrVersion = 'ATLAS-CSC-02-01-00'
DetDescrversion = 'ATLAS-GEO-02-00-00'

# doTrigger = False # for example do not run trigger simulation
doTruth=False
# number of event to process
EvtMax=1

doCBNT=False

# include my own algorithm(s)
# UserAlgs=[ "MyPackage/MyAlgorithm_jobOptions.py" ] 


# By default write ESD, AOD and TAG simultenaously, but note that in production
#      these should be done in separate steps (results might not be bit-by-bit identical).
# doESD=False # uncomment if do not run ESD making algorithms
# doWriteESD=False # uncomment if do not write ESD
# doAOD=False # uncomment if do not run AOD making algorithms
# doWriteAOD=False # uncomment if do not write AOD
# doWriteTAG=False # uncomment if do not write TAG


# if needed to configure trigger
# see https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerFlags
# include ( "TriggerRelease/TriggerFlags.py" )

# if needed to configure AOD building
# see https://twiki.cern.ch/twiki/bin/view/Atlas/UserAnalysisTest#The_AOD_Production_Flags
# from ParticleBuilderOptions.AODFlags import AODFlags



# DetFlags modifications are best set here (uncomment RecExCommon_flags first)
# include ("RecExCond/RecExCommon_flags.py")
# switch off ID, calo, or muons
# DetFlags.ID_setOff()
# DetFlags.Calo_setOff()
# DetFlags.Muon_setOff()

readRDO=False
readESD=False
readAOD=True
doESD=False
doAOD=False
doWriteESD=False
doWriteAOD=False
doWriteTAG=False

PoolAODInput = [
    # 'AOD.pool.root',
    # '/space2/tkohno/data/test/AOD.022568._00001.pool.root.1',
    #
    # AOD produced with 14.2.21
    '/space2/tkohno/data/test/valid1.105144.PythiaZee.recon.AOD.e380_s513_r634_tid043141/AOD.043141._00005.pool.root.1', 
    #'LFN:AOD_14.2.21.pool.root', 
    #'/space2/tkohno/data/test/AOD.026050._00001.pool.root.1',
    # AOD produced with 13.0.40.5
##     '/space2/tkohno/data/test/AOD.022568._01979.pool.root.1',
##     '/space2/tkohno/data/test/AOD.022568._01980.pool.root.1',
##     '/space2/tkohno/data/test/AOD.022568._01981.pool.root.1',
##     '/space2/tkohno/data/test/AOD.022568._01982.pool.root.1',
##     '/space2/tkohno/data/test/AOD.022568._01983.pool.root.2',
##     '/space2/tkohno/data/test/AOD.022568._01984.pool.root.1',
##     '/space2/tkohno/data/test/AOD.022568._01985.pool.root.1',
##     '/space2/tkohno/data/test/AOD.022568._01986.pool.root.1',
##     '/space2/tkohno/data/test/AOD.022568._01987.pool.root.1',
##     '/space2/tkohno/data/test/AOD.022568._01988.pool.root.1',
    ]
# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

#---------------------------------------------------------------------
# user modifier should come here
#---------------------------------------------------------------------

#---------------------------------------------------------------------
# Set up trigger configuration service and metadata service it relies on,
# for analysis job without RecExCommon set up trigger decision tool
#---------------------------------------------------------------------
# from AthenaCommon.GlobalFlags import GlobalFlags
# GlobalFlags.DetGeo.set_atlas()

# flags needed for TriggerConfigGetter
from RecExConfig.RecFlags  import rec
rec.readAOD=True
rec.readRDO=False
rec.doWriteAOD=False
rec.doWriteESD=False

from TrigDecision.TrigDecisionConfig import TrigDecisionTool
tdt = TrigDecisionTool()
ToolSvc += tdt

# To read files with trigger config stored as in-file meta-data,
# i.e. 13.0.40 and above: ds
# To read AOD produced with 13.0.30 you need to change ds to aod:
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList = ['ds']
TriggerFlags.configForStartup = 'HLTofflineNoL1Thr'

# set up trigger config service
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg =  TriggerConfigGetter()

from AthenaCommon.AppMgr import AlgSequence, ServiceMgr

#------------------------------------------------------------------------
# My job
#------------------------------------------------------------------------

from TriggerMenuAnalysis.TriggerMenuAnalysisConfig import CheckObjectsInChain_default
job = AlgSequence()
myalgo = CheckObjectsInChain_default()
myalgo.ChainsToCheck = [
    'L2_mu4', 'EF_mu4', 
    'L2_mu6',
    'L2_mu10',
    'L2_e10_loose', 'EF_e10_loose', 
    'L2_e10_medium', 'EF_e10_medium', 
    'L2_e15i_medium', 'EF_e15i_medium', 
    'L2_xe20', 'EF_xe20',
    'L2_tau16i_loose', 'EF_tau16i_loose',
    'L2_tau16i_4j23', 'EF_tau16i_4j23',
    'L2_J23', 'EF_J50',
    'L2_J5', 'EF_J5',
    'L2_b35', 'EF_b35',
    'L2_e6_medium',
    
    'L2_e25i_medium1_passEF',
    'L2_e10_mu6',
    'L2_e25i_medium1_passL2',
    'L2_e6_medium1',
    'L2_e5_e10_medium',
    'L2_e10_loose_passEF',
    'L2_e15i_medium_passEF',
    'L2_e15_medium',
    'L2_em20_xe15_PT',
    'L2_e12_medium',
    'L2_e15_medium_passL2',
    'L2_e105_loose1',
    'L2_e20_loose_passEF',
    'L2_e7_medium',
    'L2_e15i_medium',
    'L2_e17i_medium',
    'L2_e20_medium1',
    'L2_e15_loose',
    'L2_e20_xe15',
    'L2_e6_medium',
    'L2_e20_medium1_passL2',
    'L2_e25i_loose_passL2',
    'L2_e25i_loose_passEF',
    'L2_e5_medium',
    'L2_e55_medium2',
    'L2_e10_loose',
    'L2_e10_xe30',
    'L2_e55_loose1',
    'L2_e10Ringer',
    'L2_e15_loose_passL2',
    'L2_e15_loose_passEF',
    'L2_e20_g20',
    'L2_e15i_loose',
    'L2_e20i_medium1_passEF',
    'L2_e5_medium1',
    'L2_e15_medium_passEF',
    'L2_e20_loose_passL2',
    'L2_em20_passHLT',
    'L2_e20i_medium1_passL2',
    'L2_e15i_medium_passL2',
    'L2_em15_passHLT',
    'L2_e22i_tight',
    'L2_em20i_passHLT',
    'L2_e10_medium',
    'L2_e10_loose_passL2',
    'L2_em105_passHLT',
    'L2_e20i_loose',
    'L2_e20_loose',
    'L2_e25i_medium1',
    'L2_em15i_passHLT',
    'L2_eNoCut',
    'L2_e20_medium1_passEF',
    'L2_e140_loose1',
    'L2_e25_medium1',
    'L2_e5_e7_medium',
    'L2_e20i_medium1',
    'L2_e15_xe20',
    'L2_e20_xe30',
    'L2_e25i_loose',
    'L2_e25_loose',
    'L2_e20i_loose_passL2',
    'L2_e20i_loose_passEF',

    ]
job += myalgo

## from TrigNtMenu.TrigNtMenuConf import *
## job.CBNT_AthenaAware += CBNTAA_TrigNtMenu()
## include('TrigNtMenu/TrigNtMenuDict_joboptions.py')

## ServiceMgr.THistSvc.Output += [
##     "MenuHist DATAFILE='menuhist.root' OPT='RECREATE'"
##     ]

## ServiceMgr.ToolSvc.HLTAccessTool.OutputLevel = DEBUG
## job.CBNT_AthenaAware.CBNTAA_TrigNtMenu.OutputLevel = DEBUG

ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = DEBUG
# ServiceMgr.ToolSvc.Navigation.OutputLevel = VERBOSE

# ServiceMgr.StoreGateSvc.Dump = True

print ServiceMgr
print ServiceMgr.ToolSvc

print 'topAlgos: '
print job



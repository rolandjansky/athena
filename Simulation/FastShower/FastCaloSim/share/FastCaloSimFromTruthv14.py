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
# AllAlgs, DetDescrVersion and other flags 
# needs be set before the include, since several
# secondary flags are configured according to that one
#

# DetDescrVersion = "ATLAS-CSC-02-00-00" # default
# DetDescrVersion = "ATLAS-CSC-01-02-00" 
# DetDescrVersion = "ATLAS-DC3-02" # 11.0.42 files 

#AllAlgs = True # if false, all algorithms are switched off by defaults

doAtlfast = False
doCaloCluster = True
doEmCluster = True
doCaloTopoCluster = True
doEgamma = True
doJetRec = True
doTauRec = True


doCBNT = True
doAODall = False

#readESD = True
#OutputLevel = INFO

#doDumpTES = True
#doDumpPoolInputContent = True
#doDumpMC = True
#doDumpProperties = True
#doDumpTDS = True

if "PoolRDOInput" not in dir() :
    #PoolRDOInput=["ZEBRA.P"]
    #PoolESDInput=["/afs/physik.uni-freiburg.de/home/duhrssen/recon/fullsim/ESD/esd-13.0.10/runRECO/mc12.005144.PythiaZee.ESD.RDO.v12000003._00001.pool.root"]
    #PoolESDInput=["/afs/physik.uni-freiburg.de/home/duhrssen/recon/mcroot-12.0.6/FastCaloSimEvgen/single_id211_E50000_etamin2.5_etamax4.0.evgen.root"]
    #PoolESDInput=["/afs/physik.uni-freiburg.de/home/duhrssen/recon/mcroot-12.0.6/FastCaloSimEvgen/single_id22_E50000_etamin-5_etamax5.evgen.root"]
    #PoolESDInput=["/tmp/mduehrss/mc12.005200.T1_McAtNlo_Jimmy.evgen.EVNT.v12000604_tid008066._00216.pool.root.3"]
    #PoolESDInput=["/afs/physik.uni-freiburg.de/data/atlas/fullsim/12.0.x-1mm/005200-recov12/EVGEN/mc12.005200.T1_McAtNlo_Jimmy.evgen.EVNT.v12000605_tid008833._00001.pool.root.1"]
    PoolRDOInput=["/castor/cern.ch/user/g/groll/evnt/005144/EVNT.017988._00001.pool.root.2"]

if "EvtMax" not in dir() :
    EvtMax=1

# if you intend to use the trigger, maybe only the lvl1 calo trigger
# please use the python script: FastCaloSimFromTruthv14WithTrigT1Calo.py

if "doTrigger" not in dir() :
    doTrigger = False # for example do not run trigger simulation

doTruth=True
# number of event to process
# EvtMax=50


# include my own algorithm(s)
# UserAlgs=[ "MyPackage/MyAlgorithm_jobOptions.py" ] 


# By default write ESD, AOD and TAG simultenaously, but note that in production
#      these should be done in separate steps (results might not be bit-by-bit identical).
doWriteESD=False # uncomment if do not write ESD
doWriteAOD=False # uncomment if do not write AOD
#doAOD=False # uncomment if do not run AOD making algorithms
doWriteTAG=False # uncomment if do not write TAG

# Flags to make it run with new RecExCommon_topOptions.py from RecExCommon-00-07-98
#doTriggerConfigOnly = False
#doESD=False

# if needed to configure AOD building
# see https://twiki.cern.ch/twiki/bin/view/Atlas/UserAnalysisTest#The_AOD_Production_Flags
# from ParticleBuilderOptions.AODFlags import AODFlags


# DetFlags modifications are best set here (uncomment RecExCommon_flags first)

include ("RecExCommon/RecExCommon_flags.py")

from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doFastCaloSim = True
jobproperties.CaloCellFlags.doFastCaloSimNoise = False
DetFlags.makeRIO.Calo_setOn()

# switch off ID, calo, or muons
DetFlags.ID_setOff()
# DetFlags.Calo_setOff()
DetFlags.Muon_setOff()


IOVDbSvc = Service("IOVDbSvc")
IOVDbSvc.GlobalTag="OFLCOND-CSC-01-00-00"

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
#include ("TrigT1CaloConfigVer16.py")

print "=================================="
print "=================================="
print "=================================="
print "=================================="
print "=================================="
print "IOVDbSvc"
print IOVDbSvc
print "=================================="
print "=================================="
print "=================================="
print "=================================="
print "=================================="
print "=================================="

# Move the MC collection to be TruthEvent

theApp.Dlls += [ "HepMCTools"]
theApp.TopAlg = ["CopyMcCollection"] + theApp.TopAlg
CopyMcCollection = Algorithm("CopyMcCollection")
CopyMcCollection.oldCollectionKey="GEN_EVENT"
CopyMcCollection.newCollectionKey="TruthEvent"
CopyMcCollection.deleteOldCollection=True

from AtlfastAlgs.GlobalEventDataGetter import Atlfast__GlobalEventDataGetter
GlobalEventDataMaker = Atlfast__GlobalEventDataGetter().AlgHandle()
GlobalEventDataMaker.McLocation       = "TruthEvent"

from FastCaloSim.FastCaloSimConf import FastShowerCellBuilderTool
theFastShowerCellBuilderTool=FastShowerCellBuilderTool()
print theFastShowerCellBuilderTool

from FastCaloSim.FastCaloSimConf import EmptyCellBuilderTool
theEmptyCellBuilderTool=EmptyCellBuilderTool()

from JetRec.JetRecConf import JetAlgorithm
jetcolls=["Cone7H1TopoJets","Cone4H1TopoJets","Kt6H1TopoJets","Kt4H1TopoJets"]
for jetalg in jetcolls:
    try:
        exec 'theJetAlg=JetAlgorithm("%s")' % (jetalg)
        exec 'theJetAlg.Input.ContainerPostfix="%s"' % (jetalg)
        print theJetAlg.Input
    except:
        print 'JetCollection %s not found...' % (jetalg)



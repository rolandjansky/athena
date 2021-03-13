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


# DetDescrVersion="ATLAS-CSC-01-02-00" # default
# DetDescrVersion="ATLAS-DC3-02" # 11.0.42 files 
if "DetDescrVersion" not in dir() :
    DetDescrVersion="ATLAS-CSC-01-02-00"
    
AllAlgs = True # if false, all algorithms are switched off by defaults 

doAtlfast = True
doCaloCluster = True
doEmCluster = True
doCaloTopoCluster = True
doEgamma = True
doJetRec = True
doTauRec = True
doMissingET = True

doCBNT = True
doAODall = False

#readESD = True
#OutputLevel = DEBUG

#doDumpTES = True
#doDumpPoolInputContent = True
#doDumpMC = True
#doDumpProperties = True
#doDumpTDS = True

if "PoolRDOInput" not in dir() :
    #PoolESDInput=["ZEBRA.P"]
    #PoolESDInput=["/afs/physik.uni-freiburg.de/home/duhrssen/recon/fullsim/ESD/esd-13.0.10/runRECO/mc12.005144.PythiaZee.ESD.RDO.v12000003._00001.pool.root"]
    PoolRDOInput=["/afs/cern.ch/atlas/offline/data/testfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root"]

if "EvtMax" not in dir() :
    EvtMax=100

doTrigger = False # for example do not run trigger simulation
# doTruth=False
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
doTriggerConfigOnly = False
doESD=False

# if needed to configure AOD building
# see https://twiki.cern.ch/twiki/bin/view/Atlas/UserAnalysisTest#The_AOD_Production_Flags
# from ParticleBuilderOptions.AODFlags import AODFlags


# DetFlags modifications are best set here (uncomment RecExCommon_flags first)

include ("RecExCommon/RecExCommon_flags.py")

from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doFastCaloSim = True
jobproperties.CaloCellFlags.doFastCaloSimNoise = True
#jobproperties.CaloCellFlags.doFastCaloSimAddCells = True;

DetFlags.makeRIO.Calo_setOn()

# switch off ID, calo, or muons
# DetFlags.ID_setOff()
# DetFlags.Calo_setOff()
# DetFlags.Muon_setOff()

# main jobOption

include ("RecExCommon/RecExCommon_topOptions.py")

from FastCaloSim.FastCaloSimConf import FastShowerCellBuilderTool
theFastShowerCellBuilderTool=FastShowerCellBuilderTool()
#theFastShowerCellBuilderTool.OutputLevel=DEBUG

try:
  if doMissingET :
    METAlg.CryoTopo.CalibToolName="H1WeightToolAtlfastIIConeTower"
    METAlg.CryoCone.CalibToolName="H1WeightToolAtlfastIIConeTower"
    METAlg.CellTool.CellWeightTool="H1WeightToolAtlfastIIConeTower"
    METAlg.CellTopo.CalibToolName="H1WeightToolAtlfastIIConeTower"
    METRefAlg.METRefEle.CalibToolName         = "H1WeightToolAtlfastIIConeTower"
    METRefAlg.METRefGamma.CalibToolName       = "H1WeightToolAtlfastIIConeTower"
    METRefAlg.METRefTau.CalibToolName        = "H1WeightToolAtlfastIIConeTower"
    METRefAlg.METRefJet.CalibToolName        = "H1WeightToolAtlfastIIConeTower"
    METRefAlg.METClusterOut.CalibToolName    = "H1WeightToolAtlfastIIConeTower"
    METRefAlg.METCellOut.CalibToolName       = "H1WeightToolAtlfastIIConeTower"
except:
  print "doMissingET no longer defined"


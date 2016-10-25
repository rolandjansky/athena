
import commands

res=commands.getoutput('ls CaloTests_GeomTag*.txt')

f = open(res)
THE_GEOMETRY=f.readlines()
f.close()

print "The geometry is ", THE_GEOMETRY[0]



AllAlgs = False # if false, all algorithms ae switched off by defaults




from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.Luminosity.set_zero() # or set_high()

EvtMax=1
doNameAuditor=False

doWriteESD = False
doWriteAOD = False
doWriteTAG = False
doAOD = False
doTruth = True
doCaloNoise = True
doCaloTopoCluster = True
doTopoClusterCBNT = True
doTrigger=False
doPerfMon=False

doMoore=False
doMuonboy=False

RootNtupleOutput='ntuple_elecnoise.root'

include ("RecExCommon/RecExCommon_flags.py")



from CaloRec.CaloTopoClusterFlags import jobproperties
jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib.set_Value(True)

# switch off useless detectors
DetFlags.ID_setOff()
DetFlags.Muon_setOff()

jobproperties.Global.DetDescrVersion=THE_GEOMETRY[0]

include ("RecExCommon/RecExCommon_topOptions.py")

EventSelector.InputCollections = [ "Dig.pool.root" ]

from CaloRec.CaloRecConf import CaloTopoClusterMaker, CaloTopoClusterSplitter

TopoMaker = CaloTopoClusterMaker("TopoMaker")
TopoMaker.NeighborThresholdOnEorAbsEinSigma =    -1
TopoMaker.SeedThresholdOnEorAbsEinSigma  =    -1

TopoSplitter = CaloTopoClusterSplitter("TopoSplitter")
TopoSplitter.EnergyCut = 100*GeV
TopoSplitter.ShareBorderCells = False


theCBNT_CaloClusterTopo = CBNTAA_CaloCluster( "CBNT_CaloClusterTopo" )
theCBNT_CaloClusterTopo.AddCellDetails = TRUE
theCBNT_CaloClusterTopo.MaxCell = 200000
theCBNT_CaloClusterTopo.Suffix = "_topo"
theCBNT_CaloClusterTopo.UseLink=TRUE


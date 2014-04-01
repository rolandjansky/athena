# //////////////////////////////////////////////////////////////////
#
# Job Options to... start from a (D)ESD, run eflowRec to create a eflowRecContainer in the transient
#                   AOD along with the calibration hit information.
#                   Then dump the contents into a D3PD
#
#To be verified:
#   These JO can be submitted to the GRID by the corresponding script in the scripts folder
# //////////////////////////////////////////////////////////////////



from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax=200
athenaCommonFlags.FilesInput=["/tmp/limbach/mc12_8TeV.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1479_s1470_r3578_tid783779_00/ESD.783779._000975.pool.root.1"]



## ////////////////////////////////////////////////
## eflowRec Part
from RecExConfig.RecFlags import rec
rec.doTruth.set_Value_and_Lock( False )
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False) #explicitly disable for GRID jobs, use offline in testing for checks
rec.doAOD.set_Value_and_Lock(True)

# enable eflowRec
from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doEFlow.set_Value_and_Lock(True)

# change some calo flags (Mark's JO)
from CaloRec.CaloRecFlags import jobproperties as CaloRecJP
CaloRecJP.CaloRecFlags.Enabled.set_Value_and_Lock(True)
CaloRecJP.CaloRecFlags.doCaloCluster.set_Value_and_Lock(True)
CaloRecJP.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True)

# set eflowRec to tauMode
from eflowRec.eflowRecFlags import jobproperties as eflowRecJP
eflowRecJP.eflowRecFlags.runTauMode.set_Value_and_Lock(True)

# set noise fags (Mark's JO)
from CaloTools.CaloNoiseFlags import jobproperties as CaloNoiseJP
CaloNoiseJP.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(1.45)



## ////////////////////////////////////////////////
## Execution part

#...add truth info in D3PD
from RecExConfig.RecFlags import rec
rec.doTruth.set_Value_and_Lock( True )

include( "RecExCommon/RecExCommon_topOptions.py" )

# Add eflowContainers
# Build AOD objects and write them out to a POOL file
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
StreamAOD = AthenaPoolOutputStream( "StreamAOD" )

import AthenaCommon.PropertiesManip as manip
manip.appendItemsToList(StreamAOD,'ItemList','eflowObjectContainer#eflowObjects_tauMode')
manip.appendItemsToList(StreamAOD,'ItemList','CaloClusterContainer#eflowClusters_tauMode')

#...add the calibration hit maker to create its container
#   so that it can be dumped later by calCelInfoD3PDMaker
from MakeCellInfo.MakeCellInfoConf import MakeCalCellInfo
calCellInfo = MakeCalCellInfo()
print("(PanTau_ESD-to-eflowStudyD3PD_TopOptions.py) Setting EflowObjectsName to eflowObjects_tauMode")
calCellInfo.EflowObjectsName = "eflowObjects_tauMode"
calCellInfo.EflowClustersName = "eflowClusters_tauMode"
calCellInfo.TruthParticlesName = "INav4MomTruthEvent"
topSequence+=calCellInfo


## ////////////////////////////////////////////////
## Define the D3PD Stream

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
alg = MSMgr.NewRootStream("EFLOWD3PDSTREAM", "eflowStudyD3PD_CalHits3_WithMCBlock.root", "eflowD3PD")

# algs to run prior to dumping
from eflowD3PDMaker.eflowD3PDMakerConf import eflowClusterSelector
eflowClusSelector = eflowClusterSelector()
topSequence += eflowClusSelector

from eflowRec.eflowTrackToCaloTrackExtrapolatorTool import eflowTrackToCaloTrackExtrapolatorToolDefault
TrackToCaloTrackExtrapolatorToolDefault=eflowTrackToCaloTrackExtrapolatorToolDefault()


#Configure the D3PD
from D3PDMakerCoreComps.D3PDObject import D3PDObject

#### The basic kinematics ####
from eflowD3PDMaker.eflowD3PDObject import *
eflowD3PDObject = D3PDObject (makeeflowD3PDObject, 'eflowBase_')
eflowD3PDObject.defineBlock(0, 'Kinematics',
                            eflowD3PDMakerConf.D3PD__eflowFillerTool)
eflowD3PDObject.defineBlock (0, 'Details',
                            eflowD3PDMakerConf.D3PD__eflowObjectDetailsFillerTool)
alg += eflowD3PDObject(10)


#### The track variables ####
from eflowD3PDMaker.eflowTrackD3PDObject import *
eflowTrackD3PDObject = D3PDObject (makeeflowTrackD3PDObject, 'eflowTrack_')
eflowTrackD3PDObject.defineBlock(0,'eflowTrack',
                            eflowD3PDMakerConf.D3PD__eflowTrackFillerTool,
                            eflowTrackToCaloTrackExtrapolatorTool = TrackToCaloTrackExtrapolatorToolDefault)
alg += eflowTrackD3PDObject(10)


#### The calibration hits ####
from calCellInfoD3PDMaker.calCellInfoD3PDObject import *
calCellInfoD3PDObject = D3PDObject(makecalCellInfoD3PDObject,'CalCellInfo_')

calCellInfoD3PDObject.defineBlock(0,'CallCellInfo',
                            calCellInfoD3PDMakerConf.D3PD__calCellInfoFillerTool)
alg += calCellInfoD3PDObject(10)


#### Prepare for truth info ####
from D3PDMakerConfig.D3PDMakerFlags                 import D3PDMakerFlags
from TruthD3PDAnalysis.truthParticleConfig          import truthParticleConfig
D3PDMakerFlags.TruthParticlesSGKey = 'INav4MomTruthEvent'
truthParticleConfig(topSequence, sgkey=D3PDMakerFlags.TruthParticlesSGKey(), writeHadrons=True, writePartons=True, writeGeant=False)

#### Even more truth information ####
from TruthD3PDMaker.GenEventD3PDObject              import GenEventD3PDObject
from TruthD3PDMaker.TruthParticleD3PDObject         import TruthParticleD3PDObject
alg += TruthParticleD3PDObject (10, sgkey=D3PDMakerFlags.TruthParticlesSGKey())#"INav4MomTruthEvent")#sgkey=D3PDMakerFlags.TruthParticlesSGKey()
#alg += GenEventD3PDObject (10)









# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Job options file for Dumping LArHit to d3pd
#--------------------------------------------------------------
import math
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('geant4')
globalflags.InputFormat.set_Value_and_Lock('pool')

from AthenaCommon.JobProperties import jobproperties
#jobproperties.Global.DetDescrVersion = "ATLAS-GEO-18-00-00"   # check on AMI what geometry the G4 simulations are made with...
jobproperties.Global.DetDescrVersion = "ATLAS-IBL-03-00-00"
jobproperties.Global.ConditionsTag = "OFLCOND-SDR-BS14T-IBL-CORR-06-02"

print("**** JOBPROPERTIES.GLOBAL")
print(jobproperties.Global)
from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn() 
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()
#DetFlags.Print()

#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.EvtMax.set_Value_and_Lock(50)
theApp.EvtMax=500 
 
#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
#Get identifier mapping
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )


#include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
# The following block replaces the include above...
from LArConditionsCommon.LArCondFlags import larCondFlags 
LArDBConnection = ""
LArDB = "LAR_OFL"
if larCondFlags.LArDBConnection.statusOn  :
  LArDBConnection = larCondFlags.LArDBConnection()
  LArDB = ""
larCondFlags.config_ElecCalibMC()
if svcMgr.MessageSvc.OutputLevel <= DEBUG :
  print(larCondFlags)
from IOVDbSvc.CondDB import conddb
larCondDBFolders = ["/LAR/ElecCalibMC/fSampl"]
for i in larCondDBFolders :
  conddb.addFolder(LArDB,i+LArDBConnection)
  ## allow onverride
  larCondFlags.addTag(i,conddb)  
## apply hierarchical tag                    
larCondFlags.addTag('/LAR/ElecCalibMC',conddb)



## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSeq += xAODMaker__EventInfoCnvAlg()

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.MessageSvc.OutputLevel = INFO
#svcMgr.MessageSvc.defaultLimit = 100000 # 999999

bdir = "/usatlas/groups/bnl_local2/flanni/HITS/"
#"mc12_14TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1133_s1638_s1659_tid01222101_00/"
#"mc12_14TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1133_s1638_s1659_tid01222102_00/" 
bdsdir = "mc12_14TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e1133_s1638_s1659_tid01222103_00/" 
bfile="HITS.01222103._001912.pool.root.1"


if "inputFile" not in dir():
  inputFile = bdir + bdsdir + bfile


svcMgr.EventSelector.InputCollections = [inputFile]
  
if "outputFile" not in dir():
    import re
    boutfile = re.sub(r'root.[0-9]','root', bfile.replace("pool","NTUP_SCHIT"))
    tuple_name = bdir+"NTUP_SCHITS/"+boutfile
else:
    tuple_name = outputFile

from D3PDMakerCoreComps.MakerAlg import *
from EventCommonD3PDMaker.EventInfoD3PDObject import *
alg = MakerAlg("caloD3PD", topSequence, file = tuple_name , D3PDSvc = 'D3PD::RootD3PDSvc')
alg += EventInfoD3PDObject (10)


from CaloSysD3PDMaker.LArSCHitD3PDObject import *
from CaloSysD3PDMaker.LArHitD3PDObject import *

myEtaCut = [0.4,0.5]
myPhiCut = [-math.pi,math.pi]
myLayers = [0,1,2,3]
# myDetectors = [1]
#alg += LArHitEMBD3PDObject (10)
# This works...
# alg += LArSCHitD3PDObject(0)
alg += LArSCHitD3PDObject(0, prefix='hembsc_',
                          CaloEtaCut=myEtaCut, CaloPhiCut=myPhiCut,
                          CaloLayers=myLayers)

alg += LArSCHitD3PDObject(0, prefix='hemecsc_', sgKey="LArHitEMEC",
                          CaloLayers=myLayers)

alg += LArSCHitD3PDObject(0, prefix='hhecsc_', sgKey="LArHitHEC",
                          CaloLayers=myLayers)

alg += LArSCHitD3PDObject(0, prefix='hfcalsc_', sgKey="LArHitFCAL",
                          CaloLayers=myLayers)

# CaloDetectors=myDetectors)

print(" All Top Algorithms ", theApp.TopAlg)

        

                                        

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
###############################################################
#
# Job options file to run Digitization
#
#==============================================================
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOn()
DetFlags.Muon_setOff()
DetFlags.digitize.LAr_setOn()
DetFlags.makeRIO.LAr_setOn()

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('geant4')


include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "PartPropSvc/PartPropSvc.py" )
#
# Pool Converters
#
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
include( "G4SimAthenaPOOL/G4SimAthenaPOOL_joboptions.py" )

# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
# Get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp
#
# Pool input
#
# With these two files there are
#     5000 lowPt events
#      500 highPt event
#
#   the cross-section weights are 39.8606 and  0.138128
#  to take into account the difference in numbers of events (factor 10 )the weights should be
#       39.8606/10  = 3.98606
#  and   0.13818/1  = 0.13818
#
svcMgr.EventSelector.InputCollections = [
"mc15_13TeV.119995.Pythia8_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e3432_s2081_s2132_tid04838062_00/HITS.04838062._000027.pool.root.1",
"mc15_13TeV.119996.Pythia8_A2MSTW2008LO_minbias_inelastic_high.merge.HITS.e3432_s2081_s2132_tid04838063_00/HITS.04838063._000833.pool.root.1"
]

# the Tile, LAr and Calo detector description package
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-R2-2015-02-01-00'


from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include("TileConditions/TileConditions_jobOptions.py" )
include("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")

from LArRecUtils.LArMCSymCondAlg import LArMCSymCondAlgDefault
LArMCSymCondAlgDefault()

from CaloCondPhysAlgs.CaloCondPhysAlgsConf import LArMinBiasAlg
larMinBiasAlg = LArMinBiasAlg()
larMinBiasAlg.datasetID_lowPt =119995
larMinBiasAlg.datasetID_highPt=119996
larMinBiasAlg.weight_lowPt =3.98606
larMinBiasAlg.weight_highPt=0.138128

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += larMinBiasAlg


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#-------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO

if not hasattr(ServiceMgr, 'THistSvc'):
   from GaudiSvc.GaudiSvcConf import THistSvc
   ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output  = ["file1 DATAFILE='ntuple.root' OPT='RECREATE'"]

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1000000
theApp.EvtSel = "EventSelector"

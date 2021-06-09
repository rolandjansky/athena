#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# job options file to make SiHitAnalysis ntuples for HGTD from existing HITS inputs

from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool

include("ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include("EventAthenaPool/EventAthenaPool_joboptions.py" )

# set input file
from glob import glob
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = glob( "../../21.9/s3656_merging/800380.merged.HITS.pool.root" )
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

# set how many events to process
theApp.EvtMax = -1

# autoconfigure from input
from RecExConfig import AutoConfiguration
AutoConfiguration.ConfigureSimulationOrRealData()
AutoConfiguration.ConfigureGeo()
AutoConfiguration.ConfigureConditionsTag()
AutoConfiguration.ConfigureFieldAndGeo()

# make sure the HGTD is on
from AthenaCommon.DetFlags import DetFlags
DetFlags.geometry.HGTD_setOn()
DetFlags.HGTD_setOn()

# add preIncludes
include("InDetSLHC_Example/preInclude.SLHC.py")
include("InDetSLHC_Example/preInclude.SiliconOnly.py")
include("InDetSLHC_Example/preInclude.SLHC_Setup.py")
include("InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py")

# actually load the geometry
include("RecExCond/AllDet_detDescr.py") 

# set up algorithm sequence, add HGTDHitAnalysis, and configure it
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from HitAnalysis.HitAnalysisConf import SiHitAnalysis
topSequence += SiHitAnalysis('HGTDHitAnalysis')
topSequence.HGTDHitAnalysis.CollectionName='HGTD_Hits'
topSequence.HGTDHitAnalysis.HistPath='/SiHitAnalysis/histos/'
topSequence.HGTDHitAnalysis.NtupleFileName='/SiHitAnalysis/ntuples/'
topSequence.HGTDHitAnalysis.HistPath='/SiHitAnalysis/'
topSequence.HGTDHitAnalysis.isHGTD = True
topSequence.HGTDHitAnalysis.ExpertMode= "off"

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += [ "SiHitAnalysis DATAFILE='SiHitAnalysis.HGTD.root' OPT='RECREATE'" ]

ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 9999999
ServiceMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE

# add postIncludes
include("InDetSLHC_Example/postInclude.SLHC_Setup_ITK.py")

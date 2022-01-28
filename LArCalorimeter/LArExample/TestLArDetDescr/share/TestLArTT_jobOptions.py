# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

###############################################################
#
# jobOptions file for LArDetDescr loading and LArTT tests
#==============================================================
import AthenaCommon.AtlasUnixStandardJob
theApp.EvtMax = 1

# Set output level threshold 
#(0=ALL 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.MessageSvc.OutputLevel               = 3
svcMgr.MessageSvc.defaultLimit = 9999999  # all messages

# make sure that other subdet are not built
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('geant4')
globalflags.Luminosity.set_Value_and_Lock('zero')

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.LAr_setOn()

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = 'ATLAS-R2-2016-01-00-01'
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

include( "LArIdCnv/LArIdCnv_joboptions.py" )
include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from TestLArDetDescr.TestLArDetDescrConf import TestLArTT
topSequence += TestLArTT()

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-MC16-SDR-RUN2-08')

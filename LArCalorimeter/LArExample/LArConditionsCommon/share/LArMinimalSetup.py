# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

###########################################################
# LArMinimalSetup.py:                                     #
# Minimal Athena setup for LAr Calibration and Monitoring #
# Contains Identifier dictionaries, LArCablingSvc,        #
# ByteStreamEventSelector but NO GEOMETRY                 #
# Author: Walter Lampl                                    #
###########################################################

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
globalflags.DatabaseInstance.set_Value_and_Lock('CONDBR2')

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-R2-2016-01-00-01"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()  #Switched back  geometry
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()
#DetFlags.Print()

#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

#Get identifier mapping
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )

#Get ByteStream Event Selector
if 'online' not in dir() or not online:
  include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )

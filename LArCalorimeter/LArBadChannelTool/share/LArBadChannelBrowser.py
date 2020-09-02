# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @file MyPyAna/share/myjob.py
## @purpose steering job option file for MyAlg

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_commis()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_bytestream()

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-Comm-00-00-00"

#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


#Get identifier mapping
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )

include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

#include("CaloIdCnv/CaloIdCnv_joboptions.py")

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

## import my algorithm and add it to the list of algorithms to be run
from LArBadChannelTool.LArBadChannelBrowserLib import LArBadChannelBrowserAlg
job += LArBadChannelBrowserAlg('badchanAlg')


## get a handle on the application manager
from AthenaCommon.AppMgr import theApp


theApp.EvtMax = 1

## EOF ##

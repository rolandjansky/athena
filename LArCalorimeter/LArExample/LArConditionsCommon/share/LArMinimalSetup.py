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
jobproperties.Global.DetDescrVersion = "ATLAS-R2-2015-04-00-00"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()  #Switched off to avoid geometry
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


theApp.Dlls += [ "LArByteStream"]


# Left to do after that:
#
# Specify input file:
#
# theByteStreamInputSvc=svcMgr.ByteStreamInputSvc
# theByteStreamInputSvc.InputDirectory = [InputDir]
# theByteStreamInputSvc.FilePrefix     = FilePrefix
# theByteStreamInputSvc.RunNumber      = [RunNumber]
# theByteStreamInputSvc.RunNumber      = RunNumberList
# Alternative:
# theByteStreamInputSvc..FullFileName=["/mydir/myfile.data"]
#
# Specify the object you want to read from ByteStream
#
# theByteStreamAddressProviderSvc = svcMgr.ByteStreamAddressProviderSvc
# theByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/HIGH"]
# theByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/MEDIUM"]
# theByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/LOW"]
# theByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
# 
# ... and the algorithms you want to run.

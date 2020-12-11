# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# LAr GeoModel initialization
#
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from AthenaCommon import Logging
from AtlasGeoModel.AtlasGeoDBInterface import AtlasGeoDBInterface
from LArGeoAlgsNV import LArGeoDB

# -------------------------------------------------------------------------------------
#  Geometry jobproperties defined for the InDet geometry
# -------------------------------------------------------------------------------------

class Sagging(JobProperty):
     """ Sagging flag"""
     statusOn     = True
     allowedTypes = ['int','NoneType']
     StoredValue  = None

class BarrelOn(JobProperty):
     """ Barrel_On flag """
     statusOn     = True
     allowedTypes = ['int','NoneType']
     StoredValue  = None

class EndcapOn(JobProperty):
     """ Endcap_ON flag """
     statusOn     = True
     allowedTypes = ['int','NoneType']
     StoredValue  = None

class DetailedAbsorber(JobProperty):
     """ detailed absorber"""
     statusOn     = True
     allowedTypes = ['int','NoneType']
     StoredValue  = None

class DetailedAbsorber_EC(JobProperty):
     """ detailed absorber EC"""
     statusOn     = True
     allowedTypes = ['int','NoneType']
     StoredValue  = None

class FCalGeoType(JobProperty):
     """ FCal geotype"""
     statusOn     = True
     allowedTypes = [str]
     StoredValue  = "UNDEFINED"


# add to jobproperties
class LArGeometryFlags_JobProperties(JobPropertyContainer):
    """ The geometry flag/job property container """

    def __init__(self, context=""):
        JobPropertyContainer.__init__(self,context)
        return

    def setupValuesFromDB(self,geoTagName=None):

        dbGeomCursor = AtlasGeoDBInterface(geoTagName or globalflags.DetDescrVersion())
        dbGeomCursor.ConnectAndBrowseGeoDB()
        params = LArGeoDB.InitializeGeometryParameters(dbGeomCursor)

        self.Sagging.set_Value_and_Lock(params["Sagging"])
        self.BarrelOn.set_Value_and_Lock(params["BarrelOn"])
        self.EndcapOn.set_Value_and_Lock(params["EndcapOn"])
        self.DetailedAbsorber.set_Value_and_Lock(params["DetAbs"])
        self.DetailedAbsorber_EC.set_Value_and_Lock(params["DetAbs_EC"])
        self.FCalGeoType.set_Value_and_Lock(params["FCal_GeoType"])


    def reset(self):

        self.Sagging.unlock()
        self.BarrelOn.unlock()
        self.EndcapOn.unlock()
        self.DetailedAbsorber.unlock()
        self.DetailedAbsorber_EC.unlock()
        self.FCalGeoType.unlock()

    def dump(self):

        Logging.log.info("Sagging              = %s", self.Sagging())
        Logging.log.info("Barrel_On            = %s", self.BarrelOn())
        Logging.log.info("Endcap_On            = %s", self.EndcapOn())
        Logging.log.info("Detailed absorber    = %s", self.DetailedAbsorber())
        Logging.log.info("Detailed absorber EC = %s", self.DetailedAbsorber_EC())

        Logging.log.info("FCal GeoType         = %s", self.FCalGeoType())


jobproperties.add_Container(LArGeometryFlags_JobProperties)
jobproperties.LArGeometryFlags_JobProperties.add_JobProperty(Sagging)
jobproperties.LArGeometryFlags_JobProperties.add_JobProperty(BarrelOn)
jobproperties.LArGeometryFlags_JobProperties.add_JobProperty(EndcapOn)
jobproperties.LArGeometryFlags_JobProperties.add_JobProperty(DetailedAbsorber)
jobproperties.LArGeometryFlags_JobProperties.add_JobProperty(DetailedAbsorber_EC)
jobproperties.LArGeometryFlags_JobProperties.add_JobProperty(FCalGeoType)

LArGeometryFlags = jobproperties.LArGeometryFlags_JobProperties
LArGeometryFlags.setupValuesFromDB()


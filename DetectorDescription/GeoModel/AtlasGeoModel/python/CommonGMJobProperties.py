# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#
# Common Geometry job property base class
#

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from AthenaCommon import Logging
from AtlasGeoModel.AtlasGeoDBInterface import AtlasGeoDBInterface
from AtlasGeoModel import CommonGeoDB

# ------------------------------------------------------------------------------------------------------------------
#  Geometry jobproperties defined for the common geometry (invented by the ID - no calo and muon specific flags ...)
# ------------------------------------------------------------------------------------------------------------------

class Run(JobProperty):
     """ RUN1/RUN2 parameter  """
     statusOn     = True
     allowedTypes = ['str']
     StoredValue  = "UNDEFINED"

class GeoType(JobProperty):
     """ Geometry type ( ITKLoI, ITkLoI-VF, etc...) """
     statusOn     = True
     allowedTypes = ['str']
     StoredValue  = "UNDEFINED"

class CommonGeometryFlags_JobProperties(JobPropertyContainer):
    """ The geometry flag/job property container """

    def __init__(self, context=""):
        JobPropertyContainer.__init__(self,context)
        return


    def setupValuesFromDB(self, geoTagName=None):

        dbGeomCursor = AtlasGeoDBInterface(geoTagName or globalflags.DetDescrVersion())
        dbGeomCursor.ConnectAndBrowseGeoDB()
        params = CommonGeoDB.InitializeGeometryParameters(dbGeomCursor)

        self.Run.set_Value_and_Lock(params["Run"])
        self.GeoType.set_Value_and_Lock(params["GeoType"])


    def reset(self):
        
        self.Run.unlock()
        self.GeoType.unlock()


    def dump(self):

        Logging.log.info("RUN flag      : %s", self.Run())
        Logging.log.info("GeoType flag  : %s", self.GeoType())


jobproperties.add_Container(CommonGeometryFlags_JobProperties)
jobproperties.CommonGeometryFlags_JobProperties.add_JobProperty(Run)
jobproperties.CommonGeometryFlags_JobProperties.add_JobProperty(GeoType)

CommonGeometryFlags = jobproperties.CommonGeometryFlags_JobProperties
CommonGeometryFlags.setupValuesFromDB()

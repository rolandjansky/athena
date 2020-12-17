# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# InDet GeoModel initialization
#
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from AtlasGeoModel.AtlasGeoDBInterface import AtlasGeoDBInterface
from PixelGeoModel import PixelGeoDB
from AthenaCommon import Logging


# -------------------------------------------------------------------------------------
#  Geometry jobproperties defined for the InDet geometry
# -------------------------------------------------------------------------------------

class GeoVersionName(JobProperty):
     """ version name DC3,IBL,SLHC,... """
     statusOn     = True
     allowedTypes = ['str']
     StoredValue  = 'UNDEFINED'

class GeoLayout(JobProperty):
     """ geometry layout Final, IBL. SLHC,... """
     statusOn     = True
     allowedTypes = ['str']
     StoredValue  = 'UNDEFINED'

class isIBL(JobProperty):
     """ IBL layer defined or not True/False"""
     statusOn     = True
     allowedTypes = ['bool']
     StoredValue  = False

class IBLLayout(JobProperty):
     """ IBL layer layout : noIBL, planar (all modules are planar),  3D (hybrid geometry) """
     statusOn     = True
     allowedTypes = ['str']
     allowedValues=["planar","3D","noIBL","UNDEFINED"]
     StoredValue  = "UNDEFINED"

class isSLHC(JobProperty):
     """ SLHC geometry"""
     statusOn     = True
     allowedTypes = ['bool']
     StoredValue  = False

class isDBM(JobProperty):
     """ DBM monitoring system defined or not"""
     statusOn     = True
     allowedTypes = ['bool']
     StoredValue  = False

class useDynamicAlignFolders(JobProperty):
    """ Use to turn on dynamic alignment constants folder scheme (first deployed in 2016) """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True



# add to jobproperties
class InDetGeometryFlags_JobProperties(JobPropertyContainer):
    """ The geometry flag/job property container """

    def __init__(self, context=""):
        JobPropertyContainer.__init__(self,context)
        return

    def setupValuesFromDB(self,geoTagName=None):

        dbGeomCursor = AtlasGeoDBInterface(geoTagName or globalflags.DetDescrVersion())
        dbGeomCursor.ConnectAndBrowseGeoDB()
        params = PixelGeoDB.InitializeGeometryParameters(dbGeomCursor)

        self.isSLHC.set_Value_and_Lock(params["SLHC"] )
        self.isIBL.set_Value_and_Lock(params["IBL"] )
        self.isDBM.set_Value_and_Lock(params["DBM"])
        self.IBLLayout.set_Value_and_Lock(params["IBLlayout"])
        self.GeoVersionName.set_Value_and_Lock(params["VersionName"])
        self.GeoLayout.set_Value_and_Lock(params["Layout"])

    def reset(self,geoTagName="none"):
        
        self.isSLHC.unlock()
        self.isIBL.unlock()
        self.isDBM.unlock()
        self.IBLLayout.unlock()
        self.GeoVersionName.unlock()
        self.GeoLayout.unlock()
        self.useDynamicAlignFolders.unlock()


    def dump(self):

        Logging.log.info("VersionName = %s", self.GeoVersionName())
        Logging.log.info("Layout      = %s", self.GeoLayout())
        Logging.log.info("DBM         = %s", self.isDBM())

        Logging.log.info("SLHC flag : %s", self.isSLHC())
        Logging.log.info("IBL flag   : %s", self.isIBL())
        Logging.log.info("IBL layout : %s", self.IBLLayout())
        Logging.log.info("Dynamic alignment : %s", self.useDynamicAlignFolders())


jobproperties.add_Container(InDetGeometryFlags_JobProperties)
jobproperties.InDetGeometryFlags_JobProperties.add_JobProperty(GeoVersionName)
jobproperties.InDetGeometryFlags_JobProperties.add_JobProperty(GeoLayout)
jobproperties.InDetGeometryFlags_JobProperties.add_JobProperty(isIBL)
jobproperties.InDetGeometryFlags_JobProperties.add_JobProperty(IBLLayout)
jobproperties.InDetGeometryFlags_JobProperties.add_JobProperty(isSLHC)
jobproperties.InDetGeometryFlags_JobProperties.add_JobProperty(isDBM)
jobproperties.InDetGeometryFlags_JobProperties.add_JobProperty(useDynamicAlignFolders)

InDetGeometryFlags = jobproperties.InDetGeometryFlags_JobProperties
InDetGeometryFlags.setupValuesFromDB()



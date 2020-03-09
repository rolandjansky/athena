# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

#
# Common Geometry job property base class
#

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from AthenaCommon import Logging
from AtlasGeoModel.AtlasGeoDBInterface import AtlasGeoDBInterface

class CommonGMFlags:

    def __init__(self, geoTag="none"):

        self.__dict__={}

        # set geometry tag name
        self.__dict__["geomTag"]= globalflags.DetDescrVersion()
        if geoTag!="none": self.__dict__["geomTag"] = geoTag
        
        self.dbGeomCursor = 0
        self.connectToDB()
    
        self.InitializeGeometryParameters()


    def connectToDB(self):

        bVerbose=False
        self.dbGeomCursor = AtlasGeoDBInterface(self.__dict__["geomTag"],bVerbose)
        self.dbGeomCursor.ConnectAndBrowseGeoDB()

        return


    def InitializeGeometryParameters(self):

        # ----------------------------------------------------------------------------    
        # Read versionname, layout and dbm from AtlasCommon table

        dbId,dbCommon,dbParam = self.dbGeomCursor.GetCurrentLeafContent("AtlasCommon")

        _run = "UNDEFINED"
        _geotype = "UNDEFINED"
        _stripgeotype = "UNDEFINED"
        if len(dbId)>0:
            key=dbId[0] 
            if "CONFIG" in dbParam : _run = dbCommon[key][dbParam.index("CONFIG")]
            if "GEOTYPE" in dbParam : _geotype = dbCommon[key][dbParam.index("GEOTYPE")]
            if "STRIPGEOTYPE" in dbParam : _stripgeotype = dbCommon[key][dbParam.index("STRIPGEOTYPE")]
        
        self.__dict__["Run"]=_run
        self.__dict__["GeoType"]=_geotype
        self.__dict__["StripGeoType"]=_stripgeotype


    def getValue(self,name):

        return self.__dict__[name]


    def dump(self):

        Logging.log.info("Geometry tag CommonGMFlags : "+self.__dict__["geomTag"]+" ------------------------------------")

        Logging.log.info("RUN flag      : "+self.__dict__["Run"])
        Logging.log.info("GeoType flag  : "+self.__dict__["GeoType"])
        Logging.log.info("Strip GeoType flag  : "+self.__dict__["StripGeoType"])


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

class StripGeoType(JobProperty):
     """ Geometry strip type ( disc, petal,  etc...) """
     statusOn     = True
     allowedTypes = ['str']
     StoredValue  = "UNDEFINED"


class CommonGeometryFlags_JobProperties(JobPropertyContainer):
    """ The geometry flag/job property container """

    def __init__(self, context=""):
        JobPropertyContainer.__init__(self,context)
        return


    def setupValuesFromDB(self,geoTagName="none"):
        
        CommonGeoFlags = CommonGMFlags(geoTagName)

        self.Run.set_Value_and_Lock(CommonGeoFlags.getValue("Run"))
        self.GeoType.set_Value_and_Lock(CommonGeoFlags.getValue("GeoType"))
        self.StripGeoType.set_Value_and_Lock(CommonGeoFlags.getValue("StripGeoType"))


    def reset(self,geoTagName="none"):
        
        self.Run.unlock()
        self.GeoType.unlock()
        self.StripGeoType.unlock()


    def dump(self):

        Logging.log.info("RUN flag      :"+self.Run())
        Logging.log.info("GeoType flag  : "+self.GeoType())
        Logging.log.info("Strip geoType flag  : "+self.StripGeoType())


jobproperties.add_Container(CommonGeometryFlags_JobProperties)
jobproperties.CommonGeometryFlags_JobProperties.add_JobProperty(Run)
jobproperties.CommonGeometryFlags_JobProperties.add_JobProperty(GeoType)
jobproperties.CommonGeometryFlags_JobProperties.add_JobProperty(StripGeoType)

CommonGeometryFlags = jobproperties.CommonGeometryFlags_JobProperties
CommonGeometryFlags.setupValuesFromDB()

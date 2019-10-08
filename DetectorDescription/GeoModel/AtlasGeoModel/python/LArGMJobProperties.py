# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

#
# LAr GeoModel initialization
#
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from AtlasGeoModel.CommonGMJobProperties import CommonGMFlags
from AthenaCommon import Logging

# -------------------------------------------------------------------------------------
#  LAr geometry flags initialization
# -------------------------------------------------------------------------------------

class LArGMFlags(CommonGMFlags, object):

    def __init__(self, geoTag="none"):

        super(LArGMFlags, self).__init__()

    def InitializeGeometryParameters(self):

        # ----------------------------------------------------------------------------    
        # Read versionname, layout and dbm from PixelSwicthes table

        dbId,dbSwitches,dbParam = self.dbGeomCursor.GetCurrentLeafContent("LArSwitches")
        _sagging = None
        _barrelOn = None
        _endcapOn = None
        _FCalFlag="UNDEFINED"
        _detAbsorber = None
        _detAbsorber_EC = None

        if len(dbId)>0:
            key=dbId[0] 
            if "SAGGING" in dbParam: _sagging = dbSwitches[key][dbParam.index("SAGGING")]
            if "BARREL_ON" in dbParam: _barrelOn = dbSwitches[key][dbParam.index("BARREL_ON")]
            if "ENDCAP_ON" in dbParam : _endcapOn = dbSwitches[key][dbParam.index("ENDCAP_ON")]
            if "DETAILED_ABSORBER" in dbParam : _detAbsorber = dbSwitches[key][dbParam.index("DETAILED_ABSORBER")]
            if "DETAILED_ABSORBER_EC" in dbParam :_detAbsorber_EC = dbSwitches[key][dbParam.index("DETAILED_ABSORBER_EC")]
            if "FCAL_GEOTYPE" in dbParam : _FCalFlag = dbSwitches[key][dbParam.index("FCAL_GEOTYPE")]


        self.__dict__["Sagging"] = _sagging
        self.__dict__["BarrelOn"] = _barrelOn
        self.__dict__["EndcapOn"] = _endcapOn
        self.__dict__["FCal_GeoType"] = _FCalFlag
        self.__dict__["DetAbs"] = _detAbsorber
        self.__dict__["DetAbs_EC"] = _detAbsorber_EC

    def dump(self):

        Logging.log.info("Geometry tag LArGMFlags : "+self.__dict__["geomTag"]+" ------------------------------------")
        Logging.log.info("Sagging              = "+self.__dict__["Sagging"])
        Logging.log.info("BarrelOn             = "+self.__dict__["BarrelOn"])
        Logging.log.info("EndcapON             = "+self.__dict__["EndcapOn"])
        Logging.log.info("Detailed Absorber    = "+self.__dict__["DetAbs"])
        Logging.log.info("Detailed Absorber EC = "+self.__dict__["DetAbs_EC"])
        Logging.log.info("FCal geotype         = "+self.__dict__["FCal_GeoType"])



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

    def setupValuesFromDB(self,geoTagName="none"):
        
        LArGeoFlags = LArGMFlags(geoTagName)

        self.Sagging.set_Value_and_Lock( LArGeoFlags.getValue("Sagging"))
        self.BarrelOn.set_Value_and_Lock( LArGeoFlags.getValue("BarrelOn"))
        self.EndcapOn.set_Value_and_Lock(LArGeoFlags.getValue("EndcapOn"))
        self.DetailedAbsorber.set_Value_and_Lock(LArGeoFlags.getValue("DetAbs"))
        self.DetailedAbsorber_EC.set_Value_and_Lock(LArGeoFlags.getValue("DetAbs_EC"))
        self.FCalGeoType.set_Value_and_Lock(LArGeoFlags.getValue("FCal_GeoType"))


    def reset(self,geoTagName="none"):

        self.Sagging.unlock()
        self.BarrelOn.unlock()
        self.EndcapOn.unlock()
        self.DetailedAbsorber.unlock()
        self.DetailedAbsorber_EC.unlock()
        self.FCalGeoType.unlock()

    def dump(self):

        Logging.log.info("Sagging              = "+self.Sagging())
        Logging.log.info("Barrel_On            = "+self.BarrelOn())
        Logging.log.info("Endcap_On            = "+self.EndcapOn())
        Logging.log.info("Detailed absorber    = "+self.DetailedAbsorber())
        Logging.log.info("Detailed absorber EC = "+self.DetailedAbsorber_EC())

        Logging.log.info("FCal GeoType         = "+self.FCalGeoType())


jobproperties.add_Container(LArGeometryFlags_JobProperties)
jobproperties.LArGeometryFlags_JobProperties.add_JobProperty(Sagging)
jobproperties.LArGeometryFlags_JobProperties.add_JobProperty(BarrelOn)
jobproperties.LArGeometryFlags_JobProperties.add_JobProperty(EndcapOn)
jobproperties.LArGeometryFlags_JobProperties.add_JobProperty(DetailedAbsorber)
jobproperties.LArGeometryFlags_JobProperties.add_JobProperty(DetailedAbsorber_EC)
jobproperties.LArGeometryFlags_JobProperties.add_JobProperty(FCalGeoType)

LArGeometryFlags = jobproperties.LArGeometryFlags_JobProperties
LArGeometryFlags.setupValuesFromDB()


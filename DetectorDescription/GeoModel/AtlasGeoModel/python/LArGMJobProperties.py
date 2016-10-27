# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# LAr GeoModel initialization
#
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties

from AtlasGeoDBInterface import AtlasGeoDBInterface

# -------------------------------------------------------------------------------------
#  LAr geometry flags initialization
# -------------------------------------------------------------------------------------

class LArGMFlags:

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

    def getValue(self,name):

        return self.__dict__[name]

    def dump(self):

        print "Geometry tag LArGMFlags : ",self.__dict__["geomTag"]," ------------------------------------"
        print "Sagging              = ",self.__dict__["Sagging"]
        print "BarrelOn             = ",self.__dict__["BarrelOn"]
        print "EndcapON             = ",self.__dict__["EndcapOn"]
        print "Detailed Absorber    = ",self.__dict__["DetAbs"]
        print "Detailed Absorber EC = ",self.__dict__["DetAbs_EC"]
        print "FCal geotype         = ",self.__dict__["FCal_GeoType"]



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

        print "Sagging              = ", self.Sagging()
        print "Barrel_On            = ", self.BarrelOn()
        print "Endcap_On            = ", self.EndcapOn()
        print "Detailed absorber    = ", self.DetailedAbsorber()
        print "Detailed absorber EC = ", self.DetailedAbsorber_EC()

        print "FCal GeoType         = ", self.FCalGeoType()


jobproperties.add_Container(LArGeometryFlags_JobProperties)
jobproperties.LArGeometryFlags_JobProperties.add_JobProperty(Sagging)
jobproperties.LArGeometryFlags_JobProperties.add_JobProperty(BarrelOn)
jobproperties.LArGeometryFlags_JobProperties.add_JobProperty(EndcapOn)
jobproperties.LArGeometryFlags_JobProperties.add_JobProperty(DetailedAbsorber)
jobproperties.LArGeometryFlags_JobProperties.add_JobProperty(DetailedAbsorber_EC)
jobproperties.LArGeometryFlags_JobProperties.add_JobProperty(FCalGeoType)

LArGeometryFlags = jobproperties.LArGeometryFlags_JobProperties
LArGeometryFlags.setupValuesFromDB()
        




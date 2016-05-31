# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# InDet GeoModel initialization
#
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties

from AtlasGeoDBInterface import AtlasGeoDBInterface

# -------------------------------------------------------------------------------------
#  InDet geometry flags initialization
# -------------------------------------------------------------------------------------

class InDetGMFlags:

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

        dbId,dbSwitches,dbParam = self.dbGeomCursor.GetCurrentLeafContent("PixelSwitches")

        _versionName="UNDEFINED"
        _layout="UNDEFINED"
        _dbm = False

        if len(dbId)>0:
            key=dbId[0] 
            if "VERSIONNAME" in dbParam: _versionName = dbSwitches[key][dbParam.index("VERSIONNAME")]
            if "LAYOUT" in dbParam : _layout = dbSwitches[key][dbParam.index("LAYOUT")]
            if "BUILDDBM" in dbParam : _dbm = (dbSwitches[key][dbParam.index("BUILDDBM")] != 0)


        self.__dict__["VersionName"] = _versionName
        self.__dict__["Layout"] = _layout
        self.__dict__["DBM"] = _dbm


        # ----------------------------------------------------------------------------    
        # IBL layout

        dbId,dbLayers,dbParam = self.dbGeomCursor.GetCurrentLeafContent("PixelLayer")
        IBLStaveIndex = -1
        IBLgeoLayout = -1
        _IBLlayout = "noIBL"
        if len(dbId)>0:
            key=dbId[0] 
            if "STAVEINDEX" in dbParam and dbLayers[key][dbParam.index("STAVEINDEX")] not in ["NULL",None]:
                IBLStaveIndex = int(dbLayers[key][dbParam.index("STAVEINDEX")])

            if IBLStaveIndex>-1:
                dbId,dbStaves,dbParam = self.dbGeomCursor.GetCurrentLeafContent("PixelStave")

                if len(dbId)>0 and IBLStaveIndex<=len(dbStaves.keys()):
                    key=dbId[IBLStaveIndex] 
                    if "LAYOUT" in dbParam and dbStaves[key][dbParam.index("LAYOUT")] not in ["NULL",None]:
                        IBLgeoLayout = int(dbStaves[key][dbParam.index("LAYOUT")])
                        if IBLgeoLayout in [3,4] : _IBLlayout = "planar"
                        elif IBLgeoLayout in [5] : _IBLlayout = "3D"

        self.__dict__["IBLlayout"]=_IBLlayout

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

        # ----------------------------------------------------------------------------    
        # IBL and SLHC parameters

        self.__dict__["IBL"] = False
        self.__dict__["SLHC"] = False
        if _layout in ['IBL'] : self.__dict__["IBL"] = True
        if self.__dict__["IBL"] == False: self.__dict__["IBLlayout"]="noIBL"
        if _layout not in ['SLHC'] and _run.lower()=="run2": self.__dict__["IBL"] = True
        if _layout in ['SLHC'] : self.__dict__["SLHC"] = True


    def getValue(self,name):

        return self.__dict__[name]

    def dump(self):

        print "Geometry tag InDetGMFlags : ",self.__dict__["geomTag"]," ------------------------------------"
        print "VersionName = ",self.__dict__["VersionName"]
        print "Layout      = ",self.__dict__["Layout"]
        print "DBM         = ",self.__dict__["DBM"]

        print "SLHC flag : ",self.__dict__["SLHC"]
        print "IBL flag   : ",self.__dict__["IBL"]
        print "IBL layout : ", self.__dict__["IBLlayout"]
        print "RUN flag      : ",self.__dict__["Run"]
        print "GeoType flag  : ",self.__dict__["GeoType"]
        print "Strip GeoType flag  : ",self.__dict__["StripGeoType"]



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


# add to jobproperties
class GeometryFlags_JobProperties(JobPropertyContainer):
    """ The geometry flag/job property container """

    def __init__(self, context=""):
        JobPropertyContainer.__init__(self,context)
        return

    def setupValuesFromDB(self,geoTagName="none"):
        
        InDetGeoFlags = InDetGMFlags(geoTagName)

        self.isSLHC.set_Value_and_Lock( InDetGeoFlags.getValue("SLHC") )
        self.isIBL.set_Value_and_Lock(InDetGeoFlags.getValue("IBL"))
        self.isDBM.set_Value_and_Lock(InDetGeoFlags.getValue("DBM"))
        self.IBLLayout.set_Value_and_Lock(InDetGeoFlags.getValue("IBLlayout"))
        self.GeoVersionName.set_Value_and_Lock(InDetGeoFlags.getValue("VersionName"))
        self.GeoLayout.set_Value_and_Lock(InDetGeoFlags.getValue("Layout"))
        self.Run.set_Value_and_Lock(InDetGeoFlags.getValue("Run"))
        self.GeoType.set_Value_and_Lock(InDetGeoFlags.getValue("GeoType"))
        self.StripGeoType.set_Value_and_Lock(InDetGeoFlags.getValue("StripGeoType"))


    def reset(self,geoTagName="none"):
        
        self.isSLHC.unlock()
        self.isIBL.unlock()
        self.isDBM.unlock()
        self.IBLLayout.unlock()
        self.GeoVersionName.unlock()
        self.GeoLayout.unlock()
        self.Run.unlock()
        self.GeoType.unlock()
        self.StripGeoType.unlock()


    def dump(self):

        print "VersionName = ", self.GeoVersionName()
        print "Layout      = ", self.GeoLayout()
        print "DBM         = ", self.isDBM()

        print "SLHC flag : ", self.isSLHC()
        print "IBL flag   : ", self.isIBL()
        print "IBL layout : ", self.IBLLayout()
        print "RUN flag      :", self.Run()
        print "GeoType flag  : ", self.GeoType()
        print "Strip geoType flag  : ", self.StripGeoType()


jobproperties.add_Container(GeometryFlags_JobProperties)
jobproperties.GeometryFlags_JobProperties.add_JobProperty(GeoVersionName)
jobproperties.GeometryFlags_JobProperties.add_JobProperty(GeoLayout)
jobproperties.GeometryFlags_JobProperties.add_JobProperty(isIBL)
jobproperties.GeometryFlags_JobProperties.add_JobProperty(IBLLayout)
jobproperties.GeometryFlags_JobProperties.add_JobProperty(isSLHC)
jobproperties.GeometryFlags_JobProperties.add_JobProperty(isDBM)
jobproperties.GeometryFlags_JobProperties.add_JobProperty(Run)
jobproperties.GeometryFlags_JobProperties.add_JobProperty(GeoType)
jobproperties.GeometryFlags_JobProperties.add_JobProperty(StripGeoType)

GeometryFlags = jobproperties.GeometryFlags_JobProperties
GeometryFlags.setupValuesFromDB()
        




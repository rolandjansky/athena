# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

#
# InDet GeoModel initialization
#
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from AtlasGeoModel.CommonGMJobProperties import CommonGMFlags, CommonGeometryFlags

# -------------------------------------------------------------------------------------
#  InDet geometry flags initialization
# -------------------------------------------------------------------------------------

class InDetGMFlags(CommonGMFlags, object):

    def __init__(self, geoTag="none"):

        super(InDetGMFlags, self).__init__()

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
        # IBL and SLHC parameters

        self.__dict__["IBL"] = False
        self.__dict__["SLHC"] = False
        if _layout in ['IBL'] : self.__dict__["IBL"] = True
        if self.__dict__["IBL"] == False: self.__dict__["IBLlayout"]="noIBL"
        if _layout not in ['SLHC'] and ( CommonGeometryFlags.Run() in ["RUN2", "RUN3"] ) : self.__dict__["IBL"] = True
        if _layout in ['SLHC'] : self.__dict__["SLHC"] = True


    def dump(self):

        print "Geometry tag InDetGMFlags : ",self.__dict__["geomTag"]," ------------------------------------"
        print "VersionName = ",self.__dict__["VersionName"]
        print "Layout      = ",self.__dict__["Layout"]
        print "DBM         = ",self.__dict__["DBM"]

        print "SLHC flag : ",self.__dict__["SLHC"]
        print "IBL flag   : ",self.__dict__["IBL"]
        print "IBL layout : ", self.__dict__["IBLlayout"]


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
    StoredValue  = False

class dumpInDetGDML(JobProperty):
    """ Use to turn on dumping of InDet GDML output files from Geant4 """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

# add to jobproperties
class InDetGeometryFlags_JobProperties(JobPropertyContainer):
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


    def reset(self,geoTagName="none"):
        
        self.isSLHC.unlock()
        self.isIBL.unlock()
        self.isDBM.unlock()
        self.IBLLayout.unlock()
        self.GeoVersionName.unlock()
        self.GeoLayout.unlock()


    def dump(self):

        print "VersionName = ", self.GeoVersionName()
        print "Layout      = ", self.GeoLayout()
        print "DBM         = ", self.isDBM()

        print "SLHC flag : ", self.isSLHC()
        print "IBL flag   : ", self.isIBL()
        print "IBL layout : ", self.IBLLayout()


jobproperties.add_Container(InDetGeometryFlags_JobProperties)
jobproperties.InDetGeometryFlags_JobProperties.add_JobProperty(GeoVersionName)
jobproperties.InDetGeometryFlags_JobProperties.add_JobProperty(GeoLayout)
jobproperties.InDetGeometryFlags_JobProperties.add_JobProperty(isIBL)
jobproperties.InDetGeometryFlags_JobProperties.add_JobProperty(IBLLayout)
jobproperties.InDetGeometryFlags_JobProperties.add_JobProperty(isSLHC)
jobproperties.InDetGeometryFlags_JobProperties.add_JobProperty(isDBM)
jobproperties.InDetGeometryFlags_JobProperties.add_JobProperty(useDynamicAlignFolders)
jobproperties.InDetGeometryFlags_JobProperties.add_JobProperty(dumpInDetGDML)

InDetGeometryFlags = jobproperties.InDetGeometryFlags_JobProperties
InDetGeometryFlags.setupValuesFromDB()


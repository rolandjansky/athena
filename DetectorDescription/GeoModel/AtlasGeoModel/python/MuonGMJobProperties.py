# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from AtlasGeoModel.CommonGMJobProperties import CommonGMFlags, CommonGeometryFlags

# -------------------------------------------------------------------------------------
#  Muon geometry flags initialization
# -------------------------------------------------------------------------------------
class MuonGMFlags(CommonGMFlags, object):

    def __init__(self, geoTag="none"):
        super(MuonGMFlags, self).__init__()

    def InitializeGeometryParameters(self):
        dbId,dbSwitches,dbParam = self.dbGeomCursor.GetCurrentLeafContent("MuonSwitches")
        _layoutName=None
        _hasCsc=True
        _hasStgc=True
        _hasMM=True
        if len(dbId)>0:
            key=dbId[0] 
            if "LAYOUTNAME" in dbParam: _layoutName = dbSwitches[key][dbParam.index("LAYOUTNAME")]
            if "HASCSC" in dbParam: _hasCsc = dbSwitches[key][dbParam.index("HASCSC")]
            if "HASSTGC" in dbParam: _hasStgc = dbSwitches[key][dbParam.index("HASSTGC")]
            if "HASMM" in dbParam: _hasMM = dbSwitches[key][dbParam.index("HASMM")]
            
        self.__dict__["Layout"] = (_layoutName if _layoutName else "UNDEFINED")
        if _hasCsc == 0: self.__dict__["HasCSC"] = False
        else: self.__dict__["HasCSC"] = True
        if _hasStgc == 0: self.__dict__["HasSTGC"] = False
        else: self.__dict__["HasSTGC"] = True
        if _hasMM == 0: self.__dict__["HasMM"] = False
        else: self.__dict__["HasMM"] = True

    def dump(self):
        print ("MuonGMFlags:")
        print ("Layout      = ",self.__dict__["Layout"])
        print ("HasCSC      = ",self.__dict__["HasCSC"])
        print ("HasSTGC     = ",self.__dict__["HasSTGC"])
        print ("HasMM       = ",self.__dict__["HasMM"])


class GeoLayout(JobProperty):
     """ geometry layout """
     statusOn     = True
     allowedTypes = ['str']
     StoredValue  = 'UNDEFINED'

class hasCSC(JobProperty):
     """ does the layout contain CSC chambers? """
     statusOn     = True
     allowedTypes = ['bool']
     StoredValue  = True

class hasSTGC(JobProperty):
     """ does the layout contain STGC chambers? """
     statusOn     = True
     allowedTypes = ['bool']
     StoredValue  = True

class hasMM(JobProperty):
     """ does the layout contain MicroMegas chambers? """
     statusOn     = True
     allowedTypes = ['bool']
     StoredValue  = True

class MuonGeometryFlags_JobProperties(JobPropertyContainer):
    """ The geometry flag/job property container """

    def __init__(self, context=""):
        JobPropertyContainer.__init__(self,context)
        return
    def setupValuesFromDB(self,geoTagName="none"):
        MuonGeoFlags = MuonGMFlags(geoTagName)
        self.GeoLayout.set_Value_and_Lock(MuonGeoFlags.getValue("Layout"))
        self.hasCSC.set_Value_and_Lock(MuonGeoFlags.getValue("HasCSC"))
        self.hasSTGC.set_Value_and_Lock(MuonGeoFlags.getValue("HasSTGC"))
        self.hasMM.set_Value_and_Lock(MuonGeoFlags.getValue("HasMM"))

    def reset(self,geoTagName="none"):
        self.GeoLayout.unlock()
        self.hasCSC.unlock()
        self.hasSTGC.unlock()
        self.hasMM.unlock()

    def dump(self):
        print ("Layout      = ", self.GeoLayout())
        print ("HasCSC      = ", self.hasCSC())
        print ("HasSTGC     = ", self.hasSTGC())
        print ("HasMM       = ", self.hasMM())


jobproperties.add_Container(MuonGeometryFlags_JobProperties)
jobproperties.MuonGeometryFlags_JobProperties.add_JobProperty(GeoLayout)
jobproperties.MuonGeometryFlags_JobProperties.add_JobProperty(hasCSC)
jobproperties.MuonGeometryFlags_JobProperties.add_JobProperty(hasSTGC)
jobproperties.MuonGeometryFlags_JobProperties.add_JobProperty(hasMM)

MuonGeometryFlags = jobproperties.MuonGeometryFlags_JobProperties
MuonGeometryFlags.setupValuesFromDB()


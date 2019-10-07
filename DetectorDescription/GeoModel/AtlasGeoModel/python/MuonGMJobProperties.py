# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from AtlasGeoModel.CommonGMJobProperties import CommonGMFlags, CommonGeometryFlags
from AthenaCommon import Logging

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
        if len(dbId)>0:
            key=dbId[0] 
            if "LAYOUTNAME" in dbParam: _layoutName = dbSwitches[key][dbParam.index("LAYOUTNAME")]
            if "HASCSC" in dbParam: _hasCsc = dbSwitches[key][dbParam.index("HASCSC")]
            
        self.__dict__["Layout"] = (_layoutName if _layoutName else "UNDEFINED")
        if _hasCsc == 0: self.__dict__["HasCSC"] = False
        else: self.__dict__["HasCSC"] = True

    def dump(self):
        Logging.log.info("MuonGMFlags:")
        Logging.log.info("Layout      = "+self.__dict__["Layout"])
        Logging.log.info("HasCSC      = "+self.__dict__["HasCSC"])


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


class MuonGeometryFlags_JobProperties(JobPropertyContainer):
    """ The geometry flag/job property container """

    def __init__(self, context=""):
        JobPropertyContainer.__init__(self,context)
        return
    def setupValuesFromDB(self,geoTagName="none"):
        MuonGeoFlags = MuonGMFlags(geoTagName)
        self.GeoLayout.set_Value_and_Lock(MuonGeoFlags.getValue("Layout"))
        self.hasCSC.set_Value_and_Lock(MuonGeoFlags.getValue("HasCSC"))

    def reset(self,geoTagName="none"):
        self.GeoLayout.unlock()
        self.hasCSC.unlock()

    def dump(self):
        Logging.log.info("Layout      = "+self.GeoLayout())
        Logging.log.info("HasCSC      = "+self.hasCSC())


jobproperties.add_Container(MuonGeometryFlags_JobProperties)
jobproperties.MuonGeometryFlags_JobProperties.add_JobProperty(GeoLayout)
jobproperties.MuonGeometryFlags_JobProperties.add_JobProperty(hasCSC)

MuonGeometryFlags = jobproperties.MuonGeometryFlags_JobProperties
MuonGeometryFlags.setupValuesFromDB()


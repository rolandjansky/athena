# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from AtlasGeoModel.AtlasGeoDBInterface import AtlasGeoDBInterface
from MuonGeoModel import MuonGeoDB

# -------------------------------------------------------------------------------------
#  Muon geometry flags
# -------------------------------------------------------------------------------------

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
     StoredValue  = False

class hasMM(JobProperty):
     """ does the layout contain MicroMegas chambers? """
     statusOn     = True
     allowedTypes = ['bool']
     StoredValue  = False

class MuonGeometryFlags_JobProperties(JobPropertyContainer):
    """ The geometry flag/job property container """

    def __init__(self, context=""):
        JobPropertyContainer.__init__(self,context)
        return
    def setupValuesFromDB(self,geoTagName=None):

        dbGeomCursor = AtlasGeoDBInterface(geoTagName or globalflags.DetDescrVersion())
        dbGeomCursor.ConnectAndBrowseGeoDB()
        params = MuonGeoDB.InitializeGeometryParameters(dbGeomCursor)

        self.GeoLayout.set_Value_and_Lock(params["Layout"])
        self.hasCSC.set_Value_and_Lock(params["HasCSC"])
        self.hasSTGC.set_Value_and_Lock(params["HasSTGC"])
        self.hasMM.set_Value_and_Lock(params["HasMM"])

    def reset(self):
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


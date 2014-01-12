# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TGCgeometry.TGCgeometryConf import *
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr
from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags

class TGCgeometryConfig (TGCgeometrySvc):
    
    def __init__(self,name = Configurable.DefaultName ):
        super(TGCgeometryConfig ,self).__init__(name)

        self.__slots__ = ["FileName","DisableTagInfoMatch"]
        
        self.FileName = "TGCgeometry."+globalflags.DetDescrVersion()+".lut"


        self.DisableTagInfoMatch = False

        # use the MuonByteStreamFlags for the init
        if muonByteStreamFlags.TgcDataType() == "atlas":
            self.FileName = self.FileName[0:-4] + ".12.lut"
        else:
            self.FileName = self.FileName[0:-4] + ".8.lut"

        # superseed the configurationn with the global flags 
        if globalflags.DataSource.is_data():
            self.DisableTagInfoMatch = True
            self.FileName = self.FileName[0:self.FileName.find(globalflags.DetDescrVersion()) +
                                            len(globalflags.DetDescrVersion())] + ".12.lut"

        if globalflags.InputFormat.is_bytestream():
            self.DisableTagInfoMatch = True


    def setDefaults(cls,handle):
        if hasattr(handle,'FileName'):
            print "TGCgeometrySvc to be configured with LUT: ", handle.FileName
            if handle.FileName!="TGCgeometry.lut":
                print "TGCgeometrySvc uses the LUT for misaligned geometry."

        if hasattr(handle,'DisableTagInfoMatch'):
            if handle.DisableTagInfoMatch==True:
                print "TGCgeometrySvc has the call back to the Tag Info disabled"
            else:
                print "TGCgeometrySvc has the call back to the Tag Info active"

TGCgeometrySvc = TGCgeometryConfig()
ServiceMgr += TGCgeometrySvc

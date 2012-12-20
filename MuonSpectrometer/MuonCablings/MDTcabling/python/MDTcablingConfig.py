# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MDTcabling.MDTcablingConf import *
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr
from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags 

class MDTcablingConfig (MDTcablingSvc):
    
    def __init__(self,name = Configurable.DefaultName ):
        super(MDTcablingConfig,self).__init__(name)

        if ( globalflags.DetDescrVersion().startswith('DC1') or globalflags.DetDescrVersion().startswith('DC2') ):
            self.RODfile = "P03_RODmap.data"
        
        #print "MDTcabling: job configuration is ..."
        #print "%s" % muonByteStreamFlags.MdtDataType

        if ( muonByteStreamFlags.MdtDataType()=='atlas' ):
            self.RODfile = "AtlasRODmap.data"
            self.MEZfile = "AtlasMEZmap.data"
            self.ShiftLinks=False
            self.UseOldGeoAccess=False
        
        #print "%s" % globalflags.DataSource
        
        #superseed the configurationn with the global flags
        if globalflags.DataSource.is_data():
            self.DisableTagInfoMatch = True
            self.RODfile = ""
            self.MEZfile = ""
            self.ShiftLinks=False
            self.DisableDBcablingLoad=False
            self.UseOldGeoAccess=False
        
        #print "%s" % globalflags.InputFormat
        
        if globalflags.InputFormat.is_bytestream():
            self.DisableTagInfoMatch = True   
            self.UseOldGeoAccess=False

    def setDefaults(cls,handle):
        if hasattr(handle,'Use_cm'):
            if handle.Use_cm == False:
                print "MDTcalingSvc doesn't use cm unit!"

MDTcablingSvc = MDTcablingConfig()
ServiceMgr += MDTcablingSvc

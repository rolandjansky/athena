# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RPCgeometry.RPCgeometryConf import *
from AthenaCommon.AppMgr import ServiceMgr
from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags
from AthenaCommon.GlobalFlags import globalflags

class RPCgeometryConfig (RPCgeometrySvc):
    
    def __init__(self,name = Configurable.DefaultName ):
        super(RPCgeometryConfig,self).__init__(name)

        #print "RPCgeometry: job configuration is ..."
        #print "%s" % muonByteStreamFlags.RpcDataType

        if ( muonByteStreamFlags.RpcDataType()=='atlas' ):
            self.UseOldGeoAccess=False

        #print "%s" % globalflags.DataSource

        #superseed the configurationn with the global flags
        if globalflags.DataSource.is_data():
            self.UseOldGeoAccess=False
            self.DoMonitorAlignment=True

        #print "%s" % globalflags.InputFormat

        if globalflags.InputFormat.is_bytestream():  
            self.UseOldGeoAccess=False
            self.DoMonitorAlignment=True

    def setDefaults(cls,handle):

        if hasattr(handle,'Use_cm') :
            if handle.Use_cm == False:
                print "RPCgeometrySvc doesn't use cm unit!"

        if hasattr(handle,'H8testbeam') :
            if handle.Use_cm == True:
                print "RPCgeometrySvc built an incomplete RPC geometry!"


RPCgeometrySvc = RPCgeometryConfig()
ServiceMgr += RPCgeometrySvc

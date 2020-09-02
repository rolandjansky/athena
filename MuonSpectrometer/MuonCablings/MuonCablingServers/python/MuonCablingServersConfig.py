# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from MuonCablingServers.MuonCablingServersConf import RPCcablingServerSvc, TGCcablingServerSvc
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr,theApp
from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.Configurable import Configurable

class RpcCablingServerConfig (RPCcablingServerSvc):
    
    def __init__(self,name = Configurable.DefaultName ):
        super(RpcCablingServerConfig ,self).__init__(name)

        if ( globalflags.DetDescrVersion().startswith('DC1') or \
             globalflags.DetDescrVersion().startswith('DC2') or \
             globalflags.DetDescrVersion().startswith('DC3') or \
             globalflags.DetDescrVersion().startswith('ATLAS-CSC')):
            self.Atlas = False
        
        if muonByteStreamFlags.RpcDataType()=='atlas':
            self.Atlas = True

        if muonByteStreamFlags.RpcDataType()=='oldSimulation':
            self.Atlas = False
    
    def setDefaults(cls,handle):
        if hasattr(handle,'Atlas'):
            if handle.Atlas is not True:
                print ("RPCcabling Server uses the old RPC cabling schema")

class TgcCablingServerConfig (TGCcablingServerSvc):

    def __init__(self,name = Configurable.DefaultName ):
        super(TgcCablingServerConfig ,self).__init__(name)

        # make default Atlas
        # *TODO* How do we handle the case of old CSC data? Is new DetDescrVersion CSC too? EJWM
        self.Atlas = True
        if ( globalflags.DetDescrVersion().startswith('DC1') or
             globalflags.DetDescrVersion().startswith('DC2') or
             globalflags.DetDescrVersion().startswith('DC3')) :
            self.Atlas = False
            print ("DC")

        if (muonByteStreamFlags.TgcDataType()=='m3' or
            muonByteStreamFlags.TgcDataType()=='oldSimulation') :
            self.Atlas = False
            if (muonByteStreamFlags.TgcDataType()=='m3'):
                print ("TgcDataType is set to m3")
            else:
                print ("TgcDataType is set to oldSim")


        # setting the default to ATLAS cabling
        print (self.Atlas)

        # Setting of the default 12-fold (ATLAS) cabling
        # New MuonTGC_CablingSvc (True) uses COOL DB and old TGCcabling12Svc (False) does not use COOL DB
        self.useMuonTGC_CablingSvc = True

        # do not use the call-back for digitization job
        self.forcedUse = True

        # if we run from RDO (overlay or other jobs) we must use the call-back, 
        # even if we have hits to digitize
        if DetFlags.haveRDO.TGC_on() and not DetFlags.digitize.TGC_on():
            self.forcedUse = False

        #use the call-back in the overlay job that has no bytestream data as input
        if globalflags.isOverlay():
            self.forcedUse = False

        # superseed the previos configuration if we run from Atlas data 
        if globalflags.DataSource.is_data():
            self.forcedUse = True
            self.Atlas = True

        # avoid to use call-back if input is bytestream
        if globalflags.InputFormat.is_bytestream():
            self.forcedUse = True


    def setDefaults(cls,handle):
        if hasattr(handle,'Atlas'):
            if handle.Atlas is not True:
                print ("TGCcabling Server uses the old TGC cabling schema")
            else:
                print ("TGCcabling Server uses the new 12-fold cabling schema")


if "RPCcablingInterface" not in theApp.Dlls:
    theApp.Dlls += [ "RPCcablingInterface" ]

if "TGCcablingInterface" not in theApp.Dlls:
    theApp.Dlls += [ "TGCcablingInterface" ]


RpcCablingServer = RpcCablingServerConfig()
TgcCablingServer = TgcCablingServerConfig()

ServiceMgr += RpcCablingServer
ServiceMgr += TgcCablingServer

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ----------- module for Writing out ByteStream ----------
# 

from AthenaCommon.AppMgr import theApp

# get a handle on the ServiceManager
svcMgr = theApp.serviceMgr()

# Conversion Service instance
if not hasattr(svcMgr,"ByteStreamCnvSvc"):
    from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
    svcMgr += ByteStreamCnvSvc()

# existing streams
_streamMap = {}

# getter for StreamBS
def getStream(type,name):
    # globals in this module
    global svcMgr
    global _streamMap
    
    # type check
    type = type.upper()
    if not type in ['EVENTSTORAGE','TRANSIENT']:
        raise RuntimeError,"unsupported StreamType:%s" % type

    # return existing stream
    if _streamMap.has_key((type,name)):
        return _streamMap[(type,name)]

    # EventStorage
    if type=='EVENTSTORAGE': 
        # OutputSvc
        if not hasattr(svcMgr,"ByteStreamEventStorageOutputSvc"):
            from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc
            svcMgr += ByteStreamEventStorageOutputSvc()
    
        # Properties 
        ByteStreamCnvSvc = svcMgr.ByteStreamCnvSvc
        ByteStreamCnvSvc.ByteStreamOutputSvcList += ["ByteStreamEventStorageOutputSvc"]
            
        # OutputStream
        from AthenaServices.AthenaServicesConf import AthenaOutputStream
        StreamBS = AthenaOutputStream(
            name,
            EvtConversionSvc = "ByteStreamCnvSvc",
            OutputFile = "ByteStreamEventStorageOutputSvc",
            ForceRead=True
            )
        theApp.addOutputStream( StreamBS )
        theApp.OutStreamType ="AthenaOutputStream";
            
        # Define the output as follows: 
        ByteStreamEventStorageOutputSvc = svcMgr.ByteStreamEventStorageOutputSvc

        ByteStreamEventStorageOutputSvc.MaxFileMB = 15000
        # maximum number of event (beyond which it creates a new file)
        ByteStreamEventStorageOutputSvc.MaxFileNE = 15000000
        ByteStreamEventStorageOutputSvc.OutputDirectory = "./"
        ByteStreamEventStorageOutputSvc.AppName = "Athena"
        # release variable depends the way the env is configured
        #svcMgr.ByteStreamEventStorageOutputSvc.FileTag = release    
        svcMgr.ByteStreamEventStorageOutputSvc.RunNumber = 0   
        # does not work
        if hasattr( svcMgr.EventSelector, 'RunNumber'):
            svcMgr.ByteStreamEventStorageOutputSvc.RunNumber = svcMgr.EventSelector.RunNumber
    
        # decode BSRDOOutput property and overwrite some OutputStream properties if things are there defined
        from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
        props = athenaCommonFlags.BSRDOOutput().split(",")
        if len(props) > 1:
            for p in props:
                if "AppName" in p:
                    svcMgr.ByteStreamEventStorageOutputSvc.AppName = p.split("=")[1]
                if "OutputDirectory" in p:
                    svcMgr.ByteStreamEventStorageOutputSvc.OutputDirectory = p.split("=")[1]
                if "FileTag" in p:
                    svcMgr.ByteStreamEventStorageOutputSvc.FileTag = p.split("=")[1]
                if "Run" in p:
                    svcMgr.ByteStreamEventStorageOutputSvc.RunNumber = int(p.split("=")[1])
        else:
            svcMgr.ByteStreamEventStorageOutputSvc.SimpleFileName = athenaCommonFlags.BSRDOOutput()
        # append
        _streamMap[(type,name)] = StreamBS
        return StreamBS
    
    elif type=='TRANSIENT':
        # OutputSvc
        if not hasattr(svcMgr,"ROBDataProviderSvc"):        
            from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderSvc
            svcMgr += ROBDataProviderSvc()
        if not hasattr(svcMgr,"ByteStreamRDP_OutputSvc"):
            from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamRDP_OutputSvc
            svcMgr += ByteStreamRDP_OutputSvc()

        # Properties
        ByteStreamCnvSvc = svcMgr.ByteStreamCnvSvc
        ByteStreamCnvSvc.ByteStreamOutputSvcList += ["ByteStreamRDP_OutputSvc"]
        
        # This output stream is created as a regular OutputStream 
        from AthenaServices.AthenaServicesConf import AthenaOutputStream
        StreamBS = AthenaOutputStream(
            name,
            EvtConversionSvc = "ByteStreamCnvSvc",
            OutputFile = "ByteStreamRDP_OutputSvc",
            ForceRead=True            
            )
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += StreamBS
        
        # ByteStreamCnvSvc is an input CnvSvc now. 
        EventPersistencySvc = svcMgr.EventPersistencySvc
        EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]

        # append
        _streamMap[(type,name)] = StreamBS
        return StreamBS
                    
    else:
        raise RuntimeError,"unsupported StreamType:%s" % type


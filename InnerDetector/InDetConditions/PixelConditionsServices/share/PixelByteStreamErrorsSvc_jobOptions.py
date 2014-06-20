include.block("PixelConditionsServices/PixelByteStreamErrorsSvcSvc_jobOptions.py")

from AthenaCommon.GlobalFlags import globalflags

from PixelConditionsServices.PixelConditionsServicesConf import PixelByteStreamErrorsSvc
if ( globalflags.InputFormat == 'bytestream' ):
    ServiceMgr += PixelByteStreamErrorsSvc()
elif ( globalflags.DataSource == 'data' ) :
    # Try to access bytestream information from StoreGate (available on ESD since Athena 14.5.0)
    ServiceMgr += PixelByteStreamErrorsSvc( ReadingESD = True )
    
if hasattr(ServiceMgr,'PixelByteStreamErrorsSvc'):
    PixelByteStreamErrorsSvc = ServiceMgr.PixelByteStreamErrorsSvc
    theApp.CreateSvc += [ ServiceMgr.PixelByteStreamErrorsSvc.getFullName() ]
    print PixelByteStreamErrorsSvc
    # Due to a "feature" in the BS encoder for simulation, the information of the BS error service
    # is not reliable on MC data.
    if ( globalflags.DataSource == 'geant4' ) :
        UseByteStream = False
    else :
        UseByteStream = True
            
    if hasattr(ServiceMgr,'PixelConditionsSummarySvc'):
        ServiceMgr.PixelConditionsSummarySvc.UseByteStream = UseByteStream
    else :
        from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummarySvc
        ServiceMgr += PixelConditionsSummarySvc( UseByteStream = UseByteStream )

#

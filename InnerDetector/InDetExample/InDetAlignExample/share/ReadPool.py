# ReadPool.py
# Based on InDetAlignGenAlgs/ReadPool.py
# Read AlignableTransforms directly from file without using IOV database
# Use CondProxyProvider instead of IOVSvc to provide objects
# joboption FRAGMENT to be run e.g. in framework of InDetRecExample
#

     
# load converters for AlignableTransforms
#include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )

conddb.blockFolder("/Indet/Align")
conddb.blockFolder("/TRT/Align")

from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr += CondProxyProvider()
ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
ServiceMgr.CondProxyProvider.InputCollections += [ "alignobjs.root" ]
ServiceMgr.CondProxyProvider.OutputLevel=DEBUG
print ServiceMgr.CondProxyProvider

ServiceMgr.IOVSvc.preLoadData=True


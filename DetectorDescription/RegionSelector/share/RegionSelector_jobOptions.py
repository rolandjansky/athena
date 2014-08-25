# Joboptions fragement for backward compatability;
# please import configurables yourself instead.
from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
from AthenaCommon.AppMgr import ServiceMgr
theRegSelSvc = RegSelSvcDefault()
ServiceMgr += theRegSelSvc


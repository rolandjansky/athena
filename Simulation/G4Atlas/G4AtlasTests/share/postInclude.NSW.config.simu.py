# needed to get the right dictionary from IdDictParser
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.MuonVersionOverride="MuonSpectrometer-R.07.00-NSW"
from AthenaCommon.AppMgr import ServiceMgr
if 'ReadAGDD' in ServiceMgr.AGDDtoGeoSvc.Builders["NewSmallWheel"].__slots__ : ServiceMgr.AGDDtoGeoSvc.Builders["NewSmallWheel"].ReadAGDD=False

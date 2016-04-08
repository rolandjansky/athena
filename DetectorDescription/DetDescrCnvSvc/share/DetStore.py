#
# Joboptions for bootstrapping the loading of the DetectorStore with
# the DetDescrCnvSvc
#

# Adds and instantiates the DetDescrCnvSvc service. It is assumed that
# a view of the application manager is available as 'theApp'


from AthenaCommon.AppMgr import theApp
theApp.Dlls   += [ 'DetDescrCnvSvc' ]
theApp.ExtSvc += [ 'DetDescrCnvSvc' ]

# Create service and add to list of conversion services
#
#  NOTE: the following does not yet work for the detector store
# theApp.ExtSvc += { "DetPersistencySvc/DetectorPersistencySvc", 
#  		      "DetDescrCnvSvc" };
#DetectorPersistencySvc.CnvServices += { "DetDescrCnvSvc" };
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
EventPersistencySvc.CnvServices = [ "DetDescrCnvSvc" ]

# Specify primary Identifier dictionary to be used
DetDescrCnvSvc.IdDictName = "IdDictParser/ATLAS_IDS.xml";


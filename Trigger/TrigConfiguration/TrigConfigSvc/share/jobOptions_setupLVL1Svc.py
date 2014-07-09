theApp.CreateSvc += [ "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc" ]

LVL1ConfigSvc = Service( "LVL1ConfigSvc" )
#LVL1ConfigSvc.DBServer = "devdb"
LVL1ConfigSvc.CreateLegacyObjects = False


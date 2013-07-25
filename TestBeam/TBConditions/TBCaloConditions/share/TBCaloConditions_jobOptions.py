theApp.Dlls += [ "TBCaloConditions" ]
IOVDbSvc = Service("IOVDbSvc") 
IOVDbSvc.Folders +=["/tile/dcs/Tile_LV_62:table.theta:online..value<offset>-1</offset>" ]
IOVDbSvc.Folders +=["/tile/dcs/Tile_LV_62:table.eta:online..value<offset>-1</offset>"]
IOVDbSvc.Folders +=["/tile/dcs/Tile_LV_62:table.z:online..value<offset>-1</offset>"]
IOVDbSvc.Folders +=["/tile/dcs/Tile_LV_62:table.delta:online..value<offset>-1</offset>"]
IOVDbSvc.Folders +=["/tile/dcs/System1:table.theta:online..value<offset>-1</offset>"]
IOVDbSvc.Folders +=["/tile/dcs/System1:table.eta:online..value<offset>-1</offset>"]
IOVDbSvc.Folders +=["/tile/dcs/System1:table.z:online..value<offset>-1</offset>"]
IOVDbSvc.Folders +=["/tile/dcs/System1:table.delta:online..value<offset>-1</offset>" ]


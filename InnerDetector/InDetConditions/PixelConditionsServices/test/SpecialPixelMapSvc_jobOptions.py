include.block("PixelConditionsServices/SpecialPixelMapSvc_jobOptions.py")



### set up IOVDbSvc folder

from IOVDbSvc.CondDB import conddb

conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixMapShort","/PIXEL/PixMapShort")
conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixMapLong","/PIXEL/PixMapLong")
conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/NoiseMapShort","/PIXEL/NoiseMapShort")
conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/NoiseMapLong","/PIXEL/NoiseMapLong")
conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixMapOverlay","/PIXEL/PixMapOverlay")


### configure the special pixel map service

from PixelConditionsServices.PixelConditionsServicesConf import SpecialPixelMapSvc

ServiceMgr += SpecialPixelMapSvc()
SpecialPixelMapSvc = ServiceMgr.SpecialPixelMapSvc

SpecialPixelMapSvc.DBFolders = [ "/PIXEL/PixMapShort", "/PIXEL/PixMapLong" ]
SpecialPixelMapSvc.SpecialPixelMapKeys = [ "SpecialPixelMap", "SpecialPixelMapLong" ]

SpecialPixelMapSvc.DBFolders += [ "/PIXEL/NoiseMapShort", "/PIXEL/NoiseMapLong" ]
SpecialPixelMapSvc.SpecialPixelMapKeys += [ "NoiseMapShort", "NoiseMapLong" ]

SpecialPixelMapSvc.OverlayKey = "PixMapOverlay"
SpecialPixelMapSvc.OverlayFolder = "/PIXEL/PixMapOverlay"

if DetFlags.pixel_on() : 
   theApp.CreateSvc += [ ServiceMgr.SpecialPixelMapSvc.getFullName() ]



### other properties of the SpecialPixelMapSvc:
#SpecialPixelMapSvc.RegisterCallback = True    # default: true
#SpecialPixelMapSvc.DataSource = "None"        # default: "None", possible values: "None", "Database" or "Textfiles"
                                               # only used in create(), not in callback
#SpecialPixelMapSvc.ModuleIDsForPrinting = [ "89702400" ] # default: empty, possible values: list of module hash ids or "all" 
#SpecialPixelMapSvc.PrintVerbosePixelID = True # default: true
#SpecialPixelMapSvc.PrintBinaryPixelStatus = True # default: true
#SpecialPixelMapSvc.MarkSpecialRegions = True  # default: true
#SpecialPixelMapSvc.MergePixelMaps = True      # default: true

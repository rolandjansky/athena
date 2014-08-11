from TileRecUtils.TileRecFlags import jobproperties

if jobproperties.TileRecFlags.noiseFilter == 1:
     tag = "CaloOflNoiseCellnoise-tile_filter"
else:
     tag = "CaloOflNoiseCellnoise-tile_nofilter"
        
folder  = "/CALO/Ofl/Noise/CellNoise"

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.IOVDbSvc.overrideTags +=  ["<prefix>"+folder+"</prefix> <tag>"+tag+"</tag>"]

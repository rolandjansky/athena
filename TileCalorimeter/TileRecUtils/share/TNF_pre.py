from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DataSource.set_data()

from TileRecUtils.TileRecFlags import jobproperties
jobproperties.TileRecFlags.noiseFilter=1

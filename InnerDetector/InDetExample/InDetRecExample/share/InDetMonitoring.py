# ------------------------------------------------------------
#
# ----------- run monitoring
#
# this file is included from InDetRec_all.py (in case of ID standalone running)
# or through RecExCommon (data quality include)
# ------------------------------------------------------------
#
# --- checking for presence of flags
#
from InDetRecExample.InDetJobProperties import InDetFlags
from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags

if InDetFlags.doMonitoringGlobal() or InDetFlags.doMonitoringPrimaryVertexingEnhanced():
  include("InDetRecExample/InDetMonitoringGlobal.py")
if InDetFlags.doMonitoringPixel() and not (commonGeoFlags.Run() == "RUN1"):
  # Not run for Run-1 (see ATR-25274)
  include( "InDetRecExample/InDetMonitoringPixel.py")
if InDetFlags.doMonitoringSCT():
  include( "InDetRecExample/InDetMonitoringSCT.py" )
if InDetFlags.doMonitoringTRT():
  include( "InDetRecExample/InDetMonitoringTRT.py" )
if InDetFlags.doMonitoringAlignment():
  include("InDetRecExample/InDetMonitoringAlignment.py")

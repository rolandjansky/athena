from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon import CfgMgr

def ActsTrackingGeometrySvc(name="ActsTrackingGeometrySvc", **kwargs):
     return CfgMgr.ActsTrackingGeometrySvc(name, **kwargs)

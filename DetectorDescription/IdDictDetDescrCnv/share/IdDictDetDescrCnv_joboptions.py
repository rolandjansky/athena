#
#  Joboptions for the loading of the of InDetDDMgr
#

_log = logging.getLogger( 'IdDictDetDescrCnv_joboptions' )
from AthenaCommon.GlobalFlags  import globalflags
from IOVDbSvc.CondDB import conddb

# Disable for MC or for Run 1 (AlignL* folders are not in COMP200).
if globalflags.DataSource.get_Value() != 'data' or conddb.dbname == 'COMP200':
    _log.debug("Disabling ID dynamic alignment")
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags
    InDetGeometryFlags.useDynamicAlignFolders=False

# DLLs 
theApp.Dlls += [ "IdDictDetDescrCnv" ]

del _log

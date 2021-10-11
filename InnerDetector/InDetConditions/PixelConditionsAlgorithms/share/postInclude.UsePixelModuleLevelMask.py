# Force the job to use the Pixel Module-level mask implemented via the
# FE-level masking code. See https://gitlab.cern.ch/atlas/athena/-/merge_requests/45356

from IOVDbSvc.CondDB import conddb
from AthenaCommon.GlobalFlags import globalflags
if (globalflags.DataSource=='data' and conddb.dbdata == 'CONDBR2'):  # for data overlay
    conddb.addOverride("/PIXEL/PixelModuleFeMask","PixelModuleFeMask-RUN2-DATA-UPD4-05")
else:
    conddb.addOverride("/PIXEL/PixelModuleFeMask","PixelModuleFeMask-SIM-MC16-000-03")

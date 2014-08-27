## @file PoolSvc_jobOptions.py
## @brief PoolSvc job options file to illustrate available PoolSvc properties.
## @author Peter van Gemmeren <gemmeren@anl.gov>
###############################################################
#
# PoolSvc job options file
# Principally for documentation, to illustrate available PoolSvc properties
#
#==============================================================

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# The next lines tell Athena to load the PoolSvc, and also where to find it.
from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()

## back compat (attempt)
PoolSvc = svcMgr.PoolSvc

# Commonly used properties are listed first

# Output files are registered in this catalog.
PoolSvc.WriteCatalog = "xmlcatalog_file:PoolFileCatalog.xml";
# If WriteCatalog is not specified, POOL checks the value of the POOL_CATALOG environment variable.
# If POOL_CATALOG is not set, POOL uses "xmlcatalog_file:PoolFileCatalog.xml," creating the file
# PoolFileCatalog.xml in the current directory, if necessary.
#
# List of input file catalogs to consult.
PoolSvc.ReadCatalog = [ "" ];
# ReadCatalog is also optional. POOL always checks the WriteCatalog (same default behavior as above),
# and checks the ReadCatalogs only if the needed file is not found.
#
# Option to create catalog entry with GUID obtained from file itself (if possible).
# This will allow the unsafe opening of an input file by PFN and attempt to patch the catalog.
PoolSvc.AttemptCatalogPatch = False;

# Retry period for CORAL Connection Service.
PoolSvc.ConnectionRetrialPeriod = 30;
# The retrial time out for CORAL Connection Service.
PoolSvc.ConnectionRetrialTimeOut = 300;
# The time out for CORAL Connection Service.
PoolSvc.ConnectionTimeOut = 5;
# CleanUp for CORAL Connection Service.
PoolSvc.ConnectionCleanUp = False;
# Ccompression level for Frontier.
PoolSvc.FrontierCompression = 5;
# List of schemas to be refreshed for Frontier.
PoolSvc.FrontierRefreshSchema = [ "" ];

# The open mode for the file ("append" or "overwrite").
PoolSvc.FileOpen = "overwrite";
# Option to have PoolSvc limit the number of open Input Files. (0 = No files are closed automatically)
PoolSvc.MaxFilesOpen = 0;

# Passthrough for SEAL dictionary checking
PoolSvc.CheckDictionary = True;

# Use Replica Sorter Service
PoolSvc.SortReplicas = True;

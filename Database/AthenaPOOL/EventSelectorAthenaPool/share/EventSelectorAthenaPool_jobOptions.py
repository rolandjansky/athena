## @file EventSelectorAthenaPool_jobOptions.py
## @brief EventSelector job options file to illustrate available EventSelector properties.
## @author Peter van Gemmeren <gemmeren@anl.gov>
## $Id: EventSelectorAthenaPool_jobOptions.py,v 1.15 2008-05-28 20:50:28 gemmeren Exp $
###############################################################
#
# EventSelectorAthenaPool job options file
# Principally for documentation, to illustrate available EventSelectorAthenaPool properties
#
#==============================================================
# The next lines tell Athena to load the EventSelectorAthenaPool, and also where to find it.

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import EventSelectorAthenaPool
svcMgr += EventSelectorAthenaPool( "EventSelector" )

EventSelector = svcMgr.EventSelector
#
# Switch on back navigation to find objects in input streams.
EventSelector.BackNavigation = FALSE;
# Switch on firing of FileIncidents which will trigger processing of metadata.
EventSelector.ProcessMetadata = TRUE;
# Type of the collection.
EventSelector.CollectionType = "ImplicitROOT";
# Prefix of the collection TTree
EventSelector.CollectionTree = "POOLContainer_";
# Connection string.
EventSelector.Connection = "<connectionString>";
# Attribute name.
EventSelector.RefName = "<attributeName>";
# AttributeListKey name
EventSelector.AttributeListKey = "Input";
# Vector with names of the input collections.
EventSelector.InputCollections = [ "<collectionName>" ];
EventSelector.InputCollections = [ ];
# Query string.
EventSelector.Query = "queryString";
# Flag to keep files open after PoolCollection reaches end.
# Needed for PilUp to run without PoolFileCatalog. Relies on POOL to close files when reaching DB_AGE_LIMIT.
EventSelector.KeepInputFilesOpen = FALSE;
# Vector of names of AlgTools that are executed by the EventSelector
EventSelector.HelperTools = [ "<toolName>" ];
EventSelector.HelperTools = [ ];
# Name of AlgTool that are executed by the EventSelector as the Counter
EventSelector.CounterTool = "<counterToolName>";
EventSelector.CounterTool = "";
# Numbers of events to skip.
EventSelector.SkipEvents = 0;
# Sequence number of events to skip (not EventNumber)
EventSelector.SkipEventSequence = [];
#
# The next lines tell Athena to load the AthenaPoolCnvSvc as EventPersistencySvc, and also where to find it.
from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
svcMgr += AthenaPoolCnvSvc()

#
EventPersistencySvc = Service( "EventPersistencySvc" )
EventPersistencySvc.CnvServices += [ "AthenaPoolCnvSvc" ]

from StoreGate.StoreGateConf import ProxyProviderSvc
svcMgr += ProxyProviderSvc()
#
# AddressProvider for the ProxyProviderSvc.
svcMgr.ProxyProviderSvc.ProviderNames += [ "AthenaPoolAddressProviderSvc" ]

## backward compat
ProxyProviderSvc = svcMgr.ProxyProviderSvc

## @file MetaDataSvc_jobOptions.py
## @brief MetaDataSvc job options file to illustrate available MetaDataSvc properties.
## @author Peter van Gemmeren <gemmeren@anl.gov>
## $Id: MetaDataSvc_jobOptions.py,v 1.5 2007-04-24 20:39:51 gemmeren Exp $
###############################################################
#
# MetaDataSvc job options file
# Principally for documentation, to illustrate available MetaDataSvc properties
#
#==============================================================
# The next lines tell Athena to load the MetaDataSvc.
MetaDataSvc = Service( "MetaDataSvc" )

# Add InputMetaDataStore for reading in-file MetaData.
theApp.ExtSvc += [ "StoreGateSvc/InputMetaDataStore" ]

# Add MetaDataSvc as ProxyProviderSvc for MetaData in InputMetaDataStore.
ProxyProviderSvc = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "MetaDataSvc" ]

# Set POOL container name for MetaData.
MetaDataSvc.MetaDataContainer = ""
# Register the IMetaDataTools.
MetaDataSvc.MetaDataTools = [ ]

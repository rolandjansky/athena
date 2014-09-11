## @file AddressRemappingSvc_jobOptions.py
## @brief AddressRemappingSvc job options file to illustrate available AddressRemappingSvc properties.
## @author Peter van Gemmeren <gemmeren@anl.gov>
## $Id: AddressRemappingSvc_jobOptions.py,v 1.4 2008-06-06 18:25:04 gemmeren Exp $
###############################################################
#
# AddressRemappingSvc job options file
# Principally for documentation, to illustrate available AddressRemappingSvc properties
#
#==============================================================
# The next lines tell Athena to load the AddressRemappingSvc.
from SGComps.SGCompsConf import AddressRemappingSvc
AddressRemappingSvc = AddressRemappingSvc()
svcMgr += AddressRemappingSvc

# Add AddressRemappingSvc as ProxyProviderSvc to re-map items in updateAddress
from AthenaCommon.ConfigurableDb import getConfigurable
svcMgr += getConfigurable( "ProxyProviderSvc" )()
svcMgr.ProxyProviderSvc.ProviderNames += [ "AddressRemappingSvc" ]

# List items to be re-mapped
svcMgr.AddressRemappingSvc.TypeKeyOverwriteMaps = [ "<oldType>#<oldKey>-><newType>[,symlink]#<newKey>[,alias]" ]
svcMgr.AddressRemappingSvc.TypeKeyOverwriteMaps = [ ]

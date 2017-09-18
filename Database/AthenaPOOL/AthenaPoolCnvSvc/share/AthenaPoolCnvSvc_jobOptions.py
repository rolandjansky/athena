## @file AthenaPoolCnvSvc_jobOptions.py
## @brief AthenaPoolCnvSvc job options file to illustrate available AthenaPoolCnvSvc properties.
## @author Peter van Gemmeren <gemmeren@bnl.gov>
## $Id: AthenaPoolCnvSvc_jobOptions.py,v 1.13 2008-12-04 20:54:31 gemmeren Exp $
###############################################################
#
# AthenaPoolCnvSvc job options file
# Principally for documentation, to illustrate available AthenaPoolCnvSvc properties
#
#==============================================================
# The next lines tell Athena to load the AthenaPoolCnvSvc, and also where to find it.

## get a handle to the Service Manager
from AthenaCommon.AppMgr import ServiceMgr

from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
svcMgr += AthenaPoolCnvSvc()

# Display detailed size and timing statistics for writing and reading AthenaPool objects.
svcMgr.AthenaPoolCnvSvc.UseDetailChronoStat = False

# Prefix for top level POOL container.
svcMgr.AthenaPoolCnvSvc.PoolContainerPrefix = "CollectionTree"

# Naming hint policy for top level POOL container. E.G.:
# - "", no hint will cause all objects to be written to the same tree (requires branches). 
# - "<type>", use the data object type as container name (tree per type).
# - "<type>/<key>", use the data object type and key as container name (tree per type/key).
svcMgr.AthenaPoolCnvSvc.TopLevelContainerName = ""

# Naming hint policy for top level POOL branching. E.G.:
# - "", no hint will cause all objects of a tree to be written to the same branch.
# - "<type>", use the data object type as branch name (required if type is not used for tree name).
# - "<type>/<key>", use the data object type and key as branch name.
svcMgr.AthenaPoolCnvSvc.SubLevelBranchName = "<type>/<key>"

# MaxFileSizes, vector with maximum file sizes for Athena POOL output files.
# Set domain MaxSize default for all Databases.
# Files larger than 5GB are not supported by some DDM tools and are disallowed by ATLAS policy.
# They should only be produced for private use or in special cases.
svcMgr.AthenaPoolCnvSvc.MaxFileSizes = [ "10000000000" ]
# Set MaxSize for a Database only
svcMgr.AthenaPoolCnvSvc.MaxFileSizes += [ "<DatabaseName> = <value>" ]

svcMgr.AthenaPoolCnvSvc.MaxFileSizes = [ ]

# PoolAttributes, vector with names and values of technology specific attributes for POOL.
# Set Domain Attribute
svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "<attributeName> = '<value>'" ]
# Set Database Attribute
svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '<fileName>'; <attributeName> = '<value>'" ]
# Set Container Attribute
svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '<fileName>'; ContainerName = '<type>/<key>'; <attributeName> = '<value>'" ]

# Set Input Database Attribute - set attribute for domain
svcMgr.AthenaPoolCnvSvc.InputPoolAttributes += [ "<attributeName> = '<value>'" ]

# Set Input Database Attribute - set attribute for each incoming file/db
svcMgr.AthenaPoolCnvSvc.InputPoolAttributes += [ "DatabaseName = '*'; <attributeName> = '<value>'" ]

# Set Input Database Attribute - set attribute for each incoming file/db for a particular container/tree
svcMgr.AthenaPoolCnvSvc.InputPoolAttributes += [ "DatabaseName = '*'; ContainerName = '<treeName>'; <attributeName> = '<value>'" ]

# To get an Attribute value printed to the log while commit, use the same syntax as for
# setting attributes, but replace the <value> with its <dataType> (int, DbLonglong, double).
# E.G.: get Container Attribute
svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '<fileName>'; ContainerName = '<type>/<key>'; <attributeName> = '<dataType>'" ]

svcMgr.AthenaPoolCnvSvc.PoolAttributes = [ ]

## backward compat
AthenaPoolCnvSvc = svcMgr.AthenaPoolCnvSvc

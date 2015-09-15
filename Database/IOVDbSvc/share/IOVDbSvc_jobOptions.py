## @file IOVDbSvc_jobOptions.py
##
## @brief A joboptions for the IOVDbSvc
##
## @author A. Perus
## $Header: /build/atlas/cvs/atlas/offline/Database/IOVDbSvc/share/IOVDbSvc_jobOptions.py,v 1.9 2006-08-14 07:07:11 hawkings Exp $
##

# block multiple inclusion
include.block ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )

# DLLs 
theApp.Dlls   += [ "IOVDbSvc" ]

# Add as external service
#DO NOT USE UNTIL ONE INITS SERVICES AUTOMATICALLY WHEN CALLED:
# theApp.ExtSvc += [ "IOVDbSvc" ]

# Add to proxy provider list
ProxyProviderSvc = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "IOVDbSvc" ]

#--------------------------------------------------------------
# For IOVDbSvc: set default connection for folders that do not specify
# a connection - one must fill in <...>
IOVDbSvc = Service( "IOVDbSvc" )
#IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;<server>:<db>:<account>:<pwd>"

# To manage connections, i.e. close them at each EndEvent incident
#  and re-open them when needed - Default is false   
#IOVDbSvc.ManageConnections = TRUE

#--------------------------------------------------------------
# Folders list to be used for reading
#
#  folders may be associated to
#      a specific tag <tag> aTag </tag>            aTag is a string
#      a time offset  <offset> anOffset </offset>  anOffset is an integer
#   or a dbConnection <dbConnection> aConnection </dbConnection>
#
# Explicit list of folders
#IOVDbSvc.Folders += [ "/IOVDb:Test/IOVDbTestAMDBCorrection " ] 
#    with offset specified
#IOVDbSvc.Folders += [ "/IOVDb:Test/IOVDbTestAMDBCorrection <offset>0</offset>" ] 
#
# Implicit list of folders
#    with local tag and a time offset
#IOVDbSvc.Folders += [ "/IOVDb:Test/* <tag>myTagDC2</tag> <offset>-1</offset>" ]
#    with a specific dbConnection 
#IOVDbSvc.Folders += [ "/IOVDb:Test/* <dbConnection>atlasdev1.cern.ch:CondDB_Pool_5</dbConnection>" ]

#--------------------------------------------------------------
# A temporary property only used for the summer 2004 CTB
#  specify the time format : "time" or "run-event"
#IOVDbSvc.FoldersWithTime += [ "/IOVDbTest/aFolder <timeStamp>time</timeStamp>" ] 

#--------------------------------------------------------------
# Add in the IOVSvc
# CondDBMySQL conversion service is now only loaded by RecExTB
include ( "IOVSvc/IOVSvc.py" )

#--------------------------------------------------------------

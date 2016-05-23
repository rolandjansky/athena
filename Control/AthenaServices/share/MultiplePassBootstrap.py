## @file MultiplePassBootstrap.py
# @brief dummy. OBSOLETE
#
# $Id: MultiplePassBootstrap.py,v 1.4 2007-11-18 17:21:52 calaf Exp $
#**************************************************************

#--------------------------------------------------------------
# Atlas specific event loop and output stream
#--------------------------------------------------------------

#theApp below is the gaudimodule.iService proxy to the c++ Application manager
# NOT theApp defined in AthenaCommon.AppMgr (an AthAppMgr instance)
theApp.OutStreamType     = "AthenaOutputStream"

#--------------------------------------------------------------
# take care of job options legacy
#--------------------------------------------------------------
theApp.JobOptionsPath    = ""
theApp.JobOptionsType    = "NONE"

#==============================================================
#
# End of Athena bootstrap options file
#
#**************************************************************

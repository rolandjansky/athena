# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# This posInclude fragment is used when setting up GPT profiling of an Athena job
# to turn off CoreDumpSvc. CoreDumpSvc hooks into certain signal events, which
# seems to interfere with GPerfTools and causes random crashes
#

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if hasattr(svcMgr, 'CoreDumpSvc'):
  # By default, CoreDumpSvc.cxx defines to catch
  # all SIGSEGV, SIGBUS, SIGILL, SIGFPE events
  # -> we need to unregister all signals
  svcMgr.CoreDumpSvc.Signals = [ ]

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

'''
messageSvcFmt.py

Utility function to format the message service consistently.

Riley Xu - riley.xu@cern.ch
2019/11/04
'''

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.Constants import *


HTT_OutputLevel = min(INFO, ServiceMgr.MessageSvc.OutputLevel) # Set HTT outputs to at most info = 3
#HTT_OutputLevel = DEBUG
    # This can be useful to i.e. set all other Athena messaging to WARNING, but still get HTT INFOs
    # Or, one can only set HTT components to DEBUG, but not all of Athena (i.e. by reassigning this variable)

def formatMessageSvc():
    ServiceMgr.MessageSvc.Format='% F%30W%S%7W%R%T %0W%M' # default, but changed the 30 from 18 to increase the name width
    ServiceMgr.MessageSvc.debugColorCode = ['[38;5;133m']
    ServiceMgr.MessageSvc.infoColorCode = ['[38;5;153m']
    ServiceMgr.MessageSvc.warningColorCode = ['[93m']
    ServiceMgr.MessageSvc.errorColorCode = ['[91m']
    ServiceMgr.MessageSvc.fatalColorCode = ['[97;41m']
    ServiceMgr.MessageSvc.useColors = True

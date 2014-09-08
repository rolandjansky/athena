# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# global flag, should be set at the beginning of the job

def setUseNewTriggerMenu(use=True):
    global _useNewTriggerMenu
    _useNewTriggerMenu = use

def useNewTriggerMenu():
    # first check if environment var USE_NEW_TM exists and is not "0"
    import os
    global _useNewTriggerMenu

    _useNewTriggerMenu = (os.environ.get("USE_NEW_TM","1")=="1")
    return _useNewTriggerMenu


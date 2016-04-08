# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import string, sys, traceback

def exc2string(list):
    list   = sys.exc_info()
    errmsg  = str(list[0])
    errmsg += str(list[1])
    errmsg += string.join(traceback.format_tb(list[2]))
    return errmsg
                

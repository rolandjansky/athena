# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import traceback


def exc2string2():
    """Provide traceback ehen an exception has been raised"""
    llist = sys.exc_info()
    errmsg = str(llist[0])
    errmsg += str(llist[1])
    errmsg += ' '.join(traceback.format_tb(llist[2]))
    return errmsg

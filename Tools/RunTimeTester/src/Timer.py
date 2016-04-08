# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import time
"""
Prints the time with a particular format
"""

def timer():
    format = "%y/%m/%d %H:%M"
    return time.strftime(format, time.localtime())


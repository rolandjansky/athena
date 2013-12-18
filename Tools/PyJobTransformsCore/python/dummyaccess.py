# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Module with dummy utilities for files"""


def listdir(dirname):
    return []
  
def exists(filename):
    return True

def getsize(filename):
    return -1

def getmtime(filename):
    return 0

def access(filename,mode):
    return True

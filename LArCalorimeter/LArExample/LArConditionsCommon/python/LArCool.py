#!/usr/bin/env python

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

__all__ = [
  "larcool"
]

def _setup():
   rn=0
   try:
       from RecExConfig.AutoConfiguration import GetRunNumber
       rn=GetRunNumber()
   except Exception:
       print (" cannot find run number ")
   if (rn is not None):
       from LArConditionsCommon.LArRunFormat import getLArFormatForRun
       larcool = getLArFormatForRun(rn)
   else:
       larcool = None
   return larcool

larcool = _setup()
del _setup

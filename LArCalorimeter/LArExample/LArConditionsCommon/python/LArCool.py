#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__all__ = [
  "larcool"
]

def _setup():
   rn=0
   try:
       from RecExConfig.AutoConfiguration import GetRunNumber
       rn=GetRunNumber()
   except Exception,e:
       print " cannot find run number "
   if (rn is not None):
       from LArConditionsCommon.LArRunFormat import getLArFormatForRun
       larcool = getLArFormatForRun(rn)
   else:
       larcool = None
   return larcool

larcool = _setup()
del _setup

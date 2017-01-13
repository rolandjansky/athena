# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Utilities for HLT OKS file generation
"""

def defaultTags():
   """Return list of supported CMTCONFIG tags"""
   
   import pm.common
   tags = [pm.common.tdaqRepository.getObject('Tag', 'x86_64-slc6-gcc62-opt'),
           pm.common.tdaqRepository.getObject('Tag', 'x86_64-slc6-gcc62-dbg')]
   
   return tags


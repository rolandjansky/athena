# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Utilities for HLT OKS file generation
"""

import pm.common

def defaultTags():
   """Return list of supported CMTCONFIG tags"""
   
   import pm.common
   tags = [pm.common.tdaqRepository.getObject('Tag', 'x86_64-slc6-gcc48-opt'),
           pm.common.tdaqRepository.getObject('Tag', 'x86_64-slc6-gcc48-dbg')
           ]
   
   return tags


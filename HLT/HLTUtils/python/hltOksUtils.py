# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Utilities for HLT OKS file generation
"""

def defaultTags():
   """Return list of supported CMTCONFIG tags"""
   
   import pm.common
   import os

   project = os.environ.get('AtlasProject','')
   tag = os.environ.get('%s_PLATFORM' % project, os.environ.get('CMTCONFIG',None))
   opt_tag = '-'.join(tag.split('-')[:-1])+'-opt'
   dbg_tag = '-'.join(tag.split('-')[:-1])+'-dbg'
   tags = [pm.common.tdaqRepository.getObject('Tag', opt_tag),
           pm.common.tdaqRepository.getObject('Tag', dbg_tag)]
   
   return tags


# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/python/ResourceLimits.py
# Author: Grigori Rybkine (Grigori.Rybkine@rhul.ac.uk)
# Modified: Wim Lavrijsen (WLavrijsen@lbl.gov)

"""Helper to max soft resource limits to the hard ones."""

from Logging import logging
import resource


### data ---------------------------------------------------------------------
__version__ = '1.2.1'
__author__  = 'Grigori Rybkine (Grigori.Rybkine@cern.ch)'

__all__ = [ 'SetMaxLimits' ]

log = logging.getLogger( 'ResourceLimits' )


### private helper -----------------------------------------------------------
def _maxout( what, descr ):
   soft, hard = resource.getrlimit( what )
   if soft < hard or ( hard == -1L and soft != hard ):
      log.debug( 'setting soft %s limit to %s (was: %s)', descr,
                 hard == -1L and 'unlimited' or str(hard),
                 soft == -1L and 'unlimited' or str(soft), )
      try:
         resource.setrlimit( what, (hard,hard) )
      except ValueError, e:
         if what != resource.RLIMIT_AS or hard != -1L: raise
         import platform
         if platform.architecture()[0] != '32bit': raise
         if platform.machine() in ['i386', 'i486', 'i586', 'i686']: raise
         _max_32bit_address_space( soft, descr )

def _max_32bit_address_space( soft, descr ):
   MAX_AS = 2**32
   _64MB = 64*1024*1024
   hard = MAX_AS
   while hard > soft:
      try:
         log.debug( 'setting soft %s limit to %s (was: %s)', descr,
                    str(hard),
                    str(soft), )
         resource.setrlimit( resource.RLIMIT_AS, (hard,hard) )
      except ValueError, e:
         hard -= _64MB
      else:
         log.debug( 'set soft %s limit to %s (was: %s)', descr,
                    str(hard),
                    str(soft), )
         return

### max out soft resource limits to hard ones --------------------------------
def SetMaxLimits():
    try:
       _maxout( resource.RLIMIT_AS,  'address space' )
       _maxout( resource.RLIMIT_RSS, 'resident state' )
    except (ValueError,resource.error), e:
       log.warning( 'failed to set max resource limits (%s)', str(e) )


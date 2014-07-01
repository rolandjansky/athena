# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/python/PropertyHistoryCheck.py
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)

"""Check the set history of properties to look for duplicates and other
such problems."""


### data ---------------------------------------------------------------------
__version__ = '1.0.0'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

__all__ = [ 'check' ]


### history checker, output based on requested level of detail ---------------
def check( level ):
   import Configurable
   import GaudiKernel.GaudiHandles as GH

   ghbases = (GH.GaudiHandle, GH.GaudiHandleArray, Configurable.Configurable)

   overwrites = list()

 # collect history summary
   dseen = {}
   for kname, vconf in Configurable.Configurable.allConfigurables.items():
      if vconf.__class__ in dseen:
         continue
      dseen[ vconf.__class__ ] = 1

      for kprop, vattr in vconf.__class__.__dict__.items():
         try:
            for kobj, ventry in vattr.history.items():
               if 1 < len(ventry):
                  if type(ventry[0]) != list and not isinstance( ventry[0], ghbases ):
                     if ( ventry.count(ventry[0]) != len(ventry) ):

                        if level < 3:
                         # drop string settings from empty to value on one setting only
                           if ( len(ventry) == 2 and type( ventry[0] ) == str and not ventry[0] ):
                              continue

                      # store reportable overwrite
                        overwrites.append( (kobj.getJobOptName(), kprop, ventry) )

                     else:
                        pass # TODO: determine reportable list changes, and ToolHandle(Array)s
         except AttributeError:
            pass

 # format message
   import string
   overwrites_msg = string.join( [ '   %-40s %-30s %s' % t for t in overwrites ], '\n' )

 # provide report
   import Logging
   log = Logging.logging.getLogger( 'PropertyHistoryCheck' )
   log.info( 'Overwritten properties:\n%s', overwrites_msg )

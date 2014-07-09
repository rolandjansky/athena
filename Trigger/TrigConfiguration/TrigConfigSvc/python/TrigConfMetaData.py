# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file  TrigConfMetaData.py
## @brief Meta data for the trigger configuration
## $Id: $

from GaudiKernel.GaudiHandles import *
from GaudiKernel.Proxy.Configurable import *

class TrigConfMetaData( ConfigurableUser ) :
  """Configurable to store aribitray meta data in the job options
  catalogue and the trigger database.
  """
  __slots__ = {      
    'PreCommand' : '', # str
    'JobOptions': '',  # str
    'Modifiers': [ ],  # list
  }
  _propertyDocDct = { 
    'PreCommand' : """ Command line flags used when creating the configuration """,
    'JobOptions': """ Job options used when creating the configuraton """,
    'Modifiers': """ List of modifiers used with runHLT_standalone.py """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(TrigConfMetaData, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)

  def getType( self ):
      return 'TrigConfMetaData'

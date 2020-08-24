# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""module to access Event objects interactively

:author: Tadashi Maeno
:contact: Tadashi.Maeno@cern.ch

"""
__docformat__ = "restructuredtext en"

from PyKernel import PyKernel as PyK

def getEventInfo (aKey):
    """Retrieve EventInfo object from SG

    :param aKey: key of the object

    **examples**::

      athena> e = PyEventTools.getEventInfo('McEventInfo')
      athena> print e.event_ID().event_number()
    
    """
    return PyK.retrieve('xAOD::EventInfo',aKey)

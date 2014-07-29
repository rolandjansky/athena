# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# author Ricardo Abreu <ricardo.abreu@cern.ch>

def get_robhit():
  try:
    from robhit import robhit
  except ImportError, e:
    mesg = ('For this event modification plugin, you have to have a "robhit" '
            'module on your PYTHONPATH. A second option is to copy this module '
            'and manually overwrite the ROB hit list import: %s' % e)
    raise ImportError, mesg
  return robhit
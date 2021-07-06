# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

##
## @file InDetTrigRecExample/python/InDetTrigSliceSettings.py
##
##

from __future__ import print_function

""" InDetTrigSliceSettings
"""

__author__ = "Jiri Masik"
__doc__    = "InDetTrigSliceSettings"
__all__    = [ "InDetTrigSliceSettings" ]


class InDetTrigSliceSettingsDB:
  """
  keeps a dictionary of quantities which may vary slice by slice with
  their default settings. The settings may be overriden by user if the
  InDetTrigSliceSettings is instantiated early in the jO (before
  other InDetTrigRecExample files are included)
  for example a preExec line
  from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings; InDetTrigSliceSettings[('pTmin','bjetVtx')] = 5000.
  allows testing the bjetVtx instance with as different reconstruction threshold, as in the Run 2 configureation
  """
  def __init__(self):
    from AthenaCommon.SystemOfUnits import GeV
    _slices = ['lowPt']

    self.db = {}

    ptmin =   {}
    for i in _slices:
      ptmin[i] = 1.*GeV
    ptmin['lowPt'] = 0.1 * GeV

    self.db['pTmin']=ptmin




  def __getitem__(self, p):
    (quantity, slice) = p
    v = None
    #print ('Getting quantity ', quantity, ' for ', slice )
    try:
      q = self.db[quantity]
      try:
        v = q[slice]
      except Exception:
        print ('get InDetTrigSliceSettingsDB has no slice %s configured' % slice)
    except Exception:
        print ('get InDetTrigSliceSettingsDB has no quantity %s configured' % quantity)
    return v


  def __setitem__(self, p, value):
    (quantity, slice) = p
    try:
      q = self.db[quantity]
      try:
        q[slice] = value
      except Exception:
        print ('set InDetTrigSliceSettingsDB has no slice %s configured' % slice)
    except Exception:
      print ('set InDetTrigSliceSettingsDB has no quantity %s configured' % quantity)



InDetTrigSliceSettings = InDetTrigSliceSettingsDB()

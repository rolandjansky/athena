# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
## @file InDetTrigRecExample/python/InDetTrigSliceSettings.py
## @purpose 
##
##

""" InDetTrigSliceSettings
"""

__author__ = "Jiri Masik"
__version__= "$Revision: 1.6 $"
__doc__    = "InDetTrigSliceSettings"

__all__    = [ "InDetTrigSliceSettings" ]

class InDetTrigSliceSettingsDB:
  """
  keeps a dictionary of quantities which may vary slice by slice with
  their default settings. The settings may be overriden by user if the
  InDetTrigSliceSettings is instantiated early in the jO (before
  other InDetTrigRecExample files are included)
  """
  def __init__(self):
    from AthenaCommon.SystemOfUnits import GeV
    _slices = ['electron','photon','muon','muonIso','tau','bjet','bphysics','fullScan','minBias','beamgas',
               'cosmicsN', 'lowPt',   #these are not real slices, rather different setups of tools
               'hadCalib', 'fullScan500',       #hadCalib instances
               'minBias2',            #two pass (low-pt tracking)
               'heavyIon', 'heavyIonFS',   #RoI and FS instances for the heavy ion
               'minBias400',          #another minBias with 400MeV threshold
               ]
    self.db = {}
    
    ptmin =   {}
    etahalf = {}
    phihalf = {}
    fullscan= {}
    monps = {}
    monptmin = {}

    #ptmin
    for i in _slices:
      ptmin[i] = 1.*GeV
    ptmin['tau']=      0.8 * GeV
    ptmin['minBias'] = 0.2 * GeV
    ptmin['minBias2'] = 0.5 * GeV
    ptmin['cosmicsN'] = 0.5 * GeV
    #ptmin['fullScan'] = 0.5 * GeV    #comparison with the offline
    ptmin['beamgas'] = 0.1 * GeV
    ptmin['lowPt'] = 0.1 * GeV
    ptmin['heavyIonFS'] = 0.4 * GeV
    ptmin['hadCalib'] = 0.5 * GeV
    ptmin['fullScan500'] = 0.5 * GeV
    ptmin['minBias400'] = 0.39 * GeV

    self.db['pTmin']=ptmin

    #etaHalfWidth
    etahalf = {
      'electron'  : 0.1,
      'photon'    : 0.1,
      'muon'      : 0.1,
      'muonIso'   : 0.35,
      'tau'       : 0.4,
      'bjet'      : 0.2,
      'bphysics'  : 0.75,
      'hadCalib'  : 0.4,
      'fullScan'  : 3.0,
      'fullScan500': 3.0,
      'minBias'   : 3.0,
      'minBias2'  : 3.0,
      'beamgas'   : 3.0,
      'cosmicsN'  : 3.0,
      'lowPt'     : 3.0,
      'heavyIonFS': 3.0,
      'heavyIon'  : 0.1,
      'minBias400': 3.0,
      }
    self.db['etaHalfWidth']=etahalf

    #phiHalfWidth
    phihalf = {
      'electron'  : 0.1,
      'photon'    : 0.1,
      'muon'      : 0.1,
      'muonIso'   : 0.35,
      'tau'       : 0.4,
      'bjet'      : 0.201,
      'bphysics'  : 0.75,
      'hadCalib'  : 0.4,
      'fullScan'  : 3.14159,
      'fullScan500' : 3.14159,
      'minBias'   : 3.14159,
      'minBias2'  : 3.14159,
      'beamgas'   : 3.14159,
      'cosmicsN'  : 3.14159,
      'lowPt'     : 3.14159,
      'heavyIonFS': 3.14159,
      'heavyIon'  : 0.1,
      'minBias400': 3.14159,
      }
    self.db['phiHalfWidth']=phihalf

    #doFullScan
    for i in _slices:
      fullscan[i] = False
    fullscan['fullScan'] = True
    fullscan['fullScan500'] = True
    fullscan['minBias']  = True
    fullscan['minBias2'] = True
    fullscan['beamgas']  = True
    fullscan['cosmicsN'] = True
    fullscan['lowPt']    = True
    fullscan['heavyIonFS']=True
    fullscan['minBias400']=True

    self.db['doFullScan']=fullscan


    #Monitoring Prescales
    for i in _slices:
      monps[i] = 1
    monps['tau']= 1
    self.db['monPS']=monps

    #Monitoring pT min
    for i in _slices:
      monptmin[i] = 1 * GeV
    monptmin['muon'] = 12 * GeV
    self.db['monPtMin']=monptmin

  def __getitem__(self, (quantity, slice)):
    v = None
    try:
      q = self.db[quantity]
      try:
        v = q[slice]
      except:
        print 'InDetTrigConfigRecSliceBased has no slice %s configured' % slice
    except:
      print 'InDetTrigConfigRecSliceBased has no quantity %s configured' % quantity

    return v

  def __setitem__(self, (quantity, slice), value):
    try:
      q = self.db[quantity]
      try:
        q[slice] = value
      except:
        print 'InDetTrigConfigRecSliceBased has no slice %s configured' % slice
    except:
      print 'InDetTrigConfigRecSliceBased has no quantity %s configured' % quantity



InDetTrigSliceSettings = InDetTrigSliceSettingsDB()

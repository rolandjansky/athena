# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
## @file InDetTrigRecExample/python/InDetTrigSliceSettings.py
## @purpose 
##
##

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
  from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings; InDetTrigSliceSettings[('pTmin','bjetVtx')] = 2000.
  allows testing the bjetVtx instance with as different reconstruction threshold 
  """
  def __init__(self):
    from AthenaCommon.SystemOfUnits import GeV
    _slices = ['electron','photon',
               'muon','muonCore','muonIso',
               'tau','bjet','bphysics','fullScan','minBias','beamgas',
               'cosmicsN', 'lowPt',   #these are not real slices, rather different setups of tools
               'hadCalib', 'fullScan500',       #hadCalib instances
               'minBias2',            #two pass (low-pt tracking)
               'heavyIon', 'heavyIonFS',   #RoI and FS instances for the heavy ion
               'minBias400',          #another minBias with 400MeV threshold
               'fullScan2',           #2GeV threshold for MET
               'tauCore', 'tauIso',
               'beamSpot', 'cosmics',
               'bjetVtx',
               'FTK', 'FTKRefit', 'FTKMon',
               'bphysHighPt'
               ]

    self.db = {}
    
    ptmin =   {}
    etahalf = {}
    phihalf = {}
    fullscan= {}
    monps = {}
    monptmin = {}

    #FTF-specific settings
    drdoubletmax = {}
    seedradbinwidth = {}
    d0seedmax = {}
    d0seedppsmax = {}
    checkseedredundancy = {}
    dospphifiltering = {}
    dozfinder = {}
    docloneremoval = {}
    doresmon = {}
    dotrigindettrack = {}

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
    ptmin['fullScan2'] = 2. * GeV
    ptmin['minBias400'] = 0.39 * GeV
    ptmin['bphysHighPt'] = 2. * GeV
    ptmin['bjetVtx'] = 5. * GeV

    self.db['pTmin']=ptmin

    for i in _slices:
      d0seedmax[i] = 4.0
      d0seedppsmax[i] = 1.7

    d0seedmax['bphysics'] = 10.0
    d0seedmax['bphysHighPt'] = 10.0
    d0seedmax['muon'] = 10.0
    d0seedmax['muonCore'] = 10.0

    d0seedmax['cosmics'] = 1000.0
    d0seedppsmax['cosmics'] = 1000.0

    self.db['d0SeedMax']=d0seedmax
    self.db['d0SeedPPSMax']=d0seedppsmax

    for i in _slices:
      dozfinder[i] = False 
      dotrigindettrack[i] = False 
    dozfinder['beamSpot'] = True
    self.db['doZFinder']=dozfinder
    self.db['doTrigInDetTrack']=dotrigindettrack

    for i in _slices:
      doresmon[i] = False 
    doresmon['muon'] = True
    doresmon['FTKMon'] = True
    self.db['doResMon']=doresmon

    for i in _slices:
      dospphifiltering[i] = True
    # Turn off spacepoint phi filtering for muons
    dospphifiltering['muon'] = False
    dospphifiltering['muonCore'] = False
    dospphifiltering['bphysics'] = False
    dospphifiltering['bphysHighPt'] = False
    self.db['doSpPhiFiltering'] = dospphifiltering

    for i in _slices:
      docloneremoval[i] = True
    docloneremoval['electron'] = False
    self.db['doCloneRemoval'] = docloneremoval
    


    for i in _slices:
      checkseedredundancy[i] = False
    checkseedredundancy['electron'] = True
    checkseedredundancy['muon'] = True
    checkseedredundancy['muonCore'] = True
    checkseedredundancy['bphysics'] = True
    self.db['checkRedundantSeeds'] = checkseedredundancy

    
    for i in _slices:
      drdoubletmax[i] = 270
      seedradbinwidth[i] = 2
    drdoubletmax['beamSpot'] = 200
    seedradbinwidth['beamSpot'] = 10
    self.db['dRdoubletMax'] = drdoubletmax
    self.db['seedRadBinWidth'] = seedradbinwidth

    #etaHalfWidth
    etahalf = {
      'electron'  : 0.1,
      'photon'    : 0.1,
      'muon'      : 0.1,
      'muonCore'  : 0.1,
      'muonIso'   : 0.35,
      'tau'       : 0.4,
      'bjet'      : 0.2,
      'bphysics'  : 0.75,
      'bphysHighPt'  : 0.75,
      'hadCalib'  : 0.4,
      'fullScan'  : 3.0,
      'fullScan500': 3.0,
      'fullScan2' : 3.0,
      'minBias'   : 3.0,
      'minBias2'  : 3.0,
      'beamgas'   : 3.0,
      'cosmicsN'  : 3.0,
      'cosmics'   : 3.0,
      'lowPt'     : 3.0,
      'heavyIonFS': 3.0,
      'heavyIon'  : 0.1,
      'minBias400': 3.0,
      'tauCore'   : 0.1,
      'tauIso'    : 0.4,
      'beamSpot'  : 3.0,
      'bjetVtx'   : 0.2,
      }
    self.db['etaHalfWidth']=etahalf

    #phiHalfWidth
    phihalf = {
      'electron'  : 0.1,
      'photon'    : 0.1,
      'muon'      : 0.1,
      'muonCore'  : 0.1,
      'muonIso'   : 0.35,
      'tau'       : 0.4,
      'bjet'      : 0.201,
      'bphysics'  : 0.75,
      'bphysHighPt'  : 0.75,
      'hadCalib'  : 0.4,
      'fullScan'  : 3.14159,
      'fullScan500' : 3.14159,
      'fullScan2' : 3.14159,
      'minBias'   : 3.14159,
      'minBias2'  : 3.14159,
      'beamgas'   : 3.14159,
      'cosmicsN'  : 3.14159,
      'cosmics'   : 3.14159,
      'lowPt'     : 3.14159,
      'heavyIonFS': 3.14159,
      'heavyIon'  : 0.1,
      'minBias400': 3.14159,
      'tauCore'   : 0.1,
      'tauIso'    : 0.4,
      'beamSpot'  : 3.14159,
      'bjetVtx'   : 0.201,
      }
    self.db['phiHalfWidth']=phihalf

    #doFullScan
    for i in _slices:
      fullscan[i] = False
    fullscan['fullScan'] = True
    fullscan['fullScan2']= True
    fullscan['fullScan500'] = True
    fullscan['minBias']  = True
    fullscan['minBias2'] = True
    fullscan['beamgas']  = True
    fullscan['cosmicsN'] = True
    fullscan['cosmics']  = True
    fullscan['lowPt']    = True
    fullscan['heavyIonFS']=True
    fullscan['minBias400']=True
    fullscan['beamSpot']  =True

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
    #print 'Getting quantity ', quantity, ' for ', slice 
    try:
      q = self.db[quantity]
      try:
        v = q[slice]
      except:
        print 'get InDetTrigSliceSettingsDB has no slice %s configured' % slice
    except:
      print 'get InDetTrigSliceSettingsDB has no quantity %s configured' % quantity

    return v

  def __setitem__(self, (quantity, slice), value):
    try:
      q = self.db[quantity]
      try:
        q[slice] = value
      except:
        print 'set InDetTrigSliceSettingsDB has no slice %s configured' % slice
    except:
      print 'set InDetTrigSliceSettingsDB has no quantity %s configured' % quantity



InDetTrigSliceSettings = InDetTrigSliceSettingsDB()

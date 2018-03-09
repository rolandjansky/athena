
def createIsoToolElectronSelector():

  from AthenaCommon.AppMgr import ToolSvc
  from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaIsolationSelectorTool
  from TrigEgammaEmulationTool.TrigEgammaEmulationToolConfig        import OutputLevel
  
  from TrigEgammaHypo.TrigEFElectronHypoConfig import isolation_dict, caloisolation_dict
  from TrigEgammaHypo.TrigEFPhotonHypoConfig   import TrigEFPhotonIsoCutDefs

  # the order matter, do not change this order!
  isolations = ['ivartight','ivarmedium','ivarloose','iloose','icalotight','icalomedium','icaloloose']

  IsoToolSelectors = []

  ### loop over working points (WP)
  for wp in isolations:

    caloiso = True if 'icalo' in wp else False
    trkiso = True if not 'icalo' in wp else False
    
    tool = Trig__TrigEgammaIsolationSelectorTool( 'IsolationTool_'+wp,
                                          RelEtConeCut = caloisolation_dict[wp] if caloiso else [-1,-1,-1,-1,-1,-1],
                                          RelPtConeCut = isolation_dict[wp] if trkiso else [-1,-1,-1,-1,-1,-1],
                                          EtConeCut = [-1,-1,-1,-1,-1,-1],
                                          PtConeCut = [-1,-1,-1,-1,-1,-1],
                                          OutputLevel=OutputLevel,
                                          )
    ToolSvc += tool
    IsoToolSelectors.append( tool )

  return IsoToolSelectors

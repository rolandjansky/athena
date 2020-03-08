# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

def createIsoToolElectronSelector():

  from AthenaCommon.AppMgr import ToolSvc
  from AthenaCommon import CfgMgr
  from egammaRec.Factories                                          import ToolFactory
  from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaIsolationSelectorTool
  from TrigEgammaEmulationTool.TrigEgammaEmulationToolConfig        import OutputLevel
  
  from TrigEgammaHypo.TrigEFElectronHypoConfig import isolation_dict, caloisolation_dict
  from TrigEgammaHypo.TrigEFPhotonHypoConfig   import TrigEFPhotonIsoCutDefs

  # Track isolation -- remember to add TrackIsolation as a property of the class
  from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool


  TrackIsoTool = xAOD__TrackIsolationTool("TrackIsoTool")
  TrackIsoTool.TrackSelectionTool.maxZ0SinTheta= 3.
  TrackIsoTool.OutputLevel=0
  #TrackIsoTool.OverlapCone=0.15
  TrackIsoTool.TrackParticleLocation="HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_IDTrig"
  TrackIsoTool.TrackSelectionTool.OutputLevel=0
  TrackIsoTool.TrackSelectionTool.minPt= 1000.
  #TrackIsoTool.TrackSelectionTool.CutLevel= "Tight"
  TrackIsoTool.TrackSelectionTool.CutLevel= "Loose"
  ToolSvc += TrackIsoTool



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
                                          UseTrackIsolationTool=False,
                                          OutputLevel=OutputLevel,
                                          #TrackIsolationTool = TrigTrackIsolationTool
                                          TrackIsolationTool = TrackIsoTool
                                          )
    ToolSvc += tool
    IsoToolSelectors.append( tool )

    #Just to see WTF!
    _toolname='IsolationTool_'+wp
    RelEtConeCut = caloisolation_dict[wp] if caloiso else [-1,-1,-1,-1,-1,-1],
    RelPtConeCut = isolation_dict[wp] if trkiso else [-1,-1,-1,-1,-1,-1],

  return IsoToolSelectors


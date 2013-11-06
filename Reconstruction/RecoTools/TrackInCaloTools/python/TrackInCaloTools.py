# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"Holds a class inheriting from TrackInCaloToolsConf.TrackInCaloTools to make the creation of other instances easier"

import TrackInCaloToolsConf as TICT

class TrackInCaloTools( TICT.TrackInCaloTools ):
  def __init__(self, name='TrackInCaloTools', **kw ):
    # import the ToolSvc
    from AthenaCommon.AppMgr import ToolSvc

    # Define the default configuration
    defaults = dict(name = name, useNegativeCells = True, 
      EcoreDefinition = ['0.025x0.06', '0.03x0.07', '0.05', '0.03', '0.08x0.06', '2x1', '0.15x0.06'] + ['0.07']*4)
     
    # Try to determine the calorimeter cell container
    from RecExConfig.RecFlags import rec
    if rec.readAOD() :
      defaults['CaloCellContainer'] = "AODCellContainer"
    else:
      defaults['CaloCellContainer'] = "AllCalo"
    
    # Check if it is a cosmic run
    from AthenaCommon.BeamFlags import jobproperties
    defaults['Cosmics'] = (jobproperties.Beam.beamType() == 'cosmics')
    
    kw = dict(defaults, **kw)
    if 'NoiseTool' not in kw:
      from CaloDetDescr.CaloDepthToolBase import caloDepthToolshowerdefault
      from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
      ToolSvc += CaloNoiseToolDefault() 
      kw['NoiseTool'] = ToolSvc.CaloNoiseToolDefault


    ToolSvc += TICT.TrackExtrapolatorToCalo() # TODO: add name to kw

    if kw.get('Cosmics'):
      ToolSvc.TrackExtrapolatorToCalo.Cosmics = True
      ToolSvc += TICT.TrackDirectionTool()
    
    TICT.TrackInCaloTools.__init__(self, **kw)


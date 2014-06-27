# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AlgSequence
import D3PDMakerCoreComps
import PhotonD3PDMaker
from D3PDMakerConfig.D3PDMakerFlags                  import D3PDMakerFlags
from PhotonD3PDMaker.PhotonD3PDMakerFlags            import PhotonD3PDMakerFlags

DVGetter = D3PDMakerCoreComps.SGDataVectorGetterTool

def AddCellFilter(outputsgkey = "SelectedCells",
                  seq = AlgSequence()):

    from AthenaCommon.AppMgr      import ToolSvc

    photongetter = DVGetter('photoncellfilter_Getter',
                            TypeName = 'PhotonContainer',
                            SGKey = D3PDMakerFlags.PhotonSGKey())
    ToolSvc += photongetter

    electrongetter = DVGetter('electroncellfilter_Getter',
                              TypeName = 'ElectronContainer',
                              SGKey = D3PDMakerFlags.ElectronSGKey())
    ToolSvc += electrongetter

    from ROOT import egammaPID
    cellfilter = PhotonD3PDMaker.CellFilter(InputCellCollectionName = "AODCellContainer",
                                            OutputCellCollectionName = "SelectedCells",
                                            photonGetter = photongetter,
                                            electronGetter = electrongetter,
                                            ptCut = 15000.,
                                            etaCut = 2.5,
                                            deltaRCut = PhotonD3PDMakerFlags.CellDrCut(),
                                            photonID = egammaPID.PhotonIDLoose,
                                            electronID = egammaPID.ElectronIDMediumPP)

    seq += cellfilter
    

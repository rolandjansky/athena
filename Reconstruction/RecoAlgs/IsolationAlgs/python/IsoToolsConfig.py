# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """Tool configuration to instantiate all
 isolationTools with default configuration"""

from AthenaCommon.SystemOfUnits import GeV
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def isoTTVAToolCfg(flags,**kwargs):
    from TrackVertexAssociationTool.TTVAToolConfig import TTVAToolCfg
    kwargs.setdefault('name', 'ttvaToolForIso')
    kwargs.setdefault('WorkingPoint','Nonprompt_All_MaxWeight')
    return TTVAToolCfg(flags,**kwargs)

def isoTrackSelectionToolCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    
    kwargs.setdefault('name', 'isoTrackSelectionTool')
    kwargs.setdefault('CutLevel','Loose')
    kwargs.setdefault('minPt',1*GeV)

    acc.setPrivateTools(
        CompFactory.InDet.InDetTrackSelectionTool(**kwargs))
    return acc

def TrackIsolationToolCfg(flags, **kwargs):
    acc = ComponentAccumulator()
        
    if 'TrackSelectionTool' not in kwargs:
        kwargs['TrackSelectionTool'] = acc.popToolsAndMerge(isoTrackSelectionToolCfg(flags))
    if 'TTVATool' not in kwargs:
        kwargs['TTVATool'] = acc.popToolsAndMerge(isoTTVAToolCfg(flags))

    if flags.Beam.Type == 'cosmics':
        kwargs['VertexLocation'] = ''

    acc.setPrivateTools(CompFactory.xAOD.TrackIsolationTool(**kwargs))
    return acc

def ElectronTrackIsolationToolCfg(flags, **kwargs):
    kwargs.setdefault('name','ElectronTrackIsolationTool')
    kwargs.setdefault('CoreTrackEtaRange',0.01)
    return TrackIsolationToolCfg(flags,**kwargs)
        
def EGammaCaloIsolationToolCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    if 'IsoLeakCorrectionTool' not in kwargs:
        IsoCorrectionTool = CompFactory.CP.IsolationCorrectionTool(
            name = 'LeakageCorrTool',
            LogLogFitForLeakage = True)
        kwargs['IsoLeakCorrectionTool'] = IsoCorrectionTool

    if 'CaloFillRectangularClusterTool' not in kwargs:
        cfrc = CompFactory.CaloFillRectangularCluster(
            name="egamma_CaloFillRectangularCluster",
            eta_size=5,
            phi_size=7,
            cells_name=flags.Egamma.Keys.Input.CaloCells)
        kwargs['CaloFillRectangularClusterTool'] = cfrc
        
    kwargs.setdefault('name','egCaloIsolationTool')
    kwargs.setdefault('ParticleCaloExtensionTool',None)
    kwargs.setdefault('ParticleCaloCellAssociationTool',None)
    kwargs.setdefault('isMC',flags.Input.isMC)
        
    acc.setPrivateTools(CompFactory.xAOD.CaloIsolationTool(**kwargs))
    return acc

def MuonCaloIsolationToolCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    if 'ParticleCaloExtensionTool' not in kwargs:
        from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg
        kwargs['ParticleCaloExtensionTool'] = acc.popToolsAndMerge(
            ParticleCaloExtensionToolCfg(flags))
    if 'FlowElementsInConeTool' not in kwargs and flags.Reco.EnablePFlow:
        kwargs['FlowElementsInConeTool'] = CompFactory.xAOD.FlowElementsInConeTool(
            name='FlowElementsInConeTool')

    from CaloIdentifier import SUBCALO 
    kwargs.setdefault('EMCaloNums',[SUBCALO.LAREM])
    kwargs.setdefault('HadCaloNums',[SUBCALO.LARHEC, SUBCALO.TILE])
    kwargs.setdefault('ParticleCaloCellAssociationTool',None)
    kwargs.setdefault('UseEtaDepPUCorr',False)
    kwargs.setdefault('name','muonCaloIsolationTool')

    acc.setPrivateTools(CompFactory.xAOD.CaloIsolationTool(**kwargs))
    return acc
